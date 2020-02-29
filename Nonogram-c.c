//
// Created by s259105 on 20/02/2020.
//
#include "Nonogram.h"

struct nonogram_{
    int *N;//0:= n righe 1:=n colonne
    int ***rc; //valori
    int **n;//dimensioni
};

Nonogram NONOGRAMload(FILE *fin){
    int n,ni;
    Nonogram t=malloc(sizeof(*t));
    t->N=malloc(sizeof(int)*2);
    t->rc=malloc(sizeof(int**)*2);
    t->n=malloc(sizeof(int*)*2);
    for(int i=0;i<2;i++){
        fscanf(fin,"%d",&n);
        t->N[i]=n;
        t->rc[i]=malloc(sizeof(int*)*n);
        t->n[i]=calloc(n, sizeof(int));
        for(int j=0;j<n;j++){
            fscanf(fin,"%d",&ni);
            t->n[i][j]=ni;
            t->rc[i][j]=calloc(ni, sizeof(int));
            for(int k=0;k<ni;k++)
                fscanf(fin,"%d",&t->rc[i][j][k]);
        }
    }
    return t;
}
void NONOGRAMfree(Nonogram N){
    for(int i=0;i<2;i++){
        for(int j=0;j<N->N[i];j++)
            free(N->rc[i][j]);
        free(N->rc[i]);
        free(N->n[i]);
    }
    free(N->rc);
    free(N->n);
    free(N->N);
    free(N);
}
int** SOLload(Nonogram N,FILE *fin){
    int **sol=malloc(sizeof(int*)*N->N[0]);
    for(int i=0;i<N->N[0];i++){
        sol[i]=malloc(N->N[1]*sizeof(int));
        for(int j=0;j<N->N[1];j++){
            if(fscanf(fin,"%d",&sol[i][j])!=1) return NULL;
        }
    }
    return sol;
}
void SOLfree(Nonogram N,int **sol){
    for(int i=0;i<N->N[0];i++)
        free(sol[i]);
    free(sol);
}
int NONOGRAMcheck(Nonogram N,int **S){
    int ***rc,no=0;
    rc=malloc(sizeof(int**)*2);
    //allocazione
    for(int i=0;i<2;i++){
        rc[i]=malloc(sizeof(int*)*N->N[i]);
        for(int j=0;j<N->N[i];j++)
            rc[i][j]=calloc(N->n[i][j], sizeof(int));
    }
    //creazione Nonogram della soluzione
    for(int i=0;i<N->N[0];i++){
        for(int j=0,k=0;j<N->N[1];j++){
            if(S[i][j] && k==N->n[0][i]) no=1;//caso in cui la soluzione ha la casella nera ma nel nonogram non è prevesto alcun intervallo
            if(S[i][j]) rc[0][i][k]++; //caso in cui la soluzione ha la casella nera e nel nonogram è previsto che stia in un intervallo
            if(!S[i][j] && !rc[0][i][k]) continue; // caso di una casella vuota tra 2 intervalli
            if(!S[i][j]) k++; //caso in cui si ha una casella vuota subito dopo una nera, si segna l'incremento dell'intervallo successivo
            if(no) break;
        }
        if(no)break;
    }
    //casistica uguale ma fatto per le colonne
    for(int j=0;j<N->N[1] && no!=1;j++){
        for(int i=0,k=0;i<N->N[0];i++){
            if(S[i][j] && k==N->n[1][j]) no=1;
            if(S[i][j]) rc[1][j][k]++;
            if(!S[i][j] && !rc[1][j][k]) continue;
            if(!S[i][j]) k++;
            if(no) break;
        }
        if(no) break;
    }
    //controllo uguaianza con Nonogram
    for(int i=0;i<2 && no!=1;i++){
        for(int j=0;j<N->N[i];j++){
            for(int k=0;k<N->n[i][j];k++){
                if(rc[i][j][k]!=N->rc[i][j][k]){
                    no=1;
                }
                if(no) break;
            }
            if(no) break;
        }
        if(no) break;
    }
    //Free
    for(int i=0;i<2;i++){
        for(int j=0;j<N->N[i];j++)
            free(rc[i][j]);
        free(rc[i]);
    }
    free(rc);
    if(no)
        return 0;
    return 1;
}
/*static int pruning(Nonogram N, int** messi, int **M){
    int no=0;

    for(int i=0;i<2;i++)
        for(int j=0;j<N->N[i];j++)
            if(messi[i][j]>=M[i][j])
                no=1;

    if(no)
        return 0;
    return 1;
}*/
static int findsolR(Nonogram N,int **sol,int pos,int **messi,int **M){
    int r,c;
    if(pos==N->N[0]*N->N[1]){
        if(NONOGRAMcheck(N,sol))
            return 1;
        return 0;
    }
    r=pos/N->N[1]; c=pos%N->N[1];
    for(int i=0;i<2;i++){
        sol[r][c]=i;
        if(!i && findsolR(N,sol,pos+1,messi,M))
            return 1;
        else if(i && messi[0][r]<M[0][r] && messi[1][c]<M[1][c]){
            messi[0][r]++;
            messi[1][c]++;
            if(findsolR(N,sol,pos+1,messi,M))
                return 1;
            messi[0][r]--;
            messi[1][c]--;
        }
    }
    return 0;
}

void NONOGRAMfindstoragesol(Nonogram N,FILE *fout){
    int f;

    int **messi=malloc(sizeof(int*)*2);
    for(int i=0;i<2;i++)
        messi[i]=calloc(N->N[i], sizeof(int));

    int **sol=malloc(sizeof(int*)*N->N[0]);
    for(int i=0;i<N->N[0];i++)
        sol[i]=calloc(N->N[1], sizeof(int));

    int **M=malloc(sizeof(int*)*2);
    for(int i=0;i<2;i++)
        M[i]=calloc(N->N[i],sizeof(int));
    for(int i=0;i<2;i++)
        for(int j=0;j<N->N[i];j++)
            for(int k=0;k<N->n[i][j];k++)
                M[i][j]+=N->rc[i][j][k];
    f=findsolR(N,sol,0,messi,M);
    //stampa
    if(f)
        for(int i=0;i<N->N[0];i++){
            for(int j=0;j<N->N[1];j++){
                fprintf(fout,"%d ",sol[i][j]);
            }
            fprintf(fout,"\n");
        }
    else
        printf("Soluzione non trovata.\n");

    for(int i=0;i<2;i++)
        free(messi[i]);
    free(messi);

    for(int i=0;i<2;i++)
        free(M[i]);
    free(M);

    SOLfree(N,sol);
}
