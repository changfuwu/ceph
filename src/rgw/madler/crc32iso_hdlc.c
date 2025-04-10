#include "crc32iso_hdlc.h"

uint32_t crc32iso_hdlc_bit(uint32_t crc, void const *mem, size_t len) {
    uint8_t const *data = mem;
    if (data == NULL)
        return 0;
    crc = ~crc;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (unsigned k = 0; k < 8; k++) {
            crc = crc & 1 ? (crc >> 1) ^ 0xedb88320 : crc >> 1;
        }
    }
    crc = ~crc;
    return crc;
}

uint32_t crc32iso_hdlc_rem(uint32_t crc, unsigned val, unsigned bits) {
    crc = ~crc;
    val &= (1U << bits) - 1;
    crc ^= val;
    for (unsigned i = 0; i < bits; i++) {
        crc = crc & 1 ? (crc >> 1) ^ 0xedb88320 : crc >> 1;
    }
    crc = ~crc;
    return crc;
}

#define table_byte table_word[0]

static uint32_t const table_word[][256] = {
   {0xd202ef8d, 0xa505df1b, 0x3c0c8ea1, 0x4b0bbe37, 0xd56f2b94, 0xa2681b02,
    0x3b614ab8, 0x4c667a2e, 0xdcd967bf, 0xabde5729, 0x32d70693, 0x45d03605,
    0xdbb4a3a6, 0xacb39330, 0x35bac28a, 0x42bdf21c, 0xcfb5ffe9, 0xb8b2cf7f,
    0x21bb9ec5, 0x56bcae53, 0xc8d83bf0, 0xbfdf0b66, 0x26d65adc, 0x51d16a4a,
    0xc16e77db, 0xb669474d, 0x2f6016f7, 0x58672661, 0xc603b3c2, 0xb1048354,
    0x280dd2ee, 0x5f0ae278, 0xe96ccf45, 0x9e6bffd3, 0x0762ae69, 0x70659eff,
    0xee010b5c, 0x99063bca, 0x000f6a70, 0x77085ae6, 0xe7b74777, 0x90b077e1,
    0x09b9265b, 0x7ebe16cd, 0xe0da836e, 0x97ddb3f8, 0x0ed4e242, 0x79d3d2d4,
    0xf4dbdf21, 0x83dcefb7, 0x1ad5be0d, 0x6dd28e9b, 0xf3b61b38, 0x84b12bae,
    0x1db87a14, 0x6abf4a82, 0xfa005713, 0x8d076785, 0x140e363f, 0x630906a9,
    0xfd6d930a, 0x8a6aa39c, 0x1363f226, 0x6464c2b0, 0xa4deae1d, 0xd3d99e8b,
    0x4ad0cf31, 0x3dd7ffa7, 0xa3b36a04, 0xd4b45a92, 0x4dbd0b28, 0x3aba3bbe,
    0xaa05262f, 0xdd0216b9, 0x440b4703, 0x330c7795, 0xad68e236, 0xda6fd2a0,
    0x4366831a, 0x3461b38c, 0xb969be79, 0xce6e8eef, 0x5767df55, 0x2060efc3,
    0xbe047a60, 0xc9034af6, 0x500a1b4c, 0x270d2bda, 0xb7b2364b, 0xc0b506dd,
    0x59bc5767, 0x2ebb67f1, 0xb0dff252, 0xc7d8c2c4, 0x5ed1937e, 0x29d6a3e8,
    0x9fb08ed5, 0xe8b7be43, 0x71beeff9, 0x06b9df6f, 0x98dd4acc, 0xefda7a5a,
    0x76d32be0, 0x01d41b76, 0x916b06e7, 0xe66c3671, 0x7f6567cb, 0x0862575d,
    0x9606c2fe, 0xe101f268, 0x7808a3d2, 0x0f0f9344, 0x82079eb1, 0xf500ae27,
    0x6c09ff9d, 0x1b0ecf0b, 0x856a5aa8, 0xf26d6a3e, 0x6b643b84, 0x1c630b12,
    0x8cdc1683, 0xfbdb2615, 0x62d277af, 0x15d54739, 0x8bb1d29a, 0xfcb6e20c,
    0x65bfb3b6, 0x12b88320, 0x3fba6cad, 0x48bd5c3b, 0xd1b40d81, 0xa6b33d17,
    0x38d7a8b4, 0x4fd09822, 0xd6d9c998, 0xa1def90e, 0x3161e49f, 0x4666d409,
    0xdf6f85b3, 0xa868b525, 0x360c2086, 0x410b1010, 0xd80241aa, 0xaf05713c,
    0x220d7cc9, 0x550a4c5f, 0xcc031de5, 0xbb042d73, 0x2560b8d0, 0x52678846,
    0xcb6ed9fc, 0xbc69e96a, 0x2cd6f4fb, 0x5bd1c46d, 0xc2d895d7, 0xb5dfa541,
    0x2bbb30e2, 0x5cbc0074, 0xc5b551ce, 0xb2b26158, 0x04d44c65, 0x73d37cf3,
    0xeada2d49, 0x9ddd1ddf, 0x03b9887c, 0x74beb8ea, 0xedb7e950, 0x9ab0d9c6,
    0x0a0fc457, 0x7d08f4c1, 0xe401a57b, 0x930695ed, 0x0d62004e, 0x7a6530d8,
    0xe36c6162, 0x946b51f4, 0x19635c01, 0x6e646c97, 0xf76d3d2d, 0x806a0dbb,
    0x1e0e9818, 0x6909a88e, 0xf000f934, 0x8707c9a2, 0x17b8d433, 0x60bfe4a5,
    0xf9b6b51f, 0x8eb18589, 0x10d5102a, 0x67d220bc, 0xfedb7106, 0x89dc4190,
    0x49662d3d, 0x3e611dab, 0xa7684c11, 0xd06f7c87, 0x4e0be924, 0x390cd9b2,
    0xa0058808, 0xd702b89e, 0x47bda50f, 0x30ba9599, 0xa9b3c423, 0xdeb4f4b5,
    0x40d06116, 0x37d75180, 0xaede003a, 0xd9d930ac, 0x54d13d59, 0x23d60dcf,
    0xbadf5c75, 0xcdd86ce3, 0x53bcf940, 0x24bbc9d6, 0xbdb2986c, 0xcab5a8fa,
    0x5a0ab56b, 0x2d0d85fd, 0xb404d447, 0xc303e4d1, 0x5d677172, 0x2a6041e4,
    0xb369105e, 0xc46e20c8, 0x72080df5, 0x050f3d63, 0x9c066cd9, 0xeb015c4f,
    0x7565c9ec, 0x0262f97a, 0x9b6ba8c0, 0xec6c9856, 0x7cd385c7, 0x0bd4b551,
    0x92dde4eb, 0xe5dad47d, 0x7bbe41de, 0x0cb97148, 0x95b020f2, 0xe2b71064,
    0x6fbf1d91, 0x18b82d07, 0x81b17cbd, 0xf6b64c2b, 0x68d2d988, 0x1fd5e91e,
    0x86dcb8a4, 0xf1db8832, 0x616495a3, 0x1663a535, 0x8f6af48f, 0xf86dc419,
    0x660951ba, 0x110e612c, 0x88073096, 0xff000000},
   {0x93dbfd72, 0x8ac0cc33, 0xa1ed9ff0, 0xb8f6aeb1, 0xf7b73876, 0xeeac0937,
    0xc5815af4, 0xdc9a6bb5, 0x5b02777a, 0x4219463b, 0x693415f8, 0x702f24b9,
    0x3f6eb27e, 0x2675833f, 0x0d58d0fc, 0x1443e1bd, 0xd919ef23, 0xc002de62,
    0xeb2f8da1, 0xf234bce0, 0xbd752a27, 0xa46e1b66, 0x8f4348a5, 0x965879e4,
    0x11c0652b, 0x08db546a, 0x23f607a9, 0x3aed36e8, 0x75aca02f, 0x6cb7916e,
    0x479ac2ad, 0x5e81f3ec, 0x065fd9d0, 0x1f44e891, 0x3469bb52, 0x2d728a13,
    0x62331cd4, 0x7b282d95, 0x50057e56, 0x491e4f17, 0xce8653d8, 0xd79d6299,
    0xfcb0315a, 0xe5ab001b, 0xaaea96dc, 0xb3f1a79d, 0x98dcf45e, 0x81c7c51f,
    0x4c9dcb81, 0x5586fac0, 0x7eaba903, 0x67b09842, 0x28f10e85, 0x31ea3fc4,
    0x1ac76c07, 0x03dc5d46, 0x84444189, 0x9d5f70c8, 0xb672230b, 0xaf69124a,
    0xe028848d, 0xf933b5cc, 0xd21ee60f, 0xcb05d74e, 0x63a2b277, 0x7ab98336,
    0x5194d0f5, 0x488fe1b4, 0x07ce7773, 0x1ed54632, 0x35f815f1, 0x2ce324b0,
    0xab7b387f, 0xb260093e, 0x994d5afd, 0x80566bbc, 0xcf17fd7b, 0xd60ccc3a,
    0xfd219ff9, 0xe43aaeb8, 0x2960a026, 0x307b9167, 0x1b56c2a4, 0x024df3e5,
    0x4d0c6522, 0x54175463, 0x7f3a07a0, 0x662136e1, 0xe1b92a2e, 0xf8a21b6f,
    0xd38f48ac, 0xca9479ed, 0x85d5ef2a, 0x9ccede6b, 0xb7e38da8, 0xaef8bce9,
    0xf62696d5, 0xef3da794, 0xc410f457, 0xdd0bc516, 0x924a53d1, 0x8b516290,
    0xa07c3153, 0xb9670012, 0x3eff1cdd, 0x27e42d9c, 0x0cc97e5f, 0x15d24f1e,
    0x5a93d9d9, 0x4388e898, 0x68a5bb5b, 0x71be8a1a, 0xbce48484, 0xa5ffb5c5,
    0x8ed2e606, 0x97c9d747, 0xd8884180, 0xc19370c1, 0xeabe2302, 0xf3a51243,
    0x743d0e8c, 0x6d263fcd, 0x460b6c0e, 0x5f105d4f, 0x1051cb88, 0x094afac9,
    0x2267a90a, 0x3b7c984b, 0xa8586539, 0xb1435478, 0x9a6e07bb, 0x837536fa,
    0xcc34a03d, 0xd52f917c, 0xfe02c2bf, 0xe719f3fe, 0x6081ef31, 0x799ade70,
    0x52b78db3, 0x4bacbcf2, 0x04ed2a35, 0x1df61b74, 0x36db48b7, 0x2fc079f6,
    0xe29a7768, 0xfb814629, 0xd0ac15ea, 0xc9b724ab, 0x86f6b26c, 0x9fed832d,
    0xb4c0d0ee, 0xaddbe1af, 0x2a43fd60, 0x3358cc21, 0x18759fe2, 0x016eaea3,
    0x4e2f3864, 0x57340925, 0x7c195ae6, 0x65026ba7, 0x3ddc419b, 0x24c770da,
    0x0fea2319, 0x16f11258, 0x59b0849f, 0x40abb5de, 0x6b86e61d, 0x729dd75c,
    0xf505cb93, 0xec1efad2, 0xc733a911, 0xde289850, 0x91690e97, 0x88723fd6,
    0xa35f6c15, 0xba445d54, 0x771e53ca, 0x6e05628b, 0x45283148, 0x5c330009,
    0x137296ce, 0x0a69a78f, 0x2144f44c, 0x385fc50d, 0xbfc7d9c2, 0xa6dce883,
    0x8df1bb40, 0x94ea8a01, 0xdbab1cc6, 0xc2b02d87, 0xe99d7e44, 0xf0864f05,
    0x58212a3c, 0x413a1b7d, 0x6a1748be, 0x730c79ff, 0x3c4def38, 0x2556de79,
    0x0e7b8dba, 0x1760bcfb, 0x90f8a034, 0x89e39175, 0xa2cec2b6, 0xbbd5f3f7,
    0xf4946530, 0xed8f5471, 0xc6a207b2, 0xdfb936f3, 0x12e3386d, 0x0bf8092c,
    0x20d55aef, 0x39ce6bae, 0x768ffd69, 0x6f94cc28, 0x44b99feb, 0x5da2aeaa,
    0xda3ab265, 0xc3218324, 0xe80cd0e7, 0xf117e1a6, 0xbe567761, 0xa74d4620,
    0x8c6015e3, 0x957b24a2, 0xcda50e9e, 0xd4be3fdf, 0xff936c1c, 0xe6885d5d,
    0xa9c9cb9a, 0xb0d2fadb, 0x9bffa918, 0x82e49859, 0x057c8496, 0x1c67b5d7,
    0x374ae614, 0x2e51d755, 0x61104192, 0x780b70d3, 0x53262310, 0x4a3d1251,
    0x87671ccf, 0x9e7c2d8e, 0xb5517e4d, 0xac4a4f0c, 0xe30bd9cb, 0xfa10e88a,
    0xd13dbb49, 0xc8268a08, 0x4fbe96c7, 0x56a5a786, 0x7d88f445, 0x6493c504,
    0x2bd253c3, 0x32c96282, 0x19e43141, 0x00ff0000},
   {0xbe98cbed, 0xbf5aa1da, 0xbd1c1f83, 0xbcde75b4, 0xb9916331, 0xb8530906,
    0xba15b75f, 0xbbd7dd68, 0xb08b9a55, 0xb149f062, 0xb30f4e3b, 0xb2cd240c,
    0xb7823289, 0xb64058be, 0xb406e6e7, 0xb5c48cd0, 0xa2be689d, 0xa37c02aa,
    0xa13abcf3, 0xa0f8d6c4, 0xa5b7c041, 0xa475aa76, 0xa633142f, 0xa7f17e18,
    0xacad3925, 0xad6f5312, 0xaf29ed4b, 0xaeeb877c, 0xaba491f9, 0xaa66fbce,
    0xa8204597, 0xa9e22fa0, 0x86d58d0d, 0x8717e73a, 0x85515963, 0x84933354,
    0x81dc25d1, 0x801e4fe6, 0x8258f1bf, 0x839a9b88, 0x88c6dcb5, 0x8904b682,
    0x8b4208db, 0x8a8062ec, 0x8fcf7469, 0x8e0d1e5e, 0x8c4ba007, 0x8d89ca30,
    0x9af32e7d, 0x9b31444a, 0x9977fa13, 0x98b59024, 0x9dfa86a1, 0x9c38ec96,
    0x9e7e52cf, 0x9fbc38f8, 0x94e07fc5, 0x952215f2, 0x9764abab, 0x96a6c19c,
    0x93e9d719, 0x922bbd2e, 0x906d0377, 0x91af6940, 0xce02462d, 0xcfc02c1a,
    0xcd869243, 0xcc44f874, 0xc90beef1, 0xc8c984c6, 0xca8f3a9f, 0xcb4d50a8,
    0xc0111795, 0xc1d37da2, 0xc395c3fb, 0xc257a9cc, 0xc718bf49, 0xc6dad57e,
    0xc49c6b27, 0xc55e0110, 0xd224e55d, 0xd3e68f6a, 0xd1a03133, 0xd0625b04,
    0xd52d4d81, 0xd4ef27b6, 0xd6a999ef, 0xd76bf3d8, 0xdc37b4e5, 0xddf5ded2,
    0xdfb3608b, 0xde710abc, 0xdb3e1c39, 0xdafc760e, 0xd8bac857, 0xd978a260,
    0xf64f00cd, 0xf78d6afa, 0xf5cbd4a3, 0xf409be94, 0xf146a811, 0xf084c226,
    0xf2c27c7f, 0xf3001648, 0xf85c5175, 0xf99e3b42, 0xfbd8851b, 0xfa1aef2c,
    0xff55f9a9, 0xfe97939e, 0xfcd12dc7, 0xfd1347f0, 0xea69a3bd, 0xebabc98a,
    0xe9ed77d3, 0xe82f1de4, 0xed600b61, 0xeca26156, 0xeee4df0f, 0xef26b538,
    0xe47af205, 0xe5b89832, 0xe7fe266b, 0xe63c4c5c, 0xe3735ad9, 0xe2b130ee,
    0xe0f78eb7, 0xe135e480, 0x5fadd06d, 0x5e6fba5a, 0x5c290403, 0x5deb6e34,
    0x58a478b1, 0x59661286, 0x5b20acdf, 0x5ae2c6e8, 0x51be81d5, 0x507cebe2,
    0x523a55bb, 0x53f83f8c, 0x56b72909, 0x5775433e, 0x5533fd67, 0x54f19750,
    0x438b731d, 0x4249192a, 0x400fa773, 0x41cdcd44, 0x4482dbc1, 0x4540b1f6,
    0x47060faf, 0x46c46598, 0x4d9822a5, 0x4c5a4892, 0x4e1cf6cb, 0x4fde9cfc,
    0x4a918a79, 0x4b53e04e, 0x49155e17, 0x48d73420, 0x67e0968d, 0x6622fcba,
    0x646442e3, 0x65a628d4, 0x60e93e51, 0x612b5466, 0x636dea3f, 0x62af8008,
    0x69f3c735, 0x6831ad02, 0x6a77135b, 0x6bb5796c, 0x6efa6fe9, 0x6f3805de,
    0x6d7ebb87, 0x6cbcd1b0, 0x7bc635fd, 0x7a045fca, 0x7842e193, 0x79808ba4,
    0x7ccf9d21, 0x7d0df716, 0x7f4b494f, 0x7e892378, 0x75d56445, 0x74170e72,
    0x7651b02b, 0x7793da1c, 0x72dccc99, 0x731ea6ae, 0x715818f7, 0x709a72c0,
    0x2f375dad, 0x2ef5379a, 0x2cb389c3, 0x2d71e3f4, 0x283ef571, 0x29fc9f46,
    0x2bba211f, 0x2a784b28, 0x21240c15, 0x20e66622, 0x22a0d87b, 0x2362b24c,
    0x262da4c9, 0x27efcefe, 0x25a970a7, 0x246b1a90, 0x3311fedd, 0x32d394ea,
    0x30952ab3, 0x31574084, 0x34185601, 0x35da3c36, 0x379c826f, 0x365ee858,
    0x3d02af65, 0x3cc0c552, 0x3e867b0b, 0x3f44113c, 0x3a0b07b9, 0x3bc96d8e,
    0x398fd3d7, 0x384db9e0, 0x177a1b4d, 0x16b8717a, 0x14fecf23, 0x153ca514,
    0x1073b391, 0x11b1d9a6, 0x13f767ff, 0x12350dc8, 0x19694af5, 0x18ab20c2,
    0x1aed9e9b, 0x1b2ff4ac, 0x1e60e229, 0x1fa2881e, 0x1de43647, 0x1c265c70,
    0x0b5cb83d, 0x0a9ed20a, 0x08d86c53, 0x091a0664, 0x0c5510e1, 0x0d977ad6,
    0x0fd1c48f, 0x0e13aeb8, 0x054fe985, 0x048d83b2, 0x06cb3deb, 0x070957dc,
    0x02464159, 0x03842b6e, 0x01c29537, 0x0000ff00},
   {0xde05060e, 0x66b9616b, 0x740cce85, 0xccb0a9e0, 0x51679159, 0xe9dbf63c,
    0xfb6e59d2, 0x43d23eb7, 0x1bb12ee1, 0xa30d4984, 0xb1b8e66a, 0x0904810f,
    0x94d3b9b6, 0x2c6fded3, 0x3eda713d, 0x86661658, 0x8e1c5191, 0x36a036f4,
    0x2415991a, 0x9ca9fe7f, 0x017ec6c6, 0xb9c2a1a3, 0xab770e4d, 0x13cb6928,
    0x4ba8797e, 0xf3141e1b, 0xe1a1b1f5, 0x591dd690, 0xc4caee29, 0x7c76894c,
    0x6ec326a2, 0xd67f41c7, 0x7e37a930, 0xc68bce55, 0xd43e61bb, 0x6c8206de,
    0xf1553e67, 0x49e95902, 0x5b5cf6ec, 0xe3e09189, 0xbb8381df, 0x033fe6ba,
    0x118a4954, 0xa9362e31, 0x34e11688, 0x8c5d71ed, 0x9ee8de03, 0x2654b966,
    0x2e2efeaf, 0x969299ca, 0x84273624, 0x3c9b5141, 0xa14c69f8, 0x19f00e9d,
    0x0b45a173, 0xb3f9c616, 0xeb9ad640, 0x5326b125, 0x41931ecb, 0xf92f79ae,
    0x64f84117, 0xdc442672, 0xcef1899c, 0x764deef9, 0x45115e33, 0xfdad3956,
    0xef1896b8, 0x57a4f1dd, 0xca73c964, 0x72cfae01, 0x607a01ef, 0xd8c6668a,
    0x80a576dc, 0x381911b9, 0x2aacbe57, 0x9210d932, 0x0fc7e18b, 0xb77b86ee,
    0xa5ce2900, 0x1d724e65, 0x150809ac, 0xadb46ec9, 0xbf01c127, 0x07bda642,
    0x9a6a9efb, 0x22d6f99e, 0x30635670, 0x88df3115, 0xd0bc2143, 0x68004626,
    0x7ab5e9c8, 0xc2098ead, 0x5fdeb614, 0xe762d171, 0xf5d77e9f, 0x4d6b19fa,
    0xe523f10d, 0x5d9f9668, 0x4f2a3986, 0xf7965ee3, 0x6a41665a, 0xd2fd013f,
    0xc048aed1, 0x78f4c9b4, 0x2097d9e2, 0x982bbe87, 0x8a9e1169, 0x3222760c,
    0xaff54eb5, 0x174929d0, 0x05fc863e, 0xbd40e15b, 0xb53aa692, 0x0d86c1f7,
    0x1f336e19, 0xa78f097c, 0x3a5831c5, 0x82e456a0, 0x9051f94e, 0x28ed9e2b,
    0x708e8e7d, 0xc832e918, 0xda8746f6, 0x623b2193, 0xffec192a, 0x47507e4f,
    0x55e5d1a1, 0xed59b6c4, 0x335cb035, 0x8be0d750, 0x995578be, 0x21e91fdb,
    0xbc3e2762, 0x04824007, 0x1637efe9, 0xae8b888c, 0xf6e898da, 0x4e54ffbf,
    0x5ce15051, 0xe45d3734, 0x798a0f8d, 0xc13668e8, 0xd383c706, 0x6b3fa063,
    0x6345e7aa, 0xdbf980cf, 0xc94c2f21, 0x71f04844, 0xec2770fd, 0x549b1798,
    0x462eb876, 0xfe92df13, 0xa6f1cf45, 0x1e4da820, 0x0cf807ce, 0xb44460ab,
    0x29935812, 0x912f3f77, 0x839a9099, 0x3b26f7fc, 0x936e1f0b, 0x2bd2786e,
    0x3967d780, 0x81dbb0e5, 0x1c0c885c, 0xa4b0ef39, 0xb60540d7, 0x0eb927b2,
    0x56da37e4, 0xee665081, 0xfcd3ff6f, 0x446f980a, 0xd9b8a0b3, 0x6104c7d6,
    0x73b16838, 0xcb0d0f5d, 0xc3774894, 0x7bcb2ff1, 0x697e801f, 0xd1c2e77a,
    0x4c15dfc3, 0xf4a9b8a6, 0xe61c1748, 0x5ea0702d, 0x06c3607b, 0xbe7f071e,
    0xaccaa8f0, 0x1476cf95, 0x89a1f72c, 0x311d9049, 0x23a83fa7, 0x9b1458c2,
    0xa848e808, 0x10f48f6d, 0x02412083, 0xbafd47e6, 0x272a7f5f, 0x9f96183a,
    0x8d23b7d4, 0x359fd0b1, 0x6dfcc0e7, 0xd540a782, 0xc7f5086c, 0x7f496f09,
    0xe29e57b0, 0x5a2230d5, 0x48979f3b, 0xf02bf85e, 0xf851bf97, 0x40edd8f2,
    0x5258771c, 0xeae41079, 0x773328c0, 0xcf8f4fa5, 0xdd3ae04b, 0x6586872e,
    0x3de59778, 0x8559f01d, 0x97ec5ff3, 0x2f503896, 0xb287002f, 0x0a3b674a,
    0x188ec8a4, 0xa032afc1, 0x087a4736, 0xb0c62053, 0xa2738fbd, 0x1acfe8d8,
    0x8718d061, 0x3fa4b704, 0x2d1118ea, 0x95ad7f8f, 0xcdce6fd9, 0x757208bc,
    0x67c7a752, 0xdf7bc037, 0x42acf88e, 0xfa109feb, 0xe8a53005, 0x50195760,
    0x586310a9, 0xe0df77cc, 0xf26ad822, 0x4ad6bf47, 0xd70187fe, 0x6fbde09b,
    0x7d084f75, 0xc5b42810, 0x9dd73846, 0x256b5f23, 0x37def0cd, 0x8f6297a8,
    0x12b5af11, 0xaa09c874, 0xb8bc679a, 0x000000ff},
   {0xe7662801, 0xda0601b1, 0x9da67b61, 0xa0c652d1, 0x12e68ec1, 0x2f86a771,
    0x6826dda1, 0x5546f411, 0xd71663c0, 0xea764a70, 0xadd630a0, 0x90b61910,
    0x2296c500, 0x1ff6ecb0, 0x58569660, 0x6536bfd0, 0x8786bf83, 0xbae69633,
    0xfd46ece3, 0xc026c553, 0x72061943, 0x4f6630f3, 0x08c64a23, 0x35a66393,
    0xb7f6f442, 0x8a96ddf2, 0xcd36a722, 0xf0568e92, 0x42765282, 0x7f167b32,
    0x38b601e2, 0x05d62852, 0x26a70705, 0x1bc72eb5, 0x5c675465, 0x61077dd5,
    0xd327a1c5, 0xee478875, 0xa9e7f2a5, 0x9487db15, 0x16d74cc4, 0x2bb76574,
    0x6c171fa4, 0x51773614, 0xe357ea04, 0xde37c3b4, 0x9997b964, 0xa4f790d4,
    0x46479087, 0x7b27b937, 0x3c87c3e7, 0x01e7ea57, 0xb3c73647, 0x8ea71ff7,
    0xc9076527, 0xf4674c97, 0x7637db46, 0x4b57f2f6, 0x0cf78826, 0x3197a196,
    0x83b77d86, 0xbed75436, 0xf9772ee6, 0xc4170756, 0xbf957048, 0x82f559f8,
    0xc5552328, 0xf8350a98, 0x4a15d688, 0x7775ff38, 0x30d585e8, 0x0db5ac58,
    0x8fe53b89, 0xb2851239, 0xf52568e9, 0xc8454159, 0x7a659d49, 0x4705b4f9,
    0x00a5ce29, 0x3dc5e799, 0xdf75e7ca, 0xe215ce7a, 0xa5b5b4aa, 0x98d59d1a,
    0x2af5410a, 0x179568ba, 0x5035126a, 0x6d553bda, 0xef05ac0b, 0xd26585bb,
    0x95c5ff6b, 0xa8a5d6db, 0x1a850acb, 0x27e5237b, 0x604559ab, 0x5d25701b,
    0x7e545f4c, 0x433476fc, 0x04940c2c, 0x39f4259c, 0x8bd4f98c, 0xb6b4d03c,
    0xf114aaec, 0xcc74835c, 0x4e24148d, 0x73443d3d, 0x34e447ed, 0x09846e5d,
    0xbba4b24d, 0x86c49bfd, 0xc164e12d, 0xfc04c89d, 0x1eb4c8ce, 0x23d4e17e,
    0x64749bae, 0x5914b21e, 0xeb346e0e, 0xd65447be, 0x91f43d6e, 0xac9414de,
    0x2ec4830f, 0x13a4aabf, 0x5404d06f, 0x6964f9df, 0xdb4425cf, 0xe6240c7f,
    0xa18476af, 0x9ce45f1f, 0x56809893, 0x6be0b123, 0x2c40cbf3, 0x1120e243,
    0xa3003e53, 0x9e6017e3, 0xd9c06d33, 0xe4a04483, 0x66f0d352, 0x5b90fae2,
    0x1c308032, 0x2150a982, 0x93707592, 0xae105c22, 0xe9b026f2, 0xd4d00f42,
    0x36600f11, 0x0b0026a1, 0x4ca05c71, 0x71c075c1, 0xc3e0a9d1, 0xfe808061,
    0xb920fab1, 0x8440d301, 0x061044d0, 0x3b706d60, 0x7cd017b0, 0x41b03e00,
    0xf390e210, 0xcef0cba0, 0x8950b170, 0xb43098c0, 0x9741b797, 0xaa219e27,
    0xed81e4f7, 0xd0e1cd47, 0x62c11157, 0x5fa138e7, 0x18014237, 0x25616b87,
    0xa731fc56, 0x9a51d5e6, 0xddf1af36, 0xe0918686, 0x52b15a96, 0x6fd17326,
    0x287109f6, 0x15112046, 0xf7a12015, 0xcac109a5, 0x8d617375, 0xb0015ac5,
    0x022186d5, 0x3f41af65, 0x78e1d5b5, 0x4581fc05, 0xc7d16bd4, 0xfab14264,
    0xbd1138b4, 0x80711104, 0x3251cd14, 0x0f31e4a4, 0x48919e74, 0x75f1b7c4,
    0x0e73c0da, 0x3313e96a, 0x74b393ba, 0x49d3ba0a, 0xfbf3661a, 0xc6934faa,
    0x8133357a, 0xbc531cca, 0x3e038b1b, 0x0363a2ab, 0x44c3d87b, 0x79a3f1cb,
    0xcb832ddb, 0xf6e3046b, 0xb1437ebb, 0x8c23570b, 0x6e935758, 0x53f37ee8,
    0x14530438, 0x29332d88, 0x9b13f198, 0xa673d828, 0xe1d3a2f8, 0xdcb38b48,
    0x5ee31c99, 0x63833529, 0x24234ff9, 0x19436649, 0xab63ba59, 0x960393e9,
    0xd1a3e939, 0xecc3c089, 0xcfb2efde, 0xf2d2c66e, 0xb572bcbe, 0x8812950e,
    0x3a32491e, 0x075260ae, 0x40f21a7e, 0x7d9233ce, 0xffc2a41f, 0xc2a28daf,
    0x8502f77f, 0xb862decf, 0x0a4202df, 0x37222b6f, 0x708251bf, 0x4de2780f,
    0xaf52785c, 0x923251ec, 0xd5922b3c, 0xe8f2028c, 0x5ad2de9c, 0x67b2f72c,
    0x20128dfc, 0x1d72a44c, 0x9f22339d, 0xa2421a2d, 0xe5e260fd, 0xd882494d,
    0x6aa2955d, 0x57c2bced, 0x1062c63d, 0x2d02ef8d},
   {0x77e056be, 0xbcbc851b, 0x3a28f7b5, 0xf1742410, 0xec7114a8, 0x272dc70d,
    0xa1b9b5a3, 0x6ae56606, 0x9bb3d4d3, 0x50ef0776, 0xd67b75d8, 0x1d27a67d,
    0x002296c5, 0xcb7e4560, 0x4dea37ce, 0x86b6e46b, 0x74365425, 0xbf6a8780,
    0x39fef52e, 0xf2a2268b, 0xefa71633, 0x24fbc596, 0xa26fb738, 0x6933649d,
    0x9865d648, 0x533905ed, 0xd5ad7743, 0x1ef1a4e6, 0x03f4945e, 0xc8a847fb,
    0x4e3c3555, 0x8560e6f0, 0x704c5388, 0xbb10802d, 0x3d84f283, 0xf6d82126,
    0xebdd119e, 0x2081c23b, 0xa615b095, 0x6d496330, 0x9c1fd1e5, 0x57430240,
    0xd1d770ee, 0x1a8ba34b, 0x078e93f3, 0xccd24056, 0x4a4632f8, 0x811ae15d,
    0x739a5113, 0xb8c682b6, 0x3e52f018, 0xf50e23bd, 0xe80b1305, 0x2357c0a0,
    0xa5c3b20e, 0x6e9f61ab, 0x9fc9d37e, 0x549500db, 0xd2017275, 0x195da1d0,
    0x04589168, 0xcf0442cd, 0x49903063, 0x82cce3c6, 0x78b85cd2, 0xb3e48f77,
    0x3570fdd9, 0xfe2c2e7c, 0xe3291ec4, 0x2875cd61, 0xaee1bfcf, 0x65bd6c6a,
    0x94ebdebf, 0x5fb70d1a, 0xd9237fb4, 0x127fac11, 0x0f7a9ca9, 0xc4264f0c,
    0x42b23da2, 0x89eeee07, 0x7b6e5e49, 0xb0328dec, 0x36a6ff42, 0xfdfa2ce7,
    0xe0ff1c5f, 0x2ba3cffa, 0xad37bd54, 0x666b6ef1, 0x973ddc24, 0x5c610f81,
    0xdaf57d2f, 0x11a9ae8a, 0x0cac9e32, 0xc7f04d97, 0x41643f39, 0x8a38ec9c,
    0x7f1459e4, 0xb4488a41, 0x32dcf8ef, 0xf9802b4a, 0xe4851bf2, 0x2fd9c857,
    0xa94dbaf9, 0x6211695c, 0x9347db89, 0x581b082c, 0xde8f7a82, 0x15d3a927,
    0x08d6999f, 0xc38a4a3a, 0x451e3894, 0x8e42eb31, 0x7cc25b7f, 0xb79e88da,
    0x310afa74, 0xfa5629d1, 0xe7531969, 0x2c0fcacc, 0xaa9bb862, 0x61c76bc7,
    0x9091d912, 0x5bcd0ab7, 0xdd597819, 0x1605abbc, 0x0b009b04, 0xc05c48a1,
    0x46c83a0f, 0x8d94e9aa, 0x69504266, 0xa20c91c3, 0x2498e36d, 0xefc430c8,
    0xf2c10070, 0x399dd3d5, 0xbf09a17b, 0x745572de, 0x8503c00b, 0x4e5f13ae,
    0xc8cb6100, 0x0397b2a5, 0x1e92821d, 0xd5ce51b8, 0x535a2316, 0x9806f0b3,
    0x6a8640fd, 0xa1da9358, 0x274ee1f6, 0xec123253, 0xf11702eb, 0x3a4bd14e,
    0xbcdfa3e0, 0x77837045, 0x86d5c290, 0x4d891135, 0xcb1d639b, 0x0041b03e,
    0x1d448086, 0xd6185323, 0x508c218d, 0x9bd0f228, 0x6efc4750, 0xa5a094f5,
    0x2334e65b, 0xe86835fe, 0xf56d0546, 0x3e31d6e3, 0xb8a5a44d, 0x73f977e8,
    0x82afc53d, 0x49f31698, 0xcf676436, 0x043bb793, 0x193e872b, 0xd262548e,
    0x54f62620, 0x9faaf585, 0x6d2a45cb, 0xa676966e, 0x20e2e4c0, 0xebbe3765,
    0xf6bb07dd, 0x3de7d478, 0xbb73a6d6, 0x702f7573, 0x8179c7a6, 0x4a251403,
    0xccb166ad, 0x07edb508, 0x1ae885b0, 0xd1b45615, 0x572024bb, 0x9c7cf71e,
    0x6608480a, 0xad549baf, 0x2bc0e901, 0xe09c3aa4, 0xfd990a1c, 0x36c5d9b9,
    0xb051ab17, 0x7b0d78b2, 0x8a5bca67, 0x410719c2, 0xc7936b6c, 0x0ccfb8c9,
    0x11ca8871, 0xda965bd4, 0x5c02297a, 0x975efadf, 0x65de4a91, 0xae829934,
    0x2816eb9a, 0xe34a383f, 0xfe4f0887, 0x3513db22, 0xb387a98c, 0x78db7a29,
    0x898dc8fc, 0x42d11b59, 0xc44569f7, 0x0f19ba52, 0x121c8aea, 0xd940594f,
    0x5fd42be1, 0x9488f844, 0x61a44d3c, 0xaaf89e99, 0x2c6cec37, 0xe7303f92,
    0xfa350f2a, 0x3169dc8f, 0xb7fdae21, 0x7ca17d84, 0x8df7cf51, 0x46ab1cf4,
    0xc03f6e5a, 0x0b63bdff, 0x16668d47, 0xdd3a5ee2, 0x5bae2c4c, 0x90f2ffe9,
    0x62724fa7, 0xa92e9c02, 0x2fbaeeac, 0xe4e63d09, 0xf9e30db1, 0x32bfde14,
    0xb42bacba, 0x7f777f1f, 0x8e21cdca, 0x457d1e6f, 0xc3e96cc1, 0x08b5bf64,
    0x15b08fdc, 0xdeec5c79, 0x58782ed7, 0x9324fd72},
   {0x2cae7edd, 0x8ad97569, 0xbb316ff4, 0x1d466440, 0xd8e15ace, 0x7e96517a,
    0x4f7e4be7, 0xe9094053, 0x1f4130ba, 0xb9363b0e, 0x88de2193, 0x2ea92a27,
    0xeb0e14a9, 0x4d791f1d, 0x7c910580, 0xdae60e34, 0x4b70e213, 0xed07e9a7,
    0xdceff33a, 0x7a98f88e, 0xbf3fc600, 0x1948cdb4, 0x28a0d729, 0x8ed7dc9d,
    0x789fac74, 0xdee8a7c0, 0xef00bd5d, 0x4977b6e9, 0x8cd08867, 0x2aa783d3,
    0x1b4f994e, 0xbd3892fa, 0xe3134741, 0x45644cf5, 0x748c5668, 0xd2fb5ddc,
    0x175c6352, 0xb12b68e6, 0x80c3727b, 0x26b479cf, 0xd0fc0926, 0x768b0292,
    0x4763180f, 0xe11413bb, 0x24b32d35, 0x82c42681, 0xb32c3c1c, 0x155b37a8,
    0x84cddb8f, 0x22bad03b, 0x1352caa6, 0xb525c112, 0x7082ff9c, 0xd6f5f428,
    0xe71deeb5, 0x416ae501, 0xb72295e8, 0x11559e5c, 0x20bd84c1, 0x86ca8f75,
    0x436db1fb, 0xe51aba4f, 0xd4f2a0d2, 0x7285ab66, 0x68a50ba4, 0xced20010,
    0xff3a1a8d, 0x594d1139, 0x9cea2fb7, 0x3a9d2403, 0x0b753e9e, 0xad02352a,
    0x5b4a45c3, 0xfd3d4e77, 0xccd554ea, 0x6aa25f5e, 0xaf0561d0, 0x09726a64,
    0x389a70f9, 0x9eed7b4d, 0x0f7b976a, 0xa90c9cde, 0x98e48643, 0x3e938df7,
    0xfb34b379, 0x5d43b8cd, 0x6caba250, 0xcadca9e4, 0x3c94d90d, 0x9ae3d2b9,
    0xab0bc824, 0x0d7cc390, 0xc8dbfd1e, 0x6eacf6aa, 0x5f44ec37, 0xf933e783,
    0xa7183238, 0x016f398c, 0x30872311, 0x96f028a5, 0x5357162b, 0xf5201d9f,
    0xc4c80702, 0x62bf0cb6, 0x94f77c5f, 0x328077eb, 0x03686d76, 0xa51f66c2,
    0x60b8584c, 0xc6cf53f8, 0xf7274965, 0x515042d1, 0xc0c6aef6, 0x66b1a542,
    0x5759bfdf, 0xf12eb46b, 0x34898ae5, 0x92fe8151, 0xa3169bcc, 0x05619078,
    0xf329e091, 0x555eeb25, 0x64b6f1b8, 0xc2c1fa0c, 0x0766c482, 0xa111cf36,
    0x90f9d5ab, 0x368ede1f, 0xa4b8942f, 0x02cf9f9b, 0x33278506, 0x95508eb2,
    0x50f7b03c, 0xf680bb88, 0xc768a115, 0x611faaa1, 0x9757da48, 0x3120d1fc,
    0x00c8cb61, 0xa6bfc0d5, 0x6318fe5b, 0xc56ff5ef, 0xf487ef72, 0x52f0e4c6,
    0xc36608e1, 0x65110355, 0x54f919c8, 0xf28e127c, 0x37292cf2, 0x915e2746,
    0xa0b63ddb, 0x06c1366f, 0xf0894686, 0x56fe4d32, 0x671657af, 0xc1615c1b,
    0x04c66295, 0xa2b16921, 0x935973bc, 0x352e7808, 0x6b05adb3, 0xcd72a607,
    0xfc9abc9a, 0x5aedb72e, 0x9f4a89a0, 0x393d8214, 0x08d59889, 0xaea2933d,
    0x58eae3d4, 0xfe9de860, 0xcf75f2fd, 0x6902f949, 0xaca5c7c7, 0x0ad2cc73,
    0x3b3ad6ee, 0x9d4ddd5a, 0x0cdb317d, 0xaaac3ac9, 0x9b442054, 0x3d332be0,
    0xf894156e, 0x5ee31eda, 0x6f0b0447, 0xc97c0ff3, 0x3f347f1a, 0x994374ae,
    0xa8ab6e33, 0x0edc6587, 0xcb7b5b09, 0x6d0c50bd, 0x5ce44a20, 0xfa934194,
    0xe0b3e156, 0x46c4eae2, 0x772cf07f, 0xd15bfbcb, 0x14fcc545, 0xb28bcef1,
    0x8363d46c, 0x2514dfd8, 0xd35caf31, 0x752ba485, 0x44c3be18, 0xe2b4b5ac,
    0x27138b22, 0x81648096, 0xb08c9a0b, 0x16fb91bf, 0x876d7d98, 0x211a762c,
    0x10f26cb1, 0xb6856705, 0x7322598b, 0xd555523f, 0xe4bd48a2, 0x42ca4316,
    0xb48233ff, 0x12f5384b, 0x231d22d6, 0x856a2962, 0x40cd17ec, 0xe6ba1c58,
    0xd75206c5, 0x71250d71, 0x2f0ed8ca, 0x8979d37e, 0xb891c9e3, 0x1ee6c257,
    0xdb41fcd9, 0x7d36f76d, 0x4cdeedf0, 0xeaa9e644, 0x1ce196ad, 0xba969d19,
    0x8b7e8784, 0x2d098c30, 0xe8aeb2be, 0x4ed9b90a, 0x7f31a397, 0xd946a823,
    0x48d04404, 0xeea74fb0, 0xdf4f552d, 0x79385e99, 0xbc9f6017, 0x1ae86ba3,
    0x2b00713e, 0x8d777a8a, 0x7b3f0a63, 0xdd4801d7, 0xeca01b4a, 0x4ad710fe,
    0x8f702e70, 0x290725c4, 0x18ef3f59, 0xbe9834ed},
   {0xf84e0017, 0x34e40089, 0xba6b076a, 0x76c107f4, 0x7c040eed, 0xb0ae0e73,
    0x3e210990, 0xf28b090e, 0x2bab1ba2, 0xe7011b3c, 0x698e1cdf, 0xa5241c41,
    0xafe11558, 0x634b15c6, 0xedc41225, 0x216e12bb, 0x84f5313c, 0x485f31a2,
    0xc6d03641, 0x0a7a36df, 0x00bf3fc6, 0xcc153f58, 0x429a38bb, 0x8e303825,
    0x57102a89, 0x9bba2a17, 0x15352df4, 0xd99f2d6a, 0xd35a2473, 0x1ff024ed,
    0x917f230e, 0x5dd52390, 0x01386241, 0xcd9262df, 0x431d653c, 0x8fb765a2,
    0x85726cbb, 0x49d86c25, 0xc7576bc6, 0x0bfd6b58, 0xd2dd79f4, 0x1e77796a,
    0x90f87e89, 0x5c527e17, 0x5697770e, 0x9a3d7790, 0x14b27073, 0xd81870ed,
    0x7d83536a, 0xb12953f4, 0x3fa65417, 0xf30c5489, 0xf9c95d90, 0x35635d0e,
    0xbbec5aed, 0x77465a73, 0xae6648df, 0x62cc4841, 0xec434fa2, 0x20e94f3c,
    0x2a2c4625, 0xe68646bb, 0x68094158, 0xa4a341c6, 0xd1d3c2fa, 0x1d79c264,
    0x93f6c587, 0x5f5cc519, 0x5599cc00, 0x9933cc9e, 0x17bccb7d, 0xdb16cbe3,
    0x0236d94f, 0xce9cd9d1, 0x4013de32, 0x8cb9deac, 0x867cd7b5, 0x4ad6d72b,
    0xc459d0c8, 0x08f3d056, 0xad68f3d1, 0x61c2f34f, 0xef4df4ac, 0x23e7f432,
    0x2922fd2b, 0xe588fdb5, 0x6b07fa56, 0xa7adfac8, 0x7e8de864, 0xb227e8fa,
    0x3ca8ef19, 0xf002ef87, 0xfac7e69e, 0x366de600, 0xb8e2e1e3, 0x7448e17d,
    0x28a5a0ac, 0xe40fa032, 0x6a80a7d1, 0xa62aa74f, 0xacefae56, 0x6045aec8,
    0xeecaa92b, 0x2260a9b5, 0xfb40bb19, 0x37eabb87, 0xb965bc64, 0x75cfbcfa,
    0x7f0ab5e3, 0xb3a0b57d, 0x3d2fb29e, 0xf185b200, 0x541e9187, 0x98b49119,
    0x163b96fa, 0xda919664, 0xd0549f7d, 0x1cfe9fe3, 0x92719800, 0x5edb989e,
    0x87fb8a32, 0x4b518aac, 0xc5de8d4f, 0x09748dd1, 0x03b184c8, 0xcf1b8456,
    0x419483b5, 0x8d3e832b, 0xab7585cd, 0x67df8553, 0xe95082b0, 0x25fa822e,
    0x2f3f8b37, 0xe3958ba9, 0x6d1a8c4a, 0xa1b08cd4, 0x78909e78, 0xb43a9ee6,
    0x3ab59905, 0xf61f999b, 0xfcda9082, 0x3070901c, 0xbeff97ff, 0x72559761,
    0xd7ceb4e6, 0x1b64b478, 0x95ebb39b, 0x5941b305, 0x5384ba1c, 0x9f2eba82,
    0x11a1bd61, 0xdd0bbdff, 0x042baf53, 0xc881afcd, 0x460ea82e, 0x8aa4a8b0,
    0x8061a1a9, 0x4ccba137, 0xc244a6d4, 0x0eeea64a, 0x5203e79b, 0x9ea9e705,
    0x1026e0e6, 0xdc8ce078, 0xd649e961, 0x1ae3e9ff, 0x946cee1c, 0x58c6ee82,
    0x81e6fc2e, 0x4d4cfcb0, 0xc3c3fb53, 0x0f69fbcd, 0x05acf2d4, 0xc906f24a,
    0x4789f5a9, 0x8b23f537, 0x2eb8d6b0, 0xe212d62e, 0x6c9dd1cd, 0xa037d153,
    0xaaf2d84a, 0x6658d8d4, 0xe8d7df37, 0x247ddfa9, 0xfd5dcd05, 0x31f7cd9b,
    0xbf78ca78, 0x73d2cae6, 0x7917c3ff, 0xb5bdc361, 0x3b32c482, 0xf798c41c,
    0x82e84720, 0x4e4247be, 0xc0cd405d, 0x0c6740c3, 0x06a249da, 0xca084944,
    0x44874ea7, 0x882d4e39, 0x510d5c95, 0x9da75c0b, 0x13285be8, 0xdf825b76,
    0xd547526f, 0x19ed52f1, 0x97625512, 0x5bc8558c, 0xfe53760b, 0x32f97695,
    0xbc767176, 0x70dc71e8, 0x7a1978f1, 0xb6b3786f, 0x383c7f8c, 0xf4967f12,
    0x2db66dbe, 0xe11c6d20, 0x6f936ac3, 0xa3396a5d, 0xa9fc6344, 0x655663da,
    0xebd96439, 0x277364a7, 0x7b9e2576, 0xb73425e8, 0x39bb220b, 0xf5112295,
    0xffd42b8c, 0x337e2b12, 0xbdf12cf1, 0x715b2c6f, 0xa87b3ec3, 0x64d13e5d,
    0xea5e39be, 0x26f43920, 0x2c313039, 0xe09b30a7, 0x6e143744, 0xa2be37da,
    0x0725145d, 0xcb8f14c3, 0x45001320, 0x89aa13be, 0x836f1aa7, 0x4fc51a39,
    0xc14a1dda, 0x0de01d44, 0xd4c00fe8, 0x186a0f76, 0x96e50895, 0x5a4f080b,
    0x508a0112, 0x9c20018c, 0x12af066f, 0xde0506f1}
};

uint32_t crc32iso_hdlc_byte(uint32_t crc, void const *mem, size_t len) {
    uint8_t const *data = mem;
    if (data == NULL)
        return 0;
    for (size_t i = 0; i < len; i++) {
        crc = (crc >> 8) ^
              table_byte[(crc ^ data[i]) & 0xff];
    }
    return crc;
}

// This code assumes that integers are stored little-endian.

uint32_t crc32iso_hdlc_word(uint32_t crc, void const *mem, size_t len) {
    uint8_t const *data = mem;
    if (data == NULL)
        return 0;
    while (len && ((ptrdiff_t)data & 0x7)) {
        len--;
        crc = (crc >> 8) ^
              table_byte[(crc ^ *data++) & 0xff];
    }
    size_t n = len >> 3;
    for (size_t i = 0; i < n; i++) {
        uint64_t word = crc ^ ((uint64_t const *)data)[i];
        crc = table_word[7][word & 0xff] ^
              table_word[6][(word >> 8) & 0xff] ^
              table_word[5][(word >> 16) & 0xff] ^
              table_word[4][(word >> 24) & 0xff] ^
              table_word[3][(word >> 32) & 0xff] ^
              table_word[2][(word >> 40) & 0xff] ^
              table_word[1][(word >> 48) & 0xff] ^
              table_word[0][word >> 56];
    }
    data += n << 3;
    len &= 7;
    while (len) {
        len--;
        crc = (crc >> 8) ^
              table_byte[(crc ^ *data++) & 0xff];
    }
    return crc;
}

static uint32_t multmodp(uint32_t a, uint32_t b) {
    uint32_t prod = 0;
    for (;;) {
        if (a & 0x80000000) {
            prod ^= b;
            if ((a & 0x7fffffff) == 0)
                break;
        }
        a <<= 1;
        b = b & 1 ? (b >> 1) ^ 0xedb88320 : b >> 1;
    }
    return prod;
}

static uint32_t const table_comb[] = {
    0x40000000, 0x20000000, 0x08000000, 0x00800000, 0x00008000, 0xedb88320,
    0xb1e6b092, 0xa06a2517, 0xed627dae, 0x88d14467, 0xd7bbfe6a, 0xec447f11,
    0x8e7ea170, 0x6427800e, 0x4d47bae0, 0x09fe548f, 0x83852d0f, 0x30362f1a,
    0x7b5a9cc3, 0x31fec169, 0x9fec022a, 0x6c8dedc4, 0x15d6874d, 0x5fde7a4e,
    0xbad90e37, 0x2e4e5eef, 0x4eaba214, 0xa8a472c0, 0x429a969e, 0x148d302a,
    0xc40ba6d0, 0xc4e22c3c
};

static uint32_t x8nmodp(uintmax_t n) {
    uint32_t xp = 0x80000000;
    int k = 3;
    for (;;) {
        if (n & 1)
            xp = multmodp(table_comb[k], xp);
        n >>= 1;
        if (n == 0)
            break;
        if (++k == 32)
            k = 0;
    }
    return xp;
}

uint32_t crc32iso_hdlc_comb(uint32_t crc1, uint32_t crc2,
        uintmax_t len2) {
    return multmodp(x8nmodp(len2), crc1) ^ crc2;
}
