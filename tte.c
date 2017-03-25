#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <termios.h>

static struct termios old, new;

void terminit(int echo){
	tcgetattr(0, &old);		//Grab old attributes.
	new = old;		//Make new attributes the same as the old.
	new.c_lflag &= ~ICANON;		//Disabed buffed input/output.
	new.c_lflag &= echo ? ECHO : ~ECHO;		//Set if echo.
	tcsetattr(0, TCSANOW, &new);		//Use new terminal attibutes.
}

void termrst(){
  tcsetattr(0, TCSANOW, &old);
}

char getch(){
	char ch;
	terminit(0);
	ch = getchar();
	termrst();
	return ch;
}

void appch(char* str, char ch){
	str[strlen(str)] = ch;
	str[strlen(str)+1] = '\0';
	putchar(ch);
}

void rmch(char* str){
	str[strlen(str)-1] = '\0';
}


int main(int argc, char* argv[]){
	const int bufsize = 128;
	int cursize = 0;
	int curch;
	int incch;
	int index = 0;
	int line = 0;
	int linec = 0;
	int linelen = 0;
	size_t itt;
	char* buffer = malloc(bufsize);
	char escseq[3];

	cursize = bufsize;

	FILE* pfile;

	if(argv[1] == NULL){
		printf("Error\n");
		exit(1);
	}

	pfile = fopen(argv[1], "r+");		//If file exists, get contents.
	if(pfile != NULL){		
		while( (incch = fgetc(pfile)) != EOF){	
			buffer[index++] = (char)incch;	
			if(index == cursize){    
				cursize = index + bufsize;	 
				buffer = realloc(buffer, cursize);	 
			}
			if(buffer[index] == '\n'){
				line++;
				linec = 0;
			}
			else{
				linec++;
			}
			//index++;
		}
		buffer[index] = '\0';
		printf("\e[1;1H\e[2J");
		printf("%s", buffer);
	}
	else{
		pfile = fopen(argv[1], "w+");
		printf("\e[1;1H\e[2J");		//Else, just create a new file.
	}

	while(1){
		switch(curch = getch()){
			case 27:
				printf("\n[Wrote %zu bytes]\n", strlen(buffer));
				freopen(argv[1], "w", pfile);
				fprintf(pfile, "%s", buffer);
				//free(buffer);
				buffer = NULL;
				fclose(pfile);
				exit(1);
			case 9:
				for(int i=0; i <= 4; i++){
					appch(buffer, ' ');
					linec++;
				}
			case 127:
				printf("\b \b");
				if(buffer[strlen(buffer)-1] == '\n'){
					buffer[strlen(buffer)-1] = '\0';
					printf("\033[1A");
					for(itt = strlen(buffer); itt > 0 && buffer[itt] != '\n'; itt--){
						printf("\033[1C");
					}

				}
				else{
					rmch(buffer);
					linec--;
				}
			default:
				if(curch == 127 || curch == 9 || curch == '\0'){
					break;
				}
				appch(buffer, curch);
				linec++;	
		}
	}
	return 0;
}
