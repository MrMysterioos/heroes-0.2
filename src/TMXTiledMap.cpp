#include "stdafx.h"
#include "TMXTiledMap.h"
#include "Sprite.h"
#include "Scene.h"
#include "Barrel.h"
#include "Tree.h"

std::vector<int> GetVectorFromString(const std::string& str);

TMXTiledMapPtr TMXTiledMap::CreateMap(const std::string& nameFile) {
	TMXTiledMapPtr mapPtr = boost::intrusive_ptr<TMXTiledMap>(new TMXTiledMap());
	mapPtr->InitWithXMLFile(nameFile);
	mapPtr->InitRules("RulesOfElementsInterctions.xml");
	return mapPtr;
}

TMXTiledMap::TMXTiledMap()
{}

void TMXTiledMap::InitWithXMLFile(const std::string& xml) {
	try {

		Xml::RapidXmlDocument doc(xml);
		using Node = rapidxml::xml_node<>;

		Node* root = doc.first_node();

		if (!root) {
			Assert(false);
			throw std::runtime_error("No root node");
		}

		if (utils::equals(root->name(), "map")) {
			_tileSize.x = utils::lexical_cast<int>(root->first_attribute("tilewidth")->value());
			_tileSize.y = utils::lexical_cast<int>(root->first_attribute("tileheight")->value());

			_mapSize.x = utils::lexical_cast<int>(root->first_attribute("width")->value());
			_mapSize.y = utils::lexical_cast<int>(root->first_attribute("height")->value());
		}

		for (auto it = root->first_node(); it != nullptr; it = it->next_sibling()) {
			if (utils::equals(it->name(), "tileset")) {

				std::string type = "NoType";
				auto properties = it->first_node("properties");
				if (properties) {
					auto property = properties->first_node("property");
					while (property) {
						if (Xml::GetStringAttribute(property, "name") == "Type") {
							type = Xml::GetStringAttribute(property, "value");
						}
						property = property->next_sibling("property");
					}
				}

				if (type == "Ground") {
					std::string nameTexture = utils::lexical_cast<std::string>(it->first_attribute("name")->value());
					_texture = Core::resourceManager.Get<Render::Texture>(nameTexture);

					_texTileSize.x = utils::lexical_cast<int>(it->first_attribute("tilewidth")->value());
					_texTileSize.y = utils::lexical_cast<int>(it->first_attribute("tileheight")->value());

					for (auto tile = it->first_node(); tile != nullptr; tile = tile->next_sibling()) {

						if (utils::equals(tile->name(), "tile")) {
							int id = utils::lexical_cast<int>(tile->first_attribute("id")->value());

							Area area;

							std::string name = Xml::GetStringAttribute(tile, "type");
							area.name = name;

							IPoint tileCount;
							tileCount.x = round((float)_texture->getBitmapRect().width / _texTileSize.x);
							tileCount.y = round((float)_texture->getBitmapRect().height / _texTileSize.y);

							int x = (id % tileCount.x) * _texTileSize.x;
							int y = _texture->getBitmapRect().height - (id / tileCount.x + 1) * _texTileSize.y;

							Render::PartialTexturePtr tex(new Render::PartialTexture(_texture, x, y,
								_texTileSize.x, _texTileSize.y, 0, 0, _texTileSize.x, _texTileSize.y, 0));

							area.texture = tex;
							area.effect = "None";

							auto properties = tile->first_node("properties");
							if (properties) {
								auto property = properties->first_node("property");
								while (property) {
									if (Xml::GetStringAttribute(property, "name") == "Effect") {
										area.effect = Xml::GetStringAttribute(property, "value");
									}
									else if (Xml::GetStringAttribute(property, "name") == "Z") {
										area.ZCoord = Xml::GetFloatAttribute(property, "value");
									}
									property = property->next_sibling();
								}
							}

							_tilesId.insert(std::pair<std::string, int>(name, id));
							_areas.insert(std::pair<int, Area>(id, area));

						}
					}
				}
				else if (type == "Object") {
					int firstgid = Xml::GetIntAttribute(it, "firstgid");
					for (auto tile = it->first_node(); tile != nullptr; tile = tile->next_sibling()) {
						if (utils::equals(tile->name(), "tile")) {
							int id = Xml::GetIntAttribute(tile, "id") + firstgid;
							std::string name = Xml::GetStringAttribute(tile, "type");
							auto val = std::map<int, std::string>::value_type(id, name);
							_objectId.insert(val);
						}
					}
				}
			}
			else if (utils::equals(it->name(), "layer")) {
				if (utils::equals(it->first_attribute("name")->value(), "TileLayer")) {
					std::string stringValue = it->first_node()->value();
					auto vect = GetVectorFromString(stringValue);
					InitTiles(vect);
				}
				///TODO считывать gameObjects из xml или tmx

				if (utils::equals(it->first_attribute("name")->value(), "Objects")) {
					_gameObjects = std::vector<GameObjectPtr>(_mapSize.x * _mapSize.y, 0);
					std::string stringValue = it->first_node()->value();
					auto vect = GetVectorFromString(stringValue);
					InitObjects(vect);
				}
			}
		}
	}
	catch (std::exception const& e) {
		Log::log.WriteError(e.what());
		Assert(false);
	}
}

void TMXTiledMap::InitRules(const std::string& xml)
{
	try {

		Xml::RapidXmlDocument doc(xml);
		using Node = rapidxml::xml_node<>;

		Node* root = doc.first_node();

		if (!root) {
			Assert(false);
			throw std::runtime_error("No root node");
		}

		Node* cRules = root->first_node("Combination_rules");
		Node* eRules = root->first_node("Extension_rules");

		Node* cRule = cRules->first_node();
		while (cRule) {
			std::pair<std::string, std::string> comb;
			std::string res;

			comb.first = Xml::GetStringAttribute(cRule, "first_element");
			comb.second = Xml::GetStringAttribute(cRule, "second_element");
			res = Xml::GetStringAttribute(cRule, "result");

			Mix mix;
			mix.firstElement = comb.first;
			mix.secondElement = comb.second;
			mix.result = res;

			_combinationRules.insert(std::pair<std::pair<std::string, std::string>, Mix>(comb, mix));

			cRule = cRule->next_sibling();
		}

		Node* eRule = eRules->first_node();
		while (eRule) {

			std::string cat = Xml::GetStringAttribute(eRule, "catalyst");
			std::string start = Xml::GetStringAttribute(eRule, "start");
			std::string final = Xml::GetStringAttribute(eRule, "final");

			Transform transform;
			transform.catalyst = cat;
			transform.start = start;
			transform.final = final;

			if (_extensionRules.find(cat) == _extensionRules.end()) {
				std::vector<Transform> trans;
				trans.push_back(transform);
				_extensionRules.insert(std::pair<std::string, std::vector<Transform>>(cat, trans));
			}
			else {
				_extensionRules.at(cat).push_back(transform);
			}

			eRule = eRule->next_sibling();
		}

	}
	catch (std::exception const& e) {
		Log::log.WriteError(e.what());
		Assert(false);
	}
}

void TMXTiledMap::InitTiles(const std::vector<int>& vect) {
	for (size_t i = 0; i < vect.size(); ++i) {
		if (!vect[i])
			continue;
		int id = vect[i] - 1;

		auto tile = Tile::Create(_areas.at(id));
		tile->SetAnchorPoint(FPoint(0.5f, 0.5f));

		//смещаем тайл на значение опорной точки спрайта по Х,
		//для того, чтобы он не выходил за границы экрана
		int x = _texTileSize.x * tile->GetAnchorPoint().x + (i % _mapSize.x) * _tileSize.x;
		//прибавляем смещение к Х на каждый нечетный по У тайл
		x += ((i / _mapSize.x) % 2) * (_tileSize.x / 2.f);
		//смещаем тайл на значение опорной точки по У
		//и прибавляем к тайлу смещение по У относительно начала координат
		int y = _texTileSize.y * tile->GetAnchorPoint().y + ((vect.size() - 1 - i) / _mapSize.x) * _tileSize.y / 2.f;
		tile->SetPosition(math::Vector3(x, y, y));

		_tiles.push_back(tile);

		_tileNames.push_back(_areas.at(id).name);
	}
}

void TMXTiledMap::InitObjects(const std::vector<int>& vect)
{
	for (size_t i = 0; i < vect.size(); ++i) {
		if (!vect[i])
			continue;
		int id = vect[i];

		GameObjectPtr obj;

		if (_objectId.at(id) == "Barrel")
			obj = Barrel::Create(this, "Fire");
		else if (_objectId.at(id) == "Tree")
			obj = Tree::Create(this);

		int x = i % _mapSize.x;
		int y = i / _mapSize.x;


		if (obj)
			obj->SetPosition(IPoint(x, y));

		_gameObjects[i] = obj;
	}

}

IPoint TMXTiledMap::GetSceneCoordinate(const IPoint& tileCoord) const {
	int i = tileCoord.x + tileCoord.y * _mapSize.x;

	if (i >= _tiles.size()) {
		return IPoint(-1, -1);
	}

	return IPoint(_tiles[i]->GetPosition().x, _tiles[i]->GetPosition().y);
}

IPoint TMXTiledMap::GetTileCoordinate(const FPoint& mouse_pos)
{

	float x = (mouse_pos.x - _texTileSize.x / 2.f) / (_tileSize.x);
	int maxX = 0, minX = 0;
	if (0.5f - (abs(x) - abs(int(x))) > 0 && x >= 0.f) {
		minX = maxX = int(x);
	}
	else if (0.5f - (abs(x) - int(x)) < 0 && x >= 0.f) {
		minX = int(x);
		maxX = int(x) + 1;
	}

	if (0.5f - (abs(x) - abs(int(x))) > 0 && x < 0.f) {
		minX = int(x) - 1;
		maxX = int(x);
	}
	else if (0.5f - (abs(x) - int(x)) < 0 && x < 0.f) {
		minX = maxX = int(x);
	}

	float y = (mouse_pos.y - _texTileSize.y / 2.f) / (_tileSize.y);
	int maxY = 0, minY = 0;
	if (y >= 0.f) {
		minY = _mapSize.y - int(y / 0.5f) - 2;
		maxY = _mapSize.y - int(y / 0.5f) - 1;
	}
	else if (y < 0.f) {
		minY = _mapSize.y - int(y / 0.5f) - 1;
		maxY = _mapSize.y - int(y / 0.5f) - 2;
	}

	std::vector<IPoint> vect;

	vect.push_back(GetSceneCoordinate(IPoint(minX, maxY)));
	vect.push_back(GetSceneCoordinate(IPoint(maxX, minY)));
	vect.push_back(GetSceneCoordinate(IPoint(minX, minY)));
	vect.push_back(GetSceneCoordinate(IPoint(maxX, maxY)));

	std::vector< std::pair<float, IPoint> > vectLength;
	for (auto i : vect) {
		float length = FPoint(abs(mouse_pos.x - i.x), abs(mouse_pos.y - i.y)).Length();
		std::pair<float, IPoint> p;
		p.first = length;
		vectLength.push_back(p);
	}
	vectLength[0].second = IPoint(minX, maxY);
	vectLength[1].second = IPoint(maxX, minY);
	vectLength[2].second = IPoint(minX, minY);
	vectLength[3].second = IPoint(maxX, maxY);

	std::sort(vectLength.begin(), vectLength.end(), [](std::pair<float, IPoint> a, std::pair<float, IPoint> b) {
		return a.first < b.first;
	});

	return IPoint(vectLength[0].second.x, vectLength[0].second.y);
}

void TMXTiledMap::Update(float dt) {
	for (auto object : _gameObjects) {
		if (object != nullptr) {
			object->Update(dt);
		}
	}
}

std::vector<int> GetVectorFromString(const std::string& str) {
	std::vector<int> vect;
	std::string digitStr = "";
	for (size_t i = 0; i < str.size(); ++i) {

		if (str[i] == '\n' || str[i] == '\r') {
			continue;
		}

		if (isdigit(str[i])) {
			digitStr += str[i];
		}
		else {
			int digit = utils::lexical_cast<int>(digitStr.c_str());
			digitStr = "";
			vect.push_back(digit);
		}
	}
	int digit = utils::lexical_cast<int>(digitStr.c_str());
	vect.push_back(digit);
	return vect;
}

Area TMXTiledMap::GetArea(IPoint tileCoord) {
	try {
		if (tileCoord.x < 0 || tileCoord.x >= _mapSize.x ||
			tileCoord.y < 0 || tileCoord.y >= _mapSize.y) {
			Assert(false);
			throw std::runtime_error("out of range");
		}
		int i = tileCoord.y * _mapSize.x + tileCoord.x;
		return _areas.at(_tilesId.at(_tileNames.at(i)));
	}
	catch (std::exception& e) {
		Log::log.WriteError(e.what());
		Assert(false);
	}
}

void TMXTiledMap::ChangeArea(const std::string& elemName, IPoint pos) {
	try {
		if (pos.x < 0 || pos.x >= _mapSize.x ||
			pos.y < 0 || pos.y >= _mapSize.y) {
			Assert(false);
			throw std::runtime_error("out of range");
		}
		int i = pos.y * _mapSize.x + pos.x;

		if (_tilesId.find(elemName) == _tilesId.end()) {
			return;
		}

		int id = _tilesId.at(elemName);

		TilePtr tile = _tiles.at(i);

		auto pos = tile->GetPosition();

		tile->ChangeArea(_areas.at(_tilesId.at(elemName)));
		tile->SetPosition(pos);

		_tileNames.at(i) = _areas.at(id).name;
	}
	catch (std::exception& e) {
		Log::log.WriteError(e.what());
		Assert(false);
	}
}

void TMXTiledMap::AddElement(const std::string& elemName, IPoint pos) {

	Area area;
	try {
		if (pos.x < 0 || pos.x >= _mapSize.x ||
			pos.y < 0 || pos.y >= _mapSize.y) {
			Assert(false);
			throw std::runtime_error("out of range");
		}

		area = GetArea(pos);
	}
	catch (std::exception& e) {
		Log::log.WriteError(e.what());
		Assert(false);
		return;
	}

	Mix mix;
	if (_combinationRules.find(std::pair<std::string, std::string>(area.name, elemName)) != _combinationRules.end()) {
		mix = _combinationRules.at(std::pair<std::string, std::string>(area.name, elemName));
	}
	else if (_combinationRules.find(std::pair<std::string, std::string>(elemName, area.name)) != _combinationRules.end()) {
		mix = _combinationRules.at(std::pair<std::string, std::string>(elemName, area.name));
	}
	else
		return;

	ChangeArea(mix.result, pos);
	UpdateTile(pos);

}

void TMXTiledMap::UpdateTile(IPoint tileCoord)
{

	std::vector<Direction> dirs{
		North,
		South,
		NorthWest,
		NorthEast,
		SouthWest,
		SouthEast
	};

	auto thisArea = GetArea(tileCoord);

	std::vector<Transform> thisTrans;
	if (_extensionRules.find(thisArea.name) != _extensionRules.end()) {
		thisTrans = _extensionRules.at(thisArea.name);
	}

	for (auto dir : dirs) {

		IPoint target = GetAdjacentAreaCoords(tileCoord, dir);

		if (target.x < 0 || target.x >= _mapSize.x ||
			target.y < 0 || target.y >= _mapSize.y) {
			continue;
		}

		auto neighbor = GetArea(target);

		for (auto transform : thisTrans) {
			if (transform.start == neighbor.name) {
				ChangeArea(transform.final, target);
				UpdateTile(target);
			}
		}

		if (_extensionRules.find(neighbor.name) != _extensionRules.end()) {
			std::vector<Transform> neigTrans = _extensionRules.at(neighbor.name);
			for (auto transform : neigTrans) {
				if (transform.start == thisArea.name) {
					ChangeArea(transform.final, tileCoord);
					UpdateTile(tileCoord);
				}
			}
		}

	}

}

IPoint TMXTiledMap::GetAdjacentAreaCoords(IPoint tileCoord, Direction dir)
{
	bool even = true;
	if (tileCoord.y % 2) {
		even = false;
	}

	IPoint ret;
	if (dir == North) {
		ret = IPoint(tileCoord.x, tileCoord.y - 2);
	}
	else if (dir == South) {
		ret = IPoint(tileCoord.x, tileCoord.y + 2);
	}
	else if (dir == NorthWest) {
		if (even) {
			ret = IPoint(tileCoord.x - 1, tileCoord.y - 1);
		}
		else {
			ret = IPoint(tileCoord.x, tileCoord.y - 1);
		}
	}
	else if (dir == NorthEast) {
		if (even) {
			ret = IPoint(tileCoord.x, tileCoord.y - 1);
		}
		else {
			ret = IPoint(tileCoord.x + 1, tileCoord.y - 1);
		}
	}
	else if (dir == SouthWest) {
		if (even) {
			ret = IPoint(tileCoord.x - 1, tileCoord.y + 1);
		}
		else {
			ret = IPoint(tileCoord.x, tileCoord.y + 1);
		}
	}
	else if (dir == SouthEast) {
		if (even) {
			ret = IPoint(tileCoord.x, tileCoord.y + 1);
		}
		else {
			ret = IPoint(tileCoord.x + 1, tileCoord.y + 1);
		}
	}

	return ret;
}

void TMXTiledMap::SwapGameObject(const IPoint& point1, const IPoint& point2) {
	int i1 = point1.x + point1.y * _mapSize.x;
	int i2 = point2.x + point2.y * _mapSize.x;

	if (i1 < _gameObjects.size() && i2 < _gameObjects.size()) {
		std::swap(_gameObjects[i1], _gameObjects[i2]);
	}
}

void TMXTiledMap::PushGameObject(GameObjectPtr object) {
	IPoint point = object->GetPosition();
	int i = point.x + point.y * _mapSize.x;
	if (i < _gameObjects.size() && _gameObjects[i] == nullptr) {
		_gameObjects[i] = object;
	}
}

void TMXTiledMap::EraseGameObject(GameObjectPtr object) {
	IPoint point = object->GetPosition();
	int i = point.x + point.y * _mapSize.x;
	if (i < _gameObjects.size() && _gameObjects[i] == object) {
		_gameObjects[i] = nullptr;
	}
}