# Algoritmos Planificacion

Algoritmos

FIFO 
ROUNDROBIN 
SJF

Compilarlo
gcc -o algplan algplan.c 

Ejecucion

./algplan 

Parametros 

Si es FIFO

./algplan -F 8  

Ejecuta FIFO con 8 procesos


Si es Round Robin

./algplan -R 6 -Q 5  Ejecuta RoundRobin con 6 procesos y quantum de 5

Si es Prioridades

./algplan -P 5 -p 5 2 8 1 6  Ejecuta RoundRobin con 5 procesos

Se agrega el parametro de prioridad seguido del valor del numero de prioridades
