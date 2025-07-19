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
	void OnRecv(int32 recvByte, ExpOver* expOver);
	void Connect(SOCKET socket, int32 id);
	void Disconnect();

	void Login(const DBLoginResult& dbRes);
	void EnterGame(EWorldChannel wchannel);
	void Logout();
	bool IsLogin() { return _state != SessionState::None; }
	bool IsInGame() { return _state == SessionState::InGame; }
	int32 GetId();
	FInfoData& GetPlayerInfo();
	void SetAndSendFriendsInfo(std::vector<FFriendInfo> friends);
	std::shared_ptr<Player> GetPlayer();

	void PushJob(std::function<void()> job)
	{
		_gameJobQueue.Push(std::move(job));
	}

	void RunGameJobs() {
		_gameJobQueue.Run();
	}

	uint32 GetDBID() { return _dbId; };
	void SetDBID(uint32 dbId) { _dbId = dbId; }
	std::string GetNickName() const { return _nickName; }
	void SetNickName(const std::string& nickName) { _nickName = nickName; }

	const std::vector<FFriendInfo>& GetFriends() const { return _friends; }
	void AddFriend(const FFriendInfo& friendInfo)
	{
		_friends.push_back(friendInfo);
	}
	void RemoveFriend(uint32 dbId)
	{
		_friends.erase(std::remove_if(_friends.begin(), _friends.end(),
			[dbId](const FFriendInfo& f) { return f.DBId == dbId; }), _friends.end());
	}
	EWorldChannel GetWorldChannel() const { return _wChannel; }
private:
	JobQueue _gameJobQueue;
	int32 _id = -1;
	uint32 _dbId;
	std::string _nickName;

	std::shared_ptr<Player> _player;
	std::unique_ptr<SessionSocket> _sSocket;
	SessionState _state = SessionState::None;
	EWorldChannel _wChannel = EWorldChannel::None;
	std::mutex _sMutex;
	std::vector<FFriendInfo> _friends;
};
