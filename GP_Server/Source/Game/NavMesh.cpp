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
	std::priority_queue<PolygonNodeState, std::vector<PolygonNodeState>, std::greater<>> OpenSet;
	std::unordered_map<int, float> CostSoFar;
	std::unordered_map<int, int> CameFrom;
	std::unordered_set<int> Visited;

	FVector GoalCenter = Vertices[Triangles[GoalPolyIdx].IndexA];

	OpenSet.push({ StartPolyIdx, 0, Vertices[Triangles[StartPolyIdx].IndexA].DistanceTo(GoalCenter), -1 });
	CostSoFar[StartPolyIdx] = 0;

	while (!OpenSet.empty()) {
		PolygonNodeState Current = OpenSet.top();
		OpenSet.pop();

		if (Current.PolygonIndex == GoalPolyIdx) {
			std::vector<int> Path;
			for (int i = Current.PolygonIndex; i != -1; i = CameFrom[i]) {
				Path.push_back(i);
			}
			std::reverse(Path.begin(), Path.end());
			return Path;
		}

		if (!Visited.insert(Current.PolygonIndex).second) continue;

		for (int Neighbor : PolygonGraph[Current.PolygonIndex].Neighbors) {
			if (Visited.find(Neighbor) != Visited.end()) continue;

			float NewCost = CostSoFar[Current.PolygonIndex] + Vertices[Triangles[Neighbor].IndexA].DistanceTo(Vertices[Triangles[Current.PolygonIndex].IndexA]);

			if (CostSoFar.find(Neighbor) == CostSoFar.end() || NewCost < CostSoFar[Neighbor]) {
				CostSoFar[Neighbor] = NewCost;
				OpenSet.push({ Neighbor, NewCost, Vertices[Triangles[Neighbor].IndexA].DistanceTo(GoalCenter), Current.PolygonIndex });
				CameFrom[Neighbor] = Current.PolygonIndex;
			}
		}
	}
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
