#pragma once
#include <rapidjson/document.h>

struct BoundingBox
{
    std::string Name;
    FVector Min;
    FVector Max;

    BoundingBox() : Name(""), Min(), Max() {}
    BoundingBox(const std::string& name, const FVector& min, const FVector& max)
        : Name(name), Min(min), Max(max) {
    }

    bool CheckAABBCollision(const BoundingBox& Other) const;

    friend std::ostream& operator<<(std::ostream& os, const BoundingBox& bbox)
    {
        os << "*** "<< bbox.Name << " -  Min" << bbox.Min.ToString() << " Max" << bbox.Max.ToString();
        return os;
    }
};

bool LoadBoundingBoxFromJSON(const std::string& filePath, std::vector<BoundingBox>& BoundingBoxes, bool isPrint);