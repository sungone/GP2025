#include "pch.h"
#include "MapZone.h"

bool MapZone::Init()
{
	_default = NavMesh(MapDataPath + "NavMeshData.json");
	_playground = NavMesh(MapDataPath + "PlaygroundNavData.json");
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

NavMesh& MapZone::GetNavMesh(ZoneType type)
{
	switch (type)
	{
	case ZoneType::DEFAULT:
		return _default;
	case ZoneType::PLAYGROUND:
		return _playground;
	}
}
