#include "Texture.h"

#pragma pack(1)
struct TGAHeader
{
	uint8_t  idlength;
	uint8_t  colormap;
	uint8_t  datatype;
	uint8_t  colormapinfo[5];
	uint16_t xorigin;
	uint16_t yorigin;
	uint16_t width;
	uint16_t height;
	uint8_t  bitperpel;
	uint8_t  description;
};
#pragma pack()

Texture::Texture():
	m_handle(0), m_target(0)
{
}

Texture::~Texture()
{
	destroy();
}

void Texture::create(GLint minFilter, GLint magFilter,
	GLint warp, GLenum target)
{
	ASSERT(m_handle == 0);

	m_target = target;

	glGenTextures(1, &m_handle);
	glBindTexture(m_target, m_handle);

	glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, magFilter);

	glTexParameteri(m_target, GL_TEXTURE_WRAP_S, warp);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_T, warp);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_R, warp);
}

void Texture::destroy()
{
	glDeleteTextures(1, &m_handle);
	m_handle = 0;
	m_target = 0;
}

void Texture::image2D(const GLvoid *data, GLsizei width, GLsizei height,
	GLint internalFormat, GLenum format, GLenum type, bool genMIPs)
{
	ASSERT(m_handle);

	glBindTexture(m_target, m_handle);

	glTexImage2D(m_target, 0, internalFormat, width, height,
		0, format, type, data);

	if (genMIPs)
		glGenerateMipmap(m_target);
}

bool Texture::load2DTGA(const char *name, bool genMIPs)
{
	ASSERT(name);

	uint8_t   *buffer;
	uint32_t  size;
	GLint     format, internalFormat;

	if (!VFS::load(name, VFS_BINARY, &buffer, &size))
		return false;

	if (size <= sizeof(TGAHeader))
	{
		LOG_ERROR("Too small file '%s'\n", name);
		delete[] buffer;
		return false;
	}

	const TGAHeader *header = (const TGAHeader *)buffer;

	if (header->datatype != 2 || (header->bitperpel != 24
		&& header->bitperpel != 32))
	{
		LOG_ERROR("Wrong TGA format '%s'\n", name);
		delete[] buffer;
		return false;
	}

	format         = (header->bitperpel == 24 ? GL_BGR : GL_BGRA);
	internalFormat = (format == GL_BGR ? GL_RGB8 : GL_RGBA8);

	image2D(GL_PVOID(buffer + sizeof(TGAHeader) + header->idlength),
		header->width, header->height, internalFormat, format,
		GL_UNSIGNED_BYTE, genMIPs);

	delete[] buffer;

	return true;
}

void Texture::bind(GLint unit, bool compareToRef) const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(m_target, m_handle);

	glTexParameteri(m_target, GL_TEXTURE_COMPARE_MODE,
		compareToRef ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
}
