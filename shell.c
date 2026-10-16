#include <stdio.h>
#include <unistd.h>
#include "shell.h"
#include <string.h>
#include <wait.h>

timefull calculer_float_en_temps(float f){
    timefull t;
    t.heure = f / 3600;
    t.min = ((int)f % 3600)/60;
    t.sec = ((int)f % 60);
    return t;
} 

void lancer_shell(){
    while(1){
        char input[100] = {0};
        printf("mysh$ ");
        fflush(stdout);
        fgets(input,sizeof(input),stdin);
        //while(getchar() != '\n');
        input[strcspn(input,"\n")] = '\0';
        char *arg[20];
        parser(input,arg,20);
        if(arg[0] == NULL)continue;
        if(strcmp("exit",arg[0]) == 0){
            break;
        }else if(strcmp("status",arg[0]) == 0){
            cmd_status();
            continue;
        }
        
        if(strcmp(arg[0], "cd") == 0) {
            if(arg[1] != NULL) cmd_cd(arg[1]);
            continue;
        }else{
            int pipe_pos = -1;;//deviens 1 lors de la tournee de la boucle si c'est une cmd pipe si non reste a zero
            for(int i = 0;i < 20 && arg[i] != NULL;i++){
                if(strcmp("|",arg[i]) == 0){//voir si il y a le symbole de pipe
                    if(i == 0){//verifier si le pipe n'est pas en premiere position,doit etre entre deux element
                        pipe_pos = -2;
                        break;
                    }
                    pipe_pos = i;
                    break;
                }
            }
            if(pipe_pos == -2)continue;
            if(arg[pipe_pos+1]==NULL){
                printf("Commande manquante après |\n");
                continue;
            }
            if(pipe_pos != -1){
                char *cmd1[20] = {0};
                char *cmd2[20] = {0};
                int i;
                for( i = 0;i < pipe_pos;i++){
                    cmd1[i] = arg[i];
                }
                cmd1[i] = NULL;
                int j = 0;
                int o;
                for(o = pipe_pos+1;arg[o] != NULL;o++){
                    //printf("%s\n",arg[o]);
                    cmd2[j++] = arg[o];
                }
                cmd2[j] = NULL;
                
                executer_pipe(cmd1,cmd2);
            }else{
                executer_cmd(arg);
            }
        }

    }
}                        
void executer_cmd(char **args){
    int status = 1;
    if(args[0] == NULL){
        return;
    }
    if(strcmp("exit",args[0]) == 0){
        return;
    }
    int pid = fork();
    if(pid == 0){
        execvp(args[0],args);
        perror(args[0]);
        _exit(status);
    }else if(pid >0){
        wait(&status);
    }
    

}           
void executer_pipe(char **cmd1, char **cmd2){
    int status = 1;
    int fd[2];
    if(pipe(fd) < 0){
        printf("Une erreur s'est produite code |\n");
        return;
    }
    int pid1 = fork();
    if(pid1 == 0){
        close(fd[0]);
        dup2(fd[1],1);
        close(fd[1]);
        execvp(cmd1[0],cmd1);
        perror(cmd1[0]);
        _exit(status);
    }
    int pid2 = fork();
    if(pid2 == 0){
        close(fd[1]);
        dup2(fd[0],0);
        close(fd[0]);
        execvp(cmd2[0],cmd2);
        perror(cmd2[0]);
        _exit(status);
    }
    close(fd[0]);
    close(fd[1]);
    wait(NULL);
    wait(NULL);
    
} 
void cmd_status(){
    //loadavg(1,5,15)min,uptime(temps_ecoulee_syst_actif,temps_ecoulee_syst_inactif)secondes(en float),
    //cpuinfo(model name,cpu MHz)
    char ligne[2048] = {0};
    char retour[200] = {0};
    FILE *loadavg = fopen("/proc/loadavg","r");
    if(loadavg == NULL){
        perror("Erreur loadavg");
        return;
    }
    float s1,s2,s3;
    fscanf(loadavg,"%f %f %f",&s1,&s2,&s3);
    snprintf(retour,sizeof(retour)-strlen(retour),"==LOADAVG==\nEtat de travail du cpu il y a:\n1 min: %.2f\n5 min: %.2f\n15 min: %.2f\n",s1,s2,s3);
    strncat(ligne,retour,sizeof(ligne) - strlen(ligne)-1);
    //printf("taille %zu\n",taille);
    fclose(loadavg);

    FILE *uptim = fopen("/proc/uptime","r");
    if(uptim == NULL){
        perror("Erreur uptime");
        return;
    }
    float sys_act,sys_inact;
    fscanf(uptim,"%f %f",&sys_act,&sys_inact);
    timefull f1 = calculer_float_en_temps(sys_act);
    timefull f2 = calculer_float_en_temps(sys_inact);
    snprintf(retour,sizeof(retour)-strlen(retour),"\n==UPTIME==\nTemps passé avec systeme actif: %dh:%dmin:%ds\nTemps passé avec systeme inactif: %dh:%dmin:%ds\n",f1.heure,f1.min,f1.sec,f2.heure,f2.min,f2.sec);

    strncat(ligne,retour,(sizeof(ligne)-strlen(ligne)-1));
    //printf("Taille: %zu\n",sizeof(ligne)-strlen(ligne));
    fclose(uptim);
    FILE *cpuinfo = fopen("/proc/cpuinfo","r");
    if(cpuinfo == NULL){
        perror("Erreur cpu info");
        return;
    }
    int i = 0;
    char retour2[200] = {0};
    char retour3[200] = {0};
    memset(retour,0,sizeof(retour));
    strncat(ligne,"\n==CPU INFO==\n",sizeof(ligne)-strlen(ligne)-1);
    while(fgets(retour,sizeof(retour),cpuinfo) != NULL){
        if(strncmp(retour,"model name",10) == 0){
            char *str = strstr(retour,":");
            snprintf(retour2,sizeof(retour2),"Model name: %s\n",str+1);
            i++;
        }
        if(strncmp(retour,"cpu MHz",7) == 0){
            char *str = strstr(retour,":");
            snprintf(retour3,sizeof(retour3),"Cpu MHz: %s\n",str+1);
            i++;
        }
        if(i == 2)break;
    }
    strncat(ligne,retour2,sizeof(ligne)-strlen(ligne)-1);
    strncat(ligne,retour3,sizeof(ligne)-strlen(ligne)-1);
    fclose(cpuinfo);
    
    FILE *meminfo = fopen("/proc/meminfo","r");
    if(meminfo == NULL){
        printf("Erreur meminfo\n");
        return;
    }
    strncat(ligne,"\n==MEMINFO==\n",(sizeof(ligne)-strlen(ligne))-1);
    while(fgets(retour,sizeof(retour),meminfo) != NULL){
        if(strstr(retour,"MemTotal") ||strstr(retour,"MemAvailable") || strstr(retour,"MemFree")){
            //printf("%s\n",ligne);
            strncat(ligne,retour,(sizeof(ligne)-strlen(ligne))-1);
        }
    }
    fclose(meminfo);
    printf("%s\n",ligne);
    return;
}                         
void cmd_cd(char *chemin){
    if(chdir(chemin) != 0) {
    perror("cd");
}
}                  
void parser(char *input, char **args,int taille){
    int i = 0;
    if(input == NULL)return;
    char *toke = strtok(input," ");
    while(toke != NULL && i < taille-1){
        args[i++] = toke;
        toke = strtok(NULL," ");
    }
    args[i] = NULL;
}