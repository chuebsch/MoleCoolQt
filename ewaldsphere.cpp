
/****************************************************************************
 **
 ** Copyright (C) 2011 Christian B. Huebschle & George M. Sheldrick
 ** All rights reserved.
 ** Contact: chuebsch@moliso.de
 **
 ** This file is part of the ShelXle
 **
 ** This file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file COPYING included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 **
 ****************************************************************************/
#include "ewaldsphere.h"
#if defined (Q_WS_MAC)
#include <glu.h>
#else
#include <GL/glu.h>
#endif
#ifndef PROGRAM_NAME
#define PROGRAM_NAME "MoleCoolQt"
#endif
enum Zone {all,Acentered,Bcentered,Ccentered,Fcentered,Icentered,Rcentered,H_Layer,K_Layer,L_Layer,H_Axis,K_Axis,L_Axis};
Zone zone=all;
int Layer=0;
int law=1;
EwaldSphereGLW::EwaldSphereGLW(QString fileName,molekul *externMole, QWidget *parent){
  this->setParent(parent);
  mol=externMole;
  linesize=2.0;
  pointsize=2.0;
  QString hklFileName=fileName;
  hklFileName.chop(3);
  hklFileName.append("hkl");
  qDebug()<<hklFileName;
  QFile hklFile(hklFileName);
  const double g2r=180.0/M_PI;
  reflexe.clear();
  COSAL = (mol->zelle.al==90)?0:cos(mol->zelle.al/g2r);
  COSBE = (mol->zelle.be==90)?0:cos(mol->zelle.be/g2r);
  COSGA = (mol->zelle.ga==90)?0:cos(mol->zelle.ga/g2r);

  SINAL    = sin(mol->zelle.al/g2r);
  SINBE    = sin(mol->zelle.be/g2r);
  SINGA    = sin(mol->zelle.ga/g2r);
  rc_alpha = acos((COSBE*COSGA-COSAL)/(SINBE*SINGA));
  rc_beta  = acos((COSGA*COSAL-COSBE)/(SINGA*SINAL));
  rc_gamma = acos((COSAL*COSBE-COSGA)/(SINAL*SINBE));
 
  RCOSAL = (COSBE*COSGA-COSAL)/(SINBE*SINGA);
  RCOSBE = (COSGA*COSAL-COSBE)/(SINGA*SINAL);
  RCOSGA = (COSAL*COSBE-COSGA)/(SINAL*SINBE);

  V3  hkl=V3(0,3,2),hklp;
  for (int sy=0; sy<mol->zelle.symmops.size(); sy++){
     hklp=hkl*mol->zelle.symmops.at(sy);
    // printf ("symm#%-4d %g %g %g\n",sy,hklp.x,hklp.y,hklp.z);
  }
  double phis=  sqrt(1-(RCOSAL*RCOSAL)-(RCOSBE*RCOSBE)-(RCOSGA*RCOSGA) + 2*RCOSAL*RCOSBE*RCOSGA)/sin(rc_gamma);
 // printf("cos %g %g %g sin %g %g %g\n alpha* %g beta* %g gamma* %g rcos %g %g %g\n",COSAL,COSBE,COSGA,SINAL,SINBE,SINGA,rc_alpha,rc_beta,rc_gamma,RCOSAL,RCOSBE,RCOSGA);
  xs.x =mol->zelle.as;
  xs.y =0.0;
  xs.z =0.0;
  ys.x =mol->zelle.bs*cos(rc_gamma);
  ys.y =mol->zelle.bs*sin(rc_gamma);
  ys.z =0.0;  
  zs.x =mol->zelle.cs*cos(rc_beta);
  zs.y =mol->zelle.cs*(RCOSAL-RCOSBE*RCOSGA)/sin(rc_gamma);
  zs.z =mol->zelle.cs*phis;

  if (hklFile.open(QIODevice::ReadOnly|QIODevice::Text)){
  QStringList all=QString(hklFile.readAll()).split('\n');
  Refl ref;
  smax=0.0;
  smin=99999.9;
  imax=-99999.9;
  imin=99999.9;
  
  for (int i=1; i<all.size(); i++){
    if (!all.at(i).startsWith('!')){
    sscanf( (all.at(i).toStdString().c_str()),
        "%4d%4d%4d%3d%13lf%13lf",
        &ref.h,
        &ref.k,
        &ref.l,
        &ref.sc,
        &ref.I,
        &ref.sig
        ); 
    ref.sintl=sintl(ref.h,ref.k,ref.l);
    smin=qMin(ref.sintl,smin);
    smax=qMax(ref.sintl,smax);
    imin=qMin(ref.I/ref.sig,imin);
    imax=qMax(ref.I/ref.sig,imax);
    ref.pos=V3(
        xs.x * ref.h + ys.x * ref.k + zs.x * ref.l, 
        xs.y * ref.h + ys.y * ref.k + zs.y * ref.l,  
        xs.z * ref.h + ys.z * ref.k + zs.z * ref.l);
    reflexe.append(ref);
    }
  }
  printf("%d reflections in xd.hkl sin(theta)/lambda min=%g max=%g\n",reflexe.size(),smin,smax);

  }else{printf("can not open xd.hkl!\n"); qDebug()<<fileName<<hklFileName;}
}
double EwaldSphereGLW::sintl(int h, int k, int l){
double erg=0;
  erg=h*h*mol->zelle.as*mol->zelle.as + k*k*mol->zelle.bs*mol->zelle.bs + l*l*mol->zelle.cs*mol->zelle.cs;
  if (mol->zelle.al!=90) erg+= 2  *k*l*mol->zelle.bs*mol->zelle.cs*RCOSAL;
  if (mol->zelle.be!=90) erg+= 2*h  *l*mol->zelle.as*mol->zelle.cs*RCOSBE;
  if (mol->zelle.ga!=90) erg+= 2*h*k  *mol->zelle.as*mol->zelle.bs*RCOSGA;
  /*  sqrt($lin[0]*$lin[0]*$ra*$ra +
      $lin[1]*$lin[1]*$rb*$rb +
      $lin[2]*$lin[2]*$rc*$rc +
      2*$lin[0]*$lin[2]*$ra*$rc*cos($rbet)+
      2*$lin[1]*$lin[2]*$rb*$rc*cos($ralp)+
      2*$lin[0]*$lin[1]*$ra*$rb*cos($rgam))/2.0;*/
  return sqrt(erg)/2.0;
}
bool EwaldSphereGLW::erlaubt(int h, int k, int l){
int body=h+k+l;
int A=k+l;
int B=h+l;
int C=h+k;
switch (zone){
  case all:
    return true;
  case Acentered:
    return ((A%2)==0);
  case Bcentered:
    return ((B%2)==0);
  case Ccentered:
    return ((C%2)==0);
  case Fcentered:
    return (((A%2)==0)&&((B%2)==0) &&((C%2)==0));
  case Icentered:
    return ((body%2)==0); 
  case Rcentered:
    return (((A-h)%3)==0);
  case H_Layer:
    return (h==Layer);
  case K_Layer:
    return (k==Layer);
  case L_Layer:
    return (l==Layer);
  case H_Axis:
    return (k==l)&&(k==0);
  case K_Axis:
    return (h==l)&&(l==0);
  case L_Axis:
    return (h==k)&&(k==0);

}
return false;
}
void EwaldSphereGLW::draw(){
  if (mol->LOD>3) {
    const GLfloat  OBJ_SPE[]   = { 0.8, 0.8, 0.8, 1.0 };  
    const GLfloat  OBJ_SHIN    = 127.0;               
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,             OBJ_SPE  );
    glEnable     ( GL_COLOR_MATERIAL ) ;
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;    
    glMaterialf(  GL_FRONT_AND_BACK, GL_SHININESS,           OBJ_SHIN );
  }else{
    const GLfloat  OBJ_SPE[]   = { 0.0, 0.0, 0.0, 1.0 };  
    const GLfloat  OBJ_SHIN    = 1.0;               
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,             OBJ_SPE  );
    glEnable     ( GL_COLOR_MATERIAL ) ;
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;    
    glMaterialf(  GL_FRONT_AND_BACK, GL_SHININESS,           OBJ_SHIN );
  }
/*  static double rtx=2.0;  
  glRotated(rtx,1.0,0.0,0.0);
  glRotated(rtx,0.0,1.0,0.0);
  rtx+=0.5;*/
  glPushMatrix();
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glPointSize(pointsize);
  glBegin(GL_POINTS);
  //glColor3d(0.0,0.0,0.5);
  V3 p=V3(0,0,0),hkl=V3(0,0,0),hklp=V3(0,0,0);
  for (int i=0; i<reflexe.size(); i++){
    // void Farbverlauf (GLfloat wrt,GLfloat min,GLfloat max,GLfloat alpha=1.0f);
    mol->Farbverlauf(reflexe.at(i).I/reflexe.at(i).sig,imin,imax);//,1.0-(reflexe.at(i).I/reflexe.at(i).sig/(imax-imin)));
//    mol->Farbverlauf(reflexe.at(i).sintl,smin,smax);//,1.0-(reflexe.at(i).I/reflexe.at(i).sig/(imax-imin)));
    hkl=V3(reflexe.at(i).h, reflexe.at(i).k, reflexe.at(i).l);
    for (int sy=0; sy<mol->zelle.symmops.size(); sy++){
      hklp=hkl*mol->zelle.symmops.at(sy);
      p=V3(
        xs.x * hklp.x + ys.x * hklp.y + zs.x * hklp.z, 
        xs.y * hklp.x + ys.y * hklp.y + zs.y * hklp.z,  
        xs.z * hklp.x + ys.z * hklp.y + zs.z * hklp.z);
        //reflexe.at(i).pos*mol->zelle.symmops.at(sy);
      if (erlaubt((int)hklp.x ,(int)hklp.y, (int)hklp.z)) 
         glVertex3d(p.x,p.y,p.z);
    }
  }
  glEnd();
  glEnable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

  glColor4d(0.70,0.65,0.5,0.25);
  GLUquadricObj *q = gluNewQuadric();
  gluQuadricNormals(q, GL_SMOOTH);   
  gluQuadricTexture(q,GL_TRUE);    
  gluSphere(q,2*smax,48,48);//Atom als Kugel zeichnen
  glEnable(GL_DEPTH_TEST);
  glLineWidth(linesize);
  glDisable(GL_LIGHTING);
  glBegin(GL_LINES);
  int st[12]={0,0,0,1,1,2,2,3,4,4,5,6};
  int en[12]={1,2,4,3,5,3,6,7,5,6,7,7};
  for (int i=0,ih=0,ik=0,il=0; i<12; i++){
    ih=(en[i]  )%2;
    ik=(en[i]/2)%2; 
    il=(en[i]/4)%2;

    glColor3d(ih*0.95+0.05,ik*0.95+0.05,il*0.95+0.05);
    glVertex3d( 
        xs.x * ih + ys.x * ik + zs.x * il, 
        xs.y * ih + ys.y * ik + zs.y * il,  
        xs.z * ih + ys.z * ik + zs.z * il);
    ih=(st[i]  )%2;
    ik=(st[i]/2)%2; 
    il=(st[i]/4)%2;
    glColor3d(ih*0.95+0.05,ik*0.95+0.05,il*0.95+0.05);
    glVertex3d( 
        xs.x * ih + ys.x * ik + zs.x * il, 
        xs.y * ih + ys.y * ik + zs.y * il,  
        xs.z * ih + ys.z * ik + zs.z * il);
  }
  glEnd();

  glEnable(GL_LIGHTING);

  glPopMatrix();
}

void EwaldSphereGLW::initializeGL(){
  glEnable(GL_LINE_SMOOTH);  
  glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

  timer = new QTimer(this);
  connect( timer, SIGNAL(timeout()),
		  this, SLOT(updateGL()) );
  // Jede Sekunde updateTime() aufrufen
//  timer->start(20);

  //glEnable(GL_POLYGON_SMOOTH);   
  myFont= QFont("Arial", 18, -1, false);
  const GLfloat  position[] = {100.0f, 100.0f,100.0f,0.0f};
  const GLfloat  diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
  const GLfloat  specular[] = { 1.0, 0.9, 0.9, 1.0 };
  const GLfloat  ambient[]  = { 0.4, 0.4, 0.4, 1.0 };

  glLightModeli(  GL_LIGHT_MODEL_LOCAL_VIEWER, 1 );

  glLightfv( GL_LIGHT0, GL_POSITION, position );
  glLightfv( GL_LIGHT0, GL_AMBIENT,  ambient );
  glLightfv( GL_LIGHT0, GL_DIFFUSE,  diffuse );
  glLightfv( GL_LIGHT0, GL_SPECULAR, specular );

  glLightfv( GL_LIGHT1, GL_POSITION, position );
  glLightfv( GL_LIGHT1, GL_DIFFUSE,  diffuse );  
  glLightfv( GL_LIGHT1, GL_AMBIENT,  ambient );

  glLightfv( GL_LIGHT2, GL_DIFFUSE,  diffuse );  
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
/*
  mol->adpwall=bindTexture(QImage(QString(":/icons/adpwall3.png")),GL_TEXTURE_2D);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
*/
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );
  //  glEnable( GL_BLEND);
  glDisable(GL_BLEND);
  glAlphaFunc ( GL_GREATER, 0.01f ) ;
  //glEnable(GL_ALPHA_TEST);
  glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) ;
  const GLfloat  OBJ_SPE[]   = { 1.0, 1.0, 1.0, 1.0 };  
  const GLfloat  OBJ_SHIN    = 127.0;                   
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,             OBJ_SPE  );
  glEnable     ( GL_COLOR_MATERIAL ) ;
  glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;    
  glMaterialf(  GL_FRONT_AND_BACK, GL_SHININESS,           OBJ_SHIN );
  glShadeModel( GL_SMOOTH );
  glEnable(GL_NORMALIZE);
  glClearColor(0.0,0.0,0.0,1.0);
  glEnable(GL_DEPTH_TEST );
  glDepthFunc(GL_LEQUAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}
void EwaldSphereGLW::resizeGL(int width, int height){
  glViewport(0, 0, width, height);        
  glGetIntegerv(GL_VIEWPORT, vp);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0*(double)width/height,1.0*(double)width/height,-1.0,1.0,-1.0,1.0);
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

void EwaldSphereGLW::mousePressEvent(QMouseEvent *event){
   lastPos = event->pos();
   /*
  double nahda=200.0,da=0;
  int nahdai=-1;
  for (int j=0; j<ce.size();j++){
    da=(((ce.at(j).screenX-event->x())*( ce.at(j).screenX-event->x()))+ 
		    ((ce.at(j).screenY-event->y())*( ce.at(j).screenY-event->y())));
    nahdai=(da<nahda)?j:nahdai;
    nahda=qMin(nahda,da);
  }
     if (nahdai <= 0) {
     }else if (nahdai<ce.size()) {
       GLuint index=nahdai;
       if(dummyMode){
	 if (!durchlauf) emit message(QString("You clicked on %1. Click on next atom now!").arg(ce.at(index).Label));
	 if (!durchlauf) dpos=V3(0,0,0);
	 dpos+=Normalize(ce.at(index).pos-ce.at(0).pos);
	 durchlauf++;
	 if ((durchlauf==2)){
	   emit message(QString("You clicked on %1. and %2").arg(ce.at(oldindex).Label).arg(ce.at(index).Label));
	   MyAtom newDummy;
	   newDummy.sg=0;
           QString prev="DUM-1";
           for (int i=0; i<alle.size();i++) if (alle.at(i).an==-1) prev=alle.at(i).Label;
           prev.remove(0,3);
           int num = prev.toInt()+1;

           newDummy.Label="DUM"+QString::number(num);
	   if (dummyMode==1)newDummy.pos=Normalize(dpos)+ce.at(0).pos;
	   if (dummyMode==2)newDummy.pos=(Normalize(ce.at(index).pos-ce.at(0).pos)%dpos)+ce.at(0).pos;
	   newDummy.an=-1;
           duminuse.append(newDummy.Label);
	   int addr=0;
	   for (int k=0;k<ce.size();k++) if ((ce.at(k).an>-2)&&(!ce.at(k).sg)) addr=k+1;
	   ce.insert(addr,newDummy);
	   addr=0;
	   for (int k=0;k<alle.size();k++) if ((alle.at(k).an>-2)&&(!alle.at(k).sg)) addr=k+1;
	   alle.insert(addr,newDummy);
           allLab.insert(addr,newDummy.Label);
	   emit addatom(newDummy);
	   this->setCursor(Qt::ArrowCursor);
	   dummyMode=0;
	 }
	 oldindex=index;       
       }
     }*/
       updateGL();
}
void EwaldSphereGLW::mouseMoveEvent(QMouseEvent *event){
   GLfloat dx = GLfloat(event->x() - lastPos.x()) / width();
   GLfloat dy = GLfloat(event->y() - lastPos.y()) / height();
   
//  double nahda=200.0,da=0;
//  int nahdai=-1;
/*  for (int j=0; j<ce.size();j++){
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
   }*/
   if ((event->buttons() & Qt::LeftButton)) {
     glRotateL(dy*90.0,1.0f,0.0f,0.0f);
     glRotateL(dx*90.0,0.0f,1.0f,0.0f);
     updateGL();
   }else if((event->buttons() & Qt::RightButton)){
     glScaled(1.0-dy,1.0-dy,1.0-dy);
     updateGL();
   }
   lastPos = event->pos();
}
void EwaldSphereGLW::wheelEvent(QWheelEvent *event){   
  int numDegrees = event->delta() / 8;
  int numSteps = numDegrees / 15;
if (event->modifiers()==Qt::NoModifier){
  {
    int d = (int)(10.0*pointsize);
    d=(d+numSteps>4)?d+numSteps:d;
    pointsize=d/10.0;
   // printf("point size %g\n",pointsize);
    updateGL();
  }
}else if (event->modifiers()==Qt::ControlModifier){
    int d = (int)(10.0*linesize);
    d=(d+numSteps>4)?d+numSteps:d;
    linesize=d/10.0;
   // printf("line width %g\n",linesize);
    updateGL();
    

}  event->accept();
}

void EwaldSphereGLW::paintGL(){
  int width=QGLWidget::width(),height=QGLWidget::height();
  glClearColor(0.0,0.0,0.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, width, height);        
  glGetIntegerv(GL_VIEWPORT, vp);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0*(double)width/height,1.0*(double)width/height,-1.0,1.0,-1.0,1.0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  draw();
  glPopMatrix();
}
EwaldSphereDlg::EwaldSphereDlg(QString fileName,molekul *externMole){
  //otm=NULL;
  setMinimumSize(768,768);
  mol=externMole;
  mol->loadSettings();
  QString gr=mol->einstellung->group();
  if (gr!="Version 0.1")mol->einstellung->beginGroup("Version 0.1");
  setWindowTitle("Ewald-Shpere");  
  //mol->loadSettings();
  eacGLW = new EwaldSphereGLW(fileName,mol,this);
  eacGLW->mol->loadSettings();
  QComboBox *zonebox=new QComboBox(this);
  QStringList zl;
  zl<<"all"<<"A-centered"<<"B-centered"<<"C-centered"<<"F-centered"<<"I-centered"<<"R-centered"<<"KL-Layer"<<"HL-Layer"<<"HK-Layer"<<"H-Axis"<<"K-Axis"<<"L-Axis";
  zonebox->addItems(zl);
  zonebox->setCurrentIndex(0);
  QSpinBox *laybox=new QSpinBox(this);
  laybox->setPrefix("Layer #");
  laybox->setMinimum(0);
  laybox->setMaximum(1000);
  laybox->setValue(0);
  connect(laybox,SIGNAL(valueChanged(int)),this,SLOT(layerChanged(int)));
  connect(zonebox,SIGNAL(currentIndexChanged(int)),this,SLOT(zoneChanged(int)));
  eacGLO = new QGridLayout; 
  eacGLO->addWidget(eacGLW,1,1,10,10);
  eacGLO->addWidget(zonebox,11,1);
  eacGLO->addWidget(laybox,11,2);
  setLayout(eacGLO);
}

void EwaldSphereDlg::zoneChanged(int z){
  zone=(Zone) z;
  eacGLW->updateGL();
};

void EwaldSphereDlg::layerChanged(int l){
  Layer=l;
  eacGLW->updateGL();
};

void EwaldSphereDlg::reject(){
  eacGLW->timer->stop();
  eacGLW->disconnect();
  this->disconnect();
  delete eacGLW;
  done(QDialog::Rejected);
}

void EwaldSphereDlg::accept(){
  eacGLW->disconnect();
  this->disconnect();
  done(QDialog::Accepted);
}
