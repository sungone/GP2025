#pragma once
#include "Define.h"

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
		if (hIOCP) {
			CloseHandle(hIOCP);
			hIOCP = nullptr;
		}
	}

	bool Init();
	void RegisterSocket(SOCKET& socket, ULONG_PTR key = NULL);
	bool GetCompletion(DWORD& rw_byte, ULONG_PTR& key, LPWSAOVERLAPPED& over);

private:
	HANDLE hIOCP;
};
