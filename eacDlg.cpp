
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
#include "eacDlg.h"
#if defined (Q_WS_MAC)
#include <glu.h>
#else
#include <GL/glu.h>
#endif
#ifndef PROGRAM_NAME
#define PROGRAM_NAME "MoleCoolQt"
#endif
Connection EacGLW::connecting(const CEnvironment &atom){
  /*! Creates bonds and lbonds depending on covalent radii.
   * If edit atom style flag (eac) is not set FREE and BIND instructions are considered.
   * Feeds also Molecule.knoepfe list.
   * \returns the Connection bond list.
   * */
  Connection bonds;
  bonds.clear();
  //lbonds.clear();
  MyBond bond;
  double soll=0;
  for (int i=0; i<atom.size(); i++){
    for (int j=0; j<=i; j++ ){
      double d = sqrt(Distance(atom.at(i).pos,atom.at(j).pos));
      if ((atom.at(i).part<0)&&(atom.at(j).part!=0)&&(atom.at(i).sg!=atom.at(j).sg)) continue;
      if ((atom.at(j).part<0)&&(atom.at(i).part!=0)&&(atom.at(i).sg!=atom.at(j).sg)) continue;
      if ((atom.at(i).an==0)&&(atom.at(j).an==0)) continue;
      if ((atom.at(i).an>-1)&&(atom.at(j).an>-1)&&
          ((!atom.at(i).part)||(!atom.at(j).part)||(atom.at(i).part==atom.at(j).part)))
        soll=(mol->Kovalenz_Radien[atom.at(i).an]+ mol->Kovalenz_Radien[atom.at(j).an])*0.012;
      else
        soll=-1;
      if ((d>0.1)&&(d<soll)) {
        bond.ato1=&atom[i];
        bond.ato2=&atom[j];
        bond.a1=i;
        bond.a2=j;
        bond.length=d;
        bonds.append(bond);
      }
      /* else if ((d<qboMax)&&(d>qboMin)){
      if ((atom.at(i).an==0)||(atom.at(j).an==0)) continue;
        if  ((!atom.at(i).part)||(!atom.at(j).part)||(atom.at(i).part==atom.at(j).part)){
        bond.ato1=&atom[i];
        bond.ato2=&atom[j];
        bond.a1=i;
        bond.a2=j;
        bond.length=d;
        lbonds.append(bond);
    }
      }// */
    }
  }
  //qDebug()<<"Bondsize "<<bonds.size();
  /*
  knoepfe.clear();
  Knopf kn;
  kn.neighbors.clear();
  for (int i=0; i<asymm.size(); i++){
    kn.neighbors.clear();
    for (int j=0; j<bonds.size(); j++){
      if (bonds.at(j).a1==i){
      kn.neighbors.append(bonds.at(j).a2);
      }
      if (bonds.at(j).a2==i){
      kn.neighbors.append(bonds.at(j).a1);
      }
    }
    knoepfe.append(kn);
    */
  return bonds;
  }


EacGLW::EacGLW(molekul *externMole, QWidget *parent){
  this->setParent(parent);
  mol=externMole;
  exmin=mol->pmin;
  exmax=mol->pmax;
  mol->pmax=0.99;
  mol->pmin=0.39;
  mol->copyAcol(mol->Acol,mol->arad,mol->aStyle);
  mol->loadSettings();
  proba=50;
  MyAtom xda;
  //xda.symmGroup=0;
  //xda.hidden=0;
  xda.part=0;
  //xda.ownStyle=false;
  xda.Label="H";
  xda.pos.x=0;
  xda.pos.y=0;
  xda.pos.z=0;
  xda.uc.m11=0.1;
  xda.uc.m22=0.07;
  xda.uc.m33=0.025;
  xda.uc.m13=xda.uc.m31=0.01;
  xda.uc.m12=xda.uc.m21=-0.02;
  xda.uc.m23=xda.uc.m32=0.01;
  xda.an=6;
  xd.append(xda);

  xda.Label="H1(noADP)";
  xda.pos.x=0.00001;
  xda.pos.y=0.00001;
  xda.pos.z=1.0;
  // xda.ownStyle=true;
  //xda.style=ATOM_STYLE_NOADP;
  //xda.rad=mol->arad[0];
  //xda.color=mol->AtomColor[0];
  xda.uc.m11=0.05;
  xda.uc.m22=0.05;
  xda.uc.m33=0.05;
  xda.uc.m13=0.0;
  xda.uc.m12=0.0;
  xda.uc.m23=0.0;
  xda.an=0;
  xd.append(xda);

  xda.Label="H2";
  //xda.ownStyle=false;
  xda.pos.x=0.81649658102978810529098386315336;
  xda.pos.y=-0.4714045208499572479996295714055;
  xda.pos.z=-0.333333;
  xda.uc.m11=0.05;
  xda.uc.m22=0.05;
  xda.uc.m33=0.05;
  xda.uc.m13=0.0;
  xda.uc.m12=0.0;
  xda.uc.m23=0.0;
  xda.an=0;
  xd.append(xda);

  xda.Label="H3";
  xda.pos.x=0;
  xda.pos.y=0.94280904169991449599925914281199;
  xda.pos.z=-0.333333;
  xda.uc.m11=0.05;
  xda.uc.m22=0.05;
  xda.uc.m33=0.05;
  xda.uc.m13=0.0;
  xda.uc.m12=0.0;
  xda.uc.m23=0.0;
  xda.an=0;
  xd.append(xda);

  xda.Label="H4";
  xda.pos.x=-0.81649658102978810529098386315336;
  xda.pos.y=-0.4714045208499572479996295714055;
  xda.pos.z=-0.333333;
  xda.uc.m11=0.05;
  xda.uc.m22=0.05;
  xda.uc.m33=0.05;
  xda.uc.m13=0.0;
  xda.uc.m12=0.0;
  xda.uc.m23=0.0;
  xda.an=0;
  xd.append(xda);

  xda.Label="Q1";
  xda.pos.x=-0.81649658102978810529098386315336;
  xda.pos.y=-0.4714045208499572479996295714055;
  xda.pos.z=0.333333;
  xda.peakHeight=0.91;
  xda.uc.m11=0.05;
  xda.uc.m22=0.05;
  xda.uc.m33=0.05;
  xda.uc.m13=0.0;
  xda.uc.m12=0.0;
  xda.uc.m23=0.0;
  xda.an=-1;
  xd.append(xda);
  xda.Label="Q2";
  xda.pos.x=0.81649658102978810529098386315336;
  xda.pos.y=0.4714045208499572479996295714055;
  xda.pos.z=0.333333;
  xda.peakHeight=0.41;
  xda.uc.m11=0.05;
  xda.uc.m22=0.05;
  xda.uc.m33=0.05;
  xda.uc.m13=0.0;
  xda.uc.m12=0.0;
  xda.uc.m23=0.0;
  xda.an=-1;
  xd.append(xda);
  xda.Label="Q3";
  xda.pos.x=0;
  xda.pos.y=-0.94280904169991449599925914281199;
  xda.pos.z=0.333333;
  xda.peakHeight=0.71;
  xda.uc.m11=0.05;
  xda.uc.m22=0.05;
  xda.uc.m33=0.05;
  xda.uc.m13=0.0;
  xda.uc.m12=0.0;
  xda.uc.m23=0.0;
  xda.an=-1;
  xd.append(xda);// */
  bonds=connecting(xd);
  xd[0].an=0;//das ist ein ... trick um bindungen vom zentralatom zu den liganden zu erzeugen.
  /*

     MyBond b;
     b.ato1=&xd[0];
     b.ato2=&xd[1];
     b.a1=0;
     b.a2=1;
     b.length=1;
     bonds.append(b);
     b.ato1=&xd[0];
     b.ato2=&xd[2];
     b.a1=0;
     b.a2=2;
     b.length=1;
     bonds.append(b);
     b.ato1=&xd[0];
     b.ato2=&xd[3];
     b.a1=0;
     b.a2=3;
     b.length=1;
     bonds.append(b);
     b.ato1=&xd[0];
     b.ato2=&xd[4];
     b.a1=0;
     b.a2=4;
     b.length=1;
     bonds.append(b);
     */
}

void rhombo(double R){
  GLfloat p=0.5f*(1+sqrt(5.0f));
  GLfloat p2=p*p;
  GLfloat p3=p2*p;
  p*=0.2*R;
  p2*=0.2*R;
  p3*=0.2*R;
  GLfloat vert[96] = {p2,0,p3,0,p,p3,-p2,0,p3,0,-p,p3,p2,p2,p2,0,p3,p2,-p2,p2,p2,-p2,-p2,p2,0,-p3,p2,p2,-p2,p2,p3,0,p,-p3,0,p,p3,p2,0,p,p3,0,-p,p3,0,-p3,p2,0,-p3,-p2,0,
-p,-p3,0,p,-p3,0,p3,-p2,0,p3,0,-p,-p3,0,-p,p2,p2,-p2,0,p3,-p2,-p2,p2,-p2,-p2,-p2,-p2,0,-p3,-p2,p2,-p2,-p2,p2,0,-p3,0,p,-p3,-p2,0,-p3,0,-p,-p3};
  GLfloat normals[96]={0.5257311,   0.0000000,   0.8506508, 0.0000000,   0.3568221,   0.9341723, -0.5257311,   0.0000000,   0.8506508, 0.0000000,  -0.3568221,   0.9341723,
    0.5773503,   0.5773503,   0.5773503, 0.0000000,   0.8506508,   0.5257311, -0.5773503,   0.5773503,   0.5773503, -0.5773503,  -0.5773503,   0.5773503,
    0.0000000,  -0.8506508,   0.5257311, 0.5773503,  -0.5773503,   0.5773503, 0.9341723,   0.0000000,   0.3568221, -0.9341723,   0.0000000,   0.3568221,
    0.8506508,   0.5257311,   0.0000000, 0.3568221,   0.9341723,   0.0000000, -0.3568221,   0.9341723,   0.0000000, -0.8506508,   0.5257311,   0.0000000,
    -0.8506508,  -0.5257311,   0.0000000, -0.3568221,  -0.9341723,   0.0000000, 0.3568221,  -0.9341723,   0.0000000, 0.8506508,  -0.5257311,   0.0000000,
    0.9341723,   0.0000000,  -0.3568221, -0.9341723,   0.0000000,  -0.3568221, 0.5773503,   0.5773503,  -0.5773503, 0.0000000,   0.8506508,  -0.5257311,
    -0.5773503,   0.5773503,  -0.5773503, -0.5773503,  -0.5773503,  -0.5773503, 0.0000000,  -0.8506508,  -0.5257311, 0.5773503,  -0.5773503,  -0.5773503,
    0.5257311,   0.0000000,  -0.8506508, 0.0000000,   0.3568221,  -0.9341723, -0.5257311,   0.0000000,  -0.8506508, 0.0000000,  -0.3568221,  -0.9341723};

  GLubyte indices[180] = 
  {  0, 4,10,10, 4,12,
    0, 9, 3, 3, 9, 8,
    0,10, 9, 9,10,19,
    0, 1, 4, 4, 1, 5,
    0, 1, 3, 3, 1, 2,
    2, 6, 1, 1, 6, 5,
    2, 7,11,11, 7,16,
    2,11, 6, 6,11,15,
    2, 3, 7, 7, 3, 8,
    5, 6,14,14, 6,15,
    5,13, 4, 4,13,12,
    5,14,13,13,14,23,
    8, 9,18,18, 9,19,
    8,17, 7, 7,17,16,
    8,18,17,17,18,26,
    12,13,22,22,13,23,
    12,20,10,10,20,19,
    12,22,20,20,22,28,
    15,11,21,21,11,16,
    15,21,24,24,21,30,
    15,24,14,14,24,23,
    16,17,25,25,17,26,
    16,25,21,21,25,30,
    19,20,27,27,20,28,
    19,27,18,18,27,26,
    23,24,29,29,24,30,
    23,29,22,22,29,28,
    26,27,31,31,27,28,
    26,31,25,25,31,30,
    28,29,31,31,29,30};
  glNormalPointer(GL_FLOAT,0,normals);
  glVertexPointer(3,GL_FLOAT,0,vert);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
//  glShadeModel(GL_FLAT);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glDrawElements(GL_TRIANGLES,180,GL_UNSIGNED_BYTE,indices);
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glDrawElements(GL_TRIANGLES,180,GL_UNSIGNED_BYTE,indices);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glShadeModel(GL_SMOOTH);
}
void EacGLW::draw(){
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
  static double rtx=2.0;  
  glRotated(rtx,1.0,0.0,0.0);
  glRotated(rtx,0.0,1.0,0.0);
  rtx+=0.5;
  mol->dratom=0;
  //mol->nonPositiveDefinite=0;
//  mol->parthigh=false;
  glPushMatrix();
  glScaled( 50, 50, 50 );
  mol->adp=1;
  mol->intern=1;
  mol->tubifiedAtoms=0;
  mol->atoms(xd,proba);
  if (mol->bondColorStyle){
    qglColor(mol->bondColor);
    glDisable(GL_COLOR_MATERIAL);
  }
  mol->bonds(bonds);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_BLEND);
  mol->intern=0;
  mol->dratom=0;
  mol->atoms(xd,proba);
  glColor3f(0.4,0.4,0.4);
//  rhombo(0.25);
//  mol->lbond();
  glDisable(GL_BLEND);
  glColor3f(0,0,0);
  glClear( GL_DEPTH_BUFFER_BIT);
  for (int i=0; i<xd.size();i++){
    if ((i==1)||(!(mol->aStyle[xd[i].an]&ATOM_STYLE_NOLABEL)))
      renderText( xd.at(i).pos.x,xd.at(i).pos.y,xd.at(i).pos.z, xd.at(i).Label,myFont);
  }
  glPopMatrix();
}
void EacGLW::initializeGL(){
  glEnable(GL_LINE_SMOOTH);  
  glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

  timer = new QTimer(this);
  connect( timer, SIGNAL(timeout()),
		  this, SLOT(updateGL()) );
  // Jede Sekunde updateTime() aufrufen
  timer->start(20);

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
  mol->adpwall=bindTexture(QImage(QString(":/images/adpwall.png")),GL_TEXTURE_2D);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
/*
  mol->adpwall=bindTexture(QImage(QString(":/icons/adpwall3.png")),GL_TEXTURE_2D);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
*/
  mol->hbtex=bindTexture(QImage(QString(":/images/hbb.png")),GL_TEXTURE_2D);
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
  glClearColor(0.4,0.4,0.4,1.0);
  glEnable(GL_DEPTH_TEST );
  glDepthFunc(GL_LEQUAL);
  gluLookAt(0.0, 200, 50 ,   0.0, 0.0, 0.0,   0.0, -100, 400 );
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}
void EacGLW::resizeGL(int width, int height){
  glViewport(0, 0, width, height);        
  glGetIntegerv(GL_VIEWPORT, vp);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective( 29.0, (double)width/height, 5.0, 8000.0 );
}

void EacGLW::paintGL(){
  glClearColor(0.8,0.8,0.8,1.0);
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
void EacGLW::changeAtom(int OZ){
  xd[0].Label=mol->pse(OZ);
  xd[0].an=OZ;
  xd[1].Label=QString("%1(noADP)").arg(mol->pse(OZ));
  xd[1].an=OZ;
  //xd[1].ownStyle=true;
  //xd[1].style=ATOM_STYLE_NOADP;
  //xd[1].rad=mol->arad[OZ];
  //xd[1].color=mol->AtomColor[OZ];
  updateGL();
}
void EacGLW::colorDLG(){
  QColor bgcolor;    
  bgcolor=QColorDialog::getColor(QColor((int)(mol->Acol[xd.at(0).an][0]*255),
                                        (int)(mol->Acol[xd.at(0).an][1]*255),
                                        (int)(mol->Acol[xd.at(0).an][2]*255),
                                        (int)(mol->Acol[xd.at(0).an][3]*255)), this);
  if (bgcolor.isValid()) {
    mol->Acol[xd.at(0).an][0]=bgcolor.redF();
    mol->Acol[xd.at(0).an][1]=bgcolor.greenF();
    mol->Acol[xd.at(0).an][2]=bgcolor.blueF();
    mol->Acol[xd.at(0).an][3]=0.5;
    xd[1].an=xd.at(0).an;
    mol->einstellung->beginWriteArray("AtomColors",107);
    for (int i = 0; i < 107; ++i) {
      mol->einstellung->setArrayIndex(i);
      mol->einstellung->setValue("red",   mol->Acol[i-1][0]);
      mol->einstellung->setValue("green", mol->Acol[i-1][1]);
      mol->einstellung->setValue("blue",  mol->Acol[i-1][2]);
      mol->einstellung->setValue("alpha", mol->Acol[i-1][3]);
    }
    mol->einstellung->endArray();
    mol->einstellung->sync();
    emit colorchanged(bgcolor);

  }
}

void EacDlg::colorchange(QColor bgcolor){
  colorbut->setStyleSheet(QString(
			  "QPushButton {"
			  "border: 1px solid #000000;"
			  "border-radius: 9px;"
			  "color: %4;"
			  "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 0.5 %3, stop: 1 %2);"
			  "}"
			  "QPushButton:hover {"
			  "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %2, stop: 0.5 %1, stop: 1 %2);"
			  "}"
			  "QPushButton:flat {"
			  "    border: none; /* no border for a flat push button */"
			  "}"
			  )
		  .arg(bgcolor.name())
		  .arg(bgcolor.darker(200).name())
		  .arg(bgcolor.lighter(200).name())
		  .arg((bgcolor.value()<90)?"#ffffff":"#000000"));

}

void EacDlg::colorDLG2(){
  QColor bgcolor;
  bgcolor=QColorDialog::getColor(mol->bondColor, this);
  if (bgcolor.isValid()) {
    mol->bondColor=eacGLW->mol->bondColor=bgcolor;

    colorbut2->setStyleSheet(QString(
			    "QPushButton {"
			    "border: 1px solid #000000;"
			    "border-radius: 9px;"
			    "color: %4;"
			    "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 0.5 %3, stop: 1 %2);"
			    "}"
			    "QPushButton:hover {"
			    "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %2, stop: 0.5 %1, stop: 1 %2);"
			    "}"
			    "QPushButton:flat {"
			    "    border: none; /* no border for a flat push button */"
			    "}"
			    )
		    .arg(bgcolor.name())
		    .arg(bgcolor.darker(200).name())
		    .arg(bgcolor.lighter(200).name())
            .arg((bgcolor.value()<90)?"#ffffff":"#000000"));
  mol->einstellung->setValue("bondColor",mol->bondColor);
  mol->einstellung->setValue("singleColorBonds",mol->bondColorStyle);
  mol->einstellung->setValue("bondStrength",mol->bondStrength);
  }
  update();
}
void EacGLW::wallA(int st){
  if (st) mol->aStyle[xd.at(0).an]|=ATOM_STYLE_WALLS;
  else if (mol->aStyle[xd.at(0).an]&ATOM_STYLE_WALLS) mol->aStyle[xd.at(0).an]-=ATOM_STYLE_WALLS;
  //if (st==Qt::PartiallyChecked) mol->aStyle[xd.at(0).an]|=ATOM_STYLE_PLAID;
  //else if (mol->aStyle[xd.at(0).an]&ATOM_STYLE_PLAID) mol->aStyle[xd.at(0).an]-=ATOM_STYLE_PLAID;

}
void EacGLW::ringA(int st){
  if (st==Qt::PartiallyChecked) mol->aStyle[xd.at(0).an]|=ATOM_STYLE_WHITERING;
  else if (mol->aStyle[xd.at(0).an]&ATOM_STYLE_WHITERING) mol->aStyle[xd.at(0).an]-=ATOM_STYLE_WHITERING;
  if (st) mol->aStyle[xd.at(0).an]|=ATOM_STYLE_RINGS;
  else if (mol->aStyle[xd.at(0).an]&ATOM_STYLE_RINGS) mol->aStyle[xd.at(0).an]-=ATOM_STYLE_RINGS;

}
void EacGLW::spheA(int st){
  if (st) mol->aStyle[xd.at(0).an]|=ATOM_STYLE_SPHERE;
  else if (mol->aStyle[xd.at(0).an]&ATOM_STYLE_SPHERE) mol->aStyle[xd.at(0).an]-=ATOM_STYLE_SPHERE;

}
void EacGLW::soldA(int st){
  if (st) {
    mol->aStyle[xd.at(0).an]|=ATOM_STYLE_SOLID;
  }
  else if (mol->aStyle[xd.at(0).an]&ATOM_STYLE_SOLID) mol->aStyle[xd.at(0).an]-=ATOM_STYLE_SOLID;

}
void EacDlg::woADP(bool b){//WithOutADP
  walbx->setDisabled(b);
  rinbx->setDisabled(b);
  sphbx->setDisabled(b);
  sldbx->setDisabled(b);
  if (b) {
   eacGLW->mol->aStyle[eacGLW->xd.at(0).an]|=ATOM_STYLE_NOADP;
    sldbx->setChecked(b);
  }
  else
    if (eacGLW->mol->aStyle[eacGLW->xd.at(0).an]&ATOM_STYLE_NOADP)
      eacGLW->mol->aStyle[eacGLW->xd.at(0).an]-=ATOM_STYLE_NOADP;
}
void EacDlg::checkPolyCentr(){
    mol->allowedPolyeders[ eacGLW->xd[0].an]=pcentr->isChecked();
}
void EacGLW::whitA(int st){
  if (st) {
    mol->aStyle[xd.at(0).an]|=ATOM_STYLE_WHITERING;
  }
  else if (mol->aStyle[xd.at(0).an]&ATOM_STYLE_WHITERING) mol->aStyle[xd.at(0).an]-=ATOM_STYLE_WHITERING;
}

void EacGLW::nolaA(int st){
  if (st) {
    mol->aStyle[xd.at(0).an]|=ATOM_STYLE_NOLABEL;
  }
  else if (mol->aStyle[xd.at(0).an]&ATOM_STYLE_NOLABEL) mol->aStyle[xd.at(0).an]-=ATOM_STYLE_NOLABEL;
}

void EacGLW::noLis(int st){
  if (st) {
    mol->aStyle[xd.at(0).an]|=ATOM_STYLE_NOLISSAJOUS;
  }
  else if (mol->aStyle[xd.at(0).an]&ATOM_STYLE_NOLISSAJOUS) mol->aStyle[xd.at(0).an]-=ATOM_STYLE_NOLISSAJOUS;
}

/*
void EacGLW::metalA(int st){
  if (st) {
    //mol->aStyle[xd.at(0).an]|=ATOM_STYLE_METAL;
  }
  //else if (mol->aStyle[xd.at(0).an]&ATOM_STYLE_METAL) mol->aStyle[xd.at(0).an]-=ATOM_STYLE_METAL;
}*/
void EacDlg::checkWall(){
  if (sldbx->checkState()&&(walbx->checkState()))walbx->setCheckState(Qt::Unchecked);
}
void EacDlg::checkSolid(){
  if (sldbx->checkState()&&(walbx->checkState()))sldbx->setCheckState(Qt::Unchecked);
}
void EacDlg::checkBall(){
  if (sphbx->checkState())sldbx->setEnabled(true);
  else sldbx->setEnabled(false);
}
void EacDlg::updateLEs(int OZ){
  eacKRAD->setValue(eacGLW->mol->Kovalenz_Radien[OZ]/100.0);
  eacRAD->setValue(eacGLW->mol->arad[OZ]);
  rinbx->setCheckState((eacGLW->mol->aStyle[OZ]&ATOM_STYLE_RINGS)?((eacGLW->mol->aStyle[OZ]&ATOM_STYLE_WHITERING)?Qt::PartiallyChecked :Qt::Checked):Qt::Unchecked);
  pcentr->setCheckState(eacGLW->mol->allowedPolyeders.value(OZ,true)?Qt::Checked:Qt::Unchecked);
  sphbx->setCheckState((eacGLW->mol->aStyle[OZ]&ATOM_STYLE_SPHERE)?Qt::Checked:Qt::Unchecked);
  sldbx->setCheckState((eacGLW->mol->aStyle[OZ]&ATOM_STYLE_SOLID)?Qt::Checked:Qt::Unchecked);
  walbx->setCheckState((eacGLW->mol->aStyle[OZ]&ATOM_STYLE_WALLS)?Qt::Checked:Qt::Unchecked);
  nlabx->setCheckState((eacGLW->mol->aStyle[OZ]&ATOM_STYLE_NOLABEL)?Qt::Checked:Qt::Unchecked);
  nolis->setCheckState((eacGLW->mol->aStyle[OZ]&ATOM_STYLE_NOLISSAJOUS)?Qt::Checked:Qt::Unchecked);
  //metalic->setCheckState((eacGLW->mol->aStyle[OZ]&ATOM_STYLE_METAL)?Qt::Checked:Qt::Unchecked);
  noadp->setChecked((eacGLW->mol->aStyle[OZ]&ATOM_STYLE_NOADP)?true:false);
  woADP(noadp->isChecked());
  QColor ac=QColor((int)(mol->Acol[OZ][0]*255),
                   (int)(mol->Acol[OZ][1]*255),
                   (int)(mol->Acol[OZ][2]*255),
                   (int)(mol->Acol[OZ][3]*255));
  colorbut->setStyleSheet(QString(
			  "QPushButton {"
			  "border: 1px solid #000000;"
			  "border-radius: 9px;"
			  "color: %4;"
			  "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 0.5 %3, stop: 1 %2);"
			  "}"
			  "QPushButton:hover {"
			  "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %2, stop: 0.5 %1, stop: 1 %2);"
			  "}"
			  "QPushButton:flat {"
			  "    border: none; /* no border for a flat push button */"
			  "}"
			  )
          .arg(ac.name())
          .arg(ac.darker(200).name())
          .arg(ac.lighter(200).name())
          .arg((ac.value()<90)?"#ffffff":"#000000"));

  update();
}

void EacDlg::probaChanged(int ix){
  eacGLW->proba=mol->proba=eacProba->itemData(ix).toInt() ;
  update();
}

void EacDlg::unicolor(bool b){
  eacGLW->mol->bondColor = mol->bondColor;
  eacGLW->mol->bondColorStyle=mol->bondColorStyle=(b)?1:0;
  mol->einstellung->setValue("bondColor",mol->bondColor);
  mol->einstellung->setValue("singleColorBonds",mol->bondColorStyle);
  mol->einstellung->setValue("bondStrength",mol->bondStrength);
  update();
}

void EacDlg::strengthChanged(double d){
  eacGLW->mol->bondStrength=mol->bondStrength=d;
  mol->einstellung->setValue("bondColor",mol->bondColor);
  mol->einstellung->setValue("singleColorBonds",mol->bondColorStyle);
  mol->einstellung->setValue("bondStrength",mol->bondStrength);
  update();
}

void EacDlg::QPeakRadChanged(double d){
eacGLW->mol->qPeakRad=mol->qPeakRad=d;
  mol->einstellung->setValue("QPeaksRadius",mol->qPeakRad);
//settings...
update();	
}

void EacDlg::CPRadChanged(double d){
eacGLW->mol->CPRad=mol->CPRad=d;
  mol->einstellung->setValue("CPRadius",mol->CPRad);
//settings...
update();	
}

void EacDlg::lodChanged(int d){
  eacGLW->mol->LOD=d;
  mol->einstellung->setValue("LevelOfDetail",mol->LOD);

  update();
}

void EacDlg::importSettings(){
  QString dn=mol->einstellung->value("lastFile").toString();
  if (!dn.isEmpty()) {
    dn.replace(QRegExp(".ins$"),".ini");
    dn.replace(QRegExp(".res$"),".ini");
  }
  QString fn = QFileDialog::getOpenFileName (this,"Importing atom style settings...",dn,"Settings (*.ini)");
  if (fn.isNull())return;
  QSettings *eaSettings = new QSettings(fn,QSettings::IniFormat);

  //-------------------------
  //
  int ri=eaSettings->beginReadArray("CovaleceRadii");
  for (int i = 0; i < ri; ++i) {
    eaSettings->setArrayIndex(i);
    mol->Kovalenz_Radien[i-1]=(unsigned short)eaSettings->value("Radius").toInt();
  }
  eaSettings->endArray();

  ri = eaSettings->beginReadArray("BallRadii");
  for (int i = 0; i < ri; ++i) {
    eaSettings->setArrayIndex(i);
    mol->arad[i-1]=(GLfloat)eaSettings->value("Radius").toDouble();
  }
  eaSettings->endArray();
  ri = eaSettings->beginReadArray("AtomStyles");
  for (int i = 0; i < ri; ++i) {
    eaSettings->setArrayIndex(i);
   eacGLW->mol->aStyle[i-1]=eaSettings->value("Style" ).toInt();
  }
  eaSettings->endArray();
  ri = eaSettings->beginReadArray("AtomColors");
  for (int i = 0; i < ri; ++i) {
    eaSettings->setArrayIndex(i+1);
    eacGLW->mol->Acol[i][0] = (GLfloat)eaSettings->value("red"   ).toDouble();
    eacGLW->mol->Acol[i][1] = (GLfloat)eaSettings->value("green" ).toDouble();
    eacGLW->mol->Acol[i][2] = (GLfloat)eaSettings->value("blue"  ).toDouble();
    eacGLW->mol->Acol[i][3] = (GLfloat)eaSettings->value("alpha" ).toDouble();
  }
  eaSettings->endArray();
  mol->bondColor = eaSettings->value("bondColor").value<QColor>();
  mol->bondColorStyle = eaSettings->value("singleColorBonds").toInt();;
  mol->bondStrength = eaSettings->value("bondStrength").toDouble();

  delete eaSettings;

  //
  mol->einstellung->beginWriteArray("CovaleceRadii",95);
  for (int i = 0; i < 95; ++i) {
    mol->einstellung->setArrayIndex(i);
    mol->einstellung->setValue("Radius", mol->Kovalenz_Radien[i-1]);
    printf ("KRAD im %d = %d\n",i,mol->Kovalenz_Radien[i-1]);

  }
  mol->einstellung->endArray();

  mol->einstellung->beginWriteArray("BallRadii");
  for (int i = 0; i < 109; ++i) {
    mol->einstellung->setArrayIndex(i);
    mol->einstellung->setValue("Radius",(double)mol->arad[i-1]);
  }
  mol->einstellung->endArray();
  mol->einstellung->beginWriteArray("AtomStyles",107);
  for (int i = 0; i < 107; ++i) {
    mol->einstellung->setArrayIndex(i);
    mol->einstellung->setValue("Style", eacGLW->mol->aStyle[i-1]);
  }
  mol->einstellung->endArray();
  int asel=psew->selection;
  eaSettings->endArray();

  mol->einstellung->beginWriteArray("AtomColors",107);
  for (int i = 0; i < 107; ++i) {
    mol->einstellung->setArrayIndex(i);
    QColor ac=QColor( (int)(mol->Acol[i-1][0]*255),
                      (int)(mol->Acol[i-1][1]*255),
                      (int)(mol->Acol[i-1][2]*255),
                      (int)(mol->Acol[i-1][3]*255));
    mol->einstellung->value("red",   mol->Acol[i-1][0]);
    mol->einstellung->value("green", mol->Acol[i-1][1]);
    mol->einstellung->value("blue",  mol->Acol[i-1][2]);
    mol->einstellung->value("alpha", mol->Acol[i-1][3]);
    psew->selection=i;
    psew->colorchange(ac);
  }
  psew->selection=asel;
  mol->einstellung->endArray();

  mol->einstellung->value("bondColor",mol->bondColor);
  mol->einstellung->value("singleColorBonds",mol->bondColorStyle);
  mol->einstellung->value("bondStrength",mol->bondStrength);
  //--------------------------------------
}

void EacDlg::exportSettings(){
  QString dn=mol->einstellung->value("lastFile").toString();
  if (!dn.isEmpty()) {
    dn.replace(QRegExp(".ins$"),".ini");
    dn.replace(QRegExp(".res$"),".ini");
  }
  QString fn = QFileDialog::getSaveFileName (this,"Exporting atom style settings...",dn,"Settings (*.ini)");
  QSettings *eaSettings = new QSettings(fn,QSettings::IniFormat);
  //-------------------------
  //
  eaSettings->beginWriteArray("CovaleceRadii",95);
  for (int i = 0; i < 95; ++i) {
    eaSettings->setArrayIndex(i);
    eaSettings->setValue("Radius", mol->Kovalenz_Radien[i-1]);
  }
  eaSettings->endArray();

  eaSettings->beginWriteArray("BallRadii");
  for (int i = 0; i < 109; ++i) {
    eaSettings->setArrayIndex(i);
    eaSettings->setValue("Radius",mol->arad[i-1]);
  }
  eaSettings->endArray();
  eaSettings->beginWriteArray("AtomStyles",107);
  for (int i = 0; i < 107; ++i) {
    eaSettings->setArrayIndex(i);
    eaSettings->setValue("Style", eacGLW->mol->aStyle[i-1]);
  }
  eaSettings->endArray();
 eaSettings->beginWriteArray("AtomColors",107);
  for (int i = 0; i < 107; ++i) {
   eaSettings->setArrayIndex(i);
   eaSettings->setValue("red",   mol->Acol[i-1][0]);
   eaSettings->setValue("green", mol->Acol[i-1][1]);
   eaSettings->setValue("blue",  mol->Acol[i-1][2]);
   eaSettings->setValue("alpha", mol->Acol[i-1][3]);
  }
 eaSettings->endArray();
  eaSettings->setValue("bondColor",mol->bondColor);
  eaSettings->setValue("singleColorBonds",mol->bondColorStyle);
  eaSettings->setValue("bondStrength",mol->bondStrength);
  delete eaSettings;
  //--------------------------------------
  //
}

void EacDlg::kradChanged(double d){
    eacGLW->mol->Kovalenz_Radien[psew->selection]=mol->Kovalenz_Radien[psew->selection]=(unsigned short)(d*100);
   // qDebug()<<eacGLW->mol->Kovalenz_Radien[psew->selection]<<psew->selection<<d;
  mol->einstellung->beginWriteArray("CovaleceRadii",95);
  for (int i = 0; i < 95; ++i) {
    mol->einstellung->setArrayIndex(i);
    mol->einstellung->setValue("Radius", mol->Kovalenz_Radien[i-1]);
  }
  mol->einstellung->endArray();
  mol->einstellung->sync();
  update();
}

void EacDlg::radChanged(double d){

    eacGLW->mol->arad[psew->selection] = mol->arad[psew->selection] =(GLfloat) d;
  //eacGLW->xd[1].rad=d;

  mol->einstellung->beginWriteArray("BallRadii");
  for (int i = 0; i < 109; ++i) {
    mol->einstellung->setArrayIndex(i);
    mol->einstellung->setValue("Radius",(double)mol->arad[i-1]);
  }
  mol->einstellung->endArray();
  mol->einstellung->sync();

  update();
}

EacDlg::EacDlg(molekul *externMole){
  //otm=NULL;
  mol=externMole;
  mol->loadSettings();
  QString gr=mol->einstellung->group();
  if (gr!="Version 0.1")mol->einstellung->beginGroup("Version 0.1");
  setWindowTitle("Edit Atom Parameters");  
  //mol->loadSettings();
  eacGLW = new EacGLW(mol,this);
  connect(eacGLW,SIGNAL(colorchanged(QColor)),this,SLOT(colorchange(QColor)));
  eacGLW->mol->loadSettings();
  eacGLW->proba=mol->proba;
  //eacGLW->m=em;
  eacGLO = new QGridLayout; 
  //eacCB = new QComboBox;
  eacRAD = new QDoubleSpinBox();
  eacKRAD = new QDoubleSpinBox();
  //QLabel *smbl =new QLabel("Element symbol");
  QLabel *rad =new QLabel("Sphere radius");
  QLabel *krad =new QLabel("Covalent radius");
  QGroupBox *globalStuff = new QGroupBox("global style");
  QVBoxLayout *glt= new QVBoxLayout();
  eacProba = new QComboBox(this);
  eacProba->addItem("90 % Probability",90);
  eacProba->addItem("70 % Probability",70);
  eacProba->addItem("50 % Probability",50);
  eacProba->addItem("30 % Probability",30);
  eacProba->addItem("10 % Probability",10);
  eacProba->setCurrentIndex(2);
  switch (mol->proba){
	  case 10: eacProba->setCurrentIndex(4);break; 
	  case 30: eacProba->setCurrentIndex(3);break; 
	  case 50: eacProba->setCurrentIndex(2);break; 
	  case 70: eacProba->setCurrentIndex(1);break; 
	  case 90: eacProba->setCurrentIndex(0);break; 
  }
 // printf("zwei %d \n",mol->proba);
  connect(eacProba,SIGNAL(currentIndexChanged(int)),this,SLOT(probaChanged(int)));
  QCheckBox *singleC =new QCheckBox("uni colored bonds");
  singleC->setChecked(mol->bondColorStyle);
  unicolor(mol->bondColorStyle);
  connect(singleC,SIGNAL(toggled(bool)),this,SLOT(unicolor(bool)));
  impbut = new QPushButton("Import atom styles");
  expbut = new QPushButton("Export atom styles");
  connect(impbut,SIGNAL(pressed()),this,SLOT(importSettings()));
  connect(expbut,SIGNAL(pressed()),this,SLOT(exportSettings()));
  colorbut2 =new QPushButton("Bond Color");
  colorbut2->setStyleSheet(QString(
			  "QPushButton {"
			  "border: 1px solid #000000;"
			  "border-radius: 9px;"
			  "color: %4;"
			  "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 0.5 %3, stop: 1 %2);"
			  "}"
			  "QPushButton:hover {"
			  "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 0.5 %2, stop: 1 %1);"
			  "border: 0px"
			  "}"
			  "QPushButton:flat {"
			  "    border: none; /* no border for a flat push button */"
			  "}"
			  )
		  .arg(mol->bondColor.name())
		  .arg(mol->bondColor.darker(200).name())
		  .arg(mol->bondColor.lighter(200).name())
		  .arg((mol->bondColor.value()<90)?"#ffffff":"#000000"));
  connect(colorbut2,SIGNAL(pressed()),this,SLOT(colorDLG2()));
  bondStrength = new QDoubleSpinBox();
  bondStrength->setValue(mol->bondStrength);
  bondStrength->setDecimals(2);
  bondStrength->setSingleStep(0.01);
  bondStrength->setToolTip("Bond Strength");
  connect(bondStrength,SIGNAL(valueChanged(double)),this,SLOT(strengthChanged(double)) );
  glt->addWidget(eacProba);
  glt->addWidget(singleC);
  glt->addWidget(colorbut2);
  QHBoxLayout *glt2= new QHBoxLayout();
  glt2->addWidget(bondStrength);
  QLabel *bsl = new QLabel("Bond strength");
  QLabel *lodl = new QLabel("Level of detail");
  QLabel *qradl = new QLabel("QPeak-size");
  glt2->addWidget(bsl);
  lodSpinBox = new QSpinBox();
  lodSpinBox->setValue(mol->LOD);
  lodSpinBox->setMinimum(2);
  lodSpinBox->setMaximum(15);
  connect(lodSpinBox,SIGNAL(valueChanged(int)),this,SLOT(lodChanged(int)));
  QHBoxLayout *glt3= new QHBoxLayout();
  glt3->addWidget(lodSpinBox);
  glt3->addWidget(lodl);

  QDoubleSpinBox* qPeakSizeSBx = new QDoubleSpinBox();
  qPeakSizeSBx->setValue(mol->qPeakRad);
  qPeakSizeSBx->setDecimals(3);
  qPeakSizeSBx->setMinimum(0.005);
  qPeakSizeSBx->setMaximum(0.5);
  qPeakSizeSBx->setSingleStep(0.003);
  qPeakSizeSBx->setToolTip("QPeak-size");
  connect(qPeakSizeSBx,SIGNAL(valueChanged(double)),this,SLOT(QPeakRadChanged(double)) );
  QHBoxLayout *glt4= new QHBoxLayout();
  glt4->addWidget(qPeakSizeSBx);
  glt4->addWidget(qradl);

  QLabel *cpradl = new QLabel("CP-size");
  QDoubleSpinBox* CPSizeSBx = new QDoubleSpinBox();
  CPSizeSBx->setValue(mol->CPRad);
  CPSizeSBx->setDecimals(3);
  CPSizeSBx->setMinimum(0.005);
  CPSizeSBx->setMaximum(0.5);
  CPSizeSBx->setSingleStep(0.003);
  CPSizeSBx->setToolTip("QPeak-size");
  connect(CPSizeSBx,SIGNAL(valueChanged(double)),this,SLOT(CPRadChanged(double)) );
  QHBoxLayout *gltCP= new QHBoxLayout();
  gltCP->addWidget(CPSizeSBx);
  gltCP->addWidget(cpradl);

  glt->addLayout(glt2);
  glt->addLayout(glt3);
  glt->addLayout(glt4);
  glt->addLayout(gltCP);
  globalStuff->setLayout(glt);
  walbx = new QCheckBox("Draw intersecting planes");
//  walbx->setTristate();
  rinbx = new QCheckBox("Draw principal ellipses");
  rinbx->setTristate();
  sphbx = new QCheckBox("Draw ellipsoid surface");
  sldbx = new QCheckBox("Solid ellipsoid");
  noadp = new QCheckBox("No ADP");
  pcentr= new QCheckBox("Center of polyeder");
  //metalic = new QCheckBox("Dashed Bonds to others");
  nlabx = new QCheckBox("No label!");
  //metalic->setCheckState ( Qt::Unchecked);
  nolis = new QCheckBox("No Lissajous track");
  nolis->setCheckState ( Qt::Unchecked);
  walbx->setCheckState ( Qt::Checked);
  rinbx->setCheckState ( Qt::Checked);
  rinbx->setWhatsThis("If this check box is half selected <font color=gray><b>white</b></font> <b>outer lines</b> will be drawn around the ellipsoid which looks nice when the atom has a dark color.<br> If this is fully selected <b>black outer lines</b> will be drawn around the ellipsoid.");
  sphbx->setCheckState ( Qt::Checked);
  sldbx->setCheckState ( Qt::Unchecked);
  nlabx->setCheckState ( Qt::Unchecked);  
  noadp->setCheckState ( Qt::Unchecked);
  pcentr->setCheckState ( Qt::Checked);


  psew = new PSEWidget();
  connect(eacGLW,SIGNAL(colorchanged(QColor)),psew,SLOT(colorchange(QColor)));

  /*  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
      connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
      connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));*/
  connect(noadp,SIGNAL(toggled(bool)),this,SLOT(woADP(bool)));
  QPushButton *multi = new QPushButton("Style multiple Elements");
  colorbut =new QPushButton("Atom Color");
  QColor ac=QColor((int)(mol->Acol[0][0]*255),
                   (int)(mol->Acol[0][1]*255),
                   (int)(mol->Acol[0][2]*255),
                   (int)(mol->Acol[0][3]*255));

  colorbut->setStyleSheet(QString(
			  "QPushButton {"
			  "border: 1px solid #000000;"
			  "border-radius: 9px;"
			  "color: %4;"
			  "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 0.5 %3, stop: 1 %2);"
			  "}"
			  "QPushButton:hover {"
			  "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %2, stop: 0.5 %1, stop: 1 %2);"
			  "}"
			  "QPushButton:flat {"
			  "    border: none; /* no border for a flat push button */"
			  "}"
			  )
          .arg(ac.name())
          .arg(ac.darker(200).name())
          .arg(ac.lighter(200).name())
          .arg((ac.value()<90)?"#ffffff":"#000000"));
  eacKRAD->setValue(eacGLW->mol->Kovalenz_Radien[0]/100.0);
  eacKRAD->setDecimals(2);
  eacKRAD->setSingleStep(0.01);
  eacRAD->setValue(eacGLW->mol->arad[0]);
  eacRAD->setDecimals(2);
  eacRAD->setSingleStep(0.01);
  connect(eacKRAD,SIGNAL(valueChanged(double)),this,SLOT(kradChanged(double)) );
  connect(eacRAD,SIGNAL(valueChanged(double)),this,SLOT(radChanged(double)) );
  eacGLO->addWidget(eacGLW,0,0,6,6);
  eacGLO->addWidget(globalStuff,0,6,1,3);
  //eacGLO->addWidget(smbl,1,7);
  eacGLO->addWidget(rad,3,7);
  eacGLO->addWidget(krad,4,7);
  //eacGLO->addWidget(eacCB,1,6);
  eacGLO->addWidget(eacRAD,3,6);
  eacGLO->addWidget(eacKRAD,4,6);
  eacGLO->addWidget(colorbut,2,6,1,3);
  eacGLO->addWidget(nlabx,5,6);
  eacGLO->addWidget(nolis,5,7);
  eacGLO->addWidget(multi,4,8);
  eacGLO->addWidget(expbut,7,8);
  eacGLO->addWidget(impbut,7,7);
  eacGLO->addWidget(walbx,6,0);
  eacGLO->addWidget(rinbx,6,2);
  eacGLO->addWidget(sphbx,6,4);
  eacGLO->addWidget(sldbx,6,6);
  eacGLO->addWidget(noadp,6,7);
  eacGLO->addWidget(pcentr,6,8);
  //eacGLO->addWidget(buttonBox,7,0,1,8);
  eacGLO->addWidget(psew,1,6,1,3);
  setLayout(eacGLO);
  connect(multi,SIGNAL(pressed()),this , SLOT(doMulti()));
  connect(colorbut,SIGNAL(pressed()),eacGLW,SLOT(colorDLG()));
  connect(psew,SIGNAL(pseSelected(int)), eacGLW, SLOT(changeAtom(int)));
  connect(psew,SIGNAL(pseSelected(int)), this, SLOT(updateLEs(int)));
  updateLEs(0);
  connect(walbx,SIGNAL(stateChanged(int)),eacGLW,SLOT(wallA(int)));

  connect(nlabx,SIGNAL(stateChanged(int)),eacGLW,SLOT(nolaA(int)));
  connect(nolis,SIGNAL(stateChanged(int)),eacGLW,SLOT(noLis(int)));
  connect(walbx,SIGNAL(stateChanged(int)),this,SLOT(checkSolid()));
  connect(pcentr,SIGNAL(stateChanged(int)),this,SLOT(checkPolyCentr()));
  connect(rinbx,SIGNAL(stateChanged(int)),eacGLW,SLOT(ringA(int)));
  connect(sphbx,SIGNAL(stateChanged(int)),eacGLW,SLOT(spheA(int)));
  connect(sphbx,SIGNAL(stateChanged(int)),this,SLOT(checkBall()));
  connect(sldbx,SIGNAL(stateChanged(int)),eacGLW,SLOT(soldA(int)));
  connect(sldbx,SIGNAL(stateChanged(int)),this,SLOT(checkWall()));
  //connect(metalic,SIGNAL(stateChanged(int)),eacGLW,SLOT(metalA(int)));
}

void EacDlg::doMulti(){
  MultiStyle *m = new MultiStyle(mol);
  if (m->exec()==QDialog::Accepted){
    int stt=0;
    if (m->sphbx->checkState())stt|=ATOM_STYLE_SPHERE;
    if (m->rinbx->checkState())stt|=ATOM_STYLE_RINGS;
    if (m->rinbx->checkState()==Qt::PartiallyChecked)stt|=ATOM_STYLE_WHITERING;
    if (m->walbx->checkState())stt|=ATOM_STYLE_WALLS;
    //if (m->walbx->checkState()==Qt::PartiallyChecked)stt|=ATOM_STYLE_PLAID;
    if (m->sldbx->checkState())stt|=ATOM_STYLE_SOLID;
    //if (m->metalic->checkState())stt|=ATOM_STYLE_METAL;
    int mi=qMin(m->start->currentIndex(),m->end->currentIndex()),
	ma=qMax(m->start->currentIndex(),m->end->currentIndex());
    for (int i=mi;i<=ma;i++){
      mol->aStyle[i]=stt;
    }

  }
}

MultiStyle::MultiStyle(molekul *externMole){
  mol=externMole;
  setWindowTitle("Multiple Elements Styler");  
  QStringList psel;
  psel << "H" << "He" << "Li" << "Be" << "B" << "C" << "N" << "O" << "F" << "Ne" << "Na" << "Mg" << "Al" << "Si" << "P" << "S" << "Cl" << "Ar" <<
	  "K" << "Ca" << "Sc" << "Ti" << "V" << "Cr" << "Mn" << "Fe" << "Co" << "Ni" << "Cu" << "Zn" << "Ga" << "Ge" << "As" << "Se" << "Br" << "Kr" <<
	  "Rb" << "Sr" << "Y" << "Zr" << "Nb" << "Mo" << "Tc" << "Ru" << "Rh" << "Pd" << "Ag" << "Cd" << "In" << "Sn" << "Sb" << "Te" << "I" << "Xe" <<
	  "Cs" << "Ba" <<  "La" << "Ce" << "Pr" << "Nd" << "Pm" << "Sm" << "Eu" << "Gd" << "Tb" << "Dy" << "Ho" << "Er" << "Tm" << "Yb" << "Lu" <<
	  "Hf" << "Ta" << "W" << "Re" << "Os" << "Ir" << "Pt" << "Au" << "Hg" << "Tl" << "Pb" << "Bi" << "Po" << "At" << "Rn" << "Fr" << "Ra" <<
	  "Ac" << "Th" << "Pa" << "U" << "Np" << "Pu" << "Am" << "Cm" << "Bk" << "Cf" << "Es" << "Fm" << "Md" << "No" << "Lr" << "Ku" << "Ha" << "Rf" << "Ns" << "Hs" << "Mt";
  start = new QComboBox();
  end = new QComboBox();
  start->addItems(psel);
  start->setCurrentIndex(2);
  end->addItems(psel);
  end->setCurrentIndex(93);
  QLabel *staL =new QLabel("From: "); 
  QLabel *endL =new QLabel("To: "); 
  walbx = new QCheckBox("Draw intersecting planes");
  rinbx = new QCheckBox("Draw principal ellipses");
  rinbx->setTristate();
  sphbx = new QCheckBox("Draw ellipsoid surface");
  sldbx = new QCheckBox("Solid ellipsoid");
  //metalic = new QCheckBox("Dashed Bonds to others");
  //metalic->setCheckState ( Qt::Unchecked);
  walbx->setCheckState ( Qt::Checked);
  rinbx->setCheckState ( Qt::Checked);
  sphbx->setCheckState ( Qt::Checked);
  sldbx->setCheckState ( Qt::Unchecked);
  QGridLayout *eacGLO = new QGridLayout; 
  eacGLO->addWidget(staL,0,0,1,1,Qt::AlignRight);
  eacGLO->addWidget(endL,0,2,1,1,Qt::AlignRight);
  eacGLO->addWidget(start,0,1);
  eacGLO->addWidget(end,0,3);
  eacGLO->addWidget(walbx,1,0);
  eacGLO->addWidget(rinbx,1,1);
  eacGLO->addWidget(sphbx,1,2);
  eacGLO->addWidget(sldbx,1,3);
 // eacGLO->addWidget(metalic,1,4);
  connect(walbx,SIGNAL(stateChanged(int)),this,SLOT(checkSolid()));
  connect(sphbx,SIGNAL(stateChanged(int)),this,SLOT(checkBall()));
  connect(sldbx,SIGNAL(stateChanged(int)),this,SLOT(checkWall()));
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  eacGLO->addWidget(buttonBox,2,0,1,5);
  //  eacGLO->addWidget(,1,);
  setLayout(eacGLO);
}

void MultiStyle::checkWall(){
  if (sldbx->checkState()&&(walbx->checkState()))walbx->setCheckState(Qt::Unchecked);
}
void MultiStyle::checkSolid(){
  if (sldbx->checkState()&&(walbx->checkState()))sldbx->setCheckState(Qt::Unchecked);
}
void MultiStyle::checkBall(){
  if (sphbx->checkState())sldbx->setEnabled(true);
  else sldbx->setEnabled(false);
}

void EacDlg::reject(){
  eacGLW->timer->stop();
  mol->einstellung->beginWriteArray("AtomStyles",107);
  for (int i = 0; i < 107; ++i) {
    mol->einstellung->setArrayIndex(i);
    mol->einstellung->setValue("Style", eacGLW->mol->aStyle[i-1]);
   // printf("Index %d  Style %d\n",i,eacGLW->mol->aStyle[i-1]);
  }
  mol->einstellung->endArray();
  mol->pmin=eacGLW->exmin;
  mol->pmax=eacGLW->exmax;

  psew->esm->disconnect();
  delete psew->esm;
  psew->disconnect();
  eacGLW->disconnect();
  this->disconnect();
  delete eacGLW;
  done(QDialog::Rejected);
}

void EacDlg::accept(){
    qDebug()<<"accepted";
    /* the buuton is not there so this never happens
  eacGLW->timer->stop();
  int i=eacGLW->xd.at(0).an;
  //  extern molekul mol;
  mol->arad[i]=eacGLW->mol->arad[i]=eacRAD->text().toDouble();
  int stt=0;
  if (sphbx->checkState())stt|=ATOM_STYLE_SPHERE;
  if (rinbx->checkState())stt|=ATOM_STYLE_RINGS;
  if (rinbx->checkState()==Qt::PartiallyChecked)stt|=ATOM_STYLE_WHITERING;
  if (walbx->checkState())stt|=ATOM_STYLE_WALLS;
  if (sldbx->checkState())stt|=ATOM_STYLE_SOLID;
  if (nlabx->checkState())stt|=ATOM_STYLE_NOLABEL;
  if (noadp->checkState())stt|=ATOM_STYLE_NOADP;
  mol->aStyle[i]=eacGLW->mol->aStyle[i]=stt;
  mol->Kovalenz_Radien[i]=eacGLW->mol->Kovalenz_Radien[i]=(int)(eacKRAD->text().toDouble()*100);*/
  mol->einstellung->beginWriteArray("AtomStyles",107);
  for (int i = 0; i < 107; ++i) {
    mol->einstellung->setArrayIndex(i);
    mol->einstellung->setValue("Style", eacGLW->mol->aStyle[i-1]);
  }
  mol->einstellung->endArray();

  psew->esm->disconnect();
  delete psew->esm;
  psew->disconnect();
  eacGLW->disconnect();
  this->disconnect();
  done(QDialog::Accepted);
}
