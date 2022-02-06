#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
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


int const CAN_ON_TABLE = 0; //сколько ещё можем положить на стол
int const ON_TABLE = 1;
int const FLAG = 9; //посуда кончилась


void problem(char *text){
	//printf("%s\n", text);
	perror(text);
	exit(EXIT_FAILURE);
}

int get_type(int *arr){
	for(int i=0; i<9; ++i)
		if (arr[i]!=0)
			return i;
	problem("it snail too slow");
	return 0;
}

int main(int argc, char *argv[], char*envp[])
{
if (argc !=2)
	problem("too many arg");


int mdry[9];
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
key_t key1;
if((key1=ftok(pathname,0))<0)
	problem("Cant\'t generate key\n");
	
if ((shmid = shmget(key1, 10*sizeof(int), 0666|IPC_CREAT|IPC_EXCL))<0){
	if(errno!=EEXIST)
		problem("Cant\'t create shared memory\n");
	else{
		if((shmid = shmget(key1, 10*sizeof(int), 0)) < 0)
			problem("Cant\'t find shared memory\n");
		new=0;
		}
	}
if ((array = (int*)shmat(shmid, NULL, 0)) == (int*)(-1))
	problem("Can't attach shared memory\n");
if(new)
	for(int i=0; i<10; ++i)
		array[i] = 0;


int semid; //IPC дескриптор для массива IPC семафоров
key_t key2;
if((key2=ftok(pathname,1))<0) //1 показывает, что ключи разные
	problem("Cant\'t generate key\n");
	
if ((semid = semget(key2, 2, 0666|IPC_CREAT))<0)
		problem("Cant\'t get semid\n");


struct sembuf waiting[2];
waiting[0].sem_op = -1;
waiting[0].sem_flg = 0;
waiting[0].sem_num = ON_TABLE;

waiting[1].sem_op = 1;
waiting[1].sem_flg = 0;
waiting[1].sem_num = CAN_ON_TABLE;

int  type=0;   //тип посуды сейчас на мойке
while(array[FLAG] != -1){
		//while(array[ON_TABLE]==0){}
		if(semop(semid, waiting, 2)<0)
			problem("Can't wainting"); 
		type=get_type(array);
		sleep(mdry[type]);
		printf("dry type - %d\n", type);
		--array[type];
}
if(shmdt(array)<0)
	problem("Can't detach shared memory\n");
	
if((semctl(semid, 0, IPC_RMID, 0))<0)
	problem("Snail you can go throw fence!!!");
	
if((shmctl(shmid, IPC_RMID, NULL))<0);
	problem("shmctl not working");
return 0;
}










