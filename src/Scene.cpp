#include "stdafx.h"

#include "Scene.h"
#include "Sprite.h"

Scene::Scene() {
	Init();
}

void Scene::Init() {
	FPoint center = FPoint((float)Render::device.Width() / 2, (float)Render::device.Height() / 2);
	_cameraPosition = center;
}

void Scene::Draw()
{
	std::sort(_nodes.begin(), _nodes.end(), [](NodePtr a, NodePtr b) {
		return a->GetPosition().z > b->GetPosition().z;
	});

	int width = Render::device.Width();
	int height = Render::device.Height();

	FPoint center = FPoint((float)width / 2, (float)height / 2);
	FPoint offset = -_cameraPosition + center;

	Render::device.PushMatrix();

	Render::device.MatrixTranslate(offset);
	Render::device.MatrixScale(_cameraZoom);

	for (NodePtr node : _nodes) {
		Sprite* spritePtr = dynamic_cast<Sprite*>(node.get());
		if (spritePtr) {
			/// @todo Дописать алгоритм прорисовки спрайтов
			int topBound = _cameraPosition.y + height / _cameraZoom;
			int bottomBound = _cameraPosition.y - height / _cameraZoom;
			int rightBound = _cameraPosition.x + width / _cameraZoom;
			int leftBound = _cameraPosition.x - width / _cameraZoom;

			if (spritePtr->GetPosition().y < topBound && spritePtr->GetPosition().y > bottomBound &&
				spritePtr->GetPosition().x > leftBound && spritePtr->GetPosition().x < rightBound) {
				spritePtr->Draw();
			}

		}
		else {
			node->Draw();
		}
	}

	Render::device.PopMatrix();
}

void Scene::Update(float dt)
{
	for (NodePtr node : _nodes) {
		node->Update(dt);
	}
}

void Scene::PushVector(std::vector<NodePtr> nodes)
{
	_nodes.insert(_nodes.end(), nodes.begin(), nodes.end());
}