#include "pch.h"
#include "NavMesh.h"

std::vector<FVector> NavMesh::Vertices;
std::vector<Triangle> NavMesh::Triangles;
std::unordered_map<int, PolygonNode> NavMesh::PolygonGraph;

void NavMesh::BuildPolygonGraph(bool isPrint)
{
	PolygonGraph.clear();

	std::unordered_map<Edge, std::vector<int>, EdgeHash> edgeMap;
	PolygonGraph.reserve(Triangles.size());

	for (int i = 0; i < Triangles.size(); i++) {
		const Triangle& tri = Triangles[i];
		edgeMap[Edge(tri.IndexA, tri.IndexB)].push_back(i);
		edgeMap[Edge(tri.IndexB, tri.IndexC)].push_back(i);
		edgeMap[Edge(tri.IndexC, tri.IndexA)].push_back(i);
		PolygonGraph[i] = { i, {} };
	}

	for (const auto& [edge, triIndices] : edgeMap) {
		if (triIndices.size() >= 2) {
			for (size_t i = 0; i < triIndices.size(); i++) {
				for (size_t j = i + 1; j < triIndices.size(); j++) {
					int t1 = triIndices[i];
					int t2 = triIndices[j];
					PolygonGraph[t1].Neighbors.insert(t2);
					PolygonGraph[t2].Neighbors.insert(t1);
				}
			}
		}
	}

	if (isPrint)
	{
		PrintPolygonGraph();
	}
}

int NavMesh::FindIdxFromPos(const FVector& _pos)
{
	int BestTriangleIndex = -1;
	float MinDistance = FLT_MAX;

	for (int i = 0; i < Triangles.size(); i++) {
		const Triangle& Tri = Triangles[i];

		const FVector& A = Vertices[Tri.IndexA];
		const FVector& B = Vertices[Tri.IndexB];
		const FVector& C = Vertices[Tri.IndexC];

		FVector TriangleCenter = FVector(
			(A.X + B.X + C.X) / 3.0f,
			(A.Y + B.Y + C.Y) / 3.0f,
			(A.Z + B.Z + C.Z) / 3.0f
		);

		float Distance = _pos.DistanceTo(TriangleCenter);
		if (Distance < MinDistance) {
			MinDistance = Distance;
			BestTriangleIndex = i;
		}
	}

	return BestTriangleIndex;
}

void NavMesh::PrintNavMesh()
{
	PRINT("=== NavMesh(Vertices) ===");
	for (const auto& v : Vertices)
	{
		PRINT(v.ToString());
	}

	PRINT("=== NavMesh(Triangles) ===");
	for (const auto& t : Triangles)
	{
		PRINT(t.ToString());
	}
}

void NavMesh::PrintPolygonGraph()
{
	PRINT("=== PolygonGraph ===");
	for (const auto& entry : PolygonGraph)
	{
		const PolygonNode& node = entry.second;
		PRINT(node.ToString());
	}
}

std::vector<int> NavMesh::FindPath(int StartPolyIdx, int GoalPolyIdx)
{
	//Todo:
	return {};
}

bool NavMesh::LoadFromJson(const std::string& filePath, NavMesh& OutNavMeshData, bool isPrint)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		LOG(LogType::Warning, std::format("Failed to open file: {}", filePath));
		return false;
	}

	std::string jsonStr((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	rapidjson::Document doc;
	doc.Parse(jsonStr.c_str());

	if (doc.HasParseError()) {
		std::cerr << "JSON parse error: " << GetParseError_En(doc.GetParseError()) << " at offset " << doc.GetErrorOffset() << std::endl;
		return false;
	}

	if (doc.HasMember("Vertices") && doc["Vertices"].IsArray()) {
		const auto& verts = doc["Vertices"];
		OutNavMeshData.Vertices.reserve(verts.Size());
		for (const auto& vertVal : verts.GetArray()) {
			OutNavMeshData.Vertices.emplace_back(vertVal[0].GetFloat(), vertVal[1].GetFloat(), vertVal[2].GetFloat());
		}
	}

	if (doc.HasMember("Triangles") && doc["Triangles"].IsArray()) {
		const auto& tris = doc["Triangles"];
		OutNavMeshData.Triangles.reserve(tris.Size());
		for (const auto& triVal : tris.GetArray()) {
			OutNavMeshData.Triangles.emplace_back(triVal["IndexA"].GetInt(), triVal["IndexB"].GetInt(), triVal["IndexC"].GetInt());
		}
	}

	if (isPrint)
	{
		OutNavMeshData.PrintNavMesh();
	}

	return true;
}

FVector NavMesh::GetRandomPosition()
{
	if (Triangles.empty() || Vertices.empty())
	{
		return FVector(0, 0, 0);
	}

	int RandomTriangleIndex = RandomUtils::GetRandomInt(0, Triangles.size() - 1);
	const Triangle& SelectedTriangle = Triangles[RandomTriangleIndex];

	const FVector& A = Vertices[SelectedTriangle.IndexA];
	const FVector& B = Vertices[SelectedTriangle.IndexB];
	const FVector& C = Vertices[SelectedTriangle.IndexC];

	float r1 = RandomUtils::GetRandomFloat(0.0f, 1.0f);
	float r2 = RandomUtils::GetRandomFloat(0.0f, 1.0f);

	if (r1 + r2 > 1.0f)
	{
		r1 = 1.0f - r1;
		r2 = 1.0f - r2;
	}

	FVector SpawnPosition = A + (B - A) * r1 + (C - A) * r2;
	return SpawnPosition;
}
