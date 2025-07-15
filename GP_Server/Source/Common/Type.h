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

	// --- Player Actions ---
	C_MOVE,
	C_ATTACK,
	C_START_AIMING,
	C_STOP_AIMING,
	C_USE_SKILL,
	C_REMOVE_STATE,

	S_PLAYER_MOVE,
	S_PLAYER_STATUS_UPDATE,
	S_DAMAGED_PLAYER,
	S_PLAYER_USE_SKILL,
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

	S_QUEST_START,
	S_QUEST_REWARD,

	//--- Chat ---
	C_CHAT_SEND,
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
	S_DEBUG_TRIANGLE,
	S_DEBUG_LINE
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

#ifdef SERVER_BUILD
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


#else
UENUM(BlueprintType)
enum class ZoneType : uint8
{
	NONE UMETA(DisplayName = "None"),
	TIP  UMETA(DisplayName = "Tip"),
	TUK  UMETA(DisplayName = "TUK"),
	E    UMETA(DisplayName = "E"),
	GYM  UMETA(DisplayName = "Gym"),
	INDUSTY UMETA(DisplayName = "Industry")
};
#endif
enum class EQuestStatus { NotStarted, InProgress, Completed, };

#ifdef SERVER_BUILD
enum class EQuestCategory : uint8
{
	INTERACT,
	MOVE,
	KILL,
	ITEM
};
#else
UENUM(BlueprintType)
enum class EQuestCategory : uint8
{
	INTERACT   UMETA(DisplayName = "Interact"),
	MOVE       UMETA(DisplayName = "Move"),
	KILL       UMETA(DisplayName = "Kill"),
	ITEM       UMETA(DisplayName = "Item")
};
#endif

#ifdef SERVER_BUILD
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

	// Tutorial
	TUT_MOVE = 100,
	TUT_KILL_ONE_MON,
	TUT_USE_ITEM,
	TUT_BUY_ITEM,
	TUT_EQUIP_ITEM,
	TUT_COMPLETE
};
#else
UENUM(BlueprintType)
enum class QuestType : uint8
{
	NONE                      = 0       UMETA(DisplayName = "None"),

	// Chapter 1
	CH1_TALK_TO_STUDENT_A     = 1       UMETA(DisplayName = "Talk to Student A"),
	CH1_GO_TO_E_FIRST         = 2       UMETA(DisplayName = "Go to E First"),
	CH1_FIND_JANITOR          = 3       UMETA(DisplayName = "Find Janitor"),
	CH1_GO_TO_BUNKER          = 4       UMETA(DisplayName = "Go to Bunker"),
	CH1_BUNKER_CLEANUP        = 5       UMETA(DisplayName = "Bunker Cleanup"),
	CH1_FIND_KEY_ITEM         = 6       UMETA(DisplayName = "Find Key Item"),

	// Chapter 2
	CH2_ENTER_E_BUILDING      = 7       UMETA(DisplayName = "Enter E Building"),
	CH2_CLEAR_E_BUILDING      = 8       UMETA(DisplayName = "Clear E Building"),
	CH2_KILL_DESKMON          = 9       UMETA(DisplayName = "Kill Deskmon"),

	// Chapter 3
	CH3_RETURN_TO_TIP_WITH_DOC = 10     UMETA(DisplayName = "Return to TIP with Document"),
	CH3_CLEAR_SERVER_ROOM     = 11      UMETA(DisplayName = "Clear Server Room"),
	CH3_KILL_DRILL            = 12      UMETA(DisplayName = "Kill Drill"),

	// Chapter 4
	CH4_ENTER_GYM             = 13      UMETA(DisplayName = "Enter Gym"),
	CH4_KILL_TINO             = 14      UMETA(DisplayName = "Kill Tino"),

	// Tutorial
	TUT_MOVE                  = 100     UMETA(DisplayName = "Tutorial - Move"),
	TUT_KILL_ONE_MON          = 101     UMETA(DisplayName = "Tutorial - Kill One Monster"),
	TUT_USE_ITEM              = 102     UMETA(DisplayName = "Tutorial - Use Item"),
	TUT_BUY_ITEM              = 103     UMETA(DisplayName = "Tutorial - Buy Item"),
	TUT_EQUIP_ITEM            = 104     UMETA(DisplayName = "Tutorial - Equip Item"),
	TUT_COMPLETE              = 105     UMETA(DisplayName = "Tutorial - Complete")
};

#endif
