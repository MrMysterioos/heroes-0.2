#include "stdafx.h"

#include "Scene.h"

Scene& Scene::GetInstance()
{
	static Scene instance;
	return instance;
}


Scene::Scene() {
	Init();
}

void Scene::Init() {
	FPoint center = FPoint((float)Render::device.Width() / 2, (float)Render::device.Height() / 2);
	_cameraPosition = center;
}

void Scene::DeleteNode(Node* node)
{
	_eraceList.push(node);
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

	Render::device.MatrixTranslate(IPoint(offset.x, offset.y));
	Render::device.MatrixScale(_cameraZoom);

	for (NodePtr node : _nodes) {
		node->Draw();
	}

	Render::device.PopMatrix();
}

void Scene::Update(float dt)
{

	while (_eraceList.size()) {
		auto node = _eraceList.front();
		_eraceList.pop();
		for (int i = 0; i < _nodes.size(); ++i) {
			if (_nodes.at(i).get() == node) {
				_nodes.erase(_nodes.begin() + i);
			}
		}
	}

	for (NodePtr node : _nodes) {
		node->Update(dt);
	}
}

void Scene::PushVector(std::vector<NodePtr> nodes)
{
	_nodes.insert(_nodes.end(), nodes.begin(), nodes.end());
}

IPoint Scene::SceneToMouse(FPoint SceneCoord)
{
	int width = Render::device.Width();
	int height = Render::device.Height();

	FPoint center = FPoint((float)width / 2, (float)height / 2);

	SceneCoord -= _cameraPosition - center;
	SceneCoord *= _cameraZoom;
	IPoint res(SceneCoord.x, SceneCoord.y);
	return res;
}

FPoint Scene::MouseToScene(IPoint SceneCoord)
{
	int width = Render::device.Width();
	int height = Render::device.Height();

	FPoint center = FPoint((float)width / 2, (float)height / 2);

	FPoint res(SceneCoord);
	res += _cameraPosition - center;
	res *= (1.0f / _cameraZoom);
	return res;
}