#pragma once
#include <rapidjson/document.h>
#include "BoundingBox.h"

class BoundingBoxManager
{
private:
    std::vector<BoundingBox> BoundingBoxes;

public:

    bool LoadFromJSON(const std::string& filePath, bool isPrint = false);
    void PrintBoundingBoxes() const;
    void CheckCollisions(const FVector& Position, float Radius) const;
};