#include "pch.h"
#include "MapZone.h"

bool MapZone::Init()
{
	_default = NavMesh("NavMeshData.json");
	_playground = NavMesh("PlaygroundNavData.json");
	return true;
}

FVector MapZone::GetRandomPos(ZoneType type) const
{
	switch (type)
	{
	case ZoneType::DEFAULT:
		return _default.GetRandomPosition();
	case ZoneType::PLAYGROUND:
		return _playground.GetRandomPosition();
	}
}