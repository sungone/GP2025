#pragma once

class Server
{
public:
	static Server& GetInst()
	{
		static Server inst;
		return inst;
	}
	~Server() { Close(); }

	bool Init();
	void Run();
	void Close();

protected:
	void InitSocket(SOCKET& socket, DWORD dwFlags);

	void WorkerThreadLoop();
	void HandleCompletionError(ExpOver* ex_over, int32 _id);
	void DoAccept();
	void HandleAccept();
	void HandleRecv(int32 id, int32 recvByte, ExpOver* over);
	void HandSend(int32 id, ExpOver* over);

protected:
	bool _bRunning = true;
	SOCKET _listenSocket;
	SOCKET _acceptSocket;

	ExpOver _acceptOver;
};
