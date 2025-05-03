#include "pch.h"
#include "NavMesh.h"

void NavMesh::BuildPolygonGraph()
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
}

int NavMesh::FindIdxFromPos(const FVector _pos)
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

std::vector<int> NavMesh::FindPath(int StartPolyIdx, int GoalPolyIdx)
{
	if (StartPolyIdx == GoalPolyIdx)
		return { StartPolyIdx };

	std::unordered_map<int, int> cameFrom;
	std::unordered_map<int, float> gScore;
	std::unordered_map<int, float> fScore;
	std::unordered_set<int> closedSet;

	auto heuristic = [&](int from, int to) -> float {
		FVector fromCenter = GetTriangleCenter(from);
		FVector toCenter = GetTriangleCenter(to);
		return fromCenter.DistanceTo(toCenter);
		};

	auto cmp = [&](int left, int right) {
		return fScore[left] > fScore[right];
		};

	std::priority_queue<int, std::vector<int>, decltype(cmp)> openSet(cmp);
	openSet.push(StartPolyIdx);
	gScore[StartPolyIdx] = 0;
	fScore[StartPolyIdx] = heuristic(StartPolyIdx, GoalPolyIdx);

	while (!openSet.empty())
	{
		int current = openSet.top();
		openSet.pop();

		if (current == GoalPolyIdx)
		{
			std::vector<int> path;
			for (int node = current; node != StartPolyIdx; node = cameFrom[node])
				path.push_back(node);
			path.push_back(StartPolyIdx);
			std::reverse(path.begin(), path.end());
			return path;
		}

		closedSet.insert(current);

		for (int neighbor : PolygonGraph[current].Neighbors)
		{
			if (closedSet.contains(neighbor))
				continue;

			float tentative_g = gScore[current] + GetTriangleCenter(current).DistanceTo(GetTriangleCenter(neighbor));

			if (!gScore.contains(neighbor) || tentative_g < gScore[neighbor])
			{
				cameFrom[neighbor] = current;
				gScore[neighbor] = tentative_g;
				fScore[neighbor] = tentative_g + heuristic(neighbor, GoalPolyIdx);
				openSet.push(neighbor);
			}
		}
	}
	return {};
}

bool NavMesh::LoadFromJson(const std::string& filePath)
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
		Vertices.reserve(verts.Size());
		for (const auto& vertVal : verts.GetArray()) {
			Vertices.emplace_back(vertVal[0].GetFloat(), vertVal[1].GetFloat(), vertVal[2].GetFloat());
		}
	}

	if (doc.HasMember("Triangles") && doc["Triangles"].IsArray()) {
		const auto& tris = doc["Triangles"];
		Triangles.reserve(tris.Size());
		for (const auto& triVal : tris.GetArray()) {
			Triangles.emplace_back(triVal["IndexA"].GetInt(), triVal["IndexB"].GetInt(), triVal["IndexC"].GetInt());
		}
	}

	BuildPolygonGraph();
	return true;
}

FVector NavMesh::GetRandomPositionWithRadius(float radius) const
{
	if (Triangles.empty() || Vertices.empty())
		return FVector(0, 0, 0);

	for (int attempt = 0; attempt < 10; ++attempt)
	{
		int RandomTriangleIndex = RandomUtils::GetRandomInt(0, Triangles.size() - 1);
		const Triangle& tri = Triangles[RandomTriangleIndex];

		const FVector& A = Vertices[tri.IndexA];
		const FVector& B = Vertices[tri.IndexB];
		const FVector& C = Vertices[tri.IndexC];

		float r1 = RandomUtils::GetRandomFloat(0.0f, 1.0f);
		float r2 = RandomUtils::GetRandomFloat(0.0f, 1.0f);
		if (r1 + r2 > 1.0f)
		{
			r1 = 1.0f - r1;
			r2 = 1.0f - r2;
		}

		FVector P = A + (B - A) * r1 + (C - A) * r2;
		P.Z += 100.0f;//이건 사이즈 고려안함 현재 클라에서 설정된 캡슐의 반만큼 올려주는중 -> 추후 

		auto DistanceToEdge2D = [](const FVector& p, const FVector& a, const FVector& b) -> float {
			double dx = b.X - a.X;
			double dy = b.Y - a.Y;
			double lengthSq = dx * dx + dy * dy;

			if (lengthSq == 0.0)
				return std::sqrt((p.X - a.X) * (p.X - a.X) + (p.Y - a.Y) * (p.Y - a.Y));

			double t = ((p.X - a.X) * dx + (p.Y - a.Y) * dy) / lengthSq;
			t = std::clamp(t, 0.0, 1.0);
			double projX = a.X + t * dx;
			double projY = a.Y + t * dy;

			double distX = p.X - projX;
			double distY = p.Y - projY;
			return std::sqrt(distX * distX + distY * distY);
			};

		if (DistanceToEdge2D(P, A, B) >= radius &&
			DistanceToEdge2D(P, B, C) >= radius &&
			DistanceToEdge2D(P, C, A) >= radius)
		{
			return P;
		}
	}

	return FVector(0, 0, 0);
}


FVector NavMesh::GetTriangleCenter(int triIndex) const
{
	const Triangle& tri = Triangles[triIndex];
	const FVector& A = Vertices[tri.IndexA];
	const FVector& B = Vertices[tri.IndexB];
	const FVector& C = Vertices[tri.IndexC];

	float a = (B - C).Length();
	float b = (C - A).Length();
	float c = (A - B).Length();

	float sum = a + b + c;

	if (sum == 0) return A;

	return (A * a + B * b + C * c) / sum;
}

const std::unordered_set<int>& NavMesh::GetNeighbors(int triIdx) const
{
	auto it = PolygonGraph.find(triIdx);
	if (it != PolygonGraph.end())
		return it->second.Neighbors;
	static const std::unordered_set<int> Empty;
	return Empty;
}