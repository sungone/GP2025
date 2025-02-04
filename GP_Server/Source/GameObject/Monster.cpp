#include "pch.h"
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
    _info.CharacterType = M_MOUSE;
}

void Monster::Update()
{
   // std::lock_guard<std::mutex> lock(_cMutex);
    switch (_info.State)
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
    }
}

void Monster::ChangeState(ECharacterStateType newState)
{
    std::lock_guard<std::mutex> lock(_cMutex);

    if (_info.State != newState)
    {
        LOG(Log, std::format("monster [{}] - state from {} to {}",
            _info.ID,
            static_cast<uint32_t>(_info.State),
            static_cast<uint32_t>(newState)));
        _info.State = newState;
        _info.State = static_cast<uint32_t>(_info.State);
    }
}

bool Monster::ShouldStartWalking()
{
    std::lock_guard<std::mutex> lock(_cMutex);
    static std::uniform_int_distribution<int> dist(0, 2);
    static std::uniform_real_distribution<float> distX(-1000.0f, 1000.0f); 
    static std::uniform_real_distribution<float> distY(-1000.0f, 1000.0f); 
    if (dist(gen) == 0)
    {
        float newX = _info.X + distX(gen);
        float newY = _info.Y + distY(gen);

        float deltaX = newX - _info.X;
        float deltaY = newY - _info.Y;
        float newYaw = std::atan2(deltaY, deltaX) * (180.0f / 3.14159265f); 

        _info.SetLocation(newX, newY, _info.Z);
        _info.Yaw = newYaw;

        LOG(Log, std::format("Monster move ({}, {}, {})", newX, newY, _info.Z));
        return true;
    }
    
    return false;
}

bool Monster::ShouldAttack()
{
    static std::uniform_int_distribution<int> dist(0, 1); 
    if (dist(gen) == 0)
    {
        return true;
    }

    return false;
}