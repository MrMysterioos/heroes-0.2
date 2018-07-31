#pragma once
#include "TMXTiledMap.h"
#include "AnimateSprite.h"
#include "InteractiveObject.h"

class Unit : public InterObject {
public:
	static boost::intrusive_ptr<Unit> Create(TMXTiledMap* map, AnimateSpritePtr anim, const IPoint& posTile);

	static boost::intrusive_ptr<Unit> Create(TMXTiledMap* map, AnimateSpritePtr anim);

	void Init(TMXTiledMap* map, AnimateSpritePtr anim, const IPoint& posTile);

	void SetPosition(const IPoint& point);
	inline void SetMaxStep(int rad) { _maxStep = rad; }
	inline void SetSelect(bool select) { _isSelect = select; }

	void Update(float dt) override;
	inline bool IsSelect() const { return _isSelect; }
	inline bool IsMoving() const { return _isMove; }

	std::vector<IPoint> GetAllMoves() const;

	void MoveTo(const IPoint& tilePos);

private:
	bool InitWayPoints(const IPoint& mouseTileTap);

	/// todo придумать что-нибудь покрасивее
	void UpdateNodePosition(FPoint newPos);

private:
	bool _isMove = false;
	bool _isSelect = false;	

	int _counter = 0;		///<позволяет отслеживать перемещение по вектору построенного пути
	int _maxStep = 0.f;

	std::vector<IPoint> _wayPoints;

	SpritePtr _sprite;
	AnimateSpritePtr _animate;
};

typedef boost::intrusive_ptr<Unit> UnitPtr;
