#pragma once
#include <glm/ext/vector_float2.hpp>
#include <map>
#include <GL/glew.h>
#include "Texture.h"

class FrameBuffer
{
private:
	unsigned int m_FramebufferId;
	unsigned int m_RenderBufferId;

	bool m_hasRenderBuffer;
	glm::vec2 m_Size;
	
	// Attachment textures.
	std::map<GLenum, Texture*> m_Textures;


public:
	FrameBuffer(int sizeX, int sizeY, bool hasRenderBuffer);

	void AttachTexture(Texture* texture, GLenum attachment);
	Texture* GetTexture(GLenum attachment);

	void Bind();
	void Unbind();

	glm::vec2 GetSize() const { return m_Size; }
	void Resize(glm::vec2 size);

	void SetDrawBuffer(GLenum draw);
	void SetReadBuffer(GLenum read);

};