#include "stdafx.h"
#include "Unit.h"
#include "Scene.h"

std::string GetRunAnimation(float dtX, float dtY);

UnitPtr Unit::Create(TMXTiledMap* map, AnimateSpritePtr anim, const IPoint& posTile) {
	UnitPtr unit = boost::intrusive_ptr<Unit>(new Unit());
	unit->Init(map, anim, posTile);
	return unit;
}

void Unit::Init(TMXTiledMap* map, AnimateSpritePtr anim, const IPoint& posTile)
{
	_map = map;
	_animate = anim;
	InitHealthBar(1.0f);
	SetPosition(posTile);
	map->PushGameObject(this);

	/// todo вынести в файл
	_hp = _maxHp = 20;
}

bool Unit::InitWayPoints(const IPoint& mouseTileTap) {
	std::vector<int> obj(_map->GetMapSize().x * _map->GetMapSize().y, 0);
	auto objects = _map->GetGameObjects();
	std::vector<IPoint> points;
	IPoint mapSize = _map->GetMapSize();

	std::vector<Direction> direction = {
		North,
		NorthWest,
		SouthWest,
		South,
		SouthEast,
		NorthEast
	};

	int iMouse = mouseTileTap.x + mouseTileTap.y * mapSize.x;

	if (objects[iMouse] != nullptr || mouseTileTap == _position) {
		return false;
	}

	for (size_t i = 0; i < obj.size(); ++i) {
		if (objects[i] != nullptr) {
			obj[i] = -1;
		}
	}

	bool isFind = false;
	int d = 1;
	int begin = 0;

	int iPositionUnit = _position.x + _position.y * mapSize.x;
	obj[iPositionUnit] = d;

	points.push_back(_position);

	while (!isFind) {
		int size = points.size();
		for (int i = begin; i < size; ++i) {
			for (size_t j = 0; j < direction.size(); ++j) {
				IPoint pos = _map->GetAdjacentAreaCoords(points[i], direction[j]);
				if (pos.x < 0 || pos.x >= mapSize.x ||
					pos.y < 0 || pos.y >= mapSize.y)
				{
					continue;
				}
				int count = pos.x + pos.y * mapSize.x;
				if (obj[count] == 0) {
					points.push_back(pos);
					obj[count] = d + 1;
					if (count == mouseTileTap.x + mouseTileTap.y * mapSize.x) {
						isFind = true;
						_wayPoints.push_back(pos);
						break;
					}
				}
			}
		}
		if (size == points.size()) {
			isFind = false;
			break;
		}
		++d;
		begin = points.size() - (points.size() - size);
	}

	if (!isFind) {
		return false;
	}

	--d;
	IPoint point = _wayPoints[0];

	while (d > 0) {
		for (size_t i = 0; i < direction.size(); ++i) {
			IPoint pos = _map->GetAdjacentAreaCoords(point, direction[i]);

			if (pos.x < 0 || pos.x >= mapSize.x ||
				pos.y < 0 || pos.y >= mapSize.y)
			{
				continue;
			}

			if (obj[pos.x + pos.y * mapSize.x] == d) {
				_wayPoints.push_back(pos);
				point = pos;
				--d;
				break;
			}
		}
	}


	std::reverse(_wayPoints.begin(), _wayPoints.end());

	return true;
}

void Unit::Update(float dt) {
	//TODO вынести в файл
	float speed = 5.f;

	//в этот кадр происходит удар
	//TODO разработать систему кадровых взаимодействий
	int frame = 16;

	//урон юнита
	//TODO разработь систему урона объектов
	int damage = 5;

	if (_animate->GetCurrentFrame() > frame && _state == State::Attack) {
		if (_enemy != nullptr) {
			_enemy->Damage(damage);
			_enemy = nullptr;
		}		
	}
	if (_animate->IsFinal()) {

		if (_state == State::Attack) {
			_isSelect = false;
		}

		_state = State::Idle;
		_animate->SetAnimation("idle");
		_animate->SetRepeat(true);
	}

	if (_isSelect && _state == State::Run) {
		if (_counter + 1 != _wayPoints.size()) {
			IPoint point1 = _map->GetSceneCoordinate(_wayPoints[_counter]);
			IPoint point2 = _map->GetSceneCoordinate(_wayPoints[_counter + 1]);

			FPoint vel = point2 - point1;
			vel.Normalize();
			vel *= speed;

			std::string idAnim = GetRunAnimation(vel.x, vel.y);
			if (_idAnim != idAnim) {
				_idAnim = idAnim;
				_animate->SetAnimation(idAnim);
			}

			FPoint spritePos = FPoint(_animate->GetPosition().x, _animate->GetPosition().y);

			if (FPoint(spritePos.x - point2.x, spritePos.y - point2.y).Length() > 5.f) {

				math::Vector3 pos = _animate->GetPosition() + math::Vector3(vel.x, vel.y, 0.0f);
				pos.z = pos.y - 200;

				_animate->SetPosition(pos);
				UpdateNodePosition(FPoint(pos.x, pos.y));
			}
			else {
				_counter++;
				_map->SwapGameObject(_position, _wayPoints[_counter]);
				this->SetPosition(_wayPoints[_counter]);
				--_steps;
			}
		}
		else {

			_animate->SetAnimation("idle");
			_idAnim = "";
			_map->SwapGameObject(_position, _wayPoints[_counter]);
			_counter = 0;
			_state = State::Idle;
			_wayPoints.clear();

			if (_steps <= 0) {
				_isSelect = false;
			}
		}		
	}
}

void Unit::Destroy() {

	InterObject::Destroy();

	if (_sprite != nullptr) {
		Scene::GetInstance().DeleteNode(_sprite.get());
	}

	if (_animate != nullptr) {
		// Scene::GetInstance().DeleteNode(_animate.get());
		// Вместо удаления анимации сдвинемм ее немного по z координате
		math::Vector3 pos = _animate->GetPosition();
		pos.z += 100;
		_animate->SetPosition(pos);
	}

	if (_healthBar != nullptr) {
		Scene::GetInstance().DeleteNode(_healthBar);
	}
}

bool Unit::Damage(int damage) {
	bool isDamage = InterObject::Damage(damage);

	if (isDamage) {
		_map->EraseGameObject(this);
		//анимация смерти
		_animate->SetAnimation("death");
		_animate->SetRepeat(false);
		_state = State::Death;
		return true;
	}
	else {
		_animate->SetAnimation("damage");
		_animate->SetRepeat(false);
		_state = State::Damage;
		return false;
	}
}

void Unit::Attack(InterObjectPtr object) {
	if (_state != State::Idle) {
		return;
	}
	_state = State::Attack;
	_animate->SetAnimation(GetAttackAnimation(object->GetPosition()));
	_animate->SetRepeat(false);	
	_enemy = object;
}

void Unit::UpdateNodePosition(FPoint newPos)
{
	math::Vector3 animPos = _animate->GetPosition();
	animPos.x = newPos.x;
	animPos.y = newPos.y;
	_animate->SetPosition(animPos);
	math::Vector3 barPos = _healthBar->GetPosition();
	/// todo вынести константы
	barPos.x = newPos.x - 50;
	barPos.y = newPos.y + 125;
	_healthBar->SetPosition(barPos);
}

void Unit::MoveTo(const IPoint& tilePos) {

	auto gameObjects = _map->GetGameObjects();
	int id = _position.x + _position.y * _map->GetMapSize().x;

	if (id < gameObjects.size() && gameObjects[id] == nullptr) {
		return;
	}

	if (_state != State::Idle || !_isSelect) {
		return;
	}

	std::vector<IPoint> allMoves = GetAllMoves();

	for (size_t i = 0; i < allMoves.size(); ++i) {
		if (allMoves[i] == tilePos) {
			_wayPoints.clear();

			bool isMove = InitWayPoints(tilePos);

			if (isMove) {
				_state = State::Run;
			}

			break;
		}
	}
}

std::vector<IPoint> Unit::GetAllMoves() const {
	std::vector<IPoint> allMoves;

	std::vector<Direction> directection = {
		North,
		NorthWest,
		SouthWest,
		South,
		SouthEast,
		NorthEast
	};

	IPoint mapSize = _map->GetMapSize();

	std::vector<int> obj(mapSize.x * mapSize.y, 0);
	auto gameObjects = _map->GetGameObjects();
	for (size_t i = 0; i < gameObjects.size(); ++i) {
		if (gameObjects[i] != nullptr) {
			obj[i] = -1;
		}
	}

	if (obj.empty()) {
		return allMoves;
	}

	for (auto dir : directection) {
		IPoint aroundPoint = _map->GetAdjacentAreaCoords(_position, dir);
		int iObj = aroundPoint.x + aroundPoint.y * _map->GetMapSize().x;
		if (aroundPoint.x < 0 || aroundPoint.x >= mapSize.x ||
			aroundPoint.y < 0 || aroundPoint.y >= mapSize.y || obj[iObj] != 0)
		{
			continue;
		}
		else
			allMoves.push_back(aroundPoint);
	}

	for (size_t step = 1; step < _steps; ++step) {
		int size = allMoves.size();
		for (int i = size - 1, k = 6 * step - 1; i >= 0 && k >= 0; --k, --i) {
			IPoint adjPoint = allMoves[i];
			for (size_t j = 0; j < directection.size(); ++j) {
				IPoint aroundPoint = _map->GetAdjacentAreaCoords(adjPoint, directection[j]);
				int iObj = aroundPoint.x + aroundPoint.y * _map->GetMapSize().x;
				if (aroundPoint.x < 0 || aroundPoint.x >= mapSize.x ||
					aroundPoint.y < 0 || aroundPoint.y >= mapSize.y || obj[iObj] != 0)
				{
					continue;
				}
				bool isFindEqual = false;
				for (auto adj : allMoves) {
					if (aroundPoint == adj) {
						isFindEqual = true;
						break;
					}
				}
				if (!isFindEqual) {
					allMoves.push_back(aroundPoint);
				}
			}
		}
	}

	allMoves.push_back(_position);

	return allMoves;
}

std::vector<IPoint> Unit::GetDestroyObject() const {
	std::vector<IPoint> posObject;

	std::vector<IPoint> allMoves = this->GetAllMoves();

	std::vector<GameObjectPtr> gameObjects = _map->GetGameObjects();
	
	std::vector<Direction> direction{
		North,
		NorthWest,
		SouthWest,
		South,
		SouthEast,
		NorthEast
	};

	IPoint mapSize = _map->GetMapSize();

	for (auto move : allMoves) {
		for (auto dir : direction) {
			IPoint aroundPoint = _map->GetAdjacentAreaCoords(move, dir);
			int iObj = aroundPoint.x + aroundPoint.y * _map->GetMapSize().x;
			if (aroundPoint.x < 0 || aroundPoint.x >= mapSize.x ||
				aroundPoint.y < 0 || aroundPoint.y >= mapSize.y ||
				gameObjects[iObj] == nullptr || gameObjects[iObj] == this)
			{
				continue;
			}

			bool isFindEqual = false;
			for (auto pos : posObject) {
				if (pos == aroundPoint) {
					isFindEqual = true;
					break;
				}
			}
			if (!isFindEqual) {
				posObject.push_back(aroundPoint);
			}
		}
	}

	return posObject;
}

void Unit::SetPosition(const IPoint& point) {
	_position = point;
	IPoint pos = _map->GetSceneCoordinate(point);
	_animate->SetPosition(math::Vector3(pos.x, pos.y, pos.y - 200));

	UpdateNodePosition(FPoint(pos.x, pos.y));
}

std::string Unit::GetAttackAnimation(const IPoint& posEnemy) {
	std::string animate = "";

	std::vector<Direction> direction = {
		North,
		NorthWest,
		SouthWest,
		South,
		SouthEast,
		NorthEast
	};

	Direction direct;

	for (auto dir : direction) {
		IPoint pos = _map->GetAdjacentAreaCoords(_position, dir);
		if (pos.x < 0 || pos.x >= _map->GetMapSize().x ||
			pos.y < 0 || pos.y >= _map->GetMapSize().y)
		{
			continue;
		}
		if (pos == posEnemy) {
			direct = dir;
			break;
		}
	}

	if (direct == North) {
		animate = "attack_n";
	}
	else if (direct == NorthWest) {
		animate = "attack_nw";
	}
	else if (direct == NorthEast) {
		animate = "attack_ne";
	}
	else if (direct == SouthWest) {
		animate = "attack_sw";
	}
	else if (direct == SouthEast) {
		animate = "attack_se";
	}
	else if (direct == South) {
		animate = "attack_s";
	}

	return animate;
}

std::string GetRunAnimation(float dtX, float dtY) {
	std::string animate = "";

	if (dtX == 0 && dtY > 0) {
		animate = "run_n";
	}
	else if (dtX > 0 && dtY > 0) {
		animate = "run_ne";
	}
	else if (dtX > 0 && dtY < 0) {
		animate = "run_se";
	}
	else if (dtX == 0 && dtY < 0) {
		animate = "run_s";
	}
	else if (dtX < 0 && dtY < 0) {
		animate = "run_sw";
	}
	else if (dtX < 0 && dtY > 0) {
		animate = "run_nw";
	}

	return animate;
}
