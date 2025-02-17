#pragma once
struct Item
{
    uint32 itemId;
    int quantity;
};

struct InventorySlot
{
    int slotIndex;
    Item item;
};

class Inventory 
{
public:
    explicit Inventory(int maxSlots) : maxSlots(maxSlots)
    {
        slots.resize(maxSlots);
    }

    bool AddItem(const Item& item);
    bool RemoveItem(int itemId, int quantity);
    Item* GetItem(int slotIndex);
private:
    int maxSlots;
    std::vector<InventorySlot> slots;
};
