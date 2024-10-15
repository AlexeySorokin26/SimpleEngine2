#include "Texture2D.h"

#include <algorithm>
#include <cmath>
#include <glad/glad.h>

namespace SimpleEngine {
	Texture2D::Texture2D(const unsigned char* data, const unsigned int w, const unsigned int h)
		: m_width(w)
		, m_height(h)
	{
		const GLsizei mip_levels = static_cast<GLsizei>(log2(std::max(m_width, m_height))) + 1;
		// (target could be cube, how many handles, handle)
		glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
		// allocate memory on gpu
		// (handle, how many mipmaps, internval fomat, size)
		glTextureStorage2D(m_id, mip_levels, GL_RGB8, m_width, m_height);
		// load texture into gpu from cpu
		// (handle, level mip map, offset in memory to fill whatever part of image, w, h, data format, data type, pointer to data)
		glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, data);
		// set few parameters
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);     // how to handle if we out of our texture S-Y
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);     // how to handle if we out of our texture S-Y
		glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // how to handle if we too close to texture
		glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // how to handle if we too close too far and more than 1 pixels should be in texture
		// automatically generate mip map levels 
		glGenerateTextureMipmap(m_id);
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &m_id);
	}

	Texture2D& Texture2D::operator=(Texture2D&& texture) noexcept
	{
		glDeleteTextures(1, &m_id);
		m_id = texture.m_id;
		m_width = texture.m_width;
		m_height = texture.m_height;
		texture.m_id = 0;
		return *this;
	}

	Texture2D::Texture2D(Texture2D&& texture) noexcept
	{
		m_id = texture.m_id;
		m_width = texture.m_width;
		m_height = texture.m_height;
		texture.m_id = 0;
	}

	void Texture2D::bind(const unsigned int unit) const
	{
		// use texture
		// (place where to bind texture, handle)
		glBindTextureUnit(unit, m_id);
	}
}

