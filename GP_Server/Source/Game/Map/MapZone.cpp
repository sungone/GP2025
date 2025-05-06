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

FVector Map::GetSpawnPosition(ZoneType from, ZoneType to) const
{
	if (to == ZoneType::TUK && from == ZoneType::TIP)
		return FVector(-5270.0, 15050.0, 147);
	else if (to == ZoneType::TUK && from == ZoneType::E)
		return FVector(-4420.0, -12730.0, 837);
	else if (to == ZoneType::TUK && from == ZoneType::GYM)
		return FVector(-4180.0, 5220.0, 147);
	else if (to == ZoneType::TUK && from == ZoneType::INDUSTY)
		return FVector(8721.06, -19229.73, 146.28);
	else if (to == ZoneType::TIP)
		return FVector(-100, 100, 147);
	else if (to == ZoneType::E)
		return FVector(-150, 1500, 147);
	else if (to == ZoneType::GYM)
		return FVector(-2000, 0, 147);
	else if (to == ZoneType::INDUSTY)
		return FVector(10, -7000, 180);

	return FVector();
}

bool Map::IsZoneAccessible(ZoneType zone, uint32 playerLevel) const
{
	switch (zone)
	{
	case ZoneType::TIP:       return playerLevel >= 1;
	case ZoneType::TUK:       return playerLevel >= 1;
	case ZoneType::E:         return playerLevel >= 4;
	case ZoneType::INDUSTY:   return playerLevel >= 7;
	case ZoneType::GYM:       return playerLevel >= 10;
	default:                  return false;
	}
}