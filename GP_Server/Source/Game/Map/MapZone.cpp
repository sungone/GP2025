#include "pch.h"
#include "MapZone.h"
#include "GameWorld.h"

bool Map::Init()
{
	_navMeshes[ZoneType::PLAYGROUND] = NavMesh(MapDataPath + "NavMeshData_Playground.json");
	_navMeshes[ZoneType::TUK] = NavMesh(MapDataPath + "NavMeshData_TUK.json");
	_navMeshes[ZoneType::TIP] = NavMesh(MapDataPath + "NavMeshData_TIP.json");
	_navMeshes[ZoneType::E] = NavMesh(MapDataPath + "NavMeshData_E.json");
	_navMeshes[ZoneType::GYM] = NavMesh(MapDataPath + "NavMeshData_GYM.json");
	_navMeshes[ZoneType::INDUSTY] = NavMesh(MapDataPath + "NavMeshData_Industry.json");
	_navMeshes[ZoneType::BUNKER] = NavMesh(MapDataPath + "NavMeshData_Bunker.json");

	_entryMeshes[EntryType::TIP_IN] = NavMesh(MapDataPath + "NavMeshData_TIP_In.json");
	_entryMeshes[EntryType::TIP_OUT] = NavMesh(MapDataPath + "NavMeshData_TIP_Out.json");
	_entryMeshes[EntryType::E_IN] = NavMesh(MapDataPath + "NavMeshData_E_In.json");
	_entryMeshes[EntryType::E_OUT] = NavMesh(MapDataPath + "NavMeshData_E_Out.json");
	_entryMeshes[EntryType::GYM_IN] = NavMesh(MapDataPath + "NavMeshData_GYM_In.json");
	_entryMeshes[EntryType::GYM_OUT] = NavMesh(MapDataPath + "NavMeshData_GYM_Out.json");
	_entryMeshes[EntryType::INDUSTY_IN] = NavMesh(MapDataPath + "NavMeshData_Industry_In.json");
	_entryMeshes[EntryType::INDUSTY_OUT] = NavMesh(MapDataPath + "NavMeshData_Industry_Out.json");

	for (auto& [zone, mesh] : _navMeshes)
	{
		if (!mesh.IsLoaded())
		{
			LOG(Warning, std::format("Failed to load zone mesh [{}]", static_cast<int>(zone)));
			return false;
		}
	}

	for (auto& [entry, mesh] : _entryMeshes)
	{
		if (!mesh.IsLoaded())
		{
			LOG(Warning, std::format("Failed to load entry mesh [{}]", static_cast<int>(entry)));
			return false;
		}
	}

	return true;
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

FVector Map::GetRandomPos(ZoneType type, float collisionRadius) const
{
	auto it = _navMeshes.find(type);
	if (it != _navMeshes.end())
		return it->second.GetRandomPositionWithRadius(collisionRadius);

	return FVector::ZeroVector;
}

NavMesh& Map::GetNavMesh(ZoneType type)
{
	auto it = _navMeshes.find(type);
	if (it == _navMeshes.end())
		LOG(Warning, "type Invaild");

	return it->second;
}

FVector Map::GetRandomEntryPos(ZoneType oldZone, ZoneType targetZone) const
{
	EntryType entryType;

	switch (targetZone)
	{
	case ZoneType::TIP:
		entryType = EntryType::TIP_IN;
		break;
	case ZoneType::E:
		entryType = EntryType::E_IN;
		break;
	case ZoneType::GYM:
		entryType = EntryType::GYM_IN;
		break;
	case ZoneType::INDUSTY:
		entryType = EntryType::INDUSTY_IN;
		break;
	case ZoneType::TUK:
	{
		if (oldZone == ZoneType::TIP)
			entryType = EntryType::TIP_OUT;
		else if (oldZone == ZoneType::E)
			entryType = EntryType::E_OUT;
		else if (oldZone == ZoneType::GYM)
			entryType = EntryType::GYM_OUT;
		else if (oldZone == ZoneType::INDUSTY)
			entryType = EntryType::INDUSTY_OUT;
		else
			return FVector::ZeroVector;
		break;
	}
	default:
		return FVector::ZeroVector;
	}
	
	return GetRandomEntryPos(entryType, playerCollision);
}

FVector Map::GetRandomEntryPos(EntryType entryType, float collisionRadius) const
{
	auto it = _entryMeshes.find(entryType);
	if (it != _entryMeshes.end())
		return it->second.GetRandomPositionWithRadius(collisionRadius);

	return FVector::ZeroVector;
}

NavMesh& Map::GetEntryNavMesh(EntryType entryType)
{
	return _entryMeshes.at(entryType);
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