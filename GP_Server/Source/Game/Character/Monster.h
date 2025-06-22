#pragma once
#include "Character.h"
#include "Player.h"
#include "NavMesh.h"

const float detectDist = 1000.f;

class Monster : public Character, public std::enable_shared_from_this<Monster>
{
public:
	Monster(int32 id) : Character(id) { Init(); }
	Monster(int32 id, ZoneType zone, Type::EMonster monType)
		: Character(id), _zone(zone), _monType(monType)
	{
	}
	void Init() override;
	void UpdateViewList(std::shared_ptr<Character> other) override;
	void Update();
	void BehaviorTree();
	void SetDropItem(uint32 itemId) { _dropItemId = itemId; }
	uint32 GetDropItemId() { return _dropItemId; }
	bool HasDropItem() { return (_dropItemId != 0); }
	Type::EMonster GetMonsterType() { return _monType; }
	float GetAttackDamage() override { return _info.GetDamage(); }
	void ChangeState(ECharacterStateType newState) override
	{
		_info.State = newState;
	}

private:
	void Look();
	void Attack();
	void Chase();
	void Patrol();

	bool SetTarget();
	bool IsTargetInAttackRange();
	bool IsTargetInChaseRange();

private:
	ZoneType _zone;
	Type::EMonster _monType;
	FVector& _pos = _info.Pos;
	std::shared_ptr<Player> _target = nullptr;
	uint32 _dropItemId = 0;
};

