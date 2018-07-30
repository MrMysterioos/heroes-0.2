#include "stdafx.h"
#include "Unit.h"

std::string GetAnimation(float dtX, float dtY);

UnitPtr Unit::Create(TMXTiledMapPtr map, AnimateSpritePtr anim) {
	UnitPtr unit = boost::intrusive_ptr<Unit>(new Unit());
	unit->Init(map, anim);
	return unit;
}

void Unit::Init(TMXTiledMapPtr map, AnimateSpritePtr anim)
{
	_map = map;
	_animate = anim;

	HealthBarPtr healthbar = HealthBar::Create(map->GetPointerToScene());
	_healthBar = healthbar;
	math::Vector3 pos = _animate->GetPosition();
	_healthBar->SetPosition(pos);
}

bool Unit::InitWayPoints(const IPoint& mouseTileTap) {
	std::vector<int> obj = _map->GetObjectVector();
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

	if (obj[iMouse] != 0 || mouseTileTap == _position) {
		return false;
	}

	for (auto ob : obj) {
		if (ob != 0) {
			ob = -1;
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
	float speed = 5.;
	if (_isSelect && _isMove) {
		if (_counter + 1 != _wayPoints.size()) {
			IPoint point1 = _map->GetSceneCoordinate(_wayPoints[_counter]);
			IPoint point2 = _map->GetSceneCoordinate(_wayPoints[_counter + 1]);

			FPoint vel = point2 - point1;
			vel.Normalize();
			vel *= speed;

			std::string idAnim = GetAnimation(vel.x, vel.y);
			_animate->SetAnimation(idAnim);

			FPoint spritePos = FPoint(_animate->GetPosition().x, _animate->GetPosition().y);

			if (FPoint(spritePos.x - point2.x, spritePos.y - point2.y).Length() > 5.f) {

				math::Vector3 pos = _animate->GetPosition() + math::Vector3(vel.x, vel.y, 0.0f);
				pos.z = pos.y - 200;

				_animate->SetPosition(pos);
				UpdateNodePosition(FPoint(pos.x, pos.y));
			}
			else {
				_map->ChangeStationVectorObject(_position, 0);
				_counter++;
				SetPosition(_wayPoints[_counter]);
				_map->ChangeStationVectorObject(_position, _unitID);
			}
		}
		else {

			_animate->SetAnimation("idle");

			_counter = 0;
			_isMove = false;
			_wayPoints.clear();

			//debug
			_isSelect = false;
		}
	}
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

	if (_isMove || !_isSelect) {
		return;
	}

	std::vector<IPoint> allMoves = GetAllMoves();

	for (int i = 0; i < allMoves.size(); ++i) {
		if (allMoves[i] == tilePos) {
			_wayPoints.clear();

			_isMove = InitWayPoints(tilePos);

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

	std::vector<int> obj = _map->GetObjectVector();

	if (obj.empty()) {
		return allMoves;
	}

	IPoint mapSize = _map->GetMapSize();

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

	for (size_t step = 1; step < _maxStep; ++step) {
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

void Unit::SetPosition(const IPoint& point) {
	_position = point;
	IPoint pos = _map->GetSceneCoordinate(point);
	_animate->SetPosition(math::Vector3(pos.x, pos.y, pos.y - 200));
	UpdateNodePosition(FPoint(pos.x, pos.y));

	_map->ChangeStationVectorObject(_position, _unitID);
}

std::string GetAnimation(float dtX, float dtY) {
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