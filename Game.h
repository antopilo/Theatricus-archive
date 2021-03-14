#pragma once
#include "Rendering/FrameBuffer.h"

class Camera;
class Game
{
private:
	FrameBuffer* m_GameFrameBuffer;

	
public:
	Game();

	FrameBuffer* GetFrameBuffer();
    
	void Draw(Camera* camera);
};