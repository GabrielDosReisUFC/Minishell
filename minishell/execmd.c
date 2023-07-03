#include "main.h"

void execmd(char *argv[]) {
  char *comando = NULL, *caminho_comando = NULL;

  if (argv) {

    comando = argv[0];
    /* Consegue o path do comando */
    caminho_comando = get_location(comando);

    /* Passa o caminho do comando, os argumentos e NULL indicando que nenhum
     * path a mais está disponível para busca  */
    if (execve(caminho_comando, argv, NULL) == -1) {
      perror("Erro:");
    }
  }
}