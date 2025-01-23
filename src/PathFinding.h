#pragma once
#include <vector>
#include <queue>
#include "Vector2D.h"

struct Node
{
    Vector2D position;
    float gCost;
    float hCost;
    Node* parent;

    Node(Vector2D pos, float g, float h, Node* p = nullptr)
        : position(pos), gCost(g), hCost(h), parent(p) {}

    float fCost() const { return gCost + hCost; }

    bool operator<(const Node& other) const
    {
        return fCost() > other.fCost(); // Min-heap
    }
};

class Pathfinding
{
public:
    Pathfinding(int mapWidth, int mapHeight, int tileSize);
    ~Pathfinding();

    std::vector<Vector2D> FindPath(const Vector2D& start, const Vector2D& goal);

    void SetWalkable(int x, int y, bool walkable);

private:
    bool IsWalkable(int x, int y) const;
    std::vector<Node*> GetNeighbors(Node* node);
    float Heuristic(const Vector2D& a, const Vector2D& b) const;

private:
    int mapWidth;
    int mapHeight;
    int tileSize;
    std::vector<bool> walkableTiles; // Mapa de walkables
};