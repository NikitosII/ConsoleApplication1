#include <iostream>
#include <iomanip>
#include "Windows.h"

using namespace std;

// Объединение для хранения числа типа double в виде двух беззнаковых целых
typedef union
{
    unsigned long arr[2];
    double num;
} SaveDouble;

// Функция для печати двоичного представления целого числа
// Заполняет буфер битами числа и выводит их на экран
int PrintBinInt(unsigned long* buff, unsigned long n, int size)
{
    for (int i = size - 1; i >= 0; i--)
    {
        buff[size - 1 - i] = (n & (1ULL << i)) ? 1 : 0; // Заполнение буфера битами числа
        cout << buff[size - 1 - i]; // Вывод бита
    }
    return 0;
}

// Функция для преобразования двоичного представления в беззнаковое целое число
// Преобразует биты из буфера в беззнаковое целое число
unsigned long BinToUint(unsigned long* buff, unsigned long size, unsigned long offset) {
    unsigned long result = 0;
    for (unsigned long i = 0; i < size; i++) {
        result |= (buff[i + offset] << (size - 1 - i));
    }
    return result;
}

// Функция для преобразования двоичного представления в целое число
// Преобразует биты из буфера в целое число, учитывая знак
long long BinToInt(unsigned long* buff, unsigned long size) {
    if (buff[0] == 1) { // Если старший бит 1, то это отрицательное число
        unsigned long ConverInt = BinToUint(buff, size - 1, 1); // Пропускаем старший бит
        ConverInt = ~ConverInt + 1; // Преобразование в отрицательное число
        return -static_cast<long long>(ConverInt);
    }
    else {
        return static_cast<long long>(BinToUint(buff, size - 1, 1)); // Вывод положительного числа
    }
}

// Функция для печати двоичного представления числа типа double
// Заполняет буфер битами числа типа double и выводит их на экран
int PrintBinDouble(unsigned long* buff, SaveDouble f)
{
    unsigned long tmp_1[64], tmp_2[64];
    PrintBinInt(tmp_1, f.arr[1], 64);
    PrintBinInt(tmp_2, f.arr[0], 64);
    for (int i = 0; i < 64; i++) buff[i] = tmp_1[i];
    for (int i = 64; i < 128; i++) buff[i] = tmp_2[i - 64];
    return 0;
}

// Функция для преобразования двоичного представления в число типа double
// Преобразует биты из буфера в число типа double, учитывая знак, порядок и мантиссу
int BinToDouble(unsigned long* buff, SaveDouble& f) {
    // Получаем знаковый бит
    unsigned long sign = buff[63]; // 1 бит (старший)

    // Получаем порядок (11 бит)
    unsigned long exponent = BinToUint(buff, 11, 52);

    // Получаем мантиссу (52 бита)
    unsigned long mantissa = BinToUint(buff, 52, 0);

    // Преобразование порядка в десятичное значение
    int exponentBias = 1023; // Смещение для формата IEEE 754
    int exponentValue = static_cast<int>(exponent) - exponentBias;

    // Если порядок равен 2047, это NaN или бесконечность
    if (exponent == 2047) {
        f.num = (mantissa == 0) ? (sign ? -INFINITY : INFINITY) : NAN;
        return 0;
    }

    // Если порядок равен 0, это либо ноль, либо денормализованное число
    if (exponent == 0) {
        if (mantissa == 0) {
            f.num = sign ? -0.0 : 0.0;
            return 0;
        }
        // Денормализованное число
        exponentValue = -exponentBias + 1; // Порядок для денормализованных чисел
    }

    // Создаем значение double
    double mantissaValue = 1.0; // Добавляем единицу для нормализованного числа
    for (int i = 51; i >= 0; --i) {
        if ((mantissa >> i) & 1) {
            mantissaValue += pow(2, -static_cast<double>(51 - i));
        }
    }

    // Учитываем знак
    f.num = (sign ? -1 : 1) * mantissaValue * pow(2, exponentValue);

    return 0;
}

// Функция для изменения битов в буфере
// Сдвигает все нулевые биты вправо и все единичные биты влево
int ChangeBit(unsigned long* buff, unsigned long size) {
    if (buff == nullptr) return 1;

    // Создаем временные массивы для хранения новых значений
    unsigned long* newBuff = new unsigned long[size];

    // Счетчики для заполнения нового массива
    int leftIndex = 0; // Индекс для заполнения единицами
    int rightIndex = size - 1; // Индекс для заполнения нулями

    // Проходим по всем битам и заполняем новый массив
    for (unsigned long i = 0; i < size; i++) {
        if (buff[i] == 1) {
            newBuff[leftIndex++] = 1; // Сдвигаем единицы влево
        }
        else {
            newBuff[rightIndex--] = 0; // Сдвигаем нули вправо
        }
    }

    // Заполнение оставшихся мест нулями
    while (leftIndex < size) {
        newBuff[leftIndex++] = 0;
    }

    // Копируем новый массив обратно в оригинальный
    for (unsigned long i = 0; i < size; i++) {
        buff[i] = newBuff[i];
    }

    delete[] newBuff; // Освобождаем память
    return 0;
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    unsigned long choice, check;
    char c;
    cout << "Пожалуйста, выберите тип данных:";
    cout << "\n1.Unsigned long";
    cout << "\n2.Double";
    cout << "\n--> ";
    do
    {
        check = scanf_s("%lu%c", &choice, &c, 1);
        if (check != 2 || c != '\n' || (choice < 1 || choice > 2))
            cout << "\nError, re-enter: ";
        fflush(stdin);
    } while (check != 2 || c != '\n' || choice < 1 || choice > 2);

    switch (choice)
    {
    case 1:
        system("cls"); // Очистка консоли
        unsigned long num;

        cout << "\nEnter num (unsigned long): ";
        do
        {
            check = scanf_s("%lu%c", &num, &c, 1);
            if (check != 2 || c != '\n' || (long)num < 0)
                cout << "\nError, re-enter: ";
            fflush(stdin);
        } while (check != 2 || c != '\n' || (long)num < 0);

        cout << "\nДвоичное представление числа:\n";
        unsigned long buff_1[64];
        PrintBinInt(buff_1, num, 64);

        if (ChangeBit(buff_1, 64) != 0)
            return 1;
        cout << "\n--> После изменения:";
        cout << "\n1.Binary form:\n";
        for (unsigned long i = 0; i < 64; i++)
            cout << buff_1[i];
        cout << "\n2.Integer form:\n";
        cout << BinToInt(buff_1, 64);
        break;

    case 2:
        system("cls"); // Очистка консоли
        SaveDouble num_db;

        cout << "\nEnter num (double): ";
        do
        {
            check = scanf_s("%lf%c", &num_db.num, &c, 1);
            if (check != 2 || c != '\n')
                cout << "\nError, re-enter: ";
            fflush(stdin);
        } while (check != 2 || c != '\n');
        cout << "\nДвоичное представление числа:\n";

        unsigned long buff_2[128];
        PrintBinDouble(buff_2, num_db);

        if (ChangeBit(buff_2, 128) != 0)
            return 2;
        cout << "\n--> После изменения:";
        cout << "\n1.Binary form:\n";
        for (unsigned long i = 0; i < 128; i++)
            cout << buff_2[i];
        cout << "\n2.Double number form:\n";
        if (BinToDouble(buff_2, num_db) == 0)
            cout << "Decimal: " << num_db.num << endl;
        break;
    default:
        break;
    }
    return 0;
}