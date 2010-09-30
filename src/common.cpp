#include "common.h"

bool LoadFile(const char *fileName, bool binary, char **buffer, long *size)
{
	ASSERT(fileName);
	ASSERT(buffer);
	ASSERT(size);

	FILE *input;
	char *buffer;
	long readed;
	bool result;

	const char mode[] = {'r', binary ? 'b' : 't', '\0'};

	if ((input = fopen(fileName, mode)) == NULL)
	{
		LOG_ERROR("Opening file '%s'\n", fileName);
		return false;
	}

	fseek(input, 0L, SEEK_END);
	*size = ftell(input);
	rewind(input);

	if (*size == 0L)
	{
		LOG_ERROR("Empty file '%s'\n", fileName);
		fclose(input);
		return false;
	}

	*buffer = new char[size];
	ASSERT(*buffer);

	readed = fread(buffer, 1, *size, input);

	fclose(input);

	if (readed != *size)
	{
		LOG_ERROR("Reading file '%s'\n", fileName);
		delete[] *buffer;
		return false;
	}

	return true;
}

