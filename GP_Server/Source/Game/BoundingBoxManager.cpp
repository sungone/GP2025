#include "pch.h"
#include "BoundingBoxManager.h"

bool BoundingBoxManager::LoadFromJSON(const std::string& filePath, bool isPrint)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		LOG(LogType::Warning, std::format("Failed to open file: {}", filePath));
		return false;
	}

	std::string jsonStr((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	rapidjson::Document document;
	document.Parse(jsonStr.c_str());

	if (!document.IsObject() || !document.HasMember("BoundingBoxes") || !document["BoundingBoxes"].IsArray())
	{
		LOG(LogType::Warning, "Invalid JSON format!");
		return false;
	}

	BoundingBoxes.clear();
	for (const auto& obj : document["BoundingBoxes"].GetArray())
	{
		std::string name = obj["Name"].GetString();
		const auto& box = obj["BoundingBox"];
		FVector min(box["MinX"].GetFloat(), box["MinY"].GetFloat(), box["MinZ"].GetFloat());
		FVector max(box["MaxX"].GetFloat(), box["MaxY"].GetFloat(), box["MaxZ"].GetFloat());

		BoundingBoxes.emplace_back(name, min, max);
	}
	if (isPrint)
		PrintBoundingBoxes();
	LOG(LogType::Log, std::format("Loaded '{}'boxes from '{}'", BoundingBoxes.size(), filePath));
	return true;
}

void BoundingBoxManager::PrintBoundingBoxes() const
{
	for (const auto& bbox : BoundingBoxes)
	{
		std::cout << bbox << '\n';
	}
}

void BoundingBoxManager::CheckCollisions(const FVector& Position, float Radius) const
{
	BoundingBox targetBox("None", Position - FVector(Radius, Radius, Radius), Position + FVector(Radius, Radius, Radius));

	for (const auto& bbox : BoundingBoxes)
	{
		if (targetBox.CheckAABBCollision(bbox))
		{
			LOG(LogType::Log, std::format("Collision detected with: {}", bbox.Name));
		}
	}
}