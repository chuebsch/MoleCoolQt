#include "invariomDlg.h"
#include <QtGui>
#include <QtOpenGL>
#include "cubeGL.h"
#include <iostream> 
#include "inames.h"

invariomDlg::invariomDlg(CEnvironment ch,Connection cll,QWidget *parent,invDlg *invD): QGLWidget(parent){
   setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer) );
   papi=invD;
   oldName="";
   ce=ch;
   V3 mitte(0,0,0);
   for (int i=0;i<ce.size();i++){
     mitte+=ce.at(i).pos;
   }
   mitte*=-1.0/ce.size();
   /*   for (int i=0;i<ce.size();i++){
	ce[i].pos+=mitte;
	}
   */
   cl=cll;
   mitsav=mitte;
}
void invariomDlg::initializeGL(){
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
}
void invariomDlg::resizeGL(int width, int height){
  glViewport(0, 0, width, height);        
  glGetIntegerv(GL_VIEWPORT, vp);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective( 29.0, (double)width/height, 5.0, 8000.0 );
}
void invariomDlg::paintGL(){
  glClearColor(0.4,0.4,0.4,1.0);
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
           double * winx, double * winy) {
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

void invariomDlg::mousePressEvent(QMouseEvent *event){
   lastPos = event->pos();
}

void invariomDlg::mouseMoveEvent(QMouseEvent *event){
   GLfloat dx = GLfloat(
      event->x() - lastPos.x()) / width();
   GLfloat dy = GLfloat(
      event->y() - lastPos.y()) / height();
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

void invariomDlg::wheelEvent(QWheelEvent *event){
  double nahda=1000.0,da=0;
  int nahdai=-1;
  for (int j=0; j<cl.size();j++){
    int a1=-1,a2=-1;
    double mx,my;
    a1=ce.indexOf(*cl[j].ato1);
    a2=ce.indexOf(*cl[j].ato2);
    if ((a1>-1)&&(a2 >-1)) {
      mx=(ce.at(a1).screenX+ce.at(a2).screenX)*.5;
      my=(ce.at(a1).screenY+ce.at(a2).screenY)*.5;


    da=(((mx-event->x())*( mx-event->x()))+ 
		    ((my-event->y())*( my-event->y())));
    nahdai=(da<nahda)?j:nahdai;
    nahda=qMin(nahda,da);
    }
  }
  int numDegrees = event->delta() / 8;
  int numSteps = numDegrees / 15;  
  if ((nahdai>-1)&&(nahdai<cl.size())) {
    GLuint index=nahdai;
    int bo=cl.at(index).order;
    if ((cl.at(index).ato1->Symbol!="H")&&(cl.at(index).ato2->Symbol!="H")){
      if ((bo>1)&&(numSteps<0)) cl[index].order=bo-1;//minusminus
      if ((bo<4)&&(numSteps>0)) cl[index].order=bo+1;//minusminus
    }
  }
  updateGL();
}

/*
void invDlg::keyPressEvent(QKeyEvent *event){
  switch(event->key()){
  case Qt::Key_Q:;
  case Qt::Key_Escape: close();break;
  }
}
*/


void invariomDlg::draw(){
  double L=20;
  V3 vec;
  double wink;
  name = inames::invName(ce[0],cl,sel);
  if (oldName!=name) emit newName();
  oldName=name;
  glPushMatrix();
  glScaled( L, L, L );
  for (int i=0; i<cl.size();i++){
    switch (cl.at(i).order){
    case 1: glColor4f(1,0.4,0.0,0.4); break;
    case 2: glColor4f(0.0,0.36,0.21,0.4); break;
    case 3: glColor4f(0.0,0.54,1.0,0.4); break;
    case 4: glColor4f(0.0,0.0,1.0,0.4); break;
    default :glColor3f(0.0,0.0,0.0);
    }
    bool trans=((!sel.contains(*cl.at(i).ato1))||(!sel.contains(*cl.at(i).ato2)));
    vec.x=(cl.at(i).ato2->pos.y-cl.at(i).ato1->pos.y);
    vec.y=(cl.at(i).ato1->pos.x-cl.at(i).ato2->pos.x);
    vec.z=0;
    vec=Normalize(vec);
    glPushMatrix();
/*    
    int a1=-1,a2=-1;
    a1=ce.indexOf(*cl[i].ato1);
    a2=ce.indexOf(*cl[i].ato2);
    if ((a1>-1)&&(a2 >-1)) {
    posTo2D(cl.at(i).ato1->pos+mitsav,model,proj,viewport, &ce[a1].screenX, &ce[a1].screenY);
    posTo2D(cl.at(i).ato2->pos+mitsav,model,proj,viewport, &ce[a2].screenX, &ce[a2].screenY);
    }*/
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
    if (trans) {glEnable(GL_BLEND);glEnable(GL_CULL_FACE);}
    gluCylinder(q, 0.05, 0.05, (float)cl.at(i).length, 8, 5); 
    if (trans) {glDisable(GL_BLEND);glDisable(GL_CULL_FACE);}
    glPopMatrix();
  }
  for (int i=0;i<ce.size();i++){
    extern molekul  mol;
    glPushMatrix () ; 
    double rad=mol.Kovalenz_Radien[ce.at(i).an]/(10*L);              
    rad+=0.1;
    
    glTranslated (mitsav.x,mitsav.y,mitsav.z);
    glTranslated(ce.at(i).pos.x, ce.at(i).pos.y, ce.at(i).pos.z) ; 
    glScaled(rad,rad,rad);
    GLUquadricObj *q = gluNewQuadric();
    gluQuadricNormals(q, GL_SMOOTH);   
    gluQuadricTexture(q,GL_TRUE);
    
    glColor4fv(mol.Acol[ce.at(i).an]); 

    bool trans=!sel.contains(ce.at(i));
    if (trans) {glEnable(GL_BLEND);glEnable(GL_CULL_FACE);}
    gluSphere(q,1.0,48,48);//Atom als Kugel zeichnen
    if (trans) {glDisable(GL_BLEND);glDisable(GL_CULL_FACE);}
    glPopMatrix();
  }
  glPushMatrix();
  glClear( GL_DEPTH_BUFFER_BIT);
  glColor3f(0.0,0.0,0.0);
  GLdouble model[16];
  GLdouble proj[16];
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev( GL_PROJECTION_MATRIX , (double*)proj );
  glGetDoublev( GL_MODELVIEW_MATRIX, (double*)model );

  QFont myFont= QFont("Arial", 24, -1, false);  
  for (int j=0;j<ce.size();j++){
    renderText( ce.at(j).pos.x+mitsav.x,ce.at(j).pos.y+mitsav.y,ce.at(j).pos.z+mitsav.z, ce.at(j).Label,myFont);
    if (!posTo2D(ce.at(j).pos+mitsav,model,proj,viewport, &ce[j].screenX, &ce[j].screenY))
    {ce[j].screenX=-200; ce[j].screenY=-200;}

  }
  glPopMatrix();
  
  glPopMatrix();
}
invDlg::invDlg(CEnvironment *ch,const Connection *cll,QStringList _base): QDialog(){
  setFixedSize(800,600);  
  base=_base;
  id= new  invariomDlg(*ch,*cll,this);
  QGridLayout *grid= new QGridLayout ;
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  legend.setText("<b>Legend: &nbsp;&nbsp;&nbsp; <font color=\"#ff6600\">single bond</font>&nbsp;&nbsp;&nbsp; <font color=\"#005b36\">meso bond</font>&nbsp;&nbsp;&nbsp; <font color=\"#008aff\">double bond</font>&nbsp;&nbsp;&nbsp; <font color=\"#0000ff\">triple bond</font></b>");
  

  grid->setRowMinimumHeight(0,500);
  grid->setRowStretch(0,500);
  grid->addWidget(id  ,0,0);
  grid->addWidget(&sLabel,2,0);
  grid->addWidget(&legend,1,0);
  grid->addWidget(&found,3,0);
  grid->addWidget(buttonBox,4,0);
  setLayout(grid);
  connect(id,SIGNAL(newName()),this,SLOT(updateLabel()));
  setWindowTitle("Invariom Modification Dialog");
}
void invDlg::accept () {
  extern Connection bondList;
  for (int i=0; i<bondList.size();i++)
    for (int j=0; j<id->cl.size();j++){
      if (((bondList.at(i).ato1->pos==id->cl.at(j).ato1->pos)&&(bondList.at(i).ato2->pos==id->cl.at(j).ato2->pos))||
	  ((bondList.at(i).ato1->pos==id->cl.at(j).ato2->pos)&&(bondList.at(i).ato2->pos==id->cl.at(j).ato1->pos))){
	bondList[i]=id->cl.at(j);
      }
    }
  done(QDialog::Accepted);
}
double b2d(int bo){
  switch (bo){
  case 1:return 1.0;
  case 2:return 1.5;
  case 3:return 2.0;
  case 4:return 3.0;
  default: return 0.0;
  }
}



V3 newAtomPos(MyAtom ato,MyAtom  neighbor, MyAtom nextNeighbor,double length,double angle, double dieder){
  V3 h1,h2,h3,p;
  double emcp,cp,sp;
  const double  g2r =180.0/M_PI;
  h2=p=(neighbor.pos-ato.pos);
  //Strecke ab->normieren mit laenge B multiplizieren um (ab X ad )-Achse und winkel A drehen dann um ab-Achse und winkel D drehen  
  p=Normalize(p);
  p*=length;
  h1=(nextNeighbor.pos-ato.pos);
  h1=Normalize(h1);

  h2=Normalize(h2);
  h3=h1%h2;
  h3*=-1.0;
  h3=Normalize(h3);
  emcp=(1-cos(angle/g2r));
  sp=sin(angle/g2r);
  cp=cos(angle/g2r);  
  h1.x=((cp+h3.x*h3.x*emcp)*        p.x+
	(-h3.z*sp+h3.x*h3.y*emcp)*  p.y+
	(h3.y*sp+h3.x*h3.z*emcp)*   p.z);
  
  h1.y=((h3.z*sp+h3.x*h3.y*emcp)*   p.x+
	(cp+h3.y*h3.y*emcp)*	    p.y+
	(-h3.x*sp+h3.y*h3.z*emcp)*  p.z);    
  
  h1.z=((-h3.y*sp+h3.x*h3.z*emcp)*   p.x+
	(h3.x*sp+h3.y*h3.z*emcp)*    p.y+
	(cp+h3.z*h3.z*emcp)*         p.z);
  
    
  emcp=(1-cos(dieder/g2r));
  sp=sin(dieder/g2r);
  cp=cos(dieder/g2r);  
  p.x=((cp+h2.x*h2.x*emcp)*        h1.x+
       (-h2.z*sp+h2.x*h2.y*emcp)*  h1.y+
       (h2.y*sp+h2.x*h2.z*emcp)*   h1.z);
  
  p.y=((h2.z*sp+h2.x*h2.y*emcp)*   h1.x+
       (cp+h2.y*h2.y*emcp)*	     h1.y+
       (-h2.x*sp+h2.y*h2.z*emcp)*  h1.z);    
  
  p.z=((-h2.y*sp+h2.x*h2.z*emcp)*  h1.x+
       (h2.x*sp+h2.y*h2.z*emcp)*   h1.y+
       (cp+h2.z*h2.z*emcp)*        h1.z);
  
  p+=ato.pos;
  return p;
}


void invariomDlg::saturate(){

  CEnvironment comList;
  Connection comBonds;
  for (int i=0; i < sel.size();i++){
    comList.append(sel[i]);
  }  
  for (int i=0;i< cl.size();i++){
    if ((comList.contains(*cl.at(i).ato1))&&(comList.contains(*cl.at(i).ato2)))
      comBonds.append(cl[i]);
  }                   
  int Wertigkeit[18]={1,0,1,2,3,4,3,2,1,0,1,2,3,4,3,2,1,0};



  QList<double> valenzen;
  for (int i=0; i<40; i++) valenzen.append((int) 0);
  for (int i=0; i< comBonds.size();i++)
    for (int j=0; j<comList.size();j++){
      valenzen[j]+=((comBonds.at(i).ato1->Label==comList.at(j).Label)||(comBonds.at(i).ato2->Label==comList.at(j).Label))?b2d(comBonds.at(i).order):0;      
    }


  double w,v;
  for (int i=0; i< comList.size();i++){
    v=valenzen.at(i);
    w=Wertigkeit[comList.at(i).an]-valenzen.at(i);    

    MyAtom nachbar,dessenNachbar,neues;
    neues.Label="H(sat)";
    if ((w==3)&&(v==1)){
      V3 p;
      
      for (int k=0; k<comBonds.size();k++){
	if (cl.at(k).ato1->Label==comList.at(i).Label) nachbar=*cl[k].ato2;
	if (cl.at(k).ato2->Label==comList.at(i).Label) nachbar=*cl[k].ato1;
      }	

      for (int k=0; k<comBonds.size();k++){
	if ((cl.at(k).ato1->Label==nachbar.Label)&&(cl.at(k).ato2->Label!=comList.at(i).Label)) dessenNachbar=*cl[k].ato2;
	if ((cl.at(k).ato2->Label==nachbar.Label)&&(cl.at(k).ato1->Label!=comList.at(i).Label)) dessenNachbar=*cl[k].ato1;
      }	
      neues.Symbol="H";
      neues.an=1;

      neues.pos=newAtomPos(comList[i],nachbar,dessenNachbar,1.05,109.4712206344906936,0);
      comList.append(neues);
      neues.pos=newAtomPos(comList[i],nachbar,dessenNachbar,1.05,109.4712206344906936,120.0);
      comList.append(neues);
      neues.pos=newAtomPos(comList[i],nachbar,dessenNachbar,1.05,109.4712206344906936,-120.0);
      comList.append(neues);
    }
    if ((w==1)&&(v==3)){
      V3 p;
      
      for (int k=0; k<comBonds.size();k++){
	if (cl.at(k).ato1->Label==comList.at(i).Label) nachbar=*cl[k].ato2;
	if (cl.at(k).ato2->Label==comList.at(i).Label) nachbar=*cl[k].ato1;
      }	

      for (int k=0; k<comBonds.size();k++){
	if ((cl.at(k).ato1->Label==nachbar.Label)&&(cl.at(k).ato2->Label!=comList.at(i).Label)) dessenNachbar=*cl[k].ato2;
	if ((cl.at(k).ato2->Label==nachbar.Label)&&(cl.at(k).ato1->Label!=comList.at(i).Label)) dessenNachbar=*cl[k].ato1;
      }	
      neues.Symbol="H";
      neues.an=1;

      neues.pos=newAtomPos(comList[i],nachbar,dessenNachbar,1.03,120.0,180.0);
      for (int o=0; o<comList.size();o++)
	if (0.5>Distance(comList.at(o).pos,neues.pos)) neues.pos=newAtomPos(comList[i],nachbar,dessenNachbar,1.03,120.0,0.0);
      comList.append(neues);

    } /*
	if ((w==1.5)&&(v==1.5)){cout<<"Ar-NH2"<<endl;
    
      V3 p;
      MyAtom nachbar,dessenNachbar,neues;
      for (int k=0; k<comBonds.size();k++){
	if (cl.at(k).ato1->Label==comList.at(i).Label) nachbar=*cl[k].ato2;
	if (cl.at(k).ato2->Label==comList.at(i).Label) nachbar=*cl[k].ato1;
      }	

      for (int k=0; k<comBonds.size();k++){
	if ((cl.at(k).ato1->Label==nachbar.Label)&&(cl.at(k).ato2->Label!=comList.at(i).Label)) dessenNachbar=*cl[k].ato2;
	if ((cl.at(k).ato2->Label==nachbar.Label)&&(cl.at(k).ato1->Label!=comList.at(i).Label)) dessenNachbar=*cl[k].ato1;
      }	
      neues.Symbol="H";
      neues.an=1;
      cout<<"$$ "<<comList.at(i).Label.toStdString()<<" ? "<<nachbar.Label.toStdString()<<"  ? "<<dessenNachbar.Label.toStdString()<<endl;
      neues.pos=newAtomPos(comList[i],nachbar,dessenNachbar,1.03,120.0,180.0);
      comList.append(neues);
      neues.pos=newAtomPos(comList[i],nachbar,dessenNachbar,1.03,120.0,0.0);
      comList.append(neues);

      }*/
    if (((w==2.5)||(w==1.5))&&(v==1.5)){
      V3 p;
      bool ar2c = false;
      
      CEnvironment dN;
      for (int k=0; k<comBonds.size();k++){
	if (cl.at(k).ato1->Label==comList.at(i).Label) nachbar=*cl[k].ato2;
	if (cl.at(k).ato2->Label==comList.at(i).Label) nachbar=*cl[k].ato1;
      }	

      for (int k=0; k<comBonds.size();k++){
	if ((cl.at(k).ato1->Label==nachbar.Label)&&(cl.at(k).ato2->Label!=comList.at(i).Label)) dN.append(*cl[k].ato2);
	if ((cl.at(k).ato2->Label==nachbar.Label)&&(cl.at(k).ato1->Label!=comList.at(i).Label)) dN.append(*cl[k].ato1);
      }	
    
      for (int m=0;m<comList.size();m++){
	double dis=sqrt(Distance(comList.at(i).pos,comList.at(m).pos));
	if ((dis<2.6)&&(dis>2.0)&&(!dN.contains(comList.at(m)))&&(comList.at(m).an>1)) {
	  ar2c=true; 

	  dessenNachbar=comList[m];valenzen[m]+=1.5;}
      }
      if (ar2c){
      neues.Symbol="C";
      neues.an=6;

      neues.pos=newAtomPos(comList[i],nachbar,dessenNachbar,1.37,121.0,0.0);
      comList.append(neues);
      neues.Symbol="H";
      neues.an=1;
      neues.pos=newAtomPos(neues,comList[i],nachbar,1.03,120.0,180.0);
      comList.append(neues);
      neues.pos=newAtomPos(comList[i],nachbar,dessenNachbar,1.03,120.0,180.0);
      comList.append(neues);
      }
      else{
	for (int k=0; k<comBonds.size();k++){
	  if ((cl.at(k).ato1->Label==nachbar.Label)&&(cl.at(k).ato2->Label!=comList.at(i).Label)) dessenNachbar=*cl[k].ato2;
	  if ((cl.at(k).ato2->Label==nachbar.Label)&&(cl.at(k).ato1->Label!=comList.at(i).Label)) dessenNachbar=*cl[k].ato1;
	}	

	neues.Symbol="H";
	neues.an=1;

	neues.pos=newAtomPos(comList[i],nachbar,dessenNachbar,1.03,120.0,180.0);
	comList.append(neues);
	neues.pos=newAtomPos(comList[i],nachbar,dessenNachbar,1.03,120.0,0.0);
	comList.append(neues);
      }
    }
  }

  for (int i=0; i<comList.size();i++)
    for (int j=i+1; j<comList.size();j++)
      if (0.5>Distance(comList.at(i).pos,comList.at(j).pos)){      

	comList.removeAt(j);
	j--;
      }
  QFile com(name+".com");
  com.open(QIODevice::WriteOnly);
  QString blabla;
  com.write("#p opt PM3MM  nosymm output=wfn FChk=all Pop=CHelp iop(6/7=3) GFinput\n\nTest please check the struture!\n\n0  1\n");
  for (int i=0; i< comList.size();i++){
    blabla+=(QString("%1 %2 %3 %4\n").arg(comList.at(i).Symbol).arg(comList.at(i).pos.x).arg(comList.at(i).pos.y).arg(comList.at(i).pos.z));
  }
  blabla+="\n";
  com.write(blabla.toLatin1());
  com.close();


}








void invariomDlg::contextMenuEvent(QContextMenuEvent *event) {
   QAction X(tr("Please click on a bond."),this);
   QAction b1(tr("Make this a single bond."),this);
   QAction b15(tr("Make this a meso bond."),this);
   QAction b2(tr("Make this a double  bond."),this);
   QAction b3(tr("Make this a triple bond."),this);
   QAction HH(tr("Bonds to hydrogen atoms are always single."),this);
   connect( &b1, SIGNAL(triggered () ),
	    this, SLOT(to1() ));
   connect( &b15, SIGNAL(triggered () ),
	    this, SLOT(to15() ));
   connect( &b2, SIGNAL(triggered () ),
	    this, SLOT(to2() ));
   connect( &b3, SIGNAL(triggered () ),
	    this, SLOT(to3() ));
   QMenu menu(this);
  double nahda=1000.0,da=0;
  int nahdai=-1;

  for (int j=0; j<cl.size();j++){
    int a1=-1,a2=-1;
    double mx,my;
    a1=ce.indexOf(*cl[j].ato1);
    a2=ce.indexOf(*cl[j].ato2);
    if ((a1>-1)&&(a2 >-1)) {
      mx=(ce.at(a1).screenX+ce.at(a2).screenX)*.5;
      my=(ce.at(a1).screenY+ce.at(a2).screenY)*.5;


    da=(((mx-event->x())*( mx-event->x()))+ 
		    ((my-event->y())*( my-event->y())));
    nahdai=(da<nahda)?j:nahdai;
    nahda=qMin(nahda,da);
    }
  }
   if (nahdai < 0) {
     menu.addAction(&X);      
   }
   else if (nahdai<cl.size()) {
     index=nahdai;

     if ((cl.at(index).ato1->Symbol!="H")&&(cl.at(index).ato2->Symbol!="H")){  
     menu.addAction(&b1);
     menu.addAction(&b15);
     menu.addAction(&b2);
     menu.addAction(&b3);
     }
     else menu.addAction(&HH);
   }
   else 
     menu.addAction(&X);
   updateGL();
   menu.exec(event->globalPos());
 }

void invariomDlg::to1() { cl[index].order=1; updateGL(); }
void invariomDlg::to15(){ cl[index].order=2; updateGL(); }
void invariomDlg::to2() { cl[index].order=3; updateGL(); }
void invariomDlg::to3() { cl[index].order=4; updateGL(); }


