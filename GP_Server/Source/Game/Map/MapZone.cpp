#include "pch.h"
#include "MapZone.h"

bool Map::Init()
{

	_navMeshs[ZoneType::PLAYGROUND] = NavMesh(MapDataPath + "NavMeshData_Playground.json");
	_navMeshs[ZoneType::TUK] = NavMesh(MapDataPath + "NavMeshData_TUK.json");
	_navMeshs[ZoneType::E] = NavMesh(MapDataPath + "NavMeshData_E.json");
	_navMeshs[ZoneType::INDUSTY] = NavMesh(MapDataPath + "NavMeshData_Industry.json");
	_navMeshs[ZoneType::GYM] = NavMesh(MapDataPath + "NavMeshData_Gym.json");
	
	for (auto& [zone, navMesh] : _navMeshs)
	{
		if (!navMesh.IsLoaded())
		{
			LOG(Warning, std::format("NavMesh for zone [{}] failed to load.", static_cast<int>(zone)));
			return false;
		}
	}

	return true;
}

FVector Map::GetRandomPos(ZoneType type, float collisionRadius) const
{
	auto it = _navMeshs.find(type);
	if (it != _navMeshs.end())
		return it->second.GetRandomPositionWithRadius(collisionRadius);

	return FVector(0, 0, 0);
}

NavMesh& Map::GetNavMesh(ZoneType type)
{
	auto it = _navMeshs.find(type);
	if (it == _navMeshs.end())
		LOG(Warning, "type Invaild");

	return it->second;
}