#include "World.h"
#include <iostream>

World::World(){}

World::~World() {}

int World::getHeightMap() {
	return MAP_HEIGHT;
}

int World::getWidthMap() {
	return MAP_WIDTH;
}

bool World::loadMap(char* mapName) {
	pugi::xml_document doc;

	if (!doc.load_file(mapName)) {
		std::cout << mapName << " Map not found!\n";
		return false;
	}

	pugi::xml_node map = doc.child("map");
	pugi::xml_node tileset = map.child("tileset");

	if (map == NULL) {
		std::cout << "Map not found!\n";
		return false;;
	}
	if (tileset == NULL) {
		std::cout << "Tileset not found!\n";
		return false;
	}

	MAP_WIDTH = map.attribute("width").as_int();
	MAP_HEIGHT = map.attribute("height").as_int();

	while (tileset) {
		Tileset tiles;

		tiles.firstGid = tileset.attribute("firstgid").as_int();
		tiles.tileCount = tileset.attribute("tilecount").as_int();

		tiles.tileSize.x = tileset.attribute("tilewidth").as_int();
		tiles.tileSize.y = tileset.attribute("tileheight").as_int();

		string tilePath = tileset.child("image").attribute("source").as_string();
		tilePath.erase(0, 1);

		tiles.texture.loadFromFile(tilePath);

		countTiles++;

		int cellX = tiles.texture.getSize().x / tiles.tileSize.x;
		int cellY = tiles.texture.getSize().y / tiles.tileSize.y;
		int countTileID = tiles.firstGid;

		tilesets.push_back(tiles);

		for (int i = 0; i < cellY; i++) {
			for (int j = 0; j < cellX; j++) {
				sf::Sprite TempSpr;

				TempSpr.setTextureRect(sf::IntRect(j * tiles.tileSize.x, i * tiles.tileSize.y, tiles.tileSize.x, tiles.tileSize.y));

				sprites.insert(std::pair<int, sf::Sprite>(countTileID, TempSpr));
				countTileID++;
			}
		}
		tileset = tileset.next_sibling("tileset");
	}

	setTextureTiles();

	pugi::xml_node layer = doc.child("map").child("layer");

	while (layer) {
		Layer TempLayer;
		TempLayer.width = layer.attribute("width").as_int();
		TempLayer.height = layer.attribute("height").as_int();

		pugi::xml_node data = layer.child("data");
		string TempLayerString = data.text().as_string();
		string tempStrNum;

		for (char i : TempLayerString) {
			if (i != ',') {
				tempStrNum += i;
			}
			else {
				TempLayer.layer.push_back(atoi(tempStrNum.c_str()));
				tempStrNum.clear();
			}
		}
		TempLayer.layer.push_back(atoi(tempStrNum.c_str()));

		layers.push_back(TempLayer);

		layer = layer.next_sibling("layer");
	}

	return true;
}

void World::setTextureTiles() {
	int count = 1;
	int tilecount;
	for (auto tileList = tilesets.begin(); tileList != tilesets.end(); ++tileList) {
		tilecount = tileList->tileCount - 1;
		for (int i = 0; i < tilecount; i++) {
			sprites.find(count)->second.setTexture(tileList->texture);
			++count;
		}
	}

}

void World::drawMap(sf::RenderWindow &game) {
	x = y = 0;
	for (auto layerTiles = layers.begin(); layerTiles != layers.end(); ++layerTiles) {
		x = y = 0;
		for (auto tileID = layerTiles->layer.begin(); tileID != layerTiles->layer.end(); tileID++) {
			if (x == layerTiles->width) {
				x = 0;
				y++;
			}
			if (*tileID == 0) {
				x++;
				continue;
			}

			sprites.find(*tileID)->second.setPosition(x * 64, y * 64);

			game.draw(sprites.find(*tileID)->second);
			x++;
		}
	}
}
