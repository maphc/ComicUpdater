#include <stdio.h>
#include <string.h>

#define PASS "1234567"
int verify(char* sor){
	int v=0;
	v=strcmp(sor,PASS);

	return v;

}

void main(){
	int valid_flag=0;

	char pass[1024];
	while (1)
	{
		printf("input a word :");
		scanf("%s",pass);
		valid_flag=verify(pass);
		if(!valid_flag){
			printf("\n OK\n");
			break;
		}else{
			printf("\rError !\n");
			//break;
		}
	}

}