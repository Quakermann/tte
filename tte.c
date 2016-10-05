/*
MIT License

Copyright (c) [2016] [Quaker Man]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/








#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <termios.h>

static struct termios old, new;		//All this is just code ripped from ncurses.

void initTermios(int echo){
	tcgetattr(0, &old); /* grab old terminal i/o settings */
	new = old; /* make new settings same as old settings */
	new.c_lflag &= ~ICANON; /* disable buffered i/o */
	new.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
	tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
}

void resetTermios(void){
  tcsetattr(0, TCSANOW, &old);
}

char getch(){
	char ch;
	initTermios(0);
	ch = getchar();
	resetTermios();
	return ch;
}

int main(int argc,char* argv[]){
	const int bufsize = 128;
	int curchar;
	int incchar;
	int index = 0;
	int line = 0;
	int linec = 0;
	char* buffer = malloc(bufsize);


	FILE* pfile;

	if(argv[1] == NULL){
		printf("Error\n");
		exit(1);
	}

	pfile = fopen(argv[1], "r+");		//If file exists, get contents.
	if(pfile != NULL){		
		while((incchar = fgetc(pfile)) != EOF){	
			buffer[index] = incchar;	
			if(index == bufsize){    
				index += bufsize;	 
				buffer = realloc(buffer,bufsize);	 
			}
			if(buffer[index] == '\n'){
				line++;
				linec = 0;
			}
			else{
				linec++;
			}
			index++;
		}
		buffer[index] = '\0';
		printf("\e[1;1H\e[2J");
		printf("%s", buffer);
	}
	else{
		pfile = fopen(argv[1], "w+");		//Else, just create a new file.
	}

	while(1){
		curchar = getch();
		if(isprint(curchar) || curchar == '\n'){
			buffer[strlen(buffer)] = curchar;
			buffer[strlen(buffer)+1] = '\0';
			putchar(curchar);
			linec++;
		}
		else{
			switch(curchar){
				case 27:
					//printf("\n\n[%s]", buffer);	//--------------------------------------------
					printf("\n[Wrote %zu bytes]\n", strlen(buffer));
					freopen(argv[1], "w", pfile);
					fprintf(pfile, "%s", buffer);
					free(buffer);
					fclose(pfile);
					exit(1);
				case 10:
					buffer[strlen(buffer)] = '\n';
					buffer[strlen(buffer)+1] = '\0';
					putchar('\n');
					linec = 0;
				case 9:
					for(int i=0; i <= 4; i++){
						buffer[strlen(buffer)] = ' ';
						putchar(' ');
						linec++;
					}
					buffer[strlen(buffer)+1] = '\0';
				case 127:
					printf("\b \b");
					if(buffer[strlen(buffer)-1] == '\n'){
						buffer[strlen(buffer)-1] = '\0';
						printf("\033[1A");
						for(int i=0; i < linec-1; i++){
							printf("\033[1C");
						}
					}
					else{
						buffer[strlen(buffer)-1] = '\0';
						linec--;
					}
			}
		}
	}
	return 0;
}