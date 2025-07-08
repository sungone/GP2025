#pragma once

class NavMesh
{
public:
	static std::optional<NavMesh> LoadFromJson(const std::string& filePath);
	int FindIdxFromPos(const FVector& pos) const;
	FVector GetRandomPosition() const;
	std::vector<int> FindPath(int startTri, int goalTri) const;

public:
	std::vector<FVector>       vertices;
	std::vector<std::vector<int>> polygons;
	std::vector<std::vector<int>> neighbors;
};
