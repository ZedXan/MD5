#include "MD5.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cout << endl << "Использование: ./MD5 file_in" << endl << endl;
	
	return -1;
    }
    else
    {
        MD5 hash;

        if (!hash.read_file(argv[1]))
            cout << "Ошибка чтения файла!" << endl << endl;
        else if (!hash.write_file((char*)"MD5sum.txt", hash.md5()))
            cout << "Ошибка записи файла!" << endl << endl;
    }
    
    return 0;
}
