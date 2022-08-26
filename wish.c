#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>

/* Function to remove spaces in an string array */
char *removeSpaces(char *str) {
  int  ip_ind = 0;
  while (*(str+ip_ind) != 0) {
    if (isspace(*(str + ip_ind++)) != 0) {
      if (isspace(*(str + ip_ind)) != 0) {
        int x=ip_ind;
        while (*(str + x +1) != 0) {
          *(str + x)= *(str + 1 + x);
          x++;
        }
        *(str + x)= 0;
        --ip_ind;
      }
    }
  }

  if(str!=NULL && isspace(*str)!=0) {
      str  = (str+1);
  }
  if(str!=NULL && isspace(str[strlen(str) - 1]) != 0) {
      str[strlen(str)-1] = '\0';
  }

  return str;
}

int main(int argc, char* argv[]) {

    char* path[10];
    path[0] = "/bin";
    FILE * fp;

    char error_message[30] = "An error has occurred\n";
    if(argc > 2) {
        write(STDERR_FILENO, error_message, strlen(error_message)); 
	exit(1);
    }
    
    int interactive = 1;

    if(argc==2) {
	 interactive = 0;
	 fp = fopen(argv[1], "r");
	 if (fp == NULL) {
	     write(STDERR_FILENO, error_message, strlen(error_message));
	     exit(1);
	 }
    } 


    while(1) {

        char *input;
        size_t len = 0;
        ssize_t lineSize = 0;
       
        if(interactive == 1) { 
	    printf("wish> ");
            lineSize = getline(&input, &len, stdin);
            if(lineSize == -1 || strcmp(input,"\n")==0) {
                continue;
            }
	} else {
	    int read = getline(&input, &len, fp);
	    if (read == -1) {
		exit(0);
	    } 
	    if(strcmp(input, "\n")==0) {
		continue;
	    }

	}
       
        input = removeSpaces(input);
	if(*input=='\0') {
		continue;
	}
        char* cmd_argv[10];
	char* cmd_argv_copy[10];

        int index = 0;
        for (char *func = strtok(input," "); func != NULL; func = strtok(NULL, " ")) {	
            cmd_argv[index] = strdup(func);
            index++; 
        }
        //cmd_argv[index-1][strlen(cmd_argv[index-1])-1] = '\0';
	cmd_argv[index] = NULL;
       
        if (strcmp(cmd_argv[0], "cd") == 0) {
	    if(cmd_argv[2]!= NULL || cmd_argv[1] == NULL) {
	        write(STDERR_FILENO, error_message, strlen(error_message));
		continue;
	    } else {
		chdir(cmd_argv[1]);
		continue;
	    }
	}


	char* file;
	int redirect = 0;

        for(int i=0;i<index;i++) {
 	   if(strcmp(cmd_argv[i], ">")==0) {
	     if (i == (index - 1) || index > (i + 2) || i == 0) {
                 write(STDERR_FILENO, error_message, strlen(error_message));
	 	 redirect = -1;
		 break;
	     } 
             file = cmd_argv[i + 1];
             cmd_argv[index-1] = NULL;
             cmd_argv[index-2] = NULL;
             index = index - 2;
             redirect = 1;

	   } else if(strchr(cmd_argv[i],'>')!=NULL && sizeof(cmd_argv[i])>2) {
		 char* token = strtok(cmd_argv[i], ">");
		 file = strtok(NULL, ">");
		 cmd_argv[i] = token;
		 redirect = 1;
		 break;
	   }
	}
        if(redirect == -1) {
	       continue;
        }

        if (strcmp(cmd_argv[0], "exit") == 0) {
	    if(cmd_argv[1]!=NULL) {
		write(STDERR_FILENO, error_message, strlen(error_message));
		continue;
	    } else {
                exit(0);
	    }
        }

        if (strcmp(cmd_argv[0], "path") == 0) {
            memset(path, 0, sizeof(path));
            for(int i=1; i<index; i++) {
                path[i-1] = cmd_argv[i];
            }
            continue;
        }
        
        int freq = 1;
        int loopIsTrue = 0;

        if (strcmp(cmd_argv[0], "loop") == 0) {
	    if(index < 2 || atoi(cmd_argv[1]) < 0 || atoi(cmd_argv[1]) == 0) {
	        write(STDERR_FILENO, error_message, strlen(error_message));
                continue;
            }
            freq = atoi(cmd_argv[1]);
            loopIsTrue = 1;

            for(int i=2;i<=index;i++) {
                cmd_argv[i-2] = cmd_argv[i];
            }
            index = index-2;
	    for(int m=0;m<index;m++) {
		cmd_argv_copy[m] = cmd_argv[m];
	    }
        }
        
	for(int f=0; f<freq; f++) {
            if(loopIsTrue == 1) {
		for(int i=0;i<index;i++) {
		    char s[20];
                    if(strcmp(cmd_argv_copy[i], "$loop")==0) {
			sprintf(s,"%d",f + 1);
			cmd_argv[i] = s;
		    }
                }
            }
            int found = 0;
            for(int i=0;i<sizeof(path);i++) {
                if (found==1 || path[i]==NULL) {
                    break;
                }
                char* p = strdup(path[i]);
                strcat(p,"/");
                strcat(p,cmd_argv[0]);
                if(access(p, X_OK)==0) {
                    found = 1;
                    int rc = fork();
                    if(rc == 0) {
			if (redirect == 1) { 
			    (void) close(STDOUT_FILENO);
			    if(access( file, F_OK ) == 0) {
				 remove(file);
			    }
                            open(file, O_WRONLY | O_CREAT | O_TRUNC);
			    chmod(file, 774);
			}
                        execv(p,cmd_argv);
		        kill(getpid(), SIGINT);	
                    } else if (rc > 0) {
                        (void)wait(NULL);
                        continue;
                    }
                }
            }
            if(found==1) {
                continue;
            } else {
		write(STDERR_FILENO, error_message, strlen(error_message));
	    }
        }
    }
    return 0;
}


