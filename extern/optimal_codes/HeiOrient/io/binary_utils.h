#pragma once
#include <arpa/inet.h>

#if __BIG_ENDIAN__
#define htonll(x) (x)
#define ntohll(x) (x)
#else
#define htonlli(x) (((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohlli(x) (((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))
inline uint64_t ntohll(uint64_t x) { return ntohlli(x); }
inline uint64_t htonll(uint64_t x) { return htonlli(x); }
#endif