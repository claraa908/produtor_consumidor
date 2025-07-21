/*
Trabalho desenvolvido por:
Alana Maria Sousa Augusto - 564976
Clara Cruz Alves - 568563
Jennifer Marques de Brito - 569710

1° parte da primeira questão 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define LOCK(mtx) WaitForSingleObject(mtx, INFINITE)
#define UNLOCK(mtx) ReleaseMutex(mtx)

#define DOWN(sem)   WaitForSingleObject(sem, INFINITE)
#define UP(sem) ReleaseSemaphore(sem, 1, NULL)

HANDLE mutex;
HANDLE item;
HANDLE vaga;

//controle do consumer
int valores_processados = 0; // contador de produtos consumidos
int valores_totais = 24  + 26 + 25; //quantidade de produtos que eu passo nas threads

//buffer global
int buffer[5]; //buffer com 5 espaços
int in = 0; // indice de acesso do produtor
int out = 0; // indice de acesso do consumidor

/**
 * @brief Função do produtor recebe a quantidade de produtos que uma thread vai 
 * produzir, converte a quantidade para inteiro, cria um iterador, pega o id da
 * thread, ????. 
 * 
 * No while, diminui uma vaga disponível(DOWN) para colocar um produto, proteje a área
 * de concorrência (LOCK), aleatoriza um preço para o produto e o associa ao produto
 * atualiza o indice de forma circular (caso passe do valor, reinicia), 
*/
DWORD WINAPI producer(LPVOID args){
    int n = *((int *) args);
    int it = 0;
    DWORD tid = GetCurrentThreadId();
    srand(time(NULL) ^ tid);

    while(n-- > 0){
        DOWN(vaga);
        LOCK(mutex);
        int num = 1 + rand() % 1000;
        buffer[in] = num;
        in = (in + 1) % 5; //atualiza de forma circular
        printf("(P) TID: %lu | VALOR: R$ %d | ITERACAO: %d\n", tid, num, it);
        it++;
        UNLOCK(mutex);
        UP(item);

        int delay = 1 + rand() % 1;
        Sleep(delay * 1000);
    }
    printf("(P) TID: %lu finalizou\n", tid);
    return 0;
}

DWORD WINAPI consumer(LPVOID args){
    float media = 0;
    int it = 0;
    DWORD tid = GetCurrentThreadId();

    while(valores_processados < valores_totais){
        for(int i = 0; i < 5; i++) DOWN(item);

        LOCK(mutex);
        int contador = 0;
        for(int i = 0; i < 5; i++){
            contador += buffer[out];
            out = (out + 1) % 5;
            valores_processados++;
        }
        media = (float)contador / 5;
        it++;
        printf("(C) TID: %lu | MEDIA: R$ %.2f | ITERACAO: %d\n", tid, media, it);
        UNLOCK(mutex); 

        for(int i = 0; i < 5; i++) UP(vaga);
    }
    printf("(C) TID: %lu finalizou\n", tid);
    return 0;
}

int main(){
    //variáveis de controle
    mutex = CreateMutex(NULL, FALSE, NULL);
    vaga = CreateSemaphore(NULL,5,5,NULL);
    item = CreateSemaphore(NULL, 0,5, NULL);
    

    //threads
    HANDLE t1;
    DWORD t1Id;
    int qnt1 = 24;
    t1 = CreateThread(NULL, 0, producer, &qnt1, 0, &t1Id);

    if (t1 == NULL) {
        printf("Erro ao criar thread 1.\n");
        return 1;
    }

    HANDLE t2;
    DWORD t2Id;
    int qnt2 = 26;
    t2 = CreateThread(NULL, 0, producer, &qnt2, 0, &t2Id);

    if (t2 == NULL) {
        printf("Erro ao criar thread 2.\n");
        return 1;
    }

    HANDLE t3;
    DWORD t3Id;
    int qnt3 = 25;
    t3 = CreateThread(NULL, 0, producer, &qnt3, 0, &t3Id);

    if (t3 == NULL) {
        printf("Erro ao criar thread 3.\n");
        return 1;
    }

    HANDLE t4;
    DWORD t4Id;
    t4 = CreateThread(NULL, 0, consumer, NULL, 0, &t4Id);

    WaitForSingleObject(t1, INFINITE);
    CloseHandle(t1);

    WaitForSingleObject(t2, INFINITE);
    CloseHandle(t2);

    WaitForSingleObject(t3, INFINITE);
    CloseHandle(t3);

    WaitForSingleObject(t4, INFINITE);
    CloseHandle(t4);
    return 0;
}