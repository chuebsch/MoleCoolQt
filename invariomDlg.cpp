#include "invariomDlg.h"
#include <QtGui>
#include <QtOpenGL>
#include "cubeGL.h"
#include <iostream> 
#include "inames.h"
//const unsigned short Kovalenz_Radien[109]={37, 0, 123, 90, 80, 77, 74, 71, 72, 0, 154, 136, 118, 111, 106, 102, 99, 0, 203, 174, 144, 132, 122, 118, 117, 117, 116, 115, 117, 125, 126, 122, 120, 116, 114, 0, 218, 191, 162, 145, 134, 130, 127, 125, 125, 128, 134, 148, 144, 141, 140, 136, 133, 0, 235, 198, 169, 165, 165, 164, 164, 162, 185, 161, 159, 159, 157, 157, 156, 170, 156, 144, 134, 130, 128, 126, 127, 130, 134, 149, 148, 147, 146, 146, 145, 0, 0, 0, 188, 165, 161, 142, 130, 151, 182, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//const unsigned short ElNeg[83]={220, 550, 97, 147, 201, 250, 307, 350, 440, 480, 101, 123, 147, 174, 206, 244, 283, 320, 91, 104, 120, 132, 145, 156, 160, 164, 170, 175, 175, 166, 182, 202, 220, 248, 274, 290, 89, 99, 111, 122, 123, 130, 136, 142, 145, 130, 142, 146, 149, 172, 182, 201, 221, 240, 86, 97, 108, 108, 107, 107, 107, 107, 110, 111, 110, 110, 110, 111, 111, 106, 114, 123, 133, 140, 146, 152, 155, 142, 142, 144, 144, 155, 167 };

invariomDlg::invariomDlg(CEnvironment ch,Connection cll,QWidget *parent,invDlg *invD): QGLWidget(parent){
   setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer) );
   papi=invD;
   oldName="";
   //std::cout<<"Hallo!"<<std::endl;
   //std::cout.flush();
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
  //glEnable(GL_POLYGON_SMOOTH);   
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
  //  glEnable( GL_BLEND);
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
   //glClearDepth(1.0f);
   glEnable(GL_DEPTH_TEST );
   glDepthFunc(GL_LEQUAL);
   //   glEnable(GL_CULL_FACE);
   gluLookAt(0.0, 200, 50 ,   0.0, 0.0, 0.0,   0.0, -100, 400 );
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
   //   extern int smx,atmax;
   //if (!smx) myFont= QFont("Arial", 24, -1, false);  
   //   QGLWidget::setMouseTracking(true); 
   /*
     GLubyte *contours;
     contours=(GLubyte*)malloc(8*256);
     for (int i=0;i<256;i++){
     contours[4*i]= 0xff;
     contours[4*i+1]=0xff;
     contours[4*i+2]=0xff;
     contours[4*i+3]=((i/16)%2)?0xff:0x00;
     //fprintf(stderr,"%3x||",contours[4*i+3]);
     //    if ((i+1)%16==0)fprintf(stderr,"\n");
     }
     //fprintf(stderr,"\n");
     glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
     glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
     glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
     glTexImage2D( GL_TEXTURE_2D, 0,GL_RGBA, 16,16 , 0,		
     GL_RGBA, GL_UNSIGNED_BYTE, contours );*/
   //std::cout<<"INIT"<<std::endl;
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
  //  glTranslatef(0,0,-2.5);
   draw();
   glPopMatrix();
}

extern void __RotateCS( double c, double s, double& X, double& Y );
 extern void glTranslateL( const double dx, const double dy, const double dz );
extern void glRotateL( const double dang, const double x, const double y, const double z );

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
  int numDegrees = event->delta() / 8;
  int numSteps = numDegrees / 15;  
  GLint hits;
  glSelectBuffer(MAXSELECT, selectBuf);
  (void)glRenderMode(GL_SELECT);
  glInitNames();
  glPushName(~0);
  glPushMatrix();
  glViewport(0, 0,width(),height());        
  glGetIntegerv(GL_VIEWPORT, vp);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPickMatrix(event->x(), height()-event->y(), 8, 8, vp);
  gluPerspective( 29.0, (double)width()/height(), 50.0, 8000.0 );
  glMatrixMode(GL_MODELVIEW);
  draw();
  glPopMatrix();
  hits = glRenderMode(GL_RENDER); 
  if (hits <= 0) {      
  }else if ((int)selectBuf[(hits-1)*4+3]<cl.size()) {
    GLuint index=selectBuf[(hits-1)*4+3];
    int bo=cl.at(index).order;
    //    std::cout<<bo<<" A "<<index<<" "<<cl.size()<<std::endl;
    if ((cl.at(index).ato1->Symbol!="H")&&(cl.at(index).ato2->Symbol!="H")){
      if ((bo>1)&&(numSteps<0)) cl[index].order=bo-1;//minusminus
      if ((bo<4)&&(numSteps>0)) cl[index].order=bo+1;//minusminus
    }
    //bo=cl.at(index).order;
    //std::cout<<bo<<" O "<<index<<" "<<cl.size()<<std::endl;
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


//extern const unsigned short Kovalenz_Radien[109];

//extern const unsigned short ElNeg[83];
void invariomDlg::draw(){
  double L=20;
  V3 vec;
  //  FILE *out=fopen("unsinn.txt","a");
  double wink;
  //  inames I;
  name = inames::invName(ce[0],cl,sel);
  //  for (int i=0; i<sel.size();i++)
  //  cout<<i<<sel.at(i).Label.toStdString()<<endl; 
  //  QMessageBox::information(this,"Orientation Matrix",QString(tr("%1 ").arg(sel.size())),QMessageBox::Ok);  
  if (oldName!=name) emit newName();
  oldName=name;
  //  std::cout<<"DRAW"<<std::endl;
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
    glLoadName(i);
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
    if (trans) {glEnable(GL_BLEND);glEnable(GL_CULL_FACE);}
    gluCylinder(q, 0.05, 0.05, (float)cl.at(i).length, 8, 5); 
    if (trans) {glDisable(GL_BLEND);glDisable(GL_CULL_FACE);}
    glPopMatrix();
  }
  for (int i=0;i<ce.size();i++){
    //  printf("Atome %d, %s\n",i,ce.at(i).Label.toStdString().c_str());
    extern molekul  mol;
    glPushMatrix () ; 
    glLoadName(cl.size());
    //    std::cout<<ce.at(i).Label.toStdString ()<<" "<< ce.at(i).an<<" "<<ce.at(i).pos.x<<" "<< ce.at(i).pos.y<<" "<< ce.at(i).pos.z <<std::endl;
    double rad=mol.Kovalenz_Radien[ce.at(i).an]/(10*L);              
    rad+=0.1;
    
    glTranslated (mitsav.x,mitsav.y,mitsav.z);
    glTranslated(ce.at(i).pos.x, ce.at(i).pos.y, ce.at(i).pos.z) ; 
    glScaled(rad,rad,rad);
    GLUquadricObj *q = gluNewQuadric();
    gluQuadricNormals(q, GL_SMOOTH);   
    gluQuadricTexture(q,GL_TRUE);
    
    glColor4fv(mol.Acol[ce.at(i).an]); 
    //      for (int i=0; i<acnn;i++) if (xdinp[j].OrdZahl==(acn[i]-1)) {glColor4fv(Acol[i]); }
    //fprintf(out,"%f %f %f RAD %f %d\n",xdinp[j].kart.x,xdinp[j].kart.y,xdinp[j].kart.z,rad,mylod);

    //for (int w=0;w<sel.size();w++) fprintf(out,"%s ",sel.at(w).Label.toStdString().c_str());
    //fprintf(out,"--%s  %d\n",ce.at(i).Label.toStdString().c_str(),(sel.contains(ce.at(i)))?1:0);
    bool trans=!sel.contains(ce.at(i));
    if (trans) {glEnable(GL_BLEND);glEnable(GL_CULL_FACE);}
    gluSphere(q,1.0,48,48);//Atom als Kugel zeichnen
    if (trans) {glDisable(GL_BLEND);glDisable(GL_CULL_FACE);}
    glPopMatrix();
  }
    //glPushMatrix();
  //fprintf(out,"\n");
  //fclose(out); 
  glPushMatrix();
  glClear( GL_DEPTH_BUFFER_BIT);
  glColor3f(0.0,0.0,0.0);
  QFont myFont= QFont("Arial", 24, -1, false);  
  for (int j=0;j<ce.size();j++){
    //glLoadName(j);
    renderText( ce.at(j).pos.x+mitsav.x,ce.at(j).pos.y+mitsav.y,ce.at(j).pos.z+mitsav.z, ce.at(j).Label,myFont);
  }
  glPopMatrix();
  
  //    std::cout<<std::endl;
  glPopMatrix();
}
invDlg::invDlg(CEnvironment *ch,const Connection *cll,QStringList _base): QDialog(){
  setFixedSize(800,600);  
  base=_base;
  //std::cerr<<"Oh Oh!"<<std::endl;
  //std::cerr.flush();
  id= new  invariomDlg(*ch,*cll,this);
  QGridLayout *grid= new QGridLayout ;
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  //  QHBoxLayout qhbl;
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
  //std::cout<<"TEST"<<std::endl;
  for (int i=0; i<bondList.size();i++)
    for (int j=0; j<id->cl.size();j++){
      if (((bondList.at(i).ato1->pos==id->cl.at(j).ato1->pos)&&(bondList.at(i).ato2->pos==id->cl.at(j).ato2->pos))||
	  ((bondList.at(i).ato1->pos==id->cl.at(j).ato2->pos)&&(bondList.at(i).ato2->pos==id->cl.at(j).ato1->pos))){
	//std::cout<<"TEST"<<i<<" "<<j<<" "<<id->cl.size()<<" "<<bondList.size()<<std::endl;
	bondList[i]=id->cl.at(j);
	//std::cout<<"TESTED"<<std::endl;
      }
    }

  //  std::cout<<"TEST 99999"<<std::endl;  


  //  id->saturate();
  //qDebug()<<"about to delete id";
  //delete id;
  //qDebug()<<"id is deleted";
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
  h1=Normalize(h1);//normier(&h1.x,&h1.y,&h1.z);  
  //   //printf("h1 %f %f %f\n",h1.x,h1.y,h1.z);
  h2=Normalize(h2);//normier(&h2.x,&h2.y,&h2.z);
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
  //qDebug()<<"void saturate started!!";
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

  //for (int i=0; i< comBonds.size();i++)
  //  cout<<comBonds.at(i).ato1->Label.toStdString()<<" ---"<< comBonds.at(i).order<<"--- "<<comBonds.at(i).ato2->Label.toStdString()<<endl;
  QList<double> valenzen;
  for (int i=0; i<40; i++) valenzen.append((int) 0);
  for (int i=0; i< comBonds.size();i++)
    for (int j=0; j<comList.size();j++){
      valenzen[j]+=((comBonds.at(i).ato1->Label==comList.at(j).Label)||(comBonds.at(i).ato2->Label==comList.at(j).Label))?b2d(comBonds.at(i).order):0;      
    }
  //  for (int i=0; i< comBonds.size();i++)
  //  cout<<comBonds.at(i).ato1->Label.toStdString()<<" ---"<< comBonds.at(i).order<<"--- "<<comBonds.at(i).ato2->Label.toStdString()<<endl;
  double w,v;
  for (int i=0; i< comList.size();i++){
    v=valenzen.at(i);
    w=Wertigkeit[comList.at(i).an]-valenzen.at(i);    
    //cout<<" "<<((w>0)?"sat":"")<<endl;
    MyAtom nachbar,dessenNachbar,neues;
    neues.Label="H(sat)";
    if ((w==3)&&(v==1)){//cout<<"--CH3"<<endl;
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
      //  cout<<"$$ "<<comList.at(i).Label.toStdString()<<" ? "<<nachbar.Label.toStdString()<<"  ? "<<dessenNachbar.Label.toStdString()<<endl;
      neues.pos=newAtomPos(comList[i],nachbar,dessenNachbar,1.05,109.4712206344906936,0);
      comList.append(neues);
      neues.pos=newAtomPos(comList[i],nachbar,dessenNachbar,1.05,109.4712206344906936,120.0);
      comList.append(neues);
      neues.pos=newAtomPos(comList[i],nachbar,dessenNachbar,1.05,109.4712206344906936,-120.0);
      comList.append(neues);
    }
    if ((w==1)&&(v==3)){//cout<<"Ar-H"<<endl;
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
      //cout<<"$$ "<<comList.at(i).Label.toStdString()<<" ? "<<nachbar.Label.toStdString()<<"  ? "<<dessenNachbar.Label.toStdString()<<endl;
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
    if (((w==2.5)||(w==1.5))&&(v==1.5)){//cout<<"Ar2-CH"<<endl;
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
	  //cout<<comList.at(m).Label.toStdString()<<"---"<<comList.at(i).Label.toStdString()<<"  "<< dis<<"dist  ";
	  dessenNachbar=comList[m];valenzen[m]+=1.5;}
      }
      if (ar2c){
      neues.Symbol="C";
      neues.an=6;
      //cout<<"$$ "<<comList.at(i).Label.toStdString()<<" ? "<<nachbar.Label.toStdString()<<"  ? "<<dessenNachbar.Label.toStdString()<<endl;
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
	//cout<<"-XH2"<<endl;
	neues.Symbol="H";
	neues.an=1;
	//cout<<"$$ "<<comList.at(i).Label.toStdString()<<" ? "<<nachbar.Label.toStdString()<<"  ? "<<dessenNachbar.Label.toStdString()<<endl;
	neues.pos=newAtomPos(comList[i],nachbar,dessenNachbar,1.03,120.0,180.0);
	comList.append(neues);
	neues.pos=newAtomPos(comList[i],nachbar,dessenNachbar,1.03,120.0,0.0);
	comList.append(neues);
      }
    }
  }
  //cout<<fixed;
  for (int i=0; i<comList.size();i++)
    for (int j=i+1; j<comList.size();j++)
      if (0.5>Distance(comList.at(i).pos,comList.at(j).pos)){      
	//	cout<<" loesche "<< Distance(comList.at(i).pos,comList.at(j).pos))<< comList.at(i).Label.toStdString()<< comList.at(j).Label.toStdString() <<" " << i<<" "<<j <<endl; 
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

  //qDebug()<<"void saturate finished!!";
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
   GLint hits;
   glSelectBuffer(MAXSELECT, selectBuf);
   (void)glRenderMode(GL_SELECT);
   glInitNames();
   glPushName(~0);
   glPushMatrix();
   glViewport(0, 0,width(),height());        
   glGetIntegerv(GL_VIEWPORT, vp);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPickMatrix(event->x(), height()-event->y(), 8, 8, vp);
   gluPerspective( 29.0, (double)width()/height(), 50.0, 8000.0 );
   glMatrixMode(GL_MODELVIEW);
   draw();
   glPopMatrix();
   hits = glRenderMode(GL_RENDER); 
   if (hits <= 0) {
     menu.addAction(&X);      
   }
   else if ((int)selectBuf[(hits-1)*4+3]<cl.size()) {
     index=selectBuf[(hits-1)*4+3];
     //     int bo=cl.at(index).order;
     if ((cl.at(index).ato1->Symbol!="H")&&(cl.at(index).ato2->Symbol!="H")){  
     menu.addAction(&b1);
     menu.addAction(&b15);
     menu.addAction(&b2);
     menu.addAction(&b3);
     }
     else menu.addAction(&HH);
     //    std::cout<<bo<<" A "<<index<<" "<<cl.size()<<std::endl;
       //     if ((bo>1)&&(numSteps<0)) cl[index].order=bo-1;//minusminus
       //if ((bo<4)&&(numSteps>0)) cl[index].order=bo+1;//minusminus
     //bo=cl.at(index).order;
     //std::cout<<bo<<" O "<<index<<" "<<cl.size()<<std::endl;
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


