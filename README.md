# MyShell: Implementação de um Interpretador de Comandos em C
Shell customizado em C que simula as funcionalidades essenciais de um terminal. O projeto implementa gestão de processos (fork/execvp), tratamento de sinais (SIGCHLD) e comunicação via pipes. Inclui comandos built-in (cd, mkdir), execução em segundo plano e um loop REPL robusto. Foco em lógica de sistemas e baixo nível.

## 🚀 Funcionalidades

O shell suporta as seguintes operações baseadas no código `shell_pipe.c`:

* **Execução de Comandos Externos:** Utiliza a família `exec` para rodar binários do sistema (ex: `ls`, `grep`, `top`).
* **Comandos Internos (Built-ins):**
    * `cd <diretório>`: Altera o diretório de trabalho atual usando `chdir`.
    * `mkdir <nome>`: Cria novos diretórios com permissões padrão `0777`.
    * `exit`: Finaliza o shell de forma segura.
* **Gerenciamento de Processos:**
    * **Foreground:** Execução padrão onde o shell aguarda o término do processo.
    * **Background:** Suporte ao operador `&` para execução sem bloqueio do terminal.
    * **Prevenção de Zumbis:** Manipulador de sinal `SIGCHLD` que utiliza `waitpid` com `WNOHANG` para limpar processos encerrados automaticamente.
* **Comunicação via Pipes:** Implementação de canais de comunicação bidirecionais entre o processo pai e o processo filho através de pipes.

## 🛠️ Conceitos de Sistemas Operacionais Aplicados

1. **Chamadas de Sistema (System Calls):** Uso direto de `fork()`, `execvp()`, `pipe()`, e `dup2()` para manipulação de baixo nível.
2. **Tratamento de Sinais:** Configuração de estruturas `sigaction` para gerenciar eventos assíncronos do kernel.
3. **Manipulação de Descritores de Arquivo:** Redirecionamento de `stdin` e `stdout` para os pipes, permitindo a troca de dados entre processos.
4. **Parsing e Tokenização:** Processamento de strings de entrada utilizando `strtok` para separar comandos de argumentos.

## 💻 Como Compilar e Rodar

### Pré-requisitos
* Compilador GCC.
* Ambiente Linux ou subsistema Unix (como WSL).

### Passo a passo no bash:
1. Clone este repositório:
    ```git clone [https://github.com/seu-usuario/seu-repositorio.git](https://github.com/seu-usuario/seu-repositorio.git) ```
3. Compile o arquivo:
    ```gcc shell_pipe.c -o myshell ```
4. Execute o interpretador:
     ```./myshell ```
