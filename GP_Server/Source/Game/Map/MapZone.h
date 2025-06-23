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
	FVector GetRandomSpawnPosition(ZoneType from, ZoneType to) const;
	bool IsZoneAccessible(ZoneType zone, uint32 playerLevel) const;
	
	FVector GetStartPos(ZoneType startZone);
private:
	std::unordered_map<ZoneType, NavMesh> _navMeshs;
};

