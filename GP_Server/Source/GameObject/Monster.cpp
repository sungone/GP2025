#include "Monster.h"
#include "Proto.h"
#include "SessionManager.h"

void Monster::UpdateBehavior()
{
    switch (info.State)
    {
    case MonsterState::M_STATE_IDLE:
        std::cout << "Monster is idling." << std::endl;
        if (ShouldStartWalking())
            ChangeState(MonsterState::M_STATE_WALK);
        else if (ShouldAttack())
            ChangeState(MonsterState::M_STATE_ATTACK);
        break;

    case MonsterState::M_STATE_WALK:
        std::cout << "Monster is walking." << std::endl;
        if (ShouldAttack())
            ChangeState(MonsterState::M_STATE_ATTACK);
        else if (!ShouldStartWalking())
            ChangeState(MonsterState::M_STATE_IDLE);
        break;

    case MonsterState::M_STATE_ATTACK:
        std::cout << "Monster is attacking!" << std::endl;
        if (!ShouldAttack())
            ChangeState(MonsterState::M_STATE_IDLE);
        break;

    case MonsterState::M_STATE_DIE:
        std::cout << "Monster is dead." << std::endl;
        break;
    }
}

void Monster::ChangeState(MonsterState newState)
{
    if (info.State != newState)
    {
        std::cout << "[State Change] Monster " << info.ID
            << " from " << static_cast<uint32_t>(info.State)
            << " to " << static_cast<uint32_t>(newState) << std::endl;

        info.State = newState;
        info.State = static_cast<uint32_t>(info.State);

        //FMonsterStateData StateData = { info.ID , static_cast<uint32_t>(currentState) };
        //MonsterStatePacket packet(S_MONSTER_STATUS_UPDATE, StateData);

        //SessionManager::GetInst().Broadcast(&packet);
        //std::cout << "Monster " << info.ID << " changed state to " << static_cast<uint32_t>(newState) << "." << std::endl;
    }
}
