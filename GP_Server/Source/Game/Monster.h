#pragma once
#include "Character.h"
#include "Player.h"
#include "NavMesh.h"

class Monster : public Character
{
public:
	Monster() { Init(); }
	void Init() override;
	void UpdateViewList(std::shared_ptr<Character> other) override;
	void Update();
	void BehaviorTree();
	void SetTarget(std::shared_ptr<Player> player) { _target = player; }
private:
	void Attack();
	void Move();
	bool IsTargetDetected();
	bool IsTargetInRange();
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

