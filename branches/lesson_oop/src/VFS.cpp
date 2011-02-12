#include "VFS.h"

bool VFS::load(const char *name, VFSDataType type,
	uint8_t **buffer, uint32_t *size)
{
	ASSERT(name);
	ASSERT(buffer);
	ASSERT(size);

	FILE       *input = NULL;
	const char mode[] = {'r', type == VFS_BINARY ? 'b' : 't', '\0'};

	if ((input = fopen(name, mode)) == NULL)
	{
		LOG_ERROR("Opening file '%s'\n", name);
		return false;
	}

	fseek(input, 0, SEEK_END);
	*size = (uint32_t)ftell(input);
	rewind(input);

	if (*size == 0)
	{
		LOG_ERROR("Empty file '%s'\n", name);
		fclose(input);
		return false;
	}

	*buffer = new uint8_t[*size];
	ASSERT(*buffer);

	fread(*buffer, 1, *size, input);
	fclose(input);

	return true;
}
