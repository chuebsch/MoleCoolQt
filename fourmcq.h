#ifndef FOURMCQ_H
#define FOURMCQ_H 1
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
//#include <fftw3.h>
#include "kissfft/kiss_fftnd.h"
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
typedef struct {
long int size_head;
int  ih,
     ik,
     il;
float d1,d2,d3,d4,d5,d6,d7;
long int size_tail;
} rec128;
typedef struct {
    int h,k,l,m,n,o;
    float fo,sig,fc,fca,fcb,phase;
}fcf6d;
typedef struct {
  float m[6][6];
  float t[6];
}Mat6;

typedef struct {
  float v[6];

}V6;
inline float operator * (const V6 &a, const V6 &b){
  float erg=0.0f;
  for (int i=0; i<6; i++) erg+=a.v[i]*b.v[i];
  return erg;
}
inline float operator % (const Mat6 &a, const V6 &b){
  float erg=0.0f;
  for (int i=0; i<6; i++) erg+=a.t[i]*b.v[i];
  return erg;
}
inline V6 operator * (const Mat6 &a, const V6 &b){
  V6 erg;
  erg.v[0] = a.m[0][0]*b.v[0] + a.m[1][0]*b.v[1] + a.m[2][0]*b.v[2]+ a.m[3][0]*b.v[3]+ a.m[4][0]*b.v[4]+ a.m[5][0]*b.v[5];
  erg.v[1] = a.m[0][1]*b.v[0] + a.m[1][1]*b.v[1] + a.m[2][1]*b.v[2]+ a.m[3][1]*b.v[3]+ a.m[4][1]*b.v[4]+ a.m[5][1]*b.v[5];
  erg.v[2] = a.m[0][2]*b.v[0] + a.m[1][2]*b.v[1] + a.m[2][2]*b.v[2]+ a.m[3][2]*b.v[3]+ a.m[4][2]*b.v[4]+ a.m[5][2]*b.v[5];
  erg.v[3] = a.m[0][3]*b.v[0] + a.m[1][3]*b.v[1] + a.m[2][3]*b.v[2]+ a.m[3][3]*b.v[3]+ a.m[4][3]*b.v[4]+ a.m[5][3]*b.v[5];   
  erg.v[4] = a.m[0][4]*b.v[0] + a.m[1][4]*b.v[1] + a.m[2][4]*b.v[2]+ a.m[3][4]*b.v[3]+ a.m[4][4]*b.v[4]+ a.m[5][4]*b.v[5];  
  erg.v[5] = a.m[0][5]*b.v[0] + a.m[1][5]*b.v[1] + a.m[2][5]*b.v[2]+ a.m[3][5]*b.v[3]+ a.m[4][5]*b.v[4]+ a.m[5][5]*b.v[5];  
  return erg;
}
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
          QString voxelstr;
          float *datfo,*datfo_fc,*datf1_f2,*datfo_f2;
          float* datfo6;
          float* datfo_fc6;
          FourMCQ(molekul *mole_, CubeGL *chgl_,QToolBar *toolView, double resol=2.5, double wght=1.0);
	  ~FourMCQ();
          bool loadFouAndPerform(const char filename[],bool neu=true,int maxmap=3);
          bool loadm80AndPerform(const char filename[],bool neu=true);
          bool loadDimensionm80AndPerform(const char filename[],bool neu=true);
          void map4dto3d(double t, V3 q, const int n[4],const float *D, float *B);
          void temp(INP atom, int h, int k, int  l, double &eij, double &TA, double &TB);
          void PDFbyFFT(int i, int options=14,double proba=50.0);
          void exportMaps(int na, const char filename[], const char atomlist[]);
          double aborp(double max,double v);
          double proba(double max,double v);
          double lintrans,linwidth;
          float sigma[3];
          float iso[3];
      int n1,n2,n3,n4,n5,nn[6];
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
          void newt();
          void change_iso(int numsteps,int diff);
  signals:
  void bigmessage(const QString &);

  private:
	  double C[15],D[9],sy[12][192],wave,sintl,cral,crbe,crga;
	  inline int Intersect( double& vm, double& vp ){ return vm*vp <= 0.0 && (vm<0.0 || vp<0.0); }
      QString path,title;
	  V3  delDA[27];
	  //fftwf_plan  fwd_plan;
          //fftwf_complex *B,*B6;
          kiss_fftnd_cfg fwd_plan;
          kiss_fft_cpx *B,*B6;
          FNode *nodex,*nodey,*nodez;
          rec128 llr[LM];
	  rec64 lr[LM];
	  reco wr[LM];
      fcf6d m80r[LM];
      float fc2[LM][3];
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
Mat6 decodeSymm6d(QString symmCard);
void deletes(char *s, int count);
int readMas(const char *filename);
void sorthkl(int nr, rec64 r[]);
int dex(int x,int y, int z);
};

#endif
