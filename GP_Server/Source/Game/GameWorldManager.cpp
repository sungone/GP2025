#include "pch.h"
#include "GameWorldManager.h"

bool GameWorldManager::Init()
{
	using EC = EWorldChannel;
	for (uint8 i = static_cast<uint8>(EC::Start); i <= static_cast<uint8>(EC::End); ++i)
	{
		EC channel = static_cast<EC>(i);
		CreateChannel(channel);
	}

	TimerQueue::AddTimer([]() {
		GameWorldManager::GetInst().UpdateWorldStates();
		}, 10'000, true);
	return true;
}

void GameWorldManager::CreateChannel(EWorldChannel channelId)
{
	if (_gameWorlds.contains(channelId))
		return;

	auto world = std::make_unique<GameWorld>();
	world->Init(channelId);
	_gameWorlds[channelId] = std::move(world);
	_worldStates[channelId] = EWorldState::Good;
}

GameWorld* GameWorldManager::GetAvailableWorld(EWorldChannel& inOutChannelId)
{
	std::lock_guard<std::mutex> lock(_mutex);

	if (_gameWorlds.contains(inOutChannelId) &&
		_worldStates[inOutChannelId] != EWorldState::Bad)
	{
		return _gameWorlds[inOutChannelId].get();
	}

	for (auto& [channel, worldPtr] : _gameWorlds)
	{
		if (_worldStates[channel] != EWorldState::Bad)
		{
			inOutChannelId = channel;
			return worldPtr.get();
		}
	}

	return nullptr;
}

GameWorld* GameWorldManager::GetWorld(EWorldChannel channelId)
{
	std::lock_guard<std::mutex> lock(_mutex);
	auto it = _gameWorlds.find(channelId);
	if (it != _gameWorlds.end()) return it->second.get();
	return nullptr;
}

EWorldState GameWorldManager::GetWorldState(EWorldChannel channelId)
{
	std::lock_guard<std::mutex> lock(_mutex);
	auto it = _worldStates.find(channelId);
	if (it != _worldStates.end()) return it->second;
	return EWorldState::Normal;
}

std::array<FWorldState, WORLD_MAX_COUNT> GameWorldManager::GetAllWorldStates()
{
	std::lock_guard<std::mutex> lock(_mutex);
	std::array<FWorldState, WORLD_MAX_COUNT> result;

	using EC = EWorldChannel;
	int idx = 0;
	for (uint8 i = static_cast<uint8>(EC::Start); i <= static_cast<uint8>(EC::End); ++i)
	{
		EC channel = static_cast<EC>(i);
		result[idx].Channel = channel;
		result[idx].State = _worldStates.contains(channel) ? _worldStates[channel] : EWorldState::Normal;
		++idx;
	}

	return result;
}

void GameWorldManager::UpdateWorldStates()
{
	std::lock_guard<std::mutex> lock(_mutex);

	for (auto& [channel, world] : _gameWorlds)
	{
		int32 playerCount = world->GetPlayerCount();
		EWorldState newState = EWorldState::Normal;
		if (playerCount < GOOD_STATE_WORLD)
			_worldStates[channel] = EWorldState::Good;
		else if (playerCount < NORMAL_STATE_WORLD)
			_worldStates[channel] = EWorldState::Normal;
		else
			_worldStates[channel] = EWorldState::Bad;
		LOG_I("Ch{}. Player = {} ({})", static_cast<uint8>(channel), playerCount, ENUM_NAME(_worldStates[channel]));
	}
	LOG_I("=== World States Updated ===");
}