#ifndef MOLEKUL_H
#define MOLEKUL_H 1
#include <math.h>
#include <QList>
#include <QMap>
#include <QString>
#ifndef M_PI
#define	M_PI		3.14159265358979323846	 
#endif
#define ATOM_STYLE_WALLS 1
#define ATOM_STYLE_RINGS 2
#define ATOM_STYLE_SPHERE 4
#define ATOM_STYLE_SOLID 8
#define ATOM_STYLE_WHITERING 16
#define ATOM_STYLE_NOLABEL 32
struct V3 {
  double x, y, z;
  int rc;
  inline V3( void ){}
  inline V3( const double& _x, const double& _y, const double& _z ) : 
  x(_x), y(_y), z(_z), rc(0) {
    ;
  }
  inline V3& operator *= ( const double& d ){
    x *= d;
    y *= d;
    z *= d;
    return *this;
  }
  inline V3& operator += ( const V3& v ){
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  } 
  inline V3& operator += ( const double& v ){
    x += v;
    y += v;
    z += v;
    return *this;
  }
};
inline V3 operator + ( const V3& v1, const V3& v2 ) {
  V3 t;
  t.x = v1.x + v2.x;
  t.y = v1.y + v2.y;
  t.z = v1.z + v2.z;
  return t;
}
inline V3 operator - ( const V3& v1, const V3& v2 ) {
  V3 t;
  t.x = v1.x - v2.x;
  t.y = v1.y - v2.y;
  t.z = v1.z - v2.z;
  return t;
}
inline V3 operator * ( const V3& v, const double& d ) {
  V3 t;
  t.x = v.x*d;
  t.y = v.y*d;
  t.z = v.z*d;
  return t;
}
inline V3 operator * ( const double& d, const V3& v ) {
  V3 t;
  t.x = v.x*d;
  t.y = v.y*d;
  t.z = v.z*d;
  return t;
}
inline V3 operator % ( const V3& v1, const V3& v2 ) {
  V3 t;
  t.x = v1.y*v2.z - v2.y*v1.z;
  t.y = v1.z*v2.x - v2.z*v1.x;
  t.z = v1.x*v2.y - v2.x*v1.y;
  return t;
}
inline double operator * ( const V3& v1, const V3& v2 ) {
  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}
inline double Norm( const V3& v ) {
  return v.x*v.x + v.y*v.y + v.z*v.z;
}
inline double Distance( const V3& v1, const V3& v2 ) {
  return Norm(v1 - v2);
}
inline bool operator == (const V3& v1, const V3& v2 ) {
  //  return ((v1.x==v2.x)&&(v1.y==v2.y)&&(v1.z==v2.z));
  return (Distance(v1,v2)<0.001);
}
inline V3& Normalize( V3 v ) {
static V3 erg=V3(0,0,0);
  if (Norm(v))  erg= (v * (1.0/sqrt(Norm(v))));  
  return erg; 
} 
typedef struct MyAtom{
  QString Label;
  QString Symbol;
  V3 pos;
  int an;
  int part;
}MyAtom;
inline bool  operator ==  (const MyAtom &a1,const MyAtom &a2){
  return (a1.Label == a2.Label);
}
inline bool operator < (const MyAtom &a1, const MyAtom &a2){
  return (a1.Label < a2.Label);
}
typedef struct MyBond{
MyAtom const *ato1,*ato2;
  double length,chi;
  int order;
  inline MyBond& operator = (const MyBond& b){    
    length=b.length;
    chi=b.chi;
    ato1=b.ato1;
    ato2=b.ato2;
    order=b.order;
    return *this;
  }
}MyBond;
typedef QList<MyBond> Connection;
typedef QList<MyAtom> CEnvironment;
struct Matrix{
double m11, m21, m31, m12, m22, m32, m13, m23, m33;
 inline Matrix(void){}
 inline Matrix( const V3 &a, const V3 &b, const V3 &c):
	 m11(a.x), m21(b.x), m31(c.x),
	 m12(a.y), m22(b.y), m32(c.y),
	 m13(a.z), m23(b.z), m33(c.z){;}
 inline Matrix( const double& x11, const double& x21, const double& x31,
                const double& x12, const double& x22, const double& x32,
                const double& x13, const double& x23, const double& x33):
	 m11(x11), m21(x21), m31(x31),
	 m12(x12), m22(x22), m32(x32),
	 m13(x13), m23(x23), m33(x33){;}

};
 inline Matrix transponse (Matrix a){//transponse
    return Matrix(
		  a.m11, a.m12, a.m13,
		  a.m21, a.m22, a.m23,
		  a.m31, a.m32, a.m33);
 }
inline Matrix operator * (const Matrix &a,const Matrix &b){
  Matrix erg;
  erg.m11 = a.m11 * b.m11 + a.m21 * b.m12 + a.m31 * b.m13;
  erg.m21 = a.m11 * b.m21 + a.m21 * b.m22 + a.m31 * b.m23;
  erg.m31 = a.m11 * b.m31 + a.m21 * b.m32 + a.m31 * b.m33;
  
  erg.m12 = a.m12 * b.m11 + a.m22 * b.m12 + a.m32 * b.m13;
  erg.m22 = a.m12 * b.m21 + a.m22 * b.m22 + a.m32 * b.m23;
  erg.m32 = a.m12 * b.m31 + a.m22 * b.m32 + a.m32 * b.m33;
  
  erg.m13 = a.m13 * b.m11 + a.m23 * b.m12 + a.m33 * b.m13;
  erg.m23 = a.m13 * b.m21 + a.m23 * b.m22 + a.m33 * b.m23;
  erg.m33 = a.m13 * b.m31 + a.m23 * b.m32 + a.m33 * b.m33;
  return erg;
} 
inline V3 operator * (const Matrix &a, const V3 &b){
  V3 erg;
  erg.x = a.m11*b.x + a.m21*b.y + a.m31*b.z;
  erg.y = a.m12*b.x + a.m22*b.y + a.m32*b.z;
  erg.z = a.m13*b.x + a.m23*b.y + a.m33*b.z;
  return erg;
}
inline V3 operator * (const V3 &a, const Matrix &b){
  V3 erg;
  erg.x = b.m11*a.x + b.m12*a.y + b.m13*a.z;
  erg.y = b.m21*a.x + b.m22*a.y + b.m23*a.z;
  erg.z = b.m31*a.x + b.m32*a.y + b.m33*a.z;
  return erg;
}

#define strgl 40
struct MAS  {
  int              Atomtyp;         // Zahl des Atomtyps nach Position in der scatteringtable in xd.mas
  char             Elementname[4]; // Elementname aus Scattering Factor Tabel aus xd.mas
  int              OrdZahl; // Ordnungszahl minus 1
  int              Elektronen;      // Anzahl der Elektronen aus der Summe der Elektronen des Elements
  int              Valenz_Elektronen;      // Anzahl der Valenz-Elektronen aus der Summe der Elektronen des Elements
  char             d1[6];
  char             d2[6];
  char             d3[6];
  double           f;
  double           f2strich;
  double           NSCTL;
  int              orbital[19];
}; 
struct INP {
  char     atomname[strgl]; // Name of the Atom(s)   
  int      lflag; //R-L
  int      icor1;            // Erstes Achse des Koordinatensystems, meist 3, also Z  
  int      icor2;            // Zweites Achse des Koordinatensystems, meist 2, also Y 
  int      nax;              // Achse 1
  int      nay1;             // Achse 2
  int      nay2;             // Achse 3
  int      na3;//3ZX
  int      jtf;              // Ordnung des Verschiebungsparametertensors(ADP)
  int      atomtype;         // Atomtyp als Integer 
  int      noofkappa;        // Number des Kappas 
  int      lmax;             // lmax 
  int      isym;             // isym 
  int      ichcon;           // chemical constraint
  V3   frac;          // Fraktionelle Koordinaten in X-Richtung 
  Matrix uf;           // Temperaturparameter 
  Matrix u;
                        // Bis hier alles Einträge in xd.inp, danach berechnet   
  V3   kart;          // Berechnete kart. Koordinaten in X-Richtung 
  V3   ax1;           // Berechnete Achse 0 
  V3   ax2;           // Berechnete Achse 1 
  V3   ax3;           // Berechnete Achse 2 
  int      OrdZahl;
  int firstNeighbors;
  int secondNeighbors;
  char     invariomname[80];
  V3 labPos;
  int part;
  double peakHeight;
  int molindex;
  int sg;
  uint GLname;
}; 
struct Cell {
  double a,b,c,al,be,ga;
  double phi,V,as,bs,cs;
  QList<Matrix> symmops;
  QList<V3> trans;
};
struct bindi{
  int a;
  int e;
};
#include "cubeGL.h"
   struct SdmItem{
     double d;
     int a1,a2,sn;
     V3 floorD;
     bool covalent;
   };

inline bool operator < (const SdmItem &a1, const SdmItem &a2){
  return (a1.d<a2.d);
}

class molekul {
 public:
  Cell zelle;
  int adp,intern;
  double bondStrength;
  GLuint hbtex, hbtex2;
  GLuint adpwall;
  int proba;
  V3 VZ;
  GLfloat  pmin,pmax;
  QColor bondColor;
  bindi *bd;
  QList<bindi> cBonds;

  int bcnt;
  int firstHL;
  int lastHL;
  int dummax;
  int bonds_made;
  int knopf_made;
  double gd;
  double HAMax;
  double HAWink;
  bool decodeSymmCard(const QString symmCard);
  void countMols(QList<INP> & xdinp);
  bool applyLatticeCentro(const QChar latt,const bool centro);
  QString symmcode2human(QStringList brachSymm);
  bool tubifiedAtoms,singleColorBonds,dratom;
  molekul(void) {
    dratom=false;
    VZ.x=0;VZ.y=0;VZ.z=1;
    adp=1;
    bondStrength=0.10;
    bondColor=QColor("silver");
    tubifiedAtoms=false;
    singleColorBonds=false;
    proba=50;
    HAMax=2.3;
    pmin=2000;
    pmax=-2000;
    HAWink=135;
    gd=1.7;
    bonds_made=0;
    knopf_made=0;
    nListe=0;
    nL=NULL;
    vL=NULL;
    //
    Kovalenz_Radien[0  ]=55  ;Kovalenz_Radien[1  ]=0   ;Kovalenz_Radien[2  ]=123 ;Kovalenz_Radien[3  ]=90  ;
    Kovalenz_Radien[4  ]=80  ;Kovalenz_Radien[5  ]=77  ;Kovalenz_Radien[6  ]=74  ;Kovalenz_Radien[7  ]=71  ;
    Kovalenz_Radien[8  ]=72  ;Kovalenz_Radien[9  ]=0   ;Kovalenz_Radien[10 ]=154 ;Kovalenz_Radien[11 ]=149 ;
    Kovalenz_Radien[12 ]=118 ;Kovalenz_Radien[13 ]=111 ;Kovalenz_Radien[14 ]=106 ;Kovalenz_Radien[15 ]=102 ;
    Kovalenz_Radien[16 ]=99  ;Kovalenz_Radien[17 ]=0   ;Kovalenz_Radien[18 ]=203 ;Kovalenz_Radien[19 ]=174 ;
    Kovalenz_Radien[20 ]=144 ;Kovalenz_Radien[21 ]=132 ;Kovalenz_Radien[22 ]=122 ;Kovalenz_Radien[23 ]=118 ;
    Kovalenz_Radien[24 ]=117 ;Kovalenz_Radien[25 ]=117 ;Kovalenz_Radien[26 ]=116 ;Kovalenz_Radien[27 ]=124 ;
    Kovalenz_Radien[28 ]=117 ;Kovalenz_Radien[29 ]=125 ;Kovalenz_Radien[30 ]=126 ;Kovalenz_Radien[31 ]=122 ;
    Kovalenz_Radien[32 ]=120 ;Kovalenz_Radien[33 ]=116 ;Kovalenz_Radien[34 ]=114 ;Kovalenz_Radien[35 ]=0   ;
    Kovalenz_Radien[36 ]=218 ;Kovalenz_Radien[37 ]=191 ;Kovalenz_Radien[38 ]=162 ;Kovalenz_Radien[39 ]=145 ;
    Kovalenz_Radien[40 ]=134 ;Kovalenz_Radien[41 ]=130 ;Kovalenz_Radien[42 ]=127 ;Kovalenz_Radien[43 ]=125 ;
    Kovalenz_Radien[44 ]=125 ;Kovalenz_Radien[45 ]=128 ;Kovalenz_Radien[46 ]=134 ;Kovalenz_Radien[47 ]=148 ;
    Kovalenz_Radien[48 ]=144 ;Kovalenz_Radien[49 ]=141 ;Kovalenz_Radien[50 ]=140 ;Kovalenz_Radien[51 ]=136 ;
    Kovalenz_Radien[52 ]=133 ;Kovalenz_Radien[53 ]=0   ;Kovalenz_Radien[54 ]=235 ;Kovalenz_Radien[55 ]=198 ;
    Kovalenz_Radien[56 ]=169 ;Kovalenz_Radien[57 ]=165 ;Kovalenz_Radien[58 ]=165 ;Kovalenz_Radien[59 ]=164 ;
    Kovalenz_Radien[60 ]=164 ;Kovalenz_Radien[61 ]=162 ;Kovalenz_Radien[62 ]=185 ;Kovalenz_Radien[63 ]=161 ;
    Kovalenz_Radien[64 ]=159 ;Kovalenz_Radien[65 ]=159 ;Kovalenz_Radien[66 ]=157 ;Kovalenz_Radien[67 ]=157 ;
    Kovalenz_Radien[68 ]=156 ;Kovalenz_Radien[69 ]=170 ;Kovalenz_Radien[70 ]=156 ;Kovalenz_Radien[71 ]=144 ;
    Kovalenz_Radien[72 ]=134 ;Kovalenz_Radien[73 ]=130 ;Kovalenz_Radien[74 ]=128 ;Kovalenz_Radien[75 ]=126 ;
    Kovalenz_Radien[76 ]=127 ;Kovalenz_Radien[77 ]=130 ;Kovalenz_Radien[78 ]=134 ;Kovalenz_Radien[79 ]=149 ;
    Kovalenz_Radien[80 ]=148 ;Kovalenz_Radien[81 ]=147 ;Kovalenz_Radien[82 ]=146 ;Kovalenz_Radien[83 ]=146 ;
    Kovalenz_Radien[84 ]=145 ;Kovalenz_Radien[85 ]=0   ;Kovalenz_Radien[86 ]=0   ;Kovalenz_Radien[87 ]=0   ;
    Kovalenz_Radien[88 ]=188 ;Kovalenz_Radien[89 ]=165 ;Kovalenz_Radien[90 ]=161 ;Kovalenz_Radien[91 ]=142 ;
    Kovalenz_Radien[92 ]=130 ;Kovalenz_Radien[93 ]=151 ;Kovalenz_Radien[94 ]=182 ;
    //EN{
    ElNeg[0]=220; ElNeg[1]=550; ElNeg[2]= 97; ElNeg[3]=147;
    ElNeg[4]=201; ElNeg[5]=250; ElNeg[6]=307; ElNeg[7]=350;
    ElNeg[8]=410; ElNeg[9]=480; ElNeg[10]=101;ElNeg[11]=123;
    ElNeg[12]=147;ElNeg[13]=174;ElNeg[14]=206;ElNeg[15]=244;
    ElNeg[16]=283;ElNeg[17]=320;ElNeg[18]= 91;ElNeg[19]=104;
    ElNeg[20]=120;ElNeg[21]=132;ElNeg[22]=145;ElNeg[23]=156;
    ElNeg[24]=160;ElNeg[25]=164;ElNeg[26]=170;ElNeg[27]=175;
    ElNeg[28]=175;ElNeg[29]=166;ElNeg[30]=182;ElNeg[31]=202;
    ElNeg[32]=220;ElNeg[33]=248;ElNeg[34]=274;ElNeg[35]=290;
    ElNeg[36]= 89;ElNeg[37]= 99;ElNeg[38]=111;ElNeg[39]=122;
    ElNeg[40]=123;ElNeg[41]=130;ElNeg[42]=136;ElNeg[43]=142;
    ElNeg[44]=145;ElNeg[45]=130;ElNeg[46]=142;ElNeg[47]=146;
    ElNeg[48]=149;ElNeg[49]=172;ElNeg[50]=182;ElNeg[51]=201;
    ElNeg[52]=221;ElNeg[53]=240;ElNeg[54]= 86;ElNeg[55]= 97;
    ElNeg[56]=108;ElNeg[57]=108;ElNeg[58]=107;ElNeg[59]=107;
    ElNeg[60]=107;ElNeg[61]=107;ElNeg[62]=110;ElNeg[63]=111;
    ElNeg[64]=110;ElNeg[65]=110;ElNeg[66]=110;ElNeg[67]=111;
    ElNeg[68]=111;ElNeg[69]=106;ElNeg[70]=114;ElNeg[71]=123;
    ElNeg[72]=133;ElNeg[73]=140;ElNeg[74]=146;ElNeg[75]=152;
    ElNeg[76]=155;ElNeg[77]=142;ElNeg[78]=142;ElNeg[79]=144;
    ElNeg[80]=144;ElNeg[81]=155;ElNeg[82]=167;  

    for (int i=0; i<107; i++){      
      arad[i]=Kovalenz_Radien[i]/(250.0);
      aStyle[i]=ATOM_STYLE_WALLS|ATOM_STYLE_RINGS|ATOM_STYLE_SPHERE;
      switch (i){
      case 0:
	Acol[i][0]=0.7;
	Acol[i][1]=0.7;
	Acol[i][2]=0.6;
	Acol[i][3]=0.5;
	break;
      case 2:
	Acol[i][0]=0.9;
	Acol[i][1]=0.3;
	Acol[i][2]=0.3;
	Acol[i][3]=0.5;
	break;
      case 3:
	Acol[i][0]=0.5;
	Acol[i][1]=0.5;
	Acol[i][2]=0.3;
	Acol[i][3]=0.5;
	break;
      case 4:
	Acol[i][0]=0.0;
	Acol[i][1]=0.3;
	Acol[i][2]=0.8;
	Acol[i][3]=0.5;
	break;
      case 5:
	Acol[i][0]=0.2;
	Acol[i][1]=0.2;
	Acol[i][2]=0.3;
	Acol[i][3]=0.5;
	break;
      case 6:
	Acol[i][0]=0.0;
	Acol[i][1]=0.6;
	Acol[i][2]=0.2;
	Acol[i][3]=0.5;
	break;
      case 7:
	Acol[i][0]=0.6;
	Acol[i][1]=0.0;
	Acol[i][2]=0.2;
	Acol[i][3]=0.5;
	break;
      case 15:
	Acol[i][0]=0.7;
	Acol[i][1]=0.7;
	Acol[i][2]=0.2;
	Acol[i][3]=0.5;
	break;
      case 16:
	Acol[i][0]=0.3;
	Acol[i][1]=0.7;
	Acol[i][2]=0.4;
	Acol[i][3]=0.5;
	break;
      case 8:
	Acol[i][0]=0.4;
	Acol[i][1]=0.9;
	Acol[i][2]=0.1;
	Acol[i][3]=0.5;
	break;
      case 26:
	Acol[i][0]=0.5;
	Acol[i][1]=0.5;
	Acol[i][2]=0.5;
	Acol[i][3]=0.5;
	break;
      default:
	Acol[i][0]=0.5;
	Acol[i][1]=0.3;
	Acol[i][2]=0.5;
	Acol[i][3]=0.5;
	break; 
      }
    }
  }
    //ein Konstruktor
  void initDir();//const char * dir);
  int fvl;
  MAS scat[20];
 // QList<INP> xdinp;
  V3 uz0k,uz1k,uz2k,uz3k,uz4k,uz5k,uz6k,uz7k;  
  unsigned short Kovalenz_Radien[109];  
  unsigned short ElNeg[83];  

  void frac2kart (V3 x, V3 & y);
  void kart2frac (V3 x, V3 & y);
  void delBlanks(char *in);
  int Get_OZ(const char * S1);
  int Get_OZ(QString S1);
  void copyAcol(GLfloat _Acol[108][4],GLfloat _arad[108],int _aStyle[108]);
  void atoms(QList<INP> xdinp,const int proba);
  void bonds(QList<INP> xdinp);
  QString h_bonds(QList<INP> xdinp);
  void cbonds(QList<INP> xdinp);
  void axes(QList<INP> xdinp);
  void UnitZell(void);
  static double winkel(V3 a,V3 b);//vector3...
  double dieder(V3 a,V3 b, V3 c);
  static V3 kreuzX(double x1,double y1,double z1,double x2,double y2,double z2) ; 
  int lod;
  GLfloat Acol[108][4],arad[108];
  int aStyle[108];
  V3 *nL,*vL;
  int nListe;
  void findChains(QList<INP> xdinp);
  V3* smoothPoints(V3 *vListe, int N);
  void drawSline(V3 *vL,int N);
  int highlightResi(QList<INP> xdinp,int inv,GLfloat L,bool el);
 private:
  void ellipse(int style);
  struct knpf{
    int lz;
    int lig[30];
    float bgrad[30];
    int bnr[30];
  };

  knpf *Knopf;
  void Farbverlauf (GLfloat wrt,GLfloat min,GLfloat max);
  V3* addToList(V3 *vL,QList<INP> xdinp ,int atom,int N,int rc);
  void highlightInv(QList<INP> xdinp ,int inv,GLfloat L);
  void make_bonds(QList<INP> xdinp );
  void make_knopf(QList<INP> xdinp );

  // jacobi durch genauen algo ersetzen
  double * jacobi(double a[3][3], double d[3]); 
};
#endif
