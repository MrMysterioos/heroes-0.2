#include "stdafx.h"
#include "GameWidget.h"

GameWidget::GameWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
{
	Init(elem);
}

void GameWidget::Init(rapidxml::xml_node<>* elem)
{
	// map
	std::string mapName = Xml::GetStringAttribute(elem, "map");
	_map = TMXTiledMap::CreateMap(mapName, &_scene);

	// Camera
	_scene.SetCameraZoom(0.6);

	//units
	size_t countUnits = 3;

	for (size_t i = 0; i < countUnits; ++i) {
		AnimateSpritePtr anim = AnimateSprite::Create(&_scene, "animations/unit.xml");
		anim->SetAnimation("idle");
		anim->SetAnchorPoint(FPoint(0.5f, 0.25f));
		UnitPtr unit = Unit::Create(_map.get(), anim, IPoint(1 + i, 1 + i));
		unit->SetMaxStep(2);
		_queue.push(unit);
	}
}

void GameWidget::Draw()
{
	_scene.Draw();
}

void GameWidget::Update(float dt)
{
	_scene.Update(dt);
	_map->Update(dt);

	if (_unit == nullptr) {
		_unit = _queue.front();
		_unit->SetSelect(true);
		_queue.pop();
		_queue.push(_unit);
		//SetColorAroundUnit();
	}

	if (_unit != nullptr && !_unit->IsMoving() && _isUnitMove) {
		//ResetColotAroundUnit();
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
		float zoom = _scene.GetCameraZoom();
		IPoint mousePos = IPoint(mouse_pos.x / zoom, mouse_pos.y / zoom);
		IPoint point = _map->GetTileCoordinate(mousePos);
		if (_unit != nullptr) {
			_unit->MoveTo(point);
			_isUnitMove = _unit->IsMoving();
			auto debug2 = 0;
			//выделение области отключить елси начал двигаться
			//если не начал оставиьт всё так, как есть
		}
	}

	return false;
}

void GameWidget::MouseMove(const IPoint &mouse_pos)
{
	// moving
	if (_cameraMov) {
		FPoint move = mouse_pos - _lastPosition;
		FPoint newCameraPos = _scene.GetCamraPosition() - move;
		_scene.SetCamraPosition(newCameraPos);
	}
	_lastPosition = mouse_pos;
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

		for (auto move : allMoves) {
			for (auto tile : tiles) {
				IPoint tilePos = _map->GetTileCoordinate(IPoint(tile->GetPosition().x, tile->GetPosition().y));
				if (tilePos == move) {
					tile->SetColor(Color::Color(255, 100, 255));
				}
				/*if (move == *(allMoves.end() - 1)) {
					tile->SetColor(Color::Color(255, 255, 100));
				}*/
			}
		}
	}
}

void GameWidget::ResetColotAroundUnit() {
	std::vector<TilePtr> tiles;
	for (auto tile : tiles) {
		tile->SetColor(Color::Color(255, 255, 255));
	}
}

void GameWidget::AcceptMessage(const Message& message)
{}

void GameWidget::KeyPressed(int keyCode)
{}