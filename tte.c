/*
Copyright (C) 2016 QuakerMan

tte is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tte.  If not, see <http://www.gnu.org/licenses/>.
*/

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

int main(int argc,char* argv[]){
	const int bufsize = 128;
	int cursize = 0;
	int curchar;
	int incchar;
	int index = 0;
	int line = 0;
	int linec = 0;
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
		while( (incchar = fgetc(pfile)) != EOF){	
			buffer[index++] = (char)incchar;	
			if(index == cursize){    
				cursize = index + bufsize;	 
				buffer = realloc(buffer, cursize);	 
			}
			/*if(buffer[index] == '\n'){
				line++;
				linec = 0;
			}
			else{
				linec++;
			}*/
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
		switch(curchar = getch()){
			case 27:
				//printf("\n\n[%s]", buffer);	//--------------------------------------------
				printf("\n[Wrote %zu bytes]\n", strlen(buffer));
				freopen(argv[1], "w", pfile);
				fprintf(pfile, "%s", buffer);
				//free(buffer);
				buffer = NULL;
				fclose(pfile);
				exit(1);
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
			default:
				if(curchar == 127 || curchar == 9 || curchar == '\0'){
					break;
				}
				buffer[strlen(buffer)] = curchar;
				buffer[strlen(buffer)+1] = '\0';
				putchar(curchar);
				linec++;	
		}
	}
	return 0;
}
