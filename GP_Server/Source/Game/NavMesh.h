#pragma once
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

struct NavTriangle
{
    int IndexA;
    int IndexB;
    int IndexC;
    int AreaID;

    friend std::ostream& operator<<(std::ostream& os, const NavTriangle& t)
    {
        os << t.IndexA << " " << t.IndexB << " " << t.IndexC << " " << t.AreaID;
        return os;
    }

    std::string ToString() const
    {
        std::ostringstream oss;
        oss << *this;
        return oss.str();
    }
};

struct NavMeshData
{
    int NumTiles = 0;
    std::vector<FVector> Vertices;
    std::vector<NavTriangle> Triangles;

    void Print()
    {
		PRINT(std::format("NumTiles: {} Vertices: {} Triangles: {}", NumTiles, Vertices.size(), Triangles.size()));

		for (const auto& v : Vertices)
		{
			PRINT(v.ToString());
		}

		for (const auto& t : Triangles)
		{
			PRINT(t.ToString());
		}
    }
};

bool LoadNavMeshFromJson(const std::string& filePath, NavMeshData& OutNavMeshData, bool isPrint);
