#pragma once

// FNV1A_STR_RT - Runtime
constexpr uint32_t FNV1A_STR_RT(char* buf, uint32_t ret = 0x811C9DC5) {
  return buf[0] == 0 ? ret : FNV1A_STR_RT(&buf[1], (ret ^ buf[0]) * 0x01000193);
}

// FNV1A_STR_CT - Immediate Compile time
consteval uint32_t FNV1A_STR_CT(char* buf, uint32_t ret = 0x811C9DC5) {
  return buf[0] == 0 ? ret : FNV1A_STR_CT(&buf[1], (ret ^ buf[0]) * 0x01000193);
}

// FNV1A - Runtime
constexpr uint32_t FNV1A_RT(char* buf, int len, uint32_t ret = 0x811C9DC5) {
  return len <= 0 ? ret : FNV1A_RT(&buf[1], --len, (ret ^ buf[0]) * 0x01000193);
}

// FNV1A - Immediate Compile time
consteval uint32_t FNV1A_CT(char* buf, int len, uint32_t ret = 0x811C9DC5) {
  return len <= 0 ? ret : FNV1A_CT(&buf[1], --len, (ret ^ buf[0]) * 0x01000193);
}

// https://en.wikipedia.org/wiki/Fowler–Noll–Vo hash function
#define HASH_RT(str) (FNV1A_STR_RT(str))
#define HASH(str) (FNV1A_STR_CT(str))
//#define HASH(s) (CRC32C(s, sizeof(s) - 1))