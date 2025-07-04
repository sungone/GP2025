#pragma once

struct Triangle {
	int a, b, c;
	Triangle(int A = 0, int B = 0, int C = 0) :a(A), b(B), c(C) {}
};

class NavMesh
{
public:
	static std::optional<NavMesh> LoadFromJson(const std::string& filePath);
	void BuildAdjacencyCSR();
	int FindIdxFromPos(const FVector& pos) const;
	FVector GetRandomPosition() const;
	std::vector<int> FindPath(int startTri, int goalTri) const;

public:
	std::vector<FVector>       vertices;
	std::vector<Triangle>      triangles;
	std::vector<int>           nbrIndices;
	std::vector<int>           nbrOffsets;
};
