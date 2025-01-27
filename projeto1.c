#define _GNU_SOURCE
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sched.h>
#include <stdio.h>
#include <pthread.h> // Incluindo a biblioteca para utilizar mutexes

#define FIBER_STACK 1024*64

struct c {
    int saldo;
};

typedef struct c conta;
conta from, to;
int valor;
pthread_mutex_t mutex; // Declaração do mutex

int transferencia(void *arg) {
    pthread_mutex_lock(&mutex); // Bloqueia o mutex para exclusão mútua

    if (from.saldo >= valor) {
        from.saldo -= valor;
        to.saldo += valor;
    }

    printf("Transferência concluída com sucesso!\n");
    printf("Saldo de c1: %d\n", from.saldo);
    printf("Saldo de c2: %d\n", to.saldo);

    pthread_mutex_unlock(&mutex); // Libera o mutex após a operação

    return 0;
}

int main() {
    void* stack;
    pid_t pid;
    int i;

    // Inicialização dos mutexes
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Erro na inicialização do mutex.\n");
        exit(1);
    }

    stack = malloc(FIBER_STACK);
    if (stack == 0) {
        perror("malloc: could not allocate stack");
        exit(1);
    }
    
    from.saldo = 100;
    to.saldo = 100;

    printf("Transferindo 10 para a conta c2\n");

    valor = 10;
    for (i = 0; i < 10; i++) {
        pid = clone(&transferencia, (char*)stack + FIBER_STACK,
                    SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, 0);
        if (pid == -1) {
            perror("clone");
            exit(2);
        }
    }

    free(stack);
    printf("Transferências concluídas e memória liberada.\n");

    pthread_mutex_destroy(&mutex); // Destruir o mutex após o uso
    return 0;
}
