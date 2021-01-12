/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <signal.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int fifo(int process);
int roundrobin(int process, int quantum);
int sjf(int process);
int processId = 0;
int processEnd = 0;
int processTime = 0;
int processDiff = 0;
int processTotal = 0;
int quantum = 0;
void sig_handler(int signum);
void call_process(int pt, int pId, int q);
pid_t pid; 
int *tempPriorities;

int main(int argc, char *argv[])
{
    int alg = 0;
    if(argc < 2){
        printf("Falta argumentos %d\n",argc);
        return 0;
    }
  

    if( strcmp("-F", argv[1]) == 0 || strcmp("-R", argv[1]) == 0 || strcmp("-P", argv[1]) == 0){
        if(strcmp("-F", argv[1])== 0){
            alg = 1;
            printf("Algoritmo de FIFO\n");
        }
        else
            if(strcmp("-R", argv[1])== 0){
                alg = 2;
                printf("Algoritmo de RoundRobin\n");
            }
            else
                if(strcmp("-P", argv[1])== 0){
                    alg = 3;
                    printf("Algoritmo de Prioridades\n");
                }
        
    }else{
         printf("Primer argumento debe ser: -F = FIFO o -R = RoundRobin -P = Prioridades\n");
         return 0;
    }

    int process = atoi(argv[2]);
    if(process == 0){
        printf("Segundo argumento debe ser un numero que son la cantidad de procesos procesos\n");
    }
    else
        printf("Numero de procesos %d\n",process);
        
       
    if(alg == 2){
        if(argc < 5 ){
            printf("Se requiere parametro -Q = Quantum\n");
            return 0;
        }else{
            quantum =  atoi(argv[4]);
            if( quantum < 1){
                printf("Quantum debe ser un numero positivo\n");
                return 0;
            }
            else
                printf("Quantum %d\n",quantum);                
        }
    }
    
    if(alg == 3){
        
        if(argc < process + 4){
            int esperado = process + 4;
            printf("Faltan prioridades %d se esperaban %d\n",argc, esperado);
            return 0;

        }
        else{
            tempPriorities = malloc(sizeof(int)*process);  
            int pos = 0;
            for (int i = 4; i < argc; i++) {
                tempPriorities[pos] = atoi(argv[i]);
                printf("Argumento %d is: %s\n", i, argv[i]);
                pos++;
            }
           
        }
    }
    
    if(alg == 1)
        fifo(process);
    if(alg == 2)
        roundrobin(process,quantum);
    if(alg == 3)
        sjf(process);
    free(tempPriorities);
    return 0;
}

int roundrobin(int process, int quantum){
    printf("Ejecutando Round Robin\n");
	int np=process, procesos[process][2], nq = 0;
	double tp = 0;// tiempo promedio.
	int terminado = 0;

	for(int i=0; i<np; i++) {
		printf("\nIngresa el valor del proceso %d :", i+1);
		scanf("%d", &procesos[i][0]);
		procesos[i][1] = procesos[i][0];
	}
	// Algoritmo RR
	processDiff = 0;
	while(terminado == 0) {
		terminado = 1;// Sí terminado
		for(int i=0; i<np; i++) {
			if(procesos[i][0] > 0) {
		        processDiff =  procesos[i][0];
				procesos[i][0] -= quantum;
				
				nq++;
				printf("\nQuantum[%d] P%d",nq, i+1);
        		call_process(quantum, i+1, procesos[i][1]); //se configura el proceso
        		printf("\nRegistrando P%d", i + 1);
        		signal(SIGUSR1,sig_handler);  //se registra la señal de la funcion del proceso
        		raise(SIGUSR1); //se manda la señal para que comience a trabajar
        
        		while(processEnd == 0);	//se espera que el proceso termine				
				if (procesos[i][0]>0) {
					terminado = 0; //No terminado
					printf("\nTermina su tiempo en proceso P%d", i+1);
				} else {
					tp += nq*quantum;
					printf("\nTermina el proceso P%d", i+1);
				}
			}
		}
	}
	tp = tp / np;
	printf("\nTiempo promedio de RoundRobin %f:", tp);
	return 0;
} 

int fifo(int process){
    printf("Ejecutando : FIFO\n");
	int np=process, procesos[process];
	double tf = 0, tp;// tiempo promedio.

	for(int i=0; i<np; i++) {
		printf("\nIngresa el valor del proceso %d :", i+1);
		scanf("%d", &procesos[i]);
	}
	// Algoritmo FCFS
	for(int i=0; i<np; i++) {
		tf += procesos[i];
		tp = tp + tf;
		call_process(procesos[i], i+1,0); //se configura el proceso
		printf("\nRegistrando P%d", i + 1);
		signal(SIGUSR1,sig_handler);//se registra la señal de la funcion del proceso
		raise(SIGUSR1); //se manda la señal para que comience a trabajar

		while(processEnd == 0); //se espera que el proceso termine
		printf("\nTermina P%d, en %2.1f", i+1, tf);
	}
	printf("\n-------------------------------");
	printf("\nLa suma de los procesos %2.1f", tp);
	tp = tp / np;
	printf("\n\nTiempo promedio en FIFO fue de: %2.2f:", tp);

}


int sjf(int process) {
	printf("PRIORIDAD SJF\n");

	int np=process, procesos[process][2];
	double tf = 0, tp;// tiempo promedio.
	//Se cargan las prioridades que tambien representan la ejecucion en segundos
	for(int i=0; i<process; i++) {
		procesos[i][0] = tempPriorities[i];
		procesos[i][1] = i+1;
	}
    //Se ordenan los elementos
	for (int i=0; i<np-1; i++) {
		for(int j=i+1; j<np; j++) {
			if (procesos[j][0]<procesos[i][0]) {
				int aux[2] = {procesos[j][0], procesos[j][1]};
				procesos[j][0] = procesos[i][0];
				procesos[j][1] = procesos[i][1];
				procesos[i][0] = aux[0];
				procesos[i][1] = aux[1];
			}
		}
	}
	//se ejecuta cada proceso 
	for (int i=0; i<np; i++) {
		tf += procesos[i][0];
		tp = tp + tf;
		call_process(procesos[i][0], i+1,0); //se configura el proceso
		printf("\nRegistrando P%d", i + 1);
		signal(SIGUSR1,sig_handler); //se registra la señal de la funcion del proceso
		raise(SIGUSR1); //se manda la señal para que comience a trabajar

		while(processEnd == 0);	//se espera que el proceso termine
		printf("\nProceso %d, concluye en %2.1f", procesos[i][1], tf);
	}
	printf("\n-------------------------------");
	printf("\nLa suma de los procesos %2.1f", tp);
	tp = tp / np;
	printf("\n\nTiempo promedio en SJF fue de: %2.2f:", tp);
	return 0;
}


void call_process(int pt, int pId, int q){
    processTime = pt;
    processId = pId;
    processEnd = 0;
    processTotal = q;
}

void sig_handler(int signum){  // funcion que se ejecuta en el proceso
  if(quantum > 0)
    printf("\nProceso P%d en CPU tiempo de %d segundos de  %d / %d segundos \n",processId,processTime, processDiff,processTotal );
  else
    printf("\nProceso P%d en CPU tiempo de %d segundos \n",processId,processTime );
  sleep(processTime); //tiempo que dura ejecutandose el proceso
  processEnd = 1;
}



