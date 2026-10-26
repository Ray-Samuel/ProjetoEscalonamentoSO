# Projeto: Algoritmos de Escalonamento - Sistemas Operacionais

Este é um projeto da disciplina de **Sistemas Operacionais** do curso de Análise e Desenvolvimento de Sistemas, do IF Baiano Campus Guanambi.

O grupo é composto por:

**Geovane Neves** </br>
**aluno2** </br>
**aluno3** </br>
**aluno4** </br>

## Round Robin

O escalonador Round Robin usa cinco processos de exemplo definidos no código e um quantum de 3 segundos. Para compilar e executar em um terminal com ncurses:

```sh
gcc -std=c11 -Wall -Wextra src/rr.c -o rr -lncurses
./rr
```

## SJF

Para compilar e executar o SJF:

```sh
gcc -Wall -Wextra -std=c11 src/sjf.c -o sjf
./sjf
```