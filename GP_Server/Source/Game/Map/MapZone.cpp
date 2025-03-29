#include "pch.h"
#include "MapZone.h"
const std::string MapDataPath = "Source/Game/Map/MapJsonData/";

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