# **Recreating the System-Wide FD Tables**

# **How I Solve the Question**
I first watched the video demo and compared the output in the demo and provided examples, then I read through
the provided resource such as files and C-libraries to figure out the usage of each. Also, I searched up online to find
out more library and functions I can use. Therefore I started working on the program. 


# **Overview of functions**

## create 
```
fdNode *newfdNode()
```
this function create a new node, and initalize the node to the default information

# 

## insert
```
fdNode *insertfdNode(fdNode *head, int pid, int fd, char filename[1024], int inode){
```
this function create a new node depends on the given information, and insert the node at the end of the linked list

Parameters
- `head` 
- `pid`
- `fd`
- `filename`
- `inode`

#

## delete
```
fdNode *deleteList(fdNode *head)
```
this function delete all memory allocated
Parameters
- `head`

#


## check integer
```
bool check_valid_integer(char *input)
```
this function checks if the input is a integer

Parameters
- `input`
#

## check pid directory
```
bool check_pid_dir(const struct dirent *entry){
```
// this function checks if it is a pid directory

Parameters
- `entry`
#

## check pid existenc
```
bool check_exist_pid(int pid, uid_t current_uid){
```
this function checks if the giveen pid belongs to the current user

Parameters
- `pid`
- `current_uid`
#

## get the overall information
```
fdNode* get_information()
```
this function create the linked list about the disired information
#

## print process table
```
void print_process_table(fdNode *head, int pid)
```
this function prints the process table according to whether the pid is given

Parameters
- `head`
- `pid`

#

## print systemwide table
```
void print_systemwide_table(fdNode *head, int pid)
```
// this function prints the systemwise table according to whether the pid is given 

Parameters
- `head`
- `pid`

## print vnode table
#
```
void print_vnode_table(fdNode *head, int pid)
```
this function prints the vnode table according to whether the pid is given

Parameters
- `head`
- `pid`
#

## print composite table
```
void print_composite_table(fdNode *head, int pid)
```
this function print the compsite table according to whether the pid is given
Parameters
- `head`
- `pid`
#

## print threshold information
```
void print_threshold_information(fdNode *head, int threshold)
```
this function prints the threshold information depending on the number of threshold
Parameters
- `head`
- `threshold`
#

## save composite table to txt
```
void savetxt(fdNode *head, int pid, char *saved_file){
```
this function save the composite table to a file called compositeTable.txt
Parameters
- `head`
- `pid`
- `saved_file`
#

## check_valid_integer
```
void savebinary(fdNode *head, int pid, char *saved_file)
```
this function save the composite table to a file called compositeTable.bin

Parameters
- `head`
- `pid`
- `saved_file`
#

## main
```
main(int argc, char **argv)
```
this is the main function, display memory information, user information, cpu usage, machine information
it has two parameters, argc is the number of arguments and argc is the array of arrguments. 
It parse the arguement and print the desired output

Parameters
- `argc (int)` : Number of command-line arguments
- `argv (char**)` : Array of command-line arguments. 

# **How to Use**

Use the makefile to compile the program. 
1. Open terminal or command prompt
2. Open the directory where the file is saved
3. Type `make`. 
4. run the progtam using `./printing_table`
5. type `make clean` to remove object file


### --per-process
indicates that only the process FD table will be displayed
```
./printing_table --per-process
```

### --systemWide
indicates that only the system-wide FD table will be displayed
```
./printing_table --systemWide
```

### --Vnodes
indicates that the Vnodes FD table will be displayed
```
./printing_table --Vnodes
```

### --composite
indicates that only the composed table will be displayed
```
./printing_table --sequential
```

### --threshold=X
where X denotes an integer, indicating that processes which have a number of FD assigned larger than X should be flagged in the output.
For this it will list the PID and number of assigned FDs, e.g. PID (FD)
```
./printing_table --samples=N
```

# **Design Decison**

Default behaviour: if no argument is passed to the program, then the following approach is implemented:    

the program will display the composite table, i.e. same effect as having used the --composite flag


