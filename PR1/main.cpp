#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <queue>
#include <ctime>

const int mapWidth = 10;
const int mapHeight = 10;
const int agentCount = 3;
const int moveCost = 1;

struct Node {
    int x, y;
    Node* parent;
    int gCost;
    int hCost;

    Node(int x, int y, Node* parent, int gCost, int hCost)
        : x(x), y(y), parent(parent), gCost(gCost), hCost(hCost) {}

    int fCost() const {
        return gCost + hCost;
    }
};

// Function to calculate Manhattan distance between two points
int manhattanDistance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

struct CompareNodes {
    bool operator()(const Node* lhs, const Node* rhs) const {
        return lhs->fCost() > rhs->fCost();
    }
};

// A* pathfinding algorithm
std::vector<std::pair<int, int>> findPath(int startX, int startY, int targetX, int targetY, std::vector<std::vector<int>>& map) {
    std::vector<std::pair<int, int>> path;
    std::priority_queue<Node*, std::vector<Node*>, CompareNodes> openSet;

    std::vector<std::vector<bool>> closedSet(mapWidth, std::vector<bool>(mapHeight, false));

    Node* startNode = new Node(startX, startY, nullptr, 0, manhattanDistance(startX, startY, targetX, targetY));
    openSet.push(startNode);

    while (!openSet.empty()) {
        Node* currentNode = openSet.top();
        openSet.pop();

        if (currentNode->x == targetX && currentNode->y == targetY) {
            // Found the target, reconstruct the path
            while (currentNode != nullptr) {
                path.push_back({currentNode->x, currentNode->y});
                currentNode = currentNode->parent;
            }
            break;
        }

        closedSet[currentNode->x][currentNode->y] = true;

        // Generate neighboring nodes
        std::vector<std::pair<int, int>> neighbors = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        for (const auto& neighbor : neighbors) {
            int neighborX = currentNode->x + neighbor.first;
            int neighborY = currentNode->y + neighbor.second;

            // Check if the neighbor is valid
            if (neighborX >= 0 && neighborX < mapWidth && neighborY >= 0 && neighborY < mapHeight
                && map[neighborX][neighborY] == 0 && !closedSet[neighborX][neighborY]) {
                int gCost = currentNode->gCost + moveCost;
                int hCost = manhattanDistance(neighborX, neighborY, targetX, targetY);
                Node* neighborNode = new Node(neighborX, neighborY, currentNode, gCost, hCost);
                openSet.push(neighborNode);
            }
        }
    }

    // Clean up allocated nodes
    while (!openSet.empty()) {
        delete openSet.top();
        openSet.pop();
    }

    return path;
}

int main() {
    // Initialize SFML window
    sf::RenderWindow window(sf::VideoMode(800, 800), "Multi-Agent Pathfinding");

    // Define your initial map, agent positions, and target positions here
    std::vector<std::vector<int>> map(mapWidth, std::vector<int>(mapHeight, 0));

    // Initialize SFML shapes and colors for visualization
    sf::CircleShape agentShapes[agentCount];
    sf::CircleShape targetShapes[agentCount];
    sf::Color agentColors[agentCount] = {sf::Color::Red, sf::Color::Green, sf::Color::Blue};

    for (int i = 0; i < agentCount; ++i) {
        agentShapes[i] = sf::CircleShape(20);
        agentShapes[i].setFillColor(agentColors[i]);

        targetShapes[i] = sf::CircleShape(20);
        targetShapes[i].setFillColor(sf::Color::Yellow);
    }

    // Define your main loop for visualization and interaction here
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();

        // Draw your map
        sf::RectangleShape tile(sf::Vector2f(40, 40));
        for (int i = 0; i < mapWidth; ++i) {
            for (int j = 0; j < mapHeight; ++j) {
                if (map[i][j] == 1) {
                    tile.setFillColor(sf::Color::Black);
                } else {
                    tile.setFillColor(sf::Color::White);
                }
                tile.setPosition(i * 40, j * 40);
                window.draw(tile);
            }
        }

        // Draw the agents and their paths
        int startX[agentCount] = {2, 5, 8};
        int startY[agentCount] = {2, 5, 8};
        int targetX[agentCount] = {7, 2, 5};
        int targetY[agentCount] = {7, 4, 1};

        for (int i = 0; i < agentCount; ++i) {
            // Draw the path
            auto path = findPath(startX[i], startY[i], targetX[i], targetY[i], map);
            for (auto& node : path) {
                sf::RectangleShape pathTile(sf::Vector2f(40, 40));
                pathTile.setFillColor(sf::Color::Cyan);
                pathTile.setPosition(node.first * 40, node.second * 40);
                window.draw(pathTile);
            }

            // Draw the agent
            agentShapes[i].setPosition(startX[i] * 40, startY[i] * 40);
            window.draw(agentShapes[i]);

            // Draw the target
            targetShapes[i].setPosition(targetX[i] * 40, targetY[i] * 40);
            window.draw(targetShapes[i]);
        }

        window.display();

    }

    return 0;
}
