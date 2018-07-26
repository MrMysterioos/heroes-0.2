#pragma once
#include "Tile.h"

struct Transform {
	std::string catalyst;
	std::string start;
	std::string final;

	std::string effect;
};

struct Mix {
	std::string firstElement;
	std::string secondElement;
	std::string result;

	std::string effect;
};

enum Direction {
	North,
	South,
	NorthWest,
	NorthEast,
	SouthWest,
	SouthEast
};

class TMXTiledMap : public RefCounter {
public:
	TMXTiledMap();

	static boost::intrusive_ptr<TMXTiledMap> CreateMap(const std::string& nameFile);

	inline std::vector<TilePtr> GetVectorTiles() const { return _tiles; }

	inline std::vector<int> GetObjectVector() const { return _staticObjects; }

	IPoint GetTileCoordinate(const FPoint& pos);

	IPoint GetSceneCoordinate(const IPoint& tileCoord) const;

	inline IPoint GetMapSize() const { return _mapSize; }

	void Draw();

	Area GetArea(IPoint tileCoord);

	void ChangeArea(const std::string& name, IPoint pos);

	void AddElement(const std::string& name, IPoint pos);

	IPoint GetAdjacentAreaCoords(IPoint tileCoord, Direction dir);

private:
	void InitWithXMLFile(const std::string& nameFile);

	void InitTiles(const std::vector<int>& vect);

	void InitRules(const std::string& xml);

	void UpdateTile(IPoint tileCoord);

private:
	IPoint _mapSize = IPoint();
	IPoint _tileSize = IPoint();
	IPoint _texTileSize = IPoint();

	std::map<int, Area> _areas;
	std::map<std::string, int> _tilesId;
	std::vector<TilePtr> _tiles;
	std::map<int, std::string> _objectId;
	std::vector<std::string> _tileNames;
	std::vector<int> _staticObjects;

	Render::Texture * _texture = nullptr;

	std::map<std::pair<std::string, std::string>, Mix> _combinationRules;
	std::map<std::string, std::vector<Transform>> _extensionRules;
};

typedef boost::intrusive_ptr<TMXTiledMap> TMXTiledMapPtr;