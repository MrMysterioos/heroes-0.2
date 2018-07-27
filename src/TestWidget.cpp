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
	_map = TMXTiledMap::CreateMap(tmx, &_scene);
	_scene.SetCameraZoom(0.5f);


	_map->AddElement("Fire", IPoint(3, 8));
	Render::TexturePtr tex = Core::resourceManager.Get<Render::Texture>("hero");

	/*int countUnits = 2;
	for (int i = 0; i < countUnits; ++i) {
		AnimateSpritePtr anim = AnimateSprite::Create(&_scene, "animations/unit.xml");
		anim->SetAnimation("idle");
		anim->SetAnchorPoint(FPoint(0.5, 0.25f));
		UnitPtr unit = Unit::Create(_map, anim);
		unit->SetMaxStep(2);
		_units.push_back(unit);
	}

	_units[0]->SetPositionInTile(IPoint(1, 3));
	_units[1]->SetPositionInTile(IPoint(4, 8));

	for (auto unit : _units) {
		_scene.PushNode(unit->GetAnimate());
	}*/

	//test unit
	AnimateSpritePtr anim = AnimateSprite::Create(&_scene, "animations/unit.xml");
	anim->SetAnimation("idle");
	anim->SetAnchorPoint(FPoint(0.5, 0.25f));
	_unit = Unit::Create(_map, anim);
	_unit->SetMaxStep(3);
	_unit->SetPositionInTile(IPoint(1, 3));
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

	/*for (auto unit : _units) {
		unit->Update(dt);
	}*/

	_unit->Update(dt);
}

bool TestWidget::MouseDown(const IPoint &mouse_pos)
{
	float zoom = _scene.GetCameraZoom();

	IPoint mousePos = IPoint(mouse_pos.x / zoom, mouse_pos.y / zoom);


	_unit->SetSelect(true);
	_unit->MoveTo(mousePos);


	/*UnitPtr myUnit = nullptr;

	for (auto unit : _units) {
		if (_map->GetTileCoordinate(mousePos) == unit->GetPosition()) {
			if (unit->IsSelect() && !unit->IsMoving()) {
				unit->SetSelect(false);
			}
			else if (!unit->IsMoving()) {
				for (auto e : _units) {
					if (e->IsSelect() && !e->IsMoving()) {
						e->SetSelect(false);
					}
				}
				myUnit = unit;
			}
		}
	}

	bool bUnit = false;
	for (auto e : _units) {
		if (e->IsSelect()) {
			bUnit = true;
		}
	}

	if (!bUnit && !myUnit) {
		FPoint scenePos = FPoint(mouse_pos.x, mouse_pos.y) / zoom;
		IPoint tileCoord = _map->GetTileCoordinate(scenePos);
		if (Core::mainInput.GetMouseLeftButton()) {
			_map->AddElement("Fire", tileCoord);
		}
		else {
			_map->AddElement("Magma", tileCoord);
		}
	}

	std::vector<TilePtr> tiles = _map->GetVectorTiles();
	for (auto tile : tiles) {
		tile->SetColor(Color::Color());
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
	}*/


	return false;
}

void TestWidget::MouseMove(const IPoint &mouse_pos)
{
	/*
	float zoom = _scene.GetCameraZoom();

	IPoint mousePos = IPoint(mouse_pos.x / zoom, mouse_pos.y / zoom);
	auto tileCoord = _map->GetTileCoordinate(mousePos);
	if (tileCoord == _selectTile) {
		return;
	}

	int next = tileCoord.x + tileCoord.y * _map->GetMapSize().x;
	int prev = _selectTile.x + _selectTile.y * _map->GetMapSize().x;
	if (next < _map->GetVectorTiles().size() && next >= 0)
		_map->GetVectorTiles().at(next)->SetColor(Color(255, 255, 0));
	if (prev < _map->GetVectorTiles().size() && prev >= 0)
		_map->GetVectorTiles().at(prev)->SetColor(Color(255, 255, 255));

	_selectTile = tileCoord;
	*/
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