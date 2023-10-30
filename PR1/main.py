import networkx as nx
import matplotlib.pyplot as plt
import numpy as np
import time

# Создание графа
G = nx.Graph()

# Размер карты
map_width = 10
map_height = 10

# Параметры для алгоритма потенциальных полей
k_att = 0.2     # Коэффициент притяжения к целевой точке
k_rep = 1.0     # Коэффициент отталкивания от других агентов
rep_radius = 1  # Радиус воздействия отталкивания

# Список начальных координат агентов и их целевых точек
agents = [
    {"start": (8, 4), "target": (2, 1)},
    {"start": (9, 1), "target": (5, 6)},
    {"start": (8, 2), "target": (1, 9)},
]

# Словарь для хранения путей агентов и их цветов
agent_paths = {}

# Задание цветов агентам
agent_colors = {
    "Agent 1": "olive",
    "Agent 2": "lightsalmon",
    "Agent 3": "mediumpurple",
}

# Добавление вершин (узлов) графа для каждой координаты на карте
for x in range(map_width):
    for y in range(map_height):
        G.add_node((x, y))

# Добавление ребра (связи) между соседними вершинами (координатами)
for x in range(map_width):
    for y in range(map_height):
        # Добавляем ребра соседних вершин, ограничиваясь манхэттенским расстоянием 1
        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                if abs(dx) + abs(dy) == 1:  # Манхэттенское расстояние 1
                    new_x, new_y = x + dx, y + dy
                    if 0 <= new_x < map_width and 0 <= new_y < map_height:
                        G.add_edge((x, y), (new_x, new_y))

# Функция для расчета потенциала отталкивания от других агентов
def repulsive_potential(agent_pos, other_agent_pos):
    distance = np.linalg.norm(np.array(agent_pos) - np.array(other_agent_pos))
    if distance <= rep_radius:
        return 0.5 * k_rep * (1 / distance - 1 / rep_radius) ** 2
    else:
        return 0

# Расчет пути для каждого агента с использованием A* или алгоритма дейкстры
start_time = time.time() # Замер времени

for agent in agents:
    start = agent["start"]
    target = agent["target"]
    path = nx.astar_path(G, start, target)      # A*
    #path = nx.shortest_path(G, start, target)   # Дейкстра
    agent_paths[start] = path

end_time = time.time() # Замер времени
alg_1_time = end_time - start_time # Запись результата времени вычисления

# Визуализация путей агентов с использованием A* или Дийкстры
for agent_name, path in zip(agent_colors.keys(), agent_paths.values()):
    x_values, y_values = zip(*path)
    plt.plot(x_values, y_values, color=agent_colors[agent_name])

# Расчет пути для каждого агента с использованием алгоритма потенциальных полей
start_time = time.time() # Замер времени

for agent in agents:
    start = agent["start"]
    target = agent["target"]
    path = [start]
    current_pos = start

    max_iterations = 100  # Максимальное количество итераций для безопасности

    while current_pos != target:
        gradient = np.zeros(2)
        for neighbor in G.neighbors(current_pos):
            gradient += k_att * (np.array(target) - np.array(current_pos))
            gradient -= repulsive_potential(current_pos, neighbor) * (np.array(current_pos) - np.array(neighbor))
        new_pos = tuple(np.round(np.array(current_pos) + gradient).astype(int))
        
        if new_pos == current_pos or max_iterations <= 0:
            break
        
        path.append(new_pos)
        current_pos = new_pos
        max_iterations -= 1

    agent_paths[start] = path

end_time = time.time() # Замер времени
alg_2_time = end_time - start_time # Запись результата времени вычисления

# Визуализация путей агентов с использованием алгоритма потенциальных полей
for agent_name, path in zip(agent_colors.keys(), agent_paths.values()):
    x_values, y_values = zip(*path)
    plt.plot(x_values, y_values, color=agent_colors[agent_name], linestyle='--')

# Визуализация итоговых координат агентов
for agent in agents:
    target = agent["target"]
    agent_name = f"Agent {agents.index(agent) + 1}"
    plt.scatter(*target, color=agent_colors[agent_name], marker="x", s=150, label=f'{agent_name} {target}')
    
print(f"Время выполнения алгоритма A* (Дейкстры): {alg_1_time} секунд")
print(f"Время выполнения алгоритма потенциальных полей: {alg_2_time} секунд")

# Визуализация легенд, сетки, координат
plt.legend()
plt.grid(True)
plt.xticks(range(map_width + 1))
plt.yticks(range(map_height + 1))
plt.show()
