#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "include/cliente.h"
#include "include/servidor.h"

int main()
{
    int opc;
    setlocale(LC_ALL, "");
    printf_s("Bem vindo ao ChatAPI!\n");
    printf_s("00: SAIR;\n");
    printf_s("01: Hospedar servidor - localmente;\n");
    printf_s("02: Conectar a um servidor;\n");
    printf_s("Digite: ");
    scanf_s("%d", &opc);

    char c;
    while ((c = getchar()) != '\n' && c != EOF);

    switch(opc){
    case 1:
        int servidor = criarServidor();
        if(servidor == -1) printf_s("Erro ao criar servidor!\n");
        else{

        }
        break;
    case 2:
        int cliente = conectarCli();

        if(cliente == -1) printf_s("Erro ao conectar!\n");
        else{

        }
        break;
    default:
        if(opc != 0) printf_s("Digite uma opçõa válida!\n");
    }
    return 0;
}
