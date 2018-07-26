#include "stdafx.h"
#include "GameWidget.h"

GameWidget::GameWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
{
	Init();
}

void GameWidget::Init()
{

	
}

void GameWidget::Draw()
{
	

}

void GameWidget::Update(float dt)
{
	
}

bool GameWidget::MouseDown(const IPoint &mouse_pos)
{
	return false;
}

void GameWidget::MouseMove(const IPoint &mouse_pos)
{
	
}

void GameWidget::MouseUp(const IPoint &mouse_pos)
{
	
}

void GameWidget::AcceptMessage(const Message& message)
{
	
}

void GameWidget::KeyPressed(int keyCode)
{

	
}