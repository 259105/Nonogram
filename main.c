//
// Created by s259105 on 20/02/2020.
//
#include "Nonogram.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc,char** argv) {
    if(argc!=4) exit(1);
    FILE *fN,*fs,*fO;
    fN=fopen(argv[1],"r");
    fs=fopen(argv[2],"r");
    fO=fopen(argv[3],"w");
    if(fN==NULL || fs==NULL) exit(2);
    Nonogram N=NONOGRAMload(fN);
    /*
    int ***sol=SOLload(N,fs);

    if(sol!=NULL && NONOGRAMcheck(N,sol[0]))printf("E' una soluzione corretta\n");
    else printf("Non e' una soluzione corretta\n");
    if(sol!=NULL) SOLfree(N,sol);
*/
    NONOGRAMfindstoragesol(N,fO);

    NONOGRAMfree(N);
    fclose(fN); fclose(fs);
    return 0;
}
