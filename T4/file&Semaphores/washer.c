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
	printf("%s\n", text);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[], char*envp[])
{
if (argc !=4)
	problem("too many arg");

int TABLE_LIMIT = (*argv[3]);

int mwash[9];
int t;
int in;
FILE* fw = fopen(argv[2], "r");
while (fscanf(fw,"%d" "%d", &in, &t) != EOF){
	mwash[in] = t;
	fscanf(fw, "%*[^\n]\n");
}
fclose(fw);

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



FILE* f = fopen(argv[1],"r");
if (f == NULL)
	problem("problem with opening");

struct sembuf start;
//увеличение элемента
start.sem_op = TABLE_LIMIT;
start.sem_flg = 0;
start.sem_num = CAN_ON_TABLE;

if(semop(semid, &start, 1)<0)
	problem("Can't start");

struct sembuf waiting[2];
waiting[0].sem_op = -1;
waiting[0].sem_flg = 0;
waiting[0].sem_num = CAN_ON_TABLE;

waiting[1].sem_op = 1;
waiting[1].sem_flg = 0;
waiting[1].sem_num = ON_TABLE;


int count = 0; //количество  посуды сейчас на мойку типа type
int  type=0;   //тип посуды сейчас на мойке


while(fscanf(f,"%d %d", &type, &count) != EOF){
	printf("%d dishes of type %d \n", count, type);
	for (; count>0; --count){
		sleep(mwash[type]); //удерживает тарелку, если на столе нет места
		if(semop(semid, waiting, 2)<0) //пока на столе есть место, мы будем класть
			problem("Can't wainting");
		printf("wash type - %d\n", type);
		++array[type];
	}
	fscanf(f, "%*[^\n]\n");
}

struct sembuf fflag;
//пока на столе будет посуда мы не закончим, для выхода из while (main2)
fflag.sem_op = 0;
fflag.sem_flg = 0;
fflag.sem_num = ON_TABLE;

if(semop(semid, &fflag, 1)<0) 
			problem("Can't finish");

array[FLAG] = -1;

if(shmdt(array)<0)
	problem("Can't detach shared memory\n");
	
	
return 0;
}










