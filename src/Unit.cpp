#include "stdafx.h"
#include "Unit.h"

UnitPtr Unit::CreateUnit(TMXTiledMapPtr map) {
	UnitPtr unit = boost::intrusive_ptr<Unit>(new Unit());
	unit->_map = map;
	return unit;
}

void Unit::SetPositionInTile(const IPoint& point) {
	_positionInTile = point;
	IPoint pos = _map->GetSceneCoordinate(point);
	Sprite::_position = math::Vector3(pos.x, pos.y, 0);
}

void Unit::MouseDown(const IPoint& mouse_pos) {

	std::vector<Direction> direct = {
		North,
		NorthWest,
		SouthWest,
		South, 
		SouthEast,
		NorthEast
	};

	_adjacentTiles.clear();
	IPoint point = (_positionInTile);
	IPoint mapSize = _map->GetMapSize();
	for (auto dir : direct) {
		IPoint point2 = _map->GetAdjacentAreaCoords(point, dir);
		if (point2.x < 0 || point2.x >= mapSize.x ||
			point2.y < 0 || point2.y >= mapSize.y) {
			continue;
		}
		else
			_adjacentTiles.push_back(point2);
	}

	for (int r = 1; r < _radius; ++r) {
		int size = _adjacentTiles.size();
		for (int i = size - 1, k = 6 * r - 1; i >= 0 && k >= 0; --k, --i) {
			IPoint p = _adjacentTiles[i];
			for (int j = 0; j < direct.size(); ++j) {
				IPoint point2 = _map->GetAdjacentAreaCoords(p, direct[j]);
				auto debgu2 = 0;
				if (point2.x < 0 || point2.x >= mapSize.x ||
					point2.y < 0 || point2.y >= mapSize.y) {
					continue;
				}
				bool isFindEqual = false;
				for (auto adj : _adjacentTiles) {
					if (point2 == adj) {
						isFindEqual = true;
						break;
					}
				}
				if (!isFindEqual) {
					_adjacentTiles.push_back(point2);
				}
			}
		}
	}

	if (!_isHighlight) {
		_isHighlight ^= 1;
		HighlightUnit();
		return;
	}

	IPoint mouseTileTap = _map->GetTileCoordinate(mouse_pos);

	for (int i = 0; i < _adjacentTiles.size(); ++i) {
		if (_adjacentTiles[i] == mouseTileTap) {
			//_isMove = InitWayPoints(mouseTileTap);

			SetPositionInTile(mouseTileTap);

			_isHighlight ^= 1;

			HighlightUnit();

			break;
		}
	}
}

void Unit::HighlightUnit() {
	if (_isHighlight) {
		for (int i = 0; i < _adjacentTiles.size(); ++i) {
			auto adj = _adjacentTiles[i];
			int j = _adjacentTiles[i].x + _adjacentTiles[i].y * _map->GetMapSize().x;
			NodePtr node = _map->GetVectorTiles()[j].get();
			SpritePtr sprite = dynamic_pointer_cast<Sprite>(node);
			sprite->SetColor(Color(255, 100, 255));
		}
	}
	else {
		for (int i = 0; i < _adjacentTiles.size(); ++i) {
			int j = _adjacentTiles[i].x + _adjacentTiles[i].y * _map->GetMapSize().x;
			NodePtr node = _map->GetVectorTiles()[j].get();
			SpritePtr sprite = dynamic_pointer_cast<Sprite>(node);
			sprite->SetColor(Color(255, 255, 255));
		}
	}
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
			for (int j = 0; j < direction.size(); ++j) {
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

			if (obj[pos.x + pos.y * mapSize.x] == d) {
				_wayPoints.push_back(pos);
				point = pos;
				--d;
				break;
			}
		}
	}

	return true;
}

void Unit::Update(float dt) {
	/*float speed = 100.f;
	if (_isMove) {
		if (_counter + 1 != _wayPoints.size()) {
			IPoint point1 = _map->GetSceneCoordinate(_wayPoints[_counter]);
			IPoint point2 = _map->GetSceneCoordinate(_wayPoints[_counter + 1]);
			float dtX = (point2.x - point1.x) / 100.f;
			float dtY = (point2.y - point1.x) / 100.f;

		}
	}*/
}

void Unit::Move() {

}