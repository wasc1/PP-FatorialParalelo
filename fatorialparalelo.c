//Materia: Processamento Paralelo
//Aluno: Willian Augusto
//Obj: Paralelizar fatorial; Junção MPI/GMP; Medição tempo;
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "mpi.h"

int main(int argc, char *argv[]){

        mpf_t valor,count,tam,carga_no,aux_size,idx;
        mpf_inits(valor,count,tam,carga_no,aux_size,idx,NULL);
        mpf_set_ui(valor,1);

        int ret,rank,size=3,status,tag=100;
        double tempo;

        FILE *in,*out;

        in=fopen("/home/mpiboca/servidor/WillianAugusto/trabalho1/entrada.txt","r");
        //out=fopen("/home/mpiboca/servidor/WillianAugusto/trabalho1/saida.txt","w");

        if( in == NULL ){
                printf("\nProblemas em abrir o arquivo de ENTRADA, verifique-o e tente novamente...\n");
                exit(1);
        }
        //if( out == NULL ){
        //        printf("\nProblemas em abrir o arquivo de SAIDA, verifique-o e tente novamente...\n");
        //        exit(1);
        //}

        status = gmp_fscanf(in,"%Ff\n",tam);
        if(status == EOF) exit(1);
        if(status != 1){
                fprintf(stderr,"\nEntrada Invalida.\n");
                exit(1);
        }
        gmp_printf("\nValor lido em disco: %Ff\n",tam);


        ret = MPI_Init(&argc, &argv);
        ret = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        ret = MPI_Comm_size(MPI_COMM_WORLD, &size);

        mpf_set_ui(aux_size,size);
        mpf_div(carga_no,tam,aux_size);
        gmp_printf("Carga para cada no: %Ff\n", carga_no);

        tempo = MPI_Wtime();
        MPI_Bcast(&carga_no,1,MPI_DOUBLE,0,MPI_COMM_WORLD);

        if(rank){
                for( mpf_set_ui(idx,1); mpf_cmp(idx,carga_no)<=0; mpf_add_ui(idx,idx,1) ){
                        mpf_mul(valor,valor,idx);
                }
        }

        MPI_Reduce(&valor,&count,1,MPI_DOUBLE,MPI_PROD,0,MPI_COMM_WORLD);
        tempo = MPI_Wtime()-tempo;

        if(rank == 0){
                gmp_printf("\nEntrada: %Ff\nFatorial: %Ff\nTempo Total: %f\n",tam,valor,tempo);
        }

        ret = MPI_Finalize();

        fclose(in);
        //fclose(out);
        mpf_clears(valor,count,tam,NULL);

return 0;
}
