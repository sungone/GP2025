#pragma once
#include "Character.h"
#include "Inventory.h"

class Player : public Character
{
public:
    Inventory _inventory;
};
