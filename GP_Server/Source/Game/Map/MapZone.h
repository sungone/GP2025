#pragma once
#include "NavMesh.h"

class Map
{
public:
	static Map& GetInst()
	{
		static Map inst;
		return inst;
	}

	bool Init();

	FVector GetRandomPos(ZoneType type, float collisionRadius) const;
	NavMesh& GetNavMesh(ZoneType type);
	FVector GetSpawnPosition(ZoneType from, ZoneType to) const;
	bool IsZoneAccessible(ZoneType zone, uint32 playerLevel) const;
	
	FVector GetStartPos(ZoneType startZone);
	FVector GetBossMonsterSpawnPos(Type::EMonster monster);
private:
	std::unordered_map<ZoneType, NavMesh> _navMeshs;
};

