/*****************************************************************
* Author: Vishal Vishnani
* Date: 9/6/2017
* File: EXEC.c
* Description: Source file to be executed by new process in exec
*****************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdlib.h>
#include <time.h>

//will be executed by child process
int main(int argc,char *argv[]){
  printf("\nIn child process\n");
}
