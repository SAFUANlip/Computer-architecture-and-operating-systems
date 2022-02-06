#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define SEND_DISH 1 // помыл тарелку и положил на стол
#define END_DRY 2  // высушил тарелку
#define FINISH -1   //тарелка сигнал об окончании


void problem(char *text){
	printf("%s\n", text);
	exit(-1);
}

int main(int argc, char *argv[], char*envp[])
{
	if (argc !=4)
		problem("too many arg");

	int const TABLE_LIMIT = atoi(argv[3]);

	int mwash[10];
	int t;
	int in;
	FILE* fw = fopen(argv[2], "r");
	while (fscanf(fw,"%d" "%d", &in, &t) != EOF){
		mwash[in] = t;
		fscanf(fw, "%*[^\n]\n");
	}
	fclose(fw);

	int msgid; //IPC дескриптор для очереди сообщений
	char pathname[] = "washer.c";
	key_t key;

	struct mymsgbuf{
		long mtype;
		short dish; // тип передаваемой на стол посуды
	} mybuf;
	

	if((key=ftok(pathname,0))<0)
		problem("Cant\'t generate key\n");
		
	if ((msgid = msgget(key, 0666|IPC_CREAT))<0)
		problem("Cant\'t get msgid\n");
		
			
	FILE* f = fopen(argv[1],"r");
	if (f == NULL)
		problem("problem with opening");

	int count = 0; //количество  посуды сейчас на мойку типа type
	int  type=0;   //тип посуды сейчас на мойке
	int ON_TABLE = 0; 
	while(fscanf(f,"%d %d", &type, &count) != EOF){
		printf("%d dishes of type %d \n", count, type);
		for (; count>0; --count){
			if(ON_TABLE >= TABLE_LIMIT)
			{
				if(msgrcv(msgid, (struct msgbuf*) &mybuf, sizeof(short),
				END_DRY, 0) < 0)
					problem("Can\'t send message to queue\n");
				ON_TABLE = ON_TABLE - 1;
			}
			sleep(mwash[type]);
			printf("wash type - %d\n", type);
			mybuf.mtype = SEND_DISH;
			mybuf.dish = type;
			if(msgsnd(msgid, (struct msgbuf*) &mybuf, sizeof(short), 0) < 0){
				msgctl(msgid, IPC_RMID, (struct msqid_ds *) NULL);
				problem("Can\'t send message to queue\n");
			}
			++ON_TABLE;
		}
		fscanf(f, "%*[^\n]\n");
	}

	mybuf.mtype = SEND_DISH;
	mybuf.dish = FINISH; // сковородка от Игоря, бабушка помой пж.
	//помыл все тарелки, и отправил сигнал об окончании ввиде тарелки с подлянкой
	if(msgsnd(msgid, (struct msgbuf*) &mybuf, sizeof(short), 0) < 0){
		msgctl(msgid, IPC_RMID, (struct msqid_ds *) NULL);
		problem("Can\'t send message to queue\n");
	}

	return 0;
}

