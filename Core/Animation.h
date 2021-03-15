#pragma once

#include <vector>;
#include <string>;
#include <map>;

#include <assert.h>
#define ASSERT(x) if (!(x)) assert(false)

struct KeyFrame
{
	float time;
	float value;

};


class LinkedListNode
{
private:
	LinkedListNode* m_Next = nullptr;
	LinkedListNode* m_Previous = nullptr;

public:
	KeyFrame* Value;

	LinkedListNode(KeyFrame* value) { Value = value; }
	~LinkedListNode() { delete Value; }

	LinkedListNode* Next() 
	{ 
		
		return m_Next; 
	}
	LinkedListNode* Previous() 
	{ 
		
		return m_Previous; 
	}

	void SetNext(LinkedListNode* node) { m_Next = node; }
	void SetPrevious(LinkedListNode* node) { m_Previous = node; }
};

class LinkedList
{
private:
	int m_Count = 0;
	LinkedListNode* m_Start;
	LinkedListNode* m_End;

public:
	int Count() { return m_Count; }
	LinkedListNode* Start() { return m_Start; }
	LinkedListNode* End() { return m_End; }

	void AddNode(int position, LinkedListNode* node)
	{
		if (position < 0 || position > m_Count)
			return;

		if (m_Count == 0)
		{
			m_Start = node;
			m_End = node;
			m_Count++;
			return;
		}


		LinkedListNode* current = m_Start;

		if (position == 0) {
			current->SetPrevious(node);
			node->SetNext(current);
			m_Start = node;
		}
		else if (position == m_Count) {
			m_End->SetNext(node);
			node->SetPrevious(m_End);
			m_End = node;
		}
		else {
			for (int i = 0; i < position; i++)
			{
				current = current->Next();
			}

			LinkedListNode* newNext = current;
			current->SetNext(node);
			newNext->SetPrevious(node);
		}

		m_Count++;
	}

	void RemoveNode(int position)
	{
		if (position < 0 || position > m_Count)
			return;

		LinkedListNode* current = m_Start;
	
		if (position == 0)
		{
			if (m_Count == 1)
			{
				delete current;
			}
			else
			{
				LinkedListNode* next = current->Next();
				next->SetPrevious(nullptr);
				m_Start = next;
				delete current;
			}
			
			
		}
		else if (position == m_Count - 1)
		{
			if (m_Count == 1)
			{
				m_Start = nullptr;
				m_End = nullptr;

			}
			else
			{
				current = m_End;
				LinkedListNode* prev = current->Previous();
				m_End = prev;
				delete current;
			}
		}
		else
		{
			for (int i = 0; i < position; i++)
			{
				current = current->Next();
			}

			LinkedListNode* newNext = current->Next();
			if (position == 0) {
				this->m_Start = newNext;
			}
			LinkedListNode* newPrev = current->Previous();
			newPrev->SetNext(newNext);
			newNext->SetPrevious(newPrev);
			delete current;
		}
		m_Count--;
	}

	~LinkedList() 
	{
		for (int i = 0; i < m_Count; i++)
		{
			RemoveNode(i);
		}
	}
};


class Animation
{
private:
	std::map<std::string, LinkedList*> m_Timeline;

public:
	void AddKeyframe(const std::string uniformName, float time, float value)
	{
		KeyFrame* kf = new KeyFrame
		{
			time, value
		};

		// Order by time.
		if (m_Timeline.find(uniformName) != m_Timeline.end())
		{
			LinkedList* ll = m_Timeline[uniformName];
			
			LinkedListNode* node = new LinkedListNode(kf);

			// Check if new keyframe is before the first node.
			if (ll->Start()->Value->time > time)
			{
				ll->AddNode(0, node);
			}
			else if (ll->End()->Value->time < time) // same check with end.
			{
				ll->AddNode(ll->Count(), node);
			}
			else // if keyframe is in the middle/
			{
				LinkedListNode* current = ll->Start();
				for (int i = 0; i < ll->Count(); i++)
				{
					if (current->Value->time < time)
					{
						current = current->Next();
						continue;
					}

					ll->AddNode(i, node);
					return;
				}
			}
		}
		else
		{
			m_Timeline[uniformName] = new LinkedList();
			m_Timeline[uniformName]->AddNode(0, new LinkedListNode(kf));
		}
	}

	void RemoveKeyframe(std::string uniformName, int id)
	{
		if (m_Timeline.find(uniformName) != m_Timeline.end()) 
		{
			auto ll = m_Timeline[uniformName];
			if (ll->Count() > id)
				ll->RemoveNode(id);
		}
	}

	std::vector<std::string> GetUniforms()
	{
		std::vector < std::string> result;
		for (std::map<std::string, LinkedList*>::const_iterator it = m_Timeline.begin(); it != m_Timeline.end(); it++)
		{
			result.push_back(it->first);
		}

		return result;
	}

	std::vector<KeyFrame*> GetUniformKeyframes(std::string uniform)
	{
		std::vector<KeyFrame*> result;

		// contains uniform.
		if (m_Timeline.find(uniform) != m_Timeline.end())
		{
			LinkedList* list = m_Timeline[uniform];

			if (list->Count() > 0)
			{
				LinkedListNode* current = list->Start();
				for (int i = 0; i < list->Count(); i++)
				{
					result.push_back(current->Value);
					if (list->Count() > 1)
						current = current->Next();
				}
			}
			
		}

		return result;
	}

	float GetUniformValue(std::string u, float t)
	{
		// No uniform in the timeline.
		if (m_Timeline.find(u) == m_Timeline.end())
			return 0.0f;

		LinkedList* l = m_Timeline[u];
		if (l->Count() == 0)
			return 0.0f;
		if (l->Count() == 1)
			return l->Start()->Value->value;

		if (l->Start()->Value->time >= t)
			return l->Start()->Value->value;
		if (l->End()->Value->time <= t)
			return l->End()->Value->value;

		// Next node until time of node is > than T.
		// Take % of T between previous and found node.
		// Depending on the interpolation return lerp(prevValue, next, %);
		auto current = l->Start();
		for (int i = 0; i < l->Count(); i++)
		{
			if (current->Value->time < t && current->Next() != nullptr)
				current = current->Next();
			else
			{
				float start = current->Previous()->Value->value;
				float end = current->Value->value;

				float startT = current->Previous()->Value->time;
				float endT = current->Value->time;

				// calculate %
				float scaled = t - startT;
				float max = endT - startT;
				float perc = scaled / max;

				return lerp(start, end, perc);
			}
		}
	}

	float lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}

	bool RenameUniform(std::string s1, std::string s2)
	{
		// Check if key already exists
		if (m_Timeline.find(s2) != m_Timeline.end())
			return false;

		m_Timeline[s2] = m_Timeline[s1];

		m_Timeline.erase(s1);
	}
};

