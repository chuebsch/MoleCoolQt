#ifndef MOLEKUL_H
#define MOLEKUL_H 1
#include <math.h>
#ifndef fmin
#define fmin(x, y) (((x) < (y)) ? (x) : (y))
#endif
#ifndef fmax
#define fmax(x, y) (((x) > (y)) ? (x) : (y))
#endif
#if defined _MSC_VER &&  _MSC_VER == 1500
#ifndef round 
#define round(x) (x<0?ceil((x)-0.5):floor((x)+0.5))
#endif
#ifndef trunc
#define trunc(d) (d>0) ? floor(d) : ceil(d) 
#endif

#include <float.h>
#define finite _finite
#endif
#include <QList>
#include <QMap>
#include <QSettings>
#include <QString>
#include <QDebug>
#include <QCheckBox>
#ifndef M_PI
#define	M_PI		3.14159265358979323846	 
#endif
#define ATOM_STYLE_WALLS 1
#define ATOM_STYLE_RINGS 2
#define ATOM_STYLE_SPHERE 4
#define ATOM_STYLE_SOLID 8
#define ATOM_STYLE_WHITERING 16
#define ATOM_STYLE_NOLABEL 32
#define ATOM_STYLE_NOADP 64
#define ATOM_STYLE_PLAID 128
#define ATOM_STYLE_METAL 256
#define ATOM_STYLE_NOLISSAJOUS 512
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
  return (Distance(v1,v2)<0.001);
}
inline bool operator != (const V3& v1, const V3& v2 ) {
  return (Distance(v1,v2)>0.001);
}
inline V3& Normalize( V3 v ) {
static V3 erg=V3(0,0,0);
  if (Norm(v))  erg= (v * (1.0/sqrt(Norm(v))));  
  return erg; 
}
inline V3& clamp3(V3 v){
static V3 erg;
  double ig=1;//...nore
  erg.x=modf(v.x+99.0,&ig);
  erg.y=modf(v.y+99.0,&ig);
  erg.z=modf(v.z+99.0,&ig);
  return erg;
}
inline V3& ceil3(V3 v){
static V3 erg;
  erg.x=ceil(v.x);
  erg.y=ceil(v.y);
  erg.z=ceil(v.z);
  return erg;
}
inline V3& floor3(V3 v){
static  V3 erg;
  erg.x=floor(v.x);
  erg.y=floor(v.y);
  erg.z=floor(v.z);
  return erg;
}
inline V3& fract3(V3 v){
  static  V3 erg;
  erg.x=v.x-trunc(v.x);
  erg.y=v.y-trunc(v.y);
  erg.z=v.z-trunc(v.z);
  return erg;
}
inline double fract(double d) {return (d-trunc(d));}
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
const Matrix identity= Matrix(1,0,0, 0,1,0, 0,0,1);
 inline Matrix transponse (Matrix a){//transponse
    return Matrix(
          a.m11, a.m12, a.m13,
          a.m21, a.m22, a.m23,
          a.m31, a.m32, a.m33);
 }
inline double determinant (Matrix a){
return a.m11*a.m22*a.m33 - a.m11*a.m23*a.m32 - a.m12*a.m21*a.m33 + a.m12*a.m23*a.m31 +a.m13*a.m21*a.m32 -a.m13*a.m22*a.m31;
}
inline Matrix inverse (Matrix A){
  double D = determinant(A);
  if (D==0) return A;
  D=1.0/D;
  return Matrix(
          D*(A.m22*A.m33-A.m23*A.m32),//x11
          D*(A.m23*A.m31-A.m21*A.m33),//x12
          D*(A.m21*A.m32-A.m22*A.m31),//x13
          D*(A.m13*A.m32-A.m12*A.m33),//x21
          D*(A.m11*A.m33-A.m13*A.m31),//x22
          D*(A.m12*A.m31-A.m11*A.m32),//x23
          D*(A.m21*A.m23-A.m13*A.m22),//x31
          D*(A.m13*A.m21-A.m11*A.m23),//x32
          D*(A.m11*A.m22-A.m12*A.m21)//x33
          );
}

inline Matrix dyadic(V3 a,V3 b){
  Matrix erg;
  erg.m11 = a.x * b.x;
  erg.m21 = a.x * b.y;
  erg.m31 = a.x * b.z;
          
  erg.m12 = a.y * b.x;
  erg.m22 = a.y * b.y;
  erg.m32 = a.y * b.z;

  erg.m13 = a.z * b.x;
  erg.m23 = a.z * b.y;
  erg.m33 = a.z * b.z;


  return erg;

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
inline Matrix operator - (const Matrix &a,const Matrix &b){
  Matrix erg;
  erg.m11 = a.m11 - b.m11 ;
  erg.m21 = a.m21 - b.m21 ;
  erg.m31 = a.m31 - b.m31 ;

  erg.m12 = a.m12 - b.m11 ;
  erg.m22 = a.m32 - b.m21 ;
  erg.m32 = a.m32 - b.m31 ;

  erg.m13 = a.m13 - b.m11 ;
  erg.m23 = a.m23 - b.m21 ;
  erg.m33 = a.m33 - b.m31 ;
  return erg;
}
inline bool operator == (const Matrix &a,const Matrix &b){
  return (
      (a.m11==b.m11)&& (a.m12==b.m12)&& (a.m13==b.m13)&&
      (a.m21==b.m21)&& (a.m22==b.m22)&& (a.m23==b.m23)&&
      (a.m31==b.m31)&& (a.m32==b.m32)&& (a.m33==b.m33)
      );
}
inline Matrix operator * (const Matrix &a,const double &b){
  Matrix erg;
  erg.m11 = a.m11*b;
  erg.m21 = a.m21*b;
  erg.m31 = a.m31*b;

  erg.m12 = a.m12*b;
  erg.m22 = a.m22*b;
  erg.m32 = a.m32*b;

  erg.m13 = a.m13*b;
  erg.m23 = a.m23*b;
  erg.m33 = a.m33*b;
  return erg;
}
inline Matrix operator + (const Matrix &a, const Matrix &b){
  Matrix erg;
  erg.m11 = a.m11+b.m11;
  erg.m21 = a.m21+b.m21;
  erg.m31 = a.m31+b.m31;
                       
  erg.m12 = a.m12+b.m12;
  erg.m22 = a.m22+b.m22;
  erg.m32 = a.m32+b.m32;
                   
  erg.m13 = a.m13+b.m13;
  erg.m23 = a.m23+b.m23;
  erg.m33 = a.m33+b.m33;
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

typedef struct MyAtom{
  QString Label;
  QString Symbol;
  V3 pos,fpos;
  float peakHeight;
  Matrix uc;
double screenX,screenY;
  int an;
  int part;
  int index;
  int sg;
}MyAtom;
inline bool  operator ==  (const MyAtom &a1,const MyAtom &a2){
  return (a1.Label == a2.Label);
}
inline bool operator < (const MyAtom &a1, const MyAtom &a2){
  return (a1.Label < a2.Label);
}
typedef struct MyBond{
MyAtom const *ato1,*ato2;
int a1,a2;
  double length,chi;
  int order;
  inline MyBond& operator = (const MyBond& b){    
    length=b.length;
    chi=b.chi;
    ato1=b.ato1;
    ato2=b.ato2;
    a1=b.a1;
    a2=b.a2;
    order=b.order;
    return *this;
  }
}MyBond;
typedef QList<MyBond> Connection;
typedef QList<MyAtom> CEnvironment;
typedef struct VPoly{
 V3 mid,nor,verts0,verts1;
 int acol,intra;
 inline VPoly& operator = (const VPoly& b){
 mid=b.mid; nor=b.nor; verts0=b.verts0; verts1=b.verts1; acol=b.acol;intra=b.intra;
 return *this;
 }
}VPoly;

struct Bpth {
 int start;
 QList<V3> pth;  
};
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

struct DABA{
double m0,m1,d1p,d1m,d0,q0,q1p,q1m,q2p,q2m,o0,o1p,o1m,o2p,o2m,o3p,o3m,h0,h1p,h1m,h2p,h2m,h3p,h3m,h4p,h4m,k1,k2,k3,k4,k5,k6;
QString Symmetry,CoordinateSystem;
};
//Ele Oz f'Mo f"Mo f'Cu f"Cu Nsctl rad krad vdw 1S  2S  3S  4S  2P  3P  4P  3D  4D  4F  5S  5P  6S  6P  5D  7S  6D  5F
struct xdbank{
  char pse[5];//2+ \0 + sicherheit
  int  oz;
  double fprimMo;
  double fdprimMo;
  double fprimCu;
  double fdprimCu;
  double nsctl;
  int rad;
  int krad;
  int vdw;
  int s1,s2,s3,s4,p2,p3,p4,d3,d4,f4,s5,p5,s6,p6,d5,s7,d6,f5;
};

const xdbank XDBANK[98] ={
{"H" , 1,  0.000 , 0.000 , 0.000 , 0.000 , -3.739, 32, 32,119,-1, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"He", 2,  0.000 , 0.000 , 0.000 , 0.000 ,  3.260,  0,  0,139, 2, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Li", 3, -0.0003, 0.0001, 0.0008, 0.0003, -1.900,152,152,204, 2,-1, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Be", 4,  0.0005, 0.0002, 0.0038, 0.0014,  7.790,111,111,114, 2,-2, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"B" , 5,  0.0013, 0.0007, 0.0090, 0.0039,  5.300, 88, 88,162, 2,-2, 0, 0,-1, 0, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"C" , 6,  0.0033, 0.0016, 0.0181, 0.0091,  6.646, 77, 77,169, 2,-2, 0, 0,-2, 0, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"N" , 7,  0.0061, 0.0033, 0.0311, 0.0180,  9.360, 69, 69,155, 2,-2, 0, 0,-3, 0, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"O" , 8,  0.0106, 0.0060, 0.0492, 0.0322,  5.803, 66, 66,152, 2,-2, 0, 0,-4, 0, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"F" , 9,  0.0171, 0.0103, 0.0727, 0.0534,  5.654, 64, 64,146, 2,-2, 0, 0,-5, 0, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Ne",10,  0.0259, 0.0164, 0.1019, 0.0833,  4.566,  0,  0,154, 2, 2, 0, 0, 6, 0, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Na",11,  0.0362, 0.0249, 0.1353, 0.1239,  3.630,186,186,177, 2, 2,-1, 0, 6, 0, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Mg",12,  0.0486, 0.0363, 0.1719, 0.1771,  5.375,160,160,189, 2, 2,-2, 0, 6, 0, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Al",13,  0.0645, 0.0514, 0.2130, 0.2455,  3.449,142,142,214, 2, 2,-2, 0, 6,-1, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Si",14,  0.0817, 0.0704, 0.2541, 0.3302,  4.149,116,116,210, 2, 2,-2, 0, 6,-2, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"P" ,15,  0.1023, 0.0942, 0.2955, 0.4335,  5.130,110,110,180, 2, 2,-2, 0, 6,-3, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"S" ,16,  0.1246, 0.1234, 0.3331, 0.5567,  2.847,104,104,180, 2, 2,-2, 0, 6,-4, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Cl",17,  0.1484, 0.1585, 0.3639, 0.7018,  9.577, 98, 98,175, 2, 2,-2, 0, 6,-5, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Ar",18,  0.1743, 0.2003, 0.3843, 0.8717,  1.909,  0,  0,187, 2, 2, 2, 0, 6, 6, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"K" ,19,  0.2009, 0.2494, 0.3868, 1.0657,  3.670,231,231,212, 2, 2, 2,-1, 6, 6, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Ca",20,  0.2262, 0.3064, 0.3641, 1.2855,  4.700,196,196,179, 2, 2, 2,-2, 6, 6, 0,  0,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Sc",21,  0.2519, 0.3716, 0.3119, 1.5331, 12.290,160,160,224, 2, 2, 2, 2, 6, 6, 0, -1,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Ti",22,  0.2776, 0.4457, 0.2191, 1.8069, -3.438,145,145,227, 2, 2, 2, 2, 6, 6, 0, -2,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"V" ,23,  0.3005, 0.5294, 0.0687, 2.1097, -0.382,131,131,212, 2, 2, 2, 2, 6, 6, 0, -3,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Cr",24,  0.3209, 0.6236,-0.1635, 2.4439,  3.635,125,125,214, 2, 2, 2, 1, 6, 6, 0, -5,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Mn",25,  0.3368, 0.7283,-0.5299, 2.8052, -3.730,137,137,214, 2, 2, 2, 2, 6, 6, 0, -5,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Fe",26,  0.3463, 0.8444,-1.1336, 3.1974,  9.450,126,126,214, 2, 2, 2, 2, 6, 6, 0, -6,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Co",27,  0.3494, 0.9721,-2.3653, 3.6143,  2.490,126,126,202, 2, 2, 2, 2, 6, 6, 0, -7,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Ni",28,  0.3393, 1.1124,-3.0029, 0.5091, 10.300,123,123,229, 2, 2, 2, 1, 6, 6, 0, -9,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Cu",29,  0.3201, 1.2651,-1.9646, 0.5888,  7.718,128,128,231, 2, 2, 2, 1, 6, 6, 0,-10,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Zn",30,  0.2839, 1.4301,-1.5491, 0.6778,  5.680,133,133,225, 2, 2, 2, 2, 6, 6, 0,-10,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Ga",31,  0.2307, 1.6083,-1.2846, 0.7763,  7.288,126,126,202, 2, 2, 2,-2, 6, 6,-1, 10,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Ge",32,  0.1547, 1.8001,-1.0885, 0.8855,  8.185,121,121,196, 2, 2, 2,-2, 6, 6,-2, 10,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"As",33,  0.0499, 2.0058,-0.9300, 1.0051,  6.580,120,120,185, 2, 2, 2,-2, 6, 6,-3, 10,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Se",34, -0.0929, 2.2259,-0.7943, 1.1372,  7.970,116,116,189, 2, 2, 2,-2, 6, 6,-4, 10,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Br",35, -0.2901, 2.4595,-0.6763, 1.2805,  6.795,113,113,185, 2, 2, 2,-2, 6, 6,-5, 10,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Kr",36, -0.5574, 2.7079,-0.5657, 1.4385,  7.810,  0,  0,202, 2, 2, 2, 2, 6, 6, 6, 10,  0,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Rb",37, -0.939,  2.968, -0.469,  1.608,  7.090,243,243,227, 2, 2, 2, 2, 6, 6, 6, 10,  0,  0,-1, 0,  0, 0,  0, 0,  0,  0},
{"Sr",38, -1.531,  3.250, -0.353,  1.820,  7.020,214,214,191, 2, 2, 2, 2, 6, 6, 6, 10,  0,  0,-2, 0,  0, 0,  0, 0,  0,  0},
{"Y" ,39, -2.796,  3.567, -0.267,  2.024,  7.750,180,180,258, 2, 2, 2, 2, 6, 6, 6, 10, -1,  0, 2, 0,  0, 0,  0, 0,  0,  0},
{"Zr",40, -2.967,  0.560, -0.186,  2.245,  7.160,157,157,235, 2, 2, 2, 2, 6, 6, 6, 10, -2,  0, 2, 0,  0, 0,  0, 0,  0,  0},
{"Nb",41, -2.073,  0.622, -0.112,  2.483,  7.054,142,142,227, 2, 2, 2, 2, 6, 6, 6, 10, -4,  0, 1, 0,  0, 0,  0, 0,  0,  0},
{"Mo",42, -1.683,  0.686, -0.048,  2.734,  6.715,136,136,227, 2, 2, 2, 2, 6, 6, 6, 10, -5,  0, 1, 0,  0, 0,  0, 0,  0,  0},
{"Tc",43, -1.439,  0.759,  0.006,  3.005,  6.800,135,135,214, 2, 2, 2, 2, 6, 6, 6, 10, -5,  0, 2, 0,  0, 0,  0, 0,  0,  0},
{"Ru",44, -1.259,  0.836,  0.055,  3.296,  7.030,138,138,229, 2, 2, 2, 2, 6, 6, 6, 10, -7,  0, 1, 0,  0, 0,  0, 0,  0,  0},
{"Rh",45, -1.118,  0.919,  0.093,  3.604,  5.880,138,138,225, 2, 2, 2, 2, 6, 6, 6, 10, -8,  0, 1, 0,  0, 0,  0, 0,  0,  0},
{"Pd",46, -0.999,  1.007,  0.121,  3.934,  5.910,138,138,229, 2, 2, 2, 2, 6, 6, 6, 10,-10,  0, 0, 0,  0, 0,  0, 0,  0,  0},
{"Ag",47, -0.897,  1.101,  0.131,  4.282,  5.922,143,143,239, 2, 2, 2, 2, 6, 6, 6, 10,-10,  0, 1, 0,  0, 0,  0, 0,  0,  0},
{"Cd",48, -0.807,  1.202,  0.118,  4.653,  4.870,148,148,249, 2, 2, 2, 2, 6, 6, 6, 10,-10,  0, 2, 0,  0, 0,  0, 0,  0,  0},
{"In",49, -0.728,  1.310,  0.082,  5.045,  4.065,143,143,243, 2, 2, 2, 2, 6, 6, 6, 10, 10,  0,-2,-1,  0, 0,  0, 0,  0,  0},
{"Sn",50, -0.654,  1.425,  0.026,  5.459,  6.225,139,139,225, 2, 2, 2, 2, 6, 6, 6, 10, 10,  0,-2,-2,  0, 0,  0, 0,  0,  0},
{"Sb",51, -0.587,  1.546, -0.056,  5.895,  5.570,140,140,225, 2, 2, 2, 2, 6, 6, 6, 10, 10,  0,-2,-3,  0, 0,  0, 0,  0,  0},
{"Te",52, -0.531,  1.675, -0.176,  6.353,  5.800,137,137,206, 2, 2, 2, 2, 6, 6, 6, 10, 10,  0,-2,-4,  0, 0,  0, 0,  0,  0},
{"I" ,53, -0.474,  1.812, -0.326,  6.836,  5.280,133,133,197, 2, 2, 2, 2, 6, 6, 6, 10, 10,  0,-2,-5,  0, 0,  0, 0,  0,  0},
{"Xe",54, -0.420,  1.958, -0.518,  7.350,  4.920,150,150,216, 2, 2, 2, 2, 6, 6, 6, 10, 10,  0, 2, 6,  0, 0,  0, 0,  0,  0},
{"Cs",55, -0.368,  2.119, -0.746,  7.905,  5.420,168,168,247, 2, 2, 2, 2, 6, 6, 6, 10, 10,  0, 2, 6, -1, 0,  0, 0,  0,  0},
{"Ba",56, -0.324,  2.282, -1.046,  8.462,  5.070,135,135,214, 2, 2, 2, 2, 6, 6, 6, 10, 10,  0, 2, 6, -2, 0,  0, 0,  0,  0},
{"La",57, -0.287,  2.452, -1.409,  9.038,  8.240,187,187,266, 2, 2, 2, 2, 6, 6, 6, 10, 10,  0, 2, 6,  2, 0, -1, 0,  0,  0},
{"Ce",58, -0.249,  2.633, -1.848,  9.660,  4.840,185,185,262, 2, 2, 2, 2, 6, 6, 6, 10, 10, -1, 2, 6, -2, 0, -1, 0,  0,  0},
{"Pr",59, -0.218,  2.821, -2.416, 10.282,  4.580,182,182,262, 2, 2, 2, 2, 6, 6, 6, 10, 10, -3, 2, 6, -2, 0,  0, 0,  0,  0},
{"Nd",60, -0.194,  3.018, -3.181, 10.908,  7.690,181,181,260, 2, 2, 2, 2, 6, 6, 6, 10, 10, -4, 2, 6, -2, 0,  0, 0,  0,  0},
{"Pm",61, -0.175,  3.225, -4.060, 11.552, 12.600,181,181,260, 2, 2, 2, 2, 6, 6, 6, 10, 10, -5, 2, 6, -2, 0,  0, 0,  0,  0},
{"Sm",62, -0.164,  3.442, -5.324, 12.218,  0.800,180,180,260, 2, 2, 2, 2, 6, 6, 6, 10, 10, -6, 2, 6, -2, 0,  0, 0,  0,  0},
{"Eu",63, -0.158,  3.668, -8.929, 11.186,  7.220,200,200,279, 2, 2, 2, 2, 6, 6, 6, 10, 10, -7, 2, 6, -2, 0,  0, 0,  0,  0},
{"Gd",64, -0.165,  3.904, -8.838, 11.916,  6.500,179,179,258, 2, 2, 2, 2, 6, 6, 6, 10, 10, -7, 2, 6, -2, 0, -1, 0,  0,  0},
{"Tb",65, -0.172,  4.154, -9.147,  9.189,  7.380,176,176,256, 2, 2, 2, 2, 6, 6, 6, 10, 10, -9, 2, 6, -2, 0,  0, 0,  0,  0},
{"Dy",66, -0.189,  4.410, -9.805,  9.848, 16.900,175,175,254, 2, 2, 2, 2, 6, 6, 6, 10, 10,-10, 2, 6, -2, 0,  0, 0,  0,  0},
{"Ho",67, -0.217,  4.678,-14.973,  3.705,  8.010,173,173,254, 2, 2, 2, 2, 6, 6, 6, 10, 10,-11, 2, 6, -2, 0,  0, 0,  0,  0},
{"Er",68, -0.259,  4.958, -9.437,  3.938,  7.790,172,172,252, 2, 2, 2, 2, 6, 6, 6, 10, 10,-12, 2, 6, -2, 0,  0, 0,  0,  0},
{"Tm",69, -0.314,  5.248, -8.039,  4.182,  7.070,171,171,252, 2, 2, 2, 2, 6, 6, 6, 10, 10,-13, 2, 6, -2, 0,  0, 0,  0,  0},
{"Yb",70, -0.385,  5.549, -7.211,  4.433, 12.430,193,193,274, 2, 2, 2, 2, 6, 6, 6, 10, 10,-14, 2, 6, -2, 0,  0, 0,  0,  0},
{"Lu",71, -0.472,  5.858, -6.618,  4.694,  7.210,170,170,252, 2, 2, 2, 2, 6, 6, 6, 10, 10,-14, 2, 6, -2, 0, -1, 0,  0,  0},
{"Hf",72, -0.583,  6.185, -6.179,  4.978,  7.700,157,157,237, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 0, -2, 0,  0,  0},
{"Ta",73, -0.705,  6.523, -5.796,  5.272,  6.910,142,142,222, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 0, -3, 0,  0,  0},
{"W" ,74, -0.849,  6.872, -5.473,  5.577,  4.860,137,137,216, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 0, -4, 0,  0,  0},
{"Re",75, -1.018,  7.231, -5.208,  5.892,  9.200,137,137,214, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 0, -5, 0,  0,  0},
{"Os",76, -1.216,  7.603, -4.980,  6.222, 10.700,134,134,216, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 0, -6, 0,  0,  0},
{"Ir",77, -1.444,  7.989, -4.771,  6.567, 10.600,135,135,212, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 0, -7, 0,  0,  0},
{"Pt",78, -1.703,  8.390, -4.593,  6.926,  9.600,137,137,229, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  1, 0, -9, 0,  0,  0},
{"Au",79, -2.013,  8.802, -4.420,  7.298,  7.630,143,143,229, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  1, 0,-10, 0,  0,  0},
{"Hg",80, -2.389,  9.227, -4.292,  7.685, 12.692,155,155,250, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 0,-10, 0,  0,  0},
{"Tl",81, -2.836,  9.669, -4.163,  8.090,  8.776,170,170,235, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6, -2,-1, 10, 0,  0,  0},
{"Pb",82, -3.394, 10.111, -4.075,  8.506,  9.405,175,175,233, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6, -2,-2, 10, 0,  0,  0},
{"Bi",83, -4.108, 10.257, -4.011,  8.931,  8.532,160,160,233, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6, -2,-3, 10, 0,  0,  0},
{"Po",84, -5.121, 11.050, -3.967,  9.383,  0.000,160,160,247, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6, -2,-4, 10, 0,  0,  0},
{"At",85, -7.912,  9.978, -3.959,  9.843,  0.000,160,160,254, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6, -2,-5, 10, 0,  0,  0},
{"Rn",86, -8.066, 10.458, -3.949, 10.318,  0.000,  0,  0,270, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 6, 10, 0,  0,  0},
{"Fr",87, -7.222,  7.785, -3.969, 10.804,  0.000,270,270,270, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 6, 10,-1,  0,  0},
{"Ra",88, -6.770,  8.143, -4.009, 11.297, 10.000,220,220,270, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 6, 10,-2,  0,  0},
{"Ac",89, -6.849,  8.518, -4.079, 11.799,  0.000,180,180,268, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 6, 10, 2, -1,  0},
{"Th",90, -7.240,  8.898, -4.149, 12.330, 10.310,185,185,258, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 6, 10,-2, -2,  0},
{"Pa",91, -8.033,  9.281, -4.247, 12.868,  9.100,180,180,241, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 6, 10,-2, -1, -2},
{"U" ,92, -9.677,  9.665, -4.364, 13.409,  8.417,180,180,237, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 6, 10,-2, -1, -3},
{"Np",93,-11.494,  4.149, -4.505, 13.967, 10.550,180,180,235, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 6, 10,-2, -1, -4},
{"Pu",94, -9.410,  4.306, -4.656, 14.373,  0.000,180,180,233, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 6, 10,-2,  0, -6},
{"Am",95, -7.633,  4.520, -4.644, 15.102,  8.300,180,180,231, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 6, 10,-2,  0, -7},
{"Cm",96, -7.068,  4.705, -4.853, 15.650,  0.000,180,180,229, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 6, 10,-2, -1, -7},
{"Bk",97, -6.597,  4.915, -5.134, 16.333,  0.000,180,180,227, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 6, 10,-2,  0, -9},
{"Cf",98, -6.401,  5.085, -5.340, 16.755,  0.000,180,180,225, 2, 2, 2, 2, 6, 6, 6, 10, 10, 14, 2, 6,  2, 6, 10,-2,  0,-10}};

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
  double   amul;
  double   multiplicity;
  double monopol;
  int   imul;
  V3   frac;          // Fraktionelle Koordinaten in X-Richtung 
  Matrix uf;           // Temperaturparameter 
  Matrix u;
  double c111, c222, c333, c112, c122, c113, c133, c223, c233, c123;
  double d1111,  d2222,  d3333, d1112,  d1113,  d1122, 
	 d1123,  d1133,  d1222, d1223,  d1233,  d1333,
	 d2223,  d2233,  d2333;
                        // Bis hier alles EintrÃÂ¤ge in xd.inp, danach berechnet   
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
  int resiNr;
  char ami3[5];
  char shortname[strgl];
  double peakHeight,eps;
  double lam1,lam2,lam3;
  int cptype;
  int molindex;
  int sg;
  double screenX,screenY;
  uint GLname;
}; 


inline bool operator < (const INP &a1, const INP &a2){
  return a1.peakHeight>a2.peakHeight;
}
struct Cell {
  double a,b,c,al,be,ga;
  double phi,V,as,bs,cs,als,bes,gas,lambda;
  double cosra,cosrb,cosrg;
  double cs_al,cs_be,cs_ga;
  double sn_al,sn_be,sn_ga;
  bool centro,commensurate;
  QChar lattis;
  int symuncent;
  QList<Matrix> symmops;
  QList<V3> trans;
  QList<V3> x4sym;
  QList<double> x4, x4tr;
  double o[3][3];
 // Matrix o1;
  Matrix f2c;
  Matrix G,Gi;
  V3 qr,qi,qvec,commen;//real, imaginary and q-vector
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

struct PolyEder{
    int zi;
    V3 norm,mid;
    QList<int> edge;
    double umfang;
};
//inline bool operator == (const PolyEder &a1, const PolyEder &a2){

//  return ((a1.af==a2.af)&&(a1.bf==a2.bf)&&(a1.cf==a2.cf)&&(a1.volume==a2.volume));
//}
#include <QtGui>
class Modulat;
class Composite;
class molekul {
 public:
  QSettings *einstellung;

  QString ffmpegexe;
  Cell zelle;
  int LOD;
  double modscal;
  double Lissajous_line_width;
  Matrix jacobi(Matrix A, V3 &D);
  double * jacobi2(const Matrix &uij, V3 &ev);
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
  QList<PolyEder> polyeders;
  QList<Bpth> wombats;
  QMap<int,bool> allowedPolyeders;
  QList<VPoly> vtriangles;
  QList<int> sfac;
  QList<Composite> ccc;
  double maxvolpol,minvolpol;
  int bcnt;
  int firstHL;
  int lastHL;
  int dummax;
  int bonds_made;
  int knopf_made;
  int dimensions;
  int ncomp;
  double gd;
  double HAMax;
  double HAWink;
  double qPeakRad;
  double CPRad;
  int pseSize;
  QCheckBox *bondsBetweenSGs;
  bool polyShapColor;
  void Farbverlauf (GLfloat wrt,GLfloat min,GLfloat max,GLfloat alpha=1.0f);
  bool decodeSymmCard(const QString symmCard);
  void enviSDM(double range);
  void countMols(QList<INP> & xdinp);
  bool applyLatticeCentro(const QChar latt,const bool centro);
  QString symmcode2human(QStringList brachSymm);
  QString encodeSymm(int s);
  bool tubifiedAtoms,bondColorStyle;
  int dratom;
  void loadSettings();
  void printm(Matrix m);
  void readXDPath(QString fname);
  molekul(void) {
    modscal=1.0;
    Lissajous_line_width=1.5;
    dratom=false;
    qPeakRad=0.05;
    CPRad=0.025;
    pseSize=10;
    LOD=2;
    VZ.x=0;VZ.y=0;VZ.z=1;
    adp=1;
    bondStrength=0.03;
    bondColor=QColor("silver");
    tubifiedAtoms=false;
    bondColorStyle=false;
    proba=50;
    HAMax=2.3;
    pmin=2000;
    mbonds_last_t=-2.0;
    pmax=-2000;
    polyShapColor=false;
    HAWink=135;
    gd=2.3;
    bonds_made=0;
    knopf_made=0;
    //nListe=0;
    //nL=NULL;
    //vL=NULL;
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
    ElNeg[0]= 220; ElNeg[1]= 550;ElNeg[2]=  97;ElNeg[3]= 147;
    ElNeg[4]= 201; ElNeg[5]= 250;ElNeg[6]= 307;ElNeg[7]= 350;
    ElNeg[8]= 410; ElNeg[9]= 480;ElNeg[10]=101;ElNeg[11]=123;
    ElNeg[12]=147; ElNeg[13]=174;ElNeg[14]=206;ElNeg[15]=244;
    ElNeg[16]=283; ElNeg[17]=320;ElNeg[18]= 91;ElNeg[19]=104;
    ElNeg[20]=120; ElNeg[21]=132;ElNeg[22]=145;ElNeg[23]=156;
    ElNeg[24]=160; ElNeg[25]=164;ElNeg[26]=170;ElNeg[27]=175;
    ElNeg[28]=175; ElNeg[29]=166;ElNeg[30]=182;ElNeg[31]=202;
    ElNeg[32]=220; ElNeg[33]=248;ElNeg[34]=274;ElNeg[35]=290;
    ElNeg[36]= 89; ElNeg[37]= 99;ElNeg[38]=111;ElNeg[39]=122;
    ElNeg[40]=123; ElNeg[41]=130;ElNeg[42]=136;ElNeg[43]=142;
    ElNeg[44]=145; ElNeg[45]=130;ElNeg[46]=142;ElNeg[47]=146;
    ElNeg[48]=149; ElNeg[49]=172;ElNeg[50]=182;ElNeg[51]=201;
    ElNeg[52]=221; ElNeg[53]=240;ElNeg[54]= 86;ElNeg[55]= 97;
    ElNeg[56]=108; ElNeg[57]=108;ElNeg[58]=107;ElNeg[59]=107;
    ElNeg[60]=107; ElNeg[61]=107;ElNeg[62]=110;ElNeg[63]=111;
    ElNeg[64]=110; ElNeg[65]=110;ElNeg[66]=110;ElNeg[67]=111;
    ElNeg[68]=111; ElNeg[69]=106;ElNeg[70]=114;ElNeg[71]=123;
    ElNeg[72]=133; ElNeg[73]=140;ElNeg[74]=146;ElNeg[75]=152;
    ElNeg[76]=155; ElNeg[77]=142;ElNeg[78]=142;ElNeg[79]=144;
    ElNeg[80]=144; ElNeg[81]=155;ElNeg[82]=167;

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
    Acol[i][0]=0.8;
    Acol[i][1]=0.4;
    Acol[i][2]=0.1;
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
    Acol[i][1]=0.2;
    Acol[i][2]=0.6;
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
    Acol[i][0]=0.4;
    Acol[i][1]=0.5;
    Acol[i][2]=0.4;
	Acol[i][3]=0.5;
	break; 
      }
    }
  }
    //ein Konstruktor
  void initDir();
  int fvl;
  int vorobas;
  QString voroMsg;
  QList<SdmItem> envi_sdm;//!< shortest distance matrix for the ENVIron ment listing functionality.

  MAS scat[20];
  V3 uz0k,uz1k,uz2k,uz3k,uz4k,uz5k,uz6k,uz7k;  
  unsigned short Kovalenz_Radien[109];  
  unsigned short ElNeg[83];  

  void frac2kart (V3 x, V3 & y);
  void kart2frac (V3 x, V3 & y);
  void delBlanks(char *in);
  int Get_OZ(const char * S1);
  int Get_OZ(QString S1);
  void setup_zelle();
  void entknoten();
  void SDMprint(QList<SdmItem> sdm,QList<INP> au);
  void distfix(QList<INP> au);
  void multiplicity(QList<INP> &au);
  void Uf2Uo(const Matrix x, Matrix & y);
  void Uc2Uf(const Matrix x, Matrix & y);
  void copyAcol(GLfloat _Acol[108][4],GLfloat _arad[108],int _aStyle[108]);
  void atoms(QList<INP> xdinp,const int proba);
  void modulated(double t, QList<Modulat> mato, int draw, double steps);
  void makeLissajous(QList<Modulat> mato,int proba,bool gay, bool adps, double step,double t0);
  void bonds(QList<INP> xdinp);
  void bondss(QList<INP> xdinp);
  void bonds(Connection bond);
  double fl(double x,double y, double z);
  double fl(V3 v);
  double sintl2(int h,int k, int l);
  void drawVoronoi(V3 auge);
  void voronoij(QList<INP> au, int intat=-1);
  void draw_polyeders(QList<INP> xdinp);
  void atoms(CEnvironment atom,int proba);
  void dodecaeder(GLfloat r);
  void icosaeder(GLfloat r);
  void triacontaeder(GLfloat r);
  void cube(GLfloat r);
  QString h_bonds(QList<INP> xdinp);
  void cbonds(QList<INP> xdinp);
  void axes(QList<INP> xdinp);
  void UnitZell(double t=0.0);
  QString whatSymm();//name all symm ops
  static double winkel(V3 a,V3 b);//vector3...
  double dieder(V3 a,V3 b, V3 c);
  static V3 kreuzX(double x1,double y1,double z1,double x2,double y2,double z2) ; 
  int lod;
  GLfloat Acol[108][4],arad[108];
  int aStyle[108];
  //V3 *nL,*vL;
  //int nListe;
  //void findChains(QList<INP> xdinp);
  //V3* smoothPoints(V3 *vListe, int N);
  //void drawSline(V3 *vL,int N);
  int highlightResi(QList<INP> xdinp,int inv,GLfloat L,bool el);
  QString pse(int oz);
 private:
  void ellipse(int style);
  void sphere(int adp);
  void sp(GLfloat r=1.0f);
  struct knpf{
    int lz;
    int lig[30];
    float bgrad[30];
    int bnr[30];
  };

  knpf *Knopf;
  V3* addToList(V3 *vL,QList<INP> xdinp ,int atom,int N,int rc);
  void highlightInv(QList<INP> xdinp ,int inv,GLfloat L);
  void make_bonds(QList<INP> xdinp );
  void make_bonds(QList<Modulat> xdinp,double t );
  void make_knopf(QList<INP> xdinp );
  void make_polyeder(QList<INP> xd);
//  void cyclsort(PolyEder & p);
  int findPoly(int zi, PolyEder p,QList<INP> xd);
  void planes(QList<INP> xd);
  double * jacobi(double a[3][3], double d[3]); 
  double mbonds_last_t;
};
class Composite{
  public:
  QMatrix4x4 Wnu,Wnuinv;
  Cell nuCell;
  Matrix toCommon;
  double tfactor;
  double wnudd(){return Wnu.data()[15]; }
  V3 wnu3d(){return V3(Wnu.data()[12],Wnu.data()[13],Wnu.data()[14]);}
  V3 wnud3(){return V3(Wnu.data()[3],Wnu.data()[7],Wnu.data()[11]);}
  Matrix wnu3x3(){return Matrix(
      Wnu.data()[0],Wnu.data()[4],Wnu.data()[8],
      Wnu.data()[1],Wnu.data()[5],Wnu.data()[9],
      Wnu.data()[2],Wnu.data()[6],Wnu.data()[10]);}
  Matrix m3x3(QMatrix4x4 u){return Matrix(
      u.data()[0],u.data()[4],u.data()[8],
      u.data()[1],u.data()[5],u.data()[9],
      u.data()[2],u.data()[6],u.data()[10]);}
  const V3 oschi(const double t){
    return wnu3d()*t;
    //return clamp3(wnu3d()*t);
  }
  //constructor
Composite(double w11,double w12,double w13,double w14,double w21,double w22,double w23,double w24,double w31,double w32,double w33,double w34,double w41,double w42,double w43,double w44,
    Cell commonCell){
  Wnu = QMatrix4x4(w11,w12,w13,w14,w21,w22,w23,w24,w31,w32,w33,w34,w41,w42,w43,w44);
  Wnuinv = Wnu.inverted();
  toCommon = inverse(wnu3x3() + dyadic(wnu3d(), commonCell.qvec));
  nuCell.qvec = (wnud3() + wnudd() * commonCell.qvec) * toCommon;
  tfactor=wnudd() - nuCell.qvec * wnu3d();
  //nuCell.G=(Wnu*commonCell.G)*Wnuinv;
  for (int i=0; i<commonCell.symmops.size(); i++){
  QMatrix4x4 sc = QMatrix4x4(
      commonCell.symmops.at(i).m11,commonCell.symmops.at(i).m21,commonCell.symmops.at(i).m31,0.,
      commonCell.symmops.at(i).m12,commonCell.symmops.at(i).m22,commonCell.symmops.at(i).m32,0.,
      commonCell.symmops.at(i).m13,commonCell.symmops.at(i).m23,commonCell.symmops.at(i).m33,0.,
      commonCell.x4sym.at(i).x,commonCell.x4sym.at(i).y,commonCell.x4sym.at(i).z,commonCell.x4.at(i));
  QMatrix4x4 nus = (Wnu*sc)*Wnuinv;
  QVector4D tranu, trac = QVector4D(commonCell.trans.at(i).x,commonCell.trans.at(i).y,commonCell.trans.at(i).z,commonCell.x4tr.at(i));
  tranu=Wnu * trac;
  nuCell.symmops.append(m3x3(nus));  
  nuCell.x4sym.append(V3(nus.data()[3],nus.data()[7],nus.data()[11]));
  nuCell.x4.append(nus.data()[15]);  
  nuCell.trans.append(clamp3(V3(tranu.x(),tranu.y(),tranu.z())));
  nuCell.x4tr.append(tranu.w());
  printf("%d %d %d %d \n",commonCell.x4sym.size(),commonCell.x4.size(),commonCell.trans.size(),commonCell.x4tr.size());
  printf("nusymm  #%d/%d \n%2g %2g %2g %2g, %g\n%2g %2g %2g %2g, %g\n%2g %2g %2g %2g, %g\n%2g %2g %2g %2g, %g\n\n",i ,commonCell.symmops.size(),
      nuCell.symmops.last().m11,nuCell.symmops.last().m21,nuCell.symmops.last().m31,0.,nuCell.trans.last().x,
      nuCell.symmops.last().m12,nuCell.symmops.last().m22,nuCell.symmops.last().m32,0.,nuCell.trans.last().y,
      nuCell.symmops.last().m13,nuCell.symmops.last().m23,nuCell.symmops.last().m33,0.,nuCell.trans.last().z,
      nuCell.x4sym.last().x, nuCell.x4sym.last().y, nuCell.x4sym.last().z, nuCell.x4.last(),nuCell.x4tr.last()
      );
  }

}
};

class Modulat{
  public:
    molekul *mol;
    bool hidden;
    int iamcomp;
    char     atomname[strgl];
    V3       frac0;          // Fraktionelle Koordinaten  
    const V3       kart(const double t, bool *outside2=NULL);          // Berechnete kartesische Koordinaten  
    const V3       frac(const double t);          // Berechnete fractionelle Koordinaten  
    const V3       displacement(const double t);  // Berechnete fractionelle Auslenkung  
    Matrix   uf0;           // Temperaturparameter 
    Matrix   uf(double t);
    Matrix   u(double t);
    double   amul;
    int      OrdZahl;
    V3 x4sym;
    int x4;
    int part;
    double x4trans;
    int   imul;
    int jtf;
    int molindex;
    int sg;
    double screenX,screenY;
    /*    Modulat(const Modulat &m):so(m.so),sp(m.sp),st(m.st),wo(m.wo),wp(m.wp),wt(m.wt){
          mol=m.mol;
          frac0=m.frac0;
          uf0=m.uf0;
          strcpy(atomname,m.atomname);
          amul=m.amul;
          OrdZahl=m.OrdZahl;
          x4sym=m.x4sym;
          x4=m.x4;
          x4trans=m.x4trans;
          if (wo>0) {
          os=(double*)malloc(sizeof(double)*wo);
          oc=(double*)malloc(sizeof(double)*wo);
          }else{oc=os=NULL;}
          if (wp>0) {
          possin=(V3*)malloc(sizeof(V3)*wp);
          poscos=(V3*)malloc(sizeof(V3)*wp);
          }else{ possin=poscos=NULL;}
          if (wt>0) {
          usin=(Matrix*)malloc(sizeof(Matrix)*wt);
          ucos=(Matrix*)malloc(sizeof(Matrix)*wt);
          }else{ usin=ucos=NULL;}
          for (int i=0; i<wo; i++) {os[i]=m.os[i];oc[i]=m.oc[i];}
          for (int i=0; i<wp; i++) {possin[i]=m.possin[i]; poscos[i]=poscos[i];};
          for (int i=0; i<wo; i++) {usin[i]=m.usin[i];ucos[i]=m.ucos[i];}
    //   qDebug()<<"const"<<__LINE__<<atomname;
    }
    Modulat(Modulat &m):so(m.so),sp(m.sp),st(m.st),wo(m.wo),wp(m.wp),wt(m.wt){
    mol=m.mol;
    frac0=V3(m.frac0.x,m.frac0.y,m.frac0.z);
    uf0=m.uf0;
    strcpy(atomname,m.atomname);
    amul=m.amul;
    OrdZahl=m.OrdZahl;
    x4sym=m.x4sym;
    x4=m.x4;
    x4trans=m.x4trans;
    if (wo>0) {
    os=(double*)malloc(sizeof(double)*wo);
    oc=(double*)malloc(sizeof(double)*wo);
    }else{oc=os=NULL;}
    if (wp>0) {
    possin=(V3*)malloc(sizeof(V3)*wp);
    poscos=(V3*)malloc(sizeof(V3)*wp);
    }else{ possin=poscos=NULL;}
    if (wt>0) {
    usin=(Matrix*)malloc(sizeof(Matrix)*wt);
    ucos=(Matrix*)malloc(sizeof(Matrix)*wt);
    }else{ usin=ucos=NULL;}
    for (int i=0; i<wo; i++) {os[i]=m.os[i];oc[i]=m.oc[i];}
    for (int i=0; i<wp; i++) {possin[i]=m.possin[i]; poscos[i]=poscos[i];};
    for (int i=0; i<wo; i++) {usin[i]=m.usin[i];ucos[i]=m.ucos[i];}
    qDebug()<<"non"<<__LINE__<<atomname<<this;
    }*/
    Modulat(int _wo, int _wp, int _wt, int _so, int _sp, int _st):so(_so),sp(_sp),st(_st),wo(_wo),wp(_wp),wt(_wt){
      //printf("modulat konstr %d %d %d\n",wo, wp, wt);
      os.clear();
      oc.clear();
      hidden=false;
      possin.clear();
      poscos.clear();
      usin.clear();
      ucos.clear();
      for (int i=0; i<wo; i++) {os.append(0.0);oc.append(0.0);}
      for (int i=0; i<wp; i++) {possin.append(V3(0,0,0)); poscos.append(V3(0,0,0));};
      for (int i=0; i<wt; i++) {usin.append(Matrix(0,0,0, 0,0,0, 0,0,0));ucos.append(Matrix(0,0,0, 0,0,0, 0,0,0));}
      x4sym=V3(0,0,0);
      x4=1;
      x4trans=0.0;
      polytype=0;
      //printf("modulat konstr\n");
    };
    ~Modulat(){
      os.clear();
      oc.clear();
      hidden=false;
      possin.clear();
      poscos.clear();
      usin.clear();
      ucos.clear();

    }
    void setWaveOccPar(int w, double _o,double s, double c){
      o=_o;
      //      qDebug()<<"Occ"<<w<<wo<<os.size();
      if (w>=wo) {
        errorMsg("Error in filling occupation waves!");
        exit(33);
      }
      os[w]=s;
      oc[w]=c;
    }
    void setWavePosPar(int w, double _xs, double _ys, double _zs, double _xc, double _yc, double _zc){
      //    qDebug()<<"Pos"<<w<<wp<<possin.size();
      if (w>=wp) {
        errorMsg("Error in filling position waves!");
        exit(33);
      }
      possin[w]=V3(_xs,_ys,_zs);
      poscos[w]=V3(_xc,_yc,_zc);
    }
    void setPolyType(int type){
      polytype=type;
      printf("polynome type %d\n",type);
    }
    QString debugme(){
      QString s=QString("so %1 sp %2 st %3 wo %4 wp %5 wt %6").arg(so).arg(sp).arg(st).arg(wo).arg(wp).arg(wt);
      return s;
    }
    QString debugwp(){
      QString s=QString("%8frac xsin %1, ysin %2, zsin %3, xcos %4, ycos %5, zcos %6 x4= %7")

        .arg(possin[0].x)
        .arg(possin[0].y)
        .arg(possin[0].z)
        .arg(poscos[0].x)
        .arg(poscos[0].y)
        .arg(poscos[0].z).arg(x4).arg(atomname);
      return s;
    }
    QString plotT(int steps);
    INP toINP(double t);
    void setWaveTemPar(int w, 
        double _u11s,  double _u22s,  double _u33s,  double _u12s,  double _u13s,  double _u23s,  
        double _u11c,  double _u22c,  double _u33c,  double _u12c,  double _u13c,  double _u23c){
      // qDebug()<<"temp"<<w<<wt<<usin.size();
      if (w>=wt) {
        errorMsg("Error in filling adp waves!");
        exit(33);
      }
      usin[w]=Matrix(_u11s,_u12s,_u13s, _u12s,_u22s,_u23s, _u13s,_u23s,_u33s);
      ucos[w]=Matrix(_u11c,_u12c,_u13c, _u12c,_u22c,_u23c, _u13c,_u23c,_u33c);
    } 
    Modulat applySymm(Matrix sym3d, V3 trans3d, V3 x4sym, int x4,double x4trans);
    double occupancy(double t);
  private:
    void errorMsg(QString msg);
    //    const int id;//id of xdinp or assym
    int so,sp,st;//kind of mod functions of occupancy position and temperature parameters
    int wo,wp,wt;//number of mod functions of occupancy position and temperature parameters
    QList<V3> possin,poscos;//pos
    double o;
    QList<double> os,oc;//occ
    QList<Matrix> usin,ucos;//uij

    int xyn(int x, int y, int n){
      return (y-1)*n+x-1;
    }
    int xyn0(int x, int y, int n){
      return (y)*n+x;
    }
    double scProdXHarm0(int n0, int m0){
      int mp, np;
      if ((n0*m0)==0){
        if (n0==m0) return 2.0/3.0;
        else {if (!n0) {
          mp=m0+1;
          np=(m0+1)/2;
        }else{
          mp=n0+1;
          np=(n0+1)/2;
        }
        if ((mp%2)==1){
          if ((np%2)==0) return -2.0/(np*M_PI);
          else return 2.0/(np*M_PI);
        }else return 0.0;
        }
      }else return (m0==n0)?1.0:0.0;
    } 
    void fLegendre(double x, int npol){
      double d,f1,f2,x2;
      fpol[1]=1.0;
      fpol[2]=x;
      if (npol>2) {
        x2=2.0*x;
        f2=x;
        d=1.0;
        for (int j=3; j<=npol; j++){
          f1=d;
          f2+=x2;
          d+=1.0;
          fpol[j]=(f2*fpol[j-1]-f1*fpol[j-2])/d;
        }
      }
    }
    void makeXHarmOrtho0(double *xmat, int nd);
    double xHarmOrtho(double x, int n, double *xmat, int nd); 
    void getFPol(double x, int npol, int type);
    int polytype;
    double fpol[101];//2*50+1
};

class JmPos{
  public:
    JmPos(QString _pName, int _sig, int _jtf,double _ai,int _wo, int _wp, int _wt, int _so, int _sp, int _st):pName(_pName),sig(_sig),rotty(_jtf),aimol(_ai), smo(_so),smp(_sp),smt(_st),wmo(_wo),wmp(_wp),wmt(_wt){
      os.clear();
      oc.clear();
      possin.clear();
      poscos.clear();
      tlsSin.clear();
      tlsCos.clear();
      for (int i=0; i<wmo; i++) {os.append(0.0);oc.append(0.0);}
      for (int i=0; i<wmp; i++) {possin.append(V3(0,0,0)); poscos.append(V3(0,0,0));};
      for (int i=0; i<wmt*21; i++) {tlsSin.append(0.0);tlsCos.append(0.0);}
    //  x4sym=V3(0,0,0);
    //  x4=1;
    //  x4trans=0.0;
      polytype=0;
    };
    QString pName;
    int sig,rotty;
    double aimol;
    int  smo,smp,smt,wmo,wmp,wmt;
    double phi,chi,psi;

    V3 trans,rm,rp;
    double tls[21];
    double *tlsm(const double t);
    QList<V3> possin,poscos;//pos
    QList<V3> rotsin,rotcos;//rot
    double o;
    QList<double> os,oc;//occ
    QList<double> tlsSin,tlsCos;
    int polytype;
};



class JanaMolecule{
private:
    int nAtm, nPos,lrot;
    QString Mname;
    int IR;
    QString PointG;
    QString refp;
public:
    int np(){return nPos;}
    int na(){return nAtm;}
    QList<Modulat> atoms;
    QList<JmPos> positions;
    V3 ref;
    JanaMolecule(QString name,int ir,QString pg,QString M,int na,int np,int lr):nAtm(na),nPos(np),lrot(lr),Mname(name),IR(ir),PointG(pg),
    refp(M){}
    void printpos(const double t,QList<Modulat> &ma);
};

#endif
