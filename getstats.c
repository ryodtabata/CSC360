/* getstats.c 
 *
 * CSC 360, Summer 2023
 *
 * - If run without an argument, dumps information about the PC to STDOUT.
 *
 * - If run with a process number created by the current user, 
 *   dumps information about that process to STDOUT.
 *
 * Please change the following before submission:
 *
 * Author: Ryo Tabata
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_full_info();
void print_process_info(char * process_num);
void version();
void memtotal();
void uptime();
void switchcounter(char * switchstr);
void printfilename(char * process_num);
int switchcount = 0;

//prints specific task number
void print_process_info(char * process_num) {
    char filename[100];
    char line[255];
    FILE *file;
    sprintf(filename,"/proc/%s/status",process_num);
    file = fopen(filename, "r");
    if (file==NULL){
        char fnf[50];
        sprintf(fnf,"Process number %s not found.",process_num);
        printf("%s\n",fnf);
        return;
    }
    else {
        char vol[] = "voluntary_ctxt_switches:";
        char invol[] = "nonvoluntary_ctxt_switches:";
        char namestr[] = "Name:";
        char process[50];
        char threads[] = "Threads:";
        sprintf(process,"Process number: %s",process_num);
        printf("%s\n",process);
        while (fgets(line, 255, file) != NULL){
            if (strncmp(namestr,line,strlen(namestr))==0){
                 printf("%s", line);  
                 printfilename(process_num);
            }
            else if (strncmp(threads,line,strlen(threads))==0){
                 printf("%s", line);  
            }
            else if ((strncmp(vol,line,strlen(vol))==0)||(strncmp(invol,line,strlen(invol))==0)){
                switchcounter(line);
            } 
        }
    }
    printf("Total context swtiches: %d\n",switchcount);
    fclose(file);
}


//prints filename
void printfilename(char* process_num){
    FILE *file;
    char filename[100];
    char line[255];
    sprintf(filename,"/proc/%s/cmdline",process_num);
    file = fopen(filename, "r");
    if (file==NULL){
        return;
    }
    while (fgets(line, 255, file) != NULL){
         printf("Filename: %s\n",line);
    }
    fclose(file);
}

//adds the swtiches 
void switchcounter(char * switchstr){
    char *ptr = strtok(switchstr, ": ");
    while(ptr != NULL)
	{
		if (atoi(ptr)!=0){
            int number = atoi(ptr);
            switchcount += number;
        }
		ptr = strtok(NULL, ": ");
    }
}

//prints all info
void print_full_info() {
    FILE *file;
    file = fopen("/proc/cpuinfo", "r");
    char line[255];
    int model_name_count = 0;
    int cpu_cores_count = 0;
    while ((fgets(line, 255, file) != NULL) && (model_name_count == 0 || cpu_cores_count == 0)){
        char model_name[] = "model name";
        char cpu_cores[] =  "cpu cores";
        if (strncmp(model_name,line,strlen(model_name))==0){
            model_name_count= 1;
            printf("%s", line);
        }
        else if (strncmp(cpu_cores,line,strlen(cpu_cores))==0){
            cpu_cores_count = 1;
            printf("%s", line);
        }
    }
    version();
    memtotal();
    uptime();
    fclose(file);
    }

//finds Linux version number
void version(){
    FILE *file;
    file = fopen("/proc/version", "r");
    char line[255];
    while (fgets(line, 255, file)!= NULL){
        printf("%s", line);
    }
    fclose(file);
}

//finds total mem 
void memtotal(){
    FILE *file;
    file = fopen("/proc/meminfo", "r");
    char line[255];
    char memorytotal[] = "MemTotal";
    while (fgets(line, 255, file) != NULL){
        if (strncmp(memorytotal,line,strlen(memorytotal))==0){
            printf("%s",line);
            break;
        }
    }
    fclose(file);
}

//finds the uptime and converts to days,hours,mins,seconds
void uptime(){
    FILE *file;
    file = fopen("/proc/uptime", "r");
    char line[255];
    char *token;
    while (fgets(line, 255, file) != NULL){
        token = strtok(line," ");
    }
    int totalseconds = atoi(token);
    char str[100];
    int days = totalseconds/86400;
    totalseconds = totalseconds%86400;
    int hours = totalseconds/3600;
    totalseconds = totalseconds%3600;
    int mins = totalseconds/60;
    totalseconds = totalseconds%60;
    sprintf(str, "Uptime: %d days, %d hours, %d minutes, %d seconds", days,hours,mins,totalseconds);
    printf("%s\n", str);
    fclose(file);
}

int main(int argc, char ** argv) {  
    if (argc == 1) {
        print_full_info();
        return 0;
    } else {
        print_process_info(argv[1]);
        return 0;
    }
}

