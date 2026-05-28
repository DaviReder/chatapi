# ChatAPI 💬

O **ChatAPI** é uma aplicação de chat em tempo real baseada em terminal, desenvolvida em C utilizando a API **Winsock2** do Windows. O projeto permite que múltiplos clientes se conectem a um servidor local simultaneamente para trocar mensagens em um modelo de transmissão aberta (*broadcast*).

---

## ✨ Funcionalidades

* **Arquitetura Cliente-Servidor:** Uma única aplicação que pode atuar tanto como servidor (hospedeiro) quanto como cliente.
* **Múltiplas Conexões Simultâneas:** O servidor suporta até 10 clientes conectados ao mesmo tempo.
* **Comunicação Multithread:** Uso de threads (`CreateThread`) tanto no arquivo `servidor.c` quanto no `cliente.c` para permitir o envio e recebimento de mensagens de forma assíncrona.
* **Mensagens em Broadcast:** Tudo o que um cliente digita é replicado pelo servidor para todos os outros participantes através da função `transmitirParaTodos`.
* **Controle de Concorrência:** Implementação de seções críticas (`CRITICAL_SECTION`) no servidor para gerenciar a lista de clientes com segurança.

---

## 🛠️ Tecnologias Utilizadas

* **API de Redes:** Winsock2 (`winsock2.h`) para programação de sockets no Windows.
* **Biblioteca de Vínculo:** `Ws2_32.lib`.

---

## 🗂️ Estrutura do Projeto

```
├── include/
│   ├── cliente.h
│   └── servidor.h
├── cliente.c
├── servidor.c
├── main.c
└── README.md
```

----

## 🚀 Como Executar o Projeto

Como este projeto utiliza a biblioteca nativa do Windows, ele deve ser compilador e executado obrigatoriamente em ambiente Windows.

### Pré-requisitos
* Um compilador C (como GCC via MinGW, ou o MSVC do Visual Studio).
* Vincular a biblioteca do Winsock (`-lws2_32`) durante a compilação.

### Compilação via Terminal (GCC)
```bash
gcc main.c servidor.c cliente.c -o ChatAPI -lws2_32
```
Execute o arquivo gerado:
```bash
./ChatAPI
```
O menu inicial controlado pelo main.c exibirá as seguintes opções:
```bash
01: Hospedar um servidor localmente (escuta na porta 8080).  
02: Conectar a um servidor existente (conecta ao IP 127.0.0.1 na porta 8080).  
00: Sair do programa.
```

---
## 📈 Próximas Possíveis Melhorias (Roadmap)

- [ ] **Protocolo (JSON):** Detalha a substituição de buffers de strings brutas pela serialização estruturada por meio de objetos JSON (incluindo variáveis de controle como `tipo_evento`, `timestamp`, `nickname` e IDs de origem).

- [ ]  **Segurança (Criptografia):** Expõe a futura implementação da troca de chaves assíncrona por meio do algoritmo Diffie-Hellman combinada com a cifragem simétrica AES-256 para todas as payloads geradas.

- [ ]  **Interface e Resiliência:** Modela soluções para tratamento de quedas de rede (Heartbeats) e separação de linhas no terminal por meio de códigos ANSI para melhorar a experiência do usuário ao digitar enquanto recebe mensagens em broadcast.