/* gopipe.c
 *
 * CSC 360, Summer 2023
 *
 * Execute up to four instructions, piping the output of each into the
 * input of the next.
 *
 * Please change the following before submission:
 *
 * Author: Ryo Tabata 
 */

/* Note: The following are the **ONLY** header files you are
 * permitted to use for this assignment! */


#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>




#define MAX_NUM_TOKENS 8
#define MAX_INPUT_LINE 80

int cmdcount = 0;
char cmdlines[4][80];
void tokenizebuffer();
void processcommands(char *cmd_head[MAX_NUM_TOKENS][MAX_NUM_TOKENS], int cmd_head_count);


int main(int argc, char *argv[]) {
    int count = 0;
    char buffer[MAX_INPUT_LINE];
    ssize_t s;
    //reading in line by line
    while ((s = read(0, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[s] = '\0';
        if (strncmp(buffer, "\n", 1) == 0) {
            break;
        }
        if (buffer[s - 1] == '\n') {
            buffer[s - 1] = '\0';
        }
        strncpy(cmdlines[cmdcount], buffer, strlen(buffer));
        cmdcount++;
        count++;
        if (count >= 4) {
            break;
        }
    }
    tokenizebuffer();
    return 0;
}


void tokenizebuffer() {
    char *cmd_head[MAX_NUM_TOKENS][MAX_NUM_TOKENS];
    int cmd_head_count = 0;

    for (int i = 0; i < cmdcount; i++) {
        char *tokenized_cmd[MAX_NUM_TOKENS];
        int token_count = 0;
        char *t = strtok(cmdlines[i], " ");
        while (t != NULL && token_count < MAX_NUM_TOKENS) {
            tokenized_cmd[token_count] = t;
            token_count++;
            t = strtok(NULL, " ");
        }
        tokenized_cmd[token_count] = NULL;
        memcpy(cmd_head[cmd_head_count], tokenized_cmd, sizeof(char *) * (token_count + 1));
        cmd_head_count++;
    }
    processcommands(cmd_head, cmd_head_count);
}

//used code from appendix 
// inspo from https://barsky.ca/marina/SR/c2018/lectures/7.pipe/06.01.FileDescriptorsPipes.pdf
//this function goes through our commandlines and prints pipes and redirects our output

void processcommands(char *cmd_head[][MAX_NUM_TOKENS], int cmd_head_count) {
    int status;
    int pid[cmd_head_count];//makes pid for each of the children we will need to each cmdline
    int fd[cmd_head_count - 1][2]; //fd for each of the pipes

    for (int i = 0; i < cmd_head_count - 1; i++) {
        pipe(fd[i]);//piping for each children
    }

    for (int i = 0; i < cmd_head_count; i++) {
        if ((pid[i] = fork()) == 0) {
            //in child process
            if (i == 0) {
                //first command
                dup2(fd[i][1], 1); //output into pipe
                close(fd[i][0]);
                
            } else if (i == cmd_head_count - 1) {
                //final command 
                dup2(fd[i-1][0], 0); //input from the pipe, because our output is not piped in final command
                close(fd[i-1][1]);
            } else {
                //all other commands that arent first or last
                dup2(fd[i-1][0], 0); //input from the previous pipe
                dup2(fd[i][1], 1); // output to the current pipe
                close(fd[i-1][1]);
                close(fd[i][0]);
            }

            // Close all pipe file descriptors
            for (int j = 0; j < cmd_head_count - 1; j++) {
                close(fd[j][0]);
                close(fd[j][1]);
            }

            execvp(cmd_head[i][0], cmd_head[i]);
            // perror("execvp");
            // exit(1);
        }
    }

    //close pipes for each child input and output, 1&0
    for (int i = 0; i < cmd_head_count - 1; i++) {
        close(fd[i][0]);
        close(fd[i][1]);
    }

    //parent waiting for children processes to complete
    for (int i = 0; i < cmd_head_count; i++) {
        waitpid(pid[i], &status, 0);
    }
}
