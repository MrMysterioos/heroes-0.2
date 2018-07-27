#pragma once

class Scene;

class Node : public RefCounter {
public:

	void Init(Scene* scene);

	virtual void Draw() = 0;
	virtual void Update(float dt) = 0;

	virtual ~Node() {};

	inline math::Vector3 GetPosition() { return _position; };
	virtual void SetPosition(math::Vector3 position) { _position = position; };

	inline math::Vector3 GetRotation() { return _rotation; };
	virtual void SetRotation(math::Vector3 rotation) { _rotation = rotation; };

	inline math::Vector3 GetScale() { return _scale; };
	inline void SetScale(math::Vector3 scale) { _scale = scale; };

protected:
	math::Vector3 _position = math::Vector3(0, 0, 0);
	math::Vector3 _rotation = math::Vector3(0, 0, 0);
	math::Vector3 _scale = math::Vector3(1, 1, 1);

	Scene* _scene = nullptr;
};

typedef boost::intrusive_ptr<Node> NodePtr;