#include "pch.h"
#include "Monster.h"
#include "SessionManager.h"
#include "GameWorld.h"

void Monster::Init()
{
	Character::Init();

	auto data = MonsterTable::GetInst().GetMonsterByTypeId(static_cast<uint8>(_monType));
	if (!data)
	{
		LOG(Warning, "Invaild");
		return;
	}
	FVector newPos{};

	//_info.SetName(ConvertToWString(data->Name) + std::to_wstring(_id));
	_info.SetName(ConvertToWString(data->Name));
	_info.CharacterType = data->TypeId;
	_info.Stats.Level = data->Level;
	_info.Stats.Hp = data->Hp;
	_info.Stats.MaxHp = data->Hp;
	_info.Stats.Damage = data->Atk;
	_info.Stats.CrtRate = data->CrtRate;
	_info.Stats.Dodge = data->Dodge;
	_info.Stats.Speed = data->MoveSpd;
	_info.CollisionRadius = data->CollisionRadius;
	_info.AttackRadius = data->AtkRadius;
	_info.State = ECharacterStateType::STATE_IDLE;

}

void Monster::UpdateViewList(std::shared_ptr<Character> other)
{
	if (!other) { LOG(Warning, "Invaild!"); return; }
	auto player = std::dynamic_pointer_cast<Player>(other);
	if (!player) return;
	auto playerId = other->GetInfo().ID;
	auto self = std::static_pointer_cast<Character>(shared_from_this());

	bool added = false;
	{
		std::lock_guard lock(_vlLock);
		if (IsInViewDistance(other->GetInfo().Pos, VIEW_DIST))
			added = AddToViewList(playerId);
		else
			added = !RemoveFromViewList(playerId);
	}

	if (!added) return;

	if (IsInViewDistance(other->GetInfo().Pos, VIEW_DIST))
		player->AddMonsterToViewList(self);
	else
		player->RemoveMonsterFromViewList(self);
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
	{
		if (SetTarget())
		{
			Chase();
			ChangeState(ECharacterStateType::STATE_WALK);
		}
		else if (RandomUtils::GetRandomBool())
		{
			ChangeState(ECharacterStateType::STATE_WALK);
		}
		break;
	}
	case ECharacterStateType::STATE_WALK:
	{
		if (_target)
		{
			Chase();
			break;
		}

		if (SetTarget())
		{
			Chase();
		}
		else
		{
			Patrol();
		}
		break;
	}
	case ECharacterStateType::STATE_AUTOATTACK:
	{
		if (!_target)
		{
			ChangeState(ECharacterStateType::STATE_IDLE);
			break;
		}

		if (!IsTargetInAttackRange())
		{
			ChangeState(ECharacterStateType::STATE_WALK);
			Chase();
		}
		else
		{
			Attack();
		}
		break;
	}
	default:
		LOG(Warning, std::format("Invaild State :{}", _info.State));
	}
}

void Monster::Look()
{
	if (!_target) return;
	float yaw = _info.CalculateYaw(_target->GetInfo().Pos);
	_info.SetYaw(yaw);
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

	auto pkt = InfoPacket(EPacketType::S_DAMAGED_PLAYER, player->GetInfo());
	SessionManager::GetInst().SendPacket(playerID, &pkt);

	if (player->IsDead())
	{
		auto pkt = PlayerDeadPacket(playerID);
		std::unordered_set<int32> vlist;
		{
			std::lock_guard lock(player->_vlLock);
			vlist = player->GetViewList();
		}
		SessionManager::GetInst().SendPacket(playerID, &pkt);
		SessionManager::GetInst().BroadcastToViewList(&pkt, vlist);
	}
}

void Monster::Chase()
{
	if (!_target) return;

	if (!IsTargetInChaseRange())
	{
		LOG("Is Not In ChaseRange!");
		_target.reset();
		ChangeState(ECharacterStateType::STATE_IDLE);
		return;
	}

	LOG("Chase!");
	auto playerPos = _target->GetInfo().Pos;
	FVector dir = (playerPos - _pos).Normalize();
	FVector dist = dir * _info.GetSpeed();
	if (dist.Length() < _pos.DistanceTo(playerPos) - _info.AttackRadius)
	{
		_info.SetLocationAndYaw(_pos + dist);
	}
	else
	{
		_info.SetLocationAndYaw(playerPos - dir * _info.AttackRadius);
		ChangeState(ECharacterStateType::STATE_AUTOATTACK);
	}
}

void Monster::Patrol()
{
	//static auto& nav = Map::GetInst().GetNavMesh(_zone);
	//int currentTriIdx = nav.FindIdxFromPos(_pos);
	//if (currentTriIdx == -1)
	//	return;

	//const auto& neighbors = nav.GetNeighbors(currentTriIdx);
	//if (neighbors.empty())
	//	return;

	//int randIdx = RandomUtils::GetRandomInt(0, static_cast<int>(neighbors.size()) - 1);
	//auto it = neighbors.begin();
	//std::advance(it, randIdx);
	//int nextTriIdx = *it;

	//FVector nextPos = nav.GetTriangleCenter(nextTriIdx);
	//nextPos.Z = _pos.Z;

	//_info.SetLocationAndYaw(nextPos);
}


bool Monster::SetTarget()
{
	std::vector<int32> viewListCopy;
	{
		std::lock_guard lock(_vlLock);
		viewListCopy.assign(_viewList.begin(), _viewList.end());
	}

	for (auto& playerId : viewListCopy)
	{
		if (playerId >= MAX_PLAYER)
		{
			LOG(Warning, "Invaild");
			ChangeState(ECharacterStateType::STATE_IDLE);
			continue;
		}
		auto player = GameWorld::GetInst().GetPlayerByID(playerId);
		if (!player)
		{
			RemoveFromViewList(playerId);
			continue;
		}
		if (IsInViewDistance(player->GetInfo().Pos, detectDist))
		{
			LOG("SetTarget!");
			//Todo: 제일 가까운 플레이어를 쫓아야함...
			ChangeState(ECharacterStateType::STATE_WALK);
			_target = std::dynamic_pointer_cast<Player>(player);
			return true;
		}
	}
	return false;
}

bool Monster::IsTargetInAttackRange()
{
	if (_target != nullptr)
	{
		return this->IsInAttackRange(_target->GetInfo());
	}
	return false;
}

bool Monster::IsTargetInChaseRange()
{
	if (_target != nullptr)
	{
		return this->IsInViewDistance(_target->GetInfo().Pos, detectDist);
	}
	return false;
}
