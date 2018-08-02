#pragma once

#include "TMXTiledMap.h"
#include "Unit.h"

#include "Barrel.h"


///
/// Виджет - основной визуальный элемент на экране.
/// Он отрисовывает себя, а также может содержать другие виджеты.
///
class GameWidget : public GUI::Widget
{
public:
	GameWidget(const std::string& name, rapidxml::xml_node<>* elem);

	void Draw() override;
	void Update(float dt) override;
	
	void AcceptMessage(const Message& message) override;
	
	bool MouseDown(const IPoint& mouse_pos) override;
	void MouseMove(const IPoint& mouse_pos) override;
	void MouseUp(const IPoint& mouse_pos) override;

	void KeyPressed(int keyCode) override;

private:
	void Init(rapidxml::xml_node<>* elem);

	void SetColorAroundUnit();
	void ResetColorAroundUnit();

private:
	TMXTiledMapPtr _map;

	bool _cameraMov = false;
	IPoint _lastPosition;

	std::queue<UnitPtr> _queue;

	UnitPtr _unit = nullptr;
	ParticleEffectNodePtr _tile;

	GameObject* _gameObject = nullptr;
};
