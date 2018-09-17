#include<stdio.h>
#include<stdlib.h>

#define TAM 4000000

int main(int argc,char **argv){
    if(argc!=2){
        printf("Informe apenas o valor de k");
        exit(1);
    }
    FILE *arquivo_entrada,*arquivo_saida;
    arquivo_entrada=fopen("numeros.txt","r");
    arquivo_saida=fopen("saida.txt","w+");
    int k=atoi(argv[1]);
    if(k<0 || k>(TAM-1)){
        printf("Elemento fora do indice do vetor");
        exit(1);
    }
    int contador=0;
    int elemento=0;
    int *vet=((int*)malloc(TAM*sizeof(int)));
    int *indices=(int *)malloc(sizeof(int));
    int i,j=0;
    for(i=0;i<TAM;i++){
        fscanf(arquivo_entrada, "%d,", &vet[i]);
    }
    elemento=vet[k];
    for(i=0;i<TAM;i++)
    {
        if(vet[i]>elemento){
            contador++;
            indices=(int*)realloc(indices,contador*sizeof(int));
            indices[j]=i;
            j++;
        }
    }
    fprintf(arquivo_saida, "%d\n", contador);
    if(contador>0){
        for(i=0;i<contador;i++){
            fprintf(arquivo_saida,"%d ",indices[i]);
        }
    }
    fclose(arquivo_entrada);
    fclose(arquivo_saida);
    free(vet);
    free(indices);
}