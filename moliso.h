#ifndef MOLISO_H
#define MOLISO_H 1
#include <QString>
#include <QFile>
#include <QVector>
#include <QtOpenGL>
#include <QtGui>
#include <math.h>

struct Vector3 {
  GLfloat x, y, z;
  inline Vector3( void ){}
  inline Vector3( const GLfloat& _x, const GLfloat& _y, const GLfloat& _z ) : 
	  x(_x), y(_y), z(_z) {
	    ;
	  }
  inline Vector3& operator *= ( const GLfloat& d ){
    x *= d;
    y *= d;
    z *= d;
    return *this;
  }
  inline Vector3& operator += ( const Vector3& v ){
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  } 
  inline Vector3& operator += ( const GLfloat& v ){
    x += v;
    y += v;
    z += v;
    return *this;
  }
};
inline bool operator == (const Vector3& v1, const Vector3& v2 ) {
  return ((v1.x==v2.x)&&(v1.y==v2.y)&&(v1.z==v2.z));
}
inline Vector3 operator + ( const Vector3& v1, const Vector3& v2 ) {
  Vector3 t;
  t.x = v1.x + v2.x;
  t.y = v1.y + v2.y;
  t.z = v1.z + v2.z;
  return t;
}
inline Vector3 operator - ( const Vector3& v1, const Vector3& v2 ) {
  Vector3 t;
  t.x = v1.x - v2.x;
  t.y = v1.y - v2.y;
  t.z = v1.z - v2.z;
  return t;
}
inline Vector3 operator * ( const Vector3& v, const GLfloat& d ) {
  Vector3 t;
  t.x = v.x*d;
  t.y = v.y*d;
  t.z = v.z*d;
  return t;
}
inline Vector3 operator * ( const GLfloat& d, const Vector3& v ) {
  Vector3 t;
  t.x = v.x*d;
  t.y = v.y*d;
  t.z = v.z*d;
  return t;
}
inline Vector3 operator % ( const Vector3& v1, const Vector3& v2 ) {
  Vector3 t;
  t.x = v1.y*v2.z - v2.y*v1.z;
  t.y = v1.z*v2.x - v2.z*v1.x;
  t.z = v1.x*v2.y - v2.x*v1.y;
  return t;
}
inline GLfloat operator * ( const Vector3& v1, const Vector3& v2 ) {
  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}
inline GLfloat Norm( const Vector3& v ) {
  return v.x*v.x + v.y*v.y + v.z*v.z;
}
inline GLfloat Distance( const Vector3& v1, const Vector3& v2 ) {
  return Norm(v1 - v2);
}
inline Vector3& Normalize( Vector3 v ) {
  if (Norm(v))  return v *= 1.0/sqrt(Norm(v));
  else return v*=1.0;
}
struct Polygn {
  Vector3 vertex;
  int n;
  int ii[13];
};
struct Ort {
  Vector3 vertex;
  Vector3 normal;
  GLfloat color;
  char direct;
};
struct Node {
  int index;
  unsigned char flag; 
  inline operator unsigned char (){return flag;}
};
class MolIso:public QWidget{
Q_OBJECT
signals:
  void bigmessage(const QString &);
 private:
  bool cubeiso,cubemap;
  int atomanzahl;
  QVector<Ort> orte;
  QVector<Polygn> pgns;
  void DrawPlys();
  void PXsort( void); 
  void Pxsort( void); 
  void PYsort( void); 
  void Pysort( void); 
  void PZsort( void); 
  void Pzsort( void); 
  void makeFaces(int nn, Node poly[] );
  void MakeElement( int ix, int iy, int iz ,int s1, int s2);
  void simpelGrad(void);
  void CalcVertex( int ix, int iy, int iz );
  void CalcVertexes( void );
  void CalcNormals( void );
  Vector3& VectorSelected( Node& node0, Node& node1, Node& node2, Node& node3 );
  GLfloat CalcNormalX( int ix, int iy, int iz );
  GLfloat CalcNormalY( int ix, int iy, int iz );
  GLfloat CalcNormalZ( int ix, int iy, int iz );
  inline int Intersect( GLfloat& vm, GLfloat& vp ){
    return vm*vp <= 0.0 && (vm<0.0 || vp<0.0);
  }

  int IndexSelected( Node& node0, Node& node1, Node& node2, Node& node3 );
  Vector3 *grad;
  Node *nodex;
  Node *nodey;
  Node *nodez;
 public:
  int breite, hoehe, tiefe,bh;
  double iso_level;
  int lineNr;
  QList<double> data;
  QList<double> mdata;
  double L;
  QColor qtFarbe(int index);
  QList<double> isoValues;
  QString GHName;
  QString faceFile;
  Vector3 x_dim, y_dim, z_dim;
  double fixmax,fixmin;
  int Farben;
  GLclampd farbe[7][4];
  int mibas;
  double min,max;
  Vector3 orig;
  MolIso();
  ~MolIso();
  void loadMI(QString fname,bool om=false);
  void readXDGridHeader(QString fname,int &fileType);
  void readJanaHeader(QString fname);
  void createSurface(QString isoFileName, QString mapFileName, QString &storeFaceName,int fileType);
  void createSurface(QString &storeFaceName, double proba);
  void legende();
  void Farbverlauf (GLfloat wrt);
};
#endif
