# Projeto: Algoritmos de Escalonamento - Sistemas Operacionais

Este é um projeto da disciplina de **Sistemas Operacionais** do curso de Análise e Desenvolvimento de Sistemas, do IF Baiano Campus Guanambi.

O grupo é composto por:

**Gabriel Montalvão** </br>
**Geovane Neves** </br>
**Maria Heloisa** </br>
**Ray Samuel** </br>
**Ruan Victor** </br>

## FCFS

Para compilar e executar o FCFS:

```sh
gcc -Wall -Wextra -std=c11 src/fcfs.c -o fcfs.out
./fcfs.out
```

## Priority

Para compilar e executar o PRIORITY:

```sh
gcc -Wall -Wextra -std=c11 src/priority.c -o priority.out
./priority.out
```

## Round Robin

O escalonador Round Robin usa cinco processos de exemplo definidos no código e um quantum de 3 segundos. Para compilar e executar em um terminal com ncurses:

```sh
gcc -std=c11 -Wall -Wextra src/rr.c -o rr.out -lncurses
./rr.out
```

## SJF

Para compilar e executar o SJF:

```sh
gcc -Wall -Wextra -std=c11 src/sjf.c -o sjf.out
./sjf.out
```

## SRTF

Para compilar e executar o SRTF:

```sh
gcc -Wall -Wextra -std=c11 src/srtf.c -o srtf.out
./srtf.out
```