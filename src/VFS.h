#ifndef VFS_H
#define VFS_H

#include "common.h"

enum VFSDataType { VFS_BINARY = 0, VFS_TEXT };

namespace VFS
{
	bool load(const char *name, VFSDataType type,
		uint8_t **buffer, uint32_t *size);
}

#endif /* VFS_H */
