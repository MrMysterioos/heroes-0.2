#include "stdafx.h"
#include "TestWidget.h"
#include "Sprite.h"
#include "AnimateSprite.h"

TestWidget::TestWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
{
	Init(Xml::GetStringAttribute(elem, "map"));
}

void TestWidget::Init(const std::string& tmx)
{
	_map = TMXTiledMap::CreateMap(tmx);
	auto tiles = _map->GetVectorTiles();
	for (auto e : tiles) {
		_scene.PushNode(e);
	}
	_scene.SetCameraZoom(0.5f);

	_map->AddElement("Fire", IPoint(3, 8));

	_unit = Unit::CreateUnit(_map);
	Render::TexturePtr tex = Core::resourceManager.Get<Render::Texture>("hero");
	_unit->SetTexture(tex);
	_unit->SetAnchorPoint(FPoint(0.5f, 0.5f));
	_unit->SetPositionInTile(IPoint(1, 3));
	_unit->SetRadius(2);
	_scene.PushNode(_unit);

	_tex = Core::resourceManager.Get<Render::Texture>("hero");
}

void TestWidget::Draw()
{
	_scene.Draw();
	_effCont.Draw();
}

void TestWidget::Update(float dt)
{
	_scene.Update(dt);
	_effCont.Update(dt);
}

bool TestWidget::MouseDown(const IPoint &mouse_pos)
{
	float zoom = _scene.GetCameraZoom();

	FPoint scenePos = FPoint(mouse_pos.x, mouse_pos.y) / zoom;
	IPoint tileCoord = _map->GetTileCoordinate(scenePos);
	if (Core::mainInput.GetMouseLeftButton()) {
		_map->AddElement("Fire", tileCoord);
	}
	else {
		_map->AddElement("Magma", tileCoord);
	}

	_unit->MouseDown(IPoint(mouse_pos.x / zoom, mouse_pos.y / zoom));

	return false;
}

void TestWidget::MouseMove(const IPoint &mouse_pos)
{
}

void TestWidget::MouseUp(const IPoint &mouse_pos)
{
	
}

void TestWidget::AcceptMessage(const Message& message)
{
	
}

void TestWidget::KeyPressed(int keyCode)
{

	
}