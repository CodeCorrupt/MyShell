/*
  strtokeg - skeleton shell using strtok to parse command line
  
  usage:
  
    strtokeg
          
    reads in a line of keyboard input at a time, parsing it into
    tokens that are separated by white spaces (set by #define
    SEPARATORS).

    can use redirected input
        
    if the first token is a recognized internal command, then that
    command is executed. otherwise the tokens are printed on the
    display.
    
    internal commands:
    
        clear - clears the screen
        
        quit - exits from the program
      
 ********************************************************************
   version: 1.0
   date:    December 2003
   author:  Ian G Graham
            School of Information Technology
            Griffith University, Gold Coast
            ian.graham@griffith.edu.au
            
   copyright (c) Ian G Graham, 2003. All rights reserved.
            
   This code can be used for teaching purposes, but no warranty,
   explicit or implicit, is provided.
 *******************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_BUFFER 1024                        // max char in line
#define MAX_ARGS 64                            // max # args
#define SEPARATORS " \n"                       // token sparators
#define INIT_HIST_LEN 1

int main (int argc, char ** argv)
{
    char buf[MAX_BUFFER];                      // line buffer
    char * args[MAX_ARGS];                     // pointers to arg strings
    char ** arg;                               // working pointer thru args
    char * prompt = "#" ;                      // shell prompt
    char path[MAX_BUFFER];                     // stores path
    getcwd(path, sizeof(path));                // Initialize the path
    char ** history;
    int pc = 0;                                // Program Counter used for hist
    
    history = (char **)calloc(INIT_HIST_LEN,sizeof(char *));
    
    /* keep reading input until "quit" command or eof of redirected input */
    while (!feof(stdin)) {
        
        /* get command line from input */
        fputs (prompt, stdout);                // write prompt
        if (fgets (buf, MAX_BUFFER, stdin )) { // read a line
            
            /* If history if full, double the size */
            if (pc >= sizeof(history)) { //If the last element is not null
                realloc(history, sizeof(history)*2);
            }
            
            /* Write the command to history */
            history[pc] = (char *)malloc(sizeof(buf));
            strcpy(history[pc++], buf);
        
            /* tokenize the input into args array */
            arg = args;
            *arg++ = strtok(buf,SEPARATORS);   // tokenize input
            while ((*arg++ = strtok(NULL,SEPARATORS))); // assign the rest
                                               // of the tokens to the array
                                               // last entry will be NULL
            if (args[0]) {                     // if there's anything there
                /* check for internal/external command */
                if (!strcmp(args[0],"clear")) { // "clear" command
                    system("clear");
                    continue;
                }
            
                if (!strcmp(args[0],"quit"))   // "quit" command
                    break;                     // break out of 'while' loop

                if (!strcmp(args[0], "cd")) {  // "cd" command
                    if (chdir(args[1])) {      // Try to change directories
                        /* If it fails, print out an error */
                        fprintf(stdout, "Error: Could not change directory\n");
                    }
                    getcwd(path, MAX_BUFFER);  // Update internal variable
                    continue;
                }
                if (!strcmp(args[0], "pwd")) { // "pwd" command
                    fprintf(stdout, "%s\n", path);
                    continue;
                }
                
                if (!strcmp(args[0], "history")) { // "pwd" command
                    if (!args[1]) {
                        for (int i=0; i<pc; i++) {
                            printf("%s", history[i]);
                        }
                    }
                    else if (!strcmp(args[1], "-c")) {
                        free(history);
                        history = (char **)calloc(INIT_HIST_LEN,sizeof(char *));
                        pc = 0;
                    }
                    continue;
                }
                if (!strcmp(args[0], "run")) {
                    pid_t pid = fork();
                    if (pid == 0) {
                        execvp(args[1], &args[2]);
                    }
                    else {
                        wait(&pid);
                    }
                    
                }
                
                /* else pass command onto OS (or in this instance, print them out) */
                arg = args;
                while (*arg) fprintf(stdout,"%s ",*arg++);
                fputs ("\n", stdout);
            }
        }
    }
    return 0; 
}

