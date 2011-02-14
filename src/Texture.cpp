#include "Texture.h"
#include "png/png.h"
#include "png/pngstruct.h"
#include "png/pnginfo.h"
#include "png/pngpriv.h"

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

static void PNGError(png_structp png_ptr, png_const_charp msg)
{
	LOG_ERROR("Failed to read PNG: %s\n", msg);
	longjmp(png_ptr->png_jmpbuf, 1);
}

// libpng custom reader
static void PNGAPI PNGRead(png_structp png_ptr, png_bytep data, png_size_t length)
{
	ASSERT(png_ptr->io_ptr);
	memcpy(data, png_ptr->io_ptr, length);
	png_ptr->io_ptr = (uint8_t *)png_ptr->io_ptr + length;

	/*
	png_size_t check;
	check = (png_size_t)reader->read((void*)data, length);
	if (check != length)
		png_error(png_ptr, "Read Error");
	*/
}

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

	if (m_target == GL_TEXTURE_2D_MULTISAMPLE)
		return;

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

GLuint Texture::getHandle() const
{
	return m_handle;
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

void Texture::image2DMultisample(GLsizei width, GLsizei height,
	GLint internalFormat, GLsizei samples, GLboolean fixedSampleLocations)
{
	ASSERT(m_handle);

	glBindTexture(m_target, m_handle);

	glTexImage2DMultisample(m_target, samples, internalFormat,
		width, height, fixedSampleLocations);
}

void Texture::bind(GLint unit, bool compareToRef) const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(m_target, m_handle);

	if (m_target == GL_TEXTURE_2D_MULTISAMPLE)
		return;

	glTexParameteri(m_target, GL_TEXTURE_COMPARE_MODE,
		compareToRef ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
}

bool Texture::load2DTGA(const char *name, bool genMIPs)
{
	ASSERT(name);

	uint8_t   *buffer;
	uint32_t  size;
	GLint     internalFormat;
	GLenum    format;

	if (!VFS::load(name, VFS_BINARY, &buffer, &size))
		return false;

	if (size <= sizeof(TGAHeader))
	{
		LOG_ERROR("Too small TGA file '%s'\n", name);
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

	if (header->bitperpel == 24)
	{
		format         = GL_BGR;
		internalFormat = GL_RGB;
	} else
	{
		format         = GL_BGRA;
		internalFormat = GL_RGBA;
	}

	image2D(GL_PVOID(buffer + sizeof(TGAHeader) + header->idlength),
		header->width, header->height, internalFormat, format,
		GL_UNSIGNED_BYTE, genMIPs);

	delete[] buffer;

	return true;
}

bool Texture::load2DPNG(const char *name, bool genMIPs)
{
	ASSERT(name);

	png_structp png_ptr;
	png_infop   info_ptr;
	png_uint_32 width, height;
	int32_t     bit_depth, color_type, stride;
	uint8_t     *buffer, *data;
	png_bytep   *row_pointers;
	uint32_t    size;
	GLint       internalFormat;
	GLenum      format;

	if (!VFS::load(name, VFS_BINARY, &buffer, &size))
		return false;

	if (size <= 8)
	{
		LOG_ERROR("Too small PNG file '%s'\n", name);
		delete[] buffer;
		return false;
	}

	if (png_sig_cmp((png_byte *)buffer, 0, 8))
	{
		LOG_ERROR("Wrong PNG format '%s'\n", name);
		delete[] buffer;
		return false;
	}

	if ((png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		NULL, PNGError, NULL)) == NULL)
	{
		LOG_ERROR("Failed to create PNG read struct\n");
		delete[] buffer;
		return false;
	}

	if ((info_ptr = png_create_info_struct(png_ptr)) == NULL)
	{
		LOG_ERROR("Failed to create PNG info struct\n");
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		delete[] buffer;
		return false;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		//LOG_ERROR("Failed to set PNG jmpbuf\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		delete[] buffer;
		return false;
	}

	png_set_read_fn(png_ptr, (void *)(buffer + 8), PNGRead);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		NULL, NULL, NULL);

	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png_ptr);

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	//if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	//	png_set_gray_to_rgb(png_ptr);

	png_read_update_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		NULL, NULL, NULL);

	switch (color_type)
	{
		case PNG_COLOR_TYPE_GRAY:
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			internalFormat = GL_RED;
			format         = GL_RED;
		break;
		case PNG_COLOR_TYPE_RGB:
			internalFormat = GL_RGB;
			format         = GL_RGB;
		break;
		case PNG_COLOR_TYPE_RGBA:
			internalFormat = GL_RGBA;
			format         = GL_RGBA;
		break;
		default:
			LOG_ERROR("Unknown PNG color type %d in '%s'\n", color_type, name);
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			delete[] buffer;
			return false;
	}

	stride = png_get_rowbytes(png_ptr, info_ptr);

	row_pointers = new png_bytep[height];
	ASSERT(row_pointers);

	data = new uint8_t[height * stride];
	ASSERT(data);

	for (uint32_t i = 0; i < height; ++i)
		row_pointers[i] = (png_bytep)data + i * stride;

	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	delete[] row_pointers;
	delete[] buffer;

	image2D(GL_PVOID(data), width, height, internalFormat, format,
		GL_UNSIGNED_BYTE, genMIPs);

	delete[] data;

	return true;
}
