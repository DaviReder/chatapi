#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "include/cliente.h"
#pragma comment(lib, "Ws2_32.lib")

int clienteAtivo = 1;

int enviarMensagemC(SOCKET socketCliente){
    char mensagem[1024];

    printf_s("Você: ");
    fgets(mensagem, sizeof(mensagem), stdin);
    mensagem[strcspn(mensagem, "\r\n")] = 0; // Limpa o enter do texto

    if (!clienteAtivo) {
        return -1;
    }

    // Envia a string inteira de uma vez só
    int resultado = send(socketCliente, mensagem, strlen(mensagem), 0);

    if (resultado == SOCKET_ERROR || strcmp(mensagem, "sair") == 0) {
        clienteAtivo = 0;
        return -1;
    }
    return 0;
}

DWORD WINAPI ThreadReceberMSGServer(LPVOID param){
    SOCKET meuSocket = (SOCKET)param;
    char bufferResposta[1024];
    int bytesRecebidos;

    while(clienteAtivo) {
        bytesRecebidos = recv(meuSocket, bufferResposta, sizeof(bufferResposta) - 1, 0);

        if (bytesRecebidos <= 0 || (bytesRecebidos > 0 && strcmp(bufferResposta, "sair") == 0)) {
            if (clienteAtivo) {
                printf_s("\n\n[Sistema] O servidor encerrou o chat. Aperte ENTER para sair...\n");
                clienteAtivo = 0;
                closesocket(meuSocket);
            }
            break;
        }

        bufferResposta[bytesRecebidos] = '\0';
        printf_s("\r%s\nVocê: ", bufferResposta);
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
        clienteAtivo = 1;

        //Partindo em uma nova THREAD para ler e receber ao mesmo tempo
        HANDLE hThread = CreateThread(NULL, 0, ThreadReceberMSGServer, (LPVOID)meuSocket, 0, NULL);
        if(hThread != NULL) CloseHandle(hThread);

        while (clienteAtivo) {
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

