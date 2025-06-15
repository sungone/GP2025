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
#ifdef SERVER_BUILD
enum EQuestCategory { INTERACT, MOVE, KILL, ITEM };
#else
UENUM(BlueprintType)
enum class EQuestCategory : uint8
{
	INTERACT  UMETA(DisplayName = "Interact"),
	MOVE      UMETA(DisplayName = "Move"),
	KILL      UMETA(DisplayName = "Kill"),
	ITEM      UMETA(DisplayName = "Item")
};
#endif
#ifdef SERVER_BUILD
enum class QuestType : uint8
{
	NONE,
	CH1_TALK_TO_STUDENT_A,        // 1. 학생 A와 대화
	CH1_GO_TO_E_FIRST,            // 2. E동으로 먼저 이동
	CH1_FIND_JANITOR,             // 3. 경비 아저씨 찾기
	CH1_GO_TO_BUNKER,             // 4. 벙커로 이동
	CH1_BUNKER_CLEANUP,           // 5. 벙커 청소
	CH1_FIND_KEY_ITEM,            // 6. 열쇠 아이템 찾기
	CH1_ENTER_E_BUILDING,         // 7. E동 입장
	CH1_CLEAR_E_BUILDING,         // 8. E동 보스 제거
	CH1_RETURN_TO_TIP_WITH_DOC,   // 9. 문서 가지고 TIP으로 돌아가기
	CH2_CLEAR_SERVER_ROOM,        // 10. 서버룸 클리어
	CH3_ENTER_GYM,                // 11. 체육관 입장
	CH3_KILL_TINO,                // 12. 최종보스 Tino 제거

	TUT_MOVE = 100,				// 100 - TIP 밖으로 이동하기
	TUT_KILL_ONE_MON, 			// 101 - 몬스터 한 마리 처치하기
	TUT_USE_ITEM, 				// 102 - 회복아이템 사용해보기
	TUT_BUY_ITEM, 				// 103 - 장비 아이템 구매하기
	TUT_EQUIP_ITEM, 			// 104 - 장비 아이템 착용하기
	TUT_COMPLETE, 				// 105 - 튜토리얼 완료
};
#else
UENUM(BlueprintType)
enum class QuestType : uint8
{
	NONE                      UMETA(DisplayName = "None"),
	CH1_TALK_TO_STUDENT_A     UMETA(DisplayName = "Talk to Student A"),
	CH1_GO_TO_E_FIRST         UMETA(DisplayName = "Go to E building first"),
	CH1_FIND_JANITOR          UMETA(DisplayName = "Find Janitor"),
	CH1_GO_TO_BUNKER          UMETA(DisplayName = "Go to Bunker"),
	CH1_BUNKER_CLEANUP        UMETA(DisplayName = "Bunker Cleanup"),
	CH1_FIND_KEY_ITEM         UMETA(DisplayName = "Find Key Item"),
	CH1_ENTER_E_BUILDING      UMETA(DisplayName = "Enter E Building"),
	CH1_CLEAR_E_BUILDING      UMETA(DisplayName = "Clear E Building"),
	CH1_RETURN_TO_TIP_WITH_DOC UMETA(DisplayName = "Return to TIP with Document"),
	CH2_CLEAR_SERVER_ROOM     UMETA(DisplayName = "Clear Server Room"),
	CH3_ENTER_GYM             UMETA(DisplayName = "Enter Gym"),
	CH3_KILL_TINO             UMETA(DisplayName = "Kill Tino"),
	// 튜토리얼 퀘스트 (100번대)
	TUT_MOVE                      UMETA(DisplayName = "Move Outside TIP"),
	TUT_KILL_ONE_MON              UMETA(DisplayName = "Kill One Monster"),
	TUT_USE_ITEM                  UMETA(DisplayName = "Use Recovery Item"),
	TUT_BUY_ITEM                  UMETA(DisplayName = "Buy an Item"),
	TUT_EQUIP_ITEM                UMETA(DisplayName = "Equip an Item"),
	TUT_COMPLETE                  UMETA(DisplayName = "Complete Tutorial"),

};
#endif
 
