#include "main.h"
void diretorio(char **path) {
  char dir[50];
  if (*path == NULL) {
    printf("Uso incorreto do comando cd. Digite o diretório de destino.\n");
  } else {
    if (chdir(*path) == 0) {
      printf("Diretório alterado para: %s\n", getcwd(dir, 50));
    } else {
      printf("Falha ao alterar o diretório.\n");
    }
  }
}