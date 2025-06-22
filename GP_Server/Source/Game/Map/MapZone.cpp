#include "pch.h"
#include "MapZone.h"
#include "GameWorld.h"

bool Map::Init()
{

	_navMeshs[ZoneType::PLAYGROUND] = NavMesh(MapDataPath + "NavMeshData_Playground.json");
	_navMeshs[ZoneType::TUK] = NavMesh(MapDataPath + "NavMeshData_TUK.json");
	_navMeshs[ZoneType::E] = NavMesh(MapDataPath + "NavMeshData_E.json");
	_navMeshs[ZoneType::INDUSTY] = NavMesh(MapDataPath + "NavMeshData_Industry.json");
	_navMeshs[ZoneType::GYM] = NavMesh(MapDataPath + "NavMeshData_Gym.json");
	_navMeshs[ZoneType::TIP] = NavMesh(MapDataPath + "NavMeshData_TIP.json");
	_navMeshs[ZoneType::BUNKER] = NavMesh(MapDataPath + "NavMeshData_Bunker.json");

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

	return FVector::ZeroVector;
}

NavMesh& Map::GetNavMesh(ZoneType type)
{
	auto it = _navMeshs.find(type);
	if (it == _navMeshs.end())
		LOG(Warning, "type Invaild");

	return it->second;
}

FVector Map::GetRandomSpawnPosition(ZoneType from, ZoneType to) const
{
	auto RandomOffset = [](float radius) {
		float offsetX = RandomUtils::GetRandomFloat(-radius, radius);
		float offsetY = RandomUtils::GetRandomFloat(-radius, radius);
		return FVector(offsetX, offsetY, 0.0f);
		};

	FVector center;

	if (to == ZoneType::TUK && from == ZoneType::TIP)
		center = FVector(-5270.0, 15050.0, 147);
	else if (to == ZoneType::TUK && from == ZoneType::E)
		center = FVector(-4420.0, -12730.0, 837);
	else if (to == ZoneType::TUK && from == ZoneType::GYM)
		center = FVector(-4180.0, 5220.0, 147);
	else if (to == ZoneType::TUK && from == ZoneType::INDUSTY)
		center = FVector(8721.06, -19229.73, 146.28);
	else if (to == ZoneType::TIP)
		center = FVector(-100, 100, 147);
	else if (to == ZoneType::E)
		center = FVector(-150, 1500, 147);
	else if (to == ZoneType::GYM)
		center = FVector(-2000, 0, 147);
	else if (to == ZoneType::INDUSTY)
		center = FVector(0, -10640.0, 147);
	else
		return FVector::ZeroVector;

	const float radius = 100.0f; //범위 내에서
	return center + RandomOffset(radius);
}
bool Map::IsZoneAccessible(ZoneType zone, uint32 playerLevel) const
{
	switch (zone)
	{
	case ZoneType::TIP:       return playerLevel > 0;
	case ZoneType::TUK:       return playerLevel > 0;
	case ZoneType::E:         return playerLevel > 0;
	case ZoneType::INDUSTY:   return playerLevel > 0;
	case ZoneType::GYM:       return playerLevel > 0;
	default:                  return false;
	}
}

FVector Map::GetStartPos(ZoneType startZone)
{
	switch (startZone)
	{
	case ZoneType::TIP:
	{
		FVector newPos;
		float radius = playerCollision;
		do {
			newPos = Map::GetInst().GetRandomPos(startZone, radius);
		} while (GameWorld::GetInst().IsCollisionDetected(startZone, newPos, radius));
		return newPos;
	}
	case ZoneType::TUK:
	{
		return FVector(-5270.0, 15050.0, 147);
	}
	}

}

FVector Map::GetBossMonsterSpawnPos(Type::EMonster monster)
{
	using namespace Type;
	switch (monster)
	{
	case EMonster::TINO:
		return FVector(1500, 0, 170);
	default:
		return FVector::ZeroVector;
	}
}