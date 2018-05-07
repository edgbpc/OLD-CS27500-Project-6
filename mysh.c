/* Author: Eric Goodwin
 * Class: CS2750 E01 Assignment 6
 * Dr. Sanjiv Bhatia
 */


/*  In this assignment, you will create a minimal shell in Unix, called mysh. 
 *  You will start with creating a prompt on the terminal for the user. 
 *  The user will enter a command to be executed. You will search for the 
 *  executable corresponding to the command in all the directories specifed in the 
 *  environment variable MYPATH. This variable should be set with at least three
 *  directories before you start mysh. 
 *
 *  In case the variable is not set, use the variable PATH to search for the executable. 
 *  You can set the variable MYPATH using : to separate directories. For example, 
 *  in bash, you can set it as export MYPATH=/bin:/usr/local/bin:${HOME}/bin:. 
 *  You will search the directories in sequence. As soon as you find a corresponding to the command,
 *  check to make sure that it is an ordinary file eand it is executionable is set for you.
 *  If the second conditions are met, execute the commend by forking and using execl. 
 *  Otherwise, keep on searching in the directories. If you cannot find the command,
 *  issue the message saying Command not found preceded by the name of the command. 
 *  
 *  After executing the command, issue the prompt again. If the command entered is exit, 
 *  terminate your shell. 
 *
 *  Take the prompt from environment variable PS1 if set. Otherwise, use $ as the default prompt. 
 *
 *  Make sure that you have a Makefile, a README file, and use git. 
 *
 *  Extra Credit: Allow your shell to kill the currently executing command by typing a CTRL-C. 
 *  When the user hits CTRL-C, the command that was executed from within mysh is terminated. 
 *  The only time you terminate mysh is when the user types a CTRL-D or exit command.
 */


#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <limits.h>
#include <stdbool.h>
#include <signal.h>


/* Function prototypes for commands */

char * read_line();
int  makeargv(char *, char *, char ***);
char * read_prompt(char *);
char * create_path(int, char **, char **);
void executeCommand(char *, char **);
void sigintHandler(int);
char * replace_str(char *, char *);


int main(){

	char * PS1 = getenv("PS1");
	char *userPath;  //passed to makeargv
//	char *** argvp; //
	char path_delimiter[] = ":"; //passed to makeargv 
	char command_delimiter[] = " \t"; //pass to makeargv
	char *command_line; // get command line 
	char ** command_TokenArrays;
	char ** path_TokenArrays;
	int path_tokens;
	char * userPrompt = NULL;
	char * createPath = NULL; 


					/*********************
					|* pre-loop processe*|
					|********************/

			
					/* check for PS1 and execute  
					 * function to parse PS1 and
					 * assign to userPrompt, 
					 * otherwise, use $ for prompt
					 */

	
//	if (PS1){i
//		userPrompt = read_prompt(PS1);
//	}	
//
//	else{
//		userPrompt = "$";
//	}

	



					/* check for MYPATH  and 
					 * set userPath to PATH if 
					 * MYPATH not defined  */

	if ((userPath = getenv("MYPATH")) == NULL ){
	userPath = getenv("PATH");
	}


					/*read path for directories to 
					 * search for executable files*/

	path_tokens = makeargv( userPath, path_delimiter, &path_TokenArrays);	
						

					/*******************|
					|* Main Shell Loop *|
					|*******************/
	
	
	for ( ; ; ){


		signal(SIGINT, SIG_IGN); //sets ctrl+c to ignore
		signal(SIGINT, sigintHandler); //displays message on when ctrl c can be used

		userPrompt = read_prompt(PS1);
		printf("%s", userPrompt);

		command_line = read_line();
		if (feof(stdin)){
			free(userPrompt);
			free(createPath);  
			return 0;
		
		}
//		command_tokens = 
		makeargv( command_line, command_delimiter, &command_TokenArrays);
						// 	testing
						//
						//	printf ( "The argument array contains:\n" );
						//	int i;
						//	for ( i = 0; i < 9; i++ )
						//	printf ( "%d:%s\n", i, command_TokenArrays[i] );
								 
		createPath = create_path(path_tokens, path_TokenArrays, command_TokenArrays);	

		executeCommand(createPath, command_TokenArrays);
	}

free(userPrompt);
free(createPath);


return 0;
}


					/***********************|
					|* Function Definitions*|
					|**********************/

/* Function reads line from standard input to be passed to makeargv */

char * read_line(){
	char * line;
	int i = 0;
	ssize_t bufsize = 0;
	getline(&line, &bufsize, stdin);
	
	if (line[0] == '\n'){ //if user enters blank line, reports back no command entered
		return line;
	}
	else{
		for (i; line[i]!='\0'; i++){   //removes the \n that getline adds
			if(line[i]=='\n'){
				line[i]='\0';
			}
		}
	}
	return line;
}

char * read_prompt(char * PS1){
	char * return_prompt = ( char * ) malloc ( 256 );      //this line provided by Dr. Bhatia
	memset ( return_prompt, '\0', 256 ); //this line provided by Dr. Bhatia

	int index = 0;
	char mydir[1024];
	getcwd(mydir, sizeof(mydir));

	char * home = getenv("HOME");

	if (PS1 != NULL){
			
		for (index; index < strlen(PS1); index++){
			if (PS1[index] ==  '\\'){
				switch(PS1[index+1]){
					case 'u':
						strcat(return_prompt, getenv("USER"));
						break;
					case 'h':
						strcat(return_prompt, "@");
						strcat(return_prompt, getenv("HOSTNAME"));
						break;
					case 'w':
					case 'W':
						strcat(return_prompt, " ");
						strcat(return_prompt, replace_str(mydir, home)); //truncates string in HOME to ~ 
					//	strcat(return_prompt, getcwd(mydir, sizeof(mydir)));
						//need to fix this line.  doesn't change directory in prompt as user changes directory. //resovled
						break;
					case 'n':
						strcat(return_prompt, "\n");
						break;

				}	
			}
		}
		
		strcat(return_prompt, ":");
		return return_prompt;
	}
	else
		return_prompt = "$ ";
	 	return return_prompt;
}

char * create_path(int path_tokens, char ** path_TokenArrays, char ** command_TokenArrays ){
		int index = 0;
		
		char * createPath = malloc(sizeof(char)*MAX_CANON); 

		for (index; index < path_tokens; index++){
		strcpy(createPath, path_TokenArrays[index]);
		strcat(createPath, "/");
		strcat(createPath, command_TokenArrays[0]);
//	printf("%s\n", createPath);	
//		printf("%d\n", result);	
		
		
		if ( access(createPath, F_OK) == 0){
			if ( access(createPath, X_OK) == 0)
			return createPath;
			}

		}

	createPath =  NULL;  // if file is not found and if file is not executable returns NULL
	return createPath;
}				

void executeCommand(char * createPath, char ** command_TokenArrays ){

	pid_t child;
	pid_t childwait;
	int status;

//	if (createPath != NULL){
		if (strcmp(command_TokenArrays[0], "cd") == 0){
// testing 	printf(" cd entered")
			if (chdir(command_TokenArrays[1]) != 0) //check if the directory doesn't change
			printf("Failed to change directory\n");
			}	
	
		else if (strcmp(command_TokenArrays[0], "exit") == 0)
		exit(EXIT_SUCCESS);


		else if (createPath != NULL){ //if createPath isn't null execute command
			child=fork();


			if(child == 0){
				execv( createPath, command_TokenArrays) < 0; // returns -1 if fails
				signal(SIGINT, SIG_DFL); //resets signal handling to default during child process
				//had to use execv to be able to pass command_TokenArrays
				printf("Execution failed\n");
			}
			else{
				 {
					childwait = wait(&status);
					if(childwait != child)
						printf("End Process\n");
				} while (childwait !=  child);
			}
		}
		
		else if (strcmp(command_TokenArrays[0], "\n") == 0){ //if \n detected
			printf("No command entered.\n");
		}

		else{ //if createPath is NULL prints command not found
			printf("%s: Command not found.\n", command_TokenArrays[0]);
	}
}


/* Signal Handler for SIGINT */
void sigintHandler(int sig_num){
	printf("\n Ctrl+C only valid to terminate child process. \n CTRL-D or exit to terminate shell.\n :");
	fflush(stdout);
}

char *replace_str(char *str, char *orig)
{
	static char buffer[4096];
	char *p;
	char replaced = '~';

	if(! (p = strstr(str, orig)))  // Is 'orig' even in 'str'?
		return str;
	
	strncpy(buffer, str, p - str); // Copy characters from 'str' start to 'orig' st$
	buffer[p - str] = '\0';
			
	sprintf(buffer + (p - str), "%c%s", replaced, p + strlen(orig));

	return buffer;
}



