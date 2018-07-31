#pragma once

#include "Node.h"

class Sprite : public Node {
public:
	static boost::intrusive_ptr<Sprite> Create();

	void Init();

	void DrawNode() override;
	void Update(float dt) override {};

	inline FPoint GetAnchorPoint() const { return _anchorPoint; }

	inline void SetTexture(Render::TexturePtr newTexture) { _texture = newTexture; };

	inline void SetAnchorPoint(FPoint point) { _anchorPoint = point; };

	inline void SetColor(const Color& color) { _color = color; }

	inline Render::TexturePtr GetTexture() { return _texture; };

	virtual ~Sprite() {};

protected:
	Render::TexturePtr _texture;
	FPoint _anchorPoint = FPoint(0.0f, 0.0f);
	Color _color = Color();
};

typedef boost::intrusive_ptr<Sprite> SpritePtr;