#include "main.h"
char *get_location(char *command) {
  char *path, *path_copy, *path_token, *file_path;
  int command_length, directory_length;
  struct stat buffer;

  /*Consegue todas as variáveis de ambiente*/
  path = getenv("PATH");

  if (path) {

    /*Copia a string para não modificar o valor original*/
    path_copy = strdup(path);

    command_length = strlen(command);

    /* Separa a string em tokens separados por ":" */
    path_token = strtok(path_copy, ":");

    /* Realiza um busca nas variáveis de ambiente*/
    while (path_token != NULL) {
      directory_length = strlen(path_token);

      file_path = malloc(command_length + directory_length + 2);
      /*Copia o valor atual de path_token*/
      strcpy(file_path, path_token);
      /*Adiciona o / no final do file_path*/
      strcat(file_path, "/");
      /*Concatena com o comando passado*/
      strcat(file_path, command);
      /*Adiciona um caracter nulo ao final da string indicando seu fim*/
      strcat(file_path, "\0");

      /*Verifica se o comando está dentro daquele path*/
      if (stat(file_path, &buffer) == 0) {

        free(path_copy);
        /*Se sim, retorna o path*/
        return (file_path);
      } else {
        free(file_path);
        /*Se não, passa para o próximo path e limpa a memória*/
        path_token = strtok(NULL, ":");
      }
    }

    free(path_copy);
    /*Verifica se o comando é valido no path atual*/
    if (stat(command, &buffer) == 0) {
      return (command);
    }
    /*Não achou um diretorio com o executável*/
    return (NULL);
  }
  /*Não conseguiu acessar o PATH*/
  return (NULL);
}