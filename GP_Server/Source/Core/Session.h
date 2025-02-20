#pragma once
#include "Player.h"

//Todo: 주체객체가 아닌 컴포넌트 형태로 바꾸고자 함
// 더미클라에서도
class Session
{
public:
	Session() = default;
	~Session() { closesocket(_socket); }

public:
	void DoRecv();
	void DoSend(Packet* packet);

	void Connect(SOCKET& socket, int32 id);
	void Disconnect();

	void SetLogin();
	bool IsLogin();
	int32 GetId();
	int32 GetRemainSize();
	void SetRemainSize(int32 size);
	FInfoData& GetPlayerInfo();
private:
	//Todo: 로그인 bool로 관리하지 말고
	// sessionmgr가 로그인하면 
	// nullptr -> player 객체 가지게 하는걸로 구분하면 되지 않을까...
	bool _bLogin = false;
	int32 _id = -1;
	std::shared_ptr<Player> _player;
	SOCKET _socket = INVALID_SOCKET;
	//Todo: recv관리하는걸 분리하면 좋을 것 같다 
	ExpOver _recvOver;
	int32 _remain;

	std::mutex _sMutex;
};
