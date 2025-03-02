#pragma once

class IOCP
{
public:
	static IOCP& GetInst()
	{
		static IOCP inst;
		return inst;
	}
	~IOCP()
	{
		if (_hIOCP) {
			CloseHandle(_hIOCP);
			_hIOCP = nullptr;
		}
	}

	bool Init();
	void RegisterSocket(SOCKET& _socket, ULONG_PTR key = NULL);
	bool GetCompletion(DWORD& rw_byte, ULONG_PTR& key, LPWSAOVERLAPPED& over);
	void PostCompletion(ULONG_PTR key, DWORD bytesTransferred = 0, LPWSAOVERLAPPED over = nullptr);
private:
	HANDLE _hIOCP;
};
