#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int sizeX, int sizeY, bool hasRenderBuffer)
{
	m_Size = glm::vec2(sizeX, sizeY);
	
	glGenFramebuffers(1, &m_FramebufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FramebufferId);


	m_hasRenderBuffer = hasRenderBuffer;

	if (hasRenderBuffer)
	{
		glGenRenderbuffers(1, &m_RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, sizeX, sizeY);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferId);
	}
	else
	{
		m_RenderBufferId = -1;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void FrameBuffer::AttachTexture(Texture* texture, GLenum attachment)
{
	m_Textures[attachment] = texture;
	texture->AttachToFramebuffer(attachment);
}

// Gets a texture from the attachment.
Texture* FrameBuffer::GetTexture(GLenum attachment)
{
	// Check if texture exists
	if (m_Textures.find(attachment) != m_Textures.end())
	{
		return m_Textures[attachment];
	}
	else
	{
		return nullptr;
	}
}

void FrameBuffer::Resize(glm::vec2 size)
{
	m_Size = size;

	glDeleteFramebuffers(1, &m_FramebufferId);
	glDeleteRenderbuffers(1, &m_RenderBufferId);

	glGenFramebuffers(1, &m_FramebufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FramebufferId);

	for (auto& texture : m_Textures) // resize attached textures.
	{
		texture.second->Resize(size);
		texture.second->AttachToFramebuffer(texture.first);
	}

	if (m_hasRenderBuffer) // Recreate render buffer
	{
		glGenRenderbuffers(1, &m_RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Size.x, m_Size.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferId);
	}
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}


void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FramebufferId);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, m_Size.x, m_Size.y);
}

void FrameBuffer::Unbind()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void FrameBuffer::SetDrawBuffer(GLenum draw)
{
	Bind();
	glReadBuffer(draw);
	Unbind();
}

void FrameBuffer::SetReadBuffer(GLenum read)
{
	Bind();
	glReadBuffer(read);
	Unbind();
}