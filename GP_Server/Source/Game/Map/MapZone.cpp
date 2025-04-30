#include "pch.h"
#include "MapZone.h"

bool Map::Init()
{
	_zoneNavMeshes[ZoneType::PLAYGROUND] = NavMesh(MapDataPath + "PlaygroundNavData.json");
	_zoneNavMeshes[ZoneType::TUK] = NavMesh(MapDataPath + "NavMeshData_TUK.json");
	_zoneNavMeshes[ZoneType::E] = NavMesh(MapDataPath + "NavMeshData_E.json");
	_zoneNavMeshes[ZoneType::INDUSTY] = NavMesh(MapDataPath + "NavMeshData_Industry.json");
	_zoneNavMeshes[ZoneType::GYM] = NavMesh(MapDataPath + "NavMeshData_Gym.json");
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