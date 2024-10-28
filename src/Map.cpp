
#include "Engine.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Log.h"
#include "Physics.h"
#include "Vector2D.h"

#include <math.h>

Map::Map() : Module(), mapLoaded(false)
{
    name = "map";
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake()
{
    name = "map";
    LOG("Loading Map Parser");

    return true;
}

bool Map::Start() {

    return true;
}

bool Map::Update(float dt)
{
    bool ret = true;

    if (mapLoaded) {

        // L07 TODO 5: Prepare the loop to draw all tiles in a layer + DrawTexture()
        // iterate all tiles in a layer
        for (const auto& mapLayer : mapData.layers) {
            //Check if the property Draw exist get the value, if it's true draw the lawyer
            if (mapLayer->properties.GetProperty("Draw") != NULL && mapLayer->properties.GetProperty("Draw")->value == true) {
                for (int i = 0; i < mapData.width; i++) {
                    for (int j = 0; j < mapData.height; j++) {

                        // L07 TODO 9: Complete the draw function

                        //Get the gid from tile
                        int gid = mapLayer->Get(i, j);
                        //Check if the gid is different from 0 - some tiles are empty
                        if (gid != 0) {
                            //L09: TODO 3: Obtain the tile set using GetTilesetFromTileId
                            TileSet* tileSet = GetTilesetFromTileId(gid);
                            if (tileSet != nullptr) {
                                //Get the Rect from the tileSetTexture;
                                SDL_Rect tileRect = tileSet->GetRect(gid);
                                //Get the screen coordinates from the tile coordinates
                                Vector2D mapCoord = MapToWorld(i, j);
                                //Draw the texture
                                Engine::GetInstance().render->DrawTexture(tileSet->texture, mapCoord.getX(), mapCoord.getY(), &tileRect);
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}

// L09: TODO 2: Implement function to the Tileset based on a tile id
TileSet* Map::GetTilesetFromTileId(int gid) const
{
	TileSet* set = nullptr;

    for (const auto& tileset : mapData.tilesets) {
    	if (gid >= tileset->firstGid && gid < (tileset->firstGid + tileset->tileCount)) {
			set = tileset;
			break;
		}
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    // L06: TODO 2: Make sure you clean up any memory allocated from tilesets/map
    for (const auto& tileset : mapData.tilesets) {
        delete tileset;
    }
    mapData.tilesets.clear();

    // L07 TODO 2: clean up all layer data
    for (const auto& layer : mapData.layers)
    {
        delete layer;
    }
    mapData.layers.clear();

    return true;
}

// Load new map
bool Map::Load(std::string path, std::string fileName)
{
    bool ret = false;

    // Assigns the name of the map file and the path
    mapFileName = fileName;
    mapPath = path;
    std::string mapPathName = mapPath + mapFileName;

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapPathName.c_str());

    if (result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapPathName.c_str(), result.description());
        return false;
    }

    // Obtener el nodo raíz del mapa
    pugi::xml_node mapNode = mapFileXML.child("map");
    if (!mapNode)
    {
        LOG("Error parsing map xml file: Cannot find 'map' node.");
        return false;
    }

    try
    {
        // Cargar las propiedades del mapa
        mapData.width = mapNode.attribute("width").as_int(0);
        mapData.height = mapNode.attribute("height").as_int(0);
        mapData.tileWidth = mapNode.attribute("tilewidth").as_int(0);
        mapData.tileHeight = mapNode.attribute("tileheight").as_int(0);

        if (mapData.width == 0 || mapData.height == 0 ||
            mapData.tileWidth == 0 || mapData.tileHeight == 0)
        {
            LOG("Error loading map attributes");
            return false;
        }

        // Cargar los tilesets
        for (pugi::xml_node tilesetNode = mapNode.child("tileset");
            tilesetNode;
            tilesetNode = tilesetNode.next_sibling("tileset"))
        {
            TileSet* tileSet = new TileSet();
            if (tileSet == nullptr)
            {
                LOG("Could not allocate memory for tileset");
                continue;
            }

            // Cargar atributos del tileset con valores por defecto si fallan
            tileSet->firstGid = tilesetNode.attribute("firstgid").as_int(0);
            tileSet->name = tilesetNode.attribute("name").as_string("");
            tileSet->tileWidth = tilesetNode.attribute("tilewidth").as_int(0);
            tileSet->tileHeight = tilesetNode.attribute("tileheight").as_int(0);
            tileSet->spacing = tilesetNode.attribute("spacing").as_int(0);
            tileSet->margin = tilesetNode.attribute("margin").as_int(0);
            tileSet->tileCount = tilesetNode.attribute("tilecount").as_int(0);
            tileSet->columns = tilesetNode.attribute("columns").as_int(0);

            // Cargar la textura del tileset
            pugi::xml_node imageNode = tilesetNode.child("image");
            if (imageNode)
            {
                std::string imgPath = mapPath + imageNode.attribute("source").as_string();
                tileSet->texture = Engine::GetInstance().textures->Load(imgPath.c_str());
                if (tileSet->texture == nullptr)
                {
                    LOG("Failed to load tileset texture: %s", imgPath.c_str());
                    delete tileSet;
                    continue;
                }
            }

            mapData.tilesets.push_back(tileSet);
        }

        // Cargar las capas
        for (pugi::xml_node layerNode = mapNode.child("layer");
            layerNode;
            layerNode = layerNode.next_sibling("layer"))
        {
            MapLayer* mapLayer = new MapLayer();
            if (mapLayer == nullptr)
            {
                LOG("Could not allocate memory for map layer");
                continue;
            }

            mapLayer->id = layerNode.attribute("id").as_int(0);
            mapLayer->name = layerNode.attribute("name").as_string("");
            mapLayer->width = layerNode.attribute("width").as_int(0);
            mapLayer->height = layerNode.attribute("height").as_int(0);

            LoadProperties(layerNode, mapLayer->properties);

            // Cargar los tiles
            pugi::xml_node dataNode = layerNode.child("data");
            if (dataNode)
            {
                for (pugi::xml_node tileNode = dataNode.child("tile");
                    tileNode;
                    tileNode = tileNode.next_sibling("tile"))
                {
                    mapLayer->tiles.push_back(tileNode.attribute("gid").as_int(0));
                }
            }

            mapData.layers.push_back(mapLayer);
        }

        // Crear colliders
        for (const auto& mapLayer : mapData.layers)
        {
            if (mapLayer && mapLayer->name == "Collisions")
            {
                for (int i = 0; i < mapData.width; i++)
                {
                    for (int j = 0; j < mapData.height; j++)
                    {
                        int gid = mapLayer->Get(i, j);
                        if (gid == 401 || gid == 402)
                        {
                            try
                            {
                                Vector2D mapCoord = MapToWorld(i, j);
                                PhysBody* c1 = Engine::GetInstance().physics.get()->CreateRectangle(
                                    mapCoord.getX() + mapData.tileWidth / 2,
                                    mapCoord.getY() + mapData.tileHeight / 2,
                                    mapData.tileWidth,
                                    mapData.tileHeight,
                                    bodyType::STATIC);

                                if (c1 != nullptr)
                                {
                                    if (gid == 401)
                                    {
                                        c1->ctype = ColliderType::PLATFORM;
                                    }
                                    else if (gid == 402)
                                    {
                                        c1->ctype = ColliderType::SPIKE;
                                    }
                                }
                            }
                            catch (const std::exception& e)
                            {
                                LOG("Error creating collider: %s", e.what());
                                continue;
                            }
                        }
                    }
                }
            }
        }

        ret = true;
    }
    catch (const std::exception& e)
    {
        LOG("Error loading map: %s", e.what());
        ret = false;
    }

    if (ret)
    {
        mapLoaded = true;
        LOG("Successfully loaded map %s", fileName.c_str());
    }

    return ret;
}

// L07: TODO 8: Create a method that translates x,y coordinates from map positions to world positions
Vector2D Map::MapToWorld(int x, int y) const
{
    Vector2D ret;

    ret.setX(x * mapData.tileWidth);
    ret.setY(y * mapData.tileHeight);

    return ret;
}

// L09: TODO 6: Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

        properties.propertyList.push_back(p);
    }

    return ret;
}



