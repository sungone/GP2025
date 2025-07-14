#pragma once
#include "Common.h"

class Character
{
public:
	Character(int32 id) { _id = id; }

	virtual void Init();
	virtual void Update() {};
	virtual bool IsDead();

	virtual void OnDamaged(float damage);
	virtual float GetAttackDamage() = 0;
	void UpdatePos(const FVector& newPos);
	bool IsCollision(const FVector& pos, const float dist);
	bool IsCollision(const FVector& pos);
	bool IsCollision(const FInfoData& target);
	bool IsInAttackRange(const FInfoData& target);
	bool IsInViewDistance(const FVector& targetPos, float viewDist);
	bool IsInFieldOfView(const FInfoData& target);
	bool HasLineOfSight(const FVector& targetPos, const std::vector<FVector>& obstacles);
	bool IsInRectangularAttackRange(const FInfoData& target, float width);

	virtual void UpdateViewList(std::shared_ptr<Character> other) {}
	bool AddToViewList(int32 CharacterId);
	bool RemoveFromViewList(int32 CharacterId);
	std::unordered_set<int32> GetViewList() const{ return _viewList; }

	virtual void ChangeState(ECharacterStateType newState) = 0;
	FInfoData& GetInfo() { return _info; }
	FVector GetPos() const { return _info.Pos; }
	ZoneType GetZone() { return _info.GetZone(); }
	void SetInfo(const FInfoData& info) { _info = info; }
	void SetPos(const FVector& pos) { _info.Pos = pos; }
	bool IsMonster() const {
		return _info.ID >= MAX_PLAYER;
	}
	virtual bool IsValid();
	void Restore();

public:
	std::mutex _cLock;
	std::mutex _vlLock;
protected:
	FInfoData _info;
	int32& _id = _info.ID;
	std::unordered_set<int32> _viewList;
};

