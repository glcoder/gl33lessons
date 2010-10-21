#ifndef CRC32_H
#define CRC32_H

#include <stdint.h>
#include <stdlib.h>

uint32_t crc32_hash(const uint8_t *buffer, size_t length);

#endif /* CRC32_H */
