#pragma once
#include "NavMesh.h"

enum class ZoneType
{
	DEFAULT,
	PLAYGROUND,
};

class Zone
{
public:
	Zone() = default;
	Zone(ZoneType type, const std::string& navPath)
		: _type(type), _navMesh(navPath)
	{
	}

	FVector GetRandomPos() const { return _navMesh.GetRandomPosition(); }
	NavMesh& GetNavMesh() {return _navMesh;}

private:
	ZoneType _type;
	NavMesh _navMesh;
};

class Map
{
public:
	static Map& GetInst()
	{
		static Map inst;
		return inst;
	}

	bool Init();
	FVector GetRandomPos(ZoneType type) const;
	NavMesh& GetNavMesh(ZoneType type);

private:
	std::unordered_map<ZoneType, NavMesh> _zoneNavMeshes;
};

