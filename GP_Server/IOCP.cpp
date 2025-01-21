#include "IOCP.h"

bool IOCP::Init()
{
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (hIOCP == nullptr) {
		return false;
	}
	return true;
}

void IOCP::RegisterSocket(SOCKET& socket, ULONG_PTR key)
{
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket), hIOCP, key, 0);
}

bool IOCP::GetCompletion(DWORD& rw_byte, ULONG_PTR& key, LPWSAOVERLAPPED& over)
{
	return GetQueuedCompletionStatus(hIOCP, &rw_byte, &key, &over, INFINITE);
}