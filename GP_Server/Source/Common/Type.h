#pragma once
enum EPacketType : uint8
{
	// --- Account ---
	C_LOGIN,
	C_LOGOUT,
	C_SIGNUP,
	C_SELECT_CHARACTER,
	C_ENTER_GAME,

	S_LOGIN_SUCCESS,
	S_LOGIN_FAIL,
	S_SIGNUP_SUCCESS,
	S_SIGNUP_FAIL,
	S_ENTER_GAME,

	// --- World Channel ---
	C_CHANGE_CHANNEL,

	S_WORLD_STATE,
	S_CHANGE_CHANNEL,

	// --- Player Actions ---
	C_MOVE,
	C_ATTACK,
	C_START_AIMING,
	C_STOP_AIMING,
	C_USE_SKILL_START,
	C_USE_SKILL_END,
	C_REMOVE_STATE,

	S_PLAYER_MOVE,
	S_PLAYER_ATTACK,
	S_PLAYER_STATUS_UPDATE,
	S_DAMAGED_PLAYER,
	S_PLAYER_USE_SKILL_START,
	S_PLAYER_USE_SKILL_END,
	S_SKILL_UNLOCK,
	S_SKILL_UPGRADE,
	S_LEVEL_UP,
	S_PLAYER_DEAD,
	// --- Player Join/Leave ---
	S_ADD_PLAYER,
	S_REMOVE_PLAYER,

	// --- Monster Events ---
	S_ADD_MONSTER,
	S_REMOVE_MONSTER,
	S_MONSTER_DEAD,
	S_MONSTER_STATUS_UPDATE,
	S_DAMAGED_MONSTER,

	//--- Tino ---
	S_EARTH_QUAKE,
	S_FLAME_BREATH,

	// --- Item ---
	C_TAKE_ITEM,
	C_DROP_ITEM,// -> 나중에
	C_USE_ITEM,
	C_EQUIP_ITEM,
	C_UNEQUIP_ITEM,

	S_ITEM_SPAWN,
	S_ITEM_DESPAWN, // -> 나중에
	S_ITEM_PICKUP,
	S_ITEM_DROP, // -> 나중에
	S_ADD_INVENTORY_ITEM,
	S_REMOVE_INVENTORY_ITEM,
	S_USE_INVENTORY_ITEM,
	S_EQUIP_ITEM,
	S_UNEQUIP_ITEM,

	// --- Zone ---
	C_CHANGE_ZONE,
	C_RESPAWN,

	S_CHANGE_ZONE,
	S_RESPAWN,

	// --- Shop ---
	C_SHOP_BUY_ITEM,
	C_SHOP_SELL_ITEM,

	S_SHOP_ITEM_LIST,// -> 나중에
	S_SHOP_BUY_RESULT,
	S_SHOP_SELL_RESULT,

	// --- Quest ---
	C_REQUEST_QUEST,
	C_COMPLETE_QUEST,
	C_REJECT_QUEST,

	S_QUEST_START,
	S_QUEST_REWARD,

	//--- Chat ---
	C_CHAT_SEND,
	C_CHAT_WHISPER,
	S_CHAT_BROADCAST,

	//--- Friend ---
	C_FRIEND_REQUEST,
	C_FRIEND_ACCEPT,
	C_FRIEND_REJECT,
	C_FRIEND_REMOVE,
	S_FRIEND_OPERATION_RESULT,
	S_FRIEND_LIST,
	S_ADD_FRIEND,
	S_REMOVE_FRIEND,
	S_REQUEST_FRIEND,

	//for test
	C_FORCE_NEXT_QUEST,

	S_DEBUG_TRIANGLE,
	S_DEBUG_LINE,
};

enum class EWorldChannel : uint8
{
	None,
	Start,
	TUWorld_1 = Start,
	TUWorld_2,
	TUWorld_3,
	TUWorld_4,
	TUWorld_5,
	TUWorld_6,
	TUWorld_7,
	TUWorld_8,
	End = TUWorld_8,
};

enum class EWorldState : uint8
{
	Good,
	Normal,
	Bad,
};

struct FWorldState
{
	EWorldChannel Channel;
	EWorldState State;
	FWorldState(EWorldChannel channel = EWorldChannel::TUWorld_1, EWorldState state = EWorldState::Good)
		: Channel(channel), State(state)
	{
	}

};

enum class EChatChannel : uint8
{
	All,
	Friend,
	Zone,
	Whisper,
	ItemSys,
	SkillSys,
	FriendSys,
};

enum class EFriendOpType : uint8
{
	Request,
	Accept,
	Reject,
	Remove
};

using ECharacterType = uint8;

namespace Type
{
	enum class EPlayer : uint8
	{
		START,
		WARRIOR = START,
		GUNNER,

		END,
		NONE,
	};

	enum class EMonster : uint8
	{
		START = static_cast<uint8>(EPlayer::END),
		ENERGY_DRINK = START,
		BUBBLE_TEA,
		COFFEE,

		MOUSE,
		KEYBOARD,
		DESKMON,

		COGWHEEL,
		BOLT_NUT,
		DRILL,

		TINO,
		END
	};

	enum class ENpc : uint8
	{
		START = static_cast<uint8>(EPlayer::END),
		PROFESSOR = START,
		GUARD,
		STUDENT,
		END
	};

	enum class EWeapon : uint8
	{
		NONE,
		START = 1,

		BIRD_GUN = START,
		PULSE_GUN,
		POSITRON,

		GUN_FIRST = BIRD_GUN,
		GUN_LAST = POSITRON,

		PRAC_SWORD,
		PULSE_SWORD,
		ENERGY_SWORD,

		SWORD_FIRST = PRAC_SWORD,
		SWORD_LAST = ENERGY_SWORD,

		END = SWORD_LAST,
	};

	enum class EArmor : uint8
	{
		NONE,
		START = 10,
		ALLOY_HELMET = START,
		ENHANCED_HELMET,
		TITANIUM_ARMOR,
		POWERED_ARMOR,

		SUIT,
		TUCLOTHES,
		END = TUCLOTHES
	};

	enum class EUseable : uint8
	{
		START = 20,
		HPKIT_LOW = START,
		HPKIT_MID,
		HPKIT_HIGH,
		COFFEE,
		DDALBA,
		MANGBA,

		GOLD_SMALL,
		GOLD_MEDIUM,
		GOLD_LARGE,

		BUFFTEM_FIRST = HPKIT_LOW,
		BUFFTEM_LAST = MANGBA,
		GOLD_FIRST = GOLD_SMALL,
		GOLD_LAST = GOLD_LARGE,

		END = GOLD_LAST,
	};

	enum EQuestItem : uint8
	{
		START = 50,
		KEY = START,
		DOCUMENT,
		END = DOCUMENT
	};
}

enum class EItemCategory
{
	Weapon,
	Armor,
	Useable,
	Quest,
	Unknown
};

enum class ESkillGroup
{
	HitHard = 1,
	Clash,
	Whirlwind,
	Throwing,
	FThrowing,
	Anger,
	None
};

enum ECharacterStateType : uint32
{
	STATE_NONE = 0,
	STATE_IDLE = 1 << 0,
	STATE_RUN = 1 << 1,
	STATE_JUMP = 1 << 2,
	STATE_AUTOATTACK = 1 << 3,
	STATE_DIE = 1 << 4,
	STATE_WALK = 1 << 5,
	STATE_SKILL_Q = 1 << 6,
	STATE_SKILL_E = 1 << 7,
	STATE_SKILL_R = 1 << 8,
	STATE_AIMING = 1 << 9,
	STATE_CHASE = 1 << 10,
};

enum class ZoneType : uint8
{
	NONE = 0,
	TIP,
	TUK,
	E,
	GYM,
	INDUSTY,
	BUNKER,
};

enum class EntryType : uint8
{
	TIP_OUT,
	TIP_IN,
	E_OUT,
	E_IN,
	GYM_OUT,
	GYM_IN,
	INDUSTY_OUT,
	INDUSTY_IN,
};

enum class EQuestStatus { NotStarted, InProgress, Completed, };

enum class EQuestCategory : uint8
{
	INTERACT,
	MOVE,
	KILL,
	ITEM
};

enum class QuestType : uint8
{
	NONE = 0,

	// Chapter 1
	CH1_TALK_TO_STUDENT_A = 1,
	CH1_GO_TO_E_FIRST,
	CH1_FIND_JANITOR,
	CH1_GO_TO_BUNKER,
	CH1_BUNKER_CLEANUP,
	CH1_FIND_KEY_ITEM,

	// Chapter 2
	CH2_ENTER_E_BUILDING,
	CH2_CLEAR_E_BUILDING,
	CH2_KILL_DESKMON,

	// Chapter 3
	CH3_RETURN_TO_TIP_WITH_DOC,
	CH3_CLEAR_SERVER_ROOM,
	CH3_KILL_DRILL,

	// Chapter 4
	CH4_ENTER_GYM,
	CH4_KILL_TINO,
	CH4_GAME_CLEAR,

	// Tutorial
	TUT_START = 100,
	TUT_KILL_ONE_MON,
	TUT_USE_ITEM,
	TUT_BUY_ITEM,
	TUT_EQUIP_ITEM,
	TUT_COMPLETE
};
