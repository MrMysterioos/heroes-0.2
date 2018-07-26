#pragma once
#include "Sprite.h"
#include "TMXTiledMap.h"

class Unit : public Sprite {
public:
	static boost::intrusive_ptr<Unit> CreateUnit(TMXTiledMapPtr map);

	void SetPositionInTile(const IPoint& point);
	inline void SetRadius(int rad) { _radius = rad; }

	void MouseDown(const IPoint& mouse_pos);

	void Update(float dt);

private:
	void HighlightUnit();

	bool InitWayPoints(const IPoint& mouseTileTap);

	void Move();

private:
	int _radius = 0.f;
	IPoint _positionInTile = IPoint();
	TMXTiledMapPtr _map = nullptr;
	bool _isHighlight = false;
	bool _isMove = false;
	int _counter = 0;

	std::vector<IPoint> _wayPoints;

	std::vector<IPoint> _adjacentTiles;
};

typedef boost::intrusive_ptr<Unit> UnitPtr;
