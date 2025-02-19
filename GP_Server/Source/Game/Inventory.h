#pragma once

class Inventory 
{
public:
    explicit Inventory(int maxSlots) : maxSlots(maxSlots)
    {
        slots.resize(maxSlots);
    }

    bool AddItem(const FItem& item);
    bool RemoveItem(uint32 itemId, uint32 quantity);

private:
    int maxSlots;
    std::vector<FItem> slots;
};
