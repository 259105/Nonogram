//
// Created by s259105 on 20/02/2020.
//
#include "Nonogram.h"

struct nonogram_{
    int *N;//0:= n righe 1:=n colonne
    int ***rc; //valori
    int **n;//dimensioni
};
typedef struct node *link;
struct node{
    int *d;
    int n;
    link next;
};
typedef struct lista_ *Lista;
struct lista_{
    link h,z;
    int N;
};

static link NEW(int *d,int n,link next){
    link t=malloc(sizeof(*t));
    t->n=n;
    t->d=malloc(sizeof(int)*n);
    for(int i=0;i<n;i++)
        t->d[i]=d[i];
    t->next=next;
    return t;
}
static Lista LISTAinit(){
    Lista t=malloc(sizeof(*t));
    t->h=t->z=NEW(NULL,0,NULL);
    t->N=0;
    return t;
}
static void freenode(link h,link z){
    if(h==z) return;
    freenode(h->next,z);
    free(h->d);
    free(h);
}
static void LISTAfree(Lista L){
    freenode(L->h,L->z);
    free(L->z);
    free(L);
}

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
int*** SOLload(Nonogram N,FILE *fin){
    int ***sol;

    sol=malloc(sizeof(int**)*2);
    for(int i=0;i<2;i++){
        sol[i]=malloc(N->N[0] *sizeof(int*));
        for(int j=0;j<N->N[0];j++)
            sol[i][j]=calloc(N->N[1], sizeof(int));
    }

    for(int i=0;i<N->N[0];i++){
        for(int j=0;j<N->N[1];j++){
            if(fscanf(fin,"%d",&sol[0][i][j])!=1) return NULL;
        }
    }
    return sol;
}
void SOLfree(Nonogram N,int ***sol){
    for(int i=0;i<2;i++){
        for(int j=0;j<N->N[0];j++)
            free(sol[i][j]);
        free(sol[i]);
    }

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
            if(S[i][j] && k && k==N->n[0][i]) no=1;//caso in cui la soluzione ha la casella nera ma nel nonogram non è prevesto alcun intervallo
            if(S[i][j]) rc[0][i][k]++; //caso in cui la soluzione ha la casella nera e nel nonogram è previsto che stia in un intervallo
            if(!S[i][j] && !rc[0][i][k]) continue; // caso di una casella vuota tra 2 intervalli
            if(!S[i][j]) k++; //caso in cui si ha una casella vuota subito dopo una nera, si segna l'incremento dell'intervallo successivo
            if(k>=N->n[0][i]) break;
            if(no) break;
        }
        if(no)break;
    }
    //casistica uguale ma fatto per le colonne
    for(int j=0;j<N->N[1] && no!=1;j++){
        for(int i=0,k=0;i<N->N[0];i++){
            if(S[i][j] && k && k==N->n[1][j]) no=1;
            if(S[i][j]) rc[1][j][k]++;
            if(!S[i][j] && !rc[1][j][k]) continue;
            if(!S[i][j]) k++;
            if(k>=N->n[1][j]) break;
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
static int finddim(int *vk,int nk,int N){
    int count=0;
    for(int i=0;i<nk;i++)
        count+=vk[i];
    return nk+(N-count);
}
static int** buildsol(Nonogram N,link *sol,int scelta){
    //faccio la copia di sol
    int **solcpy=malloc(sizeof(int*)*N->N[scelta]);
    for(int i=0;i<N->N[scelta];i++){
        solcpy[i]=malloc(sizeof(int)*sol[i]->n);
        for(int j=0;j<sol[i]->n;j++)
            solcpy[i][j]=sol[i]->d[j];
    }
    //alloco la matrice della soluzione, la free va fatta fuori dalla funzione
    int **S=malloc(sizeof(int*)*N->N[0]);
    for(int i=0;i<N->N[0];i++)
        S[i]=malloc(sizeof(int)*N->N[1]);
    //riepio S con la soluzione trovata
    for(int i=0;i<N->N[scelta];i++){
        for(int j=0,k=0;j<N->N[(scelta+1)%2];j++){
            if(solcpy[i][k]){
                solcpy[i][k]--;
                if(!scelta) S[i][j]=1; //sulle righe
                if(scelta) S[j][i]=1; //sulle colonne
                if(!solcpy[i][k]) k++;
            }
            else {
                if(!scelta) S[i][j]=0; //sulle righe
                if(scelta) S[j][i]=0; //sulle colonne
                k++;
            }
        }
    }
    //free di solcpy
    for(int i=0;i<N->N[scelta];i++)
        free(solcpy[i]);
    free(solcpy);
    return S;
}
/*int checksol(Nonogram N,link *sol,int scelta){
    //alloco la matrice della soluzione
    int **S=buildsol(N,sol,scelta);
    //controllo se è un nonogram
    int result;
    result=NONOGRAMcheck(N,S);
    //free di S
    for(int i=0;i<N->N[0];i++)
        free(S[i]);
    free(S);
    //ritorno risultato
    return result;
}*/
static int checksolparz(Nonogram N,link *sol,int pos,int scelta){
    //faccio la copia di sol
    int **solcpy=malloc(sizeof(int*)*pos);
    for(int i=0;i<pos;i++){
        solcpy[i]=malloc(sizeof(int)*sol[i]->n);
        for(int j=0;j<sol[i]->n;j++)
            solcpy[i][j]=sol[i]->d[j];
    }
    //alloco matrice parziale
    int **sol_parz=malloc(sizeof(int*)*pos);
    for(int i=0;i<pos;i++){
        sol_parz[i]=malloc(sizeof(int)*N->N[(scelta+1)%2]);
        for(int j=0;j<N->N[(scelta+1)%2];j++)
            sol_parz[i][j]=sol[i]->d[j];
    }
    //riempio la matrice parziale
    for(int i=0;i<pos;i++)
        for(int j=0,k=0;j<N->N[(scelta+1)%2];j++){
            if(solcpy[i][k]){
                solcpy[i][k]--;
                sol_parz[i][j]=1;
                if(!solcpy[i][k]) k++;
            }
            else {
                sol_parz[i][j]=0;
                k++;
            }
        }
    //free solcpy
    for(int i=0;i<pos;i++)
        free(solcpy[i]);
    free(solcpy);
    //creo vettore Vk degli indizi della soluzione parziale
    int **vk=malloc(sizeof(int*)*N->N[(scelta+1)%2]);
    for(int i=0;i<N->N[(scelta+1)%2];i++)
        vk[i]=calloc(N->n[(scelta+1)%2][i],sizeof(int));
    //riempio vettore vk con indizi della soluzione parziale e confronto uguaglianza con nonogram
    int no=0;
    for(int i=0;i<N->N[(scelta+1)%2];i++){
        int k=0;
        for(int j=0;j<pos;j++){
            if(k!=N->n[(scelta+1)%2][i] && sol_parz[j][i]) vk[i][k]++;
            if(k!=N->n[(scelta+1)%2][i] && !sol_parz[j][i] && !vk[i][k]) continue;
            if(k!=N->n[(scelta+1)%2][i] && !sol_parz[j][i]) k++;
        }
        if(sol_parz[pos-1][i]) k++;
        if(k>N->n[(scelta+1)%2][i]) {no=1;break;}
        for(int j=0;j<k && !no;j++)
            if(vk[i][j]>N->rc[(scelta+1)%2][i][j]){
                no=1;
                break;
            }
        if(no) break;
    }
    //Free
    for(int i=0;i<N->N[(scelta+1)%2];i++)
        free(vk[i]);
    free(vk);
    for(int i=0;i<pos;i++)
        free(sol_parz[i]);
    free(sol_parz);
    if(no) return 0;
    return 1;
}
static int findsolR(Nonogram N,Lista *D,link *sol,int pos,int scelta){
    int c=1;
    if(pos==N->N[scelta]){
        /*
        if(checksol(N,sol,scelta))
            return 1;
        return 0;*/
        return 1;
    }
    for(link x=D[pos]->h;x!=D[pos]->z;x=x->next){ //principio di moltiplicazione
        sol[pos]=x; //faccio la copia per riferiemento
        if(pos)
            c=checksolparz(N,sol,pos+1,scelta);
        if(!pos || c)
            if(findsolR(N,D,sol,pos+1,scelta))
                return 1;
    }
    return 0;
}
static int checkDisp(int *vk,int nk,int *sol,int n){
    int k,i;
    for(i=0,k=0;i<n;i++){
        if(!sol[i]) continue;
        if(sol[i]!=vk[k]) return 0;
        if(i && sol[i-1]) return 0;
        k++;
    }
    if(k!=nk) return 0;
    return 1;
}
static void GenDispR(int *vk_eff,int nk_eff,int *vk,int nk,Lista d,int *sol,int n,int pos){
    if(pos==n){
        if(checkDisp(vk,nk,sol,n)){
            d->h=NEW(sol,n,d->h);//inserimento in testa
            d->N++;
            /*for(int i=0;i<n;i++)
                printf("%d ",sol[i]);
            printf("\n");*/
        }
        return;
    }
    for(int i=0;i<nk_eff;i++){ //combinazioni ripetute
        sol[pos]=vk_eff[i];
        GenDispR(vk_eff,nk_eff,vk,nk,d,sol,n,pos+1);
        sol[pos]=-1;
    }
}
static void GenDispW(Nonogram N,Lista **D,int i,int j){
    int n=finddim(N->rc[i][j],N->n[i][j],N->N[(i+1)%2]);
    int *sol=malloc(n*sizeof(int));
    int *vk=malloc(sizeof(int)*(N->n[i][j]+1)),*mark=calloc(N->N[(i+1)%2]+1,sizeof(int)),n_eff=1;
    vk[0]=0; mark[0]++;
    for(int x=0;x<N->n[i][j];x++)
        if(!mark[N->rc[i][j][x]]){
            mark[N->rc[i][j][x]]++;
            vk[n_eff++]=N->rc[i][j][x];
        }
    free(mark);
    vk=realloc(vk,sizeof(int)*n_eff);
    GenDispR(vk,n_eff,N->rc[i][j],N->n[i][j],D[i][j],sol,n,0);
    free(vk);
    free(sol);
}
static int GenDisp(Nonogram N,Lista **D){
    //Genero le disposizioni
    for(int i=0;i<2;i++)
        for(int j=0;j<N->N[i];j++)
            GenDispW(N,D,i,j);
    //conto le disposizini
    int cont[2]={0,0};
    for(int i=0;i<2;i++){
        for(int j=0;j<N->N[i];j++){
            cont[i]+=D[i][j]->N;
        }
    }
    //scelgo quella con meno disposizioni
    if(cont[0]>cont[1]) return 1;
    return 0;
}
void NONOGRAMfindstoragesol(Nonogram N,FILE *fout){
    int f,scelta;
    //allocazione della struttura che contiene le disposizioni
    Lista **disp=malloc(sizeof(Lista*)*2);
    for(int i=0;i<2;i++)
        disp[i]=malloc(sizeof(Lista)*N->N[i]);
    //inizializzazione
    for(int i=0;i<2;i++)
        for(int j=0;j<N->N[i];j++)
            disp[i][j]=LISTAinit();
    //Generazioni disposizioni ritorna se è meglio iniziare dalle righe (0) o dalle colonne(1)
    scelta=GenDisp(N,disp);
    //Verificare se è meglio farla per righe o per colonne in base al numero di disposizioni totale
    //allocazione della matrice che contiene la soluzione in base alla scelta precedente
    link *sol=malloc(sizeof(link)*N->N[scelta]);
    f=findsolR(N,disp[scelta],sol,0,scelta);
    if(f){
        //ricostruisco soluzione
        int **S=buildsol(N,sol,scelta);
        //stampa
        printf("Soluzione trovata.\nSalvataggio su file in corso...\n");
        for(int i=0;i<N->N[0];i++){
            for(int j=0;j<N->N[1];j++){
                fprintf(fout,"%d ",S[i][j]);
            }
            fprintf(fout,"\n");
        }
        //free
        for(int i=0;i<N->N[0];i++)
            free(S[i]);
        free(S);
        printf("Soluzione salvata con successo.\n");
    }else{
        printf("Soluzione non trovata.\n");
    }
    //free
    for(int i=0;i<2;i++){
        for(int j=0;j<N->N[i];j++)
            LISTAfree(disp[i][j]);
        free(disp[i]);
    }
    free(disp);
    free(sol);
}
