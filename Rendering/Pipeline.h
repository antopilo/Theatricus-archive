#pragma once
#include <vector>
#include "FrameBuffer.h"
#include "Shader.h"

class Pipeline
{
private:
	static Pipeline* m_Instance;
	Pipeline()
	{
		m_Instance = this;
	}

public:

	static Pipeline* Get()
	{
		if (m_Instance == nullptr)
		{
			Pipeline();
		}

		return m_Instance;
	}

	std::vector<FrameBuffer*> m_Frambuffers;
	std::vector<Texture*> m_Textures;
	std::vector<Shader*> m_Shaders;

	void DrawUI();

	

};