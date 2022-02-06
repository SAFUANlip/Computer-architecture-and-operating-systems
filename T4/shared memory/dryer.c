#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>


int const ON_TABLE = 8;
int const FLAG = 9;

void problem(char *text){
	printf("%s\n", text);
	exit(EXIT_FAILURE);
}

int get_type(int *arr){
	for(int i=0; i<8; ++i)
		if (arr[i]!=0)
			return i;
	problem("it snail too slow");
	return 0;
}

int main(int argc, char *argv[], char*envp[])
{
if (argc !=2)
	problem("too many arg");


int mdry[8];
int t;
int in;
FILE* fd = fopen(argv[1], "r");
while (fscanf(fd,"%d" "%d", &in, &t) != EOF){
	mdry[in] = t;
	fscanf(fd, "%*[^\n]\n");
}
fclose(fd);

int *array; //указатель на разделяемую память
int shmid; //IPC дескриптор для области разделяемой памяти
int new=1; //флаг необходимости инициализации элементов массива
char pathname[] = "main.c";
key_t key;
if((key=ftok(pathname,0))<0)
	problem("Cant\'t generate key\n");
	
if ((shmid = shmget(key, 10*sizeof(int), 0666|IPC_CREAT|IPC_EXCL))<0){
	if(errno!=EEXIST)
		problem("Cant\'t create shared memory\n");
	else{
		if((shmid = shmget(key, 10*sizeof(int), 0)) < 0)
			problem("Cant\'t find shared memory\n");
		new=0;
		}
	}
if ((array = (int*)shmat(shmid, NULL, 0)) == (int*)(-1))
	problem("Can't attach shared memory\n");
if(new)
	for(int i=0; i<10; ++i)
		array[i] = 0;

int  type=0;   //тип посуды сейчас на мойке
while((array[FLAG] != -1) || (array[ON_TABLE] > 0)){
		while(array[ON_TABLE]==0){}
		type=get_type(array);
		sleep(mdry[type]);
		printf("dry type - %d\n", type);
		--array[type];
		--array[ON_TABLE];
}
if(shmdt(array)<0)
	problem("Can't detach shared memory\n");

return 0;
}










