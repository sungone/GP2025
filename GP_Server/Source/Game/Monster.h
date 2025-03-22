#pragma once
#include "Character.h"
#include "NavMesh.h"

class Monster : public Character
{
public:
	Monster() { Init(); }
    void Init() override;
    void Update();
    void BehaviorTree();
	void ChangeState(ECharacterStateType newState);

    void SetTarget(const FVector& TargetPosition);
    void SetRandomPatrol();
private:
    void Move();
    void Attack();
    bool DetectTarget();
    bool IsTargetInRange();
    bool IsPathComplete();

private:
    FVector& _pos = _info.Pos;
    int _curPolyIdx;
    std::vector<int> _navPath;
    int _curPathIndex;

    FVector _targetPos;
    bool _hasTarget;
}; 

