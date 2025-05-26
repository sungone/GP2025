#include "pch.h"
#include "QuestTable.h"

#include "QuestTable.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool QuestTable::LoadFromCSV(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		std::cerr << "Failed to open QuestTable CSV: " << filePath << std::endl;
		return false;
	}

	std::string line;
	std::getline(file, line);

	while (std::getline(file, line))
	{
		std::stringstream ss(line);
		std::string cell;

		QuestData data = {};
		uint32 idx = 0;
		uint32 chap = 0;

		// QuestID (as index)
		std::getline(ss, cell, ','); idx = std::stoi(cell);
		data.QuestID = static_cast<QuestType>(idx);
		std::getline(ss, cell, ','); chap = std::stoi(cell);
		data.Chapter = chap;

		// Name, Description (skip)
		std::getline(ss, cell, ',');
		std::getline(ss, cell, ',');

		std::getline(ss, cell, ',');
		if (cell == "INTERACT")
			data.Catagory = EQuestCategory::INTERACT;
		else if (cell == "MOVE")
			data.Catagory = EQuestCategory::MOVE;
		else if (cell == "KILL")
			data.Catagory = EQuestCategory::KILL;
		else if (cell == "ITEM")
			data.Catagory = EQuestCategory::ITEM;
		else
			data.Catagory = EQuestCategory::MOVE;

		std::getline(ss, cell, ','); data.TargetID = std::stoi(cell);
		std::getline(ss, cell, ','); data.ExpReward = std::stoi(cell);
		std::getline(ss, cell, ','); data.GoldReward = std::stoi(cell);
		std::getline(ss, cell, ','); data.NextQuestID = static_cast<QuestType>(std::stoi(cell));

		_quests[idx] = data;
		_questsByChapter[chap].push_back(&_quests[idx]);
	}

	return true;
}

const QuestData* QuestTable::GetQuest(QuestType quest) const
{
	auto it = _quests.find(static_cast<int32>(quest));
	if (it != _quests.end())
		return &it->second;
	return nullptr;
}

const std::vector<const QuestData*>* QuestTable::GetQuestsByChapter(uint8 chapter) const
{
	auto it = _questsByChapter.find(chapter);
	if (it != _questsByChapter.end())
		return &it->second;
	return nullptr;
}