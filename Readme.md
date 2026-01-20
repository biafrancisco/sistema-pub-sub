**Sobre a aplicação**
Uma aplicação desenvolvida em C para monitoramento e transmissão de status de dispositivos via sockets UDP. O sistema suporta arquitetura Publisher/Subscriber, processamento de mensagens em threads e persistência de estado em arquivos.

**Funcionalidades**
* Comunicação UDP: Envio e recebimento de datagramas via sockets.
* Gerenciamento de Papéis: Validação e tratamento de papéis Publisher e Subscriber.
* Configuração Dinâmica: Carrega configurações (tópicos, papel) a partir de um arquivo externo.
* Multi-threading: Criação de thread para processar mensagens recebidas sem bloquear a execução principal.
* Persistência de Estado: Mapeia tópicos para arquivos de status e atualiza as informações sempre que há alguma alteração.
* Sistema de Logs: Registra eventos e erros na saída padrão (stdout) e em arquivos de log.

**Compilação**
As aplicações publisher e subscriber foram implementadas separadamente. O projeto inclui um *Makefile* para automatizar o processo de build e limpeza, com os comandos:
* make: compila os arquivos, e gera os executáveis 'publisher' e 'subscriber'.
* make clean: remove arquivos intermediários. 
* make purge: remove os arquivos intermediários e também os executáveis.

**Execução**
Os arquivos devem ser executados da seguinte forma:

./publisher <ip> <port>
./subscriber <port>

**Estrutura de pastas do projeto**

sistema-pub-sub/
├── Makefile
├── Readme.md
├── src/                                    <-- Códigos fonte
│   ├── logger.c                            <-- Funções para geração dos logs
│   ├── logger.h
│   ├── utils.c                             <-- Funções em comum nas duas aplicações
│   ├── utils.h
│   ├── subscriber.c                        <-- Lógica do subscriber
│   └── publisher.c                         <-- Lógica do publisher
│
└── tmp/                                    <-- Arquivos temporários
    ├── config/                             <-- Arquivos de configuração
    │   ├── config_pub1.txt
    │   ├── config_pub2.txt
    │   ├── config_pub3.txt
    │   ├── config_pub4.txt
    │   └── config_sub.txt
    │
    ├── log/                                <--Arquivos de log
    │   ├── example_pub_log.txt
    │   └── example_sub_log.txt
    |
    └── status/                             <-- Arquivos que guardam status, para persistência
        ├── home-living_room-lamp_1.txt
        └── home-living_room-lamp_1.txt


**Exemplo de Arquivo de Configuração**
Há um arquivo de configuração para o subscriber, e 4 para publishers. Sendo que desses 4, 3 são de tópicos que o subscriber está
inscrito, e 1 é de um tópico que ele não está inscrito.

*EX: Arquivo config_pub1.txt*
role:publisher
topic:/home/living_room/lamp_2


**Testes e Logs**
Para validar o funcionamento do sistema, foram realizados testes abrangendo cenários de erro e fluxo normal de execução. Os resultados de uma execução de teste estão disponíveis nos arquivos de log fornecidos:
example_pub_log.txt: Log de execução da aplicação Publisher.
example_sub_log.txt: Log de execução da aplicação Subscriber.

**Cenários de Teste**

*1. Tratamento de Erros (no Publisher)*
*Arquivo inexistente:* Tentativa de execução informando um arquivo de configuração que não existe.
*Conflito de Papel:* Tentativa de iniciar o publisher utilizando um arquivo de configuração definido com role=subscriber. A aplicação identifica a inconsistência e aborta a execução.

*2. Fluxo Normal*
*Filtragem de Tópicos:* Verificação do envio de atualizações de status para dispositivos. O sistema foi validado garantindo que:
    * O Subscriber *trata* mensagens de tópicos nos quais está inscrito.
    * O Subscriber *ignora* mensagens de tópicos nos quais *não* está inscrito.
*Disponibilidade:* Testes de envio realizados com o Subscriber em dois estados:
    * *Ativado*: Processa as mensagens em tempo real.
    * *Desativado*: Ignora todas as mensagens recebidas.


**Melhorias**
*Sistema de Logs Avançado: Implementação de níveis de severidade (INFO, WARN, ERROR) e inclusão de timestamps em cada entrada para facilitar a depuração.
*Refatoração e Modularização: Reorganização do código fonte, melhorando a clareza e facilitando a manutenção.
*Cobertura de Testes: Realização de mais testes no geral, como testes de estresse para avaliar o comportamento do programa sob alta carga de mensagens UDP.