#include "IOCP.h"

bool IOCP::Init()
{
	_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (_hIOCP == nullptr) {
		return false;
	}
	return true;
}

void IOCP::RegisterSocket(SOCKET& _socket, ULONG_PTR key)
{
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(_socket), _hIOCP, key, 0);
}

bool IOCP::GetCompletion(DWORD& rw_byte, ULONG_PTR& key, LPWSAOVERLAPPED& over)
{
	return GetQueuedCompletionStatus(_hIOCP, &rw_byte, &key, &over, INFINITE);
}