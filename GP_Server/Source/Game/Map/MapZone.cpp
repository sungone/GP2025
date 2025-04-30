#include "pch.h"
#include "MapZone.h"

bool Map::Init()
{
	_zoneNavMeshes[ZoneType::DEFAULT] = NavMesh(MapDataPath + "NavMeshData.json");
	_zoneNavMeshes[ZoneType::PLAYGROUND] = NavMesh(MapDataPath + "PlaygroundNavData.json");
	return true;
}

FVector Map::GetRandomPos(ZoneType type) const
{
	auto it = _zoneNavMeshes.find(type);
	if (it != _zoneNavMeshes.end())
		return it->second.GetRandomPosition();

	return FVector(0, 0, 0);
}

NavMesh& Map::GetNavMesh(ZoneType type)
{
	auto it = _zoneNavMeshes.find(type);
	if (it == _zoneNavMeshes.end())
		LOG(Warning, "type Invaild");

	return it->second;
}