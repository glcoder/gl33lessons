#ifndef CRC32_H
#define CRC32_H

#include <cstdint>
#include <cstdlib>

uint32_t crc32_hash(const uint8_t *buffer, size_t length);

#endif /* CRC32_H */
