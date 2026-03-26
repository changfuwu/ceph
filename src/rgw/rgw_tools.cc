// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:nil -*-
// vim: ts=8 sw=2 sts=2 expandtab ft=cpp

#include <errno.h>

#include "common/errno.h"
#include "common/safe_io.h" // for safe_read()

#include "driver/rados/rgw_tools.h"

#define dout_subsys ceph_subsys_rgw
#define dout_context g_ceph_context

#define READ_CHUNK_LEN (512 * 1024)

using namespace std;

// Built-in content type map for common file extensions.
// Used as a fallback when the system mime.types file is not available
// or does not contain an entry for the given extension.
static const std::map<std::string, std::string> builtin_content_type_map = {
  // text
  {"css",   "text/css"},
  {"csv",   "text/csv"},
  {"htm",   "text/html"},
  {"html",  "text/html"},
  {"ics",   "text/calendar"},
  {"js",    "text/javascript"},
  {"mjs",   "text/javascript"},
  {"txt",   "text/plain"},
  {"xml",   "text/xml"},
  // image
  {"bmp",   "image/bmp"},
  {"gif",   "image/gif"},
  {"ico",   "image/x-icon"},
  {"jpeg",  "image/jpeg"},
  {"jpg",   "image/jpeg"},
  {"png",   "image/png"},
  {"svg",   "image/svg+xml"},
  {"svgz",  "image/svg+xml"},
  {"tif",   "image/tiff"},
  {"tiff",  "image/tiff"},
  {"webp",  "image/webp"},
  // audio
  {"aac",   "audio/aac"},
  {"flac",  "audio/flac"},
  {"m4a",   "audio/mp4"},
  {"mid",   "audio/midi"},
  {"midi",  "audio/midi"},
  {"mp3",   "audio/mpeg"},
  {"oga",   "audio/ogg"},
  {"ogg",   "audio/ogg"},
  {"opus",  "audio/opus"},
  {"wav",   "audio/wav"},
  {"weba",  "audio/webm"},
  // video
  {"avi",   "video/x-msvideo"},
  {"m4v",   "video/mp4"},
  {"mkv",   "video/x-matroska"},
  {"mov",   "video/quicktime"},
  {"mp4",   "video/mp4"},
  {"mpeg",  "video/mpeg"},
  {"mpg",   "video/mpeg"},
  {"ogv",   "video/ogg"},
  {"ts",    "video/mp2t"},
  {"webm",  "video/webm"},
  // application
  {"bin",   "application/octet-stream"},
  {"bz",    "application/x-bzip"},
  {"bz2",   "application/x-bzip2"},
  {"doc",   "application/msword"},
  {"docx",  "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
  {"gz",    "application/gzip"},
  {"jar",   "application/java-archive"},
  {"json",  "application/json"},
  {"jsonld","application/ld+json"},
  {"odp",   "application/vnd.oasis.opendocument.presentation"},
  {"ods",   "application/vnd.oasis.opendocument.spreadsheet"},
  {"odt",   "application/vnd.oasis.opendocument.text"},
  {"pdf",   "application/pdf"},
  {"ppt",   "application/vnd.ms-powerpoint"},
  {"pptx",  "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
  {"rtf",   "application/rtf"},
  {"sh",    "application/x-sh"},
  {"tar",   "application/x-tar"},
  {"xhtml", "application/xhtml+xml"},
  {"xls",   "application/vnd.ms-excel"},
  {"xlsx",  "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
  {"xul",   "application/vnd.mozilla.xul+xml"},
  {"zip",   "application/zip"},
  {"7z",    "application/x-7z-compressed"},
  // font
  {"otf",   "font/otf"},
  {"ttf",   "font/ttf"},
  {"woff",  "font/woff"},
  {"woff2", "font/woff2"},
};

static std::map<std::string, std::string>* ext_mime_map;

void parse_mime_map_line(const char *start, const char *end)
{
  char line[end - start + 1];
  strncpy(line, start, end - start);
  line[end - start] = '\0';
  char *l = line;
#define DELIMS " \t\n\r"

  while (isspace(*l))
    l++;

  char *mime = strsep(&l, DELIMS);
  if (!mime)
    return;

  char *ext;
  do {
    ext = strsep(&l, DELIMS);
    if (ext && *ext) {
      (*ext_mime_map)[ext] = mime;
    }
  } while (ext);
}


void parse_mime_map(const char *buf)
{
  const char *start = buf, *end = buf;
  while (*end) {
    while (*end && *end != '\n') {
      end++;
    }
    parse_mime_map_line(start, end);
    end++;
    start = end;
  }
}

static int ext_mime_map_init(const DoutPrefixProvider *dpp, CephContext *cct, const char *ext_map)
{
  int fd = open(ext_map, O_RDONLY);
  char *buf = NULL;
  int ret;
  if (fd < 0) {
    ret = -errno;
    ldpp_dout(dpp, 0) << __func__ << " failed to open file=" << ext_map
                  << " : " << cpp_strerror(-ret) << dendl;
    return ret;
  }

  struct stat st;
  ret = fstat(fd, &st);
  if (ret < 0) {
    ret = -errno;
    ldpp_dout(dpp, 0) << __func__ << " failed to stat file=" << ext_map
                  << " : " << cpp_strerror(-ret) << dendl;
    goto done;
  }

  buf = (char *)malloc(st.st_size + 1);
  if (!buf) {
    ret = -ENOMEM;
    ldpp_dout(dpp, 0) << __func__ << " failed to allocate buf" << dendl;
    goto done;
  }

  ret = safe_read(fd, buf, st.st_size + 1);
  if (ret != st.st_size) {
    // huh? file size has changed?
    ldpp_dout(dpp, 0) << __func__ << " raced! will retry.." << dendl;
    free(buf);
    close(fd);
    return ext_mime_map_init(dpp, cct, ext_map);
  }
  buf[st.st_size] = '\0';

  parse_mime_map(buf);
  ret = 0;
done:
  free(buf);
  close(fd);
  return ret;
}

const char *rgw_find_mime_by_ext(string& ext)
{
  // First, check the dynamically loaded mime map (from system file).
  map<string, string>::iterator iter = ext_mime_map->find(ext);
  if (iter != ext_mime_map->end()) {
    return iter->second.c_str();
  }

  // Fall back to the built-in content type map. The returned c_str()
  // pointer is valid for the lifetime of the program since
  // builtin_content_type_map has static storage duration.
  auto builtin_iter = builtin_content_type_map.find(ext);
  if (builtin_iter != builtin_content_type_map.end()) {
    return builtin_iter->second.c_str();
  }

  return NULL;
}

int rgw_tools_init(const DoutPrefixProvider *dpp, CephContext *cct)
{
  ext_mime_map = new std::map<std::string, std::string>;
  ext_mime_map_init(dpp, cct, cct->_conf->rgw_mime_types_file.c_str());
  // ignore errors; missing mime.types is not fatal
  return 0;
}

void rgw_tools_cleanup()
{
  delete ext_mime_map;
  ext_mime_map = nullptr;
}
