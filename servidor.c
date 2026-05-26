#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "include/servidor.h"
#pragma comment(lib, "Ws2_32.lib")

int enviarMensagemS(SOCKET clientSocket){
    char respostaServidor[1024];
    printf_s("Servidor: ");
    fgets(respostaServidor, 1024, stdin); // Lê o que você digitar no servidor
    respostaServidor[strcspn(respostaServidor, "\r\n")] = 0; // Limpa o enter do texto

    // Envia para o cliente (incluindo o '\n' gerado pelo fgets) (obs tem que tirar a linha de cima)
    send(clientSocket, respostaServidor, strlen(respostaServidor), 0);
    return 0;
}

DWORD WINAPI ThreadReceberMSGCliente(LPVOID param){
    SOCKET clientSocket = (SOCKET)param;
    char bufferMensagem[1024];
    int bytesRecebidos;

    while(1){
        bytesRecebidos = recv(clientSocket, bufferMensagem, sizeof(bufferMensagem) - 1, 0);

        if (bytesRecebidos <= 0) {
            printf_s("Cliente desconectou.\n");
            return -1;
        }
        bufferMensagem[bytesRecebidos] = '\0';
        printf_s("\rO cliente disse: %s\nServidor: ", bufferMensagem);

        if (strcmp(bufferMensagem, "sair") == 0) {
            printf("Encerrando chat a pedido do cliente.\n");
            return -1;
        }
    }
    return 0;
}

int criarServidor(){
    printf("Aprendendo Sockets!\n");

    //Inicializa a biblioteca Winsock
    WSADATA wsdata;
    if (WSAStartup(MAKEWORD(2,2), &wsdata) != 0) {
        printf_s("Falha na inicialização: %d\n", WSAGetLastError());
        return 1;
    }

    //Cria o socket
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        printf_s("Erro ao criar socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Configura a estrutura de endereço
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);           // Porta 8080 (htons converte para 'Network Byte Order')
    serverAddr.sin_addr.s_addr = INADDR_ANY;    // Ouve em qualquer IP disponível na máquina

    //Bind -> setar o socket
    int res = bind(listenSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
    if(res == SOCKET_ERROR){
        printf_s("Bind failed with error %u\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    else
        printf("Bind returned success!\n");

    // A função listen não envia nem recebe dados. Ela apenas muda o estado do socket de "Fechado" para "Escutando".
    if(listen(listenSocket, 5) == SOCKET_ERROR){
        printf_s("Erro no Listen: %d\n", WSAGetLastError());
    }
    else{
        printf_s("Servidor ouvindo na porta 8080... Aguardando conexões.\n");
    }

    // --- PASSO: ACCEPT ---
    // Precisamos de uma estrutura vazia para guardar os dados de QUEM se conectou
    struct sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    // Criamos o socket do cliente que vai receber os dados pelo accept().
    SOCKET clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

    if (clientSocket == INVALID_SOCKET) {
        printf_s("Erro ao aceitar conexão: %d\n", WSAGetLastError());
    } else {
        printf_s("Cliente conectado com sucesso!\n\n");
        HANDLE hThread = CreateThread(NULL, 0, ThreadReceberMSGCliente, (LPVOID)clientSocket, 0, NULL);
        if(hThread != NULL) CloseHandle(hThread);

        // LOOP DO CHAT: Fica rodando enquanto a conexão estiver ativa
        while (1) {
            int enviou = enviarMensagemS(clientSocket);
            if (enviou == -1) break;
        }
    }

    Sleep(2000);

    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();

    return 0;
}
