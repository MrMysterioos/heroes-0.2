#pragma once
#include "TMXTiledMap.h"
#include "AnimateSprite.h"
#include "InteractiveObject.h"

class Unit : public InterObject {
public:
	static boost::intrusive_ptr<Unit> Create(TMXTiledMap* map, AnimateSpritePtr anim, const IPoint& posTile);

	void Init(TMXTiledMap* map, AnimateSpritePtr anim, const IPoint& posTile);

	void SetPosition(const IPoint& point);
	inline void SetMaxStep(int rad) { _maxStep = rad; }
	inline void SetSelect(bool select) { _isSelect = select; _steps = _maxStep; }

	void Update(float dt) override;
	inline bool IsSelect() const { return _isSelect; }
	inline bool IsMoving() const { return _isMove; }
	inline int GetMaxStep() const{ return _maxStep; }

	std::vector<IPoint> GetAllMoves() const;
	std::vector<IPoint> GetDestroyObject() const;

	void MoveTo(const IPoint& tilePos);

private:
	bool InitWayPoints(const IPoint& mouseTileTap);

	/// todo ��������� ���-������ ����������
	void UpdateNodePosition(FPoint newPos);

private:
	bool _isMove = false;
	bool _isSelect = false;	

	int _counter = 0;		///<��������� ����������� ����������� �� ������� ������������ ����
	int _maxStep = 0.f;
	int _steps = 0;

	std::vector<IPoint> _wayPoints;

	SpritePtr _sprite;
	AnimateSpritePtr _animate;
};

typedef boost::intrusive_ptr<Unit> UnitPtr;
