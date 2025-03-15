#pragma once
#include "BoundingBoxManager.h"

class Map
{
public:
	static Map& GetInst()
	{
		static Map inst;
		return inst;
	}

	bool Init()
	{
		if (!_tukMap.LoadFromJSON("MapData.json", false))
		{
			LOG(LogType::Warning, "Failed to load MapData");
			return false;
		}
		return true;
	}
public:
	BoundingBoxManager _tukMap;
};

