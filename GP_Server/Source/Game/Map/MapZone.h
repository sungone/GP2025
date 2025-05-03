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

private:
	std::unordered_map<ZoneType, NavMesh> _navMeshs;
};

