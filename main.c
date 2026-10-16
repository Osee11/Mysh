#define _GNU_SOURCE
#include "shell.h"
#include <signal.h>
#include <stdio.h>

void handle(){
    printf("\n");
}
int main(){
    //cmd_status();
    struct sigaction sa;
    sa.sa_handler = handle;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    lancer_shell();
}