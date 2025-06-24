#pragma once
struct QuestData
{
	QuestType QuestID;
	uint8 Chapter;
	EQuestCategory Catagory;
	int32 ExpReward;
	int32 GoldReward;
	QuestType NextQuestID = QuestType::NONE;
	uint8 RewarditemType;
};

class QuestTable
{
public:
	static QuestTable& GetInst()
	{
		static QuestTable inst;
		return inst;
	}

	bool LoadFromCSV(const std::string& filePath);
	const QuestData* GetQuest(QuestType quest) const;
	const std::vector<const QuestData*>* GetQuestsByChapter(uint8 chapter) const;
private:
	std::unordered_map<uint32, QuestData> _quests;
	std::unordered_map<uint8, std::vector<const QuestData*>> _questsByChapter;
};
