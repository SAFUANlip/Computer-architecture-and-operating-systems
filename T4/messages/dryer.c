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
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[], char*envp[])
{
	if (argc !=2)
		problem("too many arg");


	int mdry[10];
	int t;
	int in;
	FILE* fd = fopen(argv[1], "r");
	while (fscanf(fd,"%d" "%d", &in, &t) != EOF){
		mdry[in] = t;
		fscanf(fd, "%*[^\n]\n");
	}
	fclose(fd);

	
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

	short  type = 0;   // тип посуды сейчас на мойке
	int flag = 1;
	while(flag){
		// считали сообщение - взяли тарелку со стола
		if(msgrcv(msgid, (struct msgbuf*) &mybuf, sizeof(short), SEND_DISH, 0) < 0)
			problem("Can\'t get message to queue\n");
		type = mybuf.dish;
		if(type == FINISH) break;
		// отправили сообщение мойщику, о том что мы взяли тарелку со стола
		mybuf.mtype = END_DRY;
		if(msgsnd(msgid, (struct msgbuf*) &mybuf, sizeof(short), 0) < 0){
			msgctl(msgid, IPC_RMID, (struct msqid_ds *) NULL);
			problem("Can\'t send message to queue\n");
		}
		
		sleep(mdry[type]);
		printf("dry type - %d\n", type);
	
	}
	msgctl(msgid, IPC_RMID, (struct msqid_ds *) NULL);
	return 0;
}


