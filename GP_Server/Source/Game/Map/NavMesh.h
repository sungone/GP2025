#pragma once
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

struct Edge {
	int v1, v2;
	Edge(int a, int b) {
		if (a < b) { v1 = a; v2 = b; }
		else { v1 = b; v2 = a; }
	}
	bool operator==(const Edge& other) const {
		return v1 == other.v1 && v2 == other.v2;
	}
};

struct EdgeHash {
	std::size_t operator()(const Edge& e) const {
		return std::hash<int>()(e.v1) ^ (std::hash<int>()(e.v2) << 1);
	}
};

struct Triangle
{
	int IndexA, IndexB, IndexC;
	std::string ToString() const {
		std::ostringstream oss;
		oss << IndexA << " " << IndexB << " " << IndexC;
		return oss.str();
	}
};

struct PolygonNode
{
	int TriangleIndex;
	std::unordered_set<int> Neighbors;

	std::string ToString() const {
		std::ostringstream oss;
		oss << TriangleIndex << " -> ";
		for (int neighbor : Neighbors) {
			oss << neighbor << " ";
		}
		return oss.str();
	}
};

struct PolygonNodeState {
	int PolygonIndex;
	float Cost, Heuristic;
	int Parent;

	bool operator>(const PolygonNodeState& Other) const {
		return (Cost + Heuristic) > (Other.Cost + Other.Heuristic);
	}
};

class NavMesh
{
public:
	NavMesh() {}
	NavMesh(const std::string& filePath)
	{
		if (!LoadFromJson(filePath))
		{
			LOG(Error,"NavMesh");
		}
	}

	void BuildPolygonGraph();
	int FindIdxFromPos(const FVector& _pos);

	void PrintNavMesh();
	void PrintPolygonGraph();
	std::vector<int> FindPath(int StartPolygon, int GoalPolygon);
	bool LoadFromJson(const std::string& filePath);
	FVector GetRandomPositionWithRadius(float radius) const;

	FVector GetTriangleCenter(int triIndex) const;
	const std::unordered_set<int>& GetNeighbors(int triIdx) const;

public:
	std::vector<FVector> Vertices;
	std::vector<Triangle> Triangles;
	std::unordered_map<int, PolygonNode> PolygonGraph;
};
