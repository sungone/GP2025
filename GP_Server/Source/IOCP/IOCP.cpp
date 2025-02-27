#include "pch.h"
#include "IOCP.h"

bool IOCP::Init()
{
	_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	return _hIOCP != nullptr;
}

void IOCP::RegisterSocket(SOCKET& _socket, ULONG_PTR key)
{
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(_socket), _hIOCP, key, 0);
}

bool IOCP::GetCompletion(DWORD& rw_byte, ULONG_PTR& key, LPWSAOVERLAPPED& over)
{
	return GetQueuedCompletionStatus(_hIOCP, &rw_byte, &key, &over, INFINITE);
}

void IOCP::PostCompletion(ULONG_PTR key, DWORD bytesTransferred, LPWSAOVERLAPPED over)
{
	PostQueuedCompletionStatus(_hIOCP, bytesTransferred, key, over);
}
