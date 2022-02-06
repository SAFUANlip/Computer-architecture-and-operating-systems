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

int main(int argc, char *argv[], char*envp[])
{
if (argc !=4)
	problem("too many arg");

int TABLE_LIMIT = (*argv[3]);

int mwash[8];
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
		
FILE* f = fopen(argv[1],"r");
if (f == NULL)
	problem("problem with opening");

int count = 0; //количество  посуды сейчас на мойку типа type
int  type=0;   //тип посуды сейчас на мойке
while(fscanf(f,"%d %d", &type, &count) != EOF){
	printf("%d dishes of type %d \n", count, type);
	for (; count>0; --count){
		while(array[ON_TABLE]>=TABLE_LIMIT){}
		sleep(mwash[type]);
		printf("wash type - %d\n", type);
		++array[type];
		++array[ON_TABLE];
	}
	fscanf(f, "%*[^\n]\n");
}
array[FLAG]=-1;
if(shmdt(array)<0)
	problem("Can't detach shared memory\n");
//if((shmctl(shmid, IPC_RMID, NULL))<0);
//	problem("shmctl not working");
return 0;
}










