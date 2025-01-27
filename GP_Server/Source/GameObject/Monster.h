#pragma once
#include "Character.h"

class Monster : public Character
{
public:
	Monster() { Init(); }
    void Init() override;

    void Update();
	void ChangeState(ECharacterStateType newState);

    uint32_t GetState() const { return info.State; }
    void SetState(ECharacterStateType newState) { info.State = newState; }

public :
    bool ShouldStartWalking();
    bool ShouldAttack();
}; 

