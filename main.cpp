extern "C" {
#include <python3.5/Python.h>
}
#include <cstdio>
#include <omp.h>
#include <string>
#include <vector>
#include <future>
#include <regex>

using namespace std;

void pic_results(double arr[], int size) {
    string dir_path = "/home/levon/ClionProjects/parallel_omp/";
    FILE* python = fopen((dir_path + "imaging_script.py").c_str(), "r");

    Py_Initialize();
    PyObject* list = PyList_New(0);

    for (int i = 0; i < size; ++i) {
        PyList_Append(list, PyFloat_FromDouble(arr[i]));
    }


    PyObject* pDict = PyDict_New();
    PyDict_SetItemString(pDict, "__builtins__", PyEval_GetBuiltins());
    PyRun_String("import matplotlib.pyplot as plt", Py_file_input, pDict, pDict);
    PyDict_SetItemString(pDict, "result", list);
    PyDict_SetItemString(pDict, "size", PyLong_FromLong(size));
    PyRun_File(python, "imaging_script.py", Py_file_input, pDict, pDict);

    Py_Finalize();
    fclose(python);
}

const int max_thread_num = 12;
const int max_try_num = 3;
#define B

int main() {
    const int N = 1E9;
    double factor, sum, min_time;
    omp_sched_t s;
    int size;
    double time[3][9];

    ::omp_set_dynamic(0);
    for (auto sched_type : {omp_sched_guided}) {
        for (auto chunk_size : {16}) {
            ::omp_set_schedule(sched_type, chunk_size);
            for (int thread_count = 2; thread_count <= max_thread_num; ++thread_count) {
                printf("schedule:%d chunk_size:%d\n", sched_type, chunk_size);
                ::omp_set_num_threads(thread_count);
                min_time = 100;
                for (int k = 0; k < max_try_num; ++k) {
                    double start = omp_get_wtime();

                #ifdef A
                    factor = 1.0;
                    sum = 0.0;

                    #pragma omp parallel for reduction(+:sum)
                    for (int k = 0; k < N; k += 2) {
                        sum += factor / (2 * k + 1);
                    }

                    factor = -factor;

                    #pragma omp parallel for reduction(+:sum)
                    for (int k = 1; k < N; k += 2) {
                        sum += factor / (2 * k + 1);
                    }
                    double pi = 4.0 * sum;
                #elif defined(B)
                    const int N = 2E5;
                    int total = 0;
                    int prime, i, j;

                    #pragma omp parallel for private(i,j,prime) reduction(+:total)
                    for (i = 3; i <= N; i += 2) {
                        prime = 1;
                        for (j = 3; j * 2 <= i; j += 2) {
                            if (i % j == 0) {
                                prime = 0;
                                break;
                            }
                        }
                        #pragma omp atomic
                        total = total + prime;
                    }
                #endif

                    double elapsed = omp_get_wtime() - start;

                    if (elapsed < min_time) {
                        min_time = elapsed;
                    }

                #ifdef A
                    printf("Pi = %.16f, ", pi);
                #elif defined(B)
                    printf("Total: %d, ", total);
                #endif

                    printf("time = %.4f, thread_num = %d\n", elapsed, thread_count);
                }
                time[0][thread_count - 2] = min_time;
                printf("min_time is %.4f\n\n", min_time);
            }

            pic_results(time[0], max_thread_num - 1);
        }
    }
    return 0;
}