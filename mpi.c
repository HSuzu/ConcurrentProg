/** Grupo 6
 ** Estevam Arantes   - 9763105
 ** Henrique Andrews  - 9771463
 ** Henry Suzukawa    - 9771504
 ** Leonardo Daher    - 9771682
 **/

 /*
Particionamento: 1 task para cada elemento do vetor
Comunicação: Uma tarefa irá informar se seu elemento é maior ou não que v[k]
Aglomeração: Um processo receberá n elementos do vetor
Mapeamento:  distribuição de carga igual para todos processadores
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAM 4000000

int main(int argc, char *argv[]) {
  int npes; // números de processos
  int rank; // rank do processo
  int k;    // posição do valor a ser comparado

  int vk; // valor de vet[k]

  int n;    // tamanho do vetor local
  int offset; // offset no vetor original
  int *vet = NULL;

  MPI_Init(&argc, &argv);

  if(argc != 2) {
    printf("Informe apenas o valor de k");
    exit(1);
  }

  MPI_Comm_size(MPI_COMM_WORLD, &npes);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  k = atoi(argv[1]);
  if(k < 0 || k > (TAM-1)) {
    printf("Elemento fora do indice do vetor.\n");
    exit(1);
  }

  n = TAM/npes;
  offset = n*rank;

  int residue = TAM%npes;
  if(rank < residue) {
    n++;
    offset += rank;
  }
   else {
    offset += residue;
  }

  printf("rank %d, n %d, offset %d\n", rank, n, offset);

  vet = (int *)malloc(n*sizeof(int));
  int *tmp_vec = NULL;

  // Envio do vetor para o resto dos processos
  MPI_Request req_vec[npes], req_vk[npes];

  // communicação
  if(rank == 0) {
    tmp_vec = (int *) malloc(TAM*sizeof(int));

    FILE *arquivo_entrada;
    arquivo_entrada = fopen("numeros.txt", "r");

    int i;
    for(i = 0; i < TAM; i++) {
      fscanf(arquivo_entrada, "%d", &(tmp_vec[i]));
    }

    fclose(arquivo_entrada);

    /* Copia o vetor do rank == 0 */
    for(i = 0; i < n; i ++) {
      vet[i] = tmp_vec[i];
    }

    vk = tmp_vec[k];

    for(i = 1; i < npes; i++) {
      int rn = TAM/npes;
      int roffset = n*i;

      int residue = TAM%npes;
      if(i < residue) {
        rn++;
        roffset += i;
      } else {
        roffset += residue;
      }

      MPI_Isend(&(tmp_vec[roffset]), rn, MPI_INT, i, 0, MPI_COMM_WORLD, &(req_vec[i]));
      MPI_Isend(&(vk), 1, MPI_INT, i, 1, MPI_COMM_WORLD, &(req_vk[i]));
    }

  } else {
    MPI_Status status_vec, status_vk;

    MPI_Recv(vet, n, MPI_INT, 0, 0, MPI_COMM_WORLD, &(status_vec));
    MPI_Recv(&vk, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &(status_vk));


  }

  // CONTA!!!!!

  int contador = 0;
  int *indices = (int *)malloc(sizeof(int));
  int idx = 0;

  int i;
  for(i = 0; i < n; i++) {
    if(vet[i] > vk) {
      contador++;
      indices = (int *)realloc(indices, contador*(sizeof(int)));
      indices[idx] = i + offset;
      idx++;
    }
  }

  // JUNTAR as contas
  if(rank != 0) {
    MPI_Send(&contador, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    MPI_Send(indices, contador, MPI_INT, 0, 3, MPI_COMM_WORLD);

    free(indices);
  } else {
    int tmp_cont = 0;
    MPI_Status status;

    int idx = contador;

    int i;
    for(i = 1; i < npes; i++) {
      MPI_Recv(&tmp_cont, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);

      contador += tmp_cont;

      printf("Contador: %d\n", contador);

      indices = (int *)realloc(indices, contador*(sizeof(int)));
      MPI_Recv(&(indices[idx]), tmp_cont, MPI_INT, i, 3, MPI_COMM_WORLD, &status);


      idx += tmp_cont;
    }
  }


  // finalização
  if(rank == 0) {
    int i;
    for(i = 1; i < npes; i++) {
      MPI_Status status_vec;
      MPI_Wait(&(req_vec[i]), &status_vec);
    }

    free(tmp_vec);

    FILE *arquivo_saida = fopen("saida.txt", "w+");
    fprintf(arquivo_saida, "%d", contador);
    if(contador > 0) {
      for(i = 0; i < contador; i++) {
        fprintf(arquivo_saida, "%d ", indices[i]);
      }
    }

    fclose(arquivo_saida);

    free(indices);
  }

  free(vet);


  MPI_Finalize();

  return 0;
}
