#include "Pathfinding.h"
#include <cmath>
#include <algorithm>

Pathfinding::Pathfinding(int mapWidth, int mapHeight, int tileSize)
    : mapWidth(mapWidth), mapHeight(mapHeight), tileSize(tileSize)
{
    walkableTiles.resize(mapWidth * mapHeight, true); // Inicializa todos los tiles como caminables
}

Pathfinding::~Pathfinding() {}

void Pathfinding::SetWalkable(int x, int y, bool walkable)
{
    if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight)
    {
        walkableTiles[y * mapWidth + x] = walkable;
    }
}

bool Pathfinding::IsWalkable(int x, int y) const
{
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
    {
        return false; 
    }
    return walkableTiles[y * mapWidth + x];
}

float Pathfinding::Heuristic(const Vector2D& a, const Vector2D& b) const
{
    return std::abs(a.getX() - b.getX()) + std::abs(a.getY() - b.getY());
}

std::vector<Node*> Pathfinding::GetNeighbors(Node* node)
{
    std::vector<Node*> neighbors;

    const int dx[4] = { 1, -1, 0, 0 };
    const int dy[4] = { 0, 0, 1, -1 };

    for (int i = 0; i < 4; ++i)
    {
        int nx = node->position.getX() + dx[i];
        int ny = node->position.getY() + dy[i];

        if (IsWalkable(nx, ny))
        {
            Node* neighbor = new Node(Vector2D(nx, ny), 0, 0, nullptr);
            neighbors.push_back(neighbor);
        }
    }

    return neighbors;
}

std::vector<Vector2D> Pathfinding::FindPath(const Vector2D& start, const Vector2D& goal)
{
    std::priority_queue<Node*> openSet;
    std::vector<Node*> closedSet;

    Node* startNode = new Node(start, 0, Heuristic(start, goal));
    openSet.push(startNode);

    while (!openSet.empty())
    {
        Node* current = openSet.top();
        openSet.pop();

        if (current->position == goal)
        {
            // Reconstruir el camino
            std::vector<Vector2D> path;
            while (current != nullptr)
            {
                path.push_back(current->position);
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        closedSet.push_back(current);

        for (Node* neighbor : GetNeighbors(current))
        {
            if (std::find(closedSet.begin(), closedSet.end(), neighbor) != closedSet.end())
            {
                delete neighbor;
                continue;
            }

            neighbor->gCost = current->gCost + 1;
            neighbor->hCost = Heuristic(neighbor->position, goal);
            neighbor->parent = current;

            openSet.push(neighbor);
        }
    }

    return {}; 
}
