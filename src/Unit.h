#pragma once
#include "Sprite.h"
#include "TMXTiledMap.h"
#include "AnimateSprite.h"

class Unit : public RefCounter {
public:
	static boost::intrusive_ptr<Unit> Create(TMXTiledMapPtr map, AnimateSpritePtr anim);

	void SetPositionInTile(const IPoint& point);
	inline void SetMaxStep(int rad) { _maxStep = rad; }
	inline void SetSelect(bool select) { _isSelect = select; }

	void MouseDown(const IPoint& mouse_pos);

	void Update(float dt);

	inline AnimateSpritePtr GetSprite() const { return _animate; }
	inline IPoint GetPositionInTile() const { return _positionInTile; }
	inline bool IsSelect() const { return _isSelect; }
	inline bool IsMoving() const { return _isMove; }

	std::vector<IPoint> GetAllMoves() const;

private:
	bool InitWayPoints(const IPoint& mouseTileTap);

private:
	AnimateSpritePtr _animate;

	IPoint _positionInTile = IPoint();
	TMXTiledMapPtr _map = nullptr;
	bool _isMove = false;
	bool _isSelect = false;
	int _counter = 0;
	int _maxStep = 0.f;

	std::vector<IPoint> _wayPoints;

	//debug
	int _unitID = 40;
};

typedef boost::intrusive_ptr<Unit> UnitPtr;
