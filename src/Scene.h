#pragma once

#include "Node.h"

class Scene {
public:

	static Scene& GetInstance();

	void Init();

	void Draw();

	void Update(float dt);

	void DeleteNode(Node* node);
	void PushVector(std::vector<NodePtr> nodes);

	IPoint SceneToMouse(FPoint SceneCoord);
	FPoint MouseToScene(IPoint SceneCoord);


	inline void SetCamraPosition(FPoint newPosition) { _cameraPosition = newPosition; };
	inline FPoint GetCamraPosition() { return _cameraPosition; };

	inline void SetCameraZoom(float newValue) { _cameraZoom = newValue; };
	inline float GetCameraZoom() { return _cameraZoom; };

private:

	Scene();

private:

	std::queue<Node*> _eraceList;

	std::vector<NodePtr> _nodes;
	std::vector<EffectsContainer> _effects;

	/// todo вынести камеру в отдельный класс
	/// Добиться того, чтобы зуммирование производилось не от угла
	FPoint _cameraPosition = FPoint(0.0f, 0.0f);
	float _cameraZoom = 1.0f;

	friend class Node;
};