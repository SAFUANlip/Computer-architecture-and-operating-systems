#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <zlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>

void archivate(char* from, char* to) {
    int from_dir;

    if ((from_dir = open(from, O_RDONLY, 0)) == -1) { //открываем директорию для чтения
        perror("open");
        printf("error opening %s\n", from);
        return;
    }
    char buf[4096]; // для имён файлов
    //Возвращает указатель файла, всё что в него будем записывать - будет упаковываться
    //специально открыли архивационным способом
    gzFile to_dir = gzopen(to, "w"); //gz файл, w - метод архивации
    if (!to_dir) {
        perror("gzopen");
        printf("error opening %s\n", to);
        return;
    }
    int len = -1;
    //считываем в буфер, потом его записываем в новый файл
    while ((len = read(from_dir, buf, sizeof(buf))) > 0) { //пока есть что-то в каталоге, то записываем в файл
    	//запись в to с сжатием файла
        if(gzwrite(to_dir, buf, len) <= 0) {
            perror("gzwrite");
            printf("error writing %s\n", to);
            close(from_dir);
            return;
        }
    }
    gzclose(to_dir);
    close(from_dir);
    return;
}


void copy(char* from, char* to) {
    DIR* cur = opendir(from);  //возвращает указатель на поток каталога или NULL в случае ошибок
    if (cur == NULL) {
        perror("opendir");
        printf("error opening %s\n", from);
        return;
    }

    struct dirent* from_d = NULL; //указатель на структуру, описывающую одну запись в директории
    //структуры для чтения атрибутов файла
    struct stat from_info;        //откуда копируем
    struct stat to_info;          //куда копируем
    char from_name[256] = "", to_name[256] = ""; 
    //from_d - теперь указывает на файл в директории
    while((from_d = readdir(cur)) != NULL) {  //чтение очередной записи из потока информации для директории
        //точка - это текущая дерикторя, две точки - родительская директория, мы эту информацию пропускаем
        if (strcmp(from_d->d_name, ".") == 0 || strcmp(from_d->d_name, "..") == 0) //функция сравнения сторк, если строка это "." или ".." то пропускаем
            continue;
        
        //запишем в переменные имена директории и файла (полный путь к файлу)
        sprintf(from_name, "%s/%s", from, from_d->d_name);
        sprintf(to_name, "%s/%s", to, from_d->d_name);

	//читает информацию об атрибутах файла, на имя которого указывает from_name и заполняем ими структуру stat from_info
        stat(from_name, &from_info);

	//копирование каталогов
	//S_ISDIR проверяет является ли файл каталогом, st_mode - тип файла
        if (S_ISDIR(from_info.st_mode)) { 
            if (stat(to_name, &to_info) == -1) //заполнение структуру stat to_info
                mkdir(to_name, 0775); //если каталога нет, то он создаётся 
            copy(from_name, to_name); //рекурсивно опускаемся, чтобы копировать файлы в подкаталоге
            continue;
        }

	//S_ISREG явлется ли файл файлом
        if (S_ISREG(from_info.st_mode)) {
            sprintf(to_name, "%s.gz", to_name); //приписываем .gz (т.к хотим сделать такое расширение)
            if (stat(to_name, &to_info) == 0) { //заполняем структуру to_info
                if (to_info.st_mtime < from_info.st_mtime) //Если у нас есть более свежая версия, то копию из директории "куда" удаляем
                    unlink(to_name);         //удаление жёстких связей и файла
                else 
                    continue;

            }

            archivate(from_name, to_name);  //Архивируем только папку
        }
    }
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("erroneous number of arguments\n");
        return -1;
    }

    copy(argv[1], argv[2]);
    return 0;

}
