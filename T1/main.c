#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

void problem(char *text){
	printf("%s\n", text);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[], char*envp[])
{
if (argc !=2)
	problem("too many arg");

pid_t pid;
FILE* f = fopen(argv[1],"r"); //в argv[1] лежит имя фала, с которого считываем команды
if (f == NULL)
	problem("problem with opening");

int time = 0;
char read[64];
while(fscanf(f,"%d %s", &time, read) != EOF){
	printf("%s -  is done with delay %d\n", read, time);

	//проверить время на <0
	if(time<0)
		problem("time can't be < 0");
	pid = fork();
	if (pid == 0){ //дочерний процесс возвращает 0, меньше нуля - ошибка, больше - основной
		sleep(time);
		execlp(read, read, NULL); //read команда она же имя файла
				 	  // NULL-параметры внешней среды
		printf("%s oups :)\n", read); // если exec вернулся, то это означате ошибку
	}
	else if(pid < 0)
		problem("fail pid < 0");
	fscanf(f, "%*[^\n]\n");
}

while (wait(NULL) != -1) {}; //основной процесс ждёт завершения дочерних
if (!feof(f))
	problem("it isn't end of file");

fclose(f);
return 0;
}
