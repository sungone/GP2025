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

	S_PLAYER_MOVE,
	S_PLAYER_STATUS_UPDATE,
	S_DAMAGED_PLAYER,
	S_PLAYER_USE_SKILL,
	S_SKILL_UNLOCK,
	S_SKILL_UPGRADE,
	S_LEVEL_UP,

	// --- Player Join/Leave ---
	S_ADD_PLAYER,
	S_REMOVE_PLAYER,

	// --- Monster Events ---
	S_ADD_MONSTER,
	S_REMOVE_MONSTER,
	S_MONSTER_STATUS_UPDATE,
	S_DAMAGED_MONSTER,
	S_PLAYER_DEAD,

	// --- Item ---
	C_TAKE_ITEM,
	C_DROP_ITEM,
	C_USE_ITEM,
	C_EQUIP_ITEM,
	C_UNEQUIP_ITEM,

	S_ITEM_SPAWN,
	S_ITEM_DESPAWN,
	S_ITEM_PICKUP,
	S_ITEM_DROP,
	S_ADD_INVENTORY_ITEM,
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

	S_SHOP_ITEM_LIST,
	S_SHOP_BUY_RESULT,
	S_SHOP_SELL_RESULT,

	// --- Quest ---
	C_REQUEST_QUEST,
	C_COMPLETE_QUEST,

	S_QUEST_REWARD,
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
		START = EPlayer::END,
		ENERGY_DRINK = START,
		BUBBLE_TEA,
		COFFEE,

		MOUSE,
		KEYBOARD,
		DESKTOP,

		COGWHEEL,
		BOLT_NUT,
		DRILL,

		TINO,
		END
	};

	enum class ENpc : uint8
	{
		START = EMonster::END,
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
		END
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
	STATE_NONE = 0, // 2^0
	STATE_IDLE = 1 << 0,  // 2^1
	STATE_RUN = 1 << 1,  // 2^2
	STATE_JUMP = 1 << 2,  // 2^3
	STATE_AUTOATTACK = 1 << 3, // 2^4
	STATE_DIE = 1 << 4,
	STATE_WALK = 1 << 5,
	STATE_SKILL_Q = 1 << 6,
	STATE_SKILL_E = 1 << 7,
	STATE_SKILL_R = 1 << 8,
	STATE_AIMING = 1 << 9,
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
	PLAYGROUND,//test용
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
enum EQuestCategory { INTERACT, MOVE, KILL, ITEM };

enum class QuestType : uint8
{
	NONE,
	CH1_GO_TO_E_BUILDING,         // 1. E동으로 이동
	CH1_RETURN_TO_TIP,            // 2. TIP으로 돌아가기
	CH1_TALK_TO_STUDENT_A,        // 3. 학생 A와 대화
	CH1_FIND_JANITOR,             // 4. 경비아저씨 찾기
	CH2_GO_TO_SANYUNG,            // 5. 산융으로 이동
	CH2_CLEAR_SANYUNG,            // 6. 산융 몬스터 제거
	CH2_FIND_KEY,                 // 7. 열쇠 획득
	CH1_ENTER_E_BUILDING,         // 8. E동 입장
	CH1_CLEAR_E_BUILDING,         // 9. E동 보스 제거
	CH1_FIND_DOCUMENT,            // 10. 문서 획득
	CH1_RETURN_WITH_DOCUMENT,     // 11. 문서 가지고 TIP로 돌아가기
	CH2_CLEAR_SERVER_ROOM,        // 12. 서버룸 클리어
	CH3_ENTER_GYM,                // 13. 체육관 입장
	CH3_KILL_TINO,                // 14. 최종보스 제거
	MAX
};