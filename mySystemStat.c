#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <utmp.h>
#include <unistd.h>
#include <math.h>

// this function generate the header of the printed information according to the sequential, including the number of samples and the time of tdelay
// if sequential, the print the number of iteration based on curr_line
void generate_header(int samples, int tdelay, bool sequential, int curr_line){
    if(sequential){ // if user enters sequential, then true
        printf(">>> iteration %d\n", curr_line); // print the number of the iteration
    }
    else{ //if user doesn't enter sequential, false
        printf("\033[H\033[2J"); // clear screan
        printf("Nbr of samples: %d -- every %d secs\n", samples, tdelay); // print the number of samples and tdelay
    }
}

// this function generate the information of the memory usage using <sys/resourse.h>
void generate_memory_usage(){
    struct rusage memory_usage; // declear the struct using <sys/resource.h>
    getrusage(RUSAGE_SELF, &memory_usage); // get memory usgae
    printf("Memory usage: %ld kilobytes\n",memory_usage.ru_maxrss); // generate the memory usage using the struct
}

//this function calculate the memory information, store the information in the array memory_info at the index curr_line
// and return virtue used memory for future use
double calculate_memory_info(char memory_info[][1024], int curr_line){
    struct sysinfo memory_stat; // declear the struct using <sys/sysinfo.h>
    sysinfo(&memory_stat); // get the memory information
    // convert KB to GB by divide by 10^9
    double totalram = (float)memory_stat.totalram / 1000 / 1000 / 1000; 
    double freeram = (float)memory_stat.freeram / 1000 / 1000 / 1000;
    double totalswap = (float)memory_stat.totalswap / 1000 / 1000 / 1000;
    double freeswap = (float)memory_stat.freeswap /  1000 / 1000 / 1000;

    //calculate the desired information
    double phys_used = totalram - freeram;
    double virt_used = phys_used + totalswap - freeswap;
    double virt_total = totalram + totalswap;

    //store the information in the array according to the current time wanted
    sprintf(memory_info[curr_line], "%.2f GB / %.2f GB -- %.2f GB / %.2f GB", phys_used, totalram, virt_used, virt_total);  
    return virt_used;  
}

// this function generate the memory graph, based on the memory before delay and after the delay
// and store the information in the memory_info based on the index curr_line
void generate_memory_graphic(int curr_line, double *last_memory, double now_memory, char memory_info[][1024]){
    double diff = 0; // set the default diff between current and last time to 0;
    char graphic_info[1024] = "\0";    // declear the array for storing graphic information 
    char data_info[1024] = "\0";    // declear the arry for storing the data information
    double rounded_diff;    // store the rounded information to two decimal place
    int bar_num = 0;    // store the number of bars 
    strcpy(graphic_info, "    |");  

    if(curr_line == 0){
        diff = 0.00;   // for the first, diff is 0 by default
        rounded_diff = 0.00;    // rounded diff is also 0.00
    }
    else{
        diff = now_memory - *last_memory;   // the diff is calculated by the difference betwee now memory usage and the last time memoru usage
        rounded_diff = round(diff * 100.0) / 100.0;     // round the diff to two decimal place
    }
    
    bar_num = floor(fabs(diff) * 100);  // get the number of bar that without rounding
    if(diff >= 0 && rounded_diff == 0.00){     // zero+ : if the difference is greater than 0 but the rounded is 0.00
        strcat(graphic_info, "o "); 
    }
    else if(diff < 0 && rounded_diff == 0.00){  // zero- : if the difference is smaller than 0 but the rounded is 0.00
        strcat(graphic_info, "@ ");
    }
    else if(rounded_diff > 0){      // relative postive change
        for(float i = 0; i < bar_num; i++){
            strcat(graphic_info, "#");
        }
        strcat(graphic_info, "* ");
    }
    else if(rounded_diff < 0){     // relative negative change
        for(float i = 0; i < bar_num; i++){
            strcat(graphic_info, ":");
        }
        strcat(graphic_info, "@ ");
    }
    *last_memory = now_memory;      // store the now memory usage as the last memory usage for next time usage
    sprintf(data_info, " %.2f (%.2f)", diff, now_memory);  // store the format
    strcat(graphic_info, data_info);
    strcat(memory_info[curr_line], graphic_info);   // store the information into the array
}

// this function read the /proc/stat file and return the sum of fields
int read_cpu_stat_return_sum(){
    FILE *file = fopen("/proc/stat", "r"); // open the file
    if(file == NULL){    //checking if successfully open the file
        fprintf(stderr, "Error opening file\n");  // if failed,  print the error
        exit(1);    // exit the program
    }
    char filecontent[1024];     // declear the array for storing file content
    fgets(filecontent, 1024, file); // copy the content in the file to the char array
    fclose(file);  // close the file
    
    char *token = strtok(filecontent, " "); // the first is cpu
    token = strtok(NULL, " "); // move to user
    int cpu_user = atoi(token); // store user
    token = strtok(NULL, " "); // move to nice
    int cpu_nice = atoi(token); // store nice
    token = strtok(NULL, " "); // move to system
    int cpu_system = atoi(token); // store system
    token = strtok(NULL, " "); // move to idle
    int cpu_idle = atoi(token); // store idle
    token = strtok(NULL, " "); // move to iowait
    int cpu_iowait = atoi(token); // store iowait
    token = strtok(NULL, " "); // move to irq
    int cpu_irq = atoi(token); // store irq
    token = strtok(NULL, " "); // move to softirq
    int cpu_softirq = atoi(token); // store softirq
    token = strtok(NULL, " "); // move to steal
    int cpu_steal = atoi(token); // store steal
    token = strtok(NULL, " "); // move to guest
    int cpu_guest = atoi(token); // store guest

    //get the sum of fields
    int sum = cpu_user + cpu_nice + cpu_system + cpu_idle + cpu_iowait + cpu_irq + cpu_softirq + cpu_steal + cpu_guest;

    return sum;
}

// this function read the/proc/stat/ file, return the idle time of the cpu
int read_cpu_stat_return_idle(){
    FILE *file = fopen("/proc/stat", "r");    // open the file
    if(file == NULL){    //checking if successfully open the file
        fprintf(stderr, "Error opening file\n");  // if failed,  print the error
        exit(1);    // exit the program
    }
    char filecontent[1024];     // declear the array
    fgets(filecontent, 1024, file);     // copy the file content into the array
    fclose(file);   //close the array

    char *token = strtok(filecontent, " "); // the first is cpu
    token = strtok(NULL, " "); // move to user
    token = strtok(NULL, " "); // move to nice
    token = strtok(NULL, " "); // move to system
    token = strtok(NULL, " "); // move to idle
    int cpu_idle = atoi(token); // store idle

    return cpu_idle;
}

// this function calculate the usage of the cpu using the cpu idle time and sum time before the delay and after the delay
double generate_cpu(int cpu_now_idle, int cpu_now_sum, int cpu_last_idle, int cpu_last_sum){
    int cpu_diff = cpu_now_sum - cpu_last_sum;      // calculate the diff
    int cpu_idle = cpu_now_idle - cpu_last_idle;    // calculate the diff of idle
    int cpu_non_idle = cpu_diff - cpu_idle;     // calculate the used
    double cpu_usage = 100 * cpu_non_idle / (cpu_diff + 1);  //add one to avoid the denominator is 0, convert to percentage

    return cpu_usage;
}

// this function generate the graphic of cpu information, store the graph in the cpu_info array in the index curr_line, based on the cpu current usage
// and print the overall graph based on sequential
void generate_cpu_graphic(int samples, double now_cpu, char cpu_info[][1024], int curr_line, bool sequential){
    char graphic_info[1024] = "\0";
    
    strcat(cpu_info[curr_line], "       |||");

    for(int i = 0; i < (int)now_cpu; i++){
        strcat(cpu_info[curr_line], "|");
    }

    sprintf(graphic_info, " %.2f", now_cpu);
    strcat(cpu_info[curr_line], graphic_info);

    if(sequential)  // if the user enter --sequential 
    {
        for(int i = 0; i < samples; i++){      
            if(i == curr_line){     // for the line that correspond to the iteration
                printf("%s\n", cpu_info[i]);     // print the information
            }
            else{
                printf("\n");     // for the lines that doesn't correspond to the iteration, print empty line
            }
        }
    }
    else{      // if the user doesn't enter --sequential
        for(int i = 0; i <= curr_line; i++){      // for the lines that up to the current line
            printf("%s\n", cpu_info[i]);      // print the infomation
        }
        for(int i = curr_line + 1; i < samples; i++){     // for the rest, after the current line
            printf("\n");    // print the empty line
        }
    }
}

// this function generate the memory_info before samples based on if user eneter --sequential or not
void generate_memory_info(int samples, char memory_info[][1024], int curr_line, bool sequential){
    printf("---------------------------------------\n");
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot) \n");
    if(sequential)  // if the user enter --sequential 
    {
        for(int i = 0; i < samples; i++){      
            if(i == curr_line){     // for the line that correspond to the iteration
                printf("%s\n", memory_info[i]);     // print the information
            }
            else{
                printf("\n");     // for the lines that doesn't correspond to the iteration, print empty line
            }
        }
    }
    else{      // if the user doesn't enter --sequential
        for(int i = 0; i <= curr_line; i++){      // for the lines that up to the current line
            printf("%s\n", memory_info[i]);      // print the infomation
        }
        for(int i = curr_line + 1; i < samples; i++){     // for the rest, after the current line
            printf("\n");    // print the empty line
        }
    }
} 


// this function generates all the users' information using utmp.h
void generate_user(){
    printf("---------------------------------------\n");
    printf("### Sessions/users ### \n");
    
    struct utmp *ut;    //declear the struct
    setutent();     //go to the start of the user information
    while((ut = getutent()) != NULL){   // if the cuser urrent information is not NULL, also, read the next user information
        if(ut -> ut_type == USER_PROCESS){  // check if the user is currently login
            printf(" %s\t%s (%s)\n", ut -> ut_user, ut -> ut_line, ut -> ut_host);
        }
    }
} 

// this function generates the number of cores using sysconf
void generate_cores(){
    printf("---------------------------------------\n");
    printf("Number of cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));    // using sysconf to return the number of cores
}

//this function generates infomation of the system
void generate_system_information(){
    printf("---------------------------------------\n");
    printf("### System Information ### \n");
    struct utsname system_inf;      // declare the struct if utsname
    uname(&system_inf);     // get the infomation of system 
    printf("System Name = %s\n", system_inf.sysname);
    printf("Machine Name = %s\n", system_inf.nodename);
    printf("Version = %s\n", system_inf.version);
    printf("Release = %s\n", system_inf.release);
    printf("Architecture = %s\n", system_inf.machine);

    FILE *file = fopen("/proc/uptime", "r"); // open the /proc/uptime file to get the system running time
    if(file == NULL){    //checking if successfully open the file
        fprintf(stderr, "Error opening file\n");  // if failed,  print the error
        exit(1);    // exit the program
    }
    char filecontent[1024];     // declear the array
    fgets(filecontent, 1024, file);     // copy the content in the file into the array
    fclose(file);       // close the file

    char *token = strtok(filecontent, " ");     // get the first number in the file
    float time = atof(token);  // convert the time to float
    // convert the time to days hours minutes and seconds
    int days = time / 86400;
    time -= days * 86400;
    int hours = time / 3600;
    time -= hours * 3600.0;
    int minutes = time / 60;
    time -= minutes * 60.0;
    int seconds = time;
    int totalhours = 24 * days + hours;
    printf("System running since last reboot: %d days %d:%d:%d (%d:%d:%d)\n", days, hours, minutes, seconds, totalhours, minutes, seconds);
}

// this function checks if the user input is valid integer
bool check_valid_integer(char *input) {
    int i = 0;
    while (input[i] != '\0') {      // when the input is nonempty
        if (!isdigit(input[i])) {   //if the input is not integer
            return false;   // return false
        }
        i++;
    }
    return true;    // else, return true
}


int main(int argc, char **argv) {
    int samples = 10; // default number of samples set to 10
    int tdelay = 1; // default number of seconds set to 1
    bool system = false;    // default status is false;
    bool user = false;      // default status is false;
    bool sequential = false;    // default status is false;
    bool graphic = false;   // default status is false;
    bool sampleflag = false;    // default status is false;
    bool tdelayflag = false;    // default status is false;
    double last_memory = 0.00;    // default number is 0.00
    char memory_info[samples][1024];    // declare the array for memory information 
    char cpu_info[samples][1024];      // declare the array for cpu information
   

    if (argc > 1){
        int i = 1;  // Start from index 1 to skip the program name (argv[0])
        while (i < argc){   // iterate whening i is smaller than the number of argc
            char *token = strtok(argv[i], "=");     // if the user enters --samples=N or --tdelay=N, split these input
            if(strcmp(token, "--samples") == 0){    //if the user enters --tsamples=N, and split at =, check if input is correct
                token = strtok(NULL, "");
                if(token != NULL){      // if N is nonempty
                    samples = atoi(token);      // convert N to integer, and store in samples
                    sampleflag = true;      // turn sampleflag to true
                } 
            } 
            else if(strcmp(token, "--tdelay") == 0){    // if the user enters --tdelay=N. and split at =, ckeck if input is correct
                token = strtok(NULL, "");
                if(token != NULL){      // if N in nonempty
                    tdelay = atoi(token);      // convert N to integer, and store in tdelay
                    tdelayflag = true;      // turn tdelayfalg to true
                } 
            } 
            else if(strcmp(argv[i], "--system") == 0){  // if the user enters --system
                system = true;      // turn system to true
            } 
            else if(strcmp(argv[i], "--user") == 0){    // if the user enters --user
                user = true;        // turn user to true
            } 
            else if(strcmp(argv[i], "--sequential") == 0){      // if the user enters --sequential
                sequential = true;      // turn sequential to true
            } 
            //if the user doesn't put any words but integer, check if input has two consecutive numbers and they are valid integers
            else if(check_valid_integer(argv[i]) && i + 1 < argc && check_valid_integer(argv[i + 1])) {
                // Assume integer values for samples and tdelay in order
                samples = atoi(argv[i]);    // convert the number in argv[i] to int, and store in samples
                sampleflag = true;      // turn sampleflag to true
                tdelay = atoi(argv[i + 1]);     // convert the number in argv[i+1] to int, and store in tdelay
                tdelayflag = true;      // turn tdelayfalg to true
                i++;     // skip the next argument since we already check it is the valid integer and already store it
            }
            else if(strcmp(argv[i], "--graphic") == 0 || strcmp(token, "-g") == 0){ // check if the user eneters --graphic or -g
                graphic = true;     // turn graphic to true
            }
            else{   //if the input is none of the above
                printf("INVALID INPUT\n");      // print invalid input
                return 1;   // exit the program
            }
            i++;    // go to the next
        }       
    }

    // if the user enter both --user and --system or user doesn't enter --user
    if((user && system)|| !user ){
        for(int i = 0; i < samples; i++){   // iterate from 0 to samples - 1
            int cpu_last_sum = read_cpu_stat_return_sum();  // get the cpu sum before delay
            int cpu_last_idle = read_cpu_stat_return_idle();    // get the cpu idle before delay
            sleep(tdelay);  // cause the delay
            generate_header(samples, tdelay, sequential, i); //print the header for situation
            generate_memory_usage();    // print memory usage information
            double now_memory = calculate_memory_info(memory_info, i); // store the memory information in the array memory_info
            if(graphic){    // if user enters --graphic or -g
                generate_memory_graphic(i, &last_memory, now_memory, memory_info);  // modify array memory_info, add graphic information
            }
            generate_memory_info(samples, memory_info, i, sequential);  // print the memory information according to the current line
            if((user && system) || !system){    // if user enter both --user and system
                generate_user();    // print user information
            }
            generate_cores();   // print core information
            int cpu_now_sum = read_cpu_stat_return_sum();   // get the cpu sum after delay
            int cpu_now_idle = read_cpu_stat_return_idle();     // get the cpu idle after delay
            float cpu_use = generate_cpu(cpu_now_idle, cpu_now_sum, cpu_last_idle, cpu_last_sum);   // calculate cpu usage
            printf(" total cpu use = %.2f%%\n", cpu_use);   // print cpu usage
            
            if(graphic){    // if user enters --graphic or -g
                generate_cpu_graphic(samples, cpu_use, cpu_info, i, sequential);    // print graphic for cpu
            }
            
        }
    }
    // if the user only enters user
    else{
        generate_header(samples, tdelay, sequential, 0);    // print header
        generate_memory_usage();    // print memory usage
        generate_user();    //print user information
    }
    // print information for system
    generate_system_information();
    return 0;
}
