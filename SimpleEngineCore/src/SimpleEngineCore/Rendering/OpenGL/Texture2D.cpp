#include "Texture2D.h"

#include "SimpleEngineCore/Log.h"

#include <algorithm>
#include <cmath>
#include <glad/glad.h>

namespace SimpleEngine {
	Texture2D::Texture2D(const std::string& fileLocation, const int w, const int h)
		: m_width(w)
		, m_height(h)
	{
		unsigned char* texData = stbi_load(
			fileLocation.c_str(), &m_width, &m_height, &nrChannels, 0);
		if (!texData)
		{
			LOG_ERROR("Failed to find: {0}", fileLocation);
			return;
		}

		const GLsizei mip_levels = static_cast<GLsizei>(floor(log2(std::max(m_width, m_height)))) + 1;

		// TODO update to use texture units load 2 texture 
		// old way using only one texture in frag shader
		//{
		//	// create unique texture object 
		//	glGenTextures(
		//		1, // number of ids
		//		&m_id // pointer to array where generated texture is stored althoug its just an id
		//	);
		//	// Binding a texture essentially tells OpenGL: 
		//	// Use this texture for the following commandss
		//	// (tells that texture is 2 dimensional, id)
		//	glBindTexture(GL_TEXTURE_2D, m_id);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//	// load texture data to the gpu
		//	glTexImage2D(
		//		GL_TEXTURE_2D, // target texture
		//		0,  // mip map level
		//		GL_RGBA,  // specify number of color components 
		//		m_width, m_height, // size of texture in pixels
		//		0,  // width of border 
		//		GL_RGBA, // format of pixel data usually match with another color format above
		//		GL_UNSIGNED_BYTE, // type of bytes in data 
		//		texData
		//	);
		//	glGenerateMipmap(GL_TEXTURE_2D);
		//	glBindTexture(GL_TEXTURE_2D, 0);
		//}

		// TODO load 2 texture 
		{
			// we create and bind text object (new since opengl 4.5)
			// (target could be cube, how many handles, handle)
			glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
			// set few parameters
			glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);     // how to handle if we out of our texture S-Y
			glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);     // how to handle if we out of our texture S-Y
			glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // how to handle if we too close to texture
			glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // how to handle if we too close too far and more than 1 pixels should be in texture
			// allocate memory on gpu
			glTextureStorage2D(
				m_id, // handle
				mip_levels, // mimpaps
				GL_RGBA8,  // internal data format
				m_width,  // sizek
				m_height
			);
			// load texture into gpu from cpu
			glTextureSubImage2D(
				m_id, // hanlde
				0, // mipmaps
				0, // offset in memory in x
				0, // in y
				m_width, m_height, // size
				GL_RGBA, // internal data format
				GL_UNSIGNED_BYTE, // data format 
				texData // data
			);
			// automatically generate mip map levels 
			glGenerateTextureMipmap(m_id);
		}

		stbi_image_free(texData);
	}

	Texture2D::~Texture2D()
	{
		// clear memory (how many, handle)
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
		// is newer function introduced in OpenGL 4.5
		// that binds a texture to a specific texture unit directly in one step, 
		// without needing to first set the active texture unit.
		glBindTextureUnit(unit, m_id);
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, m_id);
	}
}

