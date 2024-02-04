# **How I Solve the Question**
I first watched the video demo and compared the output in the demo and provided examples, then I read through
the provided resource such as files and C-libraries to figure out the usage of each. Also, I searched up online to find
out more library and functions I can use. Therefore I started working on the program. 


# **Overview of functions**

## generate_header(int samples, int tdelay, bool sequential, int curr_line)
this function generate the header of the printed information according to the sequential, including the number of samples and the time of tdelay
if sequential, the print the number of iteration based on curr_line

## generate_memory_usage()
this function generate the information of the memory usage using <sys/resourse.h>

## calculate_memory_info(char memory_info[][1024], int curr_line)
this function calculate the memory information, store the information in the array memory_info at the index curr_line
and return virtue used memory for future use

## generate_memory_graphic(int curr_line, double *last_memory, double now_memory, char memory_info[][1024])
this function generate the memory graph, based on the memory before delay and after the delay
and store the information in the memory_info based on the index curr_line

## read_cpu_stat_return_sum()
this function read the /proc/stat file and return the sum of fields

## read_cpu_stat_return_idle()
this function read the/proc/stat/ file, return the idle time of the cpu

## generate_cpu(int cpu_now_idle, int cpu_now_sum, int cpu_last_idle, int cpu_last_sum)
this function calculate the usage of the cpu using the cpu idle time and sum time before the delay and after the delay

## generate_cpu_graphic(int samples, double now_cpu, char cpu_info[][1024], int curr_line, bool sequential)
this function generate the graphic of cpu information, store the graph in the cpu_info array in the index curr_line, based on the cpu current usage
and print the overall graph based on sequential

## generate_memory_info(int samples, char memory_info[][1024], int curr_line, bool sequential)
this function generate the memory_info before samples based on if user eneter --sequential or not

## generate_user()
this function generates all the users' information using utmp.h

## generate_cores()
this function generates the number of cores using sysconf

## generate_system_information()
this function generates infomation of the system

## check_valid_integer(char *input)
this function checks if the user input is valid integer

## main(int argc, char **argv)
this is the main function, display memory information, user information, cpu usage, machine information
it has two parameters, argc is the number of arguments and argc is the array of arrguments

parse the arguement and print the desired output

# **How to Use**

## Compile the program using 
```
gcc mySystemStats.c -o mySystemStats
```
## Run the program
```
./mySystemStats
```

### --system
to indicate that only the system usage should be generated
```
./mySystemStats --system
```

### --user
to indicate that only the users usage should be generated
```
./mySystemStats --user
```

### --graphic
 to include graphical output in the cases where a graphical outcome is possible as indicated below.
```
./mySystemStats --graphic
```
or
```
./mySystemStats -g
```
#### Graphical representations
for Memory utilization:
````
::::::@  total relative negative change
######*  total relative positive change

(OPTIONAL)
|o    zero+
|@    zero-
````

for CPU utilization:
````
||||   positive percentage increase
````

### --sequential
to indicate that the information will be output sequentially without needing to "refresh" the screen 
(useful if you would like to redirect the output into a file)
```
./mySystemStats --sequential
```

### --samples=N
if used the value N will indicate how many times the statistics are going to be collected and results will be average and reported based on the N number of repetitions.
If not value is indicated the default value will be 10.
```
./mySytemStats --samples=N
```

### --tdelay=T
to indicate how frequently to sample in seconds.
If not value is indicated the default value will be 1 sec.
```
./mySystemStats --tdelays=T
```
### other way for setting samples and tdelay
make sure the order is correct, the first number is samples and the second is tdelay
```
./mySystemStats 8 2
```

