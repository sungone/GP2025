#include "pch.h"
#include "MapZone.h"

bool MapZone::Init()
{
	_playground = NavMesh("playground.json");
	return true;
}

FVector MapZone::GetRandomPos(ZoneType type) const
{
	switch (type)
	{
	case ZoneType::PLAYGROUND:
		return _playground.GetRandomPosition();
	}
}