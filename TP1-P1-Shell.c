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

#define PROMPT_SIMBOL "#"
#define EXIT_COMMAND "salir"
#define ZOMBIE_CHECK_INTERVAL 5
#define DEBUG 0

void zombie_guard(int);

// Stores the trimmed input string into the given output buffer, which must be
// large enough to store the result.  If it is too small, the output is
// truncated.
size_t trimwhitespace(char *out, size_t len, const char *str)
{
    if(len == 0)
        return 0;

    const char *end;
    size_t out_size;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
    {
        *out = 0;
        return 1;
    }

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end++;

    // Set output size to minimum of trimmed string length and buffer size minus 1
    out_size = (end - str) < len-1 ? (end - str) : len-1;

    // Copy trimmed string and add null terminator
    memcpy(out, str, out_size);
    out[out_size] = 0;

    return out_size;
}

int main(int argc, char **argv) {
    pid_t pid;
    signal(SIGALRM, zombie_guard);
    alarm(ZOMBIE_CHECK_INTERVAL);
    
    char comando[256];
    int  status;

    char *p = NULL;
    int   esback = 0;
    
    do {
        // muestro prompt del shell
        printf(PROMPT_SIMBOL);
        
        // ingreso comando
        fgets(comando, 256, stdin);
        
        // quito enter final
        comando[strlen(comando)-1] = '\0';
        
        //printf("comando: [%s]\n",comando);
        if ( strlen(comando) == 0 ) 
            continue;
        
        if ( strncmp(comando, "salir", 5) != 0 ) {
            p = strstr(comando, "&");
            esback = p != NULL;
            
            if ( esback ) *p = '\0';
            
            // creo proceso!
            pid = fork();
            if ( pid > 0 ) {

                // codigo del shell (padre)
                if ( !esback ) {
                    if (DEBUG)
                        printf("proceso foreground %d creado! espero!\n", pid);
                    
                    // espero por el proceso hijo
                    waitpid(pid, &status, 0);

                    if ( DEBUG && status != 0 ) 
                        printf("comando [%s] finalizado, st=%d\n", comando, status / 256);

                } else {
                    if (DEBUG) 
                        printf("proceso background %d creado!\n",pid);
                }
            } else { 
                // codigo del hijo, nuevo proceso creado
                // cambio ese codigo por el comando que solicito el usuario
                if ( execlp(comando, comando, NULL) == -1) {

                    printf("Comando [%s] no reconocido!\n", comando);
                    exit(EXIT_SUCCESS);
                }
            }
        }

   } while( strncmp(comando, "salir", 5) != 0);
   
   exit(EXIT_SUCCESS);
}

void zombie_guard(int signo) {
    int status = 0;

    pid_t pid = waitpid(-1, &status, WNOHANG);

    while( (pid != -1) && (pid != 0) ){
        if (DEBUG)
            printf("proceso background [%d] finalizado st=%d!\n",pid,status/256);

        pid = waitpid(-1, &status, WNOHANG);
    }

    alarm(ZOMBIE_CHECK_INTERVAL);
}
