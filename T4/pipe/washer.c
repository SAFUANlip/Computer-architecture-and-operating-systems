#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h>

void problem(char *text){
	printf("%s\n", text);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[], char*envp[])
{
	if (argc !=4)
		problem("too many arg");

	int const TABLE_LIMIT = atoi(argv[3]);

	int mwash[20];
	int t;
	int in;
	FILE* fw = fopen(argv[2], "r");
	while (fscanf(fw,"%d" "%d", &in, &t) != EOF){
		mwash[in] = t;
		fscanf(fw, "%*[^\n]\n");
	}
	fclose(fw);

	FILE* f = fopen(argv[1],"r");
	if (f == NULL)
		problem("problem with opening");
	
	
	int table;
	char name_table[] ="table.fifo";
	(void)umask(0); //(void) себе показываем каой тип функции
	//if(mknod(name_table, S_IFIFO|0666, 0)<0)
	//	problem("W: cant create table.fifo");
	if ((table = open(name_table, O_RDWR))<0)
					problem("W: cant open table.fifo for writing");
					
					
	int pick;
	char name_pick[] ="pick.fifo";
	if ((pick = open(name_pick, O_RDWR))<0)
					problem("W: cant open pick.fifo for reading");
	
	int ON_TABLE=0; //сколько посуды сейчас на столе
	int count = 0; //количество  посуды сейчас на мойку типа type
	int type=0;   //тип посуды сейчас на мойке	
	while(fscanf(f,"%d %d", &type, &count) != EOF){
		printf("%d dishes of type %d \n", count, type);
		for (; count>0; --count){
			if(ON_TABLE >= TABLE_LIMIT)
			{
				int minus;
				read(pick, &minus, 4);
				ON_TABLE = ON_TABLE - 1;
			}
			sleep(mwash[type]);
			printf("wash type - %d\n", type);
			
			write(table, &type, 4);
			++ON_TABLE;
		}
		fscanf(f, "%*[^\n]\n");
	}
	int a = -1;	
	write(table, &a, 4);
	close(table);	
	close(pick);
	return 0;
}

