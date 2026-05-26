#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "include/servidor.h"
#pragma comment(lib, "Ws2_32.lib")

#define MAX_CLIENTES 10

int conexaoAtiva = 1;
SOCKET clientesConectados[MAX_CLIENTES];
int totalClientes = 0;
CRITICAL_SECTION travaListaClientes;

// --- PASSO 1: COLOCAR A TRANSMISSÃO NO TOPO ---
// Movida para cá para que a Thread abaixo saiba que ela existe!
void transmitirParaTodos(char* mensagem, SOCKET socketOrigem) {
    EnterCriticalSection(&travaListaClientes);

    for (int i = 0; i < totalClientes; i++) {
        if (clientesConectados[i] != socketOrigem) {
            send(clientesConectados[i], mensagem, strlen(mensagem), 0);
        }
    }

    LeaveCriticalSection(&travaListaClientes);
}

// --- PASSO 2: A THREAD DE LEITURA DO CLIENTE ---
DWORD WINAPI ThreadReceberMSGCliente(LPVOID param){
    SOCKET meuSocket = (SOCKET)param;
    char buffer[1024];
    int bytesRecebidos;

    while (1) {
        bytesRecebidos = recv(meuSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesRecebidos <= 0 || (bytesRecebidos > 0 && strcmp(buffer, "sair") == 0)) {
            printf("\n[Sistema] Um cliente desconectou.\n");
            break;
        }

        buffer[bytesRecebidos] = '\0';

        // Trocado %d por %llu para sumir o warning do SOCKET
        printf("\n[Log] Cliente %llu disse: %s", (unsigned long long)meuSocket, buffer);
        fflush(stdout);

        char mensagemFormatada[1200];
        sprintf(mensagemFormatada, "[Jogador %llu]: %s", (unsigned long long)meuSocket, buffer);
        transmitirParaTodos(mensagemFormatada, meuSocket);
    }

    closesocket(meuSocket);

    EnterCriticalSection(&travaListaClientes);
    for (int i = 0; i < totalClientes; i++) {
        if (clientesConectados[i] == meuSocket) {
            for (int j = i; j < totalClientes - 1; j++) {
                clientesConectados[j] = clientesConectados[j + 1];
            }
            totalClientes--;
            break;
        }
    }
    LeaveCriticalSection(&travaListaClientes);

    return 0;
}

// --- PASSO 3: FUNÇÃO PRINCIPAL DO SERVIDOR ---
int criarServidor(){
    printf("Aprendendo Sockets!\n");

    WSADATA wsdata;
    if (WSAStartup(MAKEWORD(2,2), &wsdata) != 0) {
        printf_s("Falha na inicializacao: %d\n", WSAGetLastError());
        return 1;
    }

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        printf_s("Erro ao criar socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    int res = bind(listenSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
    if(res == SOCKET_ERROR){
        printf_s("Bind failed with error %u\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    else {
        printf("Bind returned success!\n");
    }

    if (listen(listenSocket, 5) == SOCKET_ERROR) {
        printf_s("Erro no Listen: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    InitializeCriticalSection(&travaListaClientes);

    printf("\nServidor aguardando conexoes na porta 8080...\n");

    while (1) {
        struct sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);

        SOCKET newClient = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

        if (newClient == INVALID_SOCKET) {
            printf_s("[Erro] Falha ao aceitar uma nova conexao: %d\n", WSAGetLastError());
            continue;
        }

        EnterCriticalSection(&travaListaClientes);

        if (totalClientes < MAX_CLIENTES) {
            clientesConectados[totalClientes] = newClient;
            totalClientes++;

            // Trocado %d por %llu aqui tambem para o ID do socket
            printf_s("[Conexao] Novo cliente conectado! Socket ID: %llu (Total: %d/%d)\n",
                     (unsigned long long)newClient, totalClientes, MAX_CLIENTES);

            // CORRIGIDO: Nome da função alterado para ThreadReceberMSGCliente
            HANDLE hThread = CreateThread(NULL, 0, ThreadReceberMSGCliente, (LPVOID)newClient, 0, NULL);
            if (hThread != NULL) {
                CloseHandle(hThread);
            }
        }
        else {
            printf_s("[Aviso] Conexao recusada. Servidor lotado!\n");
            char* msgLotado = "Servidor lotado! Tente novamente mais tarde.\n";
            send(newClient, msgLotado, strlen(msgLotado), 0);
            closesocket(newClient);
        }

        LeaveCriticalSection(&travaListaClientes);
    }

    // Código de limpeza (caso saia do loop algum dia)
    closesocket(listenSocket);
    WSACleanup();
    DeleteCriticalSection(&travaListaClientes);
    return 0;
}
