#pragma once
#include "GameObject.h"

class Monster : public GameObject
{
public:
	Monster() { Init(); }
	void Init() override
	{
		GameObject::Init();
		static int i = 0;
		info.ID = i++;
		info.CharacterType = M_MOUSE;
	}
};

