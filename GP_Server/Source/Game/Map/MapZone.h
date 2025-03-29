#pragma once
#include "NavMesh.h"

enum class ZoneType
{
	DEFAULT,
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
	NavMesh _default;
	NavMesh _playground;
};

