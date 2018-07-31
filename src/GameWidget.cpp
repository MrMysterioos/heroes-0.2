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
	size_t countUnits = 3;

	for (size_t i = 0; i < countUnits; ++i) {
		AnimateSpritePtr anim = AnimateSprite::Create("animations/unit.xml");
		anim->SetAnimation("idle");
		anim->SetAnchorPoint(FPoint(0.5f, 0.25f));
		UnitPtr unit = Unit::Create(_map.get(), anim, IPoint(1 + i, 1 + i));
		unit->SetMaxStep(2);
		_queue.push(unit);
	}
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
		_unit->SetSelect(true);
		_queue.pop();
		_queue.push(_unit);
		SetColorAroundUnit();
	}

	if (_unit != nullptr && !_unit->IsMoving() && _isUnitMove) {
		_unit = nullptr;
		_isUnitMove = false;
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
		float zoom = scene.GetCameraZoom();
		IPoint mousePos = IPoint(mouse_pos.x / zoom, mouse_pos.y / zoom);
		IPoint point = _map->GetTileCoordinate(mousePos);
		if (_unit != nullptr) {
			_unit->MoveTo(point);
			_isUnitMove = _unit->IsMoving();
			if (_isUnitMove) {
				ResetColotAroundUnit();
				_tile->GetContainer().KillAllEffects();
			}
		}
	}

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
		IPoint mousePos = IPoint(mouse_pos.x / zoom, mouse_pos.y / zoom);
		IPoint mousePoint = _map->GetTileCoordinate(mousePos);

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
		IPoint positionUnit = _unit->GetPosition();

		for (auto move : allMoves) {
			for (auto tile : tiles) {
				IPoint tilePos = _map->GetTileCoordinate(IPoint(tile->GetPosition().x, tile->GetPosition().y));
				if (tilePos == move) {
					tile->SetColor(Color::Color(255, 50, 200));
				}
				if (tilePos == positionUnit) {
					tile->SetColor(Color::Color(100, 100, 100));
				}
			}
		}
	}
}

void GameWidget::ResetColotAroundUnit() {
	std::vector<TilePtr> tiles = _map->GetVectorTiles();
	for (auto tile : tiles) {
		tile->SetColor(Color::Color(255, 255, 255));
	}
}

void GameWidget::AcceptMessage(const Message& message)
{}

void GameWidget::KeyPressed(int keyCode)
{}