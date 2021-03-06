#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "sys/types.h"

enum {
    ERR_NONE = 0,
    ERR_IO,
};

int main(int argc, char* argv[]) {
    pid_t pid = getpid();
    int time = 20;
    if (argc == 2) {
        char* end;
        time = strtol(argv[1], &end, 0);
        if (end == argv[1]){
            printf("Not number\n");
            return ERR_IO;
        }
    }
    else if (argc > 2) {
        printf("to much argumets\n");
        return ERR_IO;
    }
    printf("[%d]: I'll sleep about %d seconds\n",(int)pid, time);
    sleep(time);
    printf("[%d]: I'm dying... \n", (int)pid);
    return ERR_NONE;
}
