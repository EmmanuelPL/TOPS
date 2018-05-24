#include "Tools.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>


/*
pid_t fork(void);
pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);

int execl(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execle(const char *path, const char *arg , ..., char * const envp[]);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);

hijos con waitpid()
*/

#define MAX_TOKEN_LENGTH 512
#define PROMPT_SIMBOL "#"
#define EXIT_COMMAND "salir"
#define DEBUG 0

void death_handler(int);


int main(int argc, char **argv) {
    pid_t pid;
    signal(SIGCHLD, death_handler);
    
    size_t len = 0;
    char *comando_raw;
    char *comando;


    int  status;

    char *p = NULL;
    int  esback = 0;
    
    do {
        // muestro prompt del shell
        printf(PROMPT_SIMBOL);
        

        // libero espacio
        if (len > 0) {
            free(comando_raw);
            free(comando);

            len = 0;
        }

        // ingreso comando_raw
        len = fetchline(&comando_raw);
        
        // reservo espacio
        comando = malloc(len);

        // Saco los espacios
        trimwhitespace(comando, len, comando_raw);

        if ( strlen(comando) == 0 ) 
            continue;
        
        if ( strncmp(comando, EXIT_COMMAND, 5) != 0 ) {
            // compruevo si esta el &
            p = strstr(comando_raw, "&");
            esback = p != NULL;

            // Saco el &            
            if ( esback ) *p = '\0';

            // Saco los nuevos espacios
            trimwhitespace(comando, len, comando_raw);
            
            // creo proceso!
            pid = fork();
            if ( pid > 0 ) 
            {
                // codigo del shell (padre)
                if ( !esback ) {
                    if (DEBUG)
                        printf("proceso foreground %d creado! espero!\n", pid);
                    
                    // espero por el proceso hijo
                    waitpid(pid, &status, 0);

                    if ( DEBUG && status != 0 ) 
                        printf("comando [%s] finalizado, st=%d\n", *comando, status / 256);

                } 
                else 
                {
                    if (DEBUG) 
                        printf("proceso background %d creado!\n",pid);
                }
            } 
            else 
            { 
                // codigo del hijo, nuevo proceso creado
                // cambio ese codigo por el comando que solicito el usuario
                if ( execlp(comando, comando, NULL) == -1) 
                {

                    printf("Comando [%s] no reconocido!\n", comando);
                    exit(EXIT_SUCCESS);
                }
            }
        }

   } while( strncmp(comando, "salir", 5) != 0);
   
   exit(EXIT_SUCCESS);
}

void death_handler(int signo) 
{
    int status = 0;

    pid_t pid = waitpid(-1, &status, WNOHANG);

    while( (pid != -1) && (pid != 0) )
    {
        if (DEBUG)
            printf("proceso background [%d] finalizado st=%d!\n", pid, status / 256);

        pid = waitpid(-1, &status, WNOHANG);
    }
}
