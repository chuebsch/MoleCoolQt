#ifndef FOURMCQ_H
#define FOURMCQ_H 1
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <fftw3.h>
#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
typedef struct  {
char  size_head;
int   ih,
      ik,
      il;
float d1,d2,d3,d4,d5,d6,d7;
char  size_tail;
} reco;
typedef struct {
int size_head;
int  ih,
     ik,
     il;
float d1,d2,d3,d4,d5,d6,d7;
int size_tail;
} rec64;
#pragma pack(pop)   /* restore original alignment from stack */
#define LM 2000000

class FourMCQ{
  public:
	  fftwf_complex *B;
          FourMCQ(const char filename[], int kind=1, double resol=5.0, double wght=1.0);
	  ~FourMCQ();
  private:
	  double C[15],D[9],sy[12][192],wave;
	  int n1,n2,n3,n4,n5;
	  fftwf_plan  fwd_plan;
	  rec64 lr[LM];
	  reco wr[LM];
	  char cen,git;
	  int nr,nc,ns,jm;
	  double rr,rw,dx,dy,dz;
char titl[80];/*fcmax=0,f000=0,resmax=99999.0,*/
void trimm(char s[]);
void deletes(char *s, int count);
int readMas(const char *filename);
void sorthkl(int nr, rec64 r[]);
};

#endif
