#include "pch.h"
#include "Inventory.h"

bool Inventory::AddItem(const Item& item)
{
	for (auto& slot : slots)
	{
		if (slot.item.itemId == item.itemId)
		{
			slot.item.quantity += item.quantity;
			return true;
		}
	}

	for (auto& slot : slots)
	{
		if (slot.item.itemId == 0) {
			slot.item = item;
			return true;
		}
	}

	return false; // 인벤토리 가득 참
}

bool Inventory::RemoveItem(int itemId, int quantity)
{
	for (auto& slot : slots)
	{
		if (slot.item.itemId == itemId)
		{
			if (slot.item.quantity >= quantity)
			{
				slot.item.quantity -= quantity;
				if (slot.item.quantity == 0)
				{
					slot.item = { 0, 0 };
				}
				return true;
			}
			return false;
		}
	}
	return false;
}

Item* Inventory::GetItem(int slotIndex)
{
	if (slotIndex < 0 || slotIndex >= maxSlots)
		return nullptr;
	if (slots[slotIndex].item.itemId == 0)
		return nullptr;

	return &slots[slotIndex].item;
}
