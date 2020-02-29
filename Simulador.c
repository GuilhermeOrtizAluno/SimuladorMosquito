#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct no{
  char nome[35];
  //Como casa: n1 = numero de pessoas saudaveis e n2 = numero de pessoas doentes.
  //Como mosquito: n1 = numero de identificação do mosquito e n2 = numero de movimentações.
  int n1, n2;
  //Lista que liga todas as nos existentes.
  struct no *prox;
  //Como casa: Cada no tem uma lista de nos as quais ela é ligada.
  struct header *baixo;
}Tno;

typedef struct header{
  //Numero de nos numa determinada lista.
  int num;
  Tno *inicio, *fim;
}Theader;

//Função para criar a cabeca de nos dentro de um header.
int criacabeca (Theader *header);
//Função para encontrar um no pelo nome.
Tno *achano (Theader *header, char nome[35]);
//Função para inserir uma casa.
int inserecasa (Theader *header, char nome[35], int pessoas);
//Função que cria a ligação entre duas casas.
int ligacasa (Theader *header, char c1[35], char c2[35]);
//Função para inserir um mosquito.
int inseremosquito (Theader *cidade, Theader *aedes, char casa[35]);
//Lê o arquivo de entrada e executa as funções.
int manipulacao (Theader *cidade, Theader *aedes, int *atua, int *saude, int *bota, int *simulacao, int *surto);
//Função que simula o cenário ja criado e cria o arquivo de saida.
int simula (Theader *cidade, Theader *aedes, int *atua, int *saude, int *bota, int *simulacao, int *surto);
//Função para movimentar todos os mosquitos existentes na lista.
int mosquito_move (Theader *cidade, Theader *aedes, int *move);
//Função que remove um no mosquito de uma lista.
int mata_mosquito (Theader *aedes, Tno *mosquito);
//Função que executa o agente para se mover em uma casa e matar os mosquitos.
int agente_atua (Theader *cidade, Theader *aedes, Tno *casax);
//Função que executa o agente para para se mover em uma casa e curar os doentes.
int agente_saude (Theader *cidade, Theader *aedes, Tno *casax);
//Função para verificar na lista de mosquitos se existe mosquitos prontos para botar.
int mosquito_bota (Theader *cidade, Theader *aedes, int bota);
//Função que verifica a cada "turno" de movimentos de mosquito, se esta ocorrendo um surto de dengue.
int verificasurto (Theader *cidade, int pop);
//Cria e imprime as listas iniciais e finais no "entrada.txt".
int imprimelistas (Theader *cidade, Theader *mosquito, FILE *arq);
//Desaloca a memória das listas.
int limpalistas (Theader *cidade, Theader *mosquito);

int main() {
  Theader *cidade = (Theader *) malloc(sizeof(Theader));
  Theader *aedes = (Theader *) malloc(sizeof(Theader));
  if (fopen("entrada.txt", "r") == NULL) {
    return 1;
  }
  FILE *arq = fopen("lista.txt", "w");
  //Inicio as variaveis inteiras como negativas por motivos de comparações futuras.
  int atua = -1, saude = -1, bota = -1, simulacao = -1, surto = 0;
  srand(time(NULL));
  if((criacabeca(cidade) == 1) || criacabeca(aedes)==1){
    return 1;
  }
  if (manipulacao(cidade, aedes, &atua, &saude, &bota, &simulacao, &surto) == 1) {
      return 1;
  }
  fprintf(arq, "Inicial:\n");
  imprimelistas(cidade,aedes,arq);
  if (simula(cidade, aedes, &atua, &saude, &bota, &simulacao, &surto) == 1) {
    printf("\nERRO NA SIMULACAO\n");
    return 1;
  }
  fprintf(arq, "\nFinal:\n");
  imprimelistas(cidade, aedes, arq);
  limpalistas(cidade, aedes);
  free(cidade);
  free(aedes);
  fclose(arq);
  return 0;
}

int criacabeca(Theader *header){
  Tno *aux = (Tno *) malloc(sizeof(Tno));
  if(header == NULL || aux == NULL){
    return 1;
  }
  header->inicio = header->fim = aux;
  header->num = 0;
  aux->n1 = aux->n2 = -1;
  aux->baixo = NULL;
  aux->prox = NULL;
  return 0;
}

Tno *achano (Theader *header, char nome[35]) {
  if (header == NULL) {
    return NULL;
  }
  Tno *aux = NULL;
  for (aux = header->inicio->prox; aux != NULL; aux = aux->prox) {
    if (strcmp(aux->nome, nome) == 0) {
      return aux;
    }
  }
  return NULL;
}

int inserecasa (Theader *header, char nome[35], int pessoas) {
  Tno *aux = (Tno *) malloc(sizeof(Tno));
  if(aux == NULL){
    printf("\nFALHA NA ALOCACAO DE MEMORIA (inserecasa)\n");
    return 1;
  }
  aux->baixo = (Theader *) malloc(sizeof(Theader));
  if ((criacabeca(aux->baixo) == 1) && (achano(header, nome) == NULL)) {
    printf("\nCABECA COM ESSE NOME JA EXISTE\n");
    return 1;
  }
  header->num = header->num + 1;
  aux->prox = NULL;
  header->fim->prox = aux;
  header->fim = aux;
  strcpy(aux->nome, nome);
  aux->n1 = pessoas;
  aux->n2 = 0;
  return 0;
}

int ligacasa (Theader *header, char c1[35], char c2[35]) {
  Tno *aux, *aux1 = NULL, *aux2 = NULL, *aux3 = NULL;
  //Procurando na lista geral se existem as casas com os nomes c1 e c2.
  for (aux = header->inicio->prox; aux!=NULL; aux = aux->prox) {
    if (strcmp(aux->nome,c1) == 0) {
      aux1 = aux;
    }
    if (strcmp(aux->nome,c2) == 0) {
      aux2 = aux;
    }
  }
  //Se não existir, retorna .
  if ((aux1 == NULL) || (aux2 == NULL)) {
    return 1;
  }
  //Dentro da lista da no c1, procura-se a no c2, se existir, é porque a ligação ja existe.
  aux3 = aux1->baixo->inicio;
  for (aux3 = aux3->prox; aux3 != NULL; aux3 = aux3->prox){
    if (strcmp(aux3->nome, c2) == 0) {
      return 1;
    }
  }
  //Se tudo estiver certo, a ligação é criada.
  if ((inserecasa(aux1->baixo, c2, -1) == 1) || (inserecasa(aux2->baixo, c1, -1) == 1)) {
    return 1;
  }
  return 0;
}

int inseremosquito (Theader *cidade, Theader *aedes, char casa[35]) {
  int n = 0;
  Tno *aux = (Tno *) malloc(sizeof(Tno));
  Tno *aux2 = achano(cidade, casa);
  if((aux == NULL) || (aux2 == NULL)){
    printf("\nERRO AO ALOCAR ESPACO OU ENCONTRAR NO (inseremosquito)\n");
    return 1;
  }
  //Quando se insere um mosquito em uma casa, ele começa ja infectando as pessoas dessa casa. (?)
  if (aux2->n1 != 0) {
    n = aux2->n1;
    aux2->n1 = 0;
    aux2->n2 = n;
  }
  aedes->num = aedes->num + 1;
  aux->n1 = aedes->num;
  strcpy(aux->nome, casa);
  aux->n2 = 0;
  aedes->fim->prox = aux;
  aedes->fim = aux;
  aux->prox = NULL;
  aux->baixo = NULL;
  return 0;
}

int manipulacao (Theader *cidade, Theader *aedes, int *atua, int *saude, int *bota, int *simulacao, int *surto) {
  FILE *arq = fopen("entrada.txt","r");
  if(arq == NULL){
    return 1;
  }
  char op[20], aux1[20], aux2[20];
  int num;
  while(fscanf(arq, "%s", op) == 1){
    if ((strcmp(op, "AGENTE_ATUA")) == 0) {
      fscanf(arq, "%d", &(*atua));
    }
    if ((strcmp(op, "AGENTE_SAUDE")) == 0) {
      fscanf(arq, "%d", &(*saude));
    }
    if ((strcmp(op, "MOSQUITO_BOTA")) == 0) {
      fscanf(arq, "%d", &(*bota));
    }
    if(strcmp(op, "inserecasa") == 0){
      fscanf(arq, "%s %d", aux1, &num);
      inserecasa(cidade, aux1 , num);
    }
    if(strcmp(op, "ligacasas") == 0){
      fscanf(arq, "%s %s", aux1, aux2);
      ligacasa(cidade, aux1, aux2);
    }
    if(strcmp(op, "inseremosquito") == 0) {
      fscanf(arq, "%s", aux1);
      inseremosquito(cidade, aedes, aux1);
    }
    if ((strcmp(op, "iniciasimulacao")) == 0) {
      fscanf(arq, "%d", &(*simulacao));
    }
    if ((strcmp(op, "verificasurto")) == 0) {
      *surto = 1;
    }
    //A simulação só pode ocorrer se os comandos AGENTE_ATUA, AGENTE_SAUDE e MOSQUITO_BOTA forem especificados e forem positivos.
    if ((strcmp(op, "FIM")) == 0) {
      if (*atua < 0) {
        printf("\nCOMANDO AGENTE_ATUA NAO ESPECIFICADO OU VALOR INVALIDO\n");
        return 1;
      }
      if (*saude < 0) {
        printf("\nCOMANDO AGENTE_SAUDE NAO ESPECIFICADO OU VALOR INVALIDO\n");
        return 1;
      }
      if (*bota < 0) {
        printf("\nCOMANDO MOSQUITO_BOTA NAO ESPECIFICADO OU VALOR INVALIDO\n");
        return 1;
      }
      //Se não encontrar problemas, ele fecha sem problemas
      fclose(arq);
      return 0;
    }
  }
  //Se não, ele da erro.
  printf("\nCOMANDO FIM NAO ENCONTRADO\n");
  return 1;
}

int simula (Theader *cidade, Theader *aedes, int *atua, int *saude, int *bota, int *simulacao, int *surto) {
  FILE *saida = fopen("log.txt", "w");
  Tno *aux;
  int i = 0 ,inicial = 0, pop = 0, acertos = 0, move = 0, surtant = 0;
  if (saida == NULL) {
    return 1;
  }
  //Contando numero inicial de mosquitos.
  for (aux = aedes->inicio->prox; aux != NULL; aux = aux->prox) {
    inicial++;
  }
  //Contando o numero de moradores.
  for (aux = cidade->inicio->prox; aux != NULL; aux = aux->prox) {
    pop = pop + aux->n1 + aux->n2;
  }
  //Repetindo as movimentações até chegar ao numero maximo de simulações ou até a lista de mosquitos estiver vazia.
  for (i = 0; (i < (*simulacao)) && (aedes->inicio != aedes->fim); ) {
    if (verificasurto(cidade, pop) == 1) {
      surtant = 1;
    }
    //Movimentando o mosquito com a função mosquito_move()
    if (mosquito_move(cidade, aedes, &move) == 1) {
      return 1;
    }
    else{
      i++;
      if ((i % *atua) == 0) {
        if(agente_atua(cidade, aedes, NULL) == 0 ){
          acertos++;
        }
      }
      if ((i % *saude) == 0) {
        if(agente_saude(cidade, aedes, NULL) == 0) {
          acertos++;
        }
      }
      mosquito_bota(cidade, aedes, *bota);
    }
  }
  fprintf(saida, "Número de mosquitos iniciais: %d\n", inicial);
  fprintf(saida, "Número de mosquitos finais: %d\n", aedes->num);
  fprintf(saida, "Número de movimentos totais dos mosquitos: %d\n", move);
  fprintf(saida, "Número de erros dos agentes: %d\n", (((i / *atua) + (i / *saude)) - acertos));
  fprintf(saida, "Número de acertos dos agentes: %d\n", acertos);
  if (i == *simulacao) {
    if (verificasurto(cidade, pop) == 1) {
      fprintf(saida, "Estado de emergência");
    }
    else{
      fprintf(saida, "Houve surto mais foi controlado/erradicado");
    }
  }
  else {
    if (surtant == 0) {
      fprintf(saida, "Não houve surto");
    }
    else{
      fprintf(saida, "Houve surto mais foi controlado/erradicado");
    }
  }
  fclose(saida);
  return 0;
}

int mosquito_move (Theader *cidade, Theader *aedes, int *move) {
  Tno *mosquito, *atual = NULL, *destino = NULL;
  int n;
  for (mosquito = aedes->inicio->prox ; mosquito != NULL; mosquito = mosquito->prox) {
    //Encontrando a casa onde o mosquito esta.
    atual = achano(cidade, mosquito->nome);
    //Se não for encontrada a casa onde o mosquito esta, ela retorna erro.
    if (atual == NULL) {
      printf("\nCASA ONDE O MOSQUITO ESTA NAO ENCONTRADA (mosquito_move)\n");
      return 1;
    }
    //Se não haver ligações, o mosquito não pode ser mover e é passado a vez.
    if (atual->baixo->num <= 0) {
      continue;
    }
    n = rand() % (atual->baixo->num);
    if (n < 0) {
      printf("\nERRO, FALHA NO SORTEIO (mosquito_move)\n");
      return 1;
    }
    destino = atual->baixo->inicio;
    int i;
    for (i = 0; i <= n; i++) {
      destino = destino->prox;
    }
    strcpy(mosquito->nome, destino->nome);
    destino = achano(cidade, destino->nome);
    if (destino->n1 != 0) {
      n = destino->n1;
      destino->n1 = 0;
      destino->n2 = n;
    }
    mosquito->n2 += 1;
    *move += 1;
  }
  return 0;
}

int mata_mosquito (Theader *aedes, Tno *mosquito) {
  Tno *aux;
  if (mosquito == NULL || aedes->inicio == aedes->fim) {
    printf("\nERRO NA MANIPULACAO DE PONTEIROS (mata_mosquito)\n");
    return 1;
  }
  for (aux = aedes->inicio; aux->prox != NULL; aux = aux->prox) {
    if (aux->prox == mosquito) {
      aux->prox = mosquito->prox;
      if (mosquito == aedes->fim) {
        aedes->fim = aux;
      }
      free(mosquito);
      aedes->num = aedes->num -1;
      return 0;
    }
  }
  return 1;
}

int agente_atua (Theader *cidade, Theader *aedes, Tno *casax) {
  Tno *destino = cidade->inicio, *alvo = NULL;
  int n,i;
  if (casax == NULL) {
    n = rand() % (cidade->num);
    if (n < 0) {
      printf("\nERRO, FALHA NO SORTEIO (agente_atua)\n");
      return 1;
    }
    for (i = 0; i <= n; i++) {
      destino = destino->prox;
    }
  }
  else {
    destino = casax;
  }
  alvo = achano(aedes, destino->nome);
  if (alvo == NULL) {
    agente_saude(cidade, aedes, destino);
    return 1;
  }
  else {
    while (alvo != NULL) {
      mata_mosquito(aedes, alvo);
      alvo = achano(aedes, destino->nome);
    }
    return 0;
  }
}

int agente_saude(Theader *cidade, Theader *aedes, Tno *casax) {
  Tno *destino = cidade->inicio;
  int n, i;
  if (casax == NULL) {
    n = rand() % (cidade->num);
    if (n < 0) {
      printf("\nERRO, FALHA NO SORTEIO (agente_saude)\n");
      return 1;
    }
    for (i = 0; i <= n; i++) {
      destino = destino->prox;
    }
  }
  else {
    destino = casax;
  }
  if (achano(aedes, destino->nome) != NULL) {
    if (agente_atua(cidade, aedes, destino) == 1){
      return 1;
    }
  }
  if (destino->n2 != 0) {
    n = destino->n2;
    destino->n2 = 0;
    destino->n1 = n;
    return 0;
  }
  return 1;
}

int mosquito_bota (Theader *cidade, Theader *aedes, int bota) {
  if ((cidade == NULL) || (aedes == NULL)) {
    return 1;
  }
  Tno *mosquito;
  for (mosquito = aedes->inicio->prox; mosquito != NULL; mosquito = mosquito->prox) {
    //Se um determinado mosquito se moveu "bota" vezes, a função insere mais dois mosquitos dentro da casa onde ele esta.
    if (((mosquito->n2 % bota) == 0) && (mosquito->n2 != 0)) {
      if (inseremosquito (cidade, aedes, mosquito->nome) == 1) {
        return 1;
      }
      if (inseremosquito (cidade, aedes, mosquito->nome) == 1) {
        return 1;
      }
    }
  }
  return 0;
}

int verificasurto (Theader *cidade, int pop) {
  int doentes = 0;
  Tno *aux = NULL;
  //Contando o numero de doentes da lista.
  for (aux = cidade->inicio->prox; aux != NULL; aux = aux->prox) {
    doentes += aux->n2;
  }
  //A variavel na funcao que recebe o este retorno continuara a ser 0 se não houver momento em que essa condição se satisfaça.
  if (doentes > (pop * 0.8)) {
    return 1;
  }
  return 0;
}

int imprimelistas (Theader *cidade, Theader *mosquito, FILE *arq) {
    Tno *aux1 = cidade->inicio->prox, *aux2 = NULL, *aux3 = mosquito->inicio->prox;
    Theader *cabecaux = NULL;
    //Movendo na lista externa de casas.
    while (aux1 != NULL) {
      cabecaux = aux1->baixo;
      aux2 = cabecaux->inicio->prox;
      if (cabecaux->inicio == cabecaux->fim) {
        fprintf(arq, "%s", aux1->nome);
      }
      else{
        fprintf(arq, "%s->", aux1->nome);
      }
      //Movendo na lista interna de ligacoes.
      while(aux2 != NULL) {
        if(aux2->prox != NULL) {
          fprintf(arq, "%s->", aux2->nome);
        }
        else {
          fprintf(arq, "%s", aux2->nome);
        }
        aux2 = aux2->prox;
      }
      //Pra cada casa, é buscado os mosquito presentes nela percorrendo a lista aedes.
      while(aux3 != NULL){
        if(strcmp(aux3->nome, aux1->nome) == 0) {
          fprintf(arq, "->M");
        }
        aux3 = aux3->prox;
      }
      aux3 = mosquito->inicio->prox;
      fprintf(arq, "\n");
      aux1= aux1->prox;
  }
}

int limpalistas(Theader *cidade, Theader *mosquito){
  Tno *aux1 = NULL;
  Tno *aux2 = NULL;
  //Cabeçalho auxiliar para auxiliar a manipulacao.
  Theader *aux3 = (Theader *) malloc(sizeof(Theader));
  if (aux3 == NULL) {
    return 1;
  }
  //Desalocando a lista externa de cidade.
  while(cidade->inicio!=cidade->fim){
    //Pego o primeiro depois da cabeça.
    aux1 = cidade->inicio->prox;
    //Se ele for o ultimo no, tenho que mover o ponteiro "fim".
    if (aux1 == cidade->fim) {
      cidade->fim = cidade->inicio;
    }
    //Preparando o header auxiliar para receber a lista de ligações.
    aux3 = aux1->baixo;
    //Desalocando a lista interna de ligaçações de cada do no.
    while(aux3->inicio!=aux3->fim){
      //Pego o primeiro depois da cabeça.
      aux2 = aux3->inicio->prox;
      //Se ele for o ultimo no, tenho que mover o ponteiro "fim".
      if (aux2 == aux3->fim) {
        aux3->fim = aux3->inicio;
      }
      //Fazendo o proximo receber o que o ponteiro do no a ser excluido aponta.
      aux3->inicio->prox = aux2->prox;
      //Desalocando a ligação.
      free(aux2);
    }
    //Desalocando a cabeça da lista de ligações.
    free(aux3->inicio);
    //Fazendo o proximo receber o que o ponteiro do no a ser excluido aponta.
    cidade->inicio->prox = aux1->prox;
    //Desalocando a casa;
    free(aux1);
  }
  //Desalocando a cabeça da lista de casa.
  free(cidade->inicio);
  while(mosquito->inicio!=mosquito->fim){
    //Pego o primeiro depois da cabeça.
    aux1 = mosquito->inicio->prox;
    //Se ele for o ultimo no, tenho que mover o ponteiro "fim".
    if (aux1 == mosquito->fim) {
      mosquito->fim = mosquito->inicio;
    }
    //Fazendo o proximo receber o que o ponteiro do no a ser excluido aponta.
    mosquito->inicio->prox = aux1->prox;
    //Desalocando a ligação.
    free(aux1);
  }
  //Desalocando a cabeça da lista de mosquito.
  free(mosquito->inicio);
  return 0;
}
