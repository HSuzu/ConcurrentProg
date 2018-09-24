// compilar: mpicc mpi_v2_master.c -o master
// executar: mpirun -np 1 ./master

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

  int errcodes[10];

  int vk; // valor de K+vet[k]

  int n;    // tamanho do vetor local
  int *vet = NULL;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &npes);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  n = TAM/(P+1);

  int residue = TAM%(P+1);
  if(rank < residue) {
    n++;
  }

  vet = (int *) malloc(n*sizeof(int));

  int *tmp_vec = NULL;

  tmp_vec = (int *)malloc((TAM + 1)*sizeof(int));
  // tmp_vec[0] = K;

  FILE *arquivo_entrada;
  arquivo_entrada = fopen("numeros.txt", "r");

  int i;
  for(i = 0; i < TAM+1; i++) {
    fscanf(arquivo_entrada, "%d", &(tmp_vec[i]));
  }

  fclose(arquivo_entrada);

  vk = tmp_vec[tmp_vec[0] + 1]; // tmp_vec[k+1]

  MPI_Comm_spawn("slave", MPI_ARGV_NULL, P, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &inter_comm, errcodes);

  MPI_Bcast(&vk, 1, MPI_INT, MPI_ROOT, inter_comm);

  int sendcount[P];
  int displs[P];

  for(i = 0; i < P; i++) {
    sendcount[i] = TAM/(P+1);
    displs[i] = sendcount[i]*(i+1);

    int tresidue = TAM%(P+1);
    if(i+1 < tresidue) {
      sendcount[i]++;
      displs[i] += i+1;
    }
     else {
      displs[i] += tresidue;
    }
  }

  int buffermestre[1];
  MPI_Scatterv(&(tmp_vec[1]), sendcount, displs, MPI_INT, buffermestre, 0, MPI_INT, MPI_ROOT, inter_comm);


  /* Copia o vetor do rank == 0 */
  for(i = 0; i < n; i ++) {
    vet[i] = tmp_vec[i+1];
  }

  // CONTA!!!!!

  int contador = 0;
  int *indices = (int *)malloc(sizeof(int));
  int idx = 0;

  for(i = 0; i < n; i++) {
    if(vet[i] > vk) {
      contador++;
      indices = (int *)realloc(indices, contador*(sizeof(int)));
      indices[idx] = i;
      idx++;
    }
  }

  int gatherBuffer[P];
  MPI_Gather(&contador, 1, MPI_INT, gatherBuffer, 1, MPI_INT, MPI_ROOT, inter_comm);


  int contador_total;
  MPI_Reduce(&contador, &contador_total, 1, MPI_INT, MPI_SUM, MPI_ROOT, inter_comm);
  contador_total += contador;



  indices = (int *)realloc(indices, contador_total*(sizeof(int)));

  int gathervDisp[P];
  gathervDisp[0] = 0;
  for(i = 1; i < P; i++) {
    gathervDisp[i] = gathervDisp[i-1] + gatherBuffer[i-1];
  }

  MPI_Gatherv(indices, contador, MPI_INT, &(indices[contador]), gatherBuffer, gathervDisp, MPI_INT, MPI_ROOT, inter_comm);



  FILE *arquivo_saida = fopen("saidav2.txt", "w+");
  fprintf(arquivo_saida, "%d\n", contador_total);
  if(contador_total > 0) {
    for(i = 0; i < contador_total; i++) {
      fprintf(arquivo_saida, "%d ", indices[i]);
    }
  }

  fclose(arquivo_saida);

  free(vet);
  free(indices);

  MPI_Finalize();

  return 0;
}
