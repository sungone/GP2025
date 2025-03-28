#include "pch.h"
#include "Monster.h"
#include "SessionManager.h"
#include "GameWorld.h"

void Monster::Init()
{
	Character::Init();
	_characterType = CharacterType::Monster;

	FVector newPos{};
	do { newPos = MapZone::GetInst().GetRandomPos(ZoneType::PLAYGROUND); } while (GameWorld::GetInst().IsCollisionDetected(newPos));
	_info.SetLocation(newPos);

	_info.CharacterType = RandomUtils::GetRandomUint8((uint8)Type::EMonster::ENERGY_DRINK, (uint8)Type::EMonster::TINO);
	_info.Stats.Level = _info.CharacterType;
	_info.CollisionRadius = 100.f;
	_info.AttackRadius = 200;
	_info.State = ECharacterStateType::STATE_IDLE;
}

void Monster::UpdateViewList(std::shared_ptr<Character> other)
{
	if (!other) { LOG(Warning, "Invaild!"); return; }
	auto player = std::dynamic_pointer_cast<Player>(other);
	if (!player) return;
	auto playerId = other->GetInfo().ID;

	if (IsInViewDistance(other->GetInfo().Pos, VIEW_DIST))
	{
		if (!AddToViewList(playerId)) return;
		player->AddMonsterToViewList(std::shared_ptr<Character>(this));
	}
	else
	{
		if (!RemoveFromViewList(playerId)) return;
		player->RemoveMonsterFromViewList(std::shared_ptr<Character>(this));
	}
}

void Monster::Update()
{
	if (IsDead())
	{
		ChangeState(ECharacterStateType::STATE_DIE);
		return;
	}

	BehaviorTree();
}

void Monster::BehaviorTree()
{
	switch (_info.State)
	{
	case ECharacterStateType::STATE_IDLE:
		if (IsTargetDetected())
		{
			ChangeState(ECharacterStateType::STATE_AUTOATTACK);
			Attack();
		}
		//else if (RandomUtils::GetRandomBool())
		//{
		//	ChangeState(ECharacterStateType::STATE_WALK);
		//}
		break;
	case ECharacterStateType::STATE_AUTOATTACK:
		if (!_target)
		{
			ChangeState(ECharacterStateType::STATE_IDLE);
			break;
		}

		if (!IsTargetInRange())
		{
			_target.reset();
			ChangeState(ECharacterStateType::STATE_IDLE);
		}
		else
		{
			Attack();
		}
		break;
	default:
		LOG(Warning, std::format("Invaild State :{}", _info.State));
	}
}

void Monster::Attack()
{
	if (!_target) return;

	auto player = _target;
	auto playerID = player->GetInfo().ID;

	float atkDamage = this->GetAttackDamage();
	if (atkDamage > 0.0f)
	{
		player->OnDamaged(atkDamage);
	}

	auto infopkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, player->GetInfo());
	SessionManager::GetInst().SendPacket(playerID, &infopkt);
	SessionManager::GetInst().BroadcastToViewList(&infopkt, playerID);

	if (player->IsDead())
	{
		//Todo: 플레이어 죽음처리
	}
}

void Monster::Move()
{

}

bool Monster::IsTargetDetected()
{
	return GameWorld::GetInst().FindTarget(_id);
}

bool Monster::IsTargetInRange()
{
	if (_target != nullptr)
	{
		return this->IsInAttackRange(_target->GetInfo());
	}
	return false;
}
