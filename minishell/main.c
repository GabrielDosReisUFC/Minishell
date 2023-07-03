#include "main.h"
#define MAX_WORDS 10
#define MAX_LINE_LENGTH 100
int deve_esperar = 1;

/*
  Tratador de sinal para Ctrl + C
*/

void sinal(int signal) {
  if (signal == SIGINT) {
    printf("\nPara sair do terminal digite:sair ou aperte Ctrl + D\n");
  }
}

/*
  Redireciona a saida de um arquivo para um comando
*/

void redirecionar_entrada(char *fileName) {
  /*Abre o arquivo fileName no modo leitura apenas*/
  int entrada = open(fileName, O_RDONLY);
  /* Pega o conteudo do "entrada" e repassa para a entrada padrão stdin*/
  dup2(entrada, 0);
  close(entrada);
}

/*
  Redireciona a saida de um comando para um arquivo
*/

void redirecionar_saida(char *fileName) {
  /*Abre o arquivo fileName no modo escrita e apaga seu conteudo, caso não
   * exista ele é criado, a permisão 0600*/
  int saida = open(fileName, O_WRONLY | O_TRUNC | O_CREAT, 0600);
  /*Redireciona a stdout para o arquivo */
  dup2(saida, 1);
  close(saida);
}

/*
  Responsável por chamar a função executar comandos bem como gerar os processos
  filhos com o fork e executa-los em primeiro ou segundo plano
*/

void executar(char *argv[]) {
  pid_t pid = fork();
  /* Cria o processo filho e executa o comando em primeiro plano ou em segundo
   * plano*/
  if (pid < 0) {
    perror("Erro na criação do processo filho");
    printf("Errou");
    exit(EXIT_FAILURE);
  }
  if (pid == 0) {
    execmd(argv);
    exit(EXIT_SUCCESS);
  } else {
    if (deve_esperar) {
      waitpid(pid, NULL, 0);
    } else {
      printf("PID: %d\n", pid);
    }
  }
  redirecionar_entrada("/dev/tty"); // entrada do terminal atual
  redirecionar_saida("/dev/tty");   // saída do terminal atual
}

void criar_pipe(char *args[]) {

  /* fd[0] será usado para a leitura e o segundo elemento fd[1] será usado para
   * a escrita */
  int fd[2];

  /*Cria o pipe*/
  if (pipe(fd) == -1) {
    perror("Erro pipe");
  };

  /*
    A função dup2() redireciona a saída padrão (stdout) para o descritor de
    arquivo fd[1] do pipe
  */
  dup2(fd[1], 1);
  close(fd[1]);

  // printf("args = %s\n", *args);

  executar(args);

  /*
  A função dup2() redireciona a entrada padrão (stdin) para o descritor de
  arquivo fd[0] do pipe
  */
  dup2(fd[0], 0);
  close(fd[0]);
}

int main(int ac, char *argv[]) {
  char dir[50];
  char *prompt = "\n(Ararashell) ";
  char *lineptr = NULL, *lineptr_copy = NULL;
  size_t n = 0;
  ssize_t nchars_read;
  const char *delim = " \n";
  int command = 0;

  if (signal(SIGINT, sinal) == SIG_ERR) {
    perror("signal");
    exit(EXIT_FAILURE);
  }

  (void)ac;

  while (1) {
    int num_tokens = 0;
    char *token;
    int i;
    deve_esperar = 1;
    printf("%s~%s$ ", prompt, getcwd(dir, 50));
    char *lineptr = NULL, *lineptr_copy = NULL;
    size_t tamanhoLinha = 0;
    ssize_t nchars_read;
    char *palavras[MAX_WORDS];
    int numPalavras = 0;
    int cd = 0;

    nchars_read = getline(&lineptr, &tamanhoLinha, stdin);
    // alocar memória suficiente para copiar o comando obtido pelo getline

    /*
      Fecha a shell caso seja usado Ctrl + D
    */
    if (nchars_read == -1) {
      printf("Fechando shell....\n");
      return (-1);
    }

    lineptr_copy = malloc(sizeof(char) * nchars_read);
    // verificando possíveis erros
    if (lineptr_copy == NULL) {
      perror("tsh: erro de alocacao de memoria");
      return (-1);
    } else {

      if (nchars_read != -1) {
        char *palavra = strtok(lineptr, " \t\n");

        while (palavra != NULL && numPalavras < MAX_WORDS) {
          if (strcmp(palavra, "listar") == 0) {
            palavras[numPalavras] = strdup("ls");
          } else if (strcmp(palavra, "matar") == 0) {
            palavras[numPalavras] = strdup("kill");
          } else if (strcmp(palavra, "removerdir") == 0) {
            palavras[numPalavras] = strdup("rmdir");
          } else if (strcmp(palavra, "criardir") == 0) {
            palavras[numPalavras] = strdup("mkdir");
          } else if (strcmp(palavra, "copiar") == 0) {
            palavras[numPalavras] = strdup("cp");
          } else if (strcmp(palavra, "buscar") == 0) {
            palavras[numPalavras] = strdup("grep");
          } else if (strcmp(palavra, "diratual") == 0) {
            palavras[numPalavras] = strdup("pwd");
          } else if (strcmp(palavra, "eco") == 0) {
            palavras[numPalavras] = strdup("echo");
          } else if (strcmp(palavra, "processos") == 0) {
            palavras[numPalavras] = strdup("ps");
          } else if (strcmp(palavra, "remove") == 0) {
            palavras[numPalavras] = strdup("rm");
          } else if (strcmp(palavra, "mover") == 0) {
            palavras[numPalavras] = strdup("mv");
          } else if (strcmp(palavra, "tocar") == 0) {
            palavras[numPalavras] = strdup("touch");
          } else if (strcmp(palavra, "concatenar") == 0) {
            palavras[numPalavras] = strdup("cat");
          } else if (strcmp(palavra, "achar") == 0) {
            palavras[numPalavras] = strdup("find");
          } else if (strcmp(palavra, "liberardisco") == 0) {
            palavras[numPalavras] = strdup("df");
          } else if (strcmp(palavra, "data") == 0) {
            palavras[numPalavras] = strdup("date");
          } else if (strcmp(palavra, "liberar") == 0) {
            palavras[numPalavras] = strdup("free");
          } else if (strcmp(palavra, "diretorio") == 0 ||
                     strcmp(palavra, "cd") == 0) {
            cd = 1;
            palavras[numPalavras] = strdup("");
          } else {
            palavras[numPalavras] = strdup(palavra);
          }
          palavra = strtok(NULL, " \t\n");
          numPalavras++;
        }
      } else {
        printf("Erro ao ler a linha.\n");
      }

      char *resultado =
          malloc(MAX_LINE_LENGTH *
                 sizeof(char)); // Aloca memória para a string resultado
      resultado[0] = '\0';      // Inicializa a string resultado vazia

      for (int i = 0; i < numPalavras; i++) {
        strcat(resultado,
               palavras[i]); // Concatena a palavra atual no resultado
        if (cd == 0) {
          if (i < numPalavras - 1) {
            strcat(resultado, " "); // Adiciona um espaço após cada palavra
          }
        } else {
          strcat(resultado, ""); // Adiciona um espaço após cada palavra
        }
      }

      for (int i = 0; i < numPalavras; i++) {
        free(palavras[i]); // Libera a memória alocada para cada palavra
      }

      lineptr = resultado;
      if (cd == 1) {
        char *strings = lineptr;
        char **dir = &strings;
        diretorio(dir);
      }

    } // Libera a memória alocada para a string resultado

    // verifica se na ultima linha contém o & indicando que o programa vai
    // operar em background
    if (lineptr[strlen(lineptr) - 1] == '&') {
      deve_esperar = 0;
      lineptr[strlen(lineptr) - 1] = ' ';
    }

    strcpy(lineptr_copy, lineptr);

    /*Separa a string de acordo com o delimitador especificado e entraga a
     * próxima string apos esse delimitador*/
    token = strtok(lineptr, delim);

    while (token != NULL) {
      num_tokens++;
      token = strtok(NULL, delim);
    }
    num_tokens++;

    argv = malloc(sizeof(char *) * num_tokens);

    token = strtok(lineptr_copy, delim);
    int cont = 0;
    /*
      Percorrer a string buscando os caracteres especiais > , < e |
    */
    for (i = 0; token != NULL; i++) {
      argv[cont] = malloc(sizeof(char) * strlen(token));
      if (*token == '<') {
        redirecionar_entrada(strtok(NULL, " "));
      } else if (*token == '>') {
        redirecionar_saida(strtok(NULL, " "));
      } else if (*token == '|') {
        argv[cont] = NULL;
        criar_pipe(argv);
        cont = 0;
      } else {
        strcpy(argv[cont], token);
        cont++;
      }
      token = strtok(NULL, delim);
    }

    argv[cont] = NULL;
    if (cd == 0) {
      executar(argv);
    }
    fflush(stdout);
  }

  /*libera a memória usada*/
  free(lineptr_copy);
  free(lineptr);

  return 0;
}