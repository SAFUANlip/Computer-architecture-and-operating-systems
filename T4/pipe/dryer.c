#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h>

#define FINISH -1   //тарелка сигнал об окончании

void problem(char *text){
	printf("%s\n", text);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[], char*envp[])
{
	if (argc !=2)
		problem("too many arg");
		
	int mdry[20];
	int t;
	int in;
	FILE* fdr = fopen(argv[1], "r");
	while (fscanf(fdr, "%d" "%d", &in, &t) !=EOF){
		mdry[in] = t;
		fscanf(fdr, "%*[^\n]\n");
	}
	fclose(fdr);	
	
	int pick;
	char name_pick[] ="pick.fifo";
	(void)umask(0); //(void) себе показываем каой тип функции
	//if(mknod(name_pick, S_IFIFO|0666, 0)<0)
	//	problem("D: cant create pick.fifo");	
	if ((pick = open(name_pick, O_RDWR))<0)
					problem("D: cant open pick.fifo for writing");
					
					
	int table;
	char name_table[] ="table.fifo";
	if ((table = open(name_table, O_RDWR))<0)
					problem("D: cant open table.fifo for reading");
		
	int  type = 0;   // тип посуды сейчас на мойке
	int flag = 1;
	while(flag){
		read(table, &type, 4);
	
		if(type == FINISH) break;
		// отправили сообщение мойщику, о том что мы взяли тарелку со стола
		int a = 1;
		write(pick, &a, 4);
		sleep(mdry[type]);
		printf("dry type - %d\n", type);
	
	}
	close(pick);
	close(table);
	return 0;
		
}
