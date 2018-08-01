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
	virtual void Attack(InterObjectPtr object);

	void Update(float dt) override;
	void Destroy() override;
	bool Damage(int damage) override;

	inline bool IsSelect() const { return _isSelect; }
	inline bool IsMoving() const { if (_state == State::Run) return true; else return false; }
	inline int GetMaxStep() const{ return _maxStep; }

	std::vector<IPoint> GetAllMoves() const;
	std::vector<IPoint> GetDestroyObject() const;

	void MoveTo(const IPoint& tilePos);

private:
	bool InitWayPoints(const IPoint& mouseTileTap);

	/// todo придумать что-нибудь покрасивее
	void UpdateNodePosition(FPoint newPos);

	std::string GetAttackAnimation(const IPoint& posEnemy);

private:

	enum class State {
		Attack,
		Run,
		Damage,
		Idle
	};

	//bool _isMove = false;
	bool _isSelect = false;	
	//bool _isAttack = false;
	InterObjectPtr _enemy = nullptr;

	int _counter = 0;		///<позволяет отслеживать перемещение по вектору построенного пути
	int _maxStep = 0.f;
	int _steps = 0;

	std::string _idAnim = "";

	std::vector<IPoint> _wayPoints;

	State _state = State::Idle;

	SpritePtr _sprite;
	AnimateSpritePtr _animate;
};

typedef boost::intrusive_ptr<Unit> UnitPtr;
