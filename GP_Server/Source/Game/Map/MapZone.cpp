#include "pch.h"
#include "MapZone.h"
#include "GameWorld.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

bool Map::Init()
{
	const std::vector<std::pair<ZoneType, std::string>> ZoneMeshFiles = {
	{ ZoneType::PLAYGROUND, "NavMeshData_Playground.json" },
	{ ZoneType::TUK,        "NavMeshData_TUK.json" },
	{ ZoneType::TIP,        "NavMeshData_TIP.json" },
	{ ZoneType::E,          "NavMeshData_E.json" },
	{ ZoneType::GYM,        "NavMeshData_GYM.json" },
	{ ZoneType::INDUSTY,    "NavMeshData_Industry.json" },
	{ ZoneType::BUNKER,     "NavMeshData_Bunker.json" }
	};
	const std::vector<std::pair<EntryType, std::string>> EntryMeshFiles = {
	{ EntryType::TIP_IN,       "NavMeshData_TIP_In.json" },
	{ EntryType::TIP_OUT,      "NavMeshData_TIP_Out.json" },
	{ EntryType::E_IN,         "NavMeshData_E_In.json" },
	{ EntryType::E_OUT,        "NavMeshData_E_Out.json" },
	{ EntryType::GYM_IN,       "NavMeshData_GYM_In.json" },
	{ EntryType::GYM_OUT,      "NavMeshData_GYM_Out.json" },
	{ EntryType::INDUSTY_IN,   "NavMeshData_Industry_In.json" },
	{ EntryType::INDUSTY_OUT,  "NavMeshData_Industry_Out.json" }
	};

	for (const auto& [zone, file] : ZoneMeshFiles)
	{
		auto meshOpt = NavMesh::LoadFromJson(MapDataPath + file);
		if (!meshOpt)
		{
			LOG(Warning, std::format("Failed to load zone mesh [{}]", static_cast<int>(zone)));
			return false;
		}
		_navMeshes[zone] = std::move(*meshOpt);
	}

	for (const auto& [entry, file] : EntryMeshFiles)
	{
		auto meshOpt = NavMesh::LoadFromJson(MapDataPath + file);
		if (!meshOpt)
		{
			LOG(Warning, std::format("Failed to load entry mesh [{}]", static_cast<int>(entry)));
			return false;
		}
		_entryMeshes[entry] = std::move(*meshOpt);
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

FVector Map::GetRandomPos(ZoneType type) const
{
	auto it = _navMeshes.find(type);
	if (it != _navMeshes.end())
		return it->second.GetRandomPosition();

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
	
	return GetRandomEntryPos(entryType);
}

FVector Map::GetRandomEntryPos(EntryType entryType) const
{
	auto it = _entryMeshes.find(entryType);
	if (it != _entryMeshes.end())
		return it->second.GetRandomPosition();

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
			newPos = Map::GetInst().GetRandomPos(startZone);
		} while (GameWorld::GetInst().IsCollisionDetected(startZone, newPos, radius));
		return newPos;
	}
	case ZoneType::TUK:
	{
		return FVector(-5270.0, 15050.0, 147);
	}
	}

}