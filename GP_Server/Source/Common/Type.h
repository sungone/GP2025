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
		BIRD_GUN = 1,
		PULSE_GUN,
		POSITRON,
		PRACS_WORD,
		PULSE_SWORD,
		ENERGY_SWORD,
	};

	enum class EArmor : uint8
	{
		ALLOY_HELMET = 7,
		ENHANCED_HELMET,
		TITANIUM_ARMOR,
		POWERED_ARMOR,

		SUIT = 201,
		TUCLOTHES = 202,
	};

	enum class EUseable : uint8
	{
		HPKIT_LOW = 101,
		HPKIT_MID,
		HPKIT_HIGH,

		COFFEE = 203,
		DDALBA,
		MANGBA,

		GOLD_SMALL = 401,
		GOLD_MEDIUM = 402,
		GOLD_LARGE = 403
	};

	enum class EExpItem : uint8
	{
		EXP_FIELD = 200,
		EXP_SMALL = 301,
		EXP_MEDIUM = 302,
		EXP_LARGE = 303
	};

	enum class EQuestItem : uint8
	{
		KEY = 501,
		DOCUMENT = 502
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
