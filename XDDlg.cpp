#include "XDDlg.h"
#include <QtGui>
#include <QtOpenGL>
#include "cubeGL.h"
#include <iostream> 
extern int egal;
QStringList allLab;
XDDlg::XDDlg(CEnvironment ch,Connection cll,CEnvironment lc,CEnvironment *all,QWidget *parent,xdEditDlg *invD): QGLWidget(parent){
   setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer) );
   papi=invD;
   locCo=lc;
   alle=*all;
   //dkilllist.clear();
   dummyMode=0;
   ce=ch;
   V3 mitte(0,0,0);
   for (int i=0;i<ce.size();i++){
     mitte+=ce.at(i).pos;
   }
   mitte*=-1.0/ce.size();
   cl=cll;
   mitsav=mitte;
   imFokus=-1;
}
void XDDlg::initializeGL(){
  glEnable(GL_LINE_SMOOTH);  
  glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
  const GLfloat  position[] = {100.0f, 100.0f,100.0f,0.0f};
  const GLfloat  diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
  const GLfloat  specular[] = { 1.0, 0.9, 0.9, 1.0 };
  const GLfloat  ambient[]  = { 0.6, 0.6, 0.6, 1.0 };

  glLightModeli(  GL_LIGHT_MODEL_LOCAL_VIEWER, 1 );

  glLightfv( GL_LIGHT0, GL_POSITION, position );
  glLightfv( GL_LIGHT0, GL_AMBIENT,  ambient );
  glLightfv( GL_LIGHT0, GL_DIFFUSE,  diffuse );
  glLightfv( GL_LIGHT0, GL_SPECULAR, specular );

  glLightfv( GL_LIGHT1, GL_POSITION, position );
  glLightfv( GL_LIGHT1, GL_DIFFUSE,  diffuse );  
  glLightfv( GL_LIGHT1, GL_AMBIENT,  ambient );

  glLightfv( GL_LIGHT2, GL_DIFFUSE,  diffuse );  

  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );
  glDisable(GL_BLEND);
  glAlphaFunc ( GL_GREATER, 0.01f ) ;
  glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) ;
  const GLfloat  OBJ_SPE[]   = { 1.0, 1.0, 1.0, 1.0 };  
  const GLfloat  OBJ_SHIN    = 127.0;                   
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,             OBJ_SPE  );
  glEnable     ( GL_COLOR_MATERIAL ) ;
  glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;    
  glMaterialf(  GL_FRONT_AND_BACK, GL_SHININESS,           OBJ_SHIN );
  glShadeModel( GL_SMOOTH );
  glEnable(GL_NORMALIZE);
  glClearColor(0.4,0.4,0.4,1.0);
   glEnable(GL_DEPTH_TEST );
   glDepthFunc(GL_LEQUAL);
   gluLookAt(0.0, 200, 50 ,   0.0, 0.0, 0.0,   0.0, -100, 400 );
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    setMouseTracking ( true );
}
void XDDlg::resizeGL(int width, int height){
  glViewport(0, 0, width, height);        
  glGetIntegerv(GL_VIEWPORT, vp);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective( 29.0, (double)width/height, 5.0, 8000.0 );
}

void XDDlg::paintGL(){
  glClearColor(0.7,0.7,0.64,1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, QGLWidget::width(), QGLWidget::height());        
  glGetIntegerv(GL_VIEWPORT, vp);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective( 29.0, (double)QGLWidget::width()/QGLWidget::height(), 5.0, 8000.0 );
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  draw();
  glPopMatrix();
}

extern void __RotateCS( double c, double s, double& X, double& Y );
extern void glTranslateL( const double dx, const double dy, const double dz );
extern void glRotateL( const double dang, const double x, const double y, const double z );

#ifndef POStO2d
#define POStO2d 1

static inline void transform_point(GLdouble out[4], const GLdouble m[16], const GLdouble in[4]) {
#define M(row,col)  m[col*4+row]
    out[0] =
        M(0, 0) * in[0] + M(0, 1) * in[1] + M(0, 2) * in[2] + M(0, 3) * in[3];
    out[1] =
        M(1, 0) * in[0] + M(1, 1) * in[1] + M(1, 2) * in[2] + M(1, 3) * in[3];
    out[2] =
        M(2, 0) * in[0] + M(2, 1) * in[1] + M(2, 2) * in[2] + M(2, 3) * in[3];
    out[3] =
        M(3, 0) * in[0] + M(3, 1) * in[1] + M(3, 2) * in[2] + M(3, 3) * in[3];
#undef M
}

static inline bool  posTo2D(V3 obj,
           const GLdouble model[16], const GLdouble proj[16],
           const GLint viewport[4],
           GLdouble * winx, GLdouble * winy) {
   GLdouble in[4], out[4];

   in[0] = obj.x;
   in[1] = obj.y;
   in[2] = obj.z;
   in[3] = 1.0;
   transform_point(out, model, in);
   transform_point(in, proj, out);

   if (in[3] == 0.0) return false;

   in[0] /= in[3];
   in[1] /= in[3];
   in[2] /= in[3];

   *winx = viewport[0] + (1 + in[0]) * viewport[2] / 2;
   *winy = viewport[1] + (1 - in[1]) * viewport[3] / 2;
   return true;

}
#endif

void XDDlg::mousePressEvent(QMouseEvent *event){
  double nahda=200.0,da=0;
  int nahdai=-1;
  for (int j=0; j<ce.size();j++){
    da=(((ce.at(j).screenX-event->x())*( ce.at(j).screenX-event->x()))+ 
		    ((ce.at(j).screenY-event->y())*( ce.at(j).screenY-event->y())));
    nahdai=(da<nahda)?j:nahdai;
    nahda=qMin(nahda,da);
  }
   lastPos = event->pos();
     if (nahdai <= 0) {
     }else if (nahdai<ce.size()) {
       GLuint index=nahdai;
       if(dummyMode){
         /*if ((dummyMode==3)&&(ce.at(index).an<0)) {
	   if (duminuse.contains(ce.at(index).Label,Qt::CaseInsensitive)){
	     emit message("Sorry this dummy is still in use! Press kill button again, if you want!");
	     for (int oo=0; oo<duminuse.size();oo++) std::cout<<duminuse.at(oo).toStdString()<<std::endl;
	     dummyMode=0;
	     this->setCursor(Qt::ArrowCursor);
	     updateGL();
	     return;
	   }
	   dkilllist.append(ce.at(index).Label);
	   emit message(QString("You killed %1.").arg(ce.at(index).Label));
           alle.removeAt(alle.indexOf(ce.at(index)));
	   emit delatom(ce.at(index));
	   ce.removeAt(index);
	   dummyMode=0;
	   this->setCursor(Qt::ArrowCursor);
	   updateGL();
	   return;
	 }else if (dummyMode==3) {
	   emit message("Sorry this is not a dummy. Press kill button again, if want!");
	   dummyMode=0;
	   this->setCursor(Qt::ArrowCursor);
	   updateGL();
	   return;
         }*/
	 if (!durchlauf) emit message(QString("You clicked on %1. Click on next atom now!").arg(ce.at(index).Label));
	 if (!durchlauf) dpos=V3(0,0,0);
	 dpos+=Normalize(ce.at(index).pos-ce.at(0).pos);
	 durchlauf++;
	 if ((durchlauf==2)){
	   emit message(QString("You clicked on %1. and %2").arg(ce.at(oldindex).Label).arg(ce.at(index).Label));
	   MyAtom newDummy;
           QString prev="DUM-1";
           for (int i=0; i<alle.size();i++) if (alle.at(i).an==-1) prev=alle.at(i).Label;
           prev.remove(0,3);
           int num = prev.toInt()+1;

           newDummy.Label="DUM"+QString::number(num);
	   if (dummyMode==1)newDummy.pos=Normalize(dpos)+ce.at(0).pos;
	   if (dummyMode==2)newDummy.pos=(Normalize(ce.at(index).pos-ce.at(0).pos)%dpos)+ce.at(0).pos;
	   newDummy.an=-1;
           duminuse.append(newDummy.Label);
	   ce.append(newDummy);
           alle.append(newDummy);
           allLab.append(newDummy.Label);
	   emit addatom(newDummy);
	   this->setCursor(Qt::ArrowCursor);
	   dummyMode=0;
	 }
	 oldindex=index;       
       }
       updateGL();
     }
}
void XDDlg::mouseMoveEvent(QMouseEvent *event){
   GLfloat dx = GLfloat(event->x() - lastPos.x()) / width();
   GLfloat dy = GLfloat(event->y() - lastPos.y()) / height();
   
  double nahda=200.0,da=0;
  int nahdai=-1;
  for (int j=0; j<ce.size();j++){
    da=((ce.at(j).screenX-event->x())*( ce.at(j).screenX-event->x()))+ 
		    ((ce.at(j).screenY-event->y())*( ce.at(j).screenY-event->y()));
    nahdai=(da<nahda)?j:nahdai;
    nahda=qMin(nahda,da);
  }
   if (!event->buttons()) {
    if ((nahdai < 0)||(ce.size()<nahdai)) {
      imFokus=-1; 
	updateGL();
    } else 
    {      
      GLuint index=nahdai;
      if (((int) index)<ce.size()) {
        imFokus=index;
	
	updateGL();
      }
    }
   }
   if ((event->buttons() & Qt::LeftButton)) {
     glRotateL(dy*360.0,1.0f,0.0f,0.0f);
     glRotateL(dx*360.0,0.0f,1.0f,0.0f);
     updateGL();
   }else if((event->buttons() & Qt::RightButton)){
     glScaled(1.0-dy,1.0-dy,1.0-dy);
     updateGL();
   }
   lastPos = event->pos();
}
void XDDlg::setLinMode(){this->setCursor(Qt::CrossCursor);dummyMode=1;durchlauf=0;emit message("To create a linear-combination dummy click on two atoms bonded to the central atom.");}
void XDDlg::setXprMode(){this->setCursor(Qt::CrossCursor);dummyMode=2;durchlauf=0;emit message("To create a cross-product dummy click on two atoms bonded to the central atom.");}
//void XDDlg::killDuMode(){this->setCursor(Qt::CrossCursor);dummyMode=3;durchlauf=0;emit message("Click on a dummy atom to remove it from the atoms list.");}
#define TWOPI 6.28318530719586
#define C cos(TWOPI/(double)i)
#define S sin(TWOPI/(double)i)
#define _C (1-C)
typedef double M9[9]; 
  struct KAP {
    int ifz;
    double k[6];
  };
  struct RES {
    char atom[9];
    int icor1,icor2,nax,nay,nay2,jtf,itbl,isfz,lmax,isym,ichcon;
    double x,y,z,amult;
    double u[31];//adp
    double m[26];//multipoles
  };
void rot_(M9 a,V3 b, V3 &c){
  c.x=a[0]*b.x+a[1]*b.y+a[2]*b.z;
  c.y=a[3]*b.x+a[4]*b.y+a[5]*b.z;
  c.z=a[6]*b.x+a[7]*b.y+a[8]*b.z;
}
void xdEditDlg::symmetryReducer(QString text){
  QStringList axl = text.split(QRegExp("[^XYZ]"), QString::SkipEmptyParts);
  QStringList axn = text.split(QRegExp("[XYZ ]"), QString::SkipEmptyParts);
  X_1 ->hide(); X_1 ->setChecked(false);
  Y_1 ->hide(); Y_1 ->setChecked(false);
  Z_1 ->hide(); Z_1 ->setChecked(false);
  X2  ->hide(); X2  ->setChecked(false);
  Xm  ->hide(); Xm  ->setChecked(false);
  X_3 ->hide(); X_3 ->setChecked(false);
  X_4 ->hide(); X_4 ->setChecked(false);
  X3  ->hide(); X3  ->setChecked(false);
  X4  ->hide(); X4  ->setChecked(false);
  Y2  ->hide(); Y2  ->setChecked(false);
  Ym  ->hide(); Ym  ->setChecked(false);
  Y_3 ->hide(); Y_3 ->setChecked(false);
  Y_4 ->hide(); Y_4 ->setChecked(false);
  Y3  ->hide(); Y3  ->setChecked(false);
  Y4  ->hide(); Y4  ->setChecked(false);
  Z2  ->hide(); Z2  ->setChecked(false);
  Zm  ->hide(); Zm  ->setChecked(false);
  Z_3 ->hide(); Z_3 ->setChecked(false);
  Z_4 ->hide(); Z_4 ->setChecked(false);
  Z3  ->hide(); Z3  ->setChecked(false);
  Z4  ->hide(); Z4  ->setChecked(false);

  for (int i=0; i<axn.size();i++) {
    if (axl.at(i)=="X"){
    if (axn.at(i).contains("-1") ) { X_1->setChecked(true); X_1->show(); }
    if (axn.at(i).contains("-3") ) { X_3->setChecked(true); X_3->show();axn[i].remove("-3");  }
    if (axn.at(i).contains("-4") ) { X_4->setChecked(true); X_4->show();axn[i].remove("-4");  }
    if (axn.at(i).contains("2") )  { X2 ->setChecked(true); X2 ->show(); }
    if (axn.at(i).contains("m") )  { Xm ->setChecked(true); Xm ->show(); }
    if (axn.at(i).contains("3") )  { X3 ->setChecked(true); X3 ->show(); }
    if (axn.at(i).contains("4") )  { X4 ->setChecked(true); X4 ->show(); }
    }else if (axl.at(i)=="Y"){                                           
    if (axn.at(i).contains("-1") ) { Y_1->setChecked(true); Y_1->show(); }
    if (axn.at(i).contains("-3") ) { Y_3->setChecked(true); Y_3->show();axn[i].remove("-3");  }
    if (axn.at(i).contains("-4") ) { Y_4->setChecked(true); Y_4->show();axn[i].remove("-4");  }
    if (axn.at(i).contains("2") )  { Y2 ->setChecked(true); Y2 ->show(); }
    if (axn.at(i).contains("m") )  { Ym ->setChecked(true); Ym ->show(); }
    if (axn.at(i).contains("3") )  { Y3 ->setChecked(true); Y3 ->show(); }
    if (axn.at(i).contains("4") )  { Y4 ->setChecked(true); Y4 ->show(); }
                                                                         
    }else if (axl.at(i)=="Z"){                                           
    if (axn.at(i).contains("-1") ) { Z_1->setChecked(true); Z_1->show(); }
    if (axn.at(i).contains("-3") ) { Z_3->setChecked(true); Z_3->show();axn[i].remove("-3"); }
    if (axn.at(i).contains("-4") ) { Z_4->setChecked(true); Z_4->show();axn[i].remove("-4");  }
    if (axn.at(i).contains("2") )  { Z2 ->setChecked(true); Z2 ->show(); }
    if (axn.at(i).contains("m") )  { Zm ->setChecked(true); Zm ->show(); }
    if (axn.at(i).contains("3") )  { Z3 ->setChecked(true); Z3 ->show(); }
    if (axn.at(i).contains("4") )  { Z4 ->setChecked(true); Z4 ->show(); }

    }
  }
update();  
}
void xdEditDlg::molman(QString text){
  int scd=0;
  QMessageBox msgBox;
  statlab.setText(text);
  QString belief="No clue sorry!";
  QStringList sl=text.split(" ",QString::SkipEmptyParts);  
  if (sl.size()==0) {belief="no symmetry along the present local-coordinate-system axes.(C<sub>1</sub>)";scd=1;}//1 (Nummerierung nach Symmetry throug the Eyes of a Chemist S.82)
  if ((sl.size()==3)&&(sl.contains("-1X"))&&(sl.contains("-1Y"))&&(sl.contains("-1Z"))) {belief="inversion symmetry.(C<sub>i</sub>)";scd=-1;}//2
  if ((sl.size()==3)&&(sl.contains("mX"))&&(sl.contains("mY"))&&(sl.contains("2Z"))) {belief="mm2 symmetry.(C<sub>2v</sub>)";scd=6;}//6
  if ((sl.size()==3)&&(sl.contains("mX"))&&(sl.contains("2Y"))&&(sl.contains("mZ"))) {belief="mm2 symmetry.(C<sub>2v</sub>)<br><font color=\"red\">Please change Y to Z!</font>";}
  if ((sl.size()==3)&&(sl.contains("2X"))&&(sl.contains("mY"))&&(sl.contains("mZ"))) {belief="mm2 symmetry.(C<sub>2v</sub>)<br><font color=\"red\">Please change X to Z!</font>";}
  if ((sl.size()==1)&&(sl.contains("mX"))){belief="m symmetry perpendicular to X.(C<sub>S</sub>)";scd=31;}//3
  if ((sl.size()==1)&&(sl.contains("mY"))){belief="m symmetry perpendicular to Y.(C<sub>S</sub>)";scd=32;}
  if ((sl.size()==1)&&(sl.contains("mZ"))){belief="m symmetry perpendicular to Z.(C<sub>S</sub>)";scd=33;}
  if ((sl.size()==1)&&(sl.contains("2X"))){belief="2 fold symmetry along X.(C<sub>2</sub>)";scd=41;}//4
  if ((sl.size()==1)&&(sl.contains("2Y"))){belief="2 fold symmetry along Y.(C<sub>2</sub>)";scd=42;}
  if ((sl.size()==1)&&(sl.contains("2Z"))){belief="2 fold symmetry along Z.(C<sub>2</sub>)";scd=43;}
  if ((sl.size()==1)&&(sl.contains("2mX"))){belief="2m symmetry along X.(C<sub>2h</sub>)";scd=51;}//5
  if ((sl.size()==1)&&(sl.contains("2mY"))){belief="2m symmetry along Y.(C<sub>2h</sub>)";scd=52;}
  if ((sl.size()==1)&&(sl.contains("2mZ"))){belief="2m symmetry along Z.(C<sub>2h</sub>)";scd=53;}  
  if ((sl.size()==3)&&(sl.contains("2X"))&&(sl.contains("2Y"))&&(sl.contains("2Z"))){belief="222 symmetry.(D<sub>2</sub>)";scd=7;}//7
  if ((sl.size()==3)&&(sl.contains("-12mX"))&&(sl.contains("-12mY"))&&(sl.contains("-12mZ"))){belief="mmm symmetry.(D<sub>2h</sub>)";scd=8;}//8
  if ((sl.size()==1)&&(sl.contains("2-4X"))){belief="-4 symmetry along X.(S<sub>4</sub>)<br><font color=\"red\"><font color=\"red\">Please change X to Z!</font>";}//10
  if ((sl.size()==1)&&(sl.contains("2-4Y"))){belief="-4 symmetry along Y.(S<sub>4</sub>)<br><font color=\"red\"><font color=\"red\">Please change Y to Z!</font>";}//10
  if ((sl.size()==1)&&(sl.contains("2-4Z"))){belief="-4 symmetry along Z.(S<sub>4</sub>)";scd=10;}//10
  if ((sl.size()==3)&&(sl.contains("mX"))&&(sl.contains("mY"))&&(sl.contains("24Z"))) {belief="4mm symmetry.(C<sub>4v</sub>)";scd=12;}//12
  if ((sl.size()==3)&&(sl.contains("mX"))&&(sl.contains("24Y"))&&(sl.contains("mZ"))) {belief="4mm symmetry.(C<sub>4v</sub>)<br><font color=\"red\">Please change Y to Z!</font>";}//12
  if ((sl.size()==3)&&(sl.contains("24X"))&&(sl.contains("mY"))&&(sl.contains("mZ"))) {belief="4mm symmetry.(C<sub>4v</sub>)<br><font color=\"red\">Please change X to Z!</font>";}//12

  if ((sl.size()==3)&&(((sl.contains("2mX"))&&(sl.contains("2Y")))||((sl.contains("2X"))&&(sl.contains("2mY"))))&&(sl.contains("24-4Z"))) {belief="-42m symmetry.(D<sub>2d</sub>)";scd=13;}//13
  if ((sl.size()==3)&&(((sl.contains("2X"))&&(sl.contains("2mZ")))||((sl.contains("2mX"))&&(sl.contains("2Z"))))&&(sl.contains("24-4Y"))) {belief="-42m symmetry.(D<sub>2d</sub>)<br>Please change Y to Z!</font>";}//13
  if ((sl.size()==3)&&(((sl.contains("2Y"))&&(sl.contains("2mZ")))||((sl.contains("2mY"))&&(sl.contains("2Z"))))&&(sl.contains("24-4X"))) {belief="-42m symmetry.(D<sub>2d</sub>)<br>Please change X to Z!</font>";}//13

  if ((sl.size()==3)&&(sl.contains("2X"))&&(sl.contains("2Y"))&&(sl.contains("24Z"))) {belief="422 symmetry.(D<sub>4</sub>)";scd=14;}//14
  if ((sl.size()==3)&&(sl.contains("2X"))&&(sl.contains("24Y"))&&(sl.contains("2Z"))) {belief="422 symmetry.(D<sub>4</sub>)<br><font color=\"red\">Please change Y to Z!</font>";}//14
  if ((sl.size()==3)&&(sl.contains("24X"))&&(sl.contains("2Y"))&&(sl.contains("2Z"))) {belief="422 symmetry.(D<sub>4</sub>)<br><font color=\"red\">Please change X to Z!</font>";}//14

  if ((sl.size()==3)&&(sl.contains("-12mX"))&&(sl.contains("-12mY"))&&(sl.contains("-12m4Z"))) {belief="4/mmm symmetry.(D<sub>4h</sub>)";scd=15;}//15
  if ((sl.size()==3)&&(sl.contains("-12mX"))&&(sl.contains("-12m4Y"))&&(sl.contains("-12mZ"))) {belief="4/mmm symmetry.(D<sub>4h</sub>)<br><font color=\"red\">Please change Y to Z!</font>";}//15
  if ((sl.size()==3)&&(sl.contains("-12m4X"))&&(sl.contains("-12mY"))&&(sl.contains("-12mZ"))) {belief="$/mmm symmetry.(D<sub>4h</sub>)<br><font color=\"red\">Please change X to Z!</font>";}//15

  if ((sl.size()==3)&&(sl.contains("-1X"))&&(sl.contains("-1Y"))&&(sl.contains("-13-3Z"))) {belief="-3 symmetry.(S<sub>6</sub>)";scd=17;}//17
  if ((sl.size()==3)&&(sl.contains("-1X"))&&(sl.contains("-13-3Y"))&&(sl.contains("-1Z"))) {belief="-3 symmetry.(S<sub>6</sub>)<br><font color=\"red\">Please change Y to Z!</font>";}//17
  if ((sl.size()==3)&&(sl.contains("-13-3X"))&&(sl.contains("-1Y"))&&(sl.contains("-1Z"))) {belief="-3 symmetry.(S<sub>6</sub>)<br><font color=\"red\">Please change X to Z!</font>";}//17

  if ((sl.size()==1)&&(sl.contains("-12m4-4X"))){belief="4/m symmetry along X.(C<sub>4h</sub>)<br><font color=\"red\">Please change X to Z!</font>";}//11
  if ((sl.size()==1)&&(sl.contains("-12m4-4Y"))){belief="4/m symmetry along Y.(C<sub>4h</sub>)<br><font color=\"red\">Please change Y to Z!</font>";}//11
  if ((sl.size()==1)&&(sl.contains("-12m4-4Z"))){belief="4/m symmetry along Z.(C<sub>4h</sub>)";scd=11;}//11

  if ((sl.size()==1)&&(sl.contains("3X"))){belief="3 fold symmetry along X.(C<sub>3</sub>)<br><font color=\"red\">Please change X to Z!</font>";}//16
  if ((sl.size()==1)&&(sl.contains("3Y"))){belief="3 fold symmetry along Y.(C<sub>3</sub>)<br><font color=\"red\">Please change Y to Z!</font>";}
  if ((sl.size()==1)&&(sl.contains("3Z"))){belief="3 fold symmetry along Z.(C<sub>3</sub>)";scd=16;}
  if ((sl.size()==1)&&(sl.contains("24X"))){belief="4 fold symmetry along X.(C<sub>4</sub>)<br><font color=\"red\">Please change X to Z!</font>";}//9
  if ((sl.size()==1)&&(sl.contains("24Y"))){belief="4 fold symmetry along Y.(C<sub>4</sub>)<br><font color=\"red\">Please change Y to Z!</font>";}
  if ((sl.size()==1)&&(sl.contains("24Z"))){belief="4 fold symmetry along Z.(C<sub>4</sub>)";scd=9;}
  if ((sl.size()==2)&&(sl.contains("3X"))&&((sl.contains("mY"))||(sl.contains("mZ")))){belief="3m symmetry along X.(C<sub>3v</sub>)<br><font color=\"red\">Please change X to Z!</font>";}//18
  if ((sl.size()==2)&&(sl.contains("3Y"))&&((sl.contains("mZ"))||(sl.contains("mX")))){belief="3m symmetry along Y.(C<sub>3v</sub>)<br><font color=\"red\">Please change Y to Z!</font>";}//18
  if ((sl.size()==2)&&(sl.contains("3Z"))&&(sl.contains("mX"))){belief="3m symmetry along Z.(C<sub>3v</sub>)";scd=18;}//18
  if ((sl.size()==2)&&(sl.contains("3Z"))&&(sl.contains("mY"))){belief="3m symmetry along Z.(C<sub>3v</sub>)";scd=19;}//18

  if ((sl.size()==3)&&(sl.contains("234Z"))&&(sl.contains("mX"))&&(sl.contains("mY"))) {belief="cylindrical symmetry along Z.(C<sub>&infin;</sub>)";scd=888;}//nicht da
  if ((sl.size()==3)&&(sl.contains("234Y"))&&(sl.contains("mX"))&&(sl.contains("mZ"))) {belief="cylindrical symmetry along Y.(C<sub>&infin;</sub>)<br><font color=\"red\">Please change Y to Z!</font>";}
  if ((sl.size()==3)&&(sl.contains("234X"))&&(sl.contains("mY"))&&(sl.contains("mZ"))) {belief="cylindrical symmetry along X.(C<sub>&infin;</sub>)<br><font color=\"red\">Please change X to Z!</font>";}

  if ((sl.size()==3)&&(sl.contains("m3X"))&&(((sl.contains("mY"))&&(sl.contains("2Z")))||((sl.contains("mZ"))&&(sl.contains("2Y"))))) {belief="-6m2 symmetry along X.(D<sub>3h</sub>)<br><font color=\"red\">Please change X to Z!</font>";;}//24
  if ((sl.size()==3)&&(sl.contains("m3Y"))&&(((sl.contains("mZ"))&&(sl.contains("2X")))||((sl.contains("mX"))&&(sl.contains("2Z"))))) {belief="-6m2 symmetry along Y.(D<sub>3h</sub>)<br><font color=\"red\">Please change Y to Z!</font>";}
  if ((sl.size()==3)&&(sl.contains("m3Z"))&&(((sl.contains("mX"))&&(sl.contains("2Y")))||((sl.contains("mY"))&&(sl.contains("2X"))))) {belief="-6m2 symmetry along Z.(D<sub>3h</sub>)";scd=24;}

  if ((sl.size()==3)&&(sl.contains("-13X"))&&(((sl.contains("-12Z"))&&(sl.contains("-1mY")))||((sl.contains("-1mZ"))&&(sl.contains("-12Y"))))) {belief="-3m symmetry along X.(D<sub>3d</sub>)<br><font color=\"red\">Please change X to Z!</font>";}//20
  if ((sl.size()==3)&&(sl.contains("-13Y"))&&(((sl.contains("-12X"))&&(sl.contains("-1mZ")))||((sl.contains("-1mX"))&&(sl.contains("-12Z"))))) {belief="-3m symmetry along Y.(D<sub>3d</sub>)<br><font color=\"red\">Please change Y to Z!</font>";}//20
  if ((sl.size()==3)&&(sl.contains("-13Z"))&&(((sl.contains("-12Y"))&&(sl.contains("-1mX")))||((sl.contains("-1mY"))&&(sl.contains("-12X"))))) {belief="-3m symmetry along Z.(D<sub>3d</sub>)";scd=20;}//20
  switch (scd) {
  case -1:keyline.replace(46,30,QString("10 000 11111 0000000 111111111"));break;//-1
  case  1:keyline.replace(46,30,QString("10 111 11111 1111111 111111111"));break;//1
  case 31:keyline.replace(46,30,QString("10 011 10110 1011001 101100110"));break;//mx
  case 32:keyline.replace(46,30,QString("10 101 11010 1101010 110101010"));break;//my
  case 33:keyline.replace(46,30,QString("10 110 10011 0110011 100110011"));break;//mz
  case 41:keyline.replace(46,30,QString("10 100 10110 1100110 101100110"));break;//2x
  case 42:keyline.replace(46,30,QString("10 010 11010 1010101 110101010"));break;//2y
  case 43:keyline.replace(46,30,QString("10 001 10011 1001100 100110011"));break;//2z
  case 51:keyline.replace(46,30,QString("10 000 10110 1000000 101100110"));break;
  case 52:keyline.replace(46,30,QString("10 000 11010 1000000 110101010"));break;
  case 53:keyline.replace(46,30,QString("10 000 10011 0000000 100110011"));break;
  case  6:keyline.replace(46,30,QString("10 001 10010 1001000 100100010"));break;
  case  7:keyline.replace(46,30,QString("10 000 10010 0000100 100100010"));break;
  case  8:keyline.replace(46,30,QString("10 000 10010 0000000 100100010"));break;
  case  9:keyline.replace(46,30,QString("10 001 10000 1000000 100000011"));break;
  case 10:keyline.replace(46,30,QString("10 000 10000 0001100 100000011"));break;
  case 11:keyline.replace(46,30,QString("10 000 10000 0000000 100000011"));break;
  case 12:keyline.replace(46,30,QString("10 001 10000 1000000 100000010"));break;
  case 13:keyline.replace(46,30,QString("10 000 10000 1001000 100000010"));break;
  case 14:keyline.replace(46,30,QString("10 001 10000 1000000 100000010"));break;
  case 15:keyline.replace(46,30,QString("10 000 10000 0000000 100000010"));break;
  case 16:keyline.replace(46,30,QString("10 001 10000 1000011 100001100"));break;
  case 17:keyline.replace(46,30,QString("10 000 10000 0000000 100001100"));break;
  case 18:keyline.replace(46,30,QString("10 001 10000 1000001 100000100"));break;
  case 19:keyline.replace(46,30,QString("10 001 10000 1000010 100001000"));break;
  case 20:keyline.replace(46,30,QString("10 000 10000 0000000 100001000"));break;
  case 24:keyline.replace(46,30,QString("10 000 10000 0000010 100000000"));break;
  case 0: 
	  break;
  case 888:keyline.replace(46,30,QString("10 001 10000 1000000 100000000"));break;
  default:QMessageBox::critical(this,"MoleCoolMan sucks!",QString(tr("Sorry but there is another programming bug!")),QMessageBox::Ok); 
  }
  messi.setText("Maybe there is<br><b>"+belief+"</b><br>"+text+"<br>"+((scd)?"":"<font color=red size=+3>Key line will not be changed!</font>"));
  keyLineEdit->setText(keyline);
}
QString  xdEditDlg::checkSymmOnAx(V3 ax, double TOL){
  QString st="";
  M9 dm;
  CEnvironment sym(*chm);
  CEnvironment asym(*chm);
  V3 sp;
  int relatm=0;
  int cend=chm->size();;
  for (int j=0;j<cend;j++){ 
    if (asym[j].an>-1)relatm++;
    asym[j].pos+=(chm->at(0).pos*-1.0);
  }
  for (int i=1; i<5; i++){
    dm[0]=C+ax.x*ax.x*_C;
    dm[1]=-ax.z*S+ax.x*ax.y*_C;
    dm[2]= ax.y*S+ax.x*ax.z*_C;
    dm[3]= ax.z*S+ax.x*ax.y*_C;
    dm[4]=C+ax.y*ax.y*_C;
    dm[5]=-ax.x*S+ax.z*ax.y*_C;
    dm[6]=-ax.y*S+ax.x*ax.z*_C;
    dm[7]= ax.x*S+ax.z*ax.y*_C;
    dm[8]=C+ax.z*ax.z*_C;
    int same=0;
    double different=0.0;
    if (i>1){
    for (int j=0;j<cend;j++){      
      rot_(dm,asym.at(j).pos,sym[j].pos);
    }
    
    for (int j=0;j<cend;j++){
      for (int k=0;k<cend;k++){
	if ((sym.at(j).an>-1)&&(asym.at(k).an>-1)&&
	    (sym.at(j).an==chm->at(k).an)&&
	    (Distance(sym.at(j).pos,asym.at(k).pos)<TOL)){
	  same++;
	  different+=sqrt(Distance(sym.at(j).pos,asym.at(k).pos))*i;
	}//if
      }//k
    }//j
    if (same==relatm) st+=QString("%1").arg(i); 
    }
    same=0;
    different=0.0;
    dm[0]*=-1;
    dm[1]*=-1;
    dm[2]*=-1;
    dm[3]*=-1;
    dm[4]*=-1;
    dm[5]*=-1;
    dm[6]*=-1;
    dm[7]*=-1;
    dm[8]*=-1;
    for (int j=0;j<cend;j++){      
      rot_(dm,asym.at(j).pos,sym[j].pos);
    }
    for (int j=0;j<cend;j++){
      for (int k=0;k<cend;k++){
	if ((sym.at(j).an>-1)&&(asym.at(k).an>-1)&&
	    (sym.at(j).an==asym.at(k).an)&&
	    (Distance(sym.at(j).pos,asym.at(k).pos)<TOL)){
	  same++;
	  different+=sqrt(Distance(sym.at(j).pos,asym.at(k).pos))*i;
	}//if
      }//k
    }//j
    if ((same==relatm)&&(i!=2)) st+=QString("-%1").arg(i);  
    if ((same==relatm)&&(i==2)) st+=QString("m");  
    
  }//i
  return st;
}//double
void XDDlg::draw(){
  double L=20;
  V3 vec;
  double wink;
  glPushMatrix();
  glScaled( L, L, L );
  glColor4f(0.6,0.6,0.6,1.0);
  for (int i=0; i<cl.size();i++){
    vec.x=(cl.at(i).ato2->pos.y-cl.at(i).ato1->pos.y);
    vec.y=(cl.at(i).ato1->pos.x-cl.at(i).ato2->pos.x);
    vec.z=0;
    vec=Normalize(vec);
    glPushMatrix();	
    glTranslated (mitsav.x,mitsav.y,mitsav.z);
    glTranslated (cl.at(i).ato2->pos.x,cl.at(i).ato2->pos.y,cl.at(i).ato2->pos.z);//Anfangspunkt 	
    wink=acos(((cl.at(i).ato1->pos.z-cl.at(i).ato2->pos.z)/
	       (sqrt((cl.at(i).ato1->pos.x-cl.at(i).ato2->pos.x)*(cl.at(i).ato1->pos.x-cl.at(i).ato2->pos.x)+
		     (cl.at(i).ato1->pos.y-cl.at(i).ato2->pos.y)*(cl.at(i).ato1->pos.y-cl.at(i).ato2->pos.y)+
		     (cl.at(i).ato1->pos.z-cl.at(i).ato2->pos.z)*(cl.at(i).ato1->pos.z-cl.at(i).ato2->pos.z)))))/M_PI*180.0;
    glRotated(wink,vec.x,vec.y,vec.z); // drehen
    GLUquadricObj *q = gluNewQuadric();
    gluQuadricNormals(q, GL_SMOOTH);   // ein Zylinder
    gluQuadricTexture(q,GL_TRUE);
    gluCylinder(q, 0.05, 0.05, (float)cl.at(i).length, 8, 5); 
    glPopMatrix();
  }
    for (int i=0;i<ce.size();i++){
      extern molekul  mol;
      glPushMatrix () ; 
      double rad=0;
      if (ce.at(i).an>=0) rad=mol.Kovalenz_Radien[ce.at(i).an]/(10*L);              
      rad+=0.1;      
      glTranslated (mitsav.x,mitsav.y,mitsav.z);
      glTranslated(ce.at(i).pos.x, ce.at(i).pos.y, ce.at(i).pos.z) ; 
      glScaled(rad,rad,rad);
      GLUquadricObj *q = gluNewQuadric();
      gluQuadricNormals(q, GL_SMOOTH);   
      gluQuadricTexture(q,GL_TRUE);    
    glColor4fv(mol.Acol[ce.at(i).an]); 
    if (ce.at(i).Label.contains("DU",Qt::CaseInsensitive)) glColor4f(0.5f,0.3f,0.5f,0.4f); 
    if (ce.at(i).Label.contains("HL",Qt::CaseInsensitive)) glColor4f(0.95f,0.0f,0.0f,0.4f); 
    if (ce.at(i).Label.contains("PK",Qt::CaseInsensitive)) glColor4f(0.0f,0.0f,0.95f,0.4f); 
    if (ce.at(i).Label.contains("CP",Qt::CaseInsensitive)) glColor3f(0.0,0.3,0.1);
    gluSphere(q,1.0,48,48);//Atom als Kugel zeichnen
    glPopMatrix();
    }
    QFont myFont= QFont("Arial", 14, -1, false);  
    glDisable(GL_LIGHTING);
    glDisable( GL_DEPTH_TEST ); 
    glPushMatrix();
    glBegin(GL_LINES);
    for (int i=0;i<3;i++){      
      if (locCo.at(i).Label=="Y") glColor4f(0.0f,1.0f,0.0f,0.5f);
      else if (locCo.at(i).Label=="Z") glColor4f(0.0f,0.0f,1.0f,0.5f);
      else glColor4f(1.0f,0.0f,0.0f,0.5f);
      glVertex3d(mitsav.x+ce.at(0).pos.x,mitsav.y+ce.at(0).pos.y,mitsav.z+ce.at(0).pos.z);
      glVertex3d(mitsav.x+locCo.at(i).pos.x,
		 mitsav.y+locCo.at(i).pos.y,
		 mitsav.z+locCo.at(i).pos.z);
    }
    glEnd();
    for (int i=0; i<3;i++){
      glEnable( GL_DEPTH_TEST ); 
      glEnable(GL_LIGHTING);
      if (locCo.at(i).Label=="Y") glColor4f(0.0f,1.0f,0.0f,1.0f);
      else if (locCo.at(i).Label=="Z") glColor4f(0.0f,0.0f,1.0f,1.0f);
      else glColor4f(1.0f,0.0f,0.0f,1.0f);
      glPushMatrix();	
      glTranslated (mitsav.x,mitsav.y,mitsav.z);
      glTranslated (locCo.at(i).pos.x,
		    locCo.at(i).pos.y,
		    locCo.at(i).pos.z);//Anfangspunkt 	      
      V3 xp,yp,zp;
      xp=(locCo.at((i+1)%3).pos-ce.at(0).pos);
      yp=(locCo.at((i+2)%3).pos-ce.at(0).pos);
      zp=(locCo.at((i+0)%3).pos-ce.at(0).pos);
      xp*=1.0/2.5;
      yp*=1.0/2.5;
      zp*=1.0/2.5;
      double DM[16];
      DM[0 ]=xp.x;
      DM[1 ]=xp.y;
      DM[2 ]=xp.z;
      DM[3 ]=0;

      DM[4 ]=yp.x;
      DM[5 ]=yp.y;
      DM[6 ]=yp.z;
      DM[7 ]=0;

      DM[8 ]=zp.x;
      DM[9 ]=zp.y;
      DM[10]=zp.z;
      DM[11]=0;

      DM[12]=0;
      DM[13]=0;
      DM[14]=0;
      DM[15]=1;
      glMultMatrixd(DM);
    GLUquadricObj *q = gluNewQuadric();
    gluQuadricNormals(q, GL_SMOOTH);   // ein Zylinder
    gluQuadricTexture(q,GL_TRUE);   
    gluCylinder(q, 0.1, 0.0002, 0.5f, 15, 15); 
    gluDisk(q,0,0.1,15,15);
    glPopMatrix();
  }
    glColor3f(1.0,1.0,1.0);
    glDisable( GL_DEPTH_TEST ); 
    for (int i=0;i<3;i++){ 
    renderText( mitsav.x+locCo.at(i).pos.x,
		mitsav.y+locCo.at(i).pos.y,
		mitsav.z+locCo.at(i).pos.z, locCo.at(i).Label,myFont);
    }
    glPopMatrix();
    glEnable( GL_DEPTH_TEST ); 
    glEnable(GL_LIGHTING);
  glPushMatrix();
  glClear( GL_DEPTH_BUFFER_BIT);
  myFont= QFont("Arial", 24, -1, false);  

  GLdouble model[16];
  GLdouble proj[16];
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev( GL_PROJECTION_MATRIX , (double*)proj );
  glGetDoublev( GL_MODELVIEW_MATRIX, (double*)model );
  for (int j=0;j<ce.size();j++){
    if (imFokus==j)  glColor3f(0.4,0.9,0.2); else glColor3f(0.0,0.0,0.0);
    renderText( ce.at(j).pos.x+mitsav.x,ce.at(j).pos.y+mitsav.y,ce.at(j).pos.z+mitsav.z, ce.at(j).Label,myFont);
    if (!posTo2D(ce.at(j).pos+mitsav,model,proj,viewport, &ce[j].screenX, &ce[j].screenY))
    {ce[j].screenX=-200; ce[j].screenY=-200;}

  }
  glPopMatrix();
  glPopMatrix();
}
CEnvironment xdEditDlg::calcAxis(QString masstr){
  MyAtom xp,yp,zp;
  MyAtom at0,at1,at2;
  reststr="  ";
  int find=0;
  int lastatom=0;
  QStringList parsed=masstr.split(" ",QString::SkipEmptyParts);
  for (int i=7; i<parsed.size();i++) reststr+=parsed.at(i)+"  "+((i==8)?" ":(i==9)?" ":"");
  for (int i=0; i<alle.size();i++) if (!alle.at(i).Label.compare(parsed.at(0),Qt::CaseInsensitive))
  {atooo=i+1;ato=alle.at(i);find=1;break;}
  if (!find) {
    for (int i=0; i<alle.size();i++) if (Distance(chm->at(0).pos,alle.at(i).pos)<0.1) alle.removeAt(i);
    for (int i=0; i<alle.size();i++) {if (alle.at(i).an==chm->at(0).an) lastatom=i+1; }
    alle.insert(lastatom,chm->at(0));
    atooo=lastatom+1;
    ato=alle.at(lastatom);
  }
  find=0;
  for (int i=0; i<alle.size();i++) if (!alle.at(i).Label.compare(parsed.at(1),Qt::CaseInsensitive))
  {at0=alle.at(i);iax1=i;find=1;break;}
  for (int i=0; i<alle.size();i++) if (!alle.at(i).Label.compare(parsed.at(3),Qt::CaseInsensitive))
  {at1=alle.at(i);find=1;break;}
  for (int i=0; i<alle.size();i++) if (!alle.at(i).Label.compare(parsed.at(4),Qt::CaseInsensitive))
  {at2=alle.at(i);iax2=i;find=1;break;}
  ic1=0;
  ic2=1;
//  for (int i=0; i<alle.size();i++) { printf("%s %d %d\n",alle.at(i).Label.toStdString().c_str(),i,alle.at(i).an);}
  const char axl[3][2]={"X","Y","Z"};
  for (int i=0;i<3;i++) if (parsed.at(2)==axl[i]) ic1=i;
  for (int i=0;i<3;i++) if (parsed.at(5)==axl[i]) ic2=i;
  xp.Label=parsed.at(2);
  xp.an=-1;
  xp.pos=Normalize(at0.pos-ato.pos);  
  yp.Label=parsed.at(5);
  yp.an=-1;
  yp.pos=Normalize(at2.pos-at1.pos);  
  zp.Label=QString(axl[3-ic1-ic2]);
  zp.an=-1;
  zp.pos=Normalize(xp.pos%yp.pos);  
  yp.pos=Normalize(zp.pos%xp.pos);
  if (parsed.at(6)=="L") zp.pos*=-1;
  QString symmdiag="";
  double tolerance=(tol.value()>0.000001 )?tol.value():0.03;
  QString xxs=checkSymmOnAx(xp.pos,tolerance);
  QString yys=checkSymmOnAx(yp.pos,tolerance);
  QString zzs=checkSymmOnAx(zp.pos,tolerance);
  if (xxs.length()) symmdiag= xxs+xp.Label+" ";
  if (yys.length()) symmdiag+=yys+yp.Label+" ";
  if (zzs.length()) symmdiag+=zzs+zp.Label+" ";
  molman(symmdiag);
  symmetryReducer(symmdiag);
  xp.pos*=2.5;
  yp.pos*=2.5;
  zp.pos*=2.5;
  xp.pos+=ato.pos;
  yp.pos+=ato.pos;
  zp.pos+=ato.pos;
  CEnvironment localco;
  localco.append(xp);
  localco.append(yp);
  localco.append(zp);
  return localco;
}
void xdEditDlg::setKeyLine(const QString &s){
  keyline=s;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
xdEditDlg::xdEditDlg(CEnvironment *ch,const Connection *cll,CEnvironment *all,MyAtom orgAtom): QDialog(){
  oa=orgAtom;
  keyLineEdit = new QLineEdit("",this);
  connect(keyLineEdit, SIGNAL(textChanged(const QString &)),
	  this, SLOT(setKeyLine(const QString &)));
  setMinimumSize(1050,400);
  chm=ch;
  alle=*all;
  duminuse.clear();
  allLab.clear();
  keyline=QString("no noit!");
  statlab.setText("");
  
  QFont font;
  font.setFamily("Courier");
  font.setFixedPitch(true);
  font.setPointSize(10);  
  for (int i=0; i<alle.size();i++) allLab.append(alle.at(i).Label);
   QFile mas("xd.mas");
   mas.open(QIODevice::ReadOnly|QIODevice::Text);
   QString allM = mas.readAll();
   mas.close();
   QStringList lnes= allM.split("\n",QString::SkipEmptyParts);
   bool an=false;
   int itbll=1;
   extern molekul mol;
   for (int i =0; i<lnes.size(); i++){
     if (lnes.at(i).startsWith("END SCAT")) an=false;
     if ((an)&&(lnes.at(i).contains(QRegExp("^[A-Za-z]+")))) {
       int sum=0;
       QStringList nnn=lnes.at(i).split(' ',QString::SkipEmptyParts);
       if (nnn.size()>22) {
	 for (int kk=4; kk<22; kk++) sum+=(nnn.at(kk).toInt()<0)?-nnn.at(kk).toInt():0;
         monocharge[itbll]=sum;
//	 printf("CHAR %d \n",sum);
       }


       atypen[itbll++]=mol.Get_OZ(lnes.at(i).section(' ',0,0)); 
     }
     if (lnes.at(i).startsWith("SCAT")) an=true;
   }
   allM = allM.remove(QRegExp("![^\n]*"));
   allM = allM.remove(QRegExp("^.{1,}CHEMCON"));

   QStringList lines= allM.split("\n",QString::SkipEmptyParts);
   for (int i=0; i<lines.size();i++) lines[i]=lines.at(i).trimmed();

   QStringList alteDummys= lines.filter(QRegExp("^DUM\\d{1,}"));
  for (int i=0; i<alteDummys.size();i++) {
    alteDummys[i].remove(QRegExp("\\s+.{1,}$"));
    lines.replaceInStrings(alteDummys.at(i),QString("DUM_%1").arg(i),Qt::CaseInsensitive );
  }
  lines.replaceInStrings("DUM_","DUM",Qt::CaseInsensitive);
   int i0, i2;
   for (i0=0; ((!(lines.at(i0).startsWith("DUM",Qt::CaseInsensitive)))&&(!(lines.at(i0).startsWith("END",Qt::CaseInsensitive)))); i0++){
   QStringList words=lines.at(i0).split(" ",QString::SkipEmptyParts);
   if (words.size()){
   if (words.at(1).startsWith("DUM",Qt::CaseInsensitive)) duminuse.append(words.at(1));
   if (words.at(4).startsWith("DUM",Qt::CaseInsensitive)) duminuse.append(words.at(4));
   }
   if (lines.at(i0).startsWith(chm->at(0).Label,Qt::CaseInsensitive)) sLabel.setText(lines.at(i0));
   }
   if (chm->size()>3) {if (sLabel.text().isEmpty()) sLabel.setText(QString("%1 %2 Z  %1 %3     X   R   2  3   1   4  NO").arg(chm->at(0).Label).arg(chm->at(1).Label).arg(chm->at(2).Label));}
   else {if (sLabel.text().isEmpty()) sLabel.setText(QString("%1 %2 Z  %1 %3     X   R   2  3   1   4  NO").arg(chm->at(0).Label).arg(alle.at(1).Label).arg(alle.at(2).Label));}

   for (i2=i0; (!(lines.at(i2).startsWith("KEY",Qt::CaseInsensitive))) ;i2++){}
   do {
     i2++; 
     keyline=lines.at(i2);
   }while (((i2-1)<lines.size())&&(!(lines.at(i2).startsWith("END")))&&(!(lines.at(i2).startsWith(chm->at(0).Label,Qt::CaseInsensitive)))); 
   if (keyline.startsWith("END")) keyline = QString("%1 111 111111 0000000000 000000000000000 10 111 11111 1111111 111111111").arg(chm->at(0).Label,-7);
   for (i2=i0; (!(lines.at(i2).startsWith("KEY",Qt::CaseInsensitive))) ;i2++){}
  QVBoxLayout *srl= new QVBoxLayout();
   srl->setAlignment(Qt::AlignTop);
   X_1 = new QCheckBox("-1 'along' X");srl->addWidget(X_1); X_1 ->setToolTip(X_1 ->text());
   X2  = new QCheckBox("2 along X");  srl->addWidget(X2);   X2  ->setToolTip(X2  ->text());
   Xm  = new QCheckBox("m over X");   srl->addWidget(Xm);   Xm  ->setToolTip(Xm  ->text());
   X_3 = new QCheckBox("-3 along X"); srl->addWidget(X_3);  X_3 ->setToolTip(X_3 ->text());
   X_4 = new QCheckBox("-4 along X"); srl->addWidget(X_4);  X_4 ->setToolTip(X_4 ->text());
   X3  = new QCheckBox("3 along X");  srl->addWidget(X3);   X3  ->setToolTip(X3  ->text());
   X4  = new QCheckBox("4 along X");  srl->addWidget(X4);   X4  ->setToolTip(X4  ->text());
                                                                                 
   Y_1 = new QCheckBox("-1 'along' Y");srl->addWidget(Y_1); Y_1 ->setToolTip(Y_1 ->text());
   Y2  = new QCheckBox("2 along Y");  srl->addWidget(Y2);   Y2  ->setToolTip(Y2  ->text());
   Ym  = new QCheckBox("m over Y");   srl->addWidget(Ym);   Ym  ->setToolTip(Ym  ->text());
   Y_3 = new QCheckBox("-3 along Y"); srl->addWidget(Y_3);  Y_3 ->setToolTip(Y_3 ->text());
   Y_4 = new QCheckBox("-4 along Y"); srl->addWidget(Y_4);  Y_4 ->setToolTip(Y_4 ->text());
   Y3  = new QCheckBox("3 along Y");  srl->addWidget(Y3);   Y3  ->setToolTip(Y3  ->text());
   Y4  = new QCheckBox("4 along Y");  srl->addWidget(Y4);   Y4  ->setToolTip(Y4  ->text());
                                                                                 
   Z_1 = new QCheckBox("-1 'along' Z");srl->addWidget(Z_1); Z_1 ->setToolTip(Z_1 ->text());
   Z2  = new QCheckBox("2 along Z");  srl->addWidget(Z2);   Z2  ->setToolTip(Z2  ->text());
   Zm  = new QCheckBox("m over Z");   srl->addWidget(Zm);   Zm  ->setToolTip(Zm  ->text());
   Z_3 = new QCheckBox("-3 along Z"); srl->addWidget(Z_3);  Z_3 ->setToolTip(Z_3 ->text());
   Z_4 = new QCheckBox("-4 along Z"); srl->addWidget(Z_4);  Z_4 ->setToolTip(Z_4 ->text());
   Z3  = new QCheckBox("3 along Z");  srl->addWidget(Z3);   Z3  ->setToolTip(Z3  ->text());
   Z4  = new QCheckBox("4 along Z");  srl->addWidget(Z4);   Z4  ->setToolTip(Z4  ->text());
   
   connect(X_1 ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(X2  ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(Xm  ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(X_3 ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(X_4 ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(X3  ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(X4  ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   
   connect(Y_1 ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(Y2  ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(Ym  ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(Y_3 ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(Y_4 ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(Y3  ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(Y4  ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   
   connect(Z_1 ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(Z2  ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(Zm  ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(Z_3 ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(Z_4 ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(Z3  ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));
   connect(Z4  ,SIGNAL(clicked(bool)),this,SLOT(updatesLabel2()));

  sRGroupBox = new QGroupBox("Symmetry elements"); 
  sRGroupBox->setLayout(srl);

  locCo=calcAxis(sLabel.text());

  id= new  XDDlg(*ch,*cll,locCo,&alle,this);
  id->duminuse=duminuse;
  lhand =new QCheckBox ("Left handed");
  lhand->setCheckState(Qt::Unchecked);
  QGridLayout *grid= new QGridLayout ;
  QHBoxLayout *qhbl2=new QHBoxLayout();
  QHBoxLayout *qhbl3=new QHBoxLayout();
  QHBoxLayout *dummyCreate=new QHBoxLayout();
  qhbl2->addWidget(&sLabel);
  qhbl2->addWidget(lhand);
  ax1lab=new QComboBox;
  ax2lab=new QComboBox;
  ax1lab->addItems(allLab);
  ax1lab->setCurrentIndex(iax1);
  ax2lab->addItems(allLab);
  ax2lab->setCurrentIndex(iax2);
  ax1type = new QComboBox;
  ax1type->addItem(("X"),0);
  ax1type->addItem(("Y"),1);
  ax1type->addItem(("Z"),2);	
  ax1type->setCurrentIndex(ic1);
  ax2type = new QComboBox;
  ax2type->addItem(("X"),0);
  ax2type->addItem(("Y"),1);
  ax2type->addItem(("Z"),2);	
  ax2type->setCurrentIndex(ic2);
  qhbl3->addWidget(ax1lab);
  qhbl3->addWidget(ax1type);
  qhbl3->addWidget(ax2lab);		
  qhbl3->addWidget(ax2type);

  QGroupBox *horizontalGroupBox3 = new QGroupBox("Coordinate system changer");

  horizontalGroupBox3->setLayout(qhbl3);
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  QGroupBox *horizontalGroupBox2 = new QGroupBox("xd.mas line:");
  QPushButton *lincomb = new QPushButton("Create a linear-combination dummy");
  QPushButton *xprod = new QPushButton("Create a cross-product dummy");
  //QPushButton *killdu = new QPushButton("Kill a dummy");

  dummyCreate->addWidget(lincomb);
  dummyCreate->addWidget(xprod);
  //dummyCreate->addWidget(killdu);
  QGroupBox *dummybox = new QGroupBox("Create or kill DUMMYS");
  dummybox->setLayout(dummyCreate);
  horizontalGroupBox2->setLayout(qhbl2);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  grid->setColumnMinimumWidth(0,400);
  grid->setColumnStretch(0,400);
  grid->setColumnMinimumWidth(1,700);
  grid->setColumnStretch(1,0);
  statbar= new  QStatusBar();
  statbar->setSizeGripEnabled(false);
  statbar->addPermanentWidget(&statlab);
  tol.setMinimum(0.00001);
  tol.setMaximum(0.8);
  tol.setSingleStep(0.01);
  tol.setDecimals(6);
  tol.setValue(0.03);
  QVBoxLayout *qhbl4=new QVBoxLayout();
  qhbl4->addWidget(&tol);
  QGroupBox *symmTolBox = new QGroupBox("Symmetry tolerance:");
  symmTolBox->setLayout(qhbl4);
  messi.setFrameShape(QFrame::WinPanel);
  messi.setFrameShadow(QFrame::Sunken);
  messi.setAlignment(Qt::AlignTop|Qt::AlignLeft);
  messi.clear(); 
  grid->addWidget(id                    ,0,0,6,1);
  grid->addWidget(horizontalGroupBox3   ,0,1);
  grid->addWidget(horizontalGroupBox2   ,1,1);
  grid->addWidget(keyLineEdit           ,2,1);
  grid->addWidget(&messi                ,3,1,2,1);
  grid->addWidget(dummybox              ,5,1);  
  grid->addWidget(statbar               ,6,0,1,2);
  grid->addWidget(symmTolBox            ,5,2,1,1);
  grid->addWidget(buttonBox             ,6,2);

  grid->addWidget(sRGroupBox            ,0,2,5,1) ;
  setLayout(grid);
  connect(id,SIGNAL(message(const QString&)),this,SLOT(updateStatusBar(const QString&)));
  connect(id,SIGNAL(addatom(const MyAtom &)),this,SLOT(updateAtoms(const MyAtom&)));
  connect(id,SIGNAL(delatom(const MyAtom &)),this,SLOT(updateAtoms2(const MyAtom&)));
  connect(lincomb,SIGNAL(clicked(bool)),id,SLOT(setLinMode()));
  connect(xprod,SIGNAL(clicked(bool)),id,SLOT(setXprMode()));
  //connect(killdu,SIGNAL(clicked(bool)),id,SLOT(killDuMode()));
  connect(lhand,SIGNAL(stateChanged(int)),this,SLOT(updatesLabel()));
  connect(&tol,SIGNAL(valueChanged(double)),this,SLOT(updatesLabel()));
  connect(ax1lab, SIGNAL(currentIndexChanged(int)),this,SLOT(updatesLabel()));
  connect(ax2lab, SIGNAL(currentIndexChanged(int)),this,SLOT(updatesLabel()));
  connect(ax1type,SIGNAL(currentIndexChanged(int)),this,SLOT(updatesLabel()));
  connect(ax2type,SIGNAL(currentIndexChanged(int)),this,SLOT(updatesLabel()));
  setWindowTitle("XD Setup Dialog");
  updatesLabel();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void xdEditDlg::updateStatusBar(const QString& s){
  statbar->showMessage(s);
}
void xdEditDlg::updateAtoms(const MyAtom& atm){
  ax1lab->addItem(atm.Label);
  ax2lab->addItem(atm.Label);
  alle.append(atm);
  if (atm.an==-1)duminuse.append(atm.Label);
}
void xdEditDlg::updateAtoms2(const MyAtom& atm){
  ax1lab->removeItem(ax1lab->findText(atm.Label));
  ax2lab->removeItem(ax2lab->findText(atm.Label));
  
}
void xdEditDlg::updatesLabel(){
  if (ax1lab->currentText()==chm->at(0).Label) {
    qDebug()<<"Starting atom "<<chm->at(0).Label<<" and axis 1 direction "<<ax1lab->currentText()<<" can not be the same!";
    ax1lab->setCurrentIndex(((ax1lab->currentIndex()+1)<ax1lab->count())?ax1lab->currentIndex()+1:ax1lab->currentIndex()-1);
    return;
  }
  if (ax2lab->currentText()==chm->at(0).Label) {
    qDebug()<<"Starting atom "<<chm->at(0).Label<<" and axis 2 direction "<<ax2lab->currentText()<<" can not be the same!";
    ax2lab->setCurrentIndex(((ax2lab->currentIndex()+1)<ax2lab->count())?ax2lab->currentIndex()+1:ax2lab->currentIndex()-1);
    return;
  }
  QString LH=(lhand->checkState () ==Qt::Checked)?"  L ":"  R ";
  if(ax1lab->currentIndex()==ax2lab->currentIndex()) ax2lab->setCurrentIndex((ax2lab->currentIndex()+1)%alle.size());
  if(ax1lab->currentText()==ato.Label) ax2lab->setCurrentIndex((ax2lab->currentIndex()+1)%alle.size());
  if(ato.Label==ax2lab->currentText()) ax2lab->setCurrentIndex((ax2lab->currentIndex()+1)%alle.size());
  if(ax1type->currentIndex()==ax2type->currentIndex()) ax2type->setCurrentIndex((ax2type->currentIndex()+1)%3);
  QString text=QString("%1 %2 %3 %1 %4 %5 %6 %7 ")
	  .arg(chm->at(0).Label,-8)
	  .arg(ax1lab->currentText(),-8)
	  .arg(ax1type->currentText(),-3)
	  .arg(ax2lab->currentText(),-8)
	  .arg(ax2type->currentText(),-3)
	  .arg(LH,5)
	  .arg(reststr); 

  sLabel.setText(text);
  locCo.clear();
  locCo=calcAxis(text);
  id->locCo=locCo;
  id->updateGL();
  update();
}
void xdEditDlg::updatesLabel2(){
  QString symmdiag;
  int test=0;
  if (X_1->isChecked()) {symmdiag.append(X_1->text());test++;}
  if (X2 ->isChecked()) {symmdiag.append(X2 ->text());test++;}
  if (Xm ->isChecked()) {symmdiag.append(Xm ->text());test++;}
  if (X3 ->isChecked()) {symmdiag.append(X3 ->text());test++;}
  if (X4 ->isChecked()) {symmdiag.append(X4 ->text());test++;}
  if (X_3->isChecked()) {symmdiag.append(X_3->text());test++;}
  if (X_4->isChecked()) {symmdiag.append(X_4->text());test++;}
  if (test) {symmdiag.append("X ");test=0;}

  if (Y_1->isChecked()) {symmdiag.append(Y_1->text());test++;}
  if (Y2 ->isChecked()) {symmdiag.append(Y2 ->text());test++;}
  if (Ym ->isChecked()) {symmdiag.append(Ym ->text());test++;}
  if (Y3 ->isChecked()) {symmdiag.append(Y3 ->text());test++;}
  if (Y4 ->isChecked()) {symmdiag.append(Y4 ->text());test++;}
  if (Y_3->isChecked()) {symmdiag.append(Y_3->text());test++;}
  if (Y_4->isChecked()) {symmdiag.append(Y_4->text());test++;}
  if (test) {symmdiag.append("Y ");test=0;}

  if (Z_1->isChecked()) {symmdiag.append(Z_1->text());test++;}
  if (Z2 ->isChecked()) {symmdiag.append(Z2 ->text());test++;}
  if (Zm ->isChecked()) {symmdiag.append(Zm ->text());test++;}
  if (Z3 ->isChecked()) {symmdiag.append(Z3 ->text());test++;}
  if (Z4 ->isChecked()) {symmdiag.append(Z4 ->text());test++;}
  if (Z_3->isChecked()) {symmdiag.append(Z_3->text());test++;}
  if (Z_4->isChecked()) {symmdiag.append(Z_4->text());test++;}
  if (test) {symmdiag.append("Z ");test=0;}
  symmdiag.remove(QRegExp("( along [XYZ])|( 'along' [XYZ])|( over [XYZ])"));
  molman(symmdiag);
  update();
}
void xdEditDlg::accept () {
    QMap<int,int> dummap;

  for (int i=0,j=0; i<alle.size();i++,j++)if (alle.at(i).an==-1){
      if (!duminuse.contains(alle.at(i).Label,Qt::CaseInsensitive)) {
          dummap.remove(i);
          alle.removeAt(i);
          i--;
     //     printf("kill\n");
      }
      dummap[j]=i;
  //    printf("--->>%d %d\n",i,j);
  }
  QFile mas("xd.mas");
  QDateTime Time = QDateTime::currentDateTime();
  // Formatieren
  QString ttext = Time.toString("hh-mm-ss-dd-MM-yyyy");
  mas.copy("xd.mas_MCQsave"+ttext);
  mas.close();
  mas.open(QIODevice::ReadOnly|QIODevice::Text);
  QStringList masli = QString(mas.readAll()).split('\n');
  mas.close();
  QStringList lsmBlock,restBlock,keyBlock,keyBlock2;
  bool lsmb = true,keyb =false;
  for (int i=0;i<masli.size();i++){
    if (lsmb) {
      lsmBlock.append(masli.at(i));
//      printf("%s\n",masli.at(i).toStdString().c_str());
    }else{
      restBlock.append(masli.at(i));
    }
    if (masli.at(i).contains("END XDLSM"))lsmb = false;
    if ((keyb)&&(!masli.at(i).contains("KAPPA"))) keyBlock.append(masli.at(i));
    if (masli.at(i).contains("KEY     XYZ --U2-- ----U3---- ------U4------- M- -D- --Q-- ---O--- ----H----")) keyb=true;
    if (masli.at(i).contains("KAPPA")) keyb=false;

  }
  QString cid;
  int na,ntmx,npolmax,nz,nto,nq,next,ncon,ntbl,ns,nv,nqq,nc,nad;
  double r1o,r2o,r1,r2,r1w,r2w,gof,sig;//r-values
  double a,b,c,d,e,f;//whgt
  RES aatm;
  QList<RES> oats,nats;
  KAP akap;
  QList<KAP> kaps; 
  double extcn[7];
  double out;
  double *sc;

  QFile res("xd.res");
  if (!res.exists()) res.setFileName("xd.inp");
  res.copy("xd.res_MCQsave"+ttext);
  res.close();
  res.open(QIODevice::ReadOnly|QIODevice::Text);
  QStringList resli = QString(res.readAll()).split('\n');
  res.close();
  int lix,mli;
  mli= lix=resli.indexOf(QRegExp("^USAGE.*"));
  QStringList usag = resli.at(lix).split(' ',QString::SkipEmptyParts);
  na      = usag.at( 1).toInt();
  ntmx    = usag.at( 2).toInt();
  npolmax = usag.at( 3).toInt();
  nz      = usag.at( 4).toInt();
  nto     = usag.at( 5).toInt();
  nq      = usag.at( 6).toInt();
  next    = usag.at( 7).toInt();
  ncon    = usag.at( 8).toInt();
  ntbl    = usag.at( 9).toInt();
  ns      = usag.at(10).toInt();
  nv      = usag.at(11).toInt();
  nqq     = usag.at(12).toInt();
  nc      = usag.at(13).toInt();
  nad     = usag.at(14).toInt();
  //printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",na,ntmx,npolmax,nz,nto,nq,next,ncon,ntbl,ns,nv,nqq,nc,nad);
  lix++;
  QStringList rvals = resli.at(lix).split(' ',QString::SkipEmptyParts);
  r1o = rvals.at( 0).toDouble();
  r2o = rvals.at( 1).toDouble();
  r1  = rvals.at( 2).toDouble();
  r2  = rvals.at( 3).toDouble();
  r1w = rvals.at( 4).toDouble();
  r2w = rvals.at( 5).toDouble();
  gof = rvals.at( 6).toDouble();
  sig = rvals.at( 7).toDouble();
  lix++;
  QStringList wght = resli.at(lix).split(' ',QString::SkipEmptyParts);
  a = wght.at( 0).toDouble();
  b = wght.at( 1).toDouble();
  c = wght.at( 2).toDouble();
  d = wght.at( 3).toDouble();
  e = wght.at( 4).toDouble();
  f = wght.at( 5).toDouble();
  QStringList adum;
  //for (int i=0; i<nad; i++) adum.append(QString("DUM%1").arg(i));
  lix+=nad+1;

  for (int i=0; i<na; i++){
    QStringList stk= resli.at(lix).split(' ',QString::SkipEmptyParts);
    strncpy(aatm.atom,stk.at( 0).toStdString().c_str(),8);
    aatm.icor1   = stk.at( 1).toInt(); 
    aatm.icor2  = stk.at( 2).toInt();
    aatm.nax    = stk.at( 3).toInt();
    aatm.nay    = stk.at( 4).toInt();
    aatm.nay2   = stk.at( 5).toInt();
    aatm.jtf    = stk.at( 6).toInt();
    aatm.itbl   = stk.at( 7).toInt();
    aatm.isfz   = stk.at( 8).toInt();
    aatm.lmax   = stk.at( 9).toInt();
    aatm.isym   = stk.at(10).toInt();
    aatm.ichcon = stk.at(11).toInt();
    aatm.x      = stk.at(12).toDouble();
    aatm.y      = stk.at(13).toDouble();
    aatm.z      = stk.at(14).toDouble();
    aatm.amult  = stk.at(15).toDouble();
    lix++;
    stk= resli.at(lix).split(' ',QString::SkipEmptyParts);
    for (int ui=0; ui<6;ui++) aatm.u[ui]=stk.at(ui).toDouble();
    if (aatm.jtf>2){
      lix++;
      stk= resli.at(lix).split(' ',QString::SkipEmptyParts);
      for (int ui=0; ui<5;ui++) aatm.u[ui+6]=stk.at(ui).toDouble();
      lix++;
      stk= resli.at(lix).split(' ',QString::SkipEmptyParts);
      for (int ui=0; ui<5;ui++) aatm.u[ui+11]=stk.at(ui).toDouble();  
    }
    if (aatm.jtf>2){
      lix++;
      stk= resli.at(lix).split(' ',QString::SkipEmptyParts);
      for (int ui=0; ui<5;ui++) aatm.u[ui+16]=stk.at(ui).toDouble();
      lix++;
      stk= resli.at(lix).split(' ',QString::SkipEmptyParts);
      for (int ui=0; ui<5;ui++) aatm.u[ui+21]=stk.at(ui).toDouble();
      lix++;
      stk= resli.at(lix).split(' ',QString::SkipEmptyParts);
      for (int ui=0; ui<5;ui++) aatm.u[ui+26]=stk.at(ui).toDouble();  
    }
    lix++;
    stk= resli.at(lix).split(' ',QString::SkipEmptyParts);
    if (npolmax<1) for (int mi=0; mi<2; mi++) aatm.m[mi] = stk.at(mi).toDouble();
    if (npolmax==1) for (int mi=0; mi<5; mi++) aatm.m[mi] = stk.at(mi).toDouble();
    if (npolmax>1) for (int mi=0; mi<10; mi++) aatm.m[mi] = stk.at(mi).toDouble();
    if (npolmax>2){
      lix++;
      stk= resli.at(lix).split(' ',QString::SkipEmptyParts);
      if (npolmax==3) for (int mi=0; mi<7; mi++) aatm.m[mi+10] = stk.at(mi).toDouble();
      else for (int mi=0; mi<10; mi++) aatm.m[mi+10] = stk.at(mi).toDouble();  
    }
    if (npolmax==4) {
      lix++;
      stk= resli.at(lix).split(' ',QString::SkipEmptyParts);
      for (int mi=0; mi<6; mi++) aatm.m[mi+20] = stk.at(mi).toDouble();    
    }    
    lix++;
    oats.append(aatm);
  }
  for (int i=0;i<nz;i++){
    QStringList kapss = resli.at(lix).split(' ',QString::SkipEmptyParts);
    akap.ifz = kapss.at( 0).toInt();
    for (int ki=0;ki<6;ki++) akap.k[ki]= kapss.at(ki+1).toDouble();
    kaps.append(akap);
    lix++;
  }
  QStringList exts = resli.at(lix).split(' ',QString::SkipEmptyParts);
  for (int i=0; i<7; i++) extcn[i]= exts.at(i).toDouble();
  lix++;
  out=resli.at(lix).toDouble();
  lix++;
  QStringList scas = resli.at(lix).split(' ',QString::SkipEmptyParts);
  sc=(double*) malloc(sizeof(double)*nq);
  for (int i=0;i<nq;i++) sc[i]=scas.at(i).toDouble();
  QStringList allLab;
  QList<MyAtom> dummys;MyAtom du;
  extern molekul mol;

  for (int i=0; i<alle.size();i++) if (alle.at(i).an>-1) allLab.append(alle.at(i).Label);
  int atsi=allLab.size();
  for (int i=0; i<alle.size();i++) if (alle.at(i).an==-1) {
    du.Label = QString("DUM%1").arg(adum.size());
    alle[i].Label=du.Label;
    du.pos=alle.at(i).pos;
    mol.kart2frac(du.pos, du.fpos);
    dummys.append(du);
    allLab.append(du.Label);
    adum.append(du.Label);
    //printf("%s  $$\n",du.Label.toStdString().c_str());
  }

  int dumsi=adum.size();
  /*
  for (int i=0; i<na;i++) {
  //  "O(5)     C(10)     X  O(5)     H(14)    Y   R   2  1   3   4  NO      O(4)"
    //int icor1,icor2,nax,nay,nay2,jtf,itbl,isfz,lmax,isym,ichcon;

  printf("%-8s %-8s  %c  %-8s %-8s %c   %c   %d  %d %3d %3d  NO %-8s  !%-8s\n",
		  oats.at(i).atom,
		  (abs(oats.at(i).nax)<=na)?oats.at(abs(oats.at(i).nax)-1).atom:adum.at(abs(oats.at(i).nax)-na-1).toStdString().c_str(),
		  (oats.at(i).icor1==1)?'X':(oats.at(i).icor1==2)?'Y':'Z',
		  (oats.at(i).nay<=na)?oats.at(oats.at(i).nay-1).atom:adum.at(oats.at(i).nay-na-1).toStdString().c_str(),
		  (oats.at(i).nay2<=na)?oats.at(oats.at(i).nay2-1).atom:adum.at(oats.at(i).nay2-na-1).toStdString().c_str(),
		  (oats.at(i).icor2==1)?'X':(oats.at(i).icor2==2)?'Y':'Z',
                  (oats.at(i).nax<0)?'L':'R',  
		  oats.at(i).jtf,
                  oats.at(i).itbl,
		  oats.at(i).isfz,
		  oats.at(i).lmax,
		  (oats.at(i).ichcon)?oats.at(oats.at(i).ichcon-1).atom:"",
                  (alle.size()>i)?alle.at(i).Label.toStdString().c_str():"!!"
	);
  
  }
*/
 // printf("\n%d==%d %d==%d\n\n",atsi,na,dumsi,nad);
  for (int i=0; i<alle.size();i++) if (alle.at(i).an>-1) {
 //   printf("%d:\n",i);
    int j;
    for (j=0; j<oats.size(); j++) if (alle.at(i).Label==oats.at(j).atom){break;}
    if (j==oats.size()) {
      for (j=0; j<oats.size(); j++) if (oa.Label==oats.at(j).atom) {break;}
    }
    if ((j==oats.size())||(oa.Label==oats.at(j).atom)){
      if (j==oats.size()) {
      V3 fpo;
      mol.kart2frac(chm->takeAt(0).pos,fpo);
      aatm.x=fpo.x;
      aatm.y=fpo.y;
      aatm.z=fpo.z;
      aatm.amult=1.0;
      }else aatm=oats.at(j);

   //   printf("%s %s %s\n",chm->at(0).Label.toStdString().c_str(),oa.Label.toStdString().c_str(),alle.at(i).Label.toStdString().c_str());

      strncpy(aatm.atom,alle.at(i).Label.toStdString().c_str(),8);
      aatm.nax=ax1lab->currentIndex()+1;
      if (lhand->isChecked()) aatm.nax*=-1;
      aatm.nay=nats.size()+1;
      aatm.nay2=ax2lab->currentIndex()+1;
      aatm.icor1=ax1type->currentIndex()+1;
      aatm.icor2=ax2type->currentIndex()+1;
      aatm.jtf=(alle.at(i).an)?2:1;
      aatm.itbl=atypen.key(alle.at(i).an);
      for (int kk=0; kk<kaps.size();kk++) if (kaps.at(kk).ifz==aatm.itbl) {aatm.isfz=kk+1;break;}
      aatm.lmax=(alle.at(i).an)?4:1;
      if (alle.at(i).an!=oa.an){
	aatm.m[0]=monocharge.value(aatm.itbl);
	for (int mm=1; mm<26; mm++) aatm.m[mm]=0;
      
	if (aatm.jtf==2) {
	  aatm.u[0]=aatm.u[1]=aatm.u[2]=0.05;
	  for (int uu = 3; uu<31; uu++) aatm.u[uu]=0;      
	}
	else {
	  aatm.u[0]=0.05;
	  for (int uu = 1; uu<31; uu++) aatm.u[uu]=0;
	}
      }
      aatm.ichcon=aatm.isym=0;
      keyBlock2.append(keyline);
    }else{
      aatm=oats.at(j);
      keyBlock2.append(keyBlock.at(j));
    strncpy(aatm.atom,alle.at(i).Label.toStdString().c_str(),8);
    if (abs(oats.at(j).nax)<=na){
      aatm.nax=allLab.indexOf(oats.at(abs(oats.at(j).nax)-1).atom);
      if (aatm.nax==-1) aatm.nax=allLab.indexOf(chm->at(0).Label);
      aatm.nax++;
      if (oats.at(j).nax<0) aatm.nax*=-1;
    }else{
      aatm.nax=dummap[oats.at(j).nax-na+atsi-1]+1;
    }
    if (abs(oats.at(j).nay)<=na){
      aatm.nay=allLab.indexOf(oats.at(abs(oats.at(j).nay)-1).atom);
      if (aatm.nay==-1) aatm.nay=allLab.indexOf(chm->at(0).Label);
      aatm.nay++;
    }else{
      aatm.nay=dummap[oats.at(j).nay-na+atsi-1]+1;
    }
    if (abs(oats.at(j).nay2)<=na){
      aatm.nay2=allLab.indexOf(oats.at(abs(oats.at(j).nay2)-1).atom);
      if (aatm.nay2==-1) aatm.nay2=allLab.indexOf(chm->at(0).Label);
      aatm.nay2++;
    }else{
      aatm.nay2=dummap[oats.at(j).nay2-na+atsi-1]+1;
    }
    } /*
    aatm.icor1= oats.at(j).icor1;
    aatm.icor2= oats.at(j).icor2;
    aatm.jtf  = oats.at(j).jtf;
    aatm.itbl = oats.at(j).itbl;
    aatm.isfz = oats.at(j).isfz;
    aatm.lmax = oats.at(j).lmax;
    aatm.ichcon = oats.at(j).ichcon;*/
    nats.append(aatm);
  }
 // printf("tes D %d %d\n",na,atsi);
  na=nats.size();/*
  for (int i = 0; i<nats.size(); i++) {
    printf("%d %d %d %d %d %d %d %d \n",nats.at(i).nax,nats.at(i).icor1,nats.at(i).nay,nats.at(i).nay2,nats.at(i).icor2,
		    nats.at(i).jtf, nats.at(i).itbl,nats.at(i).isfz); 
  printf("%-8s %-8s  %c  %-8s %-8s %c   %c   %d  %d %3d %3d  NO %-8s\n",
		  nats.at(i).atom,
		  (abs(nats.at(i).nax)<=na)?nats.at(abs(nats.at(i).nax)-1).atom:adum.at(abs(nats.at(i).nax)-na-1).toStdString().c_str(),
		  (nats.at(i).icor1==1)?'X':(nats.at(i).icor1==2)?'Y':'Z',
		  (nats.at(i).nay<=na)?nats.at(nats.at(i).nay-1).atom:adum.at(nats.at(i).nay-na-1).toStdString().c_str(),
		  (nats.at(i).nay2<=na)?nats.at(nats.at(i).nay2-1).atom:adum.at(nats.at(i).nay2-na-1).toStdString().c_str(),
		  (nats.at(i).icor2==1)?'X':(nats.at(i).icor2==2)?'Y':'Z',
                  (nats.at(i).nax<0)?'L':'R',  
		  nats.at(i).jtf,
                  nats.at(i).itbl,
		  nats.at(i).isfz,
		  nats.at(i).lmax,
		  (nats.at(i).ichcon)?nats.at(nats.at(i).ichcon-1).atom:""
	);
  }// */

  res.open(QIODevice::WriteOnly|QIODevice::Text);
  for (int i=0; i<mli; i++) {res.write(resli.at(i).toLatin1());res.write("\n");}
  res.write(QString("USAGE %1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14\n")
		  .arg(atsi)
		  .arg(ntmx)
		  .arg(npolmax)
		  .arg(nz)
		  .arg(nto)
		  .arg(nq)
		  .arg(next)
		  .arg(ncon)
		  .arg(ntbl)
		  .arg(ns)
		  .arg(nv)
		  .arg(nqq)
		  .arg(nc)
		  .arg(dumsi)
		  .toLatin1());
  res.write(QString(" %1 %2 %3 %4 %5 %6 %7 %8\n")
		  .arg( r1o ,9,'f',6)
		  .arg( r2o ,9,'f',6)
		  .arg( r1  ,9,'f',6)
		  .arg( r2  ,9,'f',6)
		  .arg( r1w ,9,'f',6)
		  .arg( r2w ,9,'f',6)
		  .arg( gof ,9,'f',6)
		  .arg( sig ,9,'E',3).toLatin1()); 
  res.write(QString(" %1 %2 %3 %4 %5 %6\n")
		  .arg(a,9,'f',6)
		  .arg(b,9,'f',6)
		  .arg(c,9,'f',6)
		  .arg(d,9,'f',6)
		  .arg(e,9,'f',6)
		  .arg(f,9,'f',6).toLatin1());
  for (int i=0; i<dumsi; i++) 
    res.write(QString(" %1 %2 %3\n")
		    .arg(dummys.at(i).fpos.x,14,'f',8)
		    .arg(dummys.at(i).fpos.y,14,'f',8)
		    .arg(dummys.at(i).fpos.z,14,'f',8).toLatin1());
  for (int i=0; i<atsi; i++){
    res.write(QString("%1%2%3%4%5%6%7%8%9%10%11%12%13%14%15%16\n")
		    .arg(nats.at(i).atom,-8)
		    .arg(nats.at(i).icor1,3  )
		    .arg(nats.at(i).icor2,2  )
		    .arg(nats.at(i).nax  ,5  )
		    .arg(nats.at(i).nay  ,4  )
		    .arg(nats.at(i).nay2 ,4  )
		    .arg(nats.at(i).jtf  ,2  )
		    .arg(nats.at(i).itbl ,3  )
		    .arg(nats.at(i).isfz ,3  )
		    .arg(nats.at(i).lmax ,2  )
		    .arg(nats.at(i).isym ,3  )
		    .arg(nats.at(i).ichcon,4 )
		    .arg(nats.at(i).x     ,10,'f',6 )
		    .arg(nats.at(i).y     ,10,'f',6  )
		    .arg(nats.at(i).z     ,10,'f',6  )
		    .arg(nats.at(i).amult ,7,'f',4 ).toLatin1());
    res.write(QString(" %1 %2 %3 %4 %5 %6\n")
		    .arg(nats.at(i).u[0],9,'f',6)
		    .arg(nats.at(i).u[1],9,'f',6)
		    .arg(nats.at(i).u[2],9,'f',6)
		    .arg(nats.at(i).u[3],9,'f',6)
		    .arg(nats.at(i).u[4],9,'f',6)
		    .arg(nats.at(i).u[5],9,'f',6).toLatin1());
    int gg=0;
    if (nats.at(i).jtf==3) gg=10;
    if (nats.at(i).jtf==4) gg=25;
    for (int j=0; j<gg;j+=5)
    res.write(QString(" %1 %2 %3 %4 %5\n")
		    .arg(nats.at(i).u[j+6],13,'f',6)
		    .arg(nats.at(i).u[j+6],13,'f',6)
		    .arg(nats.at(i).u[j+6],13,'f',6)
		    .arg(nats.at(i).u[j+6],13,'f',6)
		    .arg(nats.at(i).u[j+6],13,'f',6).toLatin1());
    switch (npolmax) {
	    case 1://dip
		    //"  6.1918  0.0000  0.0187 -0.0723  0.0000 -0.1097  0.0000  0.0000  0.0501  0.0239"
		    //" -0.1097  0.0000  0.0000  0.0501  0.0239
		    res.write(QString(" %1 %2 %3 %4 %5\n")
				    .arg(nats.at(i).m[0],7,'f',4)
				    .arg(nats.at(i).m[1],7,'f',4)
				    .arg(nats.at(i).m[2],7,'f',4)
				    .arg(nats.at(i).m[3],7,'f',4)
				    .arg(nats.at(i).m[4],7,'f',4).toLatin1());

		    break;
	    case 2://quad
		    res.write(QString(" %1 %2 %3 %4 %5 %6 %7 %8 %9 %10\n")
				    .arg(nats.at(i).m[0],7,'f',4)
				    .arg(nats.at(i).m[1],7,'f',4)
				    .arg(nats.at(i).m[2],7,'f',4)
				    .arg(nats.at(i).m[3],7,'f',4)
				    .arg(nats.at(i).m[4],7,'f',4)
				    .arg(nats.at(i).m[5],7,'f',4)
				    .arg(nats.at(i).m[6],7,'f',4)
				    .arg(nats.at(i).m[7],7,'f',4)
				    .arg(nats.at(i).m[8],7,'f',4)
				    .arg(nats.at(i).m[9],7,'f',4)
				    .toLatin1());
		    break;
	    case 3://okt
		    res.write(QString(" %1 %2 %3 %4 %5 %6 %7 %8 %9 %10\n %11 %12 %13 %14 %15 %16 %17\n")
				    .arg(nats.at(i).m[0],7,'f',4)
				    .arg(nats.at(i).m[1],7,'f',4)
				    .arg(nats.at(i).m[2],7,'f',4)
				    .arg(nats.at(i).m[3],7,'f',4)
				    .arg(nats.at(i).m[4],7,'f',4)
				    .arg(nats.at(i).m[5],7,'f',4)
				    .arg(nats.at(i).m[6],7,'f',4)
				    .arg(nats.at(i).m[7],7,'f',4)
				    .arg(nats.at(i).m[8],7,'f',4)
				    .arg(nats.at(i).m[9],7,'f',4)
				    .arg(nats.at(i).m[10],7,'f',4)
				    .arg(nats.at(i).m[11],7,'f',4)
				    .arg(nats.at(i).m[12],7,'f',4)
				    .arg(nats.at(i).m[13],7,'f',4)
				    .arg(nats.at(i).m[14],7,'f',4)
				    .arg(nats.at(i).m[15],7,'f',4)
				    .arg(nats.at(i).m[16],7,'f',4)
				    .toLatin1());
		    break;
	    case 4://hex
		    res.write(QString(" %1 %2 %3 %4 %5 %6 %7 %8 %9 %10\n %11 %12 %13 %14 %15 %16 %17 %18 %19 %20\n %21 %22 %23 %24 %25 %26\n")
				    .arg(nats.at(i).m[0],7,'f',4)
				    .arg(nats.at(i).m[1],7,'f',4)
				    .arg(nats.at(i).m[2],7,'f',4)
				    .arg(nats.at(i).m[3],7,'f',4)
				    .arg(nats.at(i).m[4],7,'f',4)
				    .arg(nats.at(i).m[5],7,'f',4)
				    .arg(nats.at(i).m[6],7,'f',4)
				    .arg(nats.at(i).m[7],7,'f',4)
				    .arg(nats.at(i).m[8],7,'f',4)
				    .arg(nats.at(i).m[9],7,'f',4)
				    .arg(nats.at(i).m[10],7,'f',4)
				    .arg(nats.at(i).m[11],7,'f',4)
				    .arg(nats.at(i).m[12],7,'f',4)
				    .arg(nats.at(i).m[13],7,'f',4)
				    .arg(nats.at(i).m[14],7,'f',4)
				    .arg(nats.at(i).m[15],7,'f',4)
				    .arg(nats.at(i).m[16],7,'f',4)
				    .arg(nats.at(i).m[17],7,'f',4)
				    .arg(nats.at(i).m[18],7,'f',4)
				    .arg(nats.at(i).m[19],7,'f',4)
				    .arg(nats.at(i).m[20],7,'f',4)
				    .arg(nats.at(i).m[21],7,'f',4)
				    .arg(nats.at(i).m[22],7,'f',4)
				    .arg(nats.at(i).m[23],7,'f',4)
				    .arg(nats.at(i).m[24],7,'f',4)
				    .arg(nats.at(i).m[25],7,'f',4)
				    .toLatin1());
		    break;
	    default ://other + mono
		    res.write(QString(" %1 %2 %3 %4 %5\n")
				    .arg(nats.at(i).m[0],7,'f',4)
				    .arg(nats.at(i).m[1],7,'f',4)
				    .toLatin1());
    
    } 
  }



  for (int i=0;i<nz;i++){
    res.write(QString("%1 %2 %3 %4 %5 %6 %7\n")
		    .arg(kaps.at(i).ifz,3)
		    .arg(kaps.at(i).k[0],11,'f',6)
		    .arg(kaps.at(i).k[1],11,'f',6)
		    .arg(kaps.at(i).k[2],11,'f',6)
		    .arg(kaps.at(i).k[3],11,'f',6)
		    .arg(kaps.at(i).k[4],11,'f',6)
		    .arg(kaps.at(i).k[5],11,'f',6).toLatin1());
  }
  res.write(QString("%1 %2 %3 %4 %5 %6 %7\n")
		    .arg(extcn[0],10,'E',4)
		    .arg(extcn[1],10,'E',4)
		    .arg(extcn[2],10,'E',4)
		    .arg(extcn[3],10,'E',4)
		    .arg(extcn[4],10,'E',4)
		    .arg(extcn[5],10,'E',4)
		    .arg(extcn[6],10,'E',4).toLatin1());
  res.write(QString("%1\n").arg(out,10,'E',4).toLatin1());
  for (int i=0; i<nq; i++){
    res.write(QString("%1").arg(sc[i],12,'E',6).toLatin1());
    if (((i+1)%6)==0) res.write("\n");
  }
  res.write("\n");
  res.close();
  mas.open(QIODevice::WriteOnly|QIODevice::Text);
  for (int i=0; i<lsmBlock.size(); i++){
    mas.write(lsmBlock.at(i).toLatin1());
    mas.write("\n");
    if (lsmBlock.at(i).startsWith("ATOM "))break;
  }
  for (int i=0; i<nats.size(); i++){
    //%-8s %-8s  %c  %-8s %-8s %c   %c   %d  %d %3d %3d  NO %-8s\n
    mas.write(QString("%1 %2  %3  %4 %5 %6   %7   %8  %9 %10 %11  NO %12\n")
		    .arg(nats.at(i).atom,-8)
		    .arg((abs(nats.at(i).nax)<=na)?nats.at(abs(nats.at(i).nax)-1).atom:adum.at(abs(nats.at(i).nax)-na-1),-8)
		    .arg((nats.at(i).icor1==1)?'X':(nats.at(i).icor1==2)?'Y':'Z')
		    .arg((nats.at(i).nay<=na)?nats.at(nats.at(i).nay-1).atom:adum.at(nats.at(i).nay-na-1),-8)
		    .arg((nats.at(i).nay2<=na)?nats.at(nats.at(i).nay2-1).atom:adum.at(nats.at(i).nay2-na-1),-8)
		    .arg((nats.at(i).icor2==1)?'X':(nats.at(i).icor2==2)?'Y':'Z')
		    .arg((nats.at(i).nax<0)?'L':'R')
		    .arg(nats.at(i).jtf)
		    .arg(nats.at(i).itbl)
		    .arg(nats.at(i).isfz,3)
		    .arg(nats.at(i).lmax,3)
		    .arg((nats.at(i).ichcon)?nats.at(nats.at(i).ichcon-1).atom:"",-8).toLatin1());
  }
  for (int i=0; i<dumsi; i++){
    mas.write(QString("DUM%1 %2 %3 %4\n")
		    .arg(i,-6)
		    .arg(dummys.at(i).fpos.x,14,'f',8)
		    .arg(dummys.at(i).fpos.y,14,'f',8)
		    .arg(dummys.at(i).fpos.z,14,'f',8).toLatin1());
  }
  int gg=lsmBlock.indexOf(QRegExp("^END ATOM\\s*"));
//  printf("%d !!!\n",gg);
  for (int i=gg; i<lsmBlock.size(); i++){
    mas.write(lsmBlock.at(i).toLatin1());
    mas.write("\n");
    if (lsmBlock.at(i).startsWith("KEY "))break;
  }
  for (int i=0; i<keyBlock2.size();i++){
    mas.write(keyBlock2.at(i).toLatin1());
    mas.write("\n");
  
  }
  gg=lsmBlock.indexOf(QRegExp("^KAPPA\\s*.*"));
  //printf("%d !!!\n",gg);
  for (int i=gg; i<lsmBlock.size(); i++){
    mas.write(lsmBlock.at(i).toLatin1());
    mas.write("\n");
  }

  for (int i=0; i<restBlock.size();i++) {
    mas.write(restBlock.at(i).toLatin1());
    mas.write("\n");  
  }
  mas.close();
  /*
  while (!mas.atEnd()){
    QString line = QString(mas.readLine(150));
    masli.append(line);
  }    
  mas.close();
  QStringList alteDummys= masli.filter(QRegExp("^DUM\\d{1,}")); 
  for (int i=0; i<alteDummys.size();i++) {
    alteDummys[i].remove(QRegExp("\\s+.{1,}\n"));
    masli.replaceInStrings(alteDummys.at(i),QString("DUM_%1").arg(i),Qt::CaseInsensitive );
  }
  masli.replaceInStrings("DUM_","DUM",Qt::CaseInsensitive);
  int flag=0;
  int dumcnt=0;
  for (int i=0; i<id->dkilllist.size();i++){
    QString bef=id->dkilllist.at(i);
    bef.remove(0,3);
    bool drin=false;
    int num,stn;num=stn=bef.toInt();
    bef=QString("DUM%1").arg(num+1);
    for (int j=0;j<masli.size();j++) drin=(drin)?drin:masli.at(j).contains(bef,Qt::CaseInsensitive );
    
    while (drin) {
      drin=false;
      bef=QString("DUM%1").arg(++num);
      for (int j=0;j<masli.size();j++) drin=(drin)?drin:masli.at(j).contains(bef,Qt::CaseInsensitive );
      masli.replaceInStrings(bef, QString("DUM%1").arg(stn++),Qt::CaseInsensitive );
    }         
  }
  int di=0;
  int symmignore=0;
  while ((di<alle.size())&&(alle.at(di).an>-1))di++;
  for (int i=0;i<masli.size();i++) if(masli.at(i).startsWith("DUM",Qt::CaseInsensitive)) {masli.removeAt(i); i--;}
  for (int i=0;i<masli.size();i++){
    if (masli.at(i).contains("ATOM0")) flag=1;
    if (masli.at(i).contains("--U2--")) flag=2;
    if((flag==1)&&(masli.at(i).startsWith(chm->at(0).Label))) masli[i] = sLabel.text()+((masli.at(i).contains('\r'))?"\r\n":"\n");
    if((flag==2)&&(masli.at(i).startsWith(chm->at(0).Label))) masli[i] = keyline+((masli.at(i).contains('\r'))?"\r\n":"\n");
    if (masli.at(i).startsWith("END KEY"))flag=0;
    if (masli.at(i).startsWith("END ATOM")) {
      while (symmignore+di+dumcnt<alle.size()){
        if (alle.at(symmignore+di+dumcnt).Label.contains("DUM",Qt::CaseInsensitive)){
	  extern molekul mol;
         // extern double xs;
         // extern double ys;
         // extern double zs;
	  V3 frac,kart;
          kart=alle.at(symmignore+di+dumcnt).pos;
         // kart.x+=xs;
         // kart.y+=ys;
         // kart.z+=zs;
	  mol.kart2frac(kart,frac);
	  masli.insert(i+dumcnt,QString("%1 %2 %3 %4 !%5 %6 %7 %8%9")
			  .arg(QString("DUM%1").arg(dumcnt))
			  .arg(frac.x)
			  .arg(frac.y)
			  .arg(frac.z)
			  .arg(kart.x).
			  arg(kart.y)
			  .arg(kart.z)
                          .arg(alle.at(symmignore+di+dumcnt).Label)
			  .arg((masli.at(i).contains('\r'))?"\r\n":"\n"));
	  dumcnt++;
	}else symmignore++;
      }
      flag=0;
    }
  }
  mas.open(QIODevice::WriteOnly);
  mas.write(masli.join("").toLatin1(),masli.join("").length());
  mas.close();
  QFile res("xd.res");
  if (!res.exists()) res.setFileName("xd.inp");
  res.copy("xd.res_MCQsave"+ttext);
  res.close();
  res.open(QIODevice::ReadOnly);
  QStringList resli;
  while (!res.atEnd()){
    QString line = QString(res.readLine(150));
    resli.append(line);
  }    
  res.close();
  dumcnt=0;
  symmignore=0;
  int nad=resli.at(6).split(" ",QString::SkipEmptyParts).at(14).toInt();
  for (int i=0;i<nad;i++) resli.removeAt(9);
  while (symmignore+di+dumcnt<alle.size()){
    if (alle.at(symmignore+di+dumcnt).Label.contains("DUM",Qt::CaseInsensitive)){
      extern molekul mol;
//      extern double xs;
//      extern double ys;
//      extern double zs;
      V3 frac,kart;
      kart=alle.at(symmignore+di+dumcnt).pos;
//      kart.x+=xs;
//      kart.y+=ys;
//      kart.z+=zs;
      mol.kart2frac(kart,frac);
      resli.insert(9+dumcnt,QString(" %1  %2  %3 \n").arg(frac.x,8,'f').arg(frac.y,8,'f').arg(frac.z,8,'f'));
      dumcnt++;
    }else {symmignore++;}
  }
  if (id->dkilllist.size()){
    int w;
    for (int i=10; i<resli.size();i++){
      if (resli.at(i).split(" ",QString::SkipEmptyParts).at(0).contains(QRegExp("^[A-Z]+"))){
        if ((w=resli.at(i).split(" ",QString::SkipEmptyParts).at(3).toInt())>alle.size()){
	  resli[i].replace(15,3,QString("%1").arg(w-id->dkilllist.size(),3,10,QChar(' ')));
	}
        if ((w=resli.at(i).split(" ",QString::SkipEmptyParts).at(5).toInt())>alle.size())
	  resli[i].replace(23,3,QString("%1").arg(w-id->dkilllist.size(),3,10,QChar(' ')));
      }
    }
  }
  int i=0;
  while (i<resli.size()){
    if (resli.at(i).startsWith(chm->at(0).Label)) resli[i].replace(10,16,
		    QString("%1 %2 %3 %4 %5")
		    .arg(ax1type->currentIndex()+1)
		    .arg(ax2type->currentIndex()+1)
		    .arg(ax1lab->currentIndex()+1,4,10,QChar(' '))
		    .arg(atooo,3,10,QChar(' '))
		    .arg(ax2lab->currentIndex()+1,3,10,QChar(' ')));
    i++;
  }
  resli[6].replace(62,2,QString("%1").arg(dumcnt,2,10,QChar(' ')));
  res.open(QIODevice::WriteOnly);
  res.write(resli.join("").toLatin1(),resli.join("").length());
  res.close();
  if (!QFile::exists ("xd.res")) res.copy("xd.res");*/
  done(QDialog::Accepted);
}
