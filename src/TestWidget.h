#pragma once
#include "Scene.h"
#include "TMXTiledMap.h"
#include "Unit.h"
#include "AnimateSprite.h"

///
/// Виджет - основной визуальный элемент на экране.
/// Он отрисовывает себя, а также может содержать другие виджеты.
///
class TestWidget : public GUI::Widget
{
public:
	TestWidget(const std::string& name, rapidxml::xml_node<>* elem);

	void Draw() override;
	void Update(float dt) override;

	void AcceptMessage(const Message& message) override;

	bool MouseDown(const IPoint& mouse_pos) override;
	void MouseMove(const IPoint& mouse_pos) override;
	void MouseUp(const IPoint& mouse_pos) override;

	void KeyPressed(int keyCode) override;

private:
	void Init(const std::string& tmx);

private:
	Scene _scene;

	TMXTiledMapPtr _map;

	IPoint _selectTile = IPoint(0, 0);

	EffectsContainer _effCont;
	ParticleEffectPtr _eff;

	std::vector<UnitPtr> _units;

	//UnitPtr _unit;
};
