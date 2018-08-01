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

	// Camera
	Scene::GetInstance().SetCameraZoom(0.6);

	//units
	size_t countUnits = 5;

	for (size_t i = 0; i < countUnits; ++i) {
		AnimateSpritePtr anim = AnimateSprite::Create("animations/unit.xml");
		anim->SetAnimation("idle");
		anim->SetAnchorPoint(FPoint(0.5f, 0.25f));
		UnitPtr unit = Unit::Create(_map.get(), anim, IPoint(i, i));
		unit->SetMaxStep(2);
		_queue.push(unit);
	}

	// temp
	_barr = Barrel::Create(_map.get(), "Fire");
	_barr->SetPosition(IPoint(3, 9));

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
		_unit->SetSelect(true);
		_queue.push(_unit);
		SetColorAroundUnit();
	}

	if (_unit != nullptr && !_unit->IsMoving() && !_unit->IsSelect()) {
		_unit = nullptr;
		_isUnitMove = false;
	}

	//TODO пересмотреть условие, чтобы оно не повторялось постоянно
	if (_unit != nullptr && !_unit->IsMoving() && _unit->IsSelect()) {
		SetColorAroundUnit();
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
			_isUnitMove = _unit->IsMoving();
			if (_isUnitMove) {
				ResetColorAroundUnit();
				_tile->GetContainer().KillAllEffects();
			}
		}

		//atack unit
		std::vector<GameObjectPtr> gameObjects = _map->GetGameObjects();
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
						/*	if (damage) {
								_map->EraseGameObject(enemy);
								DestroyInterObject(enemy);
							}*/
							ResetColorAroundUnit();
						}
						break;
					}

				}
			}
		}
	}

	// temp
	if (!_barr->IsDestroy())
		_barr->Damage(1);

	return false;
}

void GameWidget::MouseMove(const IPoint &mouse_pos)
{
	// moving
	if (_cameraMov) {
		Scene& scene = Scene::GetInstance();
		FPoint move = mouse_pos - _lastPosition;
		FPoint newCameraPos = scene.GetCamraPosition() - move;
		scene.SetCamraPosition(newCameraPos);
	}
	_lastPosition = mouse_pos;

	//select tile
	if (_unit != nullptr) {
		float zoom = Scene::GetInstance().GetCameraZoom();
		FPoint scenePos = Scene::GetInstance().MouseToScene(mouse_pos);
		IPoint mousePoint = _map->GetTileCoordinate(scenePos);

		std::vector<IPoint> allMoves = _unit->GetAllMoves();

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
					tile->SetColor(Color::Color(0, 0, 0));
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

void GameWidget::DestroyInterObject(InterObjectPtr object) {
	auto front = _queue.front();
	while (true) {
		auto obj = _queue.front();
		_queue.pop();
		if (obj != object) {
			_queue.push(obj);
		}
		else {
			break;
		}
	}

	SetColorAroundUnit();

	/*while (front != _queue.front()) {
		_queue.push(_queue.front());
		_queue.pop();
	}*/
}

void GameWidget::AcceptMessage(const Message& message)
{}

void GameWidget::KeyPressed(int keyCode)
{
	if (keyCode == VK_SPACE) {
		_unit = nullptr;
		this->ResetColorAroundUnit();
		//TODO исправить баг с эффектом после пропуска хода
		//_tile->GetContainer().KillAllEffects();
	}
}