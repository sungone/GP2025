#include "Monster.h"
#include "Proto.h"
#include "SessionManager.h"
#include "GameManager.h"
#include <random>
static std::random_device rd;
static std::mt19937 gen(rd());

void Monster::UpdateBehavior()
{
    switch (info.State)
    {
    case MonsterState::M_STATE_IDLE:
        if (ShouldStartWalking())
            ChangeState(MonsterState::M_STATE_WALK);
        else if (ShouldAttack())
            ChangeState(MonsterState::M_STATE_ATTACK);
        break;

    case MonsterState::M_STATE_WALK:
        if (ShouldAttack())
            ChangeState(MonsterState::M_STATE_ATTACK);
        else if (!ShouldStartWalking())
            ChangeState(MonsterState::M_STATE_IDLE);
        break;

    case MonsterState::M_STATE_ATTACK:
        if (!ShouldAttack())
            ChangeState(MonsterState::M_STATE_IDLE);
        break;

    case MonsterState::M_STATE_DIE:
        break;
    }
}

void Monster::ChangeState(MonsterState newState)
{
    std::lock_guard<std::mutex> lock(GameManager::GetInst().monsterMutex);

    if (info.State != newState)
    {
        std::cout << "[State Change] Monster " << info.ID
            << " from " << static_cast<uint32_t>(info.State)
            << " to " << static_cast<uint32_t>(newState) << std::endl;

        info.State = newState;
        info.State = static_cast<uint32_t>(info.State);
    }
}

bool Monster::ShouldStartWalking()
{
    static std::uniform_int_distribution<int> dist(0, 2); 
    return dist(gen) == 0; 
}

bool Monster::ShouldAttack()
{
    static std::uniform_int_distribution<int> dist(0, 4); 
    return dist(gen) == 0; 
}
