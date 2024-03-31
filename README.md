# Множество Мандельброта, или значимость оптимизации

## Множество Мадельброта
 \- такое множество точек на комплексной плоскости, для которых рекуррентное соотношение $z_{n+1} = z_{n} ^2 + c$ задаёт ограниченную последовательность.
Иными словами можно сказать, что у каждой начальной точки выстраивается последовательность точек, координаты которых равны соответсвенно : $x_{n+1} = x_{n} ^ 2 - y_{n} ^ 2 + x_0$ и $\ y_{n+1} = 2 \cdot x_n y_n  + y_0$.

Чтобы множество Мадельброта визуализировать, будем окрашивать точку в завасимости от номера итераций, когда последовательность выйдет за определенную область.

Один из примеров множества Мадельброта:

![pic1](https://github.com/ask0later/mandelbrot/blob/5c498ba4b698f384ce838ecf5e7c3c02721d1169/image/mandelbrot.png)

Посчитать цвет каждой точки всей плоскость требует определенной вычислительной мощности. Алгоритмическая сложность составляет $O(n^3)$. Поэтому хотелось бы оптимизировать нашу программу, и понять на сколько она может ускорится в выполнении. Сравнивать разные этапы оптимизаций будем по количеству кадров в секунду (frame per second) FPS. Измерения будем проводитm в одинаковых условиях (заряд батареи и видимый экран совпадают).

## Вычисление номера итерации для отдельного пикселя
~~~C++
for (int index_y = 0; index_y < HEIGHT; index_y++)
    {
        float y_0 = ((float) (HEIGHT / 2) - (float) index_y + (float) set->offset_y) * dy * set->scale;

        for (int index_x = 0; index_x < WIDTH; index_x++)
        {
            float x_0 = ((float) index_x - (float) (WIDTH / 2) + (float) set->offset_x) * dx * set->scale;

            float x = x_0, y = y_0;
            
            int step = 0;
            for (; (step < MAX_STEPS) && ((x * x) + (y * y) < MAX_RADIUS_2); step++)
            {
                float tmp = x;

                x = x * x - y * y + x_0;
                y = 2 * (tmp * y) + y_0;
            }

            FillPixel(pixels, &index_pixel, step);
        }
    }
~~~
FPS: 2.

## Использование AVX2
Заметим, что для каждого пикселя выполняются одни и те же арифметические операции. Обеспечить повышение производительсти можно с помощью SIMD (single instruction, multiple data). Это принцип компьютерных вычислений, который обеспечивает параллельное выполнение на уровне данных.
AVX2 (Advanced Vector Extensions) — система команд для архитектуры x86 для микропроцессоров Intel и AMD, придерживающаяся принципу SIMD.

Тогда некоторые операции будут выглядеть так:
~~~ C++
x = _mm256_add_ps(_mm256_sub_ps(x2, y2), x_0);
y = _mm256_add_ps(_mm256_add_ps(xy, xy), y_0);
~~~
FPS: 4.

Переменные являются массивом из элементов типа float общим размером 256 бит.


## Сравнение ФПС
Для каждого случая запустим программу с флагом -O3 (оптимизация) и без него.

|             | No flag  -O3      | -O3            |
| ------      | :---------------: | :------------: |
| No AVX      |        2          |        4       |
| AVX         |        4          |       30       |


Сделаем вывод, что встроенный оптимизатор не видит использования инструкций SIMD. Когда же мы используем их, то компилятор видит те места кода, где требуется оптимизация, и улучшает производительность программы. В конечном итоге мы получили прирост в 15 раз.