#pragma once

#include "Sprite.h"

class Scene;

class AnimateSprite : public Sprite {
public:
	static boost::intrusive_ptr<AnimateSprite> Create(const std::string& xml);

	void Init(const std::string& xml);

	void DrawNode() override;

	void Update(float dt) override;

	void SetAnimation(const std::string& id);

	inline void SetRepeat(bool newValue) { _repeat = newValue; };

	inline bool IsFinal() { return _final; };

private:

	struct Animate {
		int frameCount = 0;
		float speed = 1;
		IPoint frameSize = IPoint(0, 0);
		std::string textureId;
	};

	float _timer = 0;
	int _currentFrame = 0;
	std::string _currentAnim;

	std::map<std::string, Animate> _anims;

	bool _repeat = true;
	bool _final = false;

};

typedef boost::intrusive_ptr<AnimateSprite> AnimateSpritePtr;