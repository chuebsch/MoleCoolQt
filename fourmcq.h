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
#include <QObject>
#include <QCheckBox>
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

struct FNode
{
  V3 vertex;
  char flag;

  inline operator char (){
    return flag;
  }
  inline operator V3(){
    return vertex;
  }
  friend inline double Distance( const FNode& n1, const FNode& n2 ){
    return Norm(n1.vertex-n2.vertex);
  }
};
//

class FourMCQ:public QObject{ 
Q_OBJECT
  public:
          float *datfo,*datfo_fc,*datf1_f2;
          FourMCQ(molekul *mole_, CubeGL *chgl_,QToolBar *toolView, double resol=2.5, double wght=1.0);
	  ~FourMCQ();
          bool loadFouAndPerform(const char filename[],bool neu=true);
          bool loadm80AndPerform(const char filename[],bool neu=true);
          double lintrans,linwidth;
          float sigma[3];
          float iso[3];
	  int n1,n2,n3,n4,n5;
          V3 urs;
	  QCheckBox *doMaps;
          double rr,rw;
          int curentPhase; //structure phase id when multiple phases are refined
	  int maptrunc;
          molekul *mole;
	  double map_radius;
	  CubeGL *chgl;
	  void deleteLists();
	  void killmaps();
      void jnk();

  public slots:

          void bewegt(V3 v);
          void inimap();
          void change_iso(int numsteps,int diff);
  signals:
  void bigmessage(const QString &);

  private:
	  double C[15],D[9],sy[12][192],wave,sintl,cral,crbe,crga;
	  inline int Intersect( double& vm, double& vp ){ return vm*vp <= 0.0 && (vm<0.0 || vp<0.0); }
	  V3  delDA[27];
	  fftwf_plan  fwd_plan;
          fftwf_complex *B;
          FNode *nodex,*nodey,*nodez;
	  rec64 lr[LM];
	  reco wr[LM];
	  char cen,git;
          int nr,nc,ns;
	  int mtyp;
	  V3 dx,dy,dz;
          void gen_surface(bool neu,int imin=0, int imax=5);
          void CalcVertex( int ix, int iy, int iz);
          int IndexSelected( FNode& node0, FNode& node1, FNode& node2, FNode& node3 );
          V3& VectorSelected( FNode& node0, FNode& node1, FNode& node2, FNode& node3 );
          void MakeElement( int ix, int iy, int iz ,int s1, int s2);
          void makeFaces(int n, FNode poly[] );

char titl[80];/*fcmax=0,f000=0,resmax=99999.0,*/
void trimm(char s[]);
void decodeSymm(QString symmCard);
void decodeSymm2(QString symmCard);
void deletes(char *s, int count);
int readMas(const char *filename);
void sorthkl(int nr, rec64 r[]);
int dex(int x,int y, int z);
};

#endif
