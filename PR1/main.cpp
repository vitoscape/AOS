#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <queue>
#include <cmath>

const int mapWidth = 10;    // Ширина карты
const int mapHeight = 10;   // Высота карты
const int agentCount = 3;   // Кол-во агентов
const int moveCost = 1;     // Стоимость передвижения


class Map {
private:
    int mapWidth;
    int mapHeight;
    std::vector<std::vector<int>> data;

public:
    Map(int width, int height, std::vector<std::vector<int>> mapData) : mapWidth(width), mapHeight(height), data(mapData) {}

    int getWidth() const { return mapWidth; }

    int getHeight() const { return mapHeight; }

    std::vector<std::vector<int>> getData() const { return data; }

    bool isObstacle(int x, int y) const { return data[x][y] == 1; }
};

class Node {
private:
    int x;
    int y;
    Node *parent;
    int gCost;
    int hCost;

public:
    Node(int xPos, int yPos, Node *p, int g, int h) : x(xPos), y(yPos), parent(p), gCost(g), hCost(h) {}

    int getX() const { return x; }

    int getY() const { return y; }

    Node *getParent() const { return parent; }

    int getGCost() const { return gCost; }

    int getHCost() const { return hCost; }

    int getFCost() const { return gCost + hCost; }
};

class AStar {
private:
    struct CompareNodes {
        bool operator()(const Node *lhs, const Node *rhs) const {
            return lhs->getFCost() > rhs->getFCost();
        }
    };

public:
    static int manhattanDistance(int x1, int y1, int x2, int y2) {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }

    static std::vector<std::vector<std::pair<int, int>>> findPaths(const std::vector<int>& start_X, const std::vector<int>& start_Y, const std::vector<int>& target_X, const std::vector<int>& target_Y, const Map& map, std::vector<std::vector<bool>>& occupied) {
        std::vector<std::vector<std::pair<int, int>>> allPaths;
        for (size_t i = 0; i < start_X.size(); ++i) {
            std::vector<std::pair<int, int>> path;
            std::priority_queue<Node*, std::vector<Node*>, CompareNodes> openSet;
            int mapWidth = map.getWidth();
            int mapHeight = map.getHeight();

            std::vector<std::vector<bool>> closedSet(mapWidth, std::vector<bool>(mapHeight, false));

            Node* startNode = new Node(start_X[i], start_Y[i], nullptr, 0, manhattanDistance(start_X[i], start_Y[i], target_X[i], target_Y[i]));
            openSet.push(startNode);

            while (!openSet.empty()) {
                Node* currentNode = openSet.top();
                openSet.pop();

                if (currentNode->getX() == target_X[i] && currentNode->getY() == target_Y[i]) {
                    while (currentNode != nullptr) {
                        path.emplace_back(std::make_pair(currentNode->getX(), currentNode->getY()));
                        currentNode = currentNode->getParent();
                    }
                    break;
                }

                closedSet[currentNode->getX()][currentNode->getY()] = true;

                std::vector<std::pair<int, int>> neighbors = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
                for (const auto& neighbor : neighbors) {
                    int neighborX = currentNode->getX() + neighbor.first;
                    int neighborY = currentNode->getY() + neighbor.second;

                    if (neighborX >= 0 && neighborX < mapWidth && neighborY >= 0 && neighborY < mapHeight
                        && map.getData()[neighborX][neighborY] == 0 && !closedSet[neighborX][neighborY] && !occupied[neighborX][neighborY]) {
                        int gCost = currentNode->getGCost() + 1;
                        int hCost = manhattanDistance(neighborX, neighborY, target_X[i], target_Y[i]);
                        Node* neighborNode = new Node(neighborX, neighborY, currentNode, gCost, hCost);
                        openSet.push(neighborNode);
                    }
                }
            }

            while (!openSet.empty()) {
                delete openSet.top();
                openSet.pop();
            }
            allPaths.push_back(path);

            // Обновление массива occupied для занятых клеток пути
            for (auto& node : path) {
                occupied[node.first][node.second] = true;
            }
        }

        return allPaths;
    }
};

class PotentialFields {
public:
    static std::vector<std::vector<std::pair<int, int>>> findPaths(const std::vector<int>& start_X, const std::vector<int>& start_Y, const std::vector<int>& target_X, const std::vector<int>& target_Y, const Map& map, std::vector<std::vector<bool>>& occupied) {
        std::vector<std::vector<std::pair<int, int>>> allPaths;
        for (size_t i = 0; i < start_X.size(); ++i) {
            std::vector<std::pair<int, int>> path;
            int currentX = start_X[i];
            int currentY = start_Y[i];
            int targetX = target_X[i];
            int targetY = target_Y[i];

            while (currentX != targetX || currentY != targetY) {
                int bestX = currentX;
                int bestY = currentY;
                int bestPotential = calculateTotalPotential(currentX, currentY, start_X, start_Y, targetX, targetY);

                // Перебираем соседние клетки и находим ту, в которой потенциал меньше
                int neighbors[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
                for (const auto& neighbor : neighbors) {
                    int newX = currentX + neighbor[0];
                    int newY = currentY + neighbor[1];

                    if (newX >= 0 && newX < map.getWidth() && newY >= 0 && newY < map.getHeight() && map.getData()[newX][newY] == 0) {
                        int potential = calculateTotalPotential(newX, newY, start_X, start_Y, targetX, targetY);

                        if (potential < bestPotential) {
                            bestPotential = potential;
                            bestX = newX;
                            bestY = newY;
                        }
                    }
                }

                path.emplace_back(std::make_pair(bestX, bestY));
                currentX = bestX;
                currentY = bestY;
            }

            allPaths.push_back(path);
        }

        return allPaths;
    }

private:
    static int calculateAgentPotential(int x, int y, int agentX, int agentY) {
        int distance = manhattanDistance(x, y, agentX, agentY);
        return distance;
    }

    static int calculateGoalPotential(int x, int y, int goalX, int goalY) {
        int distance = manhattanDistance(x, y, goalX, goalY);
        return -distance;
    }

    static int calculateTotalPotential(int x, int y, const std::vector<int>& start_X, const std::vector<int>& start_Y, int targetX, int targetY) {
        int totalPotential = 0;

        for (size_t i = 0; i < start_X.size(); ++i) {
            int agentPotential = calculateAgentPotential(x, y, start_X[i], start_Y[i]);
            totalPotential += agentPotential;
        }

        int goalPotential = calculateGoalPotential(x, y, targetX, targetY);
        totalPotential += goalPotential;

        return totalPotential;
    }

    static int manhattanDistance(int x1, int y1, int x2, int y2) {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }
};

// "важно не столько то, чем пахнут собачьи локти, сколько производство индийских трусов"

int main() {
    sf::RenderWindow window(sf::VideoMode(400, 400), "Multi-Agent Pathfinding");

    std::vector<std::vector<int>> mapData = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    Map map(10, 10, mapData);

    // Инициализация SFML фигур и цветов агентов
    sf::Color darkRed(255, 139, 139);   // Темно-красный
    sf::Color darkGreen(139, 255, 139); // Темно-зеленый
    sf::Color darkBlue(139, 139,255);   // Темно-синий
    sf::Color gray(170, 170, 170);      // Серый
    sf::CircleShape agentShapes[agentCount]; // Массив точек начальных координат агентов
    sf::CircleShape targetShapes[agentCount]; // Массив точек конечных координат агентов
    sf::Color agentColors[agentCount] = {sf::Color::Red, sf::Color::Green, sf::Color::Blue}; // Массив цветов начальных точке агентов
    sf::Color targetColors[agentCount] = {sf::Color::Red, sf::Color::Green, sf::Color::Blue}; // Массив цветов конечных точекагентов
    sf::Color pathColors[agentCount] = {darkRed, darkGreen, darkBlue}; // Массив цветов путей агентов


    for (int i = 0; i < agentCount; ++i) {
        // Назаначение фигур начальных координат агентов
        agentShapes[i] = sf::CircleShape(6); // Изменение формы на круг радиусом 2
        agentShapes[i].setFillColor(agentColors[i]);

        // Назаначение фигур конечных координат агентов
        targetShapes[i] = sf::CircleShape(4); // Изменение формы на круг радиусом 2
        targetShapes[i].setFillColor(sf::Color::Yellow);
        targetShapes[i].setOutlineThickness(2);
        targetShapes[i].setOutlineColor(agentColors[i]);
    }

    //std::vector<std::vector<bool>> occupied(map.getWidth(), std::vector<bool>(map.getHeight(), false));

    // Цикл визуализации
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                //break;
            }
        }

        window.clear();

        // Визуализация карты
        sf::RectangleShape tile(sf::Vector2f(40, 40));
        sf::CircleShape dot(2);
        dot.setFillColor(gray);
        for (int i = 0; i < mapWidth; ++i) {
            for (int j = 0; j < mapHeight; ++j) {
                if (map.getData()[i][j] == 1) {
                    tile.setFillColor(sf::Color::Black);
                } else {
                    tile.setFillColor(sf::Color::White);
                    dot.setPosition(i * 40 + 20, j * 40 + 20);
                }
                tile.setPosition(i * 40, j * 40);
                window.draw(tile);
                window.draw(dot);
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////// Начальные и конечные координаты
        // Визуализация путей агентов
        std::vector<int> startX = {1, 5, 4};
        std::vector<int> startY = {1, 5, 7};
        std::vector<int> targetX = {1, 6, 0};
        std::vector<int> targetY = {9, 6, 7};

        std::vector<std::vector<bool>> occupied(map.getWidth(), std::vector<bool>(map.getHeight(), false));

        // Алгоритм A*
        // auto allPaths = AStar::findPaths(startX, startY, targetX, targetY, map, occupied);

        // Алгоритм потенциальных полей
        auto allPaths = PotentialFields::findPaths(startX, startY, targetX, targetY, map, occupied);

        // Обновление массива occupied

        for (auto &path : allPaths) {
            for (auto &node : path) {
                occupied[node.first][node.second] = true;
            }
        }

        for (size_t i = 0; i < allPaths.size(); ++i) {
            for (auto &node : allPaths[i]) {
                sf::CircleShape pathTile(4);
                pathTile.setFillColor(pathColors[i]);
                pathTile.setPosition(node.first * 40 + 19, node.second * 40 + 19);
                window.draw(pathTile);
            }
            
            // Визуализация начальных координат агентов
            agentShapes[i].setPosition(startX[i] * 40 + 18, startY[i] * 40 + 18);
            window.draw(agentShapes[i]);

            // Визуализация конечных координат агентов
            targetShapes[i].setPosition(targetX[i] * 40 + 18, targetY[i] * 40 + 18);
            window.draw(targetShapes[i]);
        }

        // Визуализация путей и агентов для потенциальных полей
        // for (size_t i = 0; i < allPaths.size(); ++i) {
        //     for (const auto& node : allPaths[i]) {
        //         sf::CircleShape pathTile(4);
        //         pathTile.setFillColor(pathColors[i]);
        //         pathTile.setPosition(node.first * 40 + 19, node.second * 40 + 19);
        //         window.draw(pathTile);
        //     }

        //     // Визуализация начальных координат агентов для потенциальных полей
        //     agentShapes[i].setPosition(startX[i] * 40 + 18, startY[i] * 40 + 18);
        //     window.draw(agentShapes[i]);

        //     // Визуализация конечных координат агентов для потенциальных полей
        //     targetShapes[i].setPosition(targetX[i] * 40 + 18, targetY[i] * 40 + 18);
        //     window.draw(targetShapes[i]);
        // }

        window.display();

    }

    return 0;
}