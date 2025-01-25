#include "Monster.h"
#include "Proto.h"
#include "SessionManager.h"
#include "GameManager.h"
#include <random>
static std::random_device rd;
static std::mt19937 gen(rd());

void Monster::Init()
{
    Character::Init();
    static int i = 0;
    info.ID = i++;
    info.CharacterType = M_MOUSE;
    info.Speed = 200.f;
    info.State = ECharacterStateType::STATE_IDLE;
}

void Monster::UpdateBehavior()
{
    switch (info.State)
    {
    case ECharacterStateType::STATE_IDLE:
        if (ShouldStartWalking())
        {
            ChangeState(ECharacterStateType::STATE_WALK);
        }
        else if (ShouldAttack())
        {
            ChangeState(ECharacterStateType::STATE_AUTOATTACK);
        }
        break;

    case ECharacterStateType::STATE_WALK:
        if (ShouldAttack())
        {
            ChangeState(ECharacterStateType::STATE_AUTOATTACK);
        }
        else if (!ShouldStartWalking())
        {
            ChangeState(ECharacterStateType::STATE_IDLE);
        }
        break;

    case ECharacterStateType::STATE_AUTOATTACK:
        if (!ShouldAttack())
        {
            ChangeState(ECharacterStateType::STATE_IDLE);
        }
        break;

    case ECharacterStateType::STATE_DIE:
        break;
    }
}

void Monster::ChangeState(ECharacterStateType newState)
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
    static std::uniform_real_distribution<float> distX(-1000.0f, 1000.0f); 
    static std::uniform_real_distribution<float> distY(-1000.0f, 1000.0f); 
    if (dist(gen) == 0)
    {
        float newX = info.X + distX(gen);
        float newY = info.Y + distY(gen);

        float deltaX = newX - info.X;
        float deltaY = newY - info.Y;
        float newYaw = std::atan2(deltaY, deltaX) * (180.0f / 3.14159265f); 

        info.SetLocation(newX, newY, info.Z);
        info.Yaw = newYaw;

        std::cout << "Monster moved to new location: (" << newX << ", " << newY << ", " << info.Z << ")" << std::endl;
        return true;
    }
    
    return false;
}

bool Monster::ShouldAttack()
{
    static std::uniform_int_distribution<int> dist(0, 4); 
    if (dist(gen) == 0)
    {
        return true;
    }

    return false;
}
