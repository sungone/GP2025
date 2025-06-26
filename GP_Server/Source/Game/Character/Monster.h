#pragma once
#include "Character.h"
#include "Player.h"
#include "NavMesh.h"

const float detectDist = 1000.f;

enum class EAttackPattern
{
	EarthQuake,
	FlameBreath,
	MeleeAttack
};

class Monster : public Character, public std::enable_shared_from_this<Monster>
{
public:
	Monster(int32 id) : Character(id) { Init(); }
	Monster(int32 id, ZoneType zone, Type::EMonster monType)
		: Character(id), _monType(monType)
	{
		_zone = zone;
	}
	void Init() override;
	void UpdateViewList(std::shared_ptr<Character> other) override;
	void Update();
	void BehaviorTree();

	Type::EMonster GetMonsterType() { return _monType; }
	float GetAttackDamage() override { return _info.GetDamage(); }
	void ChangeState(ECharacterStateType newState) override
	{
		_info.State = newState;
	}

	void SetDropItem(int32 itemId) { _dropItemId = itemId; }
	uint32 GetDropItemId() { return _dropItemId; }
	bool HasDropItem() { return (_dropItemId != -1); }

	bool IsBoss() { return _isBoss; }
	void SetBoss(bool value) { _isBoss = value; }
	bool IsActive() { return _active; }
	void SetActive(bool value) { _active = value; }

	void SetQuestID(QuestType quest) { _questID = quest; }
	QuestType GetQuestID() { return _questID; }
private:
	void Look();
	void Attack();

	void TinoAttack();
	void PerformEarthQuake();
	void PerformFlameBreath();
	void PerformMeleeAttack();
	void SetNextPattern();

	void Chase();
	void Patrol();

	bool SetTarget();
	bool IsTargetInAttackRange();
	bool IsTargetInChaseRange();
private:
	ZoneType& _zone = _info.CurrentZone;
	Type::EMonster _monType;
	FVector& _pos = _info.Pos;
	std::shared_ptr<Player> _target = nullptr;
	int32 _dropItemId = -1;
	QuestType _questID = QuestType::NONE;
	bool _active = false;
	bool _isBoss;
	EAttackPattern _currentPattern;
};

