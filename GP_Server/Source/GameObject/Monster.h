#pragma once
#include "Character.h"

class Monster : public Character
{
public:
	Monster() { Init(); }
    void Init() override;

    void UpdateBehavior();
	void ChangeState(ECharacterStateType newState);

    uint32_t GetState() const { return info.State; }

public :
    bool ShouldStartWalking();
    bool ShouldAttack();
}; 

