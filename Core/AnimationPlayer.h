#pragma once
#include "Animation.h"
#include "Timestep.h"
#include "aquila/aquila.h"
#include "aquila/global.h"
#include "aquila/transform/OouraFft.h"


class AnimationPlayer
{
private:
	static AnimationPlayer* m_Instance;

	Aquila::WaveFile*  wav;

	Animation* m_Animation;
	bool m_Playing = false;

public:
	float m_CurrentTime = 0.0f;
	float m_MaxTime = 5.0f;
	static AnimationPlayer* Get() 
	{ 
		if (m_Instance == nullptr) {
			m_Instance = new AnimationPlayer();
		}
		return m_Instance; 
	}

	AnimationPlayer() 
	{ 
		m_Instance = this; 
		m_Animation = new Animation();
		
		wav = new Aquila::WaveFile("Res/BabyElephantWalk60.wav");
	}

	~AnimationPlayer() 
	{ 
		delete m_Animation; 
	}

	Animation* GetAnimation() { return m_Animation; }

	void Update(Timestep ts)
	{
		if (IsPlaying())
			m_CurrentTime += ts;

		int currentSample = (m_CurrentTime * wav->getBytesPerSec()) / wav->getBytesPerSample();
		std::cout << "Current sample: " << currentSample << std::endl;

		auto sample = wav->sample(currentSample);



		if (m_CurrentTime >= m_MaxTime)
			Stop();
	}

	bool IsPlaying() { return m_Playing; }
	void Play() 
	{
		if (m_CurrentTime >= m_MaxTime)
			m_CurrentTime = 0;
		m_Playing = true; 
	}
	void Stop() { m_Playing = false; }

	float GetMaxTime() const { return m_MaxTime; }
	float GetCurrentTime() 
	{ 
		if (m_CurrentTime > m_MaxTime)
			m_CurrentTime = m_MaxTime;

		return m_CurrentTime; 
	}

	void SetCurrentTime(float t)
	{
		if (t < 0)
			m_CurrentTime = 0;
		else if (t > m_MaxTime)
			m_CurrentTime = m_MaxTime;
		else
			m_CurrentTime = t;
	}

	void SetMaxTime(float t)
	{
		if (t <= 0.0f)
			return;

		if (m_CurrentTime > t)
			m_CurrentTime = t;

		m_MaxTime = t;
	}

	float GetUniformvalue(std::string u, float t)
	{
		Animation* anim = GetAnimation();
		return anim->GetUniformValue(u, t);
	}
};