#pragma once
#include "NavMesh.h"

enum class ZoneType
{
	PLAYGROUND,//test¿ë
	TUK,
	TIP,
	E,
	INDUSTY,
	GYM,
};

class Map
{
public:
	static Map& GetInst()
	{
		static Map inst;
		return inst;
	}

	bool Init();
	FVector GetRandomPos(ZoneType type) const;
	NavMesh& GetNavMesh(ZoneType type);

private:
	std::unordered_map<ZoneType, NavMesh> _zoneNavMeshes;
};

