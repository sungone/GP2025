#pragma once
#include <unordered_map>
#include <memory>
#include "Monster.h"

class MonsterManager
{
public :
	
	// ���� ����
	void SpawnMonster(int32 ID, ECharacterType MonsterType, float X, float Y, float Z, float Yaw);

	// ���� ����
	void RemoveMonster(int32 ID);

	// ���� ��ȸ
	Monster* GetMonsterByID(int32 ID);

	// ��ü ���� ����Ʈ ��ȯ
	const std::unordered_map<int32, Monster>& GetAllMonsters() const;

private:
	std::unordered_map<int32, Monster> Monsters;
};

