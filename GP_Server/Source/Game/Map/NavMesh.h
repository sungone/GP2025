#pragma once

struct TriangleInfo
{
	int polyIdx;
	int triOffset;
	float cumulativeArea;
};

struct PairHash {
	std::size_t operator()(const std::pair<int, int>& p) const {
		return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
	}
};

class NavMesh
{
public:
	static std::optional<NavMesh> LoadFromJson(const std::string& filePath);
	void Init();
	void BuildPolygonCenters();
	void BuildTriangleAreas();
	void BuildPortalEdges();

	int FindIdxFromPos(const FVector& pos) const;
	int FindClosestPoly(const FVector& pos) const;
	FVector GetRandomPosition() const;
	std::vector<int> FindPathAStar(const FVector& startPos, const FVector& goalPos) const;
	std::vector<FVector> GetStraightPath(const FVector& startPos, const FVector& endPos, const std::vector<int>& polyPath) const;
	FVector GetNearbyRandomPosition(const FVector& origin) const;

public:
	std::vector<FVector>       vertices;
	std::vector<std::vector<int>> polygons;
	std::vector<std::vector<int>> neighbors;
	std::vector<FVector> polyCenters;
	std::vector<TriangleInfo> triangleSamples;
	float triangleTotalArea = 0.f;
	std::unordered_map<std::pair<int, int>, std::pair<FVector, FVector>, PairHash> portalEdgeMap;
};
