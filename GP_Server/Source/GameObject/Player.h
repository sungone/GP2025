#pragma once
#include "Character.h"

class Player :public Character
{
	//todo :
	// - session���� ���� �и��ϱ� (���� �����. ���� ����.)
	//		+ sessions/players -> shard_ptr�� ���������ϱ�
	// - ���� �� Ŭ����(Warrior/Gunner) ����� �̾�(!=���)
private:
	bool bLogin = false;
};

