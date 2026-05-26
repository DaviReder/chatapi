#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "include/cliente.h"
#pragma comment(lib, "Ws2_32.lib")

int enviarMensagemC(SOCKET socketCliente){
    char mensagem[1024];

    printf_s("Você: ");
    fgets(mensagem, sizeof(mensagem), stdin);
    mensagem[strcspn(mensagem, "\r\n")] = 0; // Limpa o enter do texto

    // Envia a string inteira de uma vez só
    send(socketCliente, mensagem, strlen(mensagem), 0);

    if (strcmp(mensagem, "sair") == 0) {
        printf_s("Saindo...\n");
        return -1;
    }
    return 0;
}

DWORD WINAPI ThreadReceberMSGServer(LPVOID param){
    SOCKET meuSocket = (SOCKET)param;
    char bufferResposta[1024];
    int bytesRecebidos;

    while(1) {
        bytesRecebidos = recv(meuSocket, bufferResposta, sizeof(bufferResposta) - 1, 0);

        if (bytesRecebidos > 0) {
            bufferResposta[bytesRecebidos] = '\0';

            // Truque visual: \r limpa o "Você: " atual para printar a mensagem do servidor por cima limpa
            printf_s("\rServidor: %s\nVocê: ", bufferResposta);
            fflush(stdout); // Força o Windows a desenhar na tela na hora

            if (strcmp(bufferResposta, "sair") == 0) {
                printf_s("\nO servidor encerrou o chat.\n");
                break;
            }
        }
        else if (bytesRecebidos == 0) {
            printf_s("\nConexao encerrada pelo servidor.\n");
            break;
        }
        else {
            // Se der erro (ex: socket fechado pelo main), encerra silenciosamente
            break;
        }
    }
    return 0;
}

int receberMensagemC(SOCKET socketCliente){
    char bufferResposta[1024];
    int bytesRecebidos;

    printf_s("Aguardando resposta do servidor...\n");
    bytesRecebidos = recv(socketCliente, bufferResposta, sizeof(bufferResposta) - 1, 0);

    if (bytesRecebidos > 0) {
        bufferResposta[bytesRecebidos] = '\0'; // Garante fechamento da string
        printf_s("Servidor: %s\n", bufferResposta);

        if (strcmp(bufferResposta, "sair") == 0) {
            printf_s("O servidor encerrou o chat.\n");
            return -1;
        }
    }
    else if (bytesRecebidos == 0) {
        printf_s("O servidor fechou a conexao.\n");
        return -1;
    }
    else {
        printf_s("Erro ao receber resposta: %d\n", WSAGetLastError());
        return -1;
    }
    return 0;
}

int conectarCli(){
    printf_s("Cliente!\n");

    //Inicializa a biblioteca Winsock
    WSADATA wsdata;
    if (WSAStartup(MAKEWORD(2,2), &wsdata) != 0) {
        printf_s("Falha na inicialização: %d\n", WSAGetLastError());
        return 1;
    }

    //Cria o socket
    SOCKET meuSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (meuSocket == INVALID_SOCKET) {
        printf_s("Erro ao criar socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Endereço do Servidor ALVO (Destino)
    struct sockaddr_in alvo;
    alvo.sin_family = AF_INET;
    alvo.sin_port = htons(8080);
    alvo.sin_addr.s_addr = inet_addr("127.0.0.1"); // Onde o servidor está rodando

    printf_s("Tentando conectar ao servidor...\n");
    int conexao = connect(meuSocket, (struct sockaddr*)&alvo, sizeof(alvo));

    if (conexao == SOCKET_ERROR) {
        printf_s("Falha na conexão: %d\n", WSAGetLastError());
    } else {
        printf_s("Conectado ao servidor com sucesso!\n");
        printf_s("Digite 'sair' para encerrar o chat.\n\n");

        //Partindo em uma nova THREAD para ler e receber ao mesmo tempo
        HANDLE hThread = CreateThread(NULL, 0, ThreadReceberMSGServer, (LPVOID)meuSocket, 0, NULL);
        if(hThread != NULL) CloseHandle(hThread);

        while (1) {
            int enviou = enviarMensagemC(meuSocket);
            if (enviou == -1) {
                break;
            }
        }
    }

    Sleep(2000);

    closesocket(meuSocket);
    WSACleanup();

    return 0;
}

