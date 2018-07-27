#include "stdafx.h"
#include "Unit.h"

std::string GetAnimation(float dtX, float dtY);

UnitPtr Unit::Create(TMXTiledMapPtr map, AnimateSpritePtr anim) {
	UnitPtr unit = boost::intrusive_ptr<Unit>(new Unit());
	unit->_map = map;
	unit->_animate = anim;
	return unit;
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

	if (obj[iMouse] != 0 || mouseTileTap == _positionInTile) {
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

	int iPositionUnit = _positionInTile.x + _positionInTile.y * mapSize.x;
	obj[iPositionUnit] = d;

	points.push_back(_positionInTile);

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

			float kx = point2.x - point1.x;
			float ky = point2.y - point1.y;

			float k = sqrt(kx * kx + ky * ky);

			float x = kx / k * speed;
			float y = ky / k * speed;

			std::string idAnim = GetAnimation(x, y);
			_animate->SetAnimation(idAnim);

			FPoint spritePos = FPoint(_animate->GetPosition().x, _animate->GetPosition().y);

			if (FPoint(spritePos.x - point2.x, spritePos.y - point2.y).Length() > 5.f) {

				_animate->SetPosition(_animate->GetPosition() + math::Vector3(x, y, 0));
			}
			else {
				_map->ChangeStationVectorObject(_positionInTile, 0);
				_counter++;
				_positionInTile = _wayPoints[_counter];
				_map->ChangeStationVectorObject(_positionInTile, _unitID);
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

void Unit::MouseDown(const IPoint& mouse_pos) {

	if (_isMove || !_isSelect) {
		return;
	}

	std::vector<IPoint> allMoves = GetAllMoves();
	auto m = _isMove;
	IPoint mouseTileTap = _map->GetTileCoordinate(mouse_pos);

	for (int i = 0; i < allMoves.size(); ++i) {
		if (allMoves[i] == mouseTileTap) {

			_wayPoints.clear();

			_isMove = InitWayPoints(mouseTileTap);

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

	IPoint mapSize = _map->GetMapSize();

	for (auto dir : directection) {
		IPoint aroundPoint = _map->GetAdjacentAreaCoords(_positionInTile, dir);
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

	return allMoves;
}

void Unit::SetPositionInTile(const IPoint& point) {
	_positionInTile = point;
	IPoint pos = _map->GetSceneCoordinate(point);
	_animate->SetPosition(math::Vector3(pos.x, pos.y, _animate->GetPosition().z));

	_map->ChangeStationVectorObject(_positionInTile, _unitID);
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