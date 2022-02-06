#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

int n = 0;  // Счетчик процессов
// обработка сигнала
void child_sig(int sig) {
    pid_t p;
    int status;
    // -1 -злюбого порожденного процесса, whohang - сразу возвращаемся из вызова - без блокировки
    while((p = waitpid(-1, &status, WNOHANG)) > 0) {
    //если процесс завершился, получаем его инд >0 и уменьшаем кол-во процессов
        --n;
    }
}

enum {
    ERR_NONE = 0,
    ERR_IO,
    ERR_MEM
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("uncorrect number of arguments\n");
        return ERR_IO;
    }
    // Делаем свою обработку сигнала, первый параметр - номер сигнала, обработку которго мы меняем, второй - функция обработки
    //SIGCHILD - тип сигнала, который предаёт дочерний процесс родительскому, сообщая о своём заврешении
    signal(SIGCHLD, child_sig);
    char show_number[] = "show";  //сколько процессов исполняется
    char* end;
    int num;    // максимальное кол-во одновременно работающих процессов
    num = strtol(argv[1], &end, 0);
    if (end == argv[1]) {
        printf("not number\n");
        return ERR_IO;
    }
    if (num <= 0) {
        printf("uncorect N\n");
        return ERR_IO;
    }
    char cmd_line[4096];
    char sep[] = " ";  // Массив символов по которым разделяем считанную строку
    // считываем имя запускаемой программы
    while(scanf("%[^\n]%*c", &cmd_line) != EOF) {
    	//показать количество текущих процессов - ввести show
        if (strcmp(show_number, cmd_line) == 0) {
            printf("now executing %d/%d processes\n", n, num);
            continue;
        }
        if (n >= num) {
            printf("Too many process\n");
            continue;
        }
        int i = 0;
        char* argl[2048]  = {};
        char* istr;
        istr = strtok(cmd_line, sep);
        while(istr != NULL) {
            argl[i++] = istr;
            istr = strtok(NULL, sep);
        }
        argl[i] = NULL; // список параметров для execvp должен заканчиваться на NULL
        pid_t p = fork();
        if (p == 0) { // дочерний процесс
            execvp(argl[0], argl); //замена образа процесса на новый из cmd
            perror("execvp"); // если вернемся то ошибка
            return ERR_MEM;
        }
        else if (p == -1) { // не смогли форкнуться
            perror("fork");
            return ERR_MEM;
        }
        ++n; //увеличели число запущенных процессов
    }

    return ERR_NONE;
}
