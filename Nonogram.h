//
// Created by s259105 on 20/02/2020.
//

#ifndef NONOGRAM_H
#define NONOGRAM_H
#include <stdio.h>
#include <stdlib.h>
typedef struct nonogram_ *Nonogram;

Nonogram NONOGRAMload(FILE *fin);
void     NONOGRAMfree(Nonogram N);
int***   SOLload(Nonogram N,FILE *fin);
void     SOLfree(Nonogram N,int ***sol);
int      NONOGRAMcheck(Nonogram N,int **S);
void     NONOGRAMfindstoragesol(Nonogram N,FILE *fout);

#endif //NONOGRAM_H
