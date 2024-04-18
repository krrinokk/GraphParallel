#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <fstream>
#include <omp.h>

using namespace std;

int main()
{
    double start_time = omp_get_wtime(); // замеряем начальное время выполнения

    int num_vertices; // количество вершин
    vector<int> parent; // вектор предков
    int big_num(10000);
    int** matrix; // матрица весов

    // считываем матрицу весов из файла
    ifstream fin("matrix.txt");
    fin >> num_vertices; // считываем количество вершин
    matrix = new int* [num_vertices];
    for (int i = 0; i < num_vertices; ++i) {
        matrix[i] = new int[num_vertices];
        for (int j = 0; j < num_vertices; ++j) {
            fin >> matrix[i][j];
        }
    }
    fin.close();

    parent.resize(num_vertices, -1); // устанавливаем размер вектора предков

    int* pos = new int[num_vertices];
    bool* node = new bool[num_vertices];
    int min(0), index_min(0);
    for (int i = 0; i < num_vertices; ++i) {
        pos[i] = big_num;
        node[i] = false;
    }
    for (int i = 0; i < num_vertices; ++i) {
        for (int j = 0; j < num_vertices; ++j) {
            cout << setw(4) << matrix[i][j];
        }
        cout << "\n";
    }
    pos[0] = 0; // начальная вершина
    cout << "\n";

#pragma omp parallel num_threads(num_vertices-1)
    {
        int thread_num = omp_get_thread_num();
        for (int i = 0; i < num_vertices - 1; ++i) {
            if (thread_num == i) {
                min = big_num;
                for (int j = 0; j < num_vertices; ++j) {
                    if (!node[j] && pos[j] < min) {
                        min = pos[j];
                        index_min = j;
                    }
                }
                node[index_min] = true;
                for (int j = 0; j < num_vertices; ++j) {
                    if (!node[j] && matrix[index_min][j] > 0 && pos[index_min] != big_num && pos[index_min] + matrix[index_min][j] < pos[j]) {
                        pos[j] = pos[index_min] + matrix[index_min][j];
                        parent.at(j) = index_min; // запоминаем предка вершины j
                    }
                }
            }
#pragma omp barrier
        }
    }

    int n(0);
    cout << "\nnumber of top? : ";
    cin >> n;

    if (n < 1 || n > num_vertices) { // проверяем, существует ли введенная вершина
        cout << "Vertex does not exist";
        return 0;
    }

    vector<int>temp; // n - 1, так как не забываем про индексацию
    for (int i = n - 1; i != -1; i = parent.at(i))temp.push_back(i + 1);
    reverse(temp.begin(), temp.end());
    for (int i = 0; i < temp.size(); ++i)cout << temp.at(i) << " ";

    cout << "\nWeight : " << pos[n - 1] << "\n";

    // записываем результаты в файл
    ofstream fout("result.txt", std::ofstream::app); // открываем файл на запись в режиме дозаписи
    fout << "Shortest path from vertex 1 to vertex " << n << ":\n";
    for (int i = 0; i < temp.size(); ++i) {
        fout << temp.at(i) << " ";
    }
    fout << "\nWeight: " << pos[n - 1] << "\n";

    double end_time = omp_get_wtime(); // замеряем конечное время выполнения
    double elapsed_time = end_time - start_time; // вычисляем время выполнения в секундах

    fout << "Elapsed time: " << elapsed_time << " seconds" << endl; // записываем время выполнения в файл
    fout.close(); // закрываем файл

    // освобождаем память
    for (int i = 0; i < num_vertices; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
    delete[] pos;
    delete[] node;

    cout << endl;
    return 0;
}