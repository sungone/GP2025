#pragma once

enum EPacketType : uint8
{
	C_LOGIN,
	C_LOGOUT,
	C_MOVE,
	C_ATTACK,

	S_LOGIN_SUCCESS,

	S_ADD_PLAYER,
	S_REMOVE_PLAYER,
	S_PLAYER_STATUS_UPDATE,

	S_ADD_MONSTER,
	S_REMOVE_MONSTER,
	S_MONSTER_STATUS_UPDATE,
	S_DAMAGED_MONSTER,

	C_USE_ITEM,
	C_DROP_ITEM,
	S_ADD_ITEMS,
	S_ADD_ITEM,
	S_REMOVE_ITEM,
};

using ECharacterType = uint8;

namespace Type
{
	enum EPlayer : uint8
	{
		WARRIOR,
		GUNNER,

		P_END
	};

	enum EMonster : uint8
	{
		ENERGY_DRINK = P_END,
		BUBBLE_TEA,
		COFFEE,

		MOUSE,
		KEYBOARD,
		DESKTOP,

		COGWHEEL,
		BOLT_NUT,
		DRILL,

		TINO,

		M_END
	};

	enum ENpc : uint8
	{
		PROFESSOR = M_END,
		GUARD,
		STUDENT,
	};

	enum class EWeapon : uint8
	{
		BIRD_GUN,
		PULSE_GUN,
		POSITRON,
		PRACS_WORD,
		PULSE_SWORD,
		ENERGY_SWORD,
	};

	enum class EArmor : uint8
	{
		ALLOY_HELMET,
		ENHANCED_HELMET,
		TITANIUM_ARMOR,
		POWERED_ARMOR,

		SUIT,
		TUCLOTHES
	};

	enum class EConsumable : uint8
	{
		HPKIT_LOW,
		HPKIT_MID,
		HPKIT_HIGH,

		COFFEE,
		DDALBA,
		MANGBA,
	};
}

enum ECharacterStateType : uint8
{
	STATE_NONE = 0, // 2^0
	STATE_IDLE = 1 << 0,  // 2^1
	STATE_RUN = 1 << 1,  // 2^2
	STATE_JUMP = 1 << 2,  // 2^3
	STATE_AUTOATTACK = 1 << 3, // 2^4
	STATE_DIE = 1 << 4,
	STATE_WALK = 1 << 5,
};
