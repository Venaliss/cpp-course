/**
 * @file main.cpp
 * @brief задача написать программу, которая будет высчитывать значение интеграла через метод трапеций
 * Особенность этой задачи: использовать потоки(threads) для ускорения вычислений (нужно разделить задачу для вычисления части интеграла на каждый поток)
 *
 * на вход программе при запуске(это в argv[]) подается 3 числа: a, b, n, tn где [a, b] - интервал (целочисленный, неотрицательный и не больше 50 для задачи), n - количество разбиений, tn - threads number - количество потоков для подсчёта
 * примечание: n - количество разбиений в тестах будет явно нацело делиться на tn - количество потоков.
 *
 * на выход программе выведите значение интеграла через метод трапеций (вывод в стандартный поток std::cout, в конце выведите '\n'), вывод в поток с точностью до 4 знака
 * в функции trapezoidalIntegral есть переменная const std::function<double (double)> &f, при вызове подсчётов передайте ссылку на функцию из задания
 * реализовать подсчёт интеграла S(a, b) = (1+e^x)^0.5 dx
 * 
 *
 * литература:
 * https://ru.wikipedia.org/wiki/Метод_трапеций
 * https://habr.com/ru/articles/420867/
 */

#include <cmath>
#include <functional>
#include <numeric>
#include <vector>
#include <thread>
#include <iostream>
#include <algorithm>
#include <iomanip>


class Integral {
private:
    int a, b, n, tn;

public:
    Integral(int& argc, char** argv) {
        if (argc != 5) {
            throw std::invalid_argument("Wrong number of arguments");
        }
        a  = std::stoi(argv[1]);
        b  = std::stoi(argv[2]);
        n  = std::stoi(argv[3]);
        tn = std::stoi(argv[4]);
    }


    static double integralFunction(double x) {
        // тут нужно реализовать функцию интеграла S(a, b) = (1+e^x)^0.5 dx
        return std::sqrt(1 + std::exp(x));
        return 0;
    }


    double calculateIntegral() {
        // в зависимости от количество потоков (tn) реализуйте подсчёт интеграла
        // шаг
        double h = (b - a) / static_cast<double>(n);

        const std::function<double(double)> f = Integral::integralFunction;

        std::vector<double> results(tn, 0.0);
        std::vector<std::thread> threads;
        // кол-во трапеций, в каждом потоке
        int chunk = n / tn;

        // создание потоков
        for (int t = 0; t < tn; ++t) {
            int start = t * chunk;           // индекс первой трапеции
            int end = start + chunk - 1;       // индекс последней трапеции
            int thread_index = t;              // копия номера потока для безопасного захвата лямбдой
            threads.emplace_back([this, start, end, h, f, &results, thread_index]() {
                double localSum = 0.0;
                // каждый поток обрабатывает свой набор трапеций, где для i-й трапеции x0 = a + i * h и x1 = a + (i+1) * h
                for (int i = start; i <= end; ++i) {
                    double x0 = a + i * h;
                    double x1 = a + (i + 1) * h;
                    localSum += (f(x0) + f(x1)) / 2.0;
                }
                results[thread_index] = localSum * h;
            });
        }

        for (auto &thr : threads) {
            thr.join();
        }

        double total = 0.0;
        for (const auto &val : results) {
            total += val;
        }
        return total;
    }

};



int main(int argc, char** argv)
{
    auto i = Integral(argc, argv);
    std::cout << std::fixed << std::setprecision (4);
    std::cout << i.calculateIntegral() << std::endl;
    return 0;
}