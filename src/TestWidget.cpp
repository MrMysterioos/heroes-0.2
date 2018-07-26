#include "stdafx.h"
#include "TestWidget.h"
#include "Sprite.h"

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
	Render::TexturePtr tex = Core::resourceManager.Get<Render::Texture>("hero");

	int countUnits = 2;
	for (int i = 0; i < countUnits; ++i) {
		SpritePtr sprite = Sprite::Create();
		sprite->SetTexture(tex);
		sprite->SetAnchorPoint(FPoint(0.5f, 0.f));
		UnitPtr unit = Unit::CreateUnit(_map, sprite);
		unit->SetMaxStep(2);
		_units.push_back(unit);
	}

	_units[0]->SetPositionInTile(IPoint(1, 3));
	_units[1]->SetPositionInTile(IPoint(4, 8));

	for (auto unit : _units) {
		_scene.PushNode(unit->GetSprite());
	}
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

	for (auto unit : _units) {
		unit->Update(dt);
	}
}

bool TestWidget::MouseDown(const IPoint &mouse_pos)
{
	float zoom = _scene.GetCameraZoom();

	IPoint mousePos = IPoint(mouse_pos.x / zoom, mouse_pos.y / zoom);

	/*FPoint scenePos = FPoint(mouse_pos.x, mouse_pos.y) / zoom;
	IPoint tileCoord = _map->GetTileCoordinate(scenePos);
	if (Core::mainInput.GetMouseLeftButton()) {
	_map->AddElement("Fire", tileCoord);
	}
	else {
	_map->AddElement("Magma", tileCoord);
	}*/

	UnitPtr myUnit = nullptr;

	for (auto unit : _units) {
		if (_map->GetTileCoordinate(mousePos) == unit->GetPositionInTile()) {
			if (unit->IsSelect() && !unit->IsMoving()) {
				unit->SetSelect(false);
			}
			else if (!unit->IsMoving()) {
				myUnit = unit;
			}
		}
	}

	if (myUnit != nullptr) {
		myUnit->SetSelect(true);
		std::vector<IPoint> allMoves = myUnit->GetAllMoves();
		std::vector<TilePtr> tiles = _map->GetVectorTiles();
		for (auto move : allMoves) {
			for (auto tile : tiles) {
				IPoint posTile = _map->GetTileCoordinate(IPoint(tile->GetPosition().x, tile->GetPosition().y));
				if (posTile == move) {
					tile->SetColor(Color::Color(255, 100, 255));
				}
			}
		}
	}
	else {
		std::vector<TilePtr> tiles = _map->GetVectorTiles();
		for (auto tile : tiles) {
			tile->SetColor(Color::Color());
		}
	}

	for (auto unit : _units) {
		unit->MouseDown(mousePos);
	}


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