#include "MD5.h"                                                                            //Подключаем описание класса

unsigned int MD5::F(unsigned int j, unsigned int x, unsigned int y, unsigned int z)         //Выбор битовой функции в соответсвии с номером цикла
{
    if (j < 16)
    {
        G = j;

        return (x & y) | ((~x) & z);
    }
    else if (j < 32)
    {
        G = (5 * j + 1) % 16;

        return (z & x) | ((~z) & y);
    }
    else if (j < 48)
    {
        G = (3 * j + 5) % 16;

        return x ^ y ^ z;
    }
    else if (j < 64)
    {
        G = (7 * j) % 16;

        return y ^ (x | (~z));
    }
    else 
        return 0;
}

unsigned int MD5::inv(unsigned int value)                                                   //Смена порядка бит (endians)
{
    unsigned int res = 0;                                                                   //Объявляем переменную результата

    res |= ((value >> 0) & 0xFF) << 24;                                                     //Менаяем
    res |= ((value >> 8) & 0xFF) << 16;                                                     //Порядок бит
    res |= ((value >> 16) & 0xFF) << 8;                                                     //В каждом из
    res |= ((value >> 24) & 0xFF) << 0;                                                     //4х байтов

    return res;                                                                             //Возвращаем результат
}

unsigned int MD5::bytes_to_uint(char* bytes)                                                //Преоразование 4х байтов в unsigned int
{
    unsigned int res = 0;                                                                   //Объявляем переменную результата

    res |= ((unsigned int)bytes[3] << 24) & 0xFF000000;                                     //Преобразуем
    res |= ((unsigned int)bytes[2] << 16) & 0xFF0000;                                       //Каждый
    res |= ((unsigned int)bytes[1] << 8) & 0xFF00;                                          //Из
    res |= ((unsigned int)bytes[0] << 0) & 0xFF;                                            //4х байтов

    return res;                                                                             //Возвращаем результат
}

bool MD5::read_file(char *fileName)                                                         //Чтение из файла
{
    ifstream in(fileName, ios::binary);                                                     //Открываем файл в бинарном режиме

    if (in.fail())                                                                          //Вернуть 0 
        return false;                                                                       //Если возникла ошибка

    in.seekg(0, ios::end);                                                                  //Подсчет
    unsigned int file_size = (unsigned int)in.tellg();                                      //Размера
    in.seekg(0, ios::beg);                                                                  //Файла

    char *mas = new char[file_size];                                                        //Выделяем память под массив для считывания

    message.resize(file_size);                                                              //Меняем размер строки результата в соответствии с размером файла
    in.read(mas, file_size);                                                                //Считываем данные в массив
    in.close();                                                                             //Закрываем файл

    for (int i = 0; i < file_size; i++)                                                     //Перекидываем все данные
        message[i] = mas[i];                                                                //В строку, с которой далее будет работать алгоритм

    delete [] mas;                                                                          //Освобождаем память

    return true;                                                                            //Возвращаем 1 в случае успеха
}

bool MD5::write_file(char *fileName, string str)                                            //Запись в файл
{
    ofstream out(fileName);                                                                 //Открываем или создаем файл

    if (out.fail())                                                                         //Вернуть 0
        return false;                                                                       //Если возникла ошибка

    out << str;                                                                             //Записываем строку с результатом в файл

    return true;                                                                            //Возвращаем 1 в случае успеха
}

void MD5::extension()                                                                       //Шаг 1 - Расширение сообщения
{
    bitlen = message.size() * 8;                                                            //Исходная длина сообщения в битах (нужна для шага 2)

    message.push_back((unsigned char)0x80);                                                 //Добавляем в конец сообщения единичный бит

    while ((message.size() * 8) % 512 != 448)                                               //До тех пор, пока длина сообщения не станет равной 448 по модулю 512,
        message.push_back(0);                                                               //Заполняем сообщение нулями

    blocks = (unsigned int)(message.size() / 64) + 1;                                       //Количество блоков для обработки (+1 нужно для блока из 8 байт, в который добавится bitlen)
}

void MD5::adding_length()                                                                   //Шаг 2 - Добавление длины сообщения
{
    X = new unsigned int*[blocks];                                                          //Выделяем память под массив массивов блоков

    for (unsigned int i = 0; i < blocks; i++)                                               //Пока не кончились блоки                                                
    {
        X[i] = new unsigned int[16];                                                        //Выделяем память под текущий блок

        for (int j = 0; j < (i == blocks - 1 ? 14 : 16); j++)                               //Если это не последний блок, то переносим преобразованное messgae в X,
            X[i][j] = bytes_to_uint(&message[(j * 4) + 64 * i]);                            //Если блок послений, то делаем то же самое, но оставляем 8 байт под bitlen

        if (i == blocks - 1)                                                                //Если это последний блок
        {
            X[i][14] = bitlen & 0xFFFFFFFF;                                                 //То добавляем в него bitlen, причем в виде двух 4-байтовых слов,
            X[i][15] = bitlen >> 32 & 0xFFFFFFFF;                                           //Где первым добавляется слово, содержащее младшие разряды.
        }
    }
}

void MD5::initialize_md()                                                                   //Шаг 3 - Инициализация MD буфера
{
    H0 = 0x67452301, H1 = 0xefcdab89, H2 = 0x98badcfe, H3 = 0x10325476;                     //Инициализиуем регистры

    for (int i = 0; i < 64; i++)                                                            //Всю таблицу констант 
        T[i] = (unsigned int)(pow(2, 32) * fabs(sin(i + 1)));                               //Заполняем в соответствии с алгоритмом
}

void MD5::message_processing()                                                              //Шаг 4 - Обработка сообщения в цикле
{
    //Массив значений сдвигов
    unsigned int s[64] = {  7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                            5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
                            4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                            6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };

    for (unsigned int i = 0; i < blocks; i++)                                               //Цикл блоков сообщения
    {
        A = H0; B = H1; C = H2; D = H3;                                                     //Сохраняем значения значения регистров на каждом этапе цикла

        //Магия
        for (unsigned int j = 0; j < 64; j++)
        {
            f = F(j, B, C, D);

            temp = D;
            D = C;
            C = B;
            B = B + ROTATE_LEFT(A + f + T[j] + X[i][G], s[j]);
            A = temp;
        }
        //Конец магии

        H0 += A; H1 += B; H2 += C; H3 += D;                                                 //Обновляем значения регистров на каждом этапе цикла
    }

    for (unsigned int i = 0; i < blocks; i++)                                               //Пока не кончились блоки
        delete [] X[i];                                                                     //Освобождаем память, выделенную под текущий блок

    delete [] X;                                                                            //Освобождаем память, выделенную под весь массив массивов блоков
}

string MD5::md5()                                                                           //Алгоритм преобразования
{
    extension();                                                                            //Шаг 1 - Расширение сообщения

    adding_length();                                                                        //Шаг 2 - Добавление длины сообщения

    initialize_md();                                                                        //Шаг 3 - Инициализация MD буфера

    message_processing();                                                                   //Шаг 4 - Обработка сообщения в цикле

    result << hex << inv(H0) << inv(H1) << inv(H2) << inv(H3);                              //Шаг 5 - Выход хэш-сообщения

    return result.str();                                                                    //Возвращаем результат в виде хэш-сообщения
}
