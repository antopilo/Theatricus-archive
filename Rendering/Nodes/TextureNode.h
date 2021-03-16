#pragma once
#include "../Texture.h"

class TextureNode
{
private:
	int nodeId;
	std::string m_Title = "Texture node";
	Texture* m_Texture;

	int m_Width;
	int m_Height;

public:
	TextureNode();
	~TextureNode();


};