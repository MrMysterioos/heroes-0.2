#pragma once
#include "TMXTiledMap.h"
#include "AnimateSprite.h"
#include "InteractiveObject.h"

class Unit : public InterObject {
public:
	static boost::intrusive_ptr<Unit> Create(TMXTiledMap* map, AnimateSpritePtr anim, const IPoint& posTile);

	static boost::intrusive_ptr<Unit> Create(TMXTiledMap* map, AnimateSpritePtr anim);

	//пока не пересмотрели архитектуру, использовать этот метод для задания позиции спрайта или анимации
	//когда добавим вектор интерактивных объектов, то перенесем его в InterObject
	void SetPositionInTile(const IPoint& point);
	inline void SetMaxStep(int rad) { _maxStep = rad; }
	inline void SetSelect(bool select) { _isSelect = select; }

	void Update(float dt);
	inline bool IsSelect() const { return _isSelect; }
	inline bool IsMoving() const { return _isMove; }

	std::vector<IPoint> GetAllMoves() const;

	void MoveTo(const IPoint& tilePos);

private:
	bool InitWayPoints(const IPoint& mouseTileTap);

private:
	bool _isMove = false;
	bool _isSelect = false;	

	int _counter = 0;		///<позволяет отслеживать перемещение по вектору построенного пути
	int _maxStep = 0.f;

	std::vector<IPoint> _wayPoints;
};

typedef boost::intrusive_ptr<Unit> UnitPtr;
