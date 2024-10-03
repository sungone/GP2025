#include "Define.h"
#include "Session.h"

extern std::array<Session, MAX_CLIENT> clients;

void Session::process_packet(DWORD transfer_size)
{
	FMovePacket ReceivedPacket;
	memcpy(&ReceivedPacket, recv_over_.send_buf, sizeof(FMovePacket));

	std::cout << "Transfer size: " << transfer_size << std::endl;
	std::cout << "Received from Client: PlayerID = " << ReceivedPacket.PlayerID
		<< ", Position = (" << ReceivedPacket.X << ", " << ReceivedPacket.Y << ", " << ReceivedPacket.Z << ")"
		<< ", Rotation = (Yaw: " << ReceivedPacket.Yaw << ", Pitch: " << ReceivedPacket.Pitch << ", Roll: " << ReceivedPacket.Roll << ")" << std::endl;

	FMovePacket ResponsePacket;
	ResponsePacket.Header.PacketType = EPacketType::C_MOVE;
	ResponsePacket.Header.PacketSize = sizeof(FMovePacket);
	ResponsePacket.PlayerID = ReceivedPacket.PlayerID;

	char SendBuffer[sizeof(FMovePacket)];
	memcpy(SendBuffer, &ResponsePacket, sizeof(FMovePacket));

	WSABUF wsabuf[1];
	wsabuf[0].buf = SendBuffer;
	wsabuf[0].len = sizeof(SendBuffer);

	for (auto& ss : clients)
		ss.do_send(id, recv_over_.send_buf, transfer_size);
	do_recv();
}
