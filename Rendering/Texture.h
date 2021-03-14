#pragma once
#include "GL/glew.h"
#include <string>
#include <glm/ext/vector_float2.hpp>
class Texture 
{
private:
	unsigned int m_TextureId;
	unsigned char* m_LocalBuffer;

	GLenum m_Format;
	GLenum m_Type;
	GLenum m_Target;

	int m_Width;
	int m_Height;
	int m_BPP;

public:
	Texture(const std::string& path);
	Texture(glm::vec2 size, GLenum format, GLenum target);

	~Texture();

	void Resize(glm::vec2 size);
	void Resize(int x, int y);

	void AttachToFramebuffer(GLenum attachment);

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	unsigned int GetId() const { return m_TextureId; }

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }

};