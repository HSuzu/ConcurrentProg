// compilar: mpicc mpi_v2_slave.c -o slave
// executar: -

/** Grupo 6
 ** Estevam Arantes   - 9763105
 ** Henrique Andrews  - 9771463
 ** Henry Suzukawa    - 9771504
 ** Leonardo Daher    - 9771682
 **/

 /** Reestruturação do código Time 6
  ** Leonardo Daher   - 9771682
  ** Henry Suzukawa   - 9771504
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
#define P   7

#define K   10

int main(int argc, char *argv[]) {
  int npes; // números de processos
  int rank; // rank do processo

  MPI_Comm inter_comm;

  int vk; // valor de K+vet[k]

  int n;    // tamanho do vetor local
  int offset; // offset no vetor original
  int *vet = NULL;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &npes);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Comm_get_parent(&inter_comm);

  n = TAM/(npes+1);
  offset = n*(rank+1);

  int residue = TAM%(npes+1);
  if((rank+1) < residue) {
    n++;
    offset += (rank+1);
  }
   else {
    offset += residue;
  }

  vet = (int *)malloc(n*sizeof(int));

  MPI_Bcast(&vk, 1, MPI_INT, 0, inter_comm);

  int *sendBuffer, *displs, *sendcount;
  MPI_Scatterv(sendBuffer, sendcount, displs, MPI_INT, vet, n, MPI_INT, 0, inter_comm);


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

  int gatherBuffer[P];
  MPI_Gather(&contador, 1, MPI_INT, gatherBuffer, 1, MPI_INT, 0, inter_comm);

  int contador_total;
  MPI_Reduce(&contador, &contador_total, 1, MPI_INT, MPI_SUM, 0, inter_comm);

  int *gathervDisp;
  MPI_Gatherv(indices, contador, MPI_INT, &(indices[contador]), gatherBuffer, gathervDisp, MPI_INT, 0, inter_comm);


  free(indices);
  free(vet);

  MPI_Finalize();

  return 0;
}
