#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <cmath>
#include <stdint.h>
#include "main.hpp"

using namespace std;



// Структура каждого узла карты
struct node {
    int x;          // Координаты узла
    int y;
    // int parentX;    // Родительские координаты
    // int parentY;
    float gCost;    // Стоимость пути от начального узла до данного узла
    float hCost;    // Эвристическая оценка стоимости от этого узла до целевого
    float fCost;    // Суммарная оценка fCost = gCost + hCost

    node (int x, int y, int gCost, int hCost) : x(x), y(y), gCost(gCost), hCost(hCost), fCost(gCost + hCost) {}
};

/* Перегрузка оператора < для сравнения объектов структуры node.
 * Это нужно для того, чтобы объекты структуры node
 * можно было добавлять в set. 
 */
inline bool operator < (const node& lhs, const node& rhs) {
    return lhs.fCost < rhs.fCost;
}



// Карта (вектор векторов)
std::vector<std::vector<int>> grid = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
};

// Направления движения агентов: вверх, вниз
std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

// Проверка на доступность узла
bool isValid(int x, int y) {
   return x >= 0 && x < grid.size() && y >= 0 && y < grid[0].size() && grid[x][y] == 0;
}

// Манхэттенское расстояние
int heuristic(int x, int y, int targetX, int targetY) {
    return std::abs(x - targetX) + std::abs(y - targetY);
}


std::vector<std::pair<int, int>> findPath(std::vector<std::vector<int>>& grid, int startX, int startY, int targetX, int targetY) {
    std::priority_queue<node> openSet;
    std::vector<std::vector<node>> nodes(grid.size(), std::vector<node>(grid[0].size(), node(0, 0, 0, 0)));

    openSet.emplace(startX, startY, 0, heuristic(startX, startY, targetX, targetY));
    nodes[startX][startY] = openSet.top();

    while (!openSet.empty()) {
        node current = openSet.top();
        openSet.pop();

        if (current.x == targetX && current.y == targetY) {
            // Найден путь до цели
            std::vector<std::pair<int, int>> path;
            while (current.x != startX || current.y != startY) {
                path.emplace_back(current.x, current.y);
                int prevX = current.x - directions[0].first;
                int prevY = current.y - directions[0].second;
                current = nodes[prevX][prevY];
            }
            path.emplace_back(startX, startY);
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (const auto& direction : directions) {
            int newX = current.x + direction.first;
            int newY = current.y + direction.second;
            if (isValid(newX, newY)) {
                int tentativeG = current.gCost + 1;
                if (tentativeG < nodes[newX][newY].gCost || nodes[newX][newY].gCost == 0) {
                    nodes[newX][newY] = {newX, newY, tentativeG, heuristic(newX, newY, targetX, targetY)};
                    openSet.emplace(newX, newY, nodes[newX][newY].gCost, nodes[newX][newY].hCost);
                }
            }
        }
    }

    return {}; // Путь не найден
}

void drawPath(sf::RenderWindow& window, const std::vector<std::pair<int, int>>& path) {
    sf::VertexArray lines(sf::LinesStrip, path.size());
    for (size_t i = 0; i < path.size(); ++i) {
        lines[i].position = sf::Vector2f(path[i].first * 50 + 25, path[i].second * 50 + 25);
        lines[i].color = sf::Color::Red;
    }
    window.draw(lines);
}

// /////////////////////////////////////////////////////////////////////////////////// Координаты агентов ///////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /* Координаты агентов задаются в виде массива из двух элементов:
//  * 0-й элемент — координата X,
//  * 1-й элемент — координата Y.
//  */
// /* Начальные координаты агентов */
// static const uint8_t A1_START[2] = {1, 1};     // Агент 1
// static const uint8_t A2_START[2] = {3, 3};     // Агент 2
// static const uint8_t A3_START[2] = {5, 5};     // Агент 3

// /* Конечные координаты агентов */
// static const uint8_t A1_FINISH[2] = {2, 2};    // Агент 1
// static const uint8_t A2_FINISH[2] = {4, 4};    // Агент 2
// static const uint8_t A3_FINISH[2] = {6, 6};    // Агент 3



/*
// Класс агента
class agent {
    uint8_t start_x;    // Начальные координаты агента
    uint8_t start_y;
    uint8_t finish_x;   // Конечные координаты агента
    uint8_t finish_y;
    uint8_t x;          // Координаты агента
    uint8_t y;

public:
    agent(uint8_t st_x, uint8_t st_y, uint8_t fin_x, uint8_t fin_y) {
        start_x = st_x;
        start_y = st_y;
        finish_x = fin_x;
        finish_y = fin_y;
    }
};
*/



int main() {
    int startX = 0, startY = 0;
    int targetX = 4, targetY = 4;

    std::vector<std::pair<int, int>> path = findPath(grid, startX, startY, targetX, targetY);

    sf::RenderWindow window(sf::VideoMode(250, 250), "A* Pathfinding Visualization");
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear(sf::Color::White);
        drawPath(window, path);
        window.display(); 
    }
    
/*
    // Создание агентов
    agent Agent1(A1_START[0], A1_START[1], A1_FINISH[0], A1_FINISH[1]);
    agent Agent2(A2_START[0], A2_START[1], A2_FINISH[0], A2_FINISH[1]);
    agent Agent3(A3_START[0], A3_START[1], A3_FINISH[0], A3_FINISH[1]);
*/
    

    return 0;
}