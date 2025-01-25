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
		info.State = MonsterState::M_STATE_IDLE;
	}

    void UpdateBehavior();

	void ChangeState(MonsterState newState);

    uint32_t GetState() const { return info.State; }

public :
    bool ShouldStartWalking();
    bool ShouldAttack();
}; 

