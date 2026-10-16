#ifndef SHELL_H
#define SHELL_H
/*
void lancer_shell()                        → boucle principale
void executer_cmd(char **args)             → fork + execvp
void executer_pipe(char **cmd1, char **cmd2) → pipe + deux fils
void cmd_status()                          → lit /proc
void cmd_cd(char *chemin)                  → chdir
void parser(char *input, char **args)      → strtok
*/

typedef struct timefull{
    int heure;
    int min;
    int sec;
}timefull;

//int status = 1;
void lancer_shell();//boucle principale                 
void executer_cmd(char **args);//fork + execvp            
void executer_pipe(char **cmd1, char **cmd2);//pipe + deux fils 
void cmd_status();//lit /proc                         
void cmd_cd(char *chemin);//chdir                  
void parser(char *input, char **args,int taille);//strtok 
timefull calculer_float_en_temps(float f);

#endif

/*
Fichiers séparés : main.c, shell.c, shell.h
Ctrl+C → affiche "\nmysh$ " et continue
cd     → chdir(), gère erreur si chemin invalide
status → lit /proc et affiche CPU, RAM, uptime, loadavg
pipe   → deux commandes maximum séparées par |
*/