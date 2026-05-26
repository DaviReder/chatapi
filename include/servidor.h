#ifndef SERVIDOR_H_INCLUDED
#define SERVIDOR_H_INCLUDED
#include <winsock2.h>

int criarServidor();

/*
1. WSAStart e funções relacionadas, fez a comunicação com a lib do windows.
2. Depois, criamos o socket listenSocket, que vai ser o servidor.
3. Criamos seu endereço também, colocamos suas especificações e porta.
4. Depois anexamos o endereço ao socket com o bind.
5. Agora precisamos escutar, usamos listen().
6. Ao escutar, espera receber uma conexão. Recebendo a conexão, precisamos de criar o endereço de quem se conectou.
7. Ao criar o endereço, criamos o segundo socket referente ao cliente, será atribuido os dados pela função accept().
8. A função accept recebe os mesmos parametros que a bind, mas com o endereço vazio, assim recebe os dados do cliente e anexa no endereço.
- Perceba que a comunicação será feita apenas pelo socket do cliente, o servidor só armazena o endereço da conexão (ponte).
9. Função send(socket cliente, sua mensagem "Oi!", Tamanho da mensagem, 0) envia do servidor uma mensagem para o cliente!
10. Função recv(socket cliente, char buffer, tamanho em bytes, 0) recebe a mensagem.
11. Testar tratamento de erro com os bytes recebidos.

int bytesRecebidos indica:
Positivo (> 0): "Chegou mensagem! Aqui está o tamanho dela."
Zero (== 0): "O cliente desconectou de forma educada (fechou o programa)."
Negativo (== SOCKET_ERROR): "A conexão caiu bruscamente ou o cabo foi puxado (Erro de rede)."
*/

#endif // SERVIDOR_H_INCLUDED
