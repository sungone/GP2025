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
		NavMesh test;
		if (!NavMesh::LoadFromJson("NavMeshData.json", test, false))
		{
			return false;
		}
		NavMesh::BuildPolygonGraph(false);
		LOG("NavMesh loaded successfully!");
		return true;
	}
};

