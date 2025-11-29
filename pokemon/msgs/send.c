// EJEMPLO DE ENVIO DE MENSAJES ENTRE PROCESOS, UTILIZANDO PRIMITIVAS
// DE COMUNICACION

// CURSO SISTEMAS OPERATIVOS 
// DEPTO. INGENIERIA EN COMPUTACION
// UNIVERSIDAD DE MAGALLANES
// PROFESOR: EDUARDO PEÑA J.

// Compilar gcc send.c -o send
// Ejecutar ./send

//Linux kataix 2.6.18-mmkernel #1 SMP Wed Jun 25 12:19:28 CLT 2008 i686 GNU/Linux
//Debian GNU/Linux 5.0 \n \l
//Linux version 2.6.18-mmkernel (2.6.18-mmkernel-10.00.Custom) (root@kataix)
//gcc version 4.1.2 20061115 (prerelease) (Debian 4.1.1-21)) #1 SMP Wed Jun 25 12:19:28 CLT 2008

//ENVIA MENSAJES DE LARGO DE 20 CARACTERES A TRAVES DE DE LA COLA 234
//CUANDO SE ENVIA EL MENSAJE "FIN" AL PROCESO RECIBE, ESTE CULMINA LA RECEPCION

//Recordar:  ipcs: lee Nos de colas y semaforos utilizados por el usuario que ejecuto este codigo
//           ipcrm -q <No cola leido de ipcs>: Elimina la colo indicada por el usuario
//           ps:   lee procesos activos por el usuario
//           kill <No Procso leido con pS>: Elimina el proceso indicado por usuario


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <time.h>
#include <math.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h> 
#include <sys/wait.h> 


#define clave 234  /* Numero de cola */
#define MAX 64

#define DATA_MAX 100

char gen_data();
double delay();
char *msg(const char *file);
void run_sender(const char *rol, const char *archivo, long tipo, int msqid, int longitud, int *shared_data);

struct {
    long tipo;
    char cadena[MAX];
}mensaje;

int main()
{
    srand(time(0));   
    int msqid;			/* identificador de la cola de mensajes */
    int shmid;
    int *shared_data;
    int created_shm = 0;

    key_t key = 1234;
    int longitud = sizeof(mensaje) - sizeof(mensaje.tipo);

    if ((shmid = shmget(key, sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) < 0) 
    { 
        if (errno != EEXIST)
        {
            perror("Error al crear la memoria compartida");
            exit(1); 
        }

        shmid = shmget(key, sizeof(int), 0666);
        if (shmid < 0)
        {
            perror("Error al obtener la memoria compartida existente");
            exit(1);
        }
    }
    else
    {
        created_shm = 1;
    }

    shared_data = shmat(shmid, NULL, 0);
    if (shared_data == (void *)-1) 
    { 
        perror("Error al adjuntar la memoria compartida"); 
        exit(1); 
    } 

    if (created_shm)
    {
        *shared_data = 0;
    }

    if((msqid = msgget(clave, IPC_CREAT | 0600)) == -1)
    {
        perror("Error al crear la cola de mensajes");
        exit(-1);
    }

    pid_t pid1 = fork();
    if (pid1 < 0)
    {
        perror("Error al crear al Padre");
        exit(1);
    }
    else if (pid1 == 0)
    {
        printf("Proceso Padre (PID: %d) en ejecución...\n", getpid());
        run_sender("Padre", "Padre.txt", 2, msqid, longitud, shared_data);
        shmdt(shared_data);
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 < 0)
    {
        perror("Error al crear al Hijo");
        exit(1);
    }
    else if (pid2 == 0)
    {
        printf("Proceso Hijo (PID: %d) en ejecución...\n", getpid());
        run_sender("Hijo", "Hijo.txt", 3, msqid, longitud, shared_data);
        shmdt(shared_data);
        exit(0);
    }

    printf("Proceso Abuelo (PID: %d) en ejecución...\n", getpid());
    run_sender("Abuelo", "Abuelo.txt", 1, msqid, longitud, shared_data);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    shmdt(shared_data);

    return 0;
}

double delay()
{
    double miliseg = 0, segs = 0;
    clock_t tiempo_inicio, tiempo_final;

    segs = (rand() % 501) + 500;

    miliseg = 1000 * segs;

    tiempo_inicio = clock();
    while (clock() < tiempo_inicio + miliseg);
    tiempo_final = clock();
    tiempo_final -= tiempo_inicio;

    return (double) tiempo_final/CLOCKS_PER_SEC;
}
char gen_data(){
    return (char)(rand() % 26) + 65;
}
char *msg(const char *File){
    FILE *MSG = fopen(File, "r");
    if(!MSG){
        printf("Error: no existe el archivo %s\n", File);
        return NULL;
    }

    size_t linescount = 0;
	size_t charcount = 0;
	size_t WMAX = 0;
    int t;

    while ((t = fgetc(MSG)) != EOF)
	{
        if (t == '\n')
		{
			if(charcount > WMAX){
				WMAX = charcount;
			}
            linescount++;
			charcount = 0;
		}
		else{
			charcount++;
        }
    }

    if(charcount > 0){
        if(charcount > WMAX){
            WMAX = charcount;
        }
        linescount++;
    }

	if(linescount == 0){
        fclose(MSG);
        return NULL;
    }

    rewind(MSG);
	//printf("Total lineas: %zu, Largo maximo total: %zu\n", linescount, WMAX);

    size_t buffer_len = WMAX + 2;
    char *line = malloc(buffer_len);
    if(!line){
        fclose(MSG);
        return NULL;
    }

    size_t target = rand() % linescount;
    size_t current_line = 0;
    while(fgets(line, buffer_len, MSG)){
        if(current_line == target){
            line[strcspn(line, "\r\n")] = '\0';
            fclose(MSG);
            return line;
        }
        current_line++;
    }

    fclose(MSG);
    free(line);
    return NULL;
}

void run_sender(const char *rol, const char *archivo, long tipo, int msqid, int longitud, int *shared_data)
{
    while (*shared_data < DATA_MAX)
    {
        char *linea = msg(archivo);
        if (!linea)
        {
            fprintf(stderr, "No se pudo leer una línea desde %s\n", archivo);
            break;
        }

        mensaje.tipo = tipo;
        snprintf(mensaje.cadena, MAX, "%s: %s", rol, linea);
        free(linea);

        if(msgsnd(msqid, &mensaje, longitud, 0) == -1)
        {
            perror("Error al enviar un mensaje a la cola de mensajes");
            break;
        }

        (*shared_data)++;
        printf("[%s - PID %d] Mensaje enviado: %s (contador compartido: %d)\n", rol, getpid(), mensaje.cadena, *shared_data);
        delay();
    }

    printf("[%s - PID %d] Deteniendo envíos. shared_data=%d\n", rol, getpid(), *shared_data);
}
