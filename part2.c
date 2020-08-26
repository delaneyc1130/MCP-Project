/*
* Description: This version of the MCP now schedules programs in the workload to run
               in a time-shared manner. This program takes the code from Part 1 which
               is an initial version of the MCP and builds upon it. This program now 
               implements the SIGUSR1 signal before calling exec thus, the processes 
               will wait for this signal before launching with the exec call. Once all
               processes are running, the signal SIGSTOP will be sent to stop then and then
               SIGCONT to continue them one more until they are terminated.
*
* Author: Delaney Carleton
*
* Date: April 24, 2020
*
* Notes:
* 1. Code taken from Part 1 of Project and then added the new program requirements 
  2. Parse token function now implements signals of SIGUSR1, SIGSTOP, SIGCONT
  3. Signal of SIGUSR1 requires sigmask for proper implementation 
  4. Comments written throughout file
  5. Attempt to account for all errors and segmentation faults that could occur.
  6. No memory leaks or memory errors when checked with Valgrind
  7. Print statements throughout in order to show implementation and execution of program

*/

/*-------------------------Preprocessor Directives---------------------------*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
//Header file optional as stated in office hours
//#include "header.h"
/*---------------------------------------------------------------------------*/


/*-----------------------------Program Launch Process----------------------------------*/


pid_t launch_process(char *command, char **program, int p)
{

/* Launch Process Program based upon Lab and Pseudocode provided in project for Part 1 */

//    printf("Abount to Launch Process %d\n", (p + 1));

    //Define Variables

    //Process
    pid_t pid;

    //Define SIGUSR1
    struct sigaction action;

    //Initializer to avoid Unitialized Memory
    //Discussed on Canvas Discussion Board
    memset(&action, '\0', sizeof(&action));

    sigset_t set; 
    sigset_t oldset;
    int sig;

    //Initializer to avoid Unitialized Memory
    //Discussed on Canvas Discussion Board
    sigemptyset(&set);
    
    sigaddset(&set, SIGUSR1);

    //Use to wait to receive SIGUSR1 from parent
    //SIG_BLOCK - set of blocked signals is union of current set and set argument
    //sigproc mask then takes the set and the oldset
    //sigprocmask needed in order to get proper output and functionality for SIGUSR1   

    sigprocmask(SIG_BLOCK, &set, &oldset);

 //   sigaction(SIGUSR1, &action, NULL);
    
    //Fork Process
    pid = fork();

    //Process Error - Not Loading/Launching
    if (pid < 0)
    {
        //Handle error appropriatly
        printf("Error! Unable to launch Process\n");
        //Exit
        exit(-1);
    }

    //Launch Process 
    if (pid == 0)
    {

        printf("Process: %i is Launching\n", getpid());

        printf("Child Process: %i - Waiting for SIGUSR1 \n", getpid());
        
        //Sleep before SIGUSR1 to show Waiting
        //In office hours Jared recommended including this
        sleep(1);

        //Check for SIGUSR1 
        if (sigwait(&set, &sig) == 0)
        {
            //Recommended by Jared in Office Hours to include which command is being executed 
            printf("Child Process: %i - Received signal: SIGUSR1 - Calling exec() for Command: %s\n", getpid(), command);
            //Execute Command
            execvp(command, program);
//          execv(command, program);
        }

        //Error if Not Executed
        //log error. starting program failed
        printf("Error! Failure to Execute Process: %i  with Command: %s\n", getpid(), command);
        //exit so duplicate copy of MCP doesn't run/fork bomb
        exit(-1);
    }


 //   printf("Returning Process %d\n", (p+1));
    //Return Process
    return pid;

}


/*-----------------------------Program Parse Token----------------------------------*/

//Function to Parse Token
//Functionality similar to parsing token function from Project 1
void parse_token(FILE *file_stream, int numprograms)
{

    //Define Variables

   // FILE *parse_stream;
    char *parse_buffer;
    size_t parse_bufsize = 1024;
    size_t parse_nread;

//      char *save_buffer = NULL;

    char *program[numprograms];
    char *command;

    char *token = NULL;
    //Set index of token for getting commands and arguments
//   int token_size
    int token_index = 0;

    //Set delimiter for token
    const char delim[1] = " ";

    char *copy_token;

//Initial attempt to get copy of string using strcpy. Errors occurred in attempt to execute program
//    char copy_token[1024];
//    char copy_token[token_size];

    //Loop to tokenize
// int n = 0;

    //Loop to launch process
    int i = 0;

    //Loop for free programs
    //Does not implement properly if declared here, must be implemented within the loop
  // int j = 0;

    //Signalers
    int k;

    //Process Pool
    pid_t *pids_pool;

    //Allocate memory for input buffer
    parse_buffer = (char *)malloc(parse_bufsize * sizeof(char));

    //copy_token = malloc(sizeof(char) * (strlen(token) + 1));

    //Allocate memory for process pool
    pids_pool = (pid_t*)malloc(sizeof(pid_t) * 1024);

    //Get lines of input file for parsing
    while((parse_nread = getline(&parse_buffer, &parse_bufsize, file_stream)) != -1) 
    {

        //Loop to tokenize
        int n = 0;

        //Tokenize on delimiter of a space
        token = strtok(parse_buffer, delim);

       /*Initially followed the same implementation of Project 1 through using strtok_r and a save_buffer but ran into errors with the program 
         not parsing correctly and getting incorrect lines with incorrect commands and arguments
         Through looking at the manual page for strtok as well as viewing further explanations and examples from stackoverflow and geeksforgeeks,
         found that I should implement strtok with the delimeter of a space in order to correctly parse the file. There were very helpful resources
         on stackoverflow along with reading the manual page which helped in gaining a greater understanding of how to properly parse the file to
         get each line with the command and its argument*/

      //  save_buffer = parse_buffer;
    
        //While Loop to tokenize through line
        //Look for Newline while parsing tokens
        //while((token = strtok_r(save_buffer, "\r", &save_buffer);
        //while((token = strtok(parse_buffer, " ");

    
        while(token != NULL)
        {
            //Check token indexed for occurence of newline
            token_index = (strlen(token) - 1);

        // Initial attempt of using strcpy to get copy of token to get program and arguments
        //    token_size = strlen(token);
        //    copy_token[token_size];

//          copy_token = strdup(token);

            //Check for Newline
            //Use strcmp and strstr to check for occurence of newline. Same logic as implemented in Project 1
            if ((strcmp((&token[token_index]), "\n") == 0) || (strstr((&token[token_index]), "\n") != NULL))
            {

/*Initially if a newline occurred then the program would be set to be the token.
  An error occurred in my implementataion as when I tried to run this, segmentation faults occurred. 
  Upon further research in figuring out how to set a program to the token, examples and discussions on stackoverflow and tutorialspoint
  assisted me in determining that I should create a new variable to hold this value. I first tried setting copy_token to token and also
  using strcpy for copy_token but both caused errors. In looking for similar implementations to strcpy, stackoverflow and cboard provided
  examples for copying a duplicate string through using strdup when allocatting memory */


                //If a newline, then set to 0 for proper tokenizing
                token[token_index] = 0;

    //            copy_token = token;
    //            strcpy(copy_token, token);
                copy_token = strdup(token);

 //Attempt with strcpy. Errors occurred with storing proper command and argument 

                //strcpy(copy_token, token);
                
                program[n] = copy_token;

                //program[n] = token;



            }

           //Otherwise continue on line
            else
            {

            // copy_token = token;
            // strcpy(copy_token, token);

                copy_token = strdup(token);
    
    //Attempt with strcpy. Errors occurred with storing proper command and argument
                //strcpy(copy_token, token);

                program[n] = copy_token;

                //program[n] = token;


            }

            //Increment through loop
            n++;

            //Tokenize on delimiter
            token = strtok(NULL, delim);

            //Set to NULL
            program[n] = NULL;

        }


        //Get workload command
        command = program[0];

        //printf("Command for Workload program is: %c\n", command);

        //Function to launch processes of program
        //printf("About to Launch Processes\n");
        pids_pool[i] = launch_process(command, program, i);

        //Iterate through Processes
        i++;

/*Program ran correctly without this implementation but when checked with Valgrind experienced memory leaks
  Issue with memory leaks. After viewing explanations/tutorials found that needed to free the commmand
  and free the arguments in order to avoid valgrind errors */

//        printf("Freeing Programs\n");

        //Free Programs
        int j = 0;
        while(program[j] != NULL)
        {
            free(program[j]);
            j++;
        }

        // free(program);
        
    }

    //printf("About to Start Sending SIGUSR1\n");

    //Signaler for SIGUSR1
    for(k = 0; k < numprograms; k++)
    {
        printf("Parent Process: %i - Sending Signal: SIGUSR1 to Child Process %i\n", getpid(), pids_pool[k]);
        kill(pids_pool[k], SIGUSR1);

    }

    //Include sleep in between signals in order to demonstrate execution
    //Sleep
    sleep(10);

    //printf("About to Start Sending Stop\n");
    //Signaler for SIGSTOP
    for(k = 0; k < numprograms; k++)
    {
        printf("Parent Process: %i - Sending Signal: SIGSTOP to Child Process %i\n", getpid(), pids_pool[k]);
        kill(pids_pool[k], SIGSTOP);
    }

    //Include sleep in between signals in order to demonstrate execution
    //Sleep
    sleep(10);

    //printf("About to Start Sending Continue\n");
    //Signaler for SIGCONT
    for(k = 0; k < numprograms; k++)
    {
        printf("Parent Process: %i - Sending Signal: SIGCONT to Child Process %i\n", getpid(), pids_pool[k]);
        kill(pids_pool[k], SIGCONT);
    }


    //printf("About to Start Waiting for Processes\n");
    //Wait for Processes to Finish
    for(k = 0; k < numprograms; k++)
    {
        //Based on manual page and tutorials, set status to NULL to ignore child return status and set options to 0
        waitpid(pids_pool[k], NULL, 0);
        printf("Done Waiting for Process: %u\n", pids_pool[k]);
    }


    //All Processes Joined/Complete
    printf("ALL Processes are Complete. Exiting\n");  

//Close File in Main
// fclose(file_stream);

    //Free Allocated Memory
    free(parse_buffer);
// free(copy_token);
    free(pids_pool);

}


/*-----------------------------Program Main----------------------------------*/

int main(int argc, char *argv[])
{

//    setbuf(stdout, NULL);

    /* Main Function Variables */
    
    FILE *stream;
    FILE *stream2;
    
    char *buffer;
    size_t bufsize = 1024;

 //   char *buffer2;
  //  size_t bufsize2 = 1024;

    ssize_t nread1;
   // ssize_t nread2;

    //char *save_buffer = NULL;

    int numprograms = 0;
    int i;

    char *token = NULL;


    /* Allocate memory for input buffer */
    buffer = (char *)malloc(bufsize * sizeof(char));

   // buffer2 = (char *)malloc(bufsize2 * sizeof(char));

    //No input file given
    if (argc < 2)
    {
        printf("Error! No File Input Given for Program. Exiting\n");
        
        //Free buffer to avoid memory leaks
        free(buffer);

        //Exit
        exit(1);
    }

    //User Input File Given
    else if (argc == 2)
    {
    
        /* Read Input File */
        stream = fopen(argv[1], "r");

        //Check if Valid File
        if(stream == NULL)
        {
            printf("Error! Unable to Open File. Exiting\n");
            
            //Exit
            exit(1);
        }

        //Read file for parsing
        stream2 = fopen(argv[1], "r");

    }

    //Not Valid Program
    else
    {
        printf("Error! Not Valid Input for Program. Exiting\n");
        
        //Free buffer to avoid memory leaks
        free(buffer);
    
        //Exit
        exit(1);
    }


    /* Get lines of input file */
    /*Count number of programs using nread1 by reading each line of the input file*/
    while ((nread1 = getline(&buffer, &bufsize, stream)) != -1)
    {
        /*
        save_buffer = buffer;
        
        while ((token = strtok_r(save_buffer, "\n\r", &save_buffer)) != NULL)
        {

            printf("Program is: %s with args %s\n", token, buffer);
        }
        */

        numprograms++;

        //Checker for line of input File
//        printf("Line of Input: %s\n", buffer);
    }
        
    //Check for number of programs 
 //   printf("Number of programs: %d\n", numprograms);

    //Call function to parse input token for commands
    //Within this function also calls function to launch process
    parse_token(stream2, numprograms);


    //Close Files
    fclose(stream);
    fclose(stream2);

    //Free allocated memory
    free(buffer);
   // free(buffer2);

    //Exit 
    exit(0);

    //Return 
    return 0;
}
/*---------------------------------------------------------------------------*/
