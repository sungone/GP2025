#pragma once
#include "Character.h"
#include "Player.h"
#include "NavMesh.h"

class Monster : public Character
{
public:
	Monster() { Init(); }
    void Init() override;
    void UpdateViewList(std::shared_ptr<Character> other) override;
    void Update();
    void BehaviorTree();
private:
	void ChangeState(ECharacterStateType newState);
    bool DetectTarget();
    bool IsTargetInRange();

private:
    FVector& _pos = _info.Pos;
    std::shared_ptr<Player> _target = nullptr;
};

