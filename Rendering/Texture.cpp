#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include <iostream>
// Load texture from paths.
Texture::Texture(const std::string& path)
{
	m_TextureId = 0;
	m_LocalBuffer = nullptr;
	m_Width = 0;
	m_Height = 0;
	m_BPP = 0;

	stbi_set_flip_vertically_on_load(1);

	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	glGenTextures(1, &m_TextureId);
	glBindTexture(GL_TEXTURE_2D, m_TextureId);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1.0f);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);
	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);
	else
		std::cout << "Error: failed to load texture: " << path << std::endl;
}

Texture::Texture(glm::vec2 size, GLenum format, GLenum target)
{
	m_Format = format;
	m_Target = target;

	m_Height = size.y;
	m_Width  = size.x;

	glGenTextures(1, &m_TextureId);
	glBindTexture(m_Target, m_TextureId);
	glTexImage2D(m_Target, 0, format, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_TextureId);
}

void Texture::AttachToFramebuffer(GLenum attachment)
{
	// TODO(antopilo): Add new texture types.
	switch (m_Target) 
	{
		case GL_TEXTURE_2D:
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, m_Target, m_TextureId, 0);
			
			break;
	}
}

void Texture::Resize(glm::vec2 size)
{
	// Delete texture
	glDeleteTextures(1, &m_TextureId);

	// Recreate it with new size.
	glGenTextures(1, &m_TextureId);
	glBindTexture(m_Target, m_TextureId);
	glTexImage2D(m_Target, 0, m_Format, size.x, size.y, 0, m_Format, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(m_Format, m_TextureId);
}

void Texture::Unbind() const
{
	glBindTexture(m_Format, 0);
}