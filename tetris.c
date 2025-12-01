#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAM_FILA 5
#define TAM_PILHA 3
#define TIPOS 4

typedef struct {
    char nome;
    int id;
} Peca;

typedef struct {
    Peca vetor[TAM_FILA];
    int inicio, fim, qtd;
} Fila;

typedef struct {
    Peca vetor[TAM_PILHA];
    int topo;
} Pilha;

// Utilitarios 
void strip_newline(char *s) {
    int l = strlen(s);
    if (l > 0 && s[l-1] == '\n') s[l-1] = '\0';
}

void pausa() {
    printf("\nPressione Enter...");
    getchar();
}

// Geracao automatica
Peca gerarPeca(int id) {
    char t[TIPOS] = {'I','O','T','L'};
    Peca p = { t[rand()%TIPOS], id };
    return p;
}

// Fila circular 
void initFila(Fila *f) { f->inicio = f->fim = f->qtd = 0; }
int filaVazia(Fila *f) { return f->qtd == 0; }
int filaCheia(Fila *f) { return f->qtd == TAM_FILA; }

int enqueue(Fila *f, Peca p) {
    if (filaCheia(f)) return 0;
    f->vetor[f->fim] = p;
    f->fim = (f->fim + 1) % TAM_FILA;
    f->qtd++;
    return 1;
}

int dequeue(Fila *f, Peca *r) {
    if (filaVazia(f)) return 0;
    *r = f->vetor[f->inicio];
    f->inicio = (f->inicio + 1) % TAM_FILA;
    f->qtd--;
    return 1;
}

void exibirFila(Fila *f) {
    printf("Fila: ");
    if (filaVazia(f)) { printf("[vazia]\n"); return; }
    int i = f->inicio;
    for (int c = 0; c < f->qtd; c++) {
        Peca p = f->vetor[i];
        printf("[%c %d] ", p.nome, p.id);
        i = (i + 1) % TAM_FILA;
    }
    printf("\n");
}

// Pilha 
void initPilha(Pilha *p) { p->topo = -1; }
int pilhaVazia(Pilha *p) { return p->topo == -1; }
int pilhaCheia(Pilha *p) { return p->topo == TAM_PILHA - 1; }

int push(Pilha *p, Peca x) {
    if (pilhaCheia(p)) return 0;
    p->vetor[++p->topo] = x;
    return 1;
}

int pop(Pilha *p, Peca *x) {
    if (pilhaVazia(p)) return 0;
    *x = p->vetor[p->topo--];
    return 1;
}

void exibirPilha(Pilha *p) {
    printf("Pilha (Topo->Base): ");
    if (pilhaVazia(p)) { printf("[vazia]\n"); return; }
    for (int i = p->topo; i >= 0; i--) {
        Peca x = p->vetor[i];
        printf("[%c %d] ", x.nome, x.id);
    }
    printf("\n");
}

// Trocas
int trocaSimples(Fila *f, Pilha *p) {
    if (filaVazia(f) || pilhaVazia(p)) return 0;
    Peca temp = f->vetor[f->inicio];
    f->vetor[f->inicio] = p->vetor[p->topo];
    p->vetor[p->topo] = temp;
    return 1;
}

int trocaBloco(Fila *f, Pilha *p) {
    if (f->qtd < 3 || p->topo+1 < 3) return 0;
    int idx = f->inicio;
    for (int i = 0; i < 3; i++) {
        Peca tmp = f->vetor[idx];
        f->vetor[idx] = p->vetor[p->topo - i];
        p->vetor[p->topo - i] = tmp;
        idx = (idx + 1) % TAM_FILA;
    }
    return 1;
}

// Menu 
void menu() {
    printf("\nOpcoes:\n");
    printf("1 - Jogar peca\n");
    printf("2 - Reservar peca\n");
    printf("3 - Usar peca reservada\n");
    printf("4 - Trocar frente da fila com topo da pilha\n");
    printf("5 - Trocar 3 pecas da fila com 3 da pilha\n");
    printf("0 - Sair\n");
    printf("Escolha: ");
}

// MAIN 
int main() {
    srand(time(NULL));

    Fila fila;
    Pilha pilha;
    initFila(&fila);
    initPilha(&pilha);

    int idGlobal = 0;
    char entrada[16];

    // preenche fila 
    for (int i = 0; i < TAM_FILA; i++)
        enqueue(&fila, gerarPeca(idGlobal++));

    while (1) {
        printf("\n------ ESTADO ------\n");
        exibirFila(&fila);
        exibirPilha(&pilha);
        printf("--------------------\n");

        menu();
        fgets(entrada, sizeof(entrada), stdin);
        strip_newline(entrada);

        // Jogar
        if (!strcmp(entrada,"1")) {
            Peca j;
            if (dequeue(&fila, &j))
                printf("\nJogada: [%c %d]\n", j.nome, j.id);
            else
                printf("\nFila vazia.\n");

            enqueue(&fila, gerarPeca(idGlobal++));
            pausa();
        }

        // Reservar 
        else if (!strcmp(entrada,"2")) {
            if (pilhaCheia(&pilha)) printf("\nPilha cheia.\n");
            else {
                Peca r;
                if (dequeue(&fila,&r)) {
                    push(&pilha,r);
                    printf("\nReservada: [%c %d]\n", r.nome, r.id);
                    enqueue(&fila, gerarPeca(idGlobal++));
                }
            }
            pausa();
        }

        // Usar reserva 
        else if (!strcmp(entrada,"3")) {
            Peca u;
            if (pop(&pilha,&u))
                printf("\nUsada: [%c %d]\n", u.nome, u.id);
            else
                printf("\nPilha vazia.\n");

            enqueue(&fila, gerarPeca(idGlobal++));
            pausa();
        }

        // Troca simples
        else if (!strcmp(entrada,"4")) {
            if (trocaSimples(&fila,&pilha))
                printf("\nTroca simples ok.\n");
            else
                printf("\nNao foi possivel trocar.\n");
            pausa();
        }

        // Troca bloco 
        else if (!strcmp(entrada,"5")) {
            if (trocaBloco(&fila,&pilha))
                printf("\nTroca em bloco realizada.\n");
            else
                printf("\nNao ha pecas suficientes.\n");
            pausa();
        }

        // Sair
        else if (!strcmp(entrada,"0")) {
            printf("\nEncerrando...\n");
            break;
        }

        else {
            printf("\nOpcao invalida.\n");
            pausa();
        }
    }

    return 0;
}