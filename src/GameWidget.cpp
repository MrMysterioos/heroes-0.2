#include "stdafx.h"
#include "GameWidget.h"
#include "Scene.h"

GameWidget::GameWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
{
	Init(elem);
}

void GameWidget::Init(rapidxml::xml_node<>* elem)
{

	//tile
	EffectsContainer effCont;
	_tile = ParticleEffectNode::Create(effCont);

	// map
	std::string mapName = Xml::GetStringAttribute(elem, "map");

	_map = TMXTiledMap::CreateMap(mapName);

	// camera
	Scene::GetInstance().SetCameraZoom(0.6);

	// music
	MM::manager.PlayTrack("epic music");

	//units
	//TODO вынести в файл
	size_t countUnits = 2;

	for (size_t i = 0; i < countUnits; ++i) {
		AnimateSpritePtr anim = AnimateSprite::Create("animations/unit.xml");
		anim->SetAnimation("idle");
		anim->SetAnchorPoint(FPoint(0.5f, 0.25f));
		UnitPtr unit = Unit::Create(_map.get(), anim, IPoint(i, i));
		unit->SetMaxStep(3);
		_queue.push(unit);
	}

	// background
	Render::Texture* back = Core::resourceManager.Get<Render::Texture>("background");
	auto sprite = Sprite::Create();
	IPoint mapSize = _map->GetMapSize();
	IPoint tileSize = _map->GetTileSize();
	IPoint center = IPoint(mapSize.x * tileSize.x / 2, mapSize.y * tileSize.y / 2);
	sprite->SetTexture(back);
	sprite->SetAnchorPoint(FPoint(0.5f, 0.5f));
	sprite->SetPosition(math::Vector3((float)center.x, (float)center.y, 1000.0f));
}

void GameWidget::Draw()
{
	Scene::GetInstance().Draw();
}

void GameWidget::Update(float dt)
{
	Scene::GetInstance().Update(dt);
	_map->Update(dt);

	if (_unit == nullptr) {
		_unit = _queue.front();
		_queue.pop();
		while(_unit->IsDestroy() && !_queue.empty()) {
			_unit = _queue.front();
			_queue.pop();
		}
		_unit->SetSelect(true);
		_queue.push(_unit);
	}

	if (_unit != nullptr && !_unit->IsMoving() && !_unit->IsSelect()) {
		_unit = nullptr;
	}

	if (_unit != nullptr && _isMovingInAttack && _unit->GetState() == Unit::State::Idle) {
		_unit->Attack(_enemy);
		_isMovingInAttack = false;
	}

	//TODO пересмотреть условие, чтобы оно не повторялось постоянно
	if (_unit != nullptr && _unit->GetState() == Unit::State::Idle) {
		SetColorAroundUnit();
	}
	else {
		ResetColorAroundUnit();
		_tile->GetContainer().KillAllEffects();
		_tile->SetPosition(math::Vector3(0, 0, _tile->GetPosition().z));
	}

	
}

bool GameWidget::MouseDown(const IPoint &mouse_pos)
{
	// moving
	if (Core::mainInput.GetMouseRightButton()) {
		_cameraMov = true;
	}
	else {
		Scene& scene = Scene::GetInstance();
		FPoint scenePos = scene.MouseToScene(mouse_pos);
		IPoint point = _map->GetTileCoordinate(scenePos);

		//moving unit
		if (_unit != nullptr) {
			_unit->MoveTo(point);
			if (_unit->IsMoving()) {
				_tile->GetContainer().KillAllEffects();
			}
		}

		//atack unit
		std::vector<GameObjectPtr> gameObjects = _map->GetVectorGameObjects();
		for (auto object : gameObjects) {
			if (object != nullptr && object != _unit &&
				point == object->GetPosition()) 
			{
				std::vector<Direction> direction{
					North,
					NorthWest,
					SouthWest,
					South,
					SouthEast,
					NorthEast
				};
				//ближний бой
				for (auto dir : direction) {
					IPoint pos = _map->GetAdjacentAreaCoords(point, dir);
					if (pos.x < 0 || pos.x >= _map->GetMapSize().x ||
						pos.y < 0 || pos.y >= _map->GetMapSize().y)
					{
						continue;
					}
					if (pos == _unit->GetPosition()) {
						auto enemy = dynamic_cast<InterObject*>(object.get());
						if (enemy != nullptr) {
							_unit->Attack(enemy);
						}
						break;
					}
				}
				//begin
				//дальний бой(игрок подходит к врагу и наносит ему урон)
				if (_unit->GetState() != Unit::State::Attack) {
					std::vector<IPoint> allMoves = _unit->GetAllMoves();
					for (auto move : allMoves) {
						for (auto dir : direction) {
							IPoint pos = _map->GetAdjacentAreaCoords(point, dir);
							if (pos.x < 0 || pos.x >= _map->GetMapSize().x ||
								pos.y < 0 || pos.y >= _map->GetMapSize().y)
							{
								continue;
							}
							if (move == pos) {
								auto enemy = dynamic_cast<InterObject*>(object.get());
								if (enemy != nullptr) {
									_unit->MoveTo(move);
									_enemy = enemy;
									_isMovingInAttack = true;
								}
								break;
							}
						}
					}
				}
				//end
			}
		}
	}

	return false;
}

void GameWidget::MouseMove(const IPoint &mouse_pos)
{
	// moving
	if (_cameraMov) {

		IPoint mapSize = _map->GetMapSize();
		IPoint tileSize = _map->GetTileSize();
		IPoint center = IPoint(mapSize.x * tileSize.x / 2, mapSize.y * tileSize.y / 2);

		float zoom = Scene::GetInstance().GetCameraZoom();

		int vertPadd = 100;
		int horPadd = 500;

		///todo зуммирование происходит не от центра камеры.
		///следовательно, границы просчитываются не совсем верно.
		int rightBound = (mapSize.x * tileSize.x - horPadd) * zoom;
		int topBound = (mapSize.y * ((float)tileSize.y / 1.5f) - vertPadd) * zoom;

		int bottomBound = horPadd - tileSize.x / 2;
		int leftBound = vertPadd - tileSize.y / 2;

		Scene& scene = Scene::GetInstance();
		FPoint move = mouse_pos - _lastPosition;
		FPoint newCameraPos = scene.GetCamraPosition() - move;

		if (newCameraPos.x < bottomBound) {
			newCameraPos.x = bottomBound;
		}
		else if (newCameraPos.x > rightBound) {
			newCameraPos.x = rightBound;
		}

		if (newCameraPos.y < leftBound) {
			newCameraPos.y = leftBound;
		}
		else if (newCameraPos.y > topBound) {
			newCameraPos.y = topBound;
		}

		scene.SetCamraPosition(newCameraPos);

	}
	_lastPosition = mouse_pos;

	//select objects
	if (_unit != nullptr) {
		float zoom = Scene::GetInstance().GetCameraZoom();
		FPoint scenePos = Scene::GetInstance().MouseToScene(mouse_pos);
		IPoint tilePoint = _map->GetTileCoordinate(scenePos);
		auto obj = _map->GetGameObject(tilePoint);
		auto interObj = dynamic_cast<InterObject*>(obj.get());

		if (interObj) {
			interObj->ShowInfo();
			if ( _selectObject && _selectObject != interObj) {
				_selectObject->HideInfo();
			}
			_selectObject = interObj;
		}

	}

	//select tile
	if (_unit != nullptr) {
		float zoom = Scene::GetInstance().GetCameraZoom();
		FPoint scenePos = Scene::GetInstance().MouseToScene(mouse_pos);
		IPoint mousePoint = _map->GetTileCoordinate(scenePos);

		std::vector<IPoint> allMoves = _unit->GetAllMoves();
		std::vector<IPoint> gameObjects = _unit->GetDestroyObject();

		allMoves.insert(allMoves.end(), gameObjects.begin(), gameObjects.end());

		IPoint positionTile = _map->GetTileCoordinate(IPoint(_tile->GetPosition().x, _tile->GetPosition().y));
		
		if (positionTile != mousePoint && !_unit->IsMoving()) {
			bool isSelectTile = false;
			for (auto move : allMoves) {
				if (mousePoint == move) {
					IPoint pos = _map->GetSceneCoordinate(mousePoint);
					_tile->GetContainer().KillAllEffects();
					_tile->GetContainer().AddEffect("Select");
					_tile->SetPosition(math::Vector3(pos.x, pos.y, 0));

					isSelectTile = true;
				}
			}
			if (!isSelectTile) {
				_tile->GetContainer().KillAllEffects();
				_tile->SetPosition(math::Vector3(0, 0, 0));
			}
		}
	}
}

void GameWidget::MouseUp(const IPoint &mouse_pos)
{
	// moving
	_cameraMov = false;
}

void GameWidget::SetColorAroundUnit() {
	if(_unit != nullptr) {
		std::vector<IPoint> allMoves = _unit->GetAllMoves();
		std::vector<TilePtr> tiles = _map->GetVectorTiles();
		std::vector<IPoint> posObjects = _unit->GetDestroyObject();
		IPoint positionUnit = _unit->GetPosition();

		for (auto move : allMoves) {
			for (auto tile : tiles) {
				IPoint tilePos = _map->GetTileCoordinate(IPoint(tile->GetPosition().x, tile->GetPosition().y));
				if (tilePos == move) {
					tile->SetColor(Color::Color(255, 50, 200));
				}
			}
		}

		for (auto pos : posObjects) {
			for (auto tile : tiles) {
				IPoint tilePos = _map->GetTileCoordinate(IPoint(tile->GetPosition().x, tile->GetPosition().y));
				if (tilePos == pos) {
					tile->SetColor(Color::Color(255, 100, 0));
				}
			}
		}
		
	}
}

void GameWidget::ResetColorAroundUnit() {
	std::vector<TilePtr> tiles = _map->GetVectorTiles();
	for (auto tile : tiles) {
		tile->SetColor(Color::Color(255, 255, 255));
	}
}

void GameWidget::AcceptMessage(const Message& message)
{}

void GameWidget::KeyPressed(int keyCode)
{
	if (keyCode == VK_SPACE) {
		_unit = nullptr;
		this->ResetColorAroundUnit();
	}
}