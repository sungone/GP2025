#pragma once
#include "GameObject.h"

class Player :public GameObject
{
	//todo :
	// - session에서 내용 분리하기 (연결 끊김시. 정보 보존.)
	//		+ sessions/players -> shard_ptr로 구조변경하기
	// - 역할 별 클래스(Warrior/Gunner) 만들어 이양(!=상속)

};

