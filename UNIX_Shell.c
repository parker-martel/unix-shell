#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80

// Check for "&" in user input
bool checkAmpersand(char *argv){
  for (int i = 0; i < MAX_LINE; i++){
    if (argv[i] == '&'){
      return true;
    }
  }
  return false;
}

// Function that parses input to create array for execvp, and then creates a process using fork
int Parse_Fork(char *input){
    // Parse input into tokens and store in array argv to be passed to execvp
    static char* argv[500];
    memset(argv, 0, sizeof(argv));
    char break_chars[] = {" "};
    char* p = strtok(input, break_chars);
    argv[0] = p;   
    int i = 0;
    while (p != NULL) {
    p = strtok(NULL, break_chars);
    argv[++i] = p;
    }
    argv[i + 1] = NULL;


    // Create process
    pid_t pid = fork();

    // Child process
    if (pid == 0) { 
      execvp(argv[0], argv);
    }

    // Fork failed
    else if (pid < 0){
    printf("Fork failed.");
    return -1;
    }

    // Parent process
    else {
    // If user entered an Ampersand, wait() is called and the parent 
    // process will wait for the child to exit
    if (checkAmpersand(*argv)){
      wait(NULL);
      printf("Waiting on child...\n");
    }
  }
}

int main(void){

  // Initialize arrays and variables
  char input[BUFSIZ]; 
  char last_command[BUFSIZ];
  bool ampersand = false;
  char history[10][15];

  // Initialize history array to 1 so when printing,
  // only the commands will be printed
  for (int k = 0; k < 10; k++){
    strcpy(history[k], "1");
  }

  // Array to parse through !N command to get the  N integer
  char temp[10][15];
  
  memset(input, 0, BUFSIZ * sizeof(char));
  memset(input, 0, BUFSIZ * sizeof(char));
  bool finished = false;
  int i = 0;

  while (!finished) {
    fflush(stdout);
    printf("osh> ");
 
    if ((fgets(input, BUFSIZ, stdin)) == NULL) {   // or gets(input, BUFSIZ);
      fprintf(stderr, "no command entered\n");
      exit(1);
    }
    input[strlen(input) - 1] = '\0';          // wipe out newline at end of string

    // check for history (!!) command
    if (strncmp(input, "!!", 2) == 0) {
      if (strlen(last_command) == 0) {
        fprintf(stderr, "no last command to execute\n");
      }
      // Run the last command
      Parse_Fork(last_command);

      // If user types "exit", user will exit the shell
    } else if (strncmp(input, "exit", 4) == 0) {   // only compare first 4 letters
      finished = true;

      // If use enters a single ! followed by an integer N, the Nth
      // command in the history is executed
    } else if (strncmp(input, "!", 1) == 0){
      // Parse through !N command to get the N integer
      static char* temp[5];
      memset(temp, 0, sizeof(temp));
      char break_chars[] = {"!"};
      char* p = strtok(input, break_chars);
      temp[0] = p;   
      int i = 0;
      while (p != NULL) {
      p = strtok(NULL, break_chars);
      temp[++i] = p;
      int num = atoi(temp[0]);
      
      // Run the command from history that the user has chosen, from 1 to 10
      Parse_Fork((history[num-1]));
      }

      // Prints a history of the user's commands, up to 10,
      // from most recent to least recent
    } else if (strncmp(input, "history", 7) == 0){
      for (int j = 9; j > -1; j--){

        // Print out only the commands
        if (strncmp(history[j], "1", 1) != 0)
        printf("%d %s\n",j+1, history[j]);
    }
      
      // User enters a UNIX command
    } else {
      strcpy(last_command, input);
      strcpy(temp, input);

      // Run the command
      Parse_Fork(input);
      
      // Add command to the history
      strcpy(history[i], temp);
      i++;
    }
    
  }
    
  printf("osh exited\n");
  printf("program finished\n");
  
  return 0;
}

/* OUTPUT

osc@ubuntu:~/final-src-osc10e/ch4$ ./UNIX_Shell -c
osh> ls
osh> Driver.java  fig4-23  fig4-23.c  openmp  openmp.c  SumTask.java  thrd-posix  thrd-posix.c  thrd-win32.c  ThreadPoolExample.java  UNIX_Shell  UNIX_Shell2  UNIX_Shell2.c  UNIX_Shell.c
cal 2020
osh>                             2020
      January               February               March          
Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  
          1  2  3  4                     1   1  2  3  4  5  6  7  
 5  6  7  8  9 10 11   2  3  4  5  6  7  8   8  9 10 11 12 13 14  
12 13 14 15 16 17 18   9 10 11 12 13 14 15  15 16 17 18 19 20 21  
19 20 21 22 23 24 25  16 17 18 19 20 21 22  22 23 24 25 26 27 28  
26 27 28 29 30 31     23 24 25 26 27 28 29  29 30 31              
                                                                  

       April                  May                   June          
Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  
          1  2  3  4                  1  2      1  2  3  4  5  6  
 5  6  7  8  9 10 11   3  4  5  6  7  8  9   7  8  9 10 11 12 13  
12 13 14 15 16 17 18  10 11 12 13 14 15 16  14 15 16 17 18 19 20  
19 20 21 22 23 24 25  17 18 19 20 21 22 23  21 22 23 24 25 26 27  
26 27 28 29 30        24 25 26 27 28 29 30  28 29 30              
                      31                                          

        July                 August              September        
Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  
          1  2  3  4                     1         1  2  3  4  5  
 5  6  7  8  9 10 11   2  3  4  5  6  7  8   6  7  8  9 10 11 12  
12 13 14 15 16 17 18   9 10 11 12 13 14 15  13 14 15 16 17 18 19  
19 20 21 22 23 24 25  16 17 18 19 20 21 22  20 21 22 23 24 25 26  
26 27 28 29 30 31     23 24 25 26 27 28 29  27 28 29 30           
                      30 31                                       

      October               November              December        
Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  
             1  2  3   1  2  3  4  5  6  7         1  2  3  4  5  
 4  5  6  7  8  9 10   8  9 10 11 12 13 14   6  7  8  9 10 11 12  
11 12 13 14 15 16 17  15 16 17 18 19 20 21  13 14 15 16 17 18 19  
18 19 20 21 22 23 24  22 23 24 25 26 27 28  20 21 22 23 24 25 26  
25 26 27 28 29 30 31  29 30                 27 28 29 30 31        
                                                                  
ls -l
osh> total 112
-rw-rw-r-- 1 osc osc  1046 Jun 19  2018 Driver.java
-rwxrwxr-x 1 osc osc 11024 Sep 21 15:38 fig4-23
-rw-rw-r-- 1 osc osc  1038 Sep 14 15:54 fig4-23.c
-rwxrwxr-x 1 osc osc  9912 Sep 16 16:03 openmp
-rw-rw-r-- 1 osc osc   549 Sep 16 16:05 openmp.c
-rw-rw-r-- 1 osc osc  1639 Jun 19  2018 SumTask.java
-rwxrwxr-x 1 osc osc 11256 Sep 14 16:16 thrd-posix
-rw-rw-r-- 1 osc osc  1420 Sep 28 20:50 thrd-posix.c
-rw-rw-r-- 1 osc osc  1067 Sep 23 16:19 thrd-win32.c
-rw-rw-r-- 1 osc osc   845 Jun 19  2018 ThreadPoolExample.java
-rwxrwxr-x 1 osc osc 16544 Oct  3 17:13 UNIX_Shell
-rwxrwxr-x 1 osc osc 16496 Oct  3 16:15 UNIX_Shell2
-rw-rw-r-- 1 osc osc  3808 Oct  3 16:04 UNIX_Shell2.c
-rw-rw-r-- 1 osc osc  3836 Oct  3 17:07 UNIX_Shell.c
!!
osh> total 112
-rw-rw-r-- 1 osc osc  1046 Jun 19  2018 Driver.java
-rwxrwxr-x 1 osc osc 11024 Sep 21 15:38 fig4-23
-rw-rw-r-- 1 osc osc  1038 Sep 14 15:54 fig4-23.c
-rwxrwxr-x 1 osc osc  9912 Sep 16 16:03 openmp
-rw-rw-r-- 1 osc osc   549 Sep 16 16:05 openmp.c
-rw-rw-r-- 1 osc osc  1639 Jun 19  2018 SumTask.java
-rwxrwxr-x 1 osc osc 11256 Sep 14 16:16 thrd-posix
-rw-rw-r-- 1 osc osc  1420 Sep 28 20:50 thrd-posix.c
-rw-rw-r-- 1 osc osc  1067 Sep 23 16:19 thrd-win32.c
-rw-rw-r-- 1 osc osc   845 Jun 19  2018 ThreadPoolExample.java
-rwxrwxr-x 1 osc osc 16544 Oct  3 17:13 UNIX_Shell
-rwxrwxr-x 1 osc osc 16496 Oct  3 16:15 UNIX_Shell2
-rw-rw-r-- 1 osc osc  3808 Oct  3 16:04 UNIX_Shell2.c
-rw-rw-r-- 1 osc osc  3836 Oct  3 17:07 UNIX_Shell.c
history
3 ls -l
2 cal 2020
1 ls
osh> who
osh> osc      tty1         2021-08-28 12:28
date
osh> Sun Oct  3 17:14:41 MDT 2021
history
5 date
4 who
3 ls -l
2 cal 2020
1 ls
osh> !1
osh> Driver.java  fig4-23  fig4-23.c  openmp  openmp.c  SumTask.java  thrd-posix  thrd-posix.c  thrd-win32.c  ThreadPoolExample.java  UNIX_Shell  UNIX_Shell2  UNIX_Shell2.c  UNIX_Shell.c
!2     
osh>                             2020
      January               February               March          
Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  
          1  2  3  4                     1   1  2  3  4  5  6  7  
 5  6  7  8  9 10 11   2  3  4  5  6  7  8   8  9 10 11 12 13 14  
12 13 14 15 16 17 18   9 10 11 12 13 14 15  15 16 17 18 19 20 21  
19 20 21 22 23 24 25  16 17 18 19 20 21 22  22 23 24 25 26 27 28  
26 27 28 29 30 31     23 24 25 26 27 28 29  29 30 31              
                                                                  

       April                  May                   June          
Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  
          1  2  3  4                  1  2      1  2  3  4  5  6  
 5  6  7  8  9 10 11   3  4  5  6  7  8  9   7  8  9 10 11 12 13  
12 13 14 15 16 17 18  10 11 12 13 14 15 16  14 15 16 17 18 19 20  
19 20 21 22 23 24 25  17 18 19 20 21 22 23  21 22 23 24 25 26 27  
26 27 28 29 30        24 25 26 27 28 29 30  28 29 30              
                      31                                          

        July                 August              September        
Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  
          1  2  3  4                     1         1  2  3  4  5  
 5  6  7  8  9 10 11   2  3  4  5  6  7  8   6  7  8  9 10 11 12  
12 13 14 15 16 17 18   9 10 11 12 13 14 15  13 14 15 16 17 18 19  
19 20 21 22 23 24 25  16 17 18 19 20 21 22  20 21 22 23 24 25 26  
26 27 28 29 30 31     23 24 25 26 27 28 29  27 28 29 30           
                      30 31                                       

      October               November              December        
Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  
             1  2  3   1  2  3  4  5  6  7         1  2  3  4  5  
 4  5  6  7  8  9 10   8  9 10 11 12 13 14   6  7  8  9 10 11 12  
11 12 13 14 15 16 17  15 16 17 18 19 20 21  13 14 15 16 17 18 19  
18 19 20 21 22 23 24  22 23 24 25 26 27 28  20 21 22 23 24 25 26  
25 26 27 28 29 30 31  29 30                 27 28 29 30 31        
                                                                  
ps
osh>   PID TTY          TIME CMD
18524 pts/94   00:00:00 bash
18561 pts/94   00:00:00 UNIX_Shell
18572 pts/94   00:00:00 ls <defunct>
18578 pts/94   00:00:00 cal <defunct>
18625 pts/94   00:00:00 ls <defunct>
18650 pts/94   00:00:00 ls <defunct>
18678 pts/94   00:00:00 who <defunct>
18742 pts/94   00:00:00 date <defunct>
18875 pts/94   00:00:00 ls <defunct>
18950 pts/94   00:00:00 cal <defunct>
19088 pts/94   00:00:00 ps
history
6 ps
5 date
4 who
3 ls -l
2 cal
1 ls
osh> !5
osh> Sun Oct  3 17:15:35 MDT 2021
ls              
osh> Driver.java  fig4-23  fig4-23.c  openmp  openmp.c  SumTask.java  thrd-posix  thrd-posix.c  thrd-win32.c  ThreadPoolExample.java  UNIX_Shell  UNIX_Shell2  UNIX_Shell2.c  UNIX_Shell.c
cat Driver.java &
Waiting on child...
osh> 
 * Summation program using exectuors/callable/futures
 *
 * Figure 4.14
 *
 * @author Gagne, Galvin, Silberschatz
 * Operating System Concepts  - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 
 
import java.util.concurrent.*;

class Summation implements Callable<Integer>
{
    private int upper;

    public Summation(int upper) {
        this.upper = upper;
    }

    public Integer call() {
        int sum = 0;
        for (int i = 1; i <= upper; i++)
            sum += i;
    
        return new Integer(sum);
    }
}


public class Driver
{
    public static void main(String[] args) {
        if (args.length == 1) {
            int upper = Integer.parseInt(args[0]);

            ExecutorService pool = Executors.newSingleThreadExecutor();
            Future<Integer> result = pool.submit(new Summation(upper));

            try {
                System.out.println("sum = " + result.get());
            }
            catch (InterruptedException | ExecutionException ie) { }

            pool.shutdown();
        }
    }
}
!!     
Waiting on child...
osh> 
 * Summation program using exectuors/callable/futures
 *
 * Figure 4.14
 *
 * @author Gagne, Galvin, Silberschatz
 * Operating System Concepts  - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 
 
import java.util.concurrent.*;

class Summation implements Callable<Integer>
{
    private int upper;

    public Summation(int upper) {
        this.upper = upper;
    }

    public Integer call() {
        int sum = 0;
        for (int i = 1; i <= upper; i++)
            sum += i;
    
        return new Integer(sum);
    }
}


public class Driver
{
    public static void main(String[] args) {
        if (args.length == 1) {
            int upper = Integer.parseInt(args[0]);

            ExecutorService pool = Executors.newSingleThreadExecutor();
            Future<Integer> result = pool.submit(new Summation(upper));

            try {
                System.out.println("sum = " + result.get());
            }
            catch (InterruptedException | ExecutionException ie) { }

            pool.shutdown();
        }
    }
}
exit
osh exited
program finished
osc@ubuntu:~/final-src-osc10e/ch4$ 

*/