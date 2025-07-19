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

	bool Init()
	{
		std::lock_guard<std::mutex> lock(_mutex);

		using EC = EWorldChannel;
		for (uint8 i = static_cast<uint8>(EC::TUWorld_1); i <= static_cast<uint8>(EC::TUWorld_3); ++i)
		{
			EC channel = static_cast<EC>(i);
			if (_gameWorlds.contains(channel)) continue;

			auto world = std::make_unique<GameWorld>();
			if (!world->Init(channel))
			{
				LOG_E("Failed to init GameWorld for channel {}", static_cast<int>(channel));
				return false;
			}

			_gameWorlds[channel] = std::move(world);
			_worldStates[channel] = EWorldState::Good;
		}
		return true;
	}

	void CreateChannel(EWorldChannel channelId)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		if (_gameWorlds.contains(channelId))
			return;

		auto world = std::make_unique<GameWorld>();
		world->Init(channelId);
		_gameWorlds[channelId] = std::move(world);
		_worldStates[channelId] = EWorldState::Good;
	}

	GameWorld* GetWorld(EWorldChannel channelId)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		auto it = _gameWorlds.find(channelId);
		if (it != _gameWorlds.end()) return it->second.get();
		return nullptr;
	}

	EWorldState GetWorldState(EWorldChannel channelId)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		auto it = _worldStates.find(channelId);
		if (it != _worldStates.end()) return it->second;
		return EWorldState::Normal;
	}

	void SetWorldState(EWorldChannel channelId, EWorldState newState)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_worldStates[channelId] = newState;
	}

private:
	std::unordered_map<EWorldChannel, std::unique_ptr<GameWorld>> _gameWorlds;
	std::unordered_map<EWorldChannel, EWorldState> _worldStates;
	std::mutex _mutex;
};
