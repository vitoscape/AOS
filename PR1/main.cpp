#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <stdint.h>
#include "main.hpp"

using namespace std;



std::vector<std::vector<bool>> map(MAP_WIDTH, std::vector<bool>(MAP_HEIGHT, 0));  // Карта (вектор векторов)


/////////////////////////////////////////////////////////////////////////////////// Координаты агентов ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Координаты агентов задаются в виде массива из двух элементов:
 * 0-й элемент — координата X,
 * 1-й элемент — координата Y.
 */
/* Начальные координаты агентов */
static const uint8_t A1_START[2] = {1, 1};     // Агент 1
static const uint8_t A2_START[2] = {3, 3};     // Агент 2
static const uint8_t A3_START[2] = {5, 5};     // Агент 3

/* Конечные координаты агентов */
static const uint8_t A1_FINISH[2] = {2, 2};    // Агент 1
static const uint8_t A2_FINISH[2] = {4, 4};    // Агент 2
static const uint8_t A3_FINISH[2] = {6, 6};    // Агент 3



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


int main() {
    /////////////////////////////////////////////////////////////////////////////// Создание препятствий /////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    map[4][6] = 1;
    map[6][3] = 1;
    

    // Создание агентов
    agent Agent1(A1_START[0], A1_START[1], A1_FINISH[0], A1_FINISH[1]);
    agent Agent2(A2_START[0], A2_START[1], A2_FINISH[0], A2_FINISH[1]);
    agent Agent3(A3_START[0], A3_START[1], A3_FINISH[0], A3_FINISH[1]);

    

    return 0;
}