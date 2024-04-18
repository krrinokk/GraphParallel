#include <iostream>
#include <fstream>
#include <omp.h>
#define INF 999

using namespace std;

int main() {
    int nV;
    ifstream file("matrix.txt");
    if (!file) {
        cout << "File not found!\n";
        return 1;
    }
    file >> nV;
    int graph[10][10];
    for (int i = 0; i < nV; i++) {
        for (int j = 0; j < nV; j++) {
            file >> graph[i][j];
            if (graph[i][j] == -1) // заменяем -1 на INF
                graph[i][j] = INF;
        }
    }
    file.close();

    int matrix[10][10], i, j, k;

    for (i = 0; i < nV; i++)
        for (j = 0; j < nV; j++)
            matrix[i][j] = graph[i][j];

    cout << "Initial weight matrix:\n";
    for (int i = 0; i < nV; i++) {
        for (int j = 0; j < nV; j++) {
            if (matrix[i][j] == INF)
                cout << "INF ";
            else
                cout << matrix[i][j] << " ";
        }
        cout << "\n";
    }
    double start_time = omp_get_wtime();
#pragma omp parallel for private(k, i, j) shared(matrix)
    for (k = 0; k < nV; k++) {
        for (i = 0; i < nV; i++) {
            for (j = 0; j < nV; j++) {
                if (matrix[i][k] != INF && matrix[k][j] != INF && matrix[i][k] + matrix[k][j] < matrix[i][j])
                    matrix[i][j] = matrix[i][k] + matrix[k][j];
            }
        }
    }

    cout << "Weight matrix after Floyd-Warshall algorithm:\n";
    for (int i = 0; i < nV; i++) {
        for (int j = 0; j < nV; j++) {
            if (matrix[i][j] == INF)
                cout << "INF ";
            else
                cout << matrix[i][j] << " ";
        }
        cout << "\n";
    }

    int vertex;
    cout << "Enter vertex number: ";
    cin >> vertex;
    vertex--;

    if (vertex < 0 || vertex >= nV) {
        cout << "Invalid vertex number!\n";
        return 0;
    }

    cout << "Shortest paths from vertex " << vertex << ":\n";
    for (int i = 0; i < nV; i++) {
        if (matrix[vertex][i] != INF && i != vertex) {
            cout << "Path to vertex " << i << ": ";
            int curr = i;
            while (curr != vertex) {
                cout << curr << " <- ";
                curr = k;
                for (k = 0; k < nV; k++) {
                    if (matrix[vertex][k] != INF && matrix[k][curr] != INF && matrix[vertex][k] + matrix[k][curr] == matrix[vertex][curr])
                        break;
                }
            }
            cout << vertex << "\n";
            cout << "Weight: " << matrix[vertex][i] << "\n";
        }
        else if (matrix[vertex][i] == INF && i != vertex) {
            cout << "No path from vertex " << vertex << " to vertex " << i << "\n";
        }
    }

    ofstream out("result.txt");
    if (out.is_open()) {
        out << "Weight matrix after Floyd-Warshall algorithm:\n";
        for (int i = 0; i < nV; i++) {
            for (int j = 0; j < nV; j++) {
                if (matrix[i][j] == INF)
                    out << "INF ";
                else
                    out << matrix[i][j] << " ";
            }
            out << "\n";
        }
        out << "Shortest paths from vertex " << vertex << ":\n";
        for (int i = 0; i < nV; i++) {
            if (matrix[vertex][i] != INF && i != vertex) {
                out << "Path to vertex " << i << ": ";
                int curr = i;
                while (curr != vertex) {
                    out << curr << " <- ";
                    curr = k;
                    for (k = 0; k < nV; k++) {
                        if (matrix[vertex][k] != INF && matrix[k][curr] != INF && matrix[vertex][k] + matrix[k][curr] == matrix[vertex][curr])
                            break;
                    }
                }
                out << vertex << "\n";
                out << "Weight: " << matrix[vertex][i] << "\n";
            }
            else if (matrix[vertex][i] == INF && i != vertex) {
                out << "No path from vertex " << vertex << " to vertex " << i << "\n";
            }
        }
        double end_time = omp_get_wtime();
        double time = end_time - start_time;
        out << "Execution time: " << time << " sec\n";
        out.close();
        cout << "Results saved to result.txt\n";
    }
    else {
        cout << "Error opening file!\n";
    }
    return 0;
}