#pragma once
#include "NavMesh.h"

class Map
{
public:
	static Map& GetInst()
	{
		static Map inst;
		return inst;
	}

	bool Init();
	bool IsZoneAccessible(ZoneType zone, uint32 playerLevel) const;

	FVector GetRandomPos(ZoneType type, float collisionRadius) const;
	NavMesh& GetNavMesh(ZoneType type);
	
	FVector GetRandomEntryPos(ZoneType oldZone, ZoneType targetZone) const;
	FVector GetRandomEntryPos(EntryType entryType, float collisionRadius) const;
	NavMesh& GetEntryNavMesh(EntryType entryType);

	FVector GetStartPos(ZoneType startZone);
private:
	std::unordered_map<ZoneType, NavMesh> _navMeshes;
	std::unordered_map<EntryType, NavMesh> _entryMeshes;

};

