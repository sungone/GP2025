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

    void Update(float deltaTime)
    {
        stateTimer += deltaTime;
        if (stateTimer >= stateInterval)
        {
            stateTimer = 0.0f;
            UpdateBehavior();
        }
    }

public :
    bool ShouldStartWalking() { return rand() % 3 == 0; }
    bool ShouldAttack() { return rand() % 5 == 0; }

public:
    float stateTimer;     
    float stateInterval;  
}; 

