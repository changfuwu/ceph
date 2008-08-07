// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2006 Sage Weil <sage@newdream.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software 
 * Foundation.  See file COPYING.
 * 
 */

#ifndef __SNAPSERVER_H
#define __SNAPSERVER_H

#include "MDSTableServer.h"
#include "snap.h"

class MDS;

class SnapServer : public MDSTableServer {
public:
  
protected:
  snapid_t last_snap;
  map<snapid_t, SnapInfo> snaps;
  set<snapid_t> pending_purge;
  
  map<version_t, SnapInfo> pending_create;
  map<version_t, snapid_t> pending_destroy;
  set<version_t>           pending_noop;

  version_t last_checked_osdmap;

public:
  SnapServer(MDS *m) : MDSTableServer(m, TABLE_SNAP) { }
    
  void init_inode();
  void reset_state();
  void encode_state(bufferlist& bl) {
    ::encode(last_snap, bl);
    ::encode(snaps, bl);
    ::encode(pending_purge, bl);
    ::encode(pending_create, bl);
    ::encode(pending_destroy, bl);
    ::encode(pending_noop, bl);
    ::encode(pending_for_mds, bl);
  }
  void decode_state(bufferlist::iterator& bl) {
    ::decode(last_snap, bl);
    ::decode(snaps, bl);
    ::decode(pending_purge, bl);
    ::decode(pending_create, bl);
    ::decode(pending_destroy, bl);
    ::decode(pending_noop, bl);
    ::decode(pending_for_mds, bl);
  }

  // server bits
  void _prepare(bufferlist &bl, __u64 reqid, int bymds);
  bool _is_prepared(version_t tid);
  void _commit(version_t tid);
  void _rollback(version_t tid);
  void handle_query(MMDSTableRequest *m);

  void check_osd_map(bool force);
};

#endif
