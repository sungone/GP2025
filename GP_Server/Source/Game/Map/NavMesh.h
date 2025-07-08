#pragma once

struct AStarNode {
	int    poly;
	float  g;        // 실제 비용
	float  f;        // g + h
	int    parent;   // 경로 재구성을 위한 이전 폴리곤
};

struct PQCompare {
	bool operator()(const AStarNode& a, const AStarNode& b) const {
		return a.f > b.f;
	}
};

class NavMesh
{
public:
	static std::optional<NavMesh> LoadFromJson(const std::string& filePath);
	int FindIdxFromPos(const FVector& pos) const;
	FVector GetRandomPosition() const;
	std::vector<int> FindPath(int startTri, int goalTri) const;
	std::vector<int> FindPathAStar(const FVector& startPos, const FVector& goalPos) const;

public:
	std::vector<FVector>       vertices;
	std::vector<std::vector<int>> polygons;
	std::vector<std::vector<int>> neighbors;
};
