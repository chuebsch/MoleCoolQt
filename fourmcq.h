#ifndef FOURMCQ_H
#define FOURMCQ_H 1
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <fftw3.h>
#include "molekul.h"
#include "cubeGL.h"
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
          float *datfo,*datfo_fc,*datf1_f2;
          FourMCQ(int kind=1, double resol=5.0, double wght=1.0);
	  ~FourMCQ();
          bool loadFouAndPerform(const char filename[]);
          void bewegt(V3 v);
          void inimap();
          float sigma[3];
          float iso[3];
          GLint basemap;
          V3 urs;
          molekul *mole;
  private:
	  double C[15],D[9],sy[12][192],wave;
	  int n1,n2,n3,n4,n5;
	  fftwf_plan  fwd_plan;
          fftwf_complex *B;
	  rec64 lr[LM];
	  reco wr[LM];
	  char cen,git;
	  int nr,nc,ns,jm;
          double rr,rw,DX,DY,DZ;
          void gen_surface();
          void CalcVertex( int ix, int iy, int iz);
          int IndexSelected( Node& node0, Node& node1, Node& node2, Node& node3 );
          V3& VectorSelected( Node& node0, Node& node1, Node& node2, Node& node3 );
          void MakeElement( int ix, int iy, int iz ,int s1, int s2);
          void makeFaces(int n, Node poly[] );

char titl[80];/*fcmax=0,f000=0,resmax=99999.0,*/
void trimm(char s[]);
void deletes(char *s, int count);
int readMas(const char *filename);
void sorthkl(int nr, rec64 r[]);
};

#endif
