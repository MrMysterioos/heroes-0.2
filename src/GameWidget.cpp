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
	
}

void GameWidget::Draw()
{
	_scene.Draw();

}

void GameWidget::Update(float dt)
{
	_scene.Update(dt);
}

bool GameWidget::MouseDown(const IPoint &mouse_pos)
{
	// moving
	if (Core::mainInput.GetMouseRightButton()) {
		_cameraMov = true;
	}
	else {
		// temp
		FPoint scene_pos = _scene.MouseToScene(mouse_pos);
		IPoint tile_pos = _map->GetTileCoordinate(scene_pos);
		_map->AddElement("Fire", tile_pos);
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

void GameWidget::AcceptMessage(const Message& message)
{
	
}

void GameWidget::KeyPressed(int keyCode)
{

	
}