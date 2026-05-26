#ifndef CLIENTE_H_INCLUDED
#define CLIENTE_H_INCLUDED
#include <winsock2.h>

extern int enviarMensagemC(SOCKET socketCliente);
extern int receberMensagemC(SOCKET socketCliente);
int conectarCli();

/*
1. WSAStart e funções relacionadas, fez a comunicação com a lib do windows.
2. Depois, criamos o socket listenSocket, que vai ser o servidor.
3. Criamos o endereço, que deverá ser o mesmo do seu servidor, colocamos suas especificações e porta.
4, Tentamos a conexão com connect(). Passando o socket do cliente, o endereço alvo e o tamanho do alvo..
5. Testamos a conexão...
*/

#endif // CLIENTE_H_INCLUDED
