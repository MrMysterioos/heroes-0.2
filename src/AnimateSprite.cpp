#include "stdafx.h"

#include "AnimateSprite.h"

boost::intrusive_ptr<AnimateSprite> AnimateSprite::Create(const std::string & xml)
{
	AnimateSpritePtr ret(new AnimateSprite);
	ret->Init(xml);
	return ret;
}

void AnimateSprite::Init(const std::string& xml) {
	try {

		Xml::RapidXmlDocument doc(xml);
		using XmlNode = rapidxml::xml_node<>;

		XmlNode* root = doc.first_node();

		if (!root) {
			Assert(false);
			throw std::runtime_error("No root node");
		}

		XmlNode* animation = root->first_node("animation");
		while (animation) {

			std::string elId = Xml::GetStringAttribute(animation, "id");

			Animate anim;

			anim.frameSize.x = Xml::GetIntAttribute(animation, "width");
			anim.frameSize.y = Xml::GetIntAttribute(animation, "height");
			anim.speed = Xml::GetFloatAttribute(animation, "speed");
			anim.textureId = Xml::GetStringAttribute(animation, "sprite_sheet");

			Render::Texture* tex = Core::resourceManager.Get<Render::Texture>(anim.textureId);

			int tileCountX = round((float)tex->Width() / anim.frameSize.x);
			int tileCountY = round((float)tex->Height() / anim.frameSize.y);
			anim.frameCount = tileCountX * tileCountY;

			_anims.insert(std::pair<std::string, Animate>(elId, anim));

			SetAnimation(elId);

			animation = animation->next_sibling("animation");
		}
		
	}
	catch (std::exception const& e) {
		Log::log.WriteError(e.what());
		Assert(false);
	}
}

void AnimateSprite::SetAnimation(const std::string& id)
{
	try {
		std::string textureId = _anims.at(id).textureId;
		Render::Texture* tex = Core::resourceManager.Get<Render::Texture>(textureId);
		SetTexture(tex);
		_currentAnim = id;
	}
	catch (std::exception const& e) {
		Log::log.WriteError(e.what());
		Assert(false);
	}
	_final = false;
}

void AnimateSprite::Draw()
{
	int tileCountX = round((float)_texture->Width() / _anims.at(_currentAnim).frameSize.x);
	int tileCountY = round((float)_texture->Height() / _anims.at(_currentAnim).frameSize.y);
	IPoint frameSize = _anims.at(_currentAnim).frameSize;
	int x = _currentFrame % tileCountX;
	int y = tileCountY - _currentFrame / tileCountX - 1;
	Render::PartialTexturePtr partTex(new Render::PartialTexture(_texture.get(), x * frameSize.x, y * frameSize.y, frameSize.x, frameSize.y, 0, 0, frameSize.x, frameSize.y, 0));

	Render::device.PushMatrix();

	Render::device.MatrixTranslate(_position);
	Render::device.MatrixRotate(math::Vector3(1, 0, 0), _rotation.x);
	Render::device.MatrixRotate(math::Vector3(0, 1, 0), _rotation.y);
	Render::device.MatrixRotate(math::Vector3(0, 0, 1), _rotation.z);

	FPoint offset = FPoint(partTex->Width() * _anchorPoint.x, partTex->Height() * _anchorPoint.y);
	Render::device.MatrixTranslate(-offset);

	Render::BeginColor(_color);
	partTex->Draw();
	Render::EndColor();

	Render::device.PopMatrix();
}

void AnimateSprite::Update(float dt) {
	if (_final) {
		return;
	}
	_timer += dt;
	float speed = _anims.at(_currentAnim).speed;
	int frameCount = _anims.at(_currentAnim).frameCount;
	if (_timer > 1.0f / speed) {
		_timer = 0;
		++_currentFrame;
		if (_currentFrame >= frameCount - 1) {
			if (_repeat) {
				_currentFrame = 0;
			}
			else {
				_final = true;
			}
		}
	}
}