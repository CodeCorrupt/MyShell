/* Author   : Tyler Hoyt / t2650010
 *   Email  : hoyt@knights.ucf.edu
 * Class    : COP-4600 / Opperating Systems
 * Semester : Fall 2015
 *
 * Extended form strtokeg by Ian G. Graham.
 *
 * Usage:
 *  Reads in a line of keyboard input at a time, parsing it into
 *  tokens that are separated by white spaces (set by #define
 *  SEPARATORS).
 *
 *  Can use redirected input
 *      
 *  If the first token is a recognized internal command, then that
 *  command is executed. otherwise the tokens are printed on the
 *  display.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

#define MAX_BUFFER 1024                        // max char in line
#define MAX_ARGS 64                            // max # args
#define SEPARATORS " \n"                       // token sparators
#define INIT_HIST_LEN 10

int main (int argc, char ** argv)
{
    char buf[MAX_BUFFER];                      // line buffer
    char * args[MAX_ARGS];                     // pointers to arg strings
    char ** arg;                               // working pointer thru args
    char * prompt = "#" ;                      // shell prompt
    char path[MAX_BUFFER];                     // stores path
    getcwd(path, sizeof(path));                // Initialize the path
    char ** history;                           // two-d array to store history stirngs
    int pc = 0;                                // Program Counter used for hist
    
    history = (char **)calloc(INIT_HIST_LEN,sizeof(char *));
    
    /* keep reading input until "quit" command or eof of redirected input */
    while (!feof(stdin)) {
        
        /* get command line from input */
        fputs (prompt, stdout);                // write prompt
        if (fgets (buf, MAX_BUFFER, stdin )) { // read a line
            
            /* If history if full, double the size */
            if (pc >= sizeof(history)) { //If the last element is not null
                history = realloc(history, sizeof(history)*2);
            }
            
            /* Write the command to history */
            history[pc] = (char *)malloc(sizeof(buf)); // allocate new memory
            if (strcmp(buf, "\n")) {
                strcpy(history[pc++], buf);            // copy in buf
            }
        
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
                        int i;
                        for (i=0; i<pc; i++) {
                            printf("%s", history[i]);
                        }
                    }
                    else if (!strcmp(args[1], "-c")) { // If given the clear flag
                        free(history);              // Release history and make again
                        history = (char **)calloc(INIT_HIST_LEN,sizeof(char *));
                        pc = 0;                     // reset program counter
                    }
                    continue;
                }
                if (!strcmp(args[0], "run")) {      // "run" command
                    pid_t pid = fork();             // fork the process
                    if (pid == 0) {
                        execvp(args[1], &args[1]);  // Have the child exec
                        printf("Could not run program"); // If it hit here, there 
                        return 0;                     // was an error. return.
                    }
                    else {                              // if parent
                        wait(&pid);                 // Wait for child to finish
                    }
                    continue;
                }

                if (!strcmp(args[0], "background")) { // "background" command
                    pid_t pid = fork();                 // Exact same as "run"
                    if (pid == 0) {
                        execvp(args[1], &args[1]);
                        printf("Could not run program");
                        return 0;
                    }                               // except parent does not wate
                    printf("%d\n", pid);            // instead it prints the PID
                    continue;                       // and gets on with it's life
                }                                   // (good parenting, huh?)
                
                if (!strcmp(args[0], "murder")) {   // "murder" command
                    if(!(kill(atoi(args[1]), SIGKILL))) {   // sends SIGKILL to PID
                        printf("The deed is done.\n");          // Let user know if it worked
                    }
                    else {
                        printf("Error: Could not kill process: %s\n", args[1]);
                    }
                    continue;
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

