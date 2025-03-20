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

    uint32_t GetState() const { return _info.State; }
    void SetState(ECharacterStateType newState) { _info.State = newState; }

private :
    void SetTargetIdx(const FVector& TargetPosition)
    {
        int TargetPolygon = NavMesh::FindIdxFromPos(TargetPosition);
        _navPath = NavMesh::FindPath(_curPolyIdx, TargetPolygon);
        _curPathIndex = 0;
    }

    void UpdatePos()
    {
        if (_curPathIndex < _navPath.size()) {
            int NextPolygonIndex = _navPath[_curPathIndex];
            _pos = NavMesh::Vertices[NavMesh::Triangles[NextPolygonIndex].IndexA];
            _curPolyIdx = NextPolygonIndex;
            _curPathIndex++;
        }
    }
	
    bool ShouldAttack();
    bool ShouldWalking();
    FVector GenerateRandomNearbyPosition();

private:
    FVector& _pos = _info.Pos;
    int _curPolyIdx;
    std::vector<int> _navPath;
    int _curPathIndex;
}; 

