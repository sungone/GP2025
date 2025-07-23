#pragma once
#include "GameWorld.h"

class GameWorldManager
{
public:
	static GameWorldManager& GetInst()
	{
		static GameWorldManager inst;
		return inst;
	}

	bool Init();
	void CreateChannel(EWorldChannel channelId);

	GameWorld* GetAvailableWorld(EWorldChannel& inOutChannelId);
	GameWorld* GetWorld(EWorldChannel channelId);
	EWorldState GetWorldState(EWorldChannel channelId);
	std::array<FWorldState, WORLD_MAX_COUNT> GetAllWorldStates();

	void UpdateWorldStates();
	
private:
	std::unordered_map<EWorldChannel, std::unique_ptr<GameWorld>> _gameWorlds;
	std::unordered_map<EWorldChannel, EWorldState> _worldStates;
	std::mutex _mutex;
};
