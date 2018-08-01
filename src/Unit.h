#pragma once
#include "TMXTiledMap.h"
#include "AnimateSprite.h"
#include "InteractiveObject.h"

class Unit : public InterObject {
public:

	enum class State {
		Attack,
		Run,
		Damage,
		Idle,
		Death
	};

	static boost::intrusive_ptr<Unit> Create(TMXTiledMap* map, AnimateSpritePtr anim, const IPoint& posTile);

	void Init(TMXTiledMap* map, AnimateSpritePtr anim, const IPoint& posTile);

	void MoveTo(const IPoint& tilePos);
	void Attack(InterObjectPtr object);
	void SetPosition(const IPoint& point);

	inline void SetMaxStep(int rad) { _maxStep = rad; }
	inline void SetSelect(bool select) { _isSelect = select; _steps = _maxStep; }

	void Update(float dt) override;
	void Destroy() override;
	bool Damage(int damage) override;

	inline bool IsSelect() const { return _isSelect; }
	inline bool IsMoving() const { if (_state == State::Run) return true; else return false; }
	inline int GetMaxStep() const{ return _maxStep; }
	inline State GetState() const { return _state; }

	std::vector<IPoint> GetAllMoves() const;
	std::vector<IPoint> GetDestroyObject() const;

private:
	bool InitWayPoints(const IPoint& mouseTileTap);

	/// todo придумать что-нибудь покрасивее
	void UpdateNodePosition(FPoint newPos);

	std::string GetAttackAnimation(const IPoint& posEnemy);

private:
	bool _isSelect = false;	
	InterObjectPtr _enemy = nullptr;
	std::string _idAnim = "";

	int _counter = 0;		///<позволяет отслеживать перемещение по вектору построенного пути
	int _maxStep = 0.f;
	int _steps = 0;

	std::vector<IPoint> _wayPoints;

	State _state = State::Idle;
	SpritePtr _sprite;
	AnimateSpritePtr _animate;
};

typedef boost::intrusive_ptr<Unit> UnitPtr;
