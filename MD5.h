#include <cmath>                                                                        //Для fabs и sin
#include <string>                                                                       //Для string
#include <iostream>                                                                     //Для cout
#include <sstream>                                                                      //Для ostringstream и функции hex
#include <fstream>                                                                      //Для файлов

using namespace std;                                                                    //Для cout, и файлов

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))                            //Битовый сдвиг на n бит 

class MD5                                                                               //Класс
{
    string message;                                                                     //Исходное сообщение

    unsigned long long bitlen;                                                          //Исходная длина сообщения (до шага 1) в битах
    
    unsigned int **X;                                                                   //Массивы блоков по 64 байта(512 бит)

    unsigned int T[64];                                                                 //Таблица констант
    
    unsigned int blocks;                                                                //Количество блоков для обработки

    unsigned int H0, H1, H2, H3;                                                        //Регистры MD буфера

    unsigned int A, B, C, D, G, f, temp;                                                //Переменные для сохранения значений регистров во время алгоритма

    unsigned int F(unsigned int j, unsigned int x, unsigned int y, unsigned int z);     //Выбор битовой функции в соответсвии с номером цикла
    unsigned int inv(unsigned int value);                                               //Смена порядка бит (endians)
    unsigned int bytes_to_uint(char* bytes);                                            //Преоразование 4х байтов в unsigned int
    
    void extension();                                                                   //Шаг 1 - Расширение сообщения
    void adding_length();                                                               //Шаг 2 - Добавление длины сообщения
    void initialize_md();                                                               //Шаг 3 - Инициализация MD буфера
    void message_processing();                                                          //Шаг 4 - Обработка сообщения в цикле
    ostringstream result;                                                               //Шаг 5 - Результат в виде хэш-сообщения
public:
    bool read_file(char *fileName);                                                     //Чтение из файла
    bool write_file(char *fileName, string str);                                        //Запись в файл
    
    string md5();                                                                       //Алгоритм преобразования
};
