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
		info.Hp = 100.f;
		info.Damage = 20.f;
		info.Speed = 200.f;
		info.State = ECharacterStateType::STATE_IDLE;
	}

    void UpdateBehavior();

	void ChangeState(ECharacterStateType newState);

    uint32_t GetState() const { return info.State; }

public :
    bool ShouldStartWalking();
    bool ShouldAttack();
}; 

