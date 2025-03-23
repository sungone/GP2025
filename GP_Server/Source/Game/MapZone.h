#pragma once
#include "NavMesh.h"

enum class ZoneType
{
	PLAYGROUND,
};

class MapZone
{
public:
	static MapZone& GetInst()
	{
		static MapZone inst;
		return inst;
	}

	bool Init();
	FVector GetRandomPos(ZoneType type) const;
public:
	NavMesh _playground;
};

