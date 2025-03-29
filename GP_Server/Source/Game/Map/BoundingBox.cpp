#include "pch.h"
#include "BoundingBox.h"

bool BoundingBox::CheckAABBCollision(const BoundingBox& Other) const
{
    return (Min.X <= Other.Max.X && Max.X >= Other.Min.X) &&
        (Min.Y <= Other.Max.Y && Max.Y >= Other.Min.Y) &&
        (Min.Z <= Other.Max.Z && Max.Z >= Other.Min.Z);
}

bool LoadBoundingBoxFromJSON(const std::string& filePath, std::vector<BoundingBox>& BoundingBoxes, bool isPrint)
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
	{
		for (const auto& bbox : BoundingBoxes)
		{
			std::cout << bbox << '\n';
		}
	}
	LOG("BoundingBoxes loaded successfully!");
	return true;
}