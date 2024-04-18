
#include <iostream>
#include <vector>
#include <set>

using namespace std;

// Структура для хранения информации о вершине графа
struct Vertex {
    int id; // идентификатор вершины
    vector<int> outEdges; // исходящие ребра
    vector<int> inEdges; // входящие ребра
};

// Функция для проверки, является ли множество вершин слабо связанным
bool isWeaklyConnected(set<int>& vertices, vector<Vertex>& graph) {
    set<int> visited; // множество посещенных вершин
    vector<int> stack; // стек для обхода в глубину
    stack.push_back(*vertices.begin()); // добавляем первую вершину в стек
    while (!stack.empty()) {
        int curVertex = stack.back(); // берем вершину с вершины стека
        stack.pop_back(); // удаляем ее из стека
        visited.insert(curVertex); // добавляем ее в множество посещенных вершин
        for (int i = 0; i < graph[curVertex].outEdges.size(); i++) {
            int nextVertex = graph[curVertex].outEdges[i]; // берем следующую вершину
            if (vertices.count(nextVertex) && !visited.count(nextVertex)) {
                // если вершина принадлежит множеству и еще не была посещена, добавляем ее в стек
                stack.push_back(nextVertex);
            }
        }
    }
    // если все вершины множества были посещены, то множество слабо связано
    return visited == vertices;
}

// Функция для нахождения всех черных дыр в графе размером от 1 до n
set<set<int>> findBlackHoles(vector<Vertex>& graph, int n) {
    set<set<int>> blackHoles; // множество черных дыр
    set<int> candidates; // список кандидатов
    set<int> prevCandidates; // список кандидатов на предыдущем шаге
    for (int i = 1; i <= n; i++) {
        set<int> pi; // множество вершин с количеством исходящих ребер, меньшим i
        for (int j = 0; j < graph.size(); j++) {
            if (graph[j].outEdges.size() < i) {
                pi.insert(j);
            }
        }
        // удаляем из списка кандидатов вершины, которые не принадлежат множеству pi
        for (auto it = candidates.begin(); it != candidates.end(); ) {
            if (pi.count(*it) == 0) {
                it = candidates.erase(it);
            }
            else {
                it++;
            }
        }
        // удаляем из списка кандидатов вершины, у которых есть потомки, не принадлежащие множеству pi
        for (auto it = candidates.begin(); it != candidates.end(); ) {
            bool hasNonPiChild = false;
            for (int j = 0; j < graph[*it].outEdges.size(); j++) {
                int child = graph[*it].outEdges[j];
                if (pi.count(child) == 0) {
                    hasNonPiChild = true;
                    break;
                }
            }
            if (hasNonPiChild) {
                it = candidates.erase(it);
                // удаляем всех предков вершины из списка кандидатов
                int prev = *it;
                while (prev != -1) {
                    prevCandidates.erase(prev);
                    prev = graph[prev].inEdges.size() > 0 ? graph[prev].inEdges[0] : -1;
                }
            }
            else {
                it++;
            }
        }
        // добавляем в список кандидатов вершины из множества pi, которые еще не были добавлены на предыдущем шаге
        for (auto it = pi.begin(); it != pi.end(); it++) {
            if (prevCandidates.count(*it) == 0) {
                candidates.insert(*it);
            }
        }
        prevCandidates = candidates; // сохраняем список кандидатов на текущем шаге
        // проверяем каждую вершину из списка кандидатов
        for (auto it = candidates.begin(); it != candidates.end(); ) {
            set<int> closure; // множество потомков вершины
            closure.insert(*it); // добавляем саму вершину
            // находим все потомки вершины
            for (int j = 0; j < graph[*it].outEdges.size(); j++) {
                int child = graph[*it].outEdges[j];
                closure.insert(child);
                for (int k = 0; k < graph[child].outEdges.size(); k++) {
                    closure.insert(graph[child].outEdges[k]);
                }
            }
            if (closure.size() == i) {
                // если размер множества потомков равен i, то добавляем его в множество черных дыр
                blackHoles.insert(closure);
                it = candidates.erase(it);
                // удаляем всех предков вершины из списка кандидатов
                int prev = *it;
                while (prev != -1) {
                    prevCandidates.erase(prev);
                    prev = graph[prev].inEdges.size() > 0 ? graph[prev].inEdges[0] : -1;
                }
            }
            else if (closure.size() > i) {
                // если размер множества потомков больше i, то удаляем вершину из списка кандидатов
                it = candidates.erase(it);
                // удаляем всех предков вершины из списка кандидатов
                int prev = *it;
                while (prev != -1) {
                    prevCandidates.erase(prev);
                    prev = graph[prev].inEdges.size() > 0 ? graph[prev].inEdges[0] : -1;
                }
            }
            else {
                it++;
            }
        }
        // применяем алгоритм полного перебора ко всем подмножествам множества Ci(i)
        for (auto it = candidates.begin(); it != candidates.end(); it++) {
            set<int> b; // подмножество множества Ci(i)
            b.insert(*it);
            for (int j = 0; j < graph[*it].inEdges.size(); j++) {
                b.insert(graph[*it].inEdges[j]);
            }
            if (b.size() == i && isWeaklyConnected(b, graph) && graph[*it].outEdges.size() == 0) {
                // если множество слабо связано и имеет нулевую исходящую степень, то добавляем его в множество черных дыр
                blackHoles.insert(b);
            }
        }
    }
    return blackHoles;
}

int main() {
    int n, m;
    cin >> n >> m; // считываем количество вершин и ребер графа
    vector<Vertex> graph(n); // граф задается списком смежности
    for (int i = 0; i < m; i++) {
        int from, to;
        cin >> from >> to; // считываем ребро из вершины from в вершину to
        graph[from].outEdges.push_back(to);
        graph[to].inEdges.push_back(from);
    }
    set<set<int>> blackHoles = findBlackHoles(graph, n); // находим все черные дыры в графе
    for (auto it = blackHoles.begin(); it != blackHoles.end(); it++) {
        // выводим каждую черную дыру на отдельной строке
        for (auto jt = it->begin(); jt != it->end(); jt++) {
            cout << *jt << " ";
        }
        cout << endl;
    }
    return 0;
}

//На вход программе сначала подается количество вершин и ребер графа, а затем ребра по одному на строку в формате "from to".Программа выводит на экран каждую черную дыру в графе на отдельной строке.