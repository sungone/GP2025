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

	bool Init()
	{
		NavMeshData test;
		if (!LoadNavMeshFromJson("NavMeshData.json", test, true))
		{
			return false;
		}
		LOG("NavMeshData loaded successfully!");
		return true;
	}
};

