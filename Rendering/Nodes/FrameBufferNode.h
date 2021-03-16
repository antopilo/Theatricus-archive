#pragma once
#include <string>
#include "../FrameBuffer.h"
#include "../Shader.h"

class FrameBufferNode
{
private:
	int nodeId = 0;
	std::string m_Name = "";
	FrameBuffer* m_FrameBuffer;

	// Input
	Shader* m_Shader;
public:

	FrameBufferNode(FrameBuffer* fb, int id);
	~FrameBufferNode();

	// Input 
	void SetShader(Shader* shader);
	Shader* GetShader() const;

	// Output
	// TODO: Find dynamic way to handle multiple outputs.

	void SetTexture(Texture* texture);
	Texture* GetTexture() const;
};