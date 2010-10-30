#include "common.h"

bool LoadFile(const char *fileName, bool binary, uint8_t **buffer, uint32_t *size)
{
	ASSERT(fileName);
	ASSERT(buffer);
	ASSERT(size);

	FILE     *input;
	uint32_t fileSize, readed;

	const char mode[] = {'r', binary ? 'b' : 't', '\0'};

	if ((input = fopen(fileName, mode)) == NULL)
	{
		LOG_ERROR("Opening file '%s'\n", fileName);
		return false;
	}

	fseek(input, 0, SEEK_END);
	fileSize = (uint32_t)ftell(input);
	rewind(input);

	if (fileSize == 0)
	{
		LOG_ERROR("Empty file '%s'\n", fileName);
		fclose(input);
		return false;
	}

	*buffer = new uint8_t[fileSize];
	ASSERT(*buffer);

	readed = fread(*buffer, 1, fileSize, input);

	fclose(input);

	if (readed != fileSize)
	{
		LOG_ERROR("Reading file '%s'\n", fileName);
		delete[] *buffer;
		return false;
	}

	*size = fileSize;

	return true;
}
