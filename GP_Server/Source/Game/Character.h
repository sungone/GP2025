#pragma once
#include "Common.h"

constexpr float VIEW_DIST = 2000.f;
enum class CharacterType { Player, Monster };
class Character
{
public:
	virtual void Init();
	virtual void Update() {};
	virtual bool IsDead();

	void OnDamaged(float damage);
	virtual float GetAttackDamage() = 0;

	bool IsColision(const FVector& pos);
	bool IsInAttackRange(const FInfoData& target);
	bool IsInViewDistance(const FVector& targetPos, float viewDist);
	bool IsInFieldOfView(const FInfoData& target, float fovAngle = 90.f);
	bool HasLineOfSight(const FVector& targetPos, const std::vector<FVector>& obstacles);

	virtual void UpdateViewList(std::shared_ptr<Character> other) {}
	bool AddToViewList(int32 CharacterId);
	bool RemoveFromViewList(int32 CharacterId);
	const std::unordered_set<int32>& GetViewList() const { return _viewList; }

	virtual void ChangeState(ECharacterStateType newState) = 0;
	FInfoData& GetInfo() { return _info; }
	void SetInfo(FInfoData& info) { _info = info; }
	bool IsMonster() { return _characterType == CharacterType::Monster; }

	void Lock() { _mutex.lock(); }
	void Unlock() { _mutex.unlock(); }
protected:
	std::mutex _mutex;
	FInfoData _info;
	int32& _id = _info.ID;
	std::unordered_set<int32> _viewList;
	CharacterType _characterType;
};

