#pragma once
#include "Player.h"
#include "SessionSocket.h"
#include "JobQueue.h"

enum class SessionState { None, LoggedIn, InGame };

class PlayerSession
{
public:
	PlayerSession() = default;
	~PlayerSession()
	{
		if (IsLogin()) Disconnect();
	}

public:
	void DoRecv();
	void DoSend(const Packet* packet);
	void HandleRecvBuffer(int32 recvByte, ExpOver* expOver);

	void Connect(SOCKET socket, int32 id);
	void Disconnect();

	void Login(const DBLoginResult& dbRes);
	void EnterGame();
	void Logout();
	bool IsLogin() { return _state != SessionState::None; }
	bool IsInGame() { return _state == SessionState::InGame; }
	int32 GetId();
	FInfoData& GetPlayerInfo();

	SessionState GetSessionState()
	{
		std::lock_guard<std::mutex> lock(_sMutex);
		return _state;
	}

	void SetSessionState(SessionState newState)
	{
		std::lock_guard<std::mutex> lock(_sMutex);
		_state = newState;
	}
	std::shared_ptr<Player> GetPlayer();

	void PushJob(std::function<void()> job)
	{
		_gameJobQueue.Push(std::move(job));
	}

	void RunGameJobs() {
		_gameJobQueue.Run();
	}

private:
	JobQueue _gameJobQueue;
	int32 _id = -1;
	uint32 _dbId;
	std::shared_ptr<Player> _player;
	std::unique_ptr<SessionSocket> _sSocket;
	SessionState _state = SessionState::None;

	std::mutex _sMutex;
};
