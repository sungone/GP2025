#pragma once
#include "Character.h"
#include "Player.h"
#include "NavMesh.h"

const float detectDist = 1000.f;

class Monster : public Character
{
public:
	Monster() { Init(); }
	void Init() override;
	void UpdateViewList(std::shared_ptr<Character> other) override;
	void Update();
	void BehaviorTree();

private:
	void Look();
	void Attack();
	void Chase();
	void Patrol();

	bool SetTarget();
	bool IsTargetInAttackRange();
	bool IsTargetInChaseRange();
	virtual float GetAttackDamage() override
	{
		return _info.GetAttackDamage(RandomUtils::GetRandomFloat(0.0f, 1.0f));
	}
	void ChangeState(ECharacterStateType newState) override
	{
		_info.State = newState;
	}
private:
	FVector& _pos = _info.Pos;
	std::shared_ptr<Player> _target = nullptr;
};

