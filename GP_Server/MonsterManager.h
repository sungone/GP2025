#pragma once
#include <unordered_map>
#include <memory>
#include "Monster.h"

class MonsterManager
{
public :
	
	// 몬스터 생성
	void SpawnMonster(int32 ID, ECharacterType MonsterType, float X, float Y, float Z, float Yaw);

	// 몬스터 제거
	void RemoveMonster(int32 ID);

	// 몬스터 조회
	Monster* GetMonsterByID(int32 ID);

	// 전체 몬스터 리스트 반환
	const std::unordered_map<int32, Monster>& GetAllMonsters() const;

private:
	std::unordered_map<int32, Monster> Monsters;
};

