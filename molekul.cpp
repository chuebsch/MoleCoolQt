#include "molekul.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <QtCore> 
#include <QtOpenGL>
#include <QtGui>
#define Ato4d(arr)       arr[0], arr[1], arr[2], arr[3]
void molekul::sphere(int adp){
  if (adp) glEnable(GL_BLEND);
  GLUquadricObj *q = gluNewQuadric();
  gluQuadricNormals(q, GL_SMOOTH);
  if (dratom){
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    if (dratom==3)glColor3d(0.4,0.8,1.0);
    if (dratom<3) glColor3d(1.0,1.0,0.0);
    gluQuadricDrawStyle(q,GLU_LINE);
    if (dratom==3)gluSphere(q,0.33,7,7);
    else if (dratom==5)gluSphere(q,1.00,7,7);
    else gluSphere(q,0.3,5,5);
    glEnable(GL_DEPTH_TEST);
  }
  else{
    gluQuadricDrawStyle(q,GLU_FILL);
    gluSphere(q,0.96,4*LOD,9*LOD);
    glDisable(GL_BLEND);
  }
  gluDeleteQuadric(q);
}


void molekul::ellipse(int style){
  if (style&ATOM_STYLE_WALLS){
  glEnable(GL_ALPHA_TEST);
  glDisable(GL_CULL_FACE);
  glBindTexture(GL_TEXTURE_2D, adpwall);
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glNormal3f(-1.0,0,0);
  
  glTexCoord2d(0,0);
  glVertex3f(0.0,-1.0,-1.0);
  glTexCoord2d(0,1);
  glVertex3f(0.0,-1.0, 1.0);
  glTexCoord2d(1,1);
  glVertex3f(0.0, 1.0, 1.0);
  glTexCoord2d(1,0);
  glVertex3f(0.0, 1.0,-1.0);
  
  glNormal3f(0.0,-1,0);
  glTexCoord2d(0,0);
  glVertex3f(-1.0,0.0,-1.0);
  glTexCoord2d(0,1);
  glVertex3f(-1.0,0.0, 1.0);
  glTexCoord2d(1,1);
  glVertex3f( 1.0,0.0, 1.0);
  glTexCoord2d(1,0);
  glVertex3f( 1.0,0.0,-1.0);

  glNormal3f(0.0,0,-1);
  glTexCoord2d(0,0);
  glVertex3f(-1.0,-1.0,0.0);
  glTexCoord2d(0,1);
  glVertex3f(-1.0, 1.0,0.0);
  glTexCoord2d(1,1);
  glVertex3f( 1.0, 1.0,0.0);
  glTexCoord2d(1,0);
  glVertex3f( 1.0,-1.0,0.0);
  glEnd();
  glDisable(GL_TEXTURE_2D);  
  }
  glEnable(GL_CULL_FACE);
  if ((style&ATOM_STYLE_SOLID)&& ( style&ATOM_STYLE_SPHERE)){
    GLUquadricObj *q = gluNewQuadric();
    gluQuadricNormals(q, GL_SMOOTH);   

    gluSphere(q,0.96,36,36);    
   }
  if (style&ATOM_STYLE_RINGS){
    if (style&ATOM_STYLE_WHITERING) glColor3f(1,1,1);
    else glColor3f(0,0,0);
    glBegin(GL_TRIANGLE_STRIP);
    glNormal3f( 0.0000000,  1.0000000,  0.0000000);//1
    glVertex3f( 0.0000000,  1.0000000,  0.0000000);//1
    glNormal3f( 0.0000000,  0.9848078, -0.0736482);//1
    glVertex3f( 0.0000000,  0.9848078, -0.0736482);//2
    glNormal3f( 0.3420201,  0.9396926,  0.0000000);//1
    glVertex3f( 0.3420201,  0.9396926,  0.0000000);//1
    glNormal3f( 0.3368241,  0.9254166, -0.0736482);//1
    glVertex3f( 0.3368241,  0.9254166, -0.0736482);//2
    glNormal3f( 0.6427876,  0.7660444,  0.0000000);//1
    glVertex3f( 0.6427876,  0.7660444,  0.0000000);//1
    glNormal3f( 0.6330222,  0.7544065, -0.0736482);//1
    glVertex3f( 0.6330222,  0.7544065, -0.0736482);//2
    glNormal3f( 0.8660254,  0.5000000,  0.0000000);//1
    glVertex3f( 0.8660254,  0.5000000,  0.0000000);//1
    glNormal3f( 0.8528685,  0.4924039, -0.0736482);//1
    glVertex3f( 0.8528685,  0.4924039, -0.0736482);//2
    glNormal3f( 0.9848078,  0.0736482,  0.0000000);//1
    glVertex3f( 0.9848078,  0.0736482,  0.0000000);//1
    glNormal3f( 0.9698463,  0.1710101, -0.0736482);//1
    glVertex3f( 0.9698463,  0.1710101, -0.0736482);//2
    glNormal3f( 0.9848078, -0.0736482,  0.0000000);//1
    glVertex3f( 0.9848078, -0.0736482,  0.0000000);//1
    glNormal3f( 0.9698463, -0.1710101, -0.0736482);//1
    glVertex3f( 0.9698463, -0.1710101, -0.0736482);//2
    glNormal3f( 0.8660254, -0.5000000,  0.0000000);//1
    glVertex3f( 0.8660254, -0.5000000,  0.0000000);//1
    glNormal3f( 0.8528685, -0.4924039, -0.0736482);//1
    glVertex3f( 0.8528685, -0.4924039, -0.0736482);//2
    glNormal3f( 0.6427876, -0.7660444,  0.0000000);//1
    glVertex3f( 0.6427876, -0.7660444,  0.0000000);//1
    glNormal3f( 0.6330222, -0.7544065, -0.0736482);//1
    glVertex3f( 0.6330222, -0.7544065, -0.0736482);//2
    glNormal3f( 0.3420201, -0.9396926,  0.0000000);//1
    glVertex3f( 0.3420201, -0.9396926,  0.0000000);//1
  glNormal3f( 0.3368241, -0.9254166, -0.0736482);//1
  glVertex3f( 0.3368241, -0.9254166, -0.0736482);//2
  glNormal3f( 0.0000000, -1.0000000,  0.0000000);//1
  glVertex3f( 0.0000000, -1.0000000,  0.0000000);//1
  glNormal3f( 0.0000000, -0.9848078, -0.0736482);//1
  glVertex3f( 0.0000000, -0.9848078, -0.0736482);//2
  glNormal3f(-0.3420201, -0.9396926,  0.0000000);//1
  glVertex3f(-0.3420201, -0.9396926,  0.0000000);//1
  glNormal3f(-0.3368241, -0.9254166, -0.0736482);//1
  glVertex3f(-0.3368241, -0.9254166, -0.0736482);//2
  glNormal3f(-0.6427876, -0.7660444,  0.0000000);//1
  glVertex3f(-0.6427876, -0.7660444,  0.0000000);//1
  glNormal3f(-0.6330222, -0.7544065, -0.0736482);//1
  glVertex3f(-0.6330222, -0.7544065, -0.0736482);//2
  glNormal3f(-0.8660254, -0.5000000,  0.0000000);//1
  glVertex3f(-0.8660254, -0.5000000,  0.0000000);//1
  glNormal3f(-0.8528685, -0.4924039, -0.0736482);//1
  glVertex3f(-0.8528685, -0.4924039, -0.0736482);//2
  glNormal3f(-0.9848078, -0.0736482,  0.0000000);//1
  glVertex3f(-0.9848078, -0.0736482,  0.0000000);//1
  glNormal3f(-0.9698463, -0.1710101, -0.0736482);//1
  glVertex3f(-0.9698463, -0.1710101, -0.0736482);//2
  glNormal3f(-0.9848078,  0.0736482,  0.0000000);//1
  glVertex3f(-0.9848078,  0.0736482,  0.0000000);//1
  glNormal3f(-0.9698463,  0.1710101, -0.0736482);//1
  glVertex3f(-0.9698463,  0.1710101, -0.0736482);//2
  glNormal3f(-0.8660254,  0.5000000,  0.0000000);//1
  glVertex3f(-0.8660254,  0.5000000,  0.0000000);//1
  glNormal3f(-0.8528685,  0.4924039, -0.0736482);//1
  glVertex3f(-0.8528685,  0.4924039, -0.0736482);//2
  glNormal3f(-0.6427876,  0.7660444,  0.0000000);//1
  glVertex3f(-0.6427876,  0.7660444,  0.0000000);//1
  glNormal3f(-0.6330222,  0.7544065, -0.0736482);//1
  glVertex3f(-0.6330222,  0.7544065, -0.0736482);//2
  glNormal3f(-0.3420201,  0.9396926,  0.0000000);//1
  glVertex3f(-0.3420201,  0.9396926,  0.0000000);//1
  glNormal3f(-0.3368241,  0.9254166, -0.0736482);//1
  glVertex3f(-0.3368241,  0.9254166, -0.0736482);//2
  glNormal3f( 0.0000000,  1.0000000,  0.0000000);//1
  glVertex3f( 0.0000000,  1.0000000,  0.0000000);//1
  glNormal3f( 0.0000000,  0.9848078, -0.0736482);//1
  glVertex3f( 0.0000000,  0.9848078, -0.0736482);//2


  glNormal3f( 0.0000000,  1.0000000, 0.0000000);//1
  glVertex3f( 0.0000000,  1.0000000, 0.0000000);//1
  glNormal3f( 0.0000000,  0.9848078, 0.0736482);//1
  glVertex3f( 0.0000000,  0.9848078, 0.0736482);//2
  glNormal3f( 0.3420201,  0.9396926, 0.0000000);//1
  glVertex3f( 0.3420201,  0.9396926, 0.0000000);//1
  glNormal3f( 0.3368241,  0.9254166, 0.0736482);//1
  glVertex3f( 0.3368241,  0.9254166, 0.0736482);//2
  glNormal3f( 0.6427876,  0.7660444, 0.0000000);//1
  glVertex3f( 0.6427876,  0.7660444, 0.0000000);//1
  glNormal3f( 0.6330222,  0.7544065, 0.0736482);//1
  glVertex3f( 0.6330222,  0.7544065, 0.0736482);//2
  glNormal3f( 0.8660254,  0.5000000, 0.0000000);//1
  glVertex3f( 0.8660254,  0.5000000, 0.0000000);//1
  glNormal3f( 0.8528685,  0.4924039, 0.0736482);//1
  glVertex3f( 0.8528685,  0.4924039, 0.0736482);//2
  glNormal3f( 0.9848078,  0.0736482, 0.0000000);//1
  glVertex3f( 0.9848078,  0.0736482, 0.0000000);//1
  glNormal3f( 0.9698463,  0.1710101, 0.0736482);//1
  glVertex3f( 0.9698463,  0.1710101, 0.0736482);//2
  glNormal3f( 0.9848078, -0.0736482, 0.0000000);//1
  glVertex3f( 0.9848078, -0.0736482, 0.0000000);//1
  glNormal3f( 0.9698463, -0.1710101, 0.0736482);//1
  glVertex3f( 0.9698463, -0.1710101, 0.0736482);//2
  glNormal3f( 0.8660254, -0.5000000, 0.0000000);//1
  glVertex3f( 0.8660254, -0.5000000, 0.0000000);//1
  glNormal3f( 0.8528685, -0.4924039, 0.0736482);//1
  glVertex3f( 0.8528685, -0.4924039, 0.0736482);//2
  glNormal3f( 0.6427876, -0.7660444, 0.0000000);//1
  glVertex3f( 0.6427876, -0.7660444, 0.0000000);//1
  glNormal3f( 0.6330222, -0.7544065, 0.0736482);//1
  glVertex3f( 0.6330222, -0.7544065, 0.0736482);//2
  glNormal3f( 0.3420201, -0.9396926, 0.0000000);//1
  glVertex3f( 0.3420201, -0.9396926, 0.0000000);//1
  glNormal3f( 0.3368241, -0.9254166, 0.0736482);//1
  glVertex3f( 0.3368241, -0.9254166, 0.0736482);//2
  glNormal3f( 0.0000000, -1.0000000, 0.0000000);//1
  glVertex3f( 0.0000000, -1.0000000, 0.0000000);//1
  glNormal3f( 0.0000000, -0.9848078, 0.0736482);//1
  glVertex3f( 0.0000000, -0.9848078, 0.0736482);//2
  glNormal3f(-0.3420201, -0.9396926, 0.0000000);//1
  glVertex3f(-0.3420201, -0.9396926, 0.0000000);//1
  glNormal3f(-0.3368241, -0.9254166, 0.0736482);//1
  glVertex3f(-0.3368241, -0.9254166, 0.0736482);//2
  glNormal3f(-0.6427876, -0.7660444, 0.0000000);//1
  glVertex3f(-0.6427876, -0.7660444, 0.0000000);//1
  glNormal3f(-0.6330222, -0.7544065, 0.0736482);//1
  glVertex3f(-0.6330222, -0.7544065, 0.0736482);//2
  glNormal3f(-0.8660254, -0.5000000, 0.0000000);//1
  glVertex3f(-0.8660254, -0.5000000, 0.0000000);//1
  glNormal3f(-0.8528685, -0.4924039, 0.0736482);//1
  glVertex3f(-0.8528685, -0.4924039, 0.0736482);//2
  glNormal3f(-0.9848078, -0.0736482, 0.0000000);//1
  glVertex3f(-0.9848078, -0.0736482, 0.0000000);//1
  glNormal3f(-0.9698463, -0.1710101, 0.0736482);//1
  glVertex3f(-0.9698463, -0.1710101, 0.0736482);//2
  glNormal3f(-0.9848078,  0.0736482, 0.0000000);//1
  glVertex3f(-0.9848078,  0.0736482, 0.0000000);//1
  glNormal3f(-0.9698463,  0.1710101, 0.0736482);//1
  glVertex3f(-0.9698463,  0.1710101, 0.0736482);//2
  glNormal3f(-0.8660254,  0.5000000, 0.0000000);//1
  glVertex3f(-0.8660254,  0.5000000, 0.0000000);//1
  glNormal3f(-0.8528685,  0.4924039, 0.0736482);//1
  glVertex3f(-0.8528685,  0.4924039, 0.0736482);//2
  glNormal3f(-0.6427876,  0.7660444, 0.0000000);//1
  glVertex3f(-0.6427876,  0.7660444, 0.0000000);//1
  glNormal3f(-0.6330222,  0.7544065, 0.0736482);//1
  glVertex3f(-0.6330222,  0.7544065, 0.0736482);//2
  glNormal3f(-0.3420201,  0.9396926, 0.0000000);//1
  glVertex3f(-0.3420201,  0.9396926, 0.0000000);//1
  glNormal3f(-0.3368241,  0.9254166, 0.0736482);//1
  glVertex3f(-0.3368241,  0.9254166, 0.0736482);//2
  glNormal3f( 0.0000000,  1.0000000, 0.0000000);//1
  glVertex3f( 0.0000000,  1.0000000, 0.0000000);//1
  glNormal3f( 0.0000000,  0.9848078, 0.0736482);//1
  glVertex3f( 0.0000000,  0.9848078, 0.0736482);//2

  glEnd();

  glBegin(GL_TRIANGLE_STRIP);
  glNormal3f(  0.0000000, 0.0000000,  1.0000000);//1
  glVertex3f(  0.0000000, 0.0000000,  1.0000000);//1
  glNormal3f( -0.0736482, 0.0000000,  0.9848078);//1
  glVertex3f( -0.0736482, 0.0000000,  0.9848078);//2
  glNormal3f(  0.0000000, 0.3420201,  0.9396926);//1
  glVertex3f(  0.0000000, 0.3420201,  0.9396926);//1
  glNormal3f( -0.0736482, 0.3368241,  0.9254166);//1
  glVertex3f( -0.0736482, 0.3368241,  0.9254166);//2
  glNormal3f(  0.0000000, 0.6427876,  0.7660444);//1
  glVertex3f(  0.0000000, 0.6427876,  0.7660444);//1
  glNormal3f( -0.0736482, 0.6330222,  0.7544065);//1
  glVertex3f( -0.0736482, 0.6330222,  0.7544065);//2
  glNormal3f(  0.0000000, 0.8660254,  0.5000000);//1
  glVertex3f(  0.0000000, 0.8660254,  0.5000000);//1
  glNormal3f( -0.0736482, 0.8528685,  0.4924039);//1
  glVertex3f( -0.0736482, 0.8528685,  0.4924039);//2
  glNormal3f(  0.0000000, 0.9848078,  0.0736482);//1
  glVertex3f(  0.0000000, 0.9848078,  0.0736482);//1
  glNormal3f( -0.0736482, 0.9698463,  0.1710101);//1
  glVertex3f( -0.0736482, 0.9698463,  0.1710101);//2
  glNormal3f(  0.0000000, 0.9848078, -0.0736482);//1
  glVertex3f(  0.0000000, 0.9848078, -0.0736482);//1
  glNormal3f( -0.0736482, 0.9698463, -0.1710101);//1
  glVertex3f( -0.0736482, 0.9698463, -0.1710101);//2
  glNormal3f(  0.0000000, 0.8660254, -0.5000000);//1
  glVertex3f(  0.0000000, 0.8660254, -0.5000000);//1
  glNormal3f( -0.0736482, 0.8528685, -0.4924039);//1
  glVertex3f( -0.0736482, 0.8528685, -0.4924039);//2
  glNormal3f(  0.0000000, 0.6427876, -0.7660444);//1
  glVertex3f(  0.0000000, 0.6427876, -0.7660444);//1
  glNormal3f( -0.0736482, 0.6330222, -0.7544065);//1
  glVertex3f( -0.0736482, 0.6330222, -0.7544065);//2
  glNormal3f(  0.0000000, 0.3420201, -0.9396926);//1
  glVertex3f(  0.0000000, 0.3420201, -0.9396926);//1
  glNormal3f( -0.0736482, 0.3368241, -0.9254166);//1
  glVertex3f( -0.0736482, 0.3368241, -0.9254166);//2
  glNormal3f(  0.0000000, 0.0000000, -1.0000000);//1
  glVertex3f(  0.0000000, 0.0000000, -1.0000000);//1
  glNormal3f( -0.0736482, 0.0000000, -0.9848078);//1
  glVertex3f( -0.0736482, 0.0000000, -0.9848078);//2
  glNormal3f(  0.0000000,-0.3420201, -0.9396926);//1
  glVertex3f(  0.0000000,-0.3420201, -0.9396926);//1
  glNormal3f( -0.0736482,-0.3368241, -0.9254166);//1
  glVertex3f( -0.0736482,-0.3368241, -0.9254166);//2
  glNormal3f(  0.0000000,-0.6427876, -0.7660444);//1
  glVertex3f(  0.0000000,-0.6427876, -0.7660444);//1
  glNormal3f( -0.0736482,-0.6330222, -0.7544065);//1
  glVertex3f( -0.0736482,-0.6330222, -0.7544065);//2
  glNormal3f(  0.0000000,-0.8660254, -0.5000000);//1
  glVertex3f(  0.0000000,-0.8660254, -0.5000000);//1
  glNormal3f( -0.0736482,-0.8528685, -0.4924039);//1
  glVertex3f( -0.0736482,-0.8528685, -0.4924039);//2
  glNormal3f(  0.0000000,-0.9848078, -0.0736482);//1
  glVertex3f(  0.0000000,-0.9848078, -0.0736482);//1
  glNormal3f( -0.0736482,-0.9698463, -0.1710101);//1
  glVertex3f( -0.0736482,-0.9698463, -0.1710101);//2
  glNormal3f(  0.0000000,-0.9848078,  0.0736482);//1
  glVertex3f(  0.0000000,-0.9848078,  0.0736482);//1
  glNormal3f( -0.0736482,-0.9698463,  0.1710101);//1
  glVertex3f( -0.0736482,-0.9698463,  0.1710101);//2
  glNormal3f(  0.0000000,-0.8660254,  0.5000000);//1
  glVertex3f(  0.0000000,-0.8660254,  0.5000000);//1
  glNormal3f( -0.0736482,-0.8528685,  0.4924039);//1
  glVertex3f( -0.0736482,-0.8528685,  0.4924039);//2
  glNormal3f(  0.0000000,-0.6427876,  0.7660444);//1
  glVertex3f(  0.0000000,-0.6427876,  0.7660444);//1
  glNormal3f( -0.0736482,-0.6330222,  0.7544065);//1
  glVertex3f( -0.0736482,-0.6330222,  0.7544065);//2
  glNormal3f(  0.0000000,-0.3420201,  0.9396926);//1
  glVertex3f(  0.0000000,-0.3420201,  0.9396926);//1
  glNormal3f( -0.0736482,-0.3368241,  0.9254166);//1
  glVertex3f( -0.0736482,-0.3368241,  0.9254166);//2
  glNormal3f(  0.0000000, 0.0000000,  1.0000000);//1
  glVertex3f(  0.0000000, 0.0000000,  1.0000000);//1
  glNormal3f( -0.0736482, 0.0000000,  0.9848078);//1
  glVertex3f( -0.0736482, 0.0000000,  0.9848078);//2

  glNormal3f( 0.0000000, 0.0000000,  1.0000000);//1
  glVertex3f( 0.0000000, 0.0000000,  1.0000000);//1
  glNormal3f( 0.0736482, 0.0000000,  0.9848078);//1
  glVertex3f( 0.0736482, 0.0000000,  0.9848078);//2
  glNormal3f( 0.0000000, 0.3420201,  0.9396926);//1
  glVertex3f( 0.0000000, 0.3420201,  0.9396926);//1
  glNormal3f( 0.0736482, 0.3368241,  0.9254166);//1
  glVertex3f( 0.0736482, 0.3368241,  0.9254166);//2
  glNormal3f( 0.0000000, 0.6427876,  0.7660444);//1
  glVertex3f( 0.0000000, 0.6427876,  0.7660444);//1
  glNormal3f( 0.0736482, 0.6330222,  0.7544065);//1
  glVertex3f( 0.0736482, 0.6330222,  0.7544065);//2
  glNormal3f( 0.0000000, 0.8660254,  0.5000000);//1
  glVertex3f( 0.0000000, 0.8660254,  0.5000000);//1
  glNormal3f( 0.0736482, 0.8528685,  0.4924039);//1
  glVertex3f( 0.0736482, 0.8528685,  0.4924039);//2
  glNormal3f( 0.0000000, 0.9848078,  0.0736482);//1
  glVertex3f( 0.0000000, 0.9848078,  0.0736482);//1
  glNormal3f( 0.0736482, 0.9698463,  0.1710101);//1
  glVertex3f( 0.0736482, 0.9698463,  0.1710101);//2
  glNormal3f( 0.0000000, 0.9848078, -0.0736482);//1
  glVertex3f( 0.0000000, 0.9848078, -0.0736482);//1
  glNormal3f( 0.0736482, 0.9698463, -0.1710101);//1
  glVertex3f( 0.0736482, 0.9698463, -0.1710101);//2
  glNormal3f( 0.0000000, 0.8660254, -0.5000000);//1
  glVertex3f( 0.0000000, 0.8660254, -0.5000000);//1
  glNormal3f( 0.0736482, 0.8528685, -0.4924039);//1
  glVertex3f( 0.0736482, 0.8528685, -0.4924039);//2
  glNormal3f( 0.0000000, 0.6427876, -0.7660444);//1
  glVertex3f( 0.0000000, 0.6427876, -0.7660444);//1
  glNormal3f( 0.0736482, 0.6330222, -0.7544065);//1
  glVertex3f( 0.0736482, 0.6330222, -0.7544065);//2
  glNormal3f( 0.0000000, 0.3420201, -0.9396926);//1
  glVertex3f( 0.0000000, 0.3420201, -0.9396926);//1
  glNormal3f( 0.0736482, 0.3368241, -0.9254166);//1
  glVertex3f( 0.0736482, 0.3368241, -0.9254166);//2
  glNormal3f( 0.0000000, 0.0000000, -1.0000000);//1
  glVertex3f( 0.0000000, 0.0000000, -1.0000000);//1
  glNormal3f( 0.0736482, 0.0000000, -0.9848078);//1
  glVertex3f( 0.0736482, 0.0000000, -0.9848078);//2
  glNormal3f( 0.0000000,-0.3420201, -0.9396926);//1
  glVertex3f( 0.0000000,-0.3420201, -0.9396926);//1
  glNormal3f( 0.0736482,-0.3368241, -0.9254166);//1
  glVertex3f( 0.0736482,-0.3368241, -0.9254166);//2
  glNormal3f( 0.0000000,-0.6427876, -0.7660444);//1
  glVertex3f( 0.0000000,-0.6427876, -0.7660444);//1
  glNormal3f( 0.0736482,-0.6330222, -0.7544065);//1
  glVertex3f( 0.0736482,-0.6330222, -0.7544065);//2
  glNormal3f( 0.0000000,-0.8660254, -0.5000000);//1
  glVertex3f( 0.0000000,-0.8660254, -0.5000000);//1
  glNormal3f( 0.0736482,-0.8528685, -0.4924039);//1
  glVertex3f( 0.0736482,-0.8528685, -0.4924039);//2
  glNormal3f( 0.0000000,-0.9848078, -0.0736482);//1
  glVertex3f( 0.0000000,-0.9848078, -0.0736482);//1
  glNormal3f( 0.0736482,-0.9698463, -0.1710101);//1
  glVertex3f( 0.0736482,-0.9698463, -0.1710101);//2
  glNormal3f( 0.0000000,-0.9848078,  0.0736482);//1
  glVertex3f( 0.0000000,-0.9848078,  0.0736482);//1
  glNormal3f( 0.0736482,-0.9698463,  0.1710101);//1
  glVertex3f( 0.0736482,-0.9698463,  0.1710101);//2
  glNormal3f( 0.0000000,-0.8660254,  0.5000000);//1
  glVertex3f( 0.0000000,-0.8660254,  0.5000000);//1
  glNormal3f( 0.0736482,-0.8528685,  0.4924039);//1
  glVertex3f( 0.0736482,-0.8528685,  0.4924039);//2
  glNormal3f( 0.0000000,-0.6427876,  0.7660444);//1
  glVertex3f( 0.0000000,-0.6427876,  0.7660444);//1
  glNormal3f( 0.0736482,-0.6330222,  0.7544065);//1
  glVertex3f( 0.0736482,-0.6330222,  0.7544065);//2
  glNormal3f( 0.0000000,-0.3420201,  0.9396926);//1
  glVertex3f( 0.0000000,-0.3420201,  0.9396926);//1
  glNormal3f( 0.0736482,-0.3368241,  0.9254166);//1
  glVertex3f( 0.0736482,-0.3368241,  0.9254166);//2
  glNormal3f( 0.0000000, 0.0000000,  1.0000000);//1
  glVertex3f( 0.0000000, 0.0000000,  1.0000000);//1
  glNormal3f( 0.0736482, 0.0000000,  0.9848078);//1
  glVertex3f( 0.0736482, 0.0000000,  0.9848078);//2

  glEnd();


  glBegin(GL_TRIANGLE_STRIP);
  glNormal3f( 0.0000000,  0.0000000,  1.0000000);//1
  glVertex3f( 0.0000000,  0.0000000,  1.0000000);//1
  glNormal3f( 0.0000000, -0.0736482,  0.9848078);//1
  glVertex3f( 0.0000000, -0.0736482,  0.9848078);//2
  glNormal3f( 0.3420201,  0.0000000,  0.9396926);//1
  glVertex3f( 0.3420201,  0.0000000,  0.9396926);//1
  glNormal3f( 0.3368241, -0.0736482,  0.9254166);//1
  glVertex3f( 0.3368241, -0.0736482,  0.9254166);//2
  glNormal3f( 0.6427876,  0.0000000,  0.7660444);//1
  glVertex3f( 0.6427876,  0.0000000,  0.7660444);//1
  glNormal3f( 0.6330222, -0.0736482,  0.7544065);//1
  glVertex3f( 0.6330222, -0.0736482,  0.7544065);//2
  glNormal3f( 0.8660254,  0.0000000,  0.5000000);//1
  glVertex3f( 0.8660254,  0.0000000,  0.5000000);//1
  glNormal3f( 0.8528685, -0.0736482,  0.4924039);//1
  glVertex3f( 0.8528685, -0.0736482,  0.4924039);//2
  glNormal3f( 0.9848078,  0.0000000,  0.0736482);//1
  glVertex3f( 0.9848078,  0.0000000,  0.0736482);//1
  glNormal3f( 0.9698463, -0.0736482,  0.1710101);//1
  glVertex3f( 0.9698463, -0.0736482,  0.1710101);//2
  glNormal3f( 0.9848078,  0.0000000, -0.0736482);//1
  glVertex3f( 0.9848078,  0.0000000, -0.0736482);//1
  glNormal3f( 0.9698463, -0.0736482, -0.1710101);//1
  glVertex3f( 0.9698463, -0.0736482, -0.1710101);//2
  glNormal3f( 0.8660254,  0.0000000, -0.5000000);//1
  glVertex3f( 0.8660254,  0.0000000, -0.5000000);//1
  glNormal3f( 0.8528685, -0.0736482, -0.4924039);//1
  glVertex3f( 0.8528685, -0.0736482, -0.4924039);//2
  glNormal3f( 0.6427876,  0.0000000, -0.7660444);//1
  glVertex3f( 0.6427876,  0.0000000, -0.7660444);//1
  glNormal3f( 0.6330222, -0.0736482, -0.7544065);//1
  glVertex3f( 0.6330222, -0.0736482, -0.7544065);//2
  glNormal3f( 0.3420201,  0.0000000, -0.9396926);//1
  glVertex3f( 0.3420201,  0.0000000, -0.9396926);//1
  glNormal3f( 0.3368241, -0.0736482, -0.9254166);//1
  glVertex3f( 0.3368241, -0.0736482, -0.9254166);//2
  glNormal3f( 0.0000000,  0.0000000, -1.0000000);//1
  glVertex3f( 0.0000000,  0.0000000, -1.0000000);//1
  glNormal3f( 0.0000000, -0.0736482, -0.9848078);//1
  glVertex3f( 0.0000000, -0.0736482, -0.9848078);//2
  glNormal3f(-0.3420201,  0.0000000, -0.9396926);//1
  glVertex3f(-0.3420201,  0.0000000, -0.9396926);//1
  glNormal3f(-0.3368241, -0.0736482, -0.9254166);//1
  glVertex3f(-0.3368241, -0.0736482, -0.9254166);//2
  glNormal3f(-0.6427876,  0.0000000, -0.7660444);//1
  glVertex3f(-0.6427876,  0.0000000, -0.7660444);//1
  glNormal3f(-0.6330222, -0.0736482, -0.7544065);//1
  glVertex3f(-0.6330222, -0.0736482, -0.7544065);//2
  glNormal3f(-0.8660254,  0.0000000, -0.5000000);//1
  glVertex3f(-0.8660254,  0.0000000, -0.5000000);//1
  glNormal3f(-0.8528685, -0.0736482, -0.4924039);//1
  glVertex3f(-0.8528685, -0.0736482, -0.4924039);//2
  glNormal3f(-0.9848078,  0.0000000, -0.0736482);//1
  glVertex3f(-0.9848078,  0.0000000, -0.0736482);//1
  glNormal3f(-0.9698463, -0.0736482, -0.1710101);//1
  glVertex3f(-0.9698463, -0.0736482, -0.1710101);//2
  glNormal3f(-0.9848078,  0.0000000,  0.0736482);//1
  glVertex3f(-0.9848078,  0.0000000,  0.0736482);//1
  glNormal3f(-0.9698463, -0.0736482,  0.1710101);//1
  glVertex3f(-0.9698463, -0.0736482,  0.1710101);//2
  glNormal3f(-0.8660254,  0.0000000,  0.5000000);//1
  glVertex3f(-0.8660254,  0.0000000,  0.5000000);//1
  glNormal3f(-0.8528685, -0.0736482,  0.4924039);//1
  glVertex3f(-0.8528685, -0.0736482,  0.4924039);//2
  glNormal3f(-0.6427876,  0.0000000,  0.7660444);//1
  glVertex3f(-0.6427876,  0.0000000,  0.7660444);//1
  glNormal3f(-0.6330222, -0.0736482,  0.7544065);//1
  glVertex3f(-0.6330222, -0.0736482,  0.7544065);//2
  glNormal3f(-0.3420201,  0.0000000,  0.9396926);//1
  glVertex3f(-0.3420201,  0.0000000,  0.9396926);//1
  glNormal3f(-0.3368241, -0.0736482,  0.9254166);//1
  glVertex3f(-0.3368241, -0.0736482,  0.9254166);//2
  glNormal3f( 0.0000000,  0.0000000,  1.0000000);//1
  glVertex3f( 0.0000000,  0.0000000,  1.0000000);//1
  glNormal3f( 0.0000000, -0.0736482,  0.9848078);//1
  glVertex3f( 0.0000000, -0.0736482,  0.9848078);//2

  glNormal3f( 0.0000000, 0.0000000,  1.0000000);//1
  glVertex3f( 0.0000000, 0.0000000,  1.0000000);//1
  glNormal3f( 0.0000000, 0.0736482,  0.9848078);//1
  glVertex3f( 0.0000000, 0.0736482,  0.9848078);//2
  glNormal3f( 0.3420201, 0.0000000,  0.9396926);//1
  glVertex3f( 0.3420201, 0.0000000,  0.9396926);//1
  glNormal3f( 0.3368241, 0.0736482,  0.9254166);//1
  glVertex3f( 0.3368241, 0.0736482,  0.9254166);//2
  glNormal3f( 0.6427876, 0.0000000,  0.7660444);//1
  glVertex3f( 0.6427876, 0.0000000,  0.7660444);//1
  glNormal3f( 0.6330222, 0.0736482,  0.7544065);//1
  glVertex3f( 0.6330222, 0.0736482,  0.7544065);//2
  glNormal3f( 0.8660254, 0.0000000,  0.5000000);//1
  glVertex3f( 0.8660254, 0.0000000,  0.5000000);//1
  glNormal3f( 0.8528685, 0.0736482,  0.4924039);//1
  glVertex3f( 0.8528685, 0.0736482,  0.4924039);//2
  glNormal3f( 0.9848078, 0.0000000,  0.0736482);//1
  glVertex3f( 0.9848078, 0.0000000,  0.0736482);//1
  glNormal3f( 0.9698463, 0.0736482,  0.1710101);//1
  glVertex3f( 0.9698463, 0.0736482,  0.1710101);//2
  glNormal3f( 0.9848078, 0.0000000, -0.0736482);//1
  glVertex3f( 0.9848078, 0.0000000, -0.0736482);//1
  glNormal3f( 0.9698463, 0.0736482, -0.1710101);//1
  glVertex3f( 0.9698463, 0.0736482, -0.1710101);//2
  glNormal3f( 0.8660254, 0.0000000, -0.5000000);//1
  glVertex3f( 0.8660254, 0.0000000, -0.5000000);//1
  glNormal3f( 0.8528685, 0.0736482, -0.4924039);//1
  glVertex3f( 0.8528685, 0.0736482, -0.4924039);//2
  glNormal3f( 0.6427876, 0.0000000, -0.7660444);//1
  glVertex3f( 0.6427876, 0.0000000, -0.7660444);//1
  glNormal3f( 0.6330222, 0.0736482, -0.7544065);//1
  glVertex3f( 0.6330222, 0.0736482, -0.7544065);//2
  glNormal3f( 0.3420201, 0.0000000, -0.9396926);//1
  glVertex3f( 0.3420201, 0.0000000, -0.9396926);//1
  glNormal3f( 0.3368241, 0.0736482, -0.9254166);//1
  glVertex3f( 0.3368241, 0.0736482, -0.9254166);//2
  glNormal3f( 0.0000000, 0.0000000, -1.0000000);//1
  glVertex3f( 0.0000000, 0.0000000, -1.0000000);//1
  glNormal3f( 0.0000000, 0.0736482, -0.9848078);//1
  glVertex3f( 0.0000000, 0.0736482, -0.9848078);//2
  glNormal3f(-0.3420201, 0.0000000, -0.9396926);//1
  glVertex3f(-0.3420201, 0.0000000, -0.9396926);//1
  glNormal3f(-0.3368241, 0.0736482, -0.9254166);//1
  glVertex3f(-0.3368241, 0.0736482, -0.9254166);//2
  glNormal3f(-0.6427876, 0.0000000, -0.7660444);//1
  glVertex3f(-0.6427876, 0.0000000, -0.7660444);//1
  glNormal3f(-0.6330222, 0.0736482, -0.7544065);//1
  glVertex3f(-0.6330222, 0.0736482, -0.7544065);//2
  glNormal3f(-0.8660254, 0.0000000, -0.5000000);//1
  glVertex3f(-0.8660254, 0.0000000, -0.5000000);//1
  glNormal3f(-0.8528685, 0.0736482, -0.4924039);//1
  glVertex3f(-0.8528685, 0.0736482, -0.4924039);//2
  glNormal3f(-0.9848078, 0.0000000, -0.0736482);//1
  glVertex3f(-0.9848078, 0.0000000, -0.0736482);//1
  glNormal3f(-0.9698463, 0.0736482, -0.1710101);//1
  glVertex3f(-0.9698463, 0.0736482, -0.1710101);//2
  glNormal3f(-0.9848078, 0.0000000,  0.0736482);//1
  glVertex3f(-0.9848078, 0.0000000,  0.0736482);//1
  glNormal3f(-0.9698463, 0.0736482,  0.1710101);//1
  glVertex3f(-0.9698463, 0.0736482,  0.1710101);//2
  glNormal3f(-0.8660254, 0.0000000,  0.5000000);//1
  glVertex3f(-0.8660254, 0.0000000,  0.5000000);//1
  glNormal3f(-0.8528685, 0.0736482,  0.4924039);//1
  glVertex3f(-0.8528685, 0.0736482,  0.4924039);//2
  glNormal3f(-0.6427876, 0.0000000,  0.7660444);//1
  glVertex3f(-0.6427876, 0.0000000,  0.7660444);//1
  glNormal3f(-0.6330222, 0.0736482,  0.7544065);//1
  glVertex3f(-0.6330222, 0.0736482,  0.7544065);//2
  glNormal3f(-0.3420201, 0.0000000,  0.9396926);//1
  glVertex3f(-0.3420201, 0.0000000,  0.9396926);//1
  glNormal3f(-0.3368241, 0.0736482,  0.9254166);//1
  glVertex3f(-0.3368241, 0.0736482,  0.9254166);//2
  glNormal3f( 0.0000000, 0.0000000,  1.0000000);//1
  glVertex3f( 0.0000000, 0.0000000,  1.0000000);//1
  glNormal3f( 0.0000000, 0.0736482,  0.9848078);//1
  glVertex3f( 0.0000000, 0.0736482,  0.9848078);//2

  glEnd();
  }
  

}
void dCube(GLfloat rad){
  glBegin(GL_QUADS);
  glNormal3f(0,0,1);
  glVertex3f(-rad,-rad, rad);
  glVertex3f( rad,-rad, rad);
  glVertex3f( rad, rad, rad);
  glVertex3f(-rad, rad, rad);
  glNormal3f(0,0,-1);
  glVertex3f(-rad,-rad, -rad);
  glVertex3f(-rad, rad, -rad);
  glVertex3f( rad, rad, -rad);
  glVertex3f( rad,-rad, -rad);
  glNormal3f(0,1,0);
  glVertex3f(-rad, rad,-rad);
  glVertex3f(-rad, rad, rad);
  glVertex3f( rad, rad, rad);
  glVertex3f( rad, rad,-rad);
  glNormal3f(0,-1,0);
  glVertex3f(-rad, -rad,-rad);
  glVertex3f( rad, -rad,-rad);
  glVertex3f( rad, -rad, rad);
  glVertex3f(-rad, -rad, rad);
  glNormal3f(1,0,0);
  glVertex3f(rad, -rad, -rad);
  glVertex3f(rad, rad, -rad);
  glVertex3f(rad, rad, rad);
  glVertex3f(rad, -rad, rad);
  glNormal3f(-1,0,0);
  glVertex3f(-rad, -rad, -rad);
  glVertex3f(-rad, -rad, rad);
  glVertex3f(-rad, rad, rad);
  glVertex3f(-rad, rad, -rad);
  glEnd();
}
/*void rhombo(double R){
  GLfloat p=0.5f*(1+sqrt(5.0f));
  GLfloat p2=p*p;
  GLfloat p3=p2*p;
  p*=R;
  p2*=R;
  p3*=R;
  GLfloat vert[96] = {p2,0,p3,0,p,p3,-p2,0,p3,0,-p,p3,p2,p2,p2,0,p3,p2,-p2,p2,p2,-p2,-p2,p2,0,-p3,p2,p2,-p2,p2,p3,0,p,-p3,0,p,p3,p2,0,p,p3,0,-p,p3,0,-p3,p2,0,-p3,-p2,0,
-p,-p3,0,p,-p3,0,p3,-p2,0,p3,0,-p,-p3,0,-p,p2,p2,-p2,0,p3,-p2,-p2,p2,-p2,-p2,-p2,-p2,0,-p3,-p2,p2,-p2,-p2,p2,0,-p3,0,p,-p3,-p2,0,-p3,0,-p,-p3};

GLubyte indices[180] = {0,4,12,12,10,0,0,9,8,8,3,0,0,10,19,19,9,0,0,1,5,5,4,0,0,1,2,2,3,0,2,6,5,5,1,2,2,7,16,16,11,2,2,11,15,15,6,2,2,3,8,8,7,2,5,6,15,15,14,5,5,13,12,12,4,5,5,14,
  23,23,13,5,8,9,19,19,18,8,8,17,16,16,7,8,8,18,26,26,17,8,12,13,23,23,22,12,12,20,19,19,10,12,12,22,28,28,20,12,15,11,16,16,21,15,15,21,30,30,24,15,15,24,23,23,14,15,16,17,
  26,26,25,16,16,25,30,30,21,16,19,20,28,28,27,19,19,27,26,26,18,19,23,24,30,30,29,23,23,29,28,28,22,23,26,27,28,28,31,26,26,31,30,30,25,26,28,29,30,30,31,28};
glVertexPointer(3,GL_FLOAT,0,vert);
glDrawElements(GL_TRIANGLES,60,GL_UNSIGNED_BYTE,indices);
}
*/
void ikosa(double R){
    /*
    (0, Â±1, Â±f)
    (Â±1, Â±f, 0)
    (Â±f, 0, Â±1)
    */

double P =R*(1 / 2.0 *(1+sqrt(5)));
double xx=3*sqrt(1+P*P);
double l=1/xx, p=P/xx;
glBegin(GL_TRIANGLES);

glNormal3f(-p,0,2*p+l);//123
glVertex3f(0,-R,P); //1
glVertex3f(0, R,P); //2
glVertex3f(-P,0,R);//3

glNormal3f(p,0,l+2*p);//124
glVertex3f( P, 0,R);//4
glVertex3f( 0, R,P);//2
glVertex3f( 0,-R,P);//1

glNormal3f(-l-2*p,-p,0);//356
glVertex3f(-R,-P, 0);//5
glVertex3f(-P, 0, R);//3
glVertex3f(-P, 0,-R);//6

glNormal3f(-p-l,-l-p,l+p);//135
glVertex3f(-P, 0,R);//3
glVertex3f(-R,-P,0);//5
glVertex3f( 0,-R,P); //1

glNormal3f(l+p,-l-p,l+p);//148
glVertex3f(0,-R,P);//1
glVertex3f(R,-P,0);//8
glVertex3f(P, 0,R);//4

glNormal3f(0,-l-2*p,p);//158
glVertex3f( 0,-R,P); //1
glVertex3f(-R,-P,0);//5
glVertex3f( R,-P,0);//8

glNormal3f(l+2*p,-p,0);//478
glVertex3f(R,-P, 0);//8
glVertex3f(P, 0,-R);//7
glVertex3f(P, 0, R);//4

glNormal3f(2*p+l,p,0);//479
glVertex3f(P,0,R);//4
glVertex3f(P,0,-R);//7
glVertex3f(R,P,0);//9

glNormal3f(l+p,l+p,l+p);//249
glVertex3f(0,R,P);//2
glVertex3f(P,0,R);//4
glVertex3f(R,P,0);//9

glNormal3f(-l-p ,-p-l,-l-p);//56 11
glVertex3f(-R,-P, 0);//5
glVertex3f(-P, 0,-R);//6
glVertex3f( 0,-R,-P);//11

glNormal3f(0,-2*p-l,-p);//58 11
glVertex3f( R,-P, 0);//8
glVertex3f(-R,-P, 0);//5
glVertex3f( 0,-R,-P);//11

glNormal3f(p+l,-p-l,-l-p);//78 11
glVertex3f(P, 0,-R);//7
glVertex3f(R,-P, 0);//8
glVertex3f(0,-R,-P);//11

glNormal3f(0,2*p+l,p);//29 12
glVertex3f( R, P,0);//9
glVertex3f(-R, P,0);//12
glVertex3f( 0, R,P);//2

glNormal3f(-p-l,l+p,l+p);//23 12
glVertex3f(-P,0,R);//3
glVertex3f( 0,R,P);//2
glVertex3f(-R,P,0);//12

glNormal3f(-2*p-l,p,0);//36 12
glVertex3f(-P,0,-R);//6
glVertex3f(-P,0, R);//3
glVertex3f(-R,P, 0);//12

glNormal3f(p,0,-2*p-l);//710 11
glVertex3f(0, R,-P);//10
glVertex3f(P, 0,-R);//7
glVertex3f(0,-R,-P);//11

glNormal3f(l+p,l+p,-p-l);//7910
glVertex3f(0,R,-P);//10
glVertex3f(R,P, 0);//9
glVertex3f(P,0,-R);//7

glNormal3f(-l-p,l+p,-l-p);//610 12
glVertex3f( 0,R,-P);//10
glVertex3f(-P,0,-R);//6
glVertex3f(-R,P, 0);//12

glNormal3f(0,l+2*p,-p);//910 12
glVertex3f( 0,R,-P);//10
glVertex3f(-R,P, 0);//12
glVertex3f( R,P, 0);//9

glNormal3f(-p, 0,-2*p-l);//610 11
glVertex3f( 0, R,-P);//10
glVertex3f( 0,-R,-P);//11
glVertex3f(-P, 0,-R);//6
glEnd();
}
double molekul::dieder(V3 a,V3 b, V3 c){

  static double erg;  
  double A[3],B[3],sig;
  sig=a.x*b.y*c.z - a.z*b.y*c.x +
    a.z*b.x*c.y - a.x*b.z*c.y +
    a.y*b.z*c.x - a.y*b.x*c.z;
  
  A[0]= a.y*b.z - a.z*b.y;
  A[1]=-a.x*b.z + a.z*b.x;
  A[2]= a.x*b.y - a.y*b.x;

  B[0]=-b.y*c.z + b.z*c.y;
  B[1]= b.x*c.z - b.z*c.x;
  B[2]=-b.x*c.y + b.y*c.x;

  erg=(A[0]*B[0]+A[1]*B[1]+A[2]*B[2])/(sqrt(A[0]*A[0]+A[1]*A[1]+A[2]*A[2])*sqrt(B[0]*B[0]+B[1]*B[1]+B[2]*B[2]));
  erg=acos(erg)/M_PI*180.0f;
 return (sig>0)?(erg):(-erg);
}
double molekul::winkel(V3 a,V3 b){

  static double erg;
  erg=(a.x*b.x+a.y*b.y+a.z*b.z)/(sqrt(a.x*a.x+a.y*a.y+a.z*a.z)*sqrt(b.x*b.x+b.y*b.y+b.z*b.z));
  erg=acos(erg)/M_PI*180.0;
 return(erg);
}
V3 molekul::kreuzX(double x1,double y1,double z1,double x2,double y2,double z2) {

  static V3 erg;
  erg.x=-y1*z2+z1*y2;
  erg.y=-z1*x2+x1*z2;
  erg.z=-x1*y2+y1*x2;
 return(erg);
}
void molekul::initDir(){
  fvl=1;
  firstHL=0;
  lastHL=0;
  bonds_made=0;
  knopf_made=0;
  adp=1;
  intern=0;
  proba=50;
  lod=3;

}//Init Dir
int molekul::Get_OZ(const QString S1){
QString s=S1.section(QRegExp("[^A-Za-z]"),0,0);
if (s=="Cv") return 5;
static QStringList PSE;
PSE<<"H"<<"He"<<"Li"<<"Be"<<"B"<<"C"<<"N"<<"O"<<"F"<<"Ne"<<"Na"<<"Mg"<<"Al"<<"Si"<<"P"<<"S"<<"Cl"<<"Ar"<<
			     "K"<<"Ca"<<"Sc"<<"Ti"<<"V"<<"Cr"<<"Mn"<<"Fe"<<"Co"<<"Ni"<<"Cu"<<"Zn"<<"Ga"<<"Ge"<<"As"<<"Se"<<"Br"<<"Kr"<<
                 "Rb"<<"Sr"<<"Y"<<"Zr"<<"Nb"<<"Mo"<<"Tc"<<"Ru"<<"Rh"<<"Pd"<<"Ag"<<"Cd"<<"In"<<"Sn"<<"Sb"<<"Te"<<"I"<<"Xe"<<
			     "Cs"<<"Ba"<< "La"<<"Ce"<<"Pr"<<"Nd"<<"Pm"<<"Sm"<<"Eu"<<"Gd"<<"Tb"<<"Dy"<<"Ho"<<"Er"<<"Tm"<<"Yb"<<"Lu"<<
			     "Hf"<<"Ta"<<"W"<<"Re"<<"Os"<<"Ir"<<"Pt"<<"Au"<<"Hg"<<"Tl"<<"Pb"<<"Bi"<<"Po"<<"At"<<"Rn"<<"Fr"<<"Ra"<<
			     "Ac"<<"Th"<<"Pa"<<"U"<<"Np"<<"Pu"<<"Am"<<"Cm"<<"Bk"<<"Cf"<<"Es"<<"Fm"<<"Md"<<"No"<<"Lr"<<"Ku"<<"Ha"<<"Rf"<<"Ns"<<"Hs"<<"Mt";
return PSE.indexOf(QRegExp(s));
}
int molekul::Get_OZ(const char * S1){//109 Elemente solltenReichen zumindest fuer XD ;-)
  char s1[3]="\0\0";
  strncpy(s1,S1,3);
  if (strlen(S1)>1) s1[1]=tolower(S1[1]);
  char PSE_Symbol[109][3] = {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar",
			     "K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr",
                 "Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe",
			     "Cs","Ba", "La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu",
			     "Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra",
			     "Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No","Lr","Ku","Ha","Rf","Ns","Hs","Mt"};

  for (int i=0;i<109;i++) if (!strcmp(PSE_Symbol[i],s1))return i;//Je hoeher die Ordnungzahl desto laenger dauerts Halt!! Dumm aber einfach.
 return -1;
}
#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau); a[k][l]=h+s*(g-h*tau);
//NumericalRecepies....
double * molekul::jacobi(double a[3][3], double d[3]) { 

  int j,iq,ip,i,n=3,nrot; 
  double tresh,theta,tau,t,sm,s,h,g,c; 
  double b[3],z[3],v[3][3];
  static double erg[4]={0.0,1.0,0.0,0.0};
  for (ip=1;ip<=n;ip++) {   
    for (iq=1;iq<=n;iq++) v[ip-1][iq-1]=0.0; 
    v[ip-1][ip-1]=1.0; 
  } 
  for (ip=1;ip<=n;ip++) {  
    b[ip-1]=d[ip-1]=a[ip-1][ip-1]; 
    z[ip-1]=0.0; 
  } 
  nrot=0; 
  for (i=1;i<=150;i++) {
    sm=0.0; 
    for (ip=1;ip<=n-1;ip++) { 
      for (iq=ip+1;iq<=n;iq++) 
	sm += fabs(a[ip-1][iq-1]);       
    } 



    if (float(sm) == 0.0) { 
      if ((v[0][0]+v[1][1]+v[2][2])!=3.0) {
      erg[0]=acos((v[0][0]+v[1][1]+v[2][2]-1.0)/2.0);
      erg[1]=(v[2][1]-v[1][2])/(2.0*sin(erg[0]));
      erg[2]=(v[0][2]-v[2][0])/(2.0*sin(erg[0]));
      erg[3]=(v[1][0]-v[0][1])/(2.0*sin(erg[0]));
      erg[0]*=180.0/M_PI;}
      else {erg[0]=0.0;erg[1]=1.0;erg[2]=0.0;erg[3]=0.0; }

/*  printf("=a======================================\n%8.5f %8.5f %8.5f \n%8.5f %8.5f %8.5f \n%8.5f %8.5f %8.5f \n%8.5f %8.5f %8.5f\n========================================\n",
		  d[0],d[1],d[2],v[0][0],v[0][1],v[0][2]
		  ,v[1][0],v[1][1],v[1][2]
		  ,v[2][0],v[2][1],v[2][2]

		  );*/
     return (double*) erg;
    }
    if (i < 4) tresh=0.0001;  
    else tresh=0.000001;  
    for (ip=1;ip<=n-1;ip++) { 
      for (iq=ip+1;iq<=n;iq++) { 

	g=100.0*fabs(a[ip-1][iq-1]);  
	if ((i > 4) && ((fabs(d[ip-1])+g) == fabs(d[ip-1])) && ((fabs(d[iq-1])+g) == fabs(d[iq-1]))) {a[ip-1][iq-1]=0.0;}
	else if (fabs(a[ip-1][iq-1]) >= tresh) { 
	  h=d[iq-1]-d[ip-1]; 
	  if ((fabs(h)+g) == fabs(h)) {t=(a[ip-1][iq-1])/h; } 
	  else { theta=0.5*h/(a[ip-1][iq-1]);  
	  t=1.0/(fabs(theta)+sqrt(1.0+theta*theta)); 
	  if (theta < 0.0) {t = -1.0*t;}
	  } 
	  c=1.0/sqrt(1+t*t); 
	  s=t*c; 
	  tau=s/(1.0+c); 
	  h=t*a[ip-1][iq-1];
	  z[ip-1] -= h;
	  z[iq-1] += h;
	  d[ip-1] -= h;
	  d[iq-1] += h;
	  a[ip-1][iq-1]=0.0;
	  for (j=1;j<=ip-1;j++) { 
           ROTATE(a,j-1,ip-1,j-1,iq-1)

	      } 
	  for (j=ip+1;j<=iq-1;j++) { 
           ROTATE(a,ip-1,j-1,j-1,iq-1)

	      } 
	  for (j=iq+1;j<=n;j++) {  
           ROTATE(a,ip-1,j-1,iq-1,j-1)

	      } 
	  for (j=1;j<=n;j++) { 
           ROTATE(v,j-1,ip-1,j-1,iq-1)
	      } 
	  ++(nrot);	  

	}
      } 
    } 
    for (ip=1;ip<=n;ip++) { 
      b[ip-1] += z[ip-1];
      d[ip-1] =b[ip-1];
      z[ip-1] =0.0;
    } 
  } 
  erg[0]=acos((v[0][0]+v[1][1]+v[2][2]-1.0)/2.0);
  if (erg[0]==0) {
    erg[1]=1.0;
    erg[2]=0.0;
    erg[3]=0.0;
  }else{
  erg[1]=(v[2][1]-v[1][2])/(2.0*sin(erg[0]));
  erg[2]=(v[0][2]-v[2][0])/(2.0*sin(erg[0]));
  erg[3]=(v[1][0]-v[0][1])/(2.0*sin(erg[0]));
  erg[0]*=180.0/M_PI;
  }
/*  printf("========================================\n%8.5f %8.5f %8.5f \n%8.5f %8.5f %8.5f \n%8.5f %8.5f %8.5f \n%8.5f %8.5f %8.5f\n========================================\n",
		  d[0],d[1],d[2],v[0][0],v[0][1],v[0][2]
		  ,v[1][0],v[1][1],v[1][2]
		  ,v[2][0],v[2][1],v[2][2]

		  );*/




 return (double*)erg;
}

Matrix molekul::jacobi(Matrix A, V3 &D) {
    double a[3][3]={{A.m11,A.m12,A.m13},
                    {A.m21,A.m22,A.m23},
                    {A.m31,A.m32,A.m33}};
    double d[3]={D.x,D.y,D.z};
  int j,iq,ip,i,n=3,nrot;
  double tresh=0,theta,tau,t,sm,s,h,g,c;
  double b[3],z[3],v[3][3];
  for (ip=1;ip<=n;ip++) {
    for (iq=1;iq<=n;iq++) v[ip-1][iq-1]=0.0;
    v[ip-1][ip-1]=1.0;
  }
  for (ip=1;ip<=n;ip++) {
    b[ip-1]=d[ip-1]=a[ip-1][ip-1];
    z[ip-1]=0.0;
  }
  nrot=0;
  for (i=1;i<=150;i++) {
    sm=0.0;
    for (ip=1;ip<=n-1;ip++) {
      for (iq=ip+1;iq<=n;iq++)
        sm += fabs(a[ip-1][iq-1]);
    }



    if (float(sm) <tresh) {
        D=V3(d[0],d[1],d[2]);
      return Matrix(v[0][0],v[0][1],v[0][2],
                    v[1][0],v[1][1],v[1][2],
                    v[2][0],v[2][1],v[2][2]);
    }
    if (i < 4) tresh=0.0001;
    else tresh=0.000001;
    for (ip=1;ip<=n-1;ip++) {
      for (iq=ip+1;iq<=n;iq++) {

        g=100.0*fabs(a[ip-1][iq-1]);
        if ((i > 4) && ((fabs(d[ip-1])+g) == fabs(d[ip-1])) && ((fabs(d[iq-1])+g) == fabs(d[iq-1]))) {a[ip-1][iq-1]=0.0;}
        else if (fabs(a[ip-1][iq-1]) >= tresh) {
          h=d[iq-1]-d[ip-1];
          if ((fabs(h)+g) == fabs(h)) {t=(a[ip-1][iq-1])/h; }
          else { theta=0.5*h/(a[ip-1][iq-1]);
          t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
          if (theta < 0.0) {t = -1.0*t;}
          }
          c=1.0/sqrt(1+t*t);
          s=t*c;
          tau=s/(1.0+c);
          h=t*a[ip-1][iq-1];
          z[ip-1] -= h;
          z[iq-1] += h;
          d[ip-1] -= h;
          d[iq-1] += h;
          a[ip-1][iq-1]=0.0;
          for (j=1;j<=ip-1;j++) {
           ROTATE(a,j-1,ip-1,j-1,iq-1)

              }
          for (j=ip+1;j<=iq-1;j++) {
           ROTATE(a,ip-1,j-1,j-1,iq-1)

              }
          for (j=iq+1;j<=n;j++) {
           ROTATE(a,ip-1,j-1,iq-1,j-1)

              }
          for (j=1;j<=n;j++) {
           ROTATE(v,j-1,ip-1,j-1,iq-1)
              }
          ++(nrot);

        }
      }
    }
    for (ip=1;ip<=n;ip++) {
      b[ip-1] += z[ip-1];
      d[ip-1] =b[ip-1];
      z[ip-1] =0.0;
    }
  }

  D=V3(d[0],d[1],d[2]);
  return Matrix(v[0][0],v[0][1],v[0][2],
                v[1][0],v[1][1],v[1][2],
                v[2][0],v[2][1],v[2][2]);
}

void molekul::atoms(QList<INP> xdinp,const int proba){//ADP Schwingungsellipsoide
  GLfloat black[4]={0.0, 0.0, 0.0, 1.0};
  GLfloat white[4]={0.2, 0.2, 0.2, 0.0};
  glBindTexture(GL_TEXTURE_2D, hbtex2);
  glEnable(GL_TEXTURE_2D);
  double EV[3], *erg=NULL;
  double a[3][3];
  int mylod =lod;
  for (int j=0;j<xdinp.size();j++){//for atmax
//    qDebug()<<__LINE__<<xdinp[j].OrdZahl;
      int myStyle=(xdinp[j].OrdZahl>-1)?aStyle[xdinp[j].OrdZahl]:0;
    int myAdp=(myStyle&ATOM_STYLE_NOADP)?0:adp;
    if (!dratom){
      glPushMatrix () ;
    glTranslated(xdinp[j].kart.x,xdinp[j].kart.y,xdinp[j].kart.z) ;
    //double rad=(atom.at(i).an==-2)?0.5:qPeakRad;
    double rad=(xdinp[j].OrdZahl>-1)?arad[xdinp[j].OrdZahl]:0.15;

      if (tubifiedAtoms) rad = bondStrength;

      if ((myAdp)&&(xdinp[j].OrdZahl>-1)) {//adp
	a[0][0]=xdinp[j].u.m11;
	a[0][1]=xdinp[j].u.m12;
	a[0][2]=xdinp[j].u.m13;
	a[1][0]=xdinp[j].u.m21;
	a[1][1]=xdinp[j].u.m22;
	a[1][2]=xdinp[j].u.m23;
	a[2][0]=xdinp[j].u.m31;
	a[2][1]=xdinp[j].u.m32;
	a[2][2]=xdinp[j].u.m33;

	erg=jacobi(a,EV);

	EV[0]=(EV[0]>0.001)?EV[0]:0.001;
	EV[1]=(EV[1]>0.001)?EV[1]:0.001;
	EV[2]=(EV[2]>0.001)?EV[2]:0.001;
	glRotated(Ato4d(erg));
	switch (proba ) {
	case 10 :{ glScaled(0.76*sqrt(EV[0]),0.76*sqrt(EV[1]),0.76*sqrt(EV[2]));break;}   //Hauptachsen der Eliipsoide 10% Wahrscheinlichkeit
	case 30 :{ glScaled(1.19*sqrt(EV[0]),1.19*sqrt(EV[1]),1.19*sqrt(EV[2]));break;}   //Hauptachsen der Eliipsoide 30% Wahrscheinlichkeit
	case 50 :{ glScaled(1.54*sqrt(EV[0]),1.54*sqrt(EV[1]),1.54*sqrt(EV[2]));break;}   //Hauptachsen der Eliipsoide 50% Wahrscheinlichkeit
	case 70 :{ glScaled(1.91*sqrt(EV[0]),1.91*sqrt(EV[1]),1.91*sqrt(EV[2]));break;}   //Hauptachsen der Eliipsoide 70% Wahrscheinlichkeit
	case 90 :{ glScaled(2.50*sqrt(EV[0]),2.50*sqrt(EV[1]),2.50*sqrt(EV[2]));break;}   //Hauptachsen der Eliipsoide 90% Wahrscheinlichkeit
	default: ;
	}
      }
      else if (xdinp[j].OrdZahl>-1){
	if (rad) glScaled(rad,rad,rad);
	else glScaled(0.15,0.15,0.15);
      }else {
	if (xdinp[j].OrdZahl==-2){
	  /*
	printf("New Mexico\n");
	double imat[16];
	glLoadIdentity();
	glGetDoublev( GL_MODELVIEW_MATRIX, (double*)imat );
	double Erg[4];
	Matrix m2,modM(imat[0],imat[1],imat[2],imat[4],imat[5],imat[6],imat[8],imat[9],imat[10]);
        m2=xdinp[j].uf*modM*transponse(xdinp[j].uf);
	printf (" %8.5f %8.5f %8.5f      %8.5f %8.5f %8.5f \n %8.5f %8.5f %8.5f      %8.5f %8.5f %8.5f \n %8.5f %8.5f %8.5f      %8.5f %8.5f %8.5f \n",
			xdinp[j].uf.m11,
			xdinp[j].uf.m12,
			xdinp[j].uf.m13,imat[0],imat[1],imat[2],
			xdinp[j].uf.m21,
			xdinp[j].uf.m22,
			xdinp[j].uf.m23,imat[4],imat[5],imat[6],
			xdinp[j].uf.m31,
			xdinp[j].uf.m32,
			xdinp[j].uf.m33,imat[8],imat[9],imat[10]);   // * /
	  imat[0]= m2.m11;
	  imat[1]= m2.m21;
	  imat[2]= m2.m31;
	  imat[4]= m2.m12;
	  imat[5]= m2.m22;
	  imat[6]= m2.m32;
	  imat[8]= m2.m13;
	  imat[9]= m2.m23;
	  imat[10]=m2.m33;
          glLoadMatrixd(imat);
	  / *
	  glBegin(GL_LINES);
	  glColor3f(0.5,0.5,0);
	  glVertex3d(xdinp[j].kart.x,xdinp[j].kart.y,xdinp[j].kart.z);
	  glVertex3d(xdinp[j].uf.m11,xdinp[j].uf.m12,xdinp[j].uf.m13);
	  glColor3f(0.0,0.5,0.5);
	  glVertex3d(xdinp[j].kart.x,xdinp[j].kart.y,xdinp[j].kart.z);
	  glVertex3d(xdinp[j].uf.m21,xdinp[j].uf.m22,xdinp[j].uf.m23);
	  glColor3f(0.5,0.0,0.5);
	  glVertex3d(xdinp[j].kart.x,xdinp[j].kart.y,xdinp[j].kart.z);
	  glVertex3d(xdinp[j].uf.m31,xdinp[j].uf.m32,xdinp[j].uf.m33);
	  glEnd();
*/
	  /*

	  Erg[0]=acos((xdinp[j].uf.m11+xdinp[j].uf.m22+xdinp[j].uf.m33-1.0)/2.0);
	  if (Erg[0]==0) {
	    Erg[1]=0.5773502691896257645;
	    Erg[2]=0.5773502691896257645;
	    Erg[3]=0.5773502691896257645;
	  }else{
	    Erg[1]=(xdinp[j].uf.m23-xdinp[j].uf.m32)/(2.0*sin(Erg[0]));
	    Erg[2]=(xdinp[j].uf.m31-xdinp[j].uf.m12)/(2.0*sin(Erg[0]));
	    Erg[3]=(xdinp[j].uf.m12-xdinp[j].uf.m21)/(2.0*sin(Erg[0]));
	    Erg[0]*=180.0/M_PI;
	  } */
	  glScaled(0.20,0.20,0.20);

// */
    }//else glScaled(1.0,1.0,1.0);
      }

      GLUquadricObj *q = gluNewQuadric();
      gluQuadricNormals(q, GL_SMOOTH);   

      if (xdinp[j].OrdZahl>-1) glColor4fv(Acol[xdinp[j].OrdZahl]); 
      if (xdinp[j].part) glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, white );
      else glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, black );


      if ((rad>0)&&(xdinp[j].OrdZahl>-1)) {
    if (myAdp&&intern) ellipse(aStyle[xdinp[j].OrdZahl]);else
      if ((!myAdp)||((aStyle[xdinp[j].OrdZahl]&ATOM_STYLE_SPHERE)&&(!(aStyle[xdinp[j].OrdZahl]&ATOM_STYLE_SOLID))))gluSphere(q,0.96,6*mylod,6*mylod);//Atom als Kugel zeichnen
	if (xdinp[j].atomname[0]=='D') printf("%s %f \n",xdinp[j].atomname,rad);
      }
      else {
	if (xdinp[j].atomname[0]=='D') glColor3f(0.5,0.0,0.5);
	if (xdinp[j].atomname[0]=='H') glColor3f(0.5,0.0,0.0);
	if (xdinp[j].atomname[0]=='P') glColor3f(0.0,0.0,0.6);
	if (xdinp[j].atomname[0]=='C') glColor3f(0.0,0.3,0.1);
	if (xdinp[j].atomname[0]=='O') glColor3f(0.0,0.3,0.1);
	if (xdinp[j].atomname[0]=='Q') {
	 Farbverlauf(xdinp[j].peakHeight,pmin,pmax);
//    qDebug()<<__LINE__<<xdinp[j].OrdZahl;
	 }
        ikosa(qPeakRad);
      }

      glPopMatrix(); 
  }else {
    glPushMatrix () ;
    glTranslated(xdinp[j].kart.x,xdinp[j].kart.y,xdinp[j].kart.z) ;
    GLUquadricObj *q = gluNewQuadric();
    gluQuadricNormals(q, GL_SMOOTH);   
    glDisable(GL_DEPTH_TEST);
    glColor3d(1.0, 1.0,0.0);
    gluQuadricDrawStyle(q,GLU_LINE);
    gluSphere(q,0.3,5,5);
    glEnable(GL_DEPTH_TEST);
      glPopMatrix(); 

  }
  }
 glDisable(GL_TEXTURE_2D);
}

QString molekul::pse(int oz){
  static QStringList PSE;

  PSE<<"H"<<"He"<<"Li"<<"Be"<<"B"<<"C"<<"N"<<"O"<<"F"<<"Ne"<<"Na"<<"Mg"<<"Al"<<"Si"<<"P"<<"S"<<"Cl"<<"Ar"<<
	  "K"<<"Ca"<<"Sc"<<"Ti"<<"V"<<"Cr"<<"Mn"<<"Fe"<<"Co"<<"Ni"<<"Cu"<<"Zn"<<"Ga"<<"Ge"<<"As"<<"Se"<<"Br"<<"Kr"<<
          "Rb"<<"Sr"<<"Y"<<"Zr"<<"Nb"<<"Mo"<<"Tc"<<"Ru"<<"Rh"<<"Pd"<<"Ag"<<"Cd"<<"In"<<"Sn"<<"Sb"<<"Te"<<"I"<<"Xe"<<
	  "Cs"<<"Ba"<< "La"<<"Ce"<<"Pr"<<"Nd"<<"Pm"<<"Sm"<<"Eu"<<"Gd"<<"Tb"<<"Dy"<<"Ho"<<"Er"<<"Tm"<<"Yb"<<"Lu"<<
	  "Hf"<<"Ta"<<"W"<<"Re"<<"Os"<<"Ir"<<"Pt"<<"Au"<<"Hg"<<"Tl"<<"Pb"<<"Bi"<<"Po"<<"At"<<"Rn"<<"Fr"<<"Ra"<<
	  "Ac"<<"Th"<<"Pa"<<"U"<<"Np"<<"Pu"<<"Am"<<"Cm"<<"Bk"<<"Cf"<<"Es"<<"Fm"<<"Md"<<"No"<<"Lr"<<"Ku"<<"Ha"<<"Rf"<<"Ns"<<"Hs"<<"Mt";
  if ((oz>-1)&&(oz<PSE.size())) return PSE.at(oz);
  return "";

}

void molekul::axes(QList<INP> xdinp) {
  glDisable(GL_LIGHTING);
  glLineWidth(3);
  for (int j=0;j<xdinp.size();j++) {
    if (xdinp[j].OrdZahl<0) continue;
    if (QString(xdinp[j].atomname).contains(QRegExp("_\\d+$",Qt::CaseInsensitive,QRegExp::RegExp))) continue;
    if (QString(xdinp[j].atomname).contains(QRegExp("'$",Qt::CaseInsensitive,QRegExp::RegExp))) continue;
/*    printf("%f %f %f\n%f %f %f\n%f %f %f\n\n",
		    xdinp[j].ax1.x, xdinp[j].ax1.y, xdinp[j].ax1.z,
		    xdinp[j].ax2.x, xdinp[j].ax2.y, xdinp[j].ax2.z,
		    xdinp[j].ax3.x, xdinp[j].ax3.y, xdinp[j].ax3.z);*/
    if ((fabs(Norm(xdinp[j].ax1)-1.0)>0.01)|| 
	(fabs(Norm(xdinp[j].ax2)-1.0)>0.01)|| 
	(fabs(Norm(xdinp[j].ax3)-1.0)>0.01))continue;
    glPushMatrix();
    glBegin(GL_LINES);
    if (xdinp[j].icor1==2) glColor4f(0.0f,1.0f,0.0f,0.5f);
    else if (xdinp[j].icor1==3) glColor4f(0.0f,0.0f,1.0f,1.0f);
    else glColor4f(1.0f,0.0f,0.0f,0.5f);
    glVertex3d(xdinp[j].kart.x,xdinp[j].kart.y,xdinp[j].kart.z);
    glVertex3d(xdinp[j].kart.x+(xdinp[j].ax1.x/2.0),
	       xdinp[j].kart.y+(xdinp[j].ax1.y/2.0),
	       xdinp[j].kart.z+(xdinp[j].ax1.z/2.0));
    
    if (xdinp[j].icor2==2) glColor4f(0.0f,1.0f,0.0f,0.5f);
    else if (xdinp[j].icor2==3) glColor4f(0.0f,0.0f,1.0f,0.5f);
    else glColor4f(1.0f,0.0f,0.0f,0.5f);
    glVertex3d(xdinp[j].kart.x,xdinp[j].kart.y,xdinp[j].kart.z);
    glVertex3d(xdinp[j].kart.x+(xdinp[j].ax2.x/2.0),
	       xdinp[j].kart.y+(xdinp[j].ax2.y/2.0),
	       xdinp[j].kart.z+(xdinp[j].ax2.z/2.0));
    
    if ((6-xdinp[j].icor2-xdinp[j].icor1)==2) glColor4f(0.0f,1.0f,0.0f,0.5f);
    else if ((6-xdinp[j].icor2-xdinp[j].icor1)==3) glColor4f(0.0f,0.0f,1.0f,0.5f);
    else glColor4f(1.0f,0.0f,0.0f,0.5f);
    glVertex3d(xdinp[j].kart.x,xdinp[j].kart.y,xdinp[j].kart.z);
    glVertex3d(xdinp[j].kart.x+(xdinp[j].ax3.x/2.0),
	       xdinp[j].kart.y+(xdinp[j].ax3.y/2.0),
	       xdinp[j].kart.z+(xdinp[j].ax3.z/2.0));
    glEnd();
    glPopMatrix();
  }
  glLineWidth(1);
  glEnable(GL_LIGHTING);
}

/*
void molekul::UnitZell2(int zz)
{

      glDisable(GL_LIGHTING);  
      glLineWidth(2);
      glPushMatrix();

      glBegin(GL_LINES);
      glColor4f(0.0f,0.0f,1.0f,1.0f);
      glVertex3d(uz0k[0],uz0k[1],uz0k[2]);
      glVertex3d(uz1k[0],uz1k[1],uz1k[2]);
      
      glColor4f(1.0f,0.0f,0.0f,1.0f);
      glVertex3d(uz0k[0],uz0k[1],uz0k[2]);
      glVertex3d(uz2k[0],uz2k[1],uz2k[2]);

      glColor4f(0.0f,1.0f,0.0f,1.0f);
      glVertex3d(uz0k[0],uz0k[1],uz0k[2]);
      glVertex3d(uz3k[0],uz3k[1],uz3k[2]);

      glColor4f(0.6f,0.7f,0.5f,1.0f);
      glVertex3d(uz1k[0],uz1k[1],uz1k[2]);
      glVertex3d(uz4k[0],uz4k[1],uz4k[2]);

      glVertex3d(uz1k[0],uz1k[1],uz1k[2]);     
      glVertex3d(uz5k[0],uz5k[1],uz5k[2]);

      glVertex3d(uz2k[0],uz2k[1],uz2k[2]);
      glVertex3d(uz4k[0],uz4k[1],uz4k[2]);

      glVertex3d(uz2k[0],uz2k[1],uz2k[2]);
      glVertex3d(uz6k[0],uz6k[1],uz6k[2]);

      glVertex3d(uz3k[0],uz3k[1],uz3k[2]);
      glVertex3d(uz5k[0],uz5k[1],uz5k[2]);

      glVertex3d(uz3k[0],uz3k[1],uz3k[2]);
      glVertex3d(uz6k[0],uz6k[1],uz6k[2]);

      glVertex3d(uz7k[0],uz7k[1],uz7k[2]);
      glVertex3d(uz5k[0],uz5k[1],uz5k[2]);
      
      glVertex3d(uz7k[0],uz7k[1],uz7k[2]);
      glVertex3d(uz4k[0],uz4k[1],uz4k[2]);
      
      glVertex3d(uz7k[0],uz7k[1],uz7k[2]);
      glVertex3d(uz6k[0],uz6k[1],uz6k[2]);
      
      glEnd();
      glEnable(GL_LIGHTING);

      glutSolidSphere(1.0,24,12);
      glDisable( GL_DEPTH_TEST );
      glColor4f(1.0f,1.0f,1.0f,1.0f); 
      glRasterPos3f(uz1k[0],uz1k[1],uz1k[2]);      
      char bez[30];
      sprintf(bez,"X%d",zz);
      if (!notext) glPrint(bez);
      glRasterPos3f(uz2k[0],uz2k[1],uz2k[2]);      
      sprintf(bez,"Y%d",zz);
      if (!notext) glPrint(bez);
      glRasterPos3f(uz3k[0],uz3k[1],uz3k[2]);      
      sprintf(bez,"Z%d",zz);
      if (!notext) glPrint(bez);
      glEnable( GL_DEPTH_TEST ); 
      glPopMatrix();     
  glLineWidth(1);

}
*/

void molekul::UnitZell(void) {
  glDisable(GL_LIGHTING);
  glLineWidth(2);
      glPushMatrix();
      glBegin(GL_LINES);
      glColor4f(1.0f,0.0f,0.0f,1.0);

      glVertex3d(uz0k.x,uz0k.y,uz0k.z);
      glVertex3d(uz1k.x,uz1k.y,uz1k.z);
      
      glColor4f(0.0f,1.0f,0.0f,1.0);
      glVertex3d(uz0k.x,uz0k.y,uz0k.z);
      glVertex3d(uz2k.x,uz2k.y,uz2k.z);

      glColor4f(0.0f,0.0f,1.0f,1.0);
      glVertex3d(uz0k.x,uz0k.y,uz0k.z);
      glVertex3d(uz3k.x,uz3k.y,uz3k.z);

      glColor4f(0.6f,0.6f,0.8f,1.0);
      glVertex3d(uz1k.x,uz1k.y,uz1k.z);
      glVertex3d(uz4k.x,uz4k.y,uz4k.z);

      glVertex3d(uz1k.x,uz1k.y,uz1k.z);     
      glVertex3d(uz5k.x,uz5k.y,uz5k.z);

      glVertex3d(uz2k.x,uz2k.y,uz2k.z);
      glVertex3d(uz4k.x,uz4k.y,uz4k.z);

      glVertex3d(uz2k.x,uz2k.y,uz2k.z);
      glVertex3d(uz6k.x,uz6k.y,uz6k.z);

      glVertex3d(uz3k.x,uz3k.y,uz3k.z);
      glVertex3d(uz5k.x,uz5k.y,uz5k.z);

      glVertex3d(uz3k.x,uz3k.y,uz3k.z);
      glVertex3d(uz6k.x,uz6k.y,uz6k.z);

      glVertex3d(uz7k.x,uz7k.y,uz7k.z);
      glVertex3d(uz5k.x,uz5k.y,uz5k.z);
      
      glVertex3d(uz7k.x,uz7k.y,uz7k.z);
      glVertex3d(uz4k.x,uz4k.y,uz4k.z);
      
      glVertex3d(uz7k.x,uz7k.y,uz7k.z);
      glVertex3d(uz6k.x,uz6k.y,uz6k.z);
      
      glEnd();
      
//      glDisable( GL_DEPTH_TEST );
//      glColor4f(1.0f,1.0f,1.0f,1.0f); 
      /*
      cubeGL->renderText( uz1k.x,uz1k.y,uz1k.z, "X",myFont);
      cubeGL->renderText( uz2k.x,uz2k.y,uz2k.z, "Y",myFont);
      cubeGL->renderText( uz3k.x,uz3k.y,uz3k.z, "Z",myFont);
				      */
//      glEnable( GL_DEPTH_TEST ); 
      glPopMatrix();     
  glLineWidth(1);
  glEnable(GL_LIGHTING);
}

void molekul::make_bonds(QList<INP> xdinp){
  if (xdinp.isEmpty()) return;
  double gg,soll_abst;
  bcnt=0;
  bd=NULL;
  int msiz=(xdinp.size()*10);
  if (NULL==(bd=(bindi*)malloc(sizeof(bindi)*msiz)))return;
  for (int i=0;i<xdinp.size();i++) {
      //printf("i=%d sg=%d part=%d\n",i,xdinp[i].sg,xdinp[i].part);
    for (int j=0;j<xdinp.size();j++) {
      if (i==j) continue;



      if((xdinp[i].OrdZahl<0)||(xdinp[j].OrdZahl<0)) continue;
      if (((xdinp[i].part<0)||(xdinp[j].part<0))&&((xdinp[i].sg!=xdinp[j].sg)||((xdinp[i].part*xdinp[j].part)&&(xdinp[i].part!=xdinp[j].part)))) continue; //part negative
      if ((xdinp[i].part>0)&&(xdinp[j].part>0)&&(xdinp[i].part!=xdinp[j].part)) continue; //different part
      if ((xdinp[i].OrdZahl<83)&&(xdinp[j].OrdZahl<83)&&(xdinp[i].OrdZahl>=0)&&(xdinp[j].OrdZahl>=0)){
	soll_abst=((Kovalenz_Radien[xdinp[i].OrdZahl]+
		    Kovalenz_Radien[xdinp[j].OrdZahl])
		   -(0.08*fabs((double)ElNeg[xdinp[i].OrdZahl]
			       -ElNeg[xdinp[j].OrdZahl])))*1.1;
	gg=100.0*sqrt( Distance(xdinp[i].kart,xdinp[j].kart));
	if (gg<soll_abst) {
	  bd[bcnt].a=i;	
	  bd[bcnt].e=j;
/*	    printf("%s[%d](%d,%d)-%s[%d](%d,%d) %d\n",
			    xdinp[bd[bcnt].a].atomname,
			    bd[bcnt].a,
			    xdinp[bd[bcnt].a].sg,
			    xdinp[bd[bcnt].a].part,
			    xdinp[bd[bcnt].e].atomname,
			    bd[bcnt].e,
			    xdinp[bd[bcnt].e].sg,
			    xdinp[bd[bcnt].e].part,
			    bcnt);*/
	  bcnt++;
          if (bcnt>=msiz){
          msiz+=10;
          bd=(bindi*)realloc(bd,sizeof(bindi)*msiz);  
          }
	}//bindung
      }//Ordnungzahl-ok
    }//j
  }//i
  bd=(bindi*)realloc(bd,sizeof(bindi)*bcnt);  
  bonds_made=1;
}
void molekul::make_bonds(QList<Modulat> xdinp,double t){
  if (xdinp.isEmpty()) return;
//  printf("make_bonds %g\n",t);
  int msiz=(xdinp.size()*10);
  double gg,soll_abst;
  bcnt=0;
  if (bd!=NULL) free(bd);
  bd=NULL;
  if (NULL==(bd=(bindi*)malloc(sizeof(bindi)*msiz)))return;
  for (int i=0;i<xdinp.size();i++) {
      //printf("i=%d sg=%d part=%d\n",i,xdinp[i].sg,xdinp[i].part);
    for (int j=0;j<xdinp.size();j++) {
      if (i==j) continue;
      if((xdinp[i].OrdZahl<0)||(xdinp[j].OrdZahl<0)) continue;
      if ((bondsBetweenSGs->isChecked())&&(xdinp[i].sg!=xdinp[j].sg))continue;
//      if (((xdinp[i].part<0)||(xdinp[j].part<0))&&((xdinp[i].sg!=xdinp[j].sg)||((xdinp[i].part*xdinp[j].part)&&(xdinp[i].part!=xdinp[j].part)))) continue; //part negative
//      if ((xdinp[i].part>0)&&(xdinp[j].part>0)&&(xdinp[i].part!=xdinp[j].part)) continue; //different part
      if ((xdinp[i].OrdZahl<83)&&(xdinp[j].OrdZahl<83)&&(xdinp[i].OrdZahl>=0)&&(xdinp[j].OrdZahl>=0)){
	soll_abst=((Kovalenz_Radien[xdinp[i].OrdZahl]+
		    Kovalenz_Radien[xdinp[j].OrdZahl])
		   -(0.08*fabs((double)ElNeg[xdinp[i].OrdZahl]
                   -ElNeg[xdinp[j].OrdZahl])))*1.25;
	gg=100.0*sqrt( Distance(xdinp[i].kart(t),xdinp[j].kart(t)));
	if (gg<soll_abst) {
	  bd[bcnt].a=i;	
	  bd[bcnt].e=j;
/*	    printf("%s[%d](%d,%d)-%s[%d](%d,%d) %d\n",
			    xdinp[bd[bcnt].a].atomname,
			    bd[bcnt].a,
			    xdinp[bd[bcnt].a].sg,
			    xdinp[bd[bcnt].a].part,
			    xdinp[bd[bcnt].e].atomname,
			    bd[bcnt].e,
			    xdinp[bd[bcnt].e].sg,
			    xdinp[bd[bcnt].e].part,
			    bcnt);*/
	  bcnt++;
          if (bcnt>=msiz){
          msiz+=10;
          bd=(bindi*)realloc(bd,sizeof(bindi)*msiz);  
          }
	}//bindung
      }//Ordnungzahl-ok
    }//j
  }//i
  bd=(bindi*)realloc(bd,sizeof(bindi)*(bcnt+10));
  bonds_made=1;
  mbonds_last_t=t;
}
/*void molekul::cyclsort(PolyEder & p){
    int mini=qMin(p.ai,p.bi);
    int dum1,dum2;
    mini=qMin(mini,p.ci);
    if (mini==p.ai){
 //richtig rum
    }
    else if(mini==p.bi){
        dum1=p.ai;
        p.ai=p.bi;
        p.bi=p.ci;
        p.ci=dum1;
    }
    else{
        dum1=p.bi;
        dum2=p.ai;
        p.ai=p.ci;
        p.bi=dum2;
        p.ci=dum1;
    }
}// */
int molekul::findPoly(int zi, PolyEder p,QList<INP> xd){
    int answer=-1;
    //GLfloat d;
    for (int i=0; i<polyeders.size(); i++){
   //     if (zi==polyeders.at(i).zi) printf("%d %g\n",zi,fabsf(p.norm*polyeders.at(i).norm));
        if ((zi==polyeders.at(i).zi)&&(0.7f<fabsf(p.norm*polyeders.at(i).norm))){
            if (0.1<(polyeders.at(i).mid-xd[zi].kart)*(p.mid-xd[zi].kart)) {
                //printf("polyeders.at(i).mid %g %g %g polyeders.at(i).norm %g %g %g\n",
               //        polyeders.at(i).mid.x,polyeders.at(i).mid.y,polyeders.at(i).mid.z,
                //       polyeders.at(i).norm.x,polyeders.at(i).norm.y,polyeders.at(i).norm.z
                //       );
                return i;
            }
        }
    }
    return answer;
}

void molekul::make_polyeder(QList<INP> xd){
    printf("make_polyeder\n");
    if (!knopf_made) make_knopf(xd);
    V3 zent;
    PolyEder polyvecs;
    polyeders.clear();
    int index;
    double vol,r,soll_abst;//,mr;
    for (int i=0; i<xd.size(); i++){
        if (Knopf[i].lz<4)continue;
        zent=xd[i].kart;
        //if (ElNeg[xd[i].OrdZahl]>240) continue;
        soll_abst=((Kovalenz_Radien[xd[i].OrdZahl]+Kovalenz_Radien[xd[Knopf[i].lig[0]].OrdZahl])
               -(0.08*fabs((double)ElNeg[xd[i].OrdZahl]-ElNeg[xd[Knopf[i].lig[0]].OrdZahl])))*1.1;
        soll_abst*=0.01;
        //soll_abst*=soll_abst;
        for (int j=0; j<Knopf[i].lz;j++){
            for (int k=j+1;k<Knopf[i].lz;k++){
                for (int l=k+1;l<Knopf[i].lz;l++){
                    vol=((xd[Knopf[i].lig[j]].kart-zent)%(xd[Knopf[i].lig[k]].kart-zent))*(xd[Knopf[i].lig[l]].kart-zent);

                    if (fabs(vol)<0.2) continue;
                    r=sqrt(Distance(xd[Knopf[i].lig[j]].kart,xd[Knopf[i].lig[k]].kart));//soll_abst;
                    r+=sqrt(Distance(xd[Knopf[i].lig[l]].kart,xd[Knopf[i].lig[k]].kart));//soll_abst;
                    r+=sqrt(Distance(xd[Knopf[i].lig[j]].kart,xd[Knopf[i].lig[l]].kart));//soll_abst;
                    polyvecs.edge.clear();
                    polyvecs.norm=Normalize((xd[Knopf[i].lig[j]].kart-xd[Knopf[i].lig[k]].kart)%(xd[Knopf[i].lig[l]].kart-xd[Knopf[i].lig[k]].kart));
                    polyvecs.mid=xd[Knopf[i].lig[j]].kart+xd[Knopf[i].lig[k]].kart+xd[Knopf[i].lig[l]].kart;
                    polyvecs.mid*=1.0/3.0;
                    polyvecs.zi=i;
                    index=findPoly(i,polyvecs,xd);
                    if (index!=-1){
                       // printf("foundpoly [%d]\n",index);
                        polyvecs=polyeders[index];
                       // printf("foundpoly %d %d\n",i,polyvecs.edge.size());
                        if (!polyvecs.edge.contains(Knopf[i].lig[j])) polyvecs.edge.append(Knopf[i].lig[j]);
                        if (!polyvecs.edge.contains(Knopf[i].lig[k])) polyvecs.edge.append(Knopf[i].lig[k]);
                        if (!polyvecs.edge.contains(Knopf[i].lig[l])) polyvecs.edge.append(Knopf[i].lig[l]);
                        polyvecs.mid=V3(0,0,0);
                        for (int m=0;m<polyvecs.edge.size();m++){
                            polyvecs.mid+=xd[polyvecs.edge.at(m)].kart;
                    //      printf("%4s ",xd[polyvecs.edge.at(m)].atomname);

                        }
                      //  printf("\n");
                        if (polyvecs.edge.size())  polyvecs.mid*=1.0/polyvecs.edge.size();
                        polyvecs.norm=Normalize(polyvecs.mid-zent);
                        polyvecs.umfang=r;
                        polyeders[index]=polyvecs;
                    }else {
                        polyvecs.mid=V3(0,0,0);
                        polyvecs.edge.append(Knopf[i].lig[j]);
                        polyvecs.edge.append(Knopf[i].lig[k]);
                        polyvecs.edge.append(Knopf[i].lig[l]);
                        for (int m=0;m<polyvecs.edge.size();m++)
                            polyvecs.mid+=xd[polyvecs.edge.at(m)].kart;
                        if (polyvecs.edge.size())  polyvecs.mid*=1.0/polyvecs.edge.size();
                        polyvecs.norm=Normalize(polyvecs.mid-zent);
                        polyvecs.umfang=r;
                        polyeders.append(polyvecs);
                    }

                }
            }
        }
      //  printf("----\n");
    }    
    minvolpol=10e38;
    maxvolpol=-10e38;
    for (int i=0;i<polyeders.size();i++){
        minvolpol=qMin(minvolpol,polyeders.at(i).umfang);
        maxvolpol=qMax(maxvolpol,polyeders.at(i).umfang);
        if (polyeders.at(i).edge.size()>3){
            int kk=polyeders.at(i).edge.size();
            for (int nn=1;nn<polyeders.at(i).edge.size()-1;nn++){
            V3 start=xd[polyeders.at(i).edge.at(0)].kart;
            for (int j=1; j<kk;j++){
                if (Distance(start,xd[polyeders.at(i).edge.at(j)].kart)>Distance(start,xd[polyeders.at(i).edge.at((j+1)%kk)].kart)){
                //    printf("swapping: %s <=> %s %d %d\n",xd[polyeders.at(i).edge.at(j)].atomname,xd[polyeders.at(i).edge.at((j+1)%kk)].atomname,j,(j+1)%kk);
                    polyeders[i].edge.swap(j,(j+1)%kk);
                //    printf("next %s \n",xd[polyeders.at(i).edge.at(j)].atomname);
                }
                /*else {printf("not swapping: %s <=> %s %d %d %g %g\n",xd[polyeders.at(i).edge.at(j)].atomname,xd[polyeders.at(i).edge.at((j+1)%kk)].atomname,j,(j+1)%kk,
                            Distance(start,xd[polyeders.at(i).edge.at(j)].kart),Distance(start,xd[polyeders.at(i).edge.at((j+1)%kk)].kart));}// */
                start=xd[polyeders.at(i).edge.at(j)].kart;
            }
            }
            for (int j=0; j<kk;j++){
            //    printf("[%4s]",xd[polyeders.at(i).edge.at(j)].atomname);
            }
           // printf("\n");
    }
    }
    printf("polyeders.size() %d\n",polyeders.size());
}


void molekul::make_knopf(QList<INP> xd){


  if (!bonds_made) make_bonds(xd);
  if (!knopf_made) {
    Knopf=(knpf*)malloc(sizeof(knpf)*xd.size());

    for (int i=0; i<xd.size(); i++) Knopf[i].lz=0;
    for (int i=0;i<bcnt;i++){


      Knopf[bd[i].a].lig[Knopf[bd[i].a].lz++]=bd[i].e;
      Knopf[bd[i].a].bnr[Knopf[bd[i].a].lz-1]=i;
      Knopf[bd[i].a].bgrad[Knopf[bd[i].a].lz-1]=(((Kovalenz_Radien[xd[bd[i].a].OrdZahl]/100.0+
						   Kovalenz_Radien[xd[bd[i].e].OrdZahl]/100.0)
						  -(0.08*fabs((ElNeg[xd[bd[i].e].OrdZahl]-
							       ElNeg[xd[bd[i].a].OrdZahl])/100.0)))-
						 sqrt(Distance (xd[bd[i].a].kart, xd[bd[i].e].kart)));
    }
  }


  extern QList<INP> xdinp;
  for (int i=0; i<xdinp.size(); i++) {
    xdinp[i].firstNeighbors=Knopf[i].lz;
    xdinp[i].secondNeighbors=0;
    for (int j=0;j<Knopf[i].lz;j++){
      xdinp[i].secondNeighbors+=Knopf[Knopf[i].lig[j]].lz;

    }
    xdinp[i].secondNeighbors-=xdinp[i].firstNeighbors;

  }
  knopf_made=1;
  make_polyeder(xd);
}

void molekul::countMols(QList<INP> & xdinp){
  for (int i=0; i<xdinp.size();i++)
    xdinp[i].molindex=0;
  int maxmol=1;
  xdinp[0].molindex=1;
  int nochjemand=0;
  int nextmol=0;
  do {//bis alle atome einen molindex > 0 haben 
    nextmol=0;
    do {//bis kein atom mehr in diesem molekul zu finden ist
      nochjemand=0;
      for (int i=1;i<xdinp.size();i++){//atom 0 ist immer in mol 1 daher von 1 zaehlen
	if (xdinp[i].OrdZahl<0) continue;
	for (int j=0; j< Knopf[i].lz;j++){
	  if (xdinp[Knopf[i].lig[j]].OrdZahl<0) continue;
	  if ((xdinp[Knopf[i].lig[j]].molindex==maxmol)&&(xdinp[i].molindex==0)) {
	    //alle nachbarn anschauen ob sie ein molindex == maxmol haben falls das
	    //betrachtete atom noch nicht zugeordnet ist.

	    xdinp[i].molindex=maxmol;
	    nochjemand++;//wir haben nochjemand gefunden der im selben molekuel noch keinen molindex > 0 hatte
	  }
	}
      }
    }while (nochjemand);
    for (int i=0; i<xdinp.size();i++) //gibt es noch atome die molendex == 0 haben dann sind dise im nextmol
      if ((xdinp[i].OrdZahl>=0)&&(xdinp[i].molindex==0)) {
	nextmol=i;
	break;
      }
    if (nextmol) xdinp[nextmol].molindex=++maxmol;//maxmol um 1 ergoehen und DANN an das gefundene atom zuweisen.
    
  }while (nextmol);
  printf ("The structure contains %d molecules\n",maxmol);
}
#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau); a[k][l]=h+s*(g-h*tau);
//NumericalRecepies....
double * molekul::jacobi2(const Matrix &uij, V3 &ev) {

  int j,iq,ip,i,n=3,nrot;
  double tresh=0,theta,tau,t,sm,s,h,g,c;
  double a[3][3],b[3],z[3],v[3][3],d[3];
  a[0][0]=uij.m11;
  a[0][1]=uij.m12;
  a[0][2]=uij.m13;
  a[1][0]=uij.m21;
  a[1][1]=uij.m22;
  a[1][2]=uij.m23;
  a[2][0]=uij.m31;
  a[2][1]=uij.m32;
  a[2][2]=uij.m33;
  static double erg[4]={0.0,1.0,0.0,0.0};
  for (ip=1;ip<=n;ip++) {
    for (iq=1;iq<=n;iq++) v[ip-1][iq-1]=0.0;
    v[ip-1][ip-1]=1.0;
  }
  for (ip=1;ip<=n;ip++) {
    b[ip-1]=d[ip-1]=a[ip-1][ip-1];
    z[ip-1]=0.0;
  }
  nrot=0;
  for (i=1;i<=150;i++) {
    sm=0.0;
    for (ip=1;ip<=n-1;ip++) {
      for (iq=ip+1;iq<=n;iq++)
    sm += fabs(a[ip-1][iq-1]);
    }

    //printf("sm =%20.19f\n",sm);

    if (float(sm) < tresh) {
      if ((v[0][0]+v[1][1]+v[2][2])!=3.0) {
      erg[0]=acos((v[0][0]+v[1][1]+v[2][2]-1.0)/2.0);
      erg[1]=(v[2][1]-v[1][2])/(2.0*sin(erg[0]));
      erg[2]=(v[0][2]-v[2][0])/(2.0*sin(erg[0]));
      erg[3]=(v[1][0]-v[0][1])/(2.0*sin(erg[0]));
      erg[0]*=180.0/M_PI;}
      else {erg[0]=0.0;erg[1]=1.0;erg[2]=0.0;erg[3]=0.0; }
      //printf("%d??ERG:%f %f %f %f\n",i,Ato4d(erg));
      /*
      printf("=%d======================================\n%8.5f %8.5f %8.5f \n%8.5f %8.5f %8.5f \n%8.5f %8.5f %8.5f \n%8.5f %8.5f %8.5f\n========================================\n",i,
              d[0],d[1],d[2],v[0][0],v[0][1],v[0][2]
          ,v[1][0],v[1][1],v[1][2]
          ,v[2][0],v[2][1],v[2][2]
          );*/
     ev=V3(d[0],d[1],d[2]);
     return (double*) erg;
    }
    if (i < 4) tresh=0.00001;
    else tresh=0.0001;
    for (ip=1;ip<=n-1;ip++) {
      for (iq=ip+1;iq<=n;iq++) {
    //printf("\np:%i q:%i i:%i nrot:%i\n",ip,iq,i,nrot);
    g=100.0*fabs(a[ip-1][iq-1]);
    if ((i > 4) && ((fabs(d[ip-1])+g) == fabs(d[ip-1])) && ((fabs(d[iq-1])+g) == fabs(d[iq-1]))) {a[ip-1][iq-1]=0.0;}
    else if (fabs(a[ip-1][iq-1]) >= tresh) {
      h=d[iq-1]-d[ip-1];
      if ((fabs(h)+g) == fabs(h)) {t=(a[ip-1][iq-1])/h; }
      else { theta=0.5*h/(a[ip-1][iq-1]);
      t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
      if (theta < 0.0) {t = -1.0*t;}
      }
      c=1.0/sqrt(1+t*t);
      s=t*c;
      tau=s/(1.0+c);
      h=t*a[ip-1][iq-1];
      z[ip-1] -= h;
      z[iq-1] += h;
      d[ip-1] -= h;
      d[iq-1] += h;
      a[ip-1][iq-1]=0.0;
      for (j=1;j<=ip-1;j++) {
           ROTATE(a,j-1,ip-1,j-1,iq-1)
          //printf("%i %i %i %i",j,ip,j,iq);
          }
      for (j=ip+1;j<=iq-1;j++) {
           ROTATE(a,ip-1,j-1,j-1,iq-1)
          //printf("%i %i %i %i ",ip,j,j,iq);
          }
      for (j=iq+1;j<=n;j++) {
           ROTATE(a,ip-1,j-1,iq-1,j-1)
          //printf("%i %i %i %i",ip,j,iq,j);
          }
      for (j=1;j<=n;j++) {
           ROTATE(v,j-1,ip-1,j-1,iq-1)
          }
      ++(nrot);
      //    printf("U|\n%f %f %f  \n%f %f %f\n%f %f %f\nV|\n%f %f %f  \n%f %f %f\n%f %f %f\n\n",a[0][0],a[1][0],a[2][0],a[0][1],a[1][1],a[2][1],a[0][2],a[1][2],a[2][2],v[0][0],v[1][0],v[2][0],v[0][1],v[1][1],v[2][1],v[0][2],v[1][2],v[2][2]);
    } //else ;//printf("nix:%f p%i q%i",fabs(a[ip-1][iq-1]),ip,iq);
      }
    }
    for (ip=1;ip<=n;ip++) {
      b[ip-1] += z[ip-1];
      d[ip-1] =b[ip-1];
      z[ip-1] =0.0;
    }
  }
  erg[0]=acos((v[0][0]+v[1][1]+v[2][2]-1.0)/2.0);
  if (erg[0]==0) {
    erg[1]=1.0;
    erg[2]=0.0;
    erg[3]=0.0;
  }else{
  erg[1]=(v[2][1]-v[1][2])/(2.0*sin(erg[0]));
  erg[2]=(v[0][2]-v[2][0])/(2.0*sin(erg[0]));
  erg[3]=(v[1][0]-v[0][1])/(2.0*sin(erg[0]));
  erg[0]*=180.0/M_PI;
  }
  /*printf("=%d=======================================\n%8.5f %8.5f %8.5f \n%8.5f %8.5f %8.5f \n%8.5f %8.5f %8.5f \n%8.5f %8.5f %8.5f\n========================================\n",i,
          d[0],d[1],d[2],v[0][0],v[0][1],v[0][2]
          ,v[1][0],v[1][1],v[1][2]
          ,v[2][0],v[2][1],v[2][2]

          );*/
  ev=V3(d[0],d[1],d[2]);
 return (double*)erg;
}
void molekul::loadSettings(){
    QSettings einstellung( QSettings::IniFormat, QSettings::UserScope ,"Christian_B._Huebschle","MoleCoolQt" );
    einstellung.beginGroup("Version 0.1");
    pseSize=einstellung.value("FontSize",10).toInt();
    LOD=einstellung.value("LevelOfDetail",3).toInt();
    tubifiedAtoms=einstellung.value("tubes").toBool();
    bondColorStyle=einstellung.value("singleColorBonds").toBool();
    QVariant variant;
    if (einstellung.contains("bondColor")){
    variant = einstellung.value("bondColor");
    bondColor=variant.value<QColor>();
    }else{bondColor=QColor("silver");}
    if ((einstellung.value("bondStrength").toDouble())>0.001)
        bondStrength=einstellung.value("bondStrength").toDouble();

    int acsize = einstellung.beginReadArray("AtomColors");
    for (int i = 0; i < acsize; ++i) {
      einstellung.setArrayIndex(i);
      Acol[i-1][0] = (GLfloat)einstellung.value("red"   ).toDouble();
      Acol[i-1][1] = (GLfloat)einstellung.value("green" ).toDouble();
      Acol[i-1][2] = (GLfloat)einstellung.value("blue"  ).toDouble();
      Acol[i-1][3] = (GLfloat)einstellung.value("alpha" ).toDouble();
    }
    einstellung.endArray();
    acsize = einstellung.beginReadArray("AtomStyles");
    for (int i = 0; i < acsize; ++i) {
      einstellung.setArrayIndex(i);
      aStyle[i-1] = einstellung.value("Style").toInt();
    }
    einstellung.endArray();
    acsize = einstellung.beginReadArray("CovaleceRadii");
    for (int i = 0; i < acsize; ++i) {
      einstellung.setArrayIndex(i);
      Kovalenz_Radien[i-1] = einstellung.value("Radius").toInt();
    }
    einstellung.endArray();

    acsize = einstellung.beginReadArray("BallRadii");
    for (int i = 0; i < acsize; ++i) {
      einstellung.setArrayIndex(i);
      arad[i-1]=(GLfloat)einstellung.value("Radius").toDouble();
    }
    einstellung.endArray();
}
#define Ato4d(arr)       arr[0], arr[1], arr[2], arr[3]
#include <QGLWidget>

void molekul::modulated(double t,QList<Modulat> mato,int draw,double steps) {
  const int dr_atoms =1;
  const int dr_bonds =2;
  const int dr_unit =4;
  const int dr_adp =8;
//  const int dr_la =16;

  if (draw&dr_atoms) {
    for  (int i=0; i<mato.size();i++){
      int myStyle=aStyle[mato[i].OrdZahl];
      int myAdp=(myStyle&ATOM_STYLE_NOADP)?0:(draw&dr_adp);
      glPushMatrix();
      V3 pos=mato[i].kart(t);
      int nonPositiveDefinite=0;
      //int proba=50;

      //     if (i==2) printf("%-8s %9g %9g %9g at t=%g\n",mato[i].atomname,pos.x,pos.y,pos.z,t);
      double rad=(mato[i].OrdZahl>=0)?arad[mato[i].OrdZahl]:0.15;
      glTranslated(pos.x,pos.y,pos.z);
      if (myAdp){
        Matrix u=mato[i].u(t);
        V3 ev=V3(0,0,0);
        double *arr=jacobi2(u,ev);
        if ((ev.x>0)&&(ev.y>0)&&(ev.z>0)){
          nonPositiveDefinite=0;
          glRotated(Ato4d(arr));
          double psc=1.0;
          // if (atom.at(i).part==666) psc*=1.2;
          switch (proba ) {
            case 10 :{ glScaled(0.76*sqrt(ev.x)*psc,0.76*sqrt(ev.y)*psc,0.76*sqrt(ev.z)*psc);break;}   //Hauptachsen der Eliipsoide 10% Wahrscheinlichkeit
            case 30 :{ glScaled(1.19*sqrt(ev.x)*psc,1.19*sqrt(ev.y)*psc,1.19*sqrt(ev.z)*psc);break;}   //Hauptachsen der Eliipsoide 30% Wahrscheinlichkeit
            case 50 :{ glScaled(1.54*sqrt(ev.x)*psc,1.54*sqrt(ev.y)*psc,1.54*sqrt(ev.z)*psc);break;}   //Hauptachsen der Eliipsoide 50% Wahrscheinlichkeit
            case 70 :{ glScaled(1.91*sqrt(ev.x)*psc,1.91*sqrt(ev.y)*psc,1.91*sqrt(ev.z)*psc);break;}   //Hauptachsen der Eliipsoide 70% Wahrscheinlichkeit
            case 90 :{ glScaled(2.50*sqrt(ev.x)*psc,2.50*sqrt(ev.y)*psc,2.50*sqrt(ev.z)*psc);break;}   //Hauptachsen der Eliipsoide 90% Wahrscheinlichkeit
            default: ;
          }
        }else {nonPositiveDefinite=1;}
      }else glScaled(rad,rad,rad);
      GLUquadricObj *q = gluNewQuadric();
      gluQuadricNormals(q, GL_SMOOTH);   
      glColor4fv(Acol[mato[i].OrdZahl]); 
      if (!nonPositiveDefinite) sphere(0);
      else dCube(rad);

      glPopMatrix();
    }
  }//dr_atoms
  if (draw&dr_bonds){
    glPushMatrix();
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glColor3f(0.5,0.3,0.3);
//    printf("bonds_made %d t-mbonds_last_t %g t= %g\n",bonds_made,t-mbonds_last_t,t);
    if ((!bonds_made)||(fabs(t-mbonds_last_t)>(steps*10)))make_bonds(mato,t);
/*    V3 beg,end;
    double soll_abst;
    for  (int i=0; i<mato.size();i++)
      for  (int j=i+1; j<mato.size();j++) {
        beg=mato[i].kart(t);
        end=mato[j].kart(t); 
        soll_abst=((Kovalenz_Radien[mato[i].OrdZahl]+
              Kovalenz_Radien[mato[j].OrdZahl]))*0.011;
        soll_abst*=soll_abst;
        if (soll_abst>Distance(beg,end)){
          glVertex3d(beg.x, beg.y, beg.z);
          glVertex3d(end.x, end.y, end.z);
        }
      }
//      */
    V3 beg,end;
    for (int k=0;k<bcnt;k++){
        beg=mato[bd[k].a].kart(t);
        end=mato[bd[k].e].kart(t); 
        glVertex3d(beg.x, beg.y, beg.z);
        glVertex3d(end.x, end.y, end.z);
    }
    glEnd();
    glLineWidth(1);
    glPopMatrix();
  }
  if (draw&dr_unit){
    glPushMatrix();
    UnitZell();
    glEnable(GL_LIGHTING);    
    glDisable(GL_BLEND);
    glPopMatrix();
  }
}

void molekul::atoms(CEnvironment atom,int proba){
  /*! Draws atoms as spheres or as ellipses cubes or icosahedrons.
   * @params atoms list of atoms to be drawn .
   * @params proba ellipsoid probability (currently 10 30 50 70 90 %).
   *
   * Molecule.dratom controls the wireframe style of the resulting atom .
   *
   * Molecule.tubifiedAtoms atom balls is drawn in bondstrength.
   *
   * Molecule.adp atom is drawn as a ellipsoid.
   *
   * Molecule.monoQrom Q-Peaks are drawn in Color Molecule.mQolor .
   * */
int myAdp=adp;
int myStyle=7;
int pminus=0;
int wire=0;
//GLfloat white[4]={0.2f, 0.2f, 0.3f, 0.0f};
GLfloat black[4]={0.0f, 0.0f, 0.0f, 1.0f};
for  (int i=0; i<atom.size();i++){
//if ((!dratom)&&(atom.at(i).hidden)) continue;
//if ((nopm1)&&(atom.at(i).sg)&&(atom.at(i).part<0))continue;
if ((dratom==2)&&(atom.at(i).part==0))continue;
if ((dratom==2)&&(atom.at(i).sg)&&(atom.at(i).part<0))continue;
//if ((dratom==2)&&(atom.at(i).hidden))continue;
//if ((dratom==5)&&(atom.at(i).hidden))continue;
int  nonPositiveDefinite=0;
    glPushMatrix();
    nonPositiveDefinite=0;
    glTranslated(atom.at(i).pos.x,atom.at(i).pos.y,atom.at(i).pos.z);
    double rad=(atom.at(i).an==-2)?0.5:qPeakRad;
    bool parthigh=false;
    if (dratom==5){
      wire=1;
      dratom=0;
    }
    if (dratom==2) {
      dratom=0;
      parthigh=true;
    }
    if (!dratom){
 //     printf("ph %d alpha %g tubes %d,myAdp %d %d\n",parthigh,Acol[atom.at(i).an][3],tubes,myAdp,((!myAdp)||((myStyle&ATOM_STYLE_SPHERE)&&(!(myStyle&ATOM_STYLE_SOLID)))));
     if (atom.at(i).an>-1) {
       myStyle=aStyle[atom.at(i).an];
       if (atom.at(i).Label.contains("(noADP)")) myStyle|=ATOM_STYLE_NOADP;
       glColor4d(Acol[atom.at(i).an][0],
               Acol[atom.at(i).an][1],
               Acol[atom.at(i).an][2],
              Acol[atom.at(i).an][3]);
       //printf("r%g g%g b%g a%g\n", Acol[atom.at(i).an][0],
         //             Acol[atom.at(i).an][1],
           //           Acol[atom.at(i).an][2],
             //        Acol[atom.at(i).an][3]);
       rad=arad[atom.at(i).an];

    glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION,black );
                  if ((atom.at(i).sg)&&(atom.at(i).part<0)){
                    glColor4d(Acol[atom.at(i).an][0],
                              Acol[atom.at(i).an][1],
                  1.0,
                  0.3);
            pminus=1;
                    myStyle|=ATOM_STYLE_NOADP;
          }
                myAdp=(myStyle&ATOM_STYLE_NOADP)?0:adp;
    }
     if (wire) {myStyle|=ATOM_STYLE_SPHERE;if (myStyle&ATOM_STYLE_SOLID)myStyle-=ATOM_STYLE_SOLID; }
     if (tubifiedAtoms){
       rad=bondStrength;
       if (atom.at(i).part==666) rad*=4.5;
       myAdp=0;
       nonPositiveDefinite=0;
     }
    if ((atom.at(i).an<0)||(atom.at(i).Label.startsWith('Q',Qt::CaseInsensitive))){
      glColor4d(1.0 ,0.7, 0.8 ,0.4);
      if (atom.at(i).an==-66) {
    rad*=2;
    glColor4d(0.6,0.3,0.1,0.3);
      }
      //if((atom.at(i).an==-1)&&(!monoQrom))
          Farbverlauf(atom.at(i).peakHeight, pmin, pmax);
      //if (monoQrom) glColor3d(mQolor[0],mQolor[1],mQolor.blueF());
//      if (!wire)
    ikosa(rad);

    }
    else {
      if (myAdp){
    V3 ev=V3(0,0,0);
    double *arr=jacobi2(atom.at(i).uc,ev);
    if ((ev.x>0)&&(ev.y>0)&&(ev.z>0)){
          nonPositiveDefinite=0;
    glRotated(Ato4d(arr));
    double psc=(parthigh)?1.32:1.0;
        if (atom.at(i).part==666) psc*=1.2;
    switch (proba ) {
        case 10 :{ glScaled(0.76*sqrt(ev.x)*psc,0.76*sqrt(ev.y)*psc,0.76*sqrt(ev.z)*psc);break;}   //Hauptachsen der Eliipsoide 10% Wahrscheinlichkeit
        case 30 :{ glScaled(1.19*sqrt(ev.x)*psc,1.19*sqrt(ev.y)*psc,1.19*sqrt(ev.z)*psc);break;}   //Hauptachsen der Eliipsoide 30% Wahrscheinlichkeit
        case 50 :{ glScaled(1.54*sqrt(ev.x)*psc,1.54*sqrt(ev.y)*psc,1.54*sqrt(ev.z)*psc);break;}   //Hauptachsen der Eliipsoide 50% Wahrscheinlichkeit
        case 70 :{ glScaled(1.91*sqrt(ev.x)*psc,1.91*sqrt(ev.y)*psc,1.91*sqrt(ev.z)*psc);break;}   //Hauptachsen der Eliipsoide 70% Wahrscheinlichkeit
        case 90 :{ glScaled(2.50*sqrt(ev.x)*psc,2.50*sqrt(ev.y)*psc,2.50*sqrt(ev.z)*psc);break;}   //Hauptachsen der Eliipsoide 90% Wahrscheinlichkeit
        default: ;
        }
      }else {nonPositiveDefinite=1;}
      }

       if (parthigh) {
     rad*=1.32;
         if (atom.at(i).part==666) rad*=1.2;
         glEnable(GL_BLEND);
         myStyle&=(ATOM_STYLE_SPHERE|ATOM_STYLE_NOADP);
     myStyle|=ATOM_STYLE_SPHERE;
       }
// */
      /*
#define ATOM_STYLE_WALLS 1
#define ATOM_STYLE_RINGS 2
#define ATOM_STYLE_SPHERE 4
#define ATOM_STYLE_SOLID 8
#define ATOM_STYLE_WHITERING 16
#define ATOM_STYLE_NOLABEL 32
*/
      if (atom.at(i).an>-1) {
/*
          glEnable(GL_BLEND);
          glDisable(GL_CULL_FACE);*/
        if (myAdp&&intern) {
          if (!nonPositiveDefinite) ellipse(myStyle);
          else dCube(rad);
          }
        else if ((!myAdp)||((myStyle&ATOM_STYLE_SPHERE)&&(!(myStyle&ATOM_STYLE_SOLID)))){
          if (!myAdp) glScaled(rad,rad,rad);
          if (!nonPositiveDefinite) {
        if (wire)dratom =5;
        sphere(myAdp|pminus);

        //ellipse(myAdp);
      }
          else dCube(rad);
        }
      }
      else
        ikosa(rad);
    }
    }
    else{
     //   atom[i].hidden=0;
      sphere(0);
    }
    glEnable(GL_COLOR_MATERIAL);
    glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION,black );
    glPopMatrix();
    if (parthigh) dratom=2;
  }
}
void molekul::draw_polyeders(QList<INP> xdinp){
    //printf("draw_polyeders() %d\n",polyeders.size());

    if (polyeders.empty()) return;
    V3 nor;
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    for (int i=0; i<polyeders.size();i++){
         if (!allowedPolyeders.value(xdinp[polyeders.at(i).zi].OrdZahl,true)) continue;
        GLfloat colo[4]={
            Acol[xdinp[polyeders.at(i).zi].OrdZahl][0],
            Acol[xdinp[polyeders.at(i).zi].OrdZahl][1],
            Acol[xdinp[polyeders.at(i).zi].OrdZahl][2],
            Acol[xdinp[polyeders.at(i).zi].OrdZahl][3]}
            ;
        //colo[3]=0.7-(polyeders.at(i).volume/maxvolpol);
        nor=polyeders.at(i).norm;
        glNormal3d(nor.x,nor.y,nor.z);
        //glColor4f(0.0f,0.5f,0.0f,0.7f);
        if(polyShapColor)
            Farbverlauf((float)i,(float)0.0,(float)polyeders.size()-1.0f,0.5f);
         // Farbverlauf((float)polyeders.at(i).umfang,(float)maxvolpol-0.01,(float)maxvolpol+0.01,0.5);
     // printf("maxvolpol %g %g %g\n",maxvolpol,polyeders.at(i).volume,minvolpol);
        else glColor4fv(colo);
        if (polyeders.at(i).edge.size()==3){
        //    glColor4f(0.0f,0.0f,0.0f,0.0f);
            glVertex3d(xdinp[polyeders.at(i).edge.at(0)].kart.x,xdinp[polyeders.at(i).edge.at(0)].kart.y,xdinp[polyeders.at(i).edge.at(0)].kart.z);
            glVertex3d(xdinp[polyeders.at(i).edge.at(1)].kart.x,xdinp[polyeders.at(i).edge.at(1)].kart.y,xdinp[polyeders.at(i).edge.at(1)].kart.z);
            glVertex3d(xdinp[polyeders.at(i).edge.at(2)].kart.x,xdinp[polyeders.at(i).edge.at(2)].kart.y,xdinp[polyeders.at(i).edge.at(2)].kart.z);
        }
        else{
          for (int j=0; j<polyeders.at(i).edge.size();j++){
              int kk=polyeders.at(i).edge.size();
              int v1=polyeders.at(i).edge.at(j),v2=polyeders.at(i).edge.at((j+1)%kk);
              glVertex3d(polyeders.at(i).mid.x,polyeders.at(i).mid.y,polyeders.at(i).mid.z);
              glVertex3d(
                      xdinp[v1].kart.x,
                      xdinp[v1].kart.y,
                      xdinp[v1].kart.z);
              glVertex3d(
                      xdinp[v2].kart.x,
                      xdinp[v2].kart.y,
                      xdinp[v2].kart.z);
          }

        }
    }
    glEnd();/*
    glColor3f(0,0,0);
    glBegin(GL_LINES);
    for (int i=0; i<polyeders.size();i++){
    glVertex3d(polyeders.at(i).mid.x,polyeders.at(i).mid.y,polyeders.at(i).mid.z);
    glVertex3d(polyeders.at(i).mid.x+polyeders.at(i).norm.x,
               polyeders.at(i).mid.y+polyeders.at(i).norm.y,
               polyeders.at(i).mid.z+polyeders.at(i).norm.z);
    }
    glEnd();*/
    glPopMatrix();
    //glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}
void molekul::bonds(QList<INP> xdinp){

  //BINDUNGEN ab hier
  double gg;
  float wink;
  V3 vec;
  glDisable(GL_CULL_FACE);



  if (!knopf_made) make_knopf(xdinp);
  for (int k=0;k<bcnt;k++){
    if (!bondColorStyle) glColor4fv(Acol[xdinp[bd[k].a].OrdZahl]);
    vec=kreuzX(xdinp[bd[k].a].kart.x-xdinp[bd[k].e].kart.x,xdinp[bd[k].a].kart.y-xdinp[bd[k].e].kart.y,xdinp[bd[k].a].kart.z-xdinp[bd[k].e].kart.z,
	       0.0f,0.0f,1.0f);                 //Achse senkrecht zur Ebene Ursprung, Bindungs-Ende, Z-Achse 

    if (Norm(vec)==0.0) vec=V3(1.0,0.0,0.0);
    vec=Normalize(vec);
    glPushMatrix();	
    glTranslated (xdinp[bd[k].a].kart.x,xdinp[bd[k].a].kart.y,xdinp[bd[k].a].kart.z);//Anfangspunkt 	    
    wink=180+winkel(xdinp[bd[k].a].kart-xdinp[bd[k].e].kart,VZ); // Winkel zwischen Bindungs-Richtung und Z-Achse
    /*printf("Bond %d w%f %f %f %f  dis%f\n",k,wink,xdinp[bd[k].a].kart.x-xdinp[bd[k].e].kart.x,xdinp[bd[k].a].kart.y-xdinp[bd[k].e].kart.y,xdinp[bd[k].a].kart.z-xdinp[bd[k].e].kart.z,
	   //vec.x,vec.y,vec.z,
	   Norm(vec));*/

    glRotatef((float)wink,(float)vec.x,(float)vec.y,(float)vec.z); // drehen    
    GLUquadricObj *q = gluNewQuadric();
    gluQuadricNormals(q, GL_SMOOTH);   // ein Zylinder
    gluQuadricTexture(q,GL_TRUE);
    gg=100.0*sqrt( Distance(xdinp[bd[k].a].kart,xdinp[bd[k].e].kart));

    gluCylinder(q, bondStrength, bondStrength, (float)gg/(200.0f), 5*LOD, 1); 

    glPopMatrix();
  }	
  glEnable(GL_CULL_FACE);
}

int molekul::highlightResi(QList<INP> xdinp,int inv,GLfloat L,bool el){

  if ((inv<0)||(inv>=xdinp.size())){
    firstHL=0;
    lastHL=0;
   return 0;
  }

  QList<INP> xx; 
  firstHL=-1;
  lastHL=0;
  static int iit=21;
  char *tok,nme[80];
  int ri,rii=0,Ri;

  strcpy(nme,xdinp[inv].atomname);
  tok=strtok(nme,"@");

  tok=strtok(NULL,"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_");

  ri=atoi(tok);
  for (int i=0;i<xdinp.size();i++){
    strcpy(nme,xdinp[i].atomname);
    tok=strtok(nme,"@");

    tok=strtok(NULL,"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_");

    Ri=atoi(tok);
    if (ri==Ri){
      xx.append(xdinp[i]);
      rii++;
      firstHL=(firstHL==-1)?i:firstHL;
      lastHL=i;

    }

  }
  if (rii) {  
    static molekul m;
    if (iit) m.initDir();
    copyAcol(m.Acol,m.arad,m.aStyle);
    m.firstHL=firstHL;
    m.lastHL=lastHL;
    m.bonds_made=0;
    m.knopf_made=0;
    m.pmin=pmin;
    m.pmax=pmax;
    m.intern=1;
    m.adpwall=adpwall;
    iit=0;
    glPushMatrix();
    glScaled( L, L, L );
    m.adp=el;
    m.atoms(xx,proba);
    m.intern=0;
    glEnable(GL_BLEND);
    m.atoms(xx,proba);
    glDisable(GL_BLEND);
    m.bonds(xx);
    glPopMatrix(); 
  }
  firstHL=(firstHL==-1)?0:firstHL;

 return lastHL-firstHL;
}
void molekul::highlightInv(QList<INP> xdinp,int inv,GLfloat L){ 
  char ivname[80]="C1.5c[1.5c1.5c]1.5c[1.5c1.5c]1.5c[1.5c1.5c]",*tok; 
  strcpy(ivname,xdinp[inv].invariomname);
  int nextSphere[24],ci=0,fst=0,kla=0;
  if (!bonds_made) make_bonds(xdinp);
  if (!knopf_made) make_knopf(xdinp);

  {
  int al,ab;
  float ag;



  for (int i=0; i<Knopf[inv].lz-1;i++)
    for (int j=0; j<Knopf[inv].lz-1;j++)
      if ((xdinp[Knopf[inv].lig[j]].OrdZahl)&&(xdinp[Knopf[inv].lig[j+1]].OrdZahl)&&(Knopf[inv].bgrad[j]<Knopf[inv].bgrad[j+1])){
	al=Knopf[inv].lig[j];  
	ab=Knopf[inv].bnr[j];  
	ag=Knopf[inv].bgrad[j];
	Knopf[inv].lig[j]=  Knopf[inv].lig[j+1];    
	Knopf[inv].bnr[j]=  Knopf[inv].bnr[j+1];    
	Knopf[inv].bgrad[j]=Knopf[inv].bgrad[j+1];
	Knopf[inv].lig[j+1]=al;  
	Knopf[inv].bnr[j+1]=ab;  
	Knopf[inv].bgrad[j+1]=ag;
      }
  }



    kla=0;
  for (tok=strtok(ivname,"01235.");tok&&ci<40;tok=strtok(NULL,"01235.")){
    if (ci&&strchr(tok,'[')) {nextSphere[fst++]=1;kla=1;}
    else if (ci&&!kla) {nextSphere[fst++]=0;}
    if (strchr(tok,']')) {kla=0;}

    ci++;
  }



  int acnt=0;
  QList<INP> xx;

  xx.append(xdinp[inv]);
  acnt++;
  for (int i=0 ; i<Knopf[inv].lz;i++){



    xx.append(xdinp[Knopf[inv].lig[i]]);
    acnt++;
    if (nextSphere[i])
      for (int j=0 ; j<Knopf[Knopf[inv].lig[i]].lz;j++){
	if (strcmp(xdinp[inv].atomname,xdinp[Knopf[Knopf[inv].lig[i]].lig[j]].atomname)){



	  xx.append(xdinp[Knopf[Knopf[inv].lig[i]].lig[j]]);
	  acnt++;


	}
      }
  }
  static molekul m;
  glPushMatrix();
  glScaled( L, L, L );
  m.adp=0;
  m.atoms(xx,50);
  m.bonds(xx);
  glPopMatrix();
}
QString molekul::h_bonds(QList<INP> xdinp) { 
  
  glDisable(GL_CULL_FACE);
  if (HAMax<0.01)return "";
  glEnable(GL_ALPHA_TEST);
  glBindTexture(GL_TEXTURE_2D, hbtex);
  glEnable(GL_TEXTURE_2D);
  QString hbonds;
/*  FILE *out=fopen("Hbonds.txt","w");*/

  double gg=0,kk=0,ll=0;
  float wink,hb_wink;
  V3 vec;
  int Hyd,Acc,Don=0;//Wasserstoff Akzeptor und Donator
  if (!bonds_made) make_bonds(xdinp);
  if (!knopf_made) make_knopf(xdinp); 
  
  /*printf("nachm Knopf: %s 1st neighbors %d 2nd neighbors %d\n%d %d %d\n",
	 xdinp[xdinp.size()/3].atomname,
	 xdinp[xdinp.size()/3].firstNeighbors,
	 xdinp[xdinp.size()/3].secondNeighbors,knopf_made,xdinp.size(),bcnt);
  */
//  if (out) fprintf(out,
hbonds.append("<h2>Hydrogen Bonds</h2><table border=1><tr><th>Donator---Hydrogen...Acceptor</th><th>Don--Hyd [&Aring;]</th><th> Hyd--Acc [&Aring;]</th><th>Don--Acc [&Aring;]</th><th>D--H-----A </th></tr>");
  for (int i=0;i<xdinp.size();i++) {
    for (int j=i+1;j<xdinp.size();j++) {

      if (((xdinp[i].OrdZahl==0)&&(ElNeg[xdinp[j].OrdZahl]>240))||
	  ((xdinp[j].OrdZahl==0)&&(ElNeg[xdinp[i].OrdZahl]>240))) {      
	Hyd=(xdinp[i].OrdZahl==0)?i:j;
	Acc=(xdinp[i].OrdZahl==0)?j:i;
	Don=(Knopf[Hyd].lz)?Knopf[Hyd].lig[0]:0;	

	gg=sqrt( Distance(xdinp[i].kart,xdinp[j].kart));
	
	if (gg<=HAMax){
	  hb_wink=winkel(xdinp[Acc].kart-xdinp[Hyd].kart, xdinp[Don].kart-xdinp[Hyd].kart);	  
          if ((gg>=1.3)&&(fabs(hb_wink)>HAWink)&&(strstr(xdinp[i].atomname,"dum")==NULL)&&(strstr(xdinp[j].atomname,"dum")==NULL)) {//wenn AbstandRichtig
	    ll=sqrt(Distance(xdinp[Acc].kart,xdinp[Don].kart));
	    kk=sqrt(Distance(xdinp[Don].kart,xdinp[Hyd].kart));
	    hbonds.append(QString("<tr><td>%1---%2...%3</td><td>%4</td><td>%5</td><td>%6</td><td>%7&deg;</td></tr>").arg(xdinp[Don].atomname).arg(xdinp[Hyd].atomname).arg(xdinp[Acc].atomname).arg(kk).arg(gg).arg(ll).arg(hb_wink));
	    glPushMatrix();
	    glTranslated (xdinp[j].kart.x,xdinp[j].kart.y,xdinp[j].kart.z);//z 	    
	    vec=kreuzX(xdinp[i].kart.x-xdinp[j].kart.x,xdinp[i].kart.y-xdinp[j].kart.y,xdinp[i].kart.z-xdinp[j].kart.z,
		       0.0f,0.0f,1.0f); 

	    wink=winkel(xdinp[i].kart-xdinp[j].kart,VZ);               // Winkel zwischen Bindungs-Richtung und Z-Achse	    
	    glRotatef(wink,vec.x,vec.y,vec.z); // drehen
	    GLUquadricObj *q = gluNewQuadric();
	    gluQuadricNormals(q, GL_SMOOTH);   // ein Zylinder
	    //if (!adp) 
	    if (gg>2.0) glColor4f(1,0.6,0,1);	else glColor4f(1,1,0,1);	
	    gluQuadricTexture(q,GL_TRUE);
	    gluCylinder(q,bondStrength*1.05 , bondStrength*1.05, (float)gg, 5*LOD, 1);	    
	    glPopMatrix(); 
	  }
	}
      }
    }
  }

  glDisable(GL_ALPHA_TEST);

  glDisable(GL_TEXTURE_2D);

  

  
   glEnable(   GL_CULL_FACE);
  if (hbonds.contains("deg;")) hbonds.append("</table><br>");
  else hbonds.clear();
 return hbonds;
} 

void molekul::cbonds(QList<INP> xdinp){
  glDisable(GL_CULL_FACE);
  glEnable(GL_ALPHA_TEST);
  glBindTexture(GL_TEXTURE_2D, hbtex);
  glEnable(GL_TEXTURE_2D);
  float wink;
  V3 vec;
  for (int k=0; k<cBonds.size();k++){
    int i=cBonds.at(k).a;
    int j=cBonds.at(k).e;
    float gg=sqrt( Distance(xdinp[i].kart,xdinp[j].kart));
    glPushMatrix();
    glTranslated (xdinp[j].kart.x,xdinp[j].kart.y,xdinp[j].kart.z);//z 	    
    vec=kreuzX(xdinp[i].kart.x-xdinp[j].kart.x,xdinp[i].kart.y-xdinp[j].kart.y,xdinp[i].kart.z-xdinp[j].kart.z,
		    0.0f,0.0f,1.0f); 

    wink=winkel(xdinp[i].kart-xdinp[j].kart,VZ);               // Winkel zwischen Bindungs-Richtung und Z-Achse	    

    glRotatef(wink,vec.x,vec.y,vec.z); // drehen
    GLUquadricObj *q = gluNewQuadric();
    gluQuadricNormals(q, GL_SMOOTH);   // ein Zylinder
    //if (!adp) 
    glColor4f(1,0,1,1);	
    gluQuadricTexture(q,GL_TRUE);
    gluCylinder(q,bondStrength*1.05 , bondStrength*1.05, (float)gg, 5*LOD, 1);	    
    glPopMatrix(); 
  }
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_TEXTURE_2D);
  glEnable(   GL_CULL_FACE);
}

/*
void molekul::labels( QList<INP> xdinp)
{
  //LABELS
  glDisable( GL_LIGHTING );
  glDisable( GL_DEPTH_TEST ); 

  glPushMatrix();{ 
    for (int j=0;j<xdinp.size();j++){

    } 
    
    }glPopMatrix(); 
  glEnable( GL_LIGHTING ); 
  glEnable( GL_DEPTH_TEST );  
}
*/
/*
V3 *  molekul::smoothPoints(V3 *vListe, int N){
  //berecnet aus N puntkten N-1 neue Punkte um Kurven etwas abzurunden
  V3 v1,v2,*newP,ppP,laP=V3(0,0,0);
  int ende=0,stopped=1;

  int newN,ni,ai;
  ppP.x=0;
  ppP.y=0;
  ppP.z=0;
  for (int i=1; i<N-1; i++){
    if (((vListe[i-1].x+vListe[i-1].y+vListe[i-1].z)==0.0)&&
	((vListe[i+1].x+vListe[i+1].y+vListe[i+1].z)==0.0)){
	  vListe[i].x=0.0;
	  vListe[i].y=0.0;
	  vListe[i].z=0.0;
    }    
  }

  newP=(V3*)malloc(sizeof(V3)*(N-2));

  while (ende<N-2){ 
    for (int i=ende+1; i<N-1; i++){
      if (((vListe[i-1].x+vListe[i-1].y+vListe[i-1].z)==0.0)||
	  ((vListe[i].x+vListe[i].y+vListe[i].z)==0.0)||
	  ((vListe[i+1].x+vListe[i+1].y+vListe[i+1].z)==0.0)) {

	if ((vListe[i-1].x+vListe[i-1].y+vListe[i-1].z)==0) {
	  newP[i-1].x=0.0;
	  newP[i-1].y=0.0;
	  newP[i-1].z=0.0;
	} else newP[i-1]=laP;

	ende=i;
	stopped=1;
	break; 
      }
      if(i) v1=vListe[i]-vListe[i-1];
      v2=vListe[i+1]-vListe[i];
      newP[i-1]=(!stopped)?0.5*(ppP+vListe[i]-1/7.0*v2-1/7.0*v1):
	vListe[i]-1/7.0*v2-1/7.0*v1;    
      newP[i-1].rc=vListe[i].rc;
      ppP=vListe[i]+1/7.0*v1+1/7.0*v2;

      laP=vListe[i+1];
      ende=i;
      stopped=0;
    }

  }

  newN=2*N-2;
  ni=0;ai=0;

  if (NULL==(nL=(V3*)malloc(sizeof(V3)*newN))) {perror("sch...\n");exit(0);}

  for (int i=0; i<newN; i++){


    nL[i]=((i)%2)?newP[ni++]:vListe[ai++];
  }
  nL[newN-1]= vListe[N-1];



  free(newP);
  nListe=newN;
  free(vListe);
  vListe=NULL;
 return nL;
}
*/
void molekul::Farbverlauf (GLfloat wrt,GLfloat min,GLfloat max,GLfloat alpha){
  GLclampd ff[4];
  int lauf=0;
  const GLclampd farbe[6][4]={{1.0,0.0,0.0,1.0},
			{1.0,1.0,0.0,1.0},
			{0.0,1.0,0.0,1.0},
			{0.0,1.0,1.0,1.0},
			{0.0,0.0,1.0,1.0},
			{1.0,0.0,1.0,1.0}};
  if ((max-min)<0.0001) max+=0.0001;
  GLfloat nwrt=(wrt-min)/(max-min);
  nwrt=(nwrt>=1.0)?0.99999:nwrt;
  nwrt=(nwrt<=0.0)?0.00001:nwrt;
  lauf=(int (nwrt/0.2));
  nwrt-=(0.2*lauf);
  nwrt/=(0.2);
  


  ff[0]=(1.0-nwrt)*farbe[lauf][0]+farbe[lauf+1][0]*nwrt;
  ff[1]=(1.0-nwrt)*farbe[lauf][1]+farbe[lauf+1][1]*nwrt;
  ff[2]=(1.0-nwrt)*farbe[lauf][2]+farbe[lauf+1][2]*nwrt;
  ff[3]=alpha;

  glColor4dv(ff);


 return;
}

/*
void molekul::drawSline(V3 *vL,int N){


  V3 nv1,v1,v2,lnv;
  GLfloat nv2;
  int arc=-55;
  glColor3f(0.9f,0.3f,0.5f);
  //  
  glDisable(GL_LIGHTING);
  glLineWidth(1);
  glLightModeli(  GL_LIGHT_MODEL_TWO_SIDE, 1 );
  glBegin(GL_LINE_STRIP);
  for (int i=0;i<N;i++){

    if ((arc!=vL[i].rc)||((vL[i].x+vL[i].y+vL[i].z)==0.0)) {
      glEnd();
      switch (vL[i].rc){
      case 1:glEnable(GL_LIGHTING);glBegin(GL_TRIANGLE_STRIP);break;
      default :glDisable(GL_LIGHTING);glBegin(GL_LINE_STRIP);break;
      }
      if(fvl)Farbverlauf((float)i-1,0,N);
      if (i&&(vL[i-1].x+vL[i-1].y+vL[i-1].z)!=0.0) glVertex3f(vL[i-1].x,vL[i-1].y,vL[i-1].z);
      arc=vL[i].rc;
    }
    if ((vL[i].x+vL[i].y+vL[i].z)==0.0) 
      continue;


    if ((i>1)&&(i<N-2)) {
      nv1=(((vL[i-2]-vL[i])+(vL[i-1]-vL[i]))%((vL[i+1]-vL[i])+(vL[i+2]-vL[i])));
      nv2=Norm(nv1);
      nv1=Normalize(nv1);
      if (nv2<0.005) nv1*=0.1; 
      
      if (vL[i].rc!=1)nv1*=0.001;
      lnv=(vL[i-1]-vL[i])
	+ (vL[i+1]-vL[i])
	+ (vL[i-2]-vL[i])
	+ (vL[i+2]-vL[i]);
      lnv=Normalize(lnv);
      lnv*=-1.0;
      v1=vL[i]+0.3*nv1;
      v2=vL[i]-0.3*nv1;
      / *
      V3 dumv=lnv+v1;
      glEnd();
      glDisable(GL_LIGHTING);
      glBegin(GL_LINES);
      if (vL[i].rc==1) glColor3f(1.0,1.0,1.0); else glColor3f(1.0,1.0,0.0);
      glVertex3f(v1.x,v1.y,v1.z);
      glColor3f(0.0,0.0,1.0);
      glVertex3f(dumv.x,dumv.y,dumv.z);
      glEnd();
      switch (vL[i].rc){
      case 1:glEnable(GL_LIGHTING);glBegin(GL_TRIANGLE_STRIP);break;
      default :glDisable(GL_LIGHTING);glBegin(GL_LINE_STRIP);break;
      }
      * /
    if(fvl)Farbverlauf((float)i,0,N);
      glNormal3f(lnv.x,lnv.y,lnv.z);
      glVertex3f(v1.x,v1.y,v1.z);

      glVertex3f(v2.x,v2.y,v2.z);
    }

    glColor3f(0.2f,0.3f,0.5f);
  }
  glEnd();
  glLineWidth(1);

  glEnable(GL_LIGHTING);
}
V3* molekul::addToList(V3 *vL,QList<INP> xdinp,int atom,int N,int rc){
  vL=(V3*)realloc(vL,sizeof(V3)*(N+1));
  vL[N].x=xdinp[atom].kart.x;
  vL[N].y=xdinp[atom].kart.y;
  vL[N].z=xdinp[atom].kart.z;
  vL[N].rc=rc;


 return vL;
}
void molekul::findChains(QList<INP> xdinp){


  if (!bonds_made) make_bonds(xdinp);


  if (!knopf_made) make_knopf(xdinp);




  QString rama;
  int lastI=-100,lastJ=0,j,k,l=0,h=0;  
  int rc=0;;
  char match[25],match1[25],match2[25];
  for (int i=0; i<xdinp.size(); i++){
    if (xdinp[i].OrdZahl==6) {//N


      if ((Knopf[i].lz)&&(xdinp[Knopf[i].lig[0]].OrdZahl==5)) h=Knopf[i].lig[0];
      for (int n=0;n<Knopf[i].lz;n++){


	j=Knopf[i].lig[n];
	


	if ((xdinp[j].OrdZahl==5))//C 	
	  for (int m=0;m<Knopf[j].lz;m++){


	    k=Knopf[j].lig[m]; 
	    strcpy(match,"");
	    for (int o=0;o<Knopf[k].lz;o++){
	      match[o]=((char)xdinp[Knopf[k].lig[o]].OrdZahl+64);
	      match[o+1]='\0';
	      if ((xdinp[k].OrdZahl==5)&&(xdinp[Knopf[k].lig[o]].OrdZahl==6)) l=Knopf[k].lig[o]; 
	    }

	    for (int w=0;w<Knopf[i].lz;w++){
	      match1[w]=((char)xdinp[Knopf[i].lig[w]].OrdZahl+64);
	      match1[w+1]='\0';
	    }
	    for (int v=0;v<Knopf[j].lz;v++){
	      match2[v]=((char)xdinp[Knopf[j].lig[v]].OrdZahl+64);
	      match2[v+1]='\0';
	    }
	      //if (xdinp[Knopf[k].lig[o]].OrdZahl==7)



	    if ((strchr(xdinp[i].atomname,'G'))&&
		(!strstr(xdinp[i].atomname,"Glu"))&&
		(!strstr(xdinp[i].atomname,"Gln"))&&
		(!strstr(xdinp[i].atomname,"Gly"))) lastI=i;
	    if ((strstr(match1,"@@"))&&(strstr(match,"EGG")))lastI=i;
	    if ((lastI!=i)&&(strchr(match,'G'))&&(!strchr(match1,'G'))&&(!strchr(match2,'G'))){
	      lastI=i;
	      double ll=0;
	      if (nListe) ll=sqrt( Distance(vL[nListe-1],xdinp[j].kart));
		if ((nListe)&&((3.9<ll)||(3.7>ll))){

		  vL=(V3*)realloc(vL,sizeof(V3)*(++nListe)); 
		  vL[nListe-1].x=0; 
		  vL[nListe-1].y=0; 
		  vL[nListe-1].z=0; 
		} 
        / *
		l2=sqrt( pow((double)xdinp[i].kart[0]-xdinp[l].kart[0],2.0)+//Ketten-Ende
			 pow((double)xdinp[i].kart[1]-xdinp[l].kart[1],2.0)+
			 pow((double)xdinp[i].kart[2]-xdinp[l].kart[2],2.0));
		mini=(mini<l2)?mini:l2;
		maxi=(maxi>l2)?maxi:l2;
        * /;
		double phi=0,psi=0; 
                rama.append(QString("%1-%2-%3-%4-%5  phi=%6 psi=%7 %8\n") 
				 .arg(xdinp[h].atomname)
				 .arg(xdinp[i].atomname)
				 .arg(xdinp[j].atomname)
				 .arg(xdinp[k].atomname)
				 .arg(xdinp[l].atomname)
				 .arg(phi=dieder(xdinp[i].kart-xdinp[h].kart, xdinp[i].kart-xdinp[k].kart, xdinp[j].kart-xdinp[k].kart),8,'f',5)	  
				 .arg(psi=dieder(xdinp[j].kart-xdinp[i].kart, xdinp[j].kart-xdinp[k].kart, xdinp[k].kart-xdinp[l].kart),8,'f',5) 
				 .arg(j));	  
                if ((phi<-30)&&(phi>-90)&&(psi<0)&&(psi>-60)) rc=1;
                else if ((phi<-20)&&(phi>-170)&&(psi<170)&&(psi>0)) rc=2;
                else rc=0;


		lastJ=j;
		while (nListe<3) vL=addToList(vL,xdinp,j,nListe++,rc);			    
		vL=addToList(vL,xdinp,j,nListe++,rc);			    
	      }
	    }
	  
      }
    }
  }
  if(nListe) {vL=addToList(vL,xdinp,lastJ,nListe++,rc);vL=addToList(vL,xdinp,lastJ,nListe++,rc);}


  if (!rama.isEmpty()){
    QFile ramaf("ramachandran.txt");
    if (ramaf.open(QIODevice::WriteOnly)){
      ramaf.write(rama.toUtf8());
      ramaf.close();
    }
  }

}
*/
void molekul::frac2kart (V3 x, V3 & y){
//x ist fraktionell, y wird kartesisch
  
  y.x = x.x * zelle.f2c.m11 + x.y * zelle.f2c.m12 + x.z * zelle.f2c.m13;
  y.y = x.x * zelle.f2c.m21 + x.y * zelle.f2c.m22 + x.z * zelle.f2c.m23;
  y.z = x.x * zelle.f2c.m31 + x.y * zelle.f2c.m32 + x.z * zelle.f2c.m33;
  
}
void molekul::kart2frac (V3 x, V3 & y){
//x ist kartesisch, y wird fraktionell


  double phi;

  const double g2r=180.0/M_PI;
  Matrix u;   /*Cholesky decomposition of theReal space Metric tensor
              Wird fÃÂ¼r die Umrechnung von fraktionellen in kartesischen Korrdinaten benÃÂ¶tigt.*/

  phi=sqrt(1-pow(cos(zelle.al/g2r),2.0)-pow(cos(zelle.be/g2r),2.0)-pow(cos(zelle.ga/g2r),2.0)+2.0*cos(zelle.al/g2r)*cos(zelle.be/g2r)*cos(zelle.ga/g2r));
  y.x =0.0 ;
  y.y =0.0 ;
  y.z =0.0;
  u.m11 = 1.0/zelle.a;
  u.m21 = 0.0;
  u.m31 = 0.0;
  u.m12 = -1.0/(zelle.a * tan(zelle.ga/g2r));
  u.m22 = 1.0/(zelle.b * sin(zelle.ga/g2r));
  u.m32 = 0.0;
  u.m13 = (cos(zelle.al/g2r)*cos(zelle.ga/g2r)-cos(zelle.be/g2r))/(zelle.a*phi*sin(zelle.ga/g2r));
  u.m23 = (cos(zelle.be/g2r)*cos(zelle.ga/g2r)-cos(zelle.al/g2r))/(zelle.b*phi*sin(zelle.ga/g2r));
  u.m33 = sin(zelle.ga/g2r)/(zelle.c*phi);
      // Wird jetzt hier genauso wie in int Tab B S.345 gerechnet (M^-1^).
  y.x = x.x * u.m11 + x.y * u.m12 + x.z * u.m13;
  y.y = x.x * u.m21 + x.y * u.m22 + x.z * u.m23;
  y.z = x.x * u.m31 + x.y * u.m32 + x.z * u.m33;
}
void molekul::delBlanks(char *in){
  char s[82];
  int j=0;
  for (unsigned int i=0;i<=strlen(in);i++){
    if (in[i]!=' ') {s[j++]=in[i];}
  }
  in=strcpy(in,s);
}

void molekul::copyAcol(GLfloat _Acol[108][4],GLfloat _arad[108],int _aStyle[108]){//hier gibts was zu tun!
  for (int i=0; i<107;i++){
    _Acol[i][0]= Acol[i][0];
    _Acol[i][1]= Acol[i][1];
    _Acol[i][2]= Acol[i][2];
    _Acol[i][3]= Acol[i][3];
    _arad[i]=arad[i];
    _aStyle[i]=aStyle[i];
  }
}
bool molekul::decodeSymmCard(const QString symmCard){

  QString sc=symmCard.toUpper().remove("SYMM").trimmed();  
  sc.remove("'");
  sc.remove(" ");

  QStringList axe=sc.split(",");
  QStringList bruch;
  if (axe.size()<3) return false;
  double sx[3],sy[3],sz[3],t[3];
  for (int i=0; i<3; i++){
    sx[i]=0;sy[i]=0;sz[i]=0;t[i]=0;
    if (axe.at(i).contains("-X")) {sx[i]=-1.0;axe[i].remove("-X");}
    else if (axe.at(i).contains("X")) {sx[i]=1.0;axe[i].remove("X");}
    if (axe.at(i).contains("-Y")) {sy[i]=-1.0;axe[i].remove("-Y");}
    else if (axe.at(i).contains("Y")) {sy[i]=1.0;axe[i].remove("Y");}
    if (axe.at(i).contains("-Z")) {sz[i]=-1.0;axe[i].remove("-Z");}
    else if (axe.at(i).contains("Z")) {sz[i]=1.0;axe[i].remove("Z");}
    if (axe.at(i).endsWith("+")) axe[i].remove("+");
    if (axe.at(i).contains("/")) {
      bruch=axe.at(i).split("/");
      if (bruch.size()==2) t[i]=bruch.at(0).toDouble() / bruch.at(1).toDouble();  
    }
    else if (!axe.at(i).isEmpty()) t[i]=axe.at(i).toDouble();
  }
  Matrix sm = Matrix(
      sx[0],sy[0],sz[0],	  
      sx[1],sy[1],sz[1],  
      sx[2],sy[2],sz[2]);
  if (axe.size()==4){
    V3 x4sym;
    double x4,x4tr;
    x4sym.x=0;x4sym.y=0;x4sym.z=0;x4tr=0;
    if (axe.at(3).contains("-X")) {x4sym.x=-1.0;axe[3].remove("-X");}
    else if (axe.at(3).contains("X")) {x4sym.x=1.0;axe[3].remove("X");}
    if (axe.at(3).contains("-Y")) {x4sym.y=-1.0;axe[3].remove("-Y");}
    else if (axe.at(3).contains("Y")) {x4sym.y=1.0;axe[3].remove("Y");}
    if (axe.at(3).contains("-Z")) {x4sym.z=-1.0;axe[3].remove("-Z");}
    else if (axe.at(3).contains("Z")) {x4sym.z=1.0;axe[3].remove("Z");}
    if (axe.at(3).contains("-R")) {x4=-1.0;axe[3].remove("-R");}
    else if (axe.at(3).contains("R")) {x4=1.0;axe[3].remove("R");}
    if (axe.at(3).endsWith("+")) axe[3].remove("+");
    if (axe.at(3).contains("/")) {
      bruch=axe.at(3).split("/");
      if (bruch.size()==2) x4tr=bruch.at(0).toDouble() / bruch.at(1).toDouble();  
    }
    else if (!axe.at(3).isEmpty()) x4tr=axe.at(3).toDouble();
  
  zelle.x4.append(x4);
  zelle.x4tr.append(x4tr);
  zelle.x4sym.append(x4sym);
  printf("sm\n%2g %2g %2g %2g, %g\n%2g %2g %2g %2g, %g\n%2g %2g %2g %2g, %g\n%2g %2g %2g %2g, %g\n\n", sm.m11,sm.m21,sm.m31,0.,t[0],sm.m12,sm.m22,sm.m32,0.,t[1],sm.m13,sm.m23,sm.m33,0.,t[2],x4sym.x,x4sym.y,x4sym.z,x4,x4tr);
//  printf("[%d]%s%s%s%s%s%f\n",zelle.x4.size(),(x4sym.x==0.0)?"":(x4sym.x==1.0)?"x1":"-x1", (x4sym.y==0.0)?"":(x4sym.y==1.0)?"+x2":"-x2", (x4sym.z==0.0)?"":(x4sym.z==1.0)?"+x3":"-x3", (x4==1.0)?"+x4":"-x4", (x4tr<0)?"":"+",x4tr); 
  }
  zelle.symmops.append(sm);
  zelle.trans.append(V3(t[0],t[1],t[2]));
//printf("\n%g %g %g %g\n%g %g %g %g\n%g %g %g %g\n\n", sx[0],sy[0],sz[0],t[0],sx[1],sy[1],sz[1],t[1],  sx[2],sy[2],sz[2],t[2]);
//printf("Sm\n%g %g %g \n%g %g %g \n%g %g %g \n\n", sm.m11,sm.m21,sm.m31,sm.m12,sm.m22,sm.m32,sm.m13,sm.m23,sm.m33);


  return true;
}
QString molekul::encodeSymm(int s){
    QString erg="  ";
    Matrix m;
    V3 t;
    m=zelle.symmops.at(s);
    QString symstrX,symstrY,symstrZ;
    if (m.m11==1) symstrX.append("+x");
    if (m.m11==-1) symstrX.append("-x");
    if (m.m12==1) symstrX.append("+y");
    if (m.m12==-1) symstrX.append("-y");
    if (m.m13==1) symstrX.append("+z");
    if (m.m13==-1) symstrX.append("-z");

    if (m.m21==1) symstrY.append("+x");
    if (m.m21==-1) symstrY.append("-x");
    if (m.m22==1) symstrY.append("+y");
    if (m.m22==-1) symstrY.append("-y");
    if (m.m23==1) symstrY.append("+z");
    if (m.m23==-1) symstrY.append("-z");

    if (m.m31==1) symstrZ.append("+x");
    if (m.m31==-1) symstrZ.append("-x");
    if (m.m32==1) symstrZ.append("+y");
    if (m.m32==-1) symstrZ.append("-y");
    if (m.m33==1) symstrZ.append("+z");
    if (m.m33==-1) symstrZ.append("-z");
    t=zelle.trans.at(s);
    V3 zaehler,nenner;
    double egal;
    for (int g=1;g<5; g++){
        nenner.x=(t.x<0)?-g:g;;
        zaehler.x=(int)((t.x)*g);
        if (fabs(modf(t.x*g,&egal))<0.05) break;
    }
    for (int g=1;g<5; g++){
        nenner.y=(t.y<0)?-g:g;
        zaehler.y=(int)((t.y)*g);
        if (fabs(modf(t.y*g,&egal))<0.05) break;
    }
    for (int g=1;g<5; g++){
        nenner.z=(t.z<0)?-g:g;
        zaehler.z=(int)((t.z)*g);
        if (fabs(modf(t.z*g,&egal))<0.05) break;
    }
    erg.append(QString("%1/%2%3, %4/%5%6, %7/%8%9 \n")
               .arg(zaehler.x)
               .arg(nenner.x)
               .arg(symstrX)
               .arg(zaehler.y)
               .arg(nenner.y)
               .arg(symstrY)
               .arg(zaehler.z)
               .arg(nenner.z)
               .arg(symstrZ));

    erg.remove(QRegExp("0/\\d"));
    erg.replace(" +","  ");
    erg.replace("1/1","1");
    erg.replace("2/1","2");
    erg.replace("3/1","3");
    erg.replace("4/1","4");
    erg.replace("5/1","5");
    return erg;
}


void molekul::bonds(Connection bond){
 /*! @params bond list of bonds to be drawn
  * draws bonds as cylinders with radius Molecule.bondStrength .
  * Bonds start and stop at the atom surface.
  */
    //printf("Bonds %d %d\n",bond.size(),dratom);
//  GLfloat white[4]={0.2, 0.2, 0.3, 0.0};
  GLfloat black[4]={0.0, 0.0, 0.0, 1.0};
  glDisable(GL_BLEND);
  glEnable(GL_ALPHA_TEST);
  glBindTexture(GL_TEXTURE_2D, hbtex);
  //printf("Doing Bonds adp=%d proba=%d tubes%d\n",adp,proba,tubes);
  V3 vec,bondir;
  double kurz=0,ara1,ara2;
  double wink;
  bool parthigh=false;
  double psc=1.0;
  if (dratom==2) {
    dratom=0;
    parthigh=true;
    psc=1.32;
    glEnable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
//    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  }
  for (int i=0; i<bond.size();i++){
      if ((parthigh)&&((bond.at(i).ato1->part==0)||(bond.at(i).ato2->part==0))) continue;
      if ((parthigh)&&(((bond.at(i).ato1->part<0)&&(bond.at(i).ato1->sg))||((bond.at(i).ato2->part<0)&&(bond.at(i).ato2->sg)))) continue;
      //if ((nopm1)&&(bond.at(i).ato1->sg)&&(bond.at(i).ato1->part<0))continue;
      //if ((nopm1)&&(bond.at(i).ato2->sg)&&(bond.at(i).ato2->part<0))continue;
      glDisable(GL_TEXTURE_2D);
    glColor4d(0.5,0.3,0.3,1.0);
    if (adp) {
      V3 hin=Normalize(bond.at(i).ato2->pos-bond.at(i).ato1->pos);
      double r=1;
      switch (proba){
          case 10:r=0.76;break;
          case 30:r=1.19;break;
          case 50:r=1.54;break;
          case 70:r=1.91;break;
          case 90:r=2.50;break;
      }
      ara1=sqrt((hin*bond.at(i).ato1->uc)*hin);
      ara1*=r;
      ara2=sqrt((hin*bond.at(i).ato2->uc)*hin);
      ara2*=r;
      //printf("%s-%s %f %f %d %f\n",bond.at(i).ato1->Label.toStdString().c_str(),bond.at(i).ato2->Label.toStdString().c_str(),ara1, ara2, proba,r);
    }
    else{
      ara1=arad[bond.at(i).ato1->an];
      ara2=arad[bond.at(i).ato2->an];
    }
    if (bond.at(i).ato1->Label.contains("noADP"))ara1=arad[bond.at(i).ato1->an];
    if (bond.at(i).ato2->Label.contains("noADP"))ara2=arad[bond.at(i).ato2->an];
    if (aStyle[bond.at(i).ato1->an]&ATOM_STYLE_NOADP) ara1=arad[bond.at(i).ato1->an];
    if (aStyle[bond.at(i).ato2->an]&ATOM_STYLE_NOADP) ara2=arad[bond.at(i).ato2->an];

    if (((bond.at(i).ato1->sg)&&(bond.at(i).ato1->part<0))) ara1=arad[bond.at(i).ato1->an];
    if (((bond.at(i).ato2->sg)&&(bond.at(i).ato2->part<0))) ara2=arad[bond.at(i).ato2->an];
    if (tubifiedAtoms) ara1=ara2=bondStrength;
    if (bond.at(i).ato2->an>-1) {
        glColor4d(Acol[bond.at(i).ato2->an][0],
                  Acol[bond.at(i).ato2->an][1],
                  Acol[bond.at(i).ato2->an][2],
                  Acol[bond.at(i).ato2->an][3]);

    }

    ara1=qMax(ara1,bondStrength);
    ara2=qMax(ara2,bondStrength);

   /* if ((highlightEquivalents)&&(bond.at(i).ato2->sg|bond.at(i).ato1->sg))
        glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION,white );
    else */
        glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION,black );
    if ((bond.at(i).ato2->sg|bond.at(i).ato1->sg)&&((bond.at(i).ato2->part<0)||(bond.at(i).ato1->part<0))){
      //		    glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION,blue );
      glColor4d(Acol[bond.at(i).ato2->an][0],
                Acol[bond.at(i).ato2->an][1],
        1.0,
        0.3);
      glEnable(GL_BLEND);
    }
    vec.x=(bond.at(i).ato2->pos.y-bond.at(i).ato1->pos.y);
    vec.y=(bond.at(i).ato1->pos.x-bond.at(i).ato2->pos.x);
    vec.z=0;
    if (Norm(vec)) vec=Normalize(vec);
    else vec=V3(0,1,0);
    bondir =V3(0,0,0);
    if (bondStrength<ara2)
      bondir= Normalize(bond.at(i).ato1->pos-bond.at(i).ato2->pos)*(0.9*sqrt((ara2*ara2)-bondStrength*bondStrength));
    kurz=sqrt(Norm(bondir));//  */
    //printf("%s--%s kurz %f len %f ara2 %f\n",bond.at(i).ato1->Label.toStdString().c_str(),bond.at(i).ato2->Label.toStdString().c_str(),kurz,bond.at(i).length,ara2);
    glPushMatrix();
    //    glTranslated (mitsav.x,mitsav.y,mitsav.z);
    glTranslated (
    bond.at(i).ato2->pos.x+bondir.x,
    bond.at(i).ato2->pos.y+bondir.y,
    bond.at(i).ato2->pos.z+bondir.z);//Anfangspunkt
    wink=acos(((bond.at(i).ato1->pos.z-bond.at(i).ato2->pos.z)/
                (sqrt((bond.at(i).ato1->pos.x-bond.at(i).ato2->pos.x)*(bond.at(i).ato1->pos.x-bond.at(i).ato2->pos.x)+
                  (bond.at(i).ato1->pos.y-bond.at(i).ato2->pos.y)*(bond.at(i).ato1->pos.y-bond.at(i).ato2->pos.y)+
                  (bond.at(i).ato1->pos.z-bond.at(i).ato2->pos.z)*(bond.at(i).ato1->pos.z-bond.at(i).ato2->pos.z)))))/M_PI*180.0;

    glRotated(wink,vec.x,vec.y,vec.z); // drehen
    GLUquadricObj *q = gluNewQuadric();
    gluQuadricNormals(q, GL_SMOOTH);   // ein Zylinder
    gluQuadricTexture(q,GL_TRUE);
       if (parthigh) {
     glEnable(GL_BLEND);
     QColor pcolor;
     switch (bond.at(i).ato1->part){
         case 1: pcolor=QColor("lightskyblue");break;
         case 2: pcolor=QColor("darkgoldenrod");break;
         case 3: pcolor=Qt::green;break;
         case 4: pcolor=Qt::yellow;break;
         case 5: pcolor=QColor("orange");break;
         case 6: pcolor=Qt::black;break;
         case 7: pcolor=QColor("lightsalmon");break;
         case 8: pcolor=QColor("lightseagreen");break;
         case -1: pcolor=QColor("aquamarine");break;
         case -2: pcolor=QColor("olive");break;
         case -3: pcolor=QColor("bisque");break;
                 case 666: pcolor=QColor("fuchsia");break;
         default: if (bond.at(i).ato1->part>0)
                pcolor=QColor::colorNames().at(qMin(bond.at(i).ato1->part,QColor::colorNames().size()-1));
              else pcolor=Qt::magenta;
     }
     pcolor.setAlpha(140);
     glColor4d(pcolor.redF(),
             pcolor.greenF(),
             pcolor.blueF(),
             pcolor.alphaF());

       }
    //if (trans) {glEnable(GL_BLEND);glEnable(GL_CULL_FACE);}

       //gluCylinder(q, bondStrength*psc, bondStrength*psc, (float)bond.at(i).length/2.0f-kurz, 5*LOD, 1);
       //printf("%g %g %d\n",bondStrength,bond.at(i).length,LOD);
       gluCylinder(q, bondStrength*psc, bondStrength*psc, (float)(bond.at(i).length /2.0f -kurz), 5*LOD, 1);
    //if (trans) {glDisable(GL_BLEND);glDisable(GL_CULL_FACE);}
    glPopMatrix();

    if (bond.at(i).ato1->an>-1){
        glColor4d(Acol[bond.at(i).ato1->an][0],
                        Acol[bond.at(i).ato1->an][1],
                        Acol[bond.at(i).ato1->an][2],
                        Acol[bond.at(i).ato1->an][3]);

    }
    if ((bond.at(i).ato2->sg|bond.at(i).ato1->sg)&&((bond.at(i).ato2->part<0)||(bond.at(i).ato1->part<0))){
      //		    glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION,blue );
      glColor4d(Acol[bond.at(i).ato1->an][0],
                Acol[bond.at(i).ato1->an][1],
        1.0,
        0.3);
      glEnable(GL_BLEND);
    }

    vec.x=(bond.at(i).ato1->pos.y-bond.at(i).ato2->pos.y);
    vec.y=(bond.at(i).ato2->pos.x-bond.at(i).ato1->pos.x);
    vec.z=0;
    if (Norm(vec)) vec=Normalize(vec);
    else vec=V3(0,1,0);

    bondir =V3(0,0,0);
    if (bondStrength<ara1)
      bondir= Normalize(bond.at(i).ato2->pos-bond.at(i).ato1->pos)*(0.9*sqrt((ara1*ara1)-bondStrength*bondStrength));
    kurz=sqrt(Norm(bondir));// */
    glPushMatrix();
    //    glTranslated (mitsav.x,mitsav.y,mitsav.z);
    glTranslated (
            bond.at(i).ato1->pos.x+bondir.x,
            bond.at(i).ato1->pos.y+bondir.y,
            bond.at(i).ato1->pos.z+bondir.z);//Anfangspunkt
    wink=acos(((bond.at(i).ato2->pos.z-bond.at(i).ato1->pos.z)/
                (sqrt((bond.at(i).ato2->pos.x-bond.at(i).ato1->pos.x)*(bond.at(i).ato2->pos.x-bond.at(i).ato1->pos.x)+
                  (bond.at(i).ato2->pos.y-bond.at(i).ato1->pos.y)*(bond.at(i).ato2->pos.y-bond.at(i).ato1->pos.y)+
                  (bond.at(i).ato2->pos.z-bond.at(i).ato1->pos.z)*(bond.at(i).ato2->pos.z-bond.at(i).ato1->pos.z)))))/M_PI*180.0;
    glRotated(wink,vec.x,vec.y,vec.z); // drehen
    q = gluNewQuadric();
    gluQuadricNormals(q, GL_SMOOTH);   // ein Zylinder
    gluQuadricTexture(q,GL_TRUE);
    //if (trans) {glEnable(GL_BLEND);glEnable(GL_CULL_FACE);}
       if (parthigh) {
     glEnable(GL_BLEND);
     QColor pcolor;
     switch (bond.at(i).ato2->part){
         case 1: pcolor=QColor("lightskyblue");break;
         case 2: pcolor=QColor("darkgoldenrod");break;
         case 3: pcolor=Qt::green;break;
         case 4: pcolor=Qt::yellow;break;
         case 5: pcolor=QColor("orange");break;
         case 6: pcolor=Qt::black;break;
         case 7: pcolor=QColor("lightsalmon");break;
         case 8: pcolor=QColor("lightseagreen");break;
         case -1: pcolor=QColor("aquamarine");break;
         case -2: pcolor=QColor("olive");break;
         case -3: pcolor=QColor("bisque");break;
                 case 666: pcolor=QColor("fuchsia");break;
         default: if (bond.at(i).ato2->part>0)
                pcolor=QColor::colorNames().at(qMin(bond.at(i).ato2->part,QColor::colorNames().size()-1));
              else pcolor=Qt::magenta;
     }
     pcolor.setAlpha(140);
     glColor4d(pcolor.redF(),
             pcolor.greenF(),
             pcolor.blueF(),
             pcolor.alphaF());

       }
    gluCylinder(q, bondStrength*psc, bondStrength*psc, (float)(bond.at(i).length/2.0f-kurz), 5*LOD, 1);
    //printf("%g %g %d\n",bondStrength,bond.at(i).length,LOD);
    gluDeleteQuadric(q);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
  }

}

QString molekul::symmcode2human(QStringList brauchSymm){
  QString erg;
  Matrix m;
  V3 t;
  int h,k,l,s;
  for (int j=0; j<brauchSymm.size();j++){
  sscanf(brauchSymm[j].toLatin1(),"%d_%1d%1d%1d",&s,&h,&k,&l);
  h-=5;
  k-=5;
  l-=5;
  s--;
  m=zelle.symmops.at(s);
  QString symstrX,symstrY,symstrZ;
/*  if (m.m11==1) symstrX.append("+x");
  if (m.m11==-1) symstrX.append("-x");
  if (m.m12==1) symstrX.append("+y");
  if (m.m12==-1) symstrX.append("-y");
  if (m.m13==1) symstrX.append("+z");
  if (m.m13==-1) symstrX.append("-z");

  if (m.m21==1) symstrY.append("+x");
  if (m.m21==-1) symstrY.append("-x");
  if (m.m22==1) symstrY.append("+y");
  if (m.m22==-1) symstrY.append("-y");
  if (m.m23==1) symstrY.append("+z");
  if (m.m23==-1) symstrY.append("-z");

  if (m.m31==1) symstrZ.append("+x");
  if (m.m31==-1) symstrZ.append("-x");
  if (m.m32==1) symstrZ.append("+y");
  if (m.m32==-1) symstrZ.append("-y");
  if (m.m33==1) symstrZ.append("+z");
  if (m.m33==-1) symstrZ.append("-z");*/
  if (m.m11==1) symstrX.append("+x");
  if (m.m11==-1) symstrX.append("-x");
  if (m.m21==1) symstrX.append("+y");
  if (m.m21==-1) symstrX.append("-y");
  if (m.m31==1) symstrX.append("+z");
  if (m.m31==-1) symstrX.append("-z");
          
  if (m.m12==1) symstrY.append("+x");
  if (m.m12==-1) symstrY.append("-x");
  if (m.m22==1) symstrY.append("+y");
  if (m.m22==-1) symstrY.append("-y");
  if (m.m32==1) symstrY.append("+z");
  if (m.m32==-1) symstrY.append("-z");
          
  if (m.m13==1) symstrZ.append("+x");
  if (m.m13==-1) symstrZ.append("-x");
  if (m.m23==1) symstrZ.append("+y");
  if (m.m23==-1) symstrZ.append("-y");
  if (m.m33==1) symstrZ.append("+z");
  if (m.m33==-1) symstrZ.append("-z");
  t=zelle.trans.at(s);
  V3 zaehler,nenner;
  double egal;
  for (int g=1;g<5; g++){
    nenner.x=(t.x<0)?-g:g;;
    zaehler.x=(int)((t.x+h)*g);
    if (fabs(modf(t.x*g,&egal))<0.05) break;
  }
  for (int g=1;g<5; g++){
    nenner.y=(t.y<0)?-g:g;
    zaehler.y=(int)((t.y+k)*g);
    if (fabs(modf(t.y*g,&egal))<0.05) break;
  }
  for (int g=1;g<5; g++){
    nenner.z=(t.z<0)?-g:g;
    zaehler.z=(int)((t.z+l)*g);
    if (fabs(modf(t.z*g,&egal))<0.05) break;
  }
  erg.append(QString("%1 ''%2'':<b> %3/%4%5, %6/%7%8, %9/%10%11</b><br>")
		  .arg(j+1)
		  .arg(brauchSymm.at(j))
		  .arg(zaehler.x)
		  .arg(nenner.x)
		  .arg(symstrX)
		  .arg(zaehler.y)
		  .arg(nenner.y)
		  .arg(symstrY)
		  .arg(zaehler.z)
		  .arg(nenner.z)
		  .arg(symstrZ));

  }
  erg.remove(QRegExp("0/\\d"));
  erg.replace("1/1","1");
  erg.replace("2/1","2");
  erg.replace("3/1","3");
  erg.replace("4/1","4");
  erg.replace("5/1","5");
  erg.replace(",,",",");
  return erg;
}

bool molekul::applyLatticeCentro(const QChar latt,const bool centro){
  int z=zelle.symuncent=zelle.symmops.size();
  zelle.centro=centro;
  zelle.lattis=latt;

  Matrix inv(-1.0,0.0,0.0, 0.0,-1.0,0.0, 0.0,0.0,-1.0);
  if (centro) 
    for (int i=0; i<z;i++){
  //    Matrix mm=zelle.symmops.at(i);
      Matrix m=zelle.symmops.at(i)*inv;
      zelle.symmops.append(m);
      zelle.trans.append(zelle.trans.at(i));
//printf("inv\n%g %g %g \n%g %g %g \n%g %g %g \n\n", inv.m11,inv.m12,inv.m13,inv.m21,inv.m22,inv.m23,inv.m31,inv.m32,inv.m33);
//printf("m\n%g %g %g \n%g %g %g \n%g %g %g \n\n", m.m11,m.m12,m.m13,m.m21,m.m22,m.m23,m.m31,m.m32,m.m33);
//printf("om\n%g %g %g \n%g %g %g \n%g %g %g \n\n", mm.m11,mm.m12,mm.m13,mm.m21,mm.m22,mm.m23,mm.m31,mm.m32,mm.m33);
    }
  z=zelle.symmops.size();
  switch (latt.toAscii()){
	  case 'A' :
		  for (int i=0; i<z;i++){
		    V3 tt = zelle.trans.at(i)+V3(0.0, 0.5, 0.5);
		    tt.x=(tt.x>1)?tt.x-1:tt.x;
		    tt.y=(tt.y>1)?tt.y-1:tt.y;
		    tt.z=(tt.z>1)?tt.z-1:tt.z;
		    zelle.symmops.append(zelle.symmops.at(i));
		    zelle.trans.append(tt);
                    if (!zelle.x4sym.isEmpty()){
                    zelle.x4sym.append(zelle.x4sym.at(i));
                    zelle.x4.append(zelle.x4.at(i));
                    zelle.x4tr.append(zelle.x4tr.at(i));
                    }
		  }
		  break;
	  case 'B' :
		  for (int i=0; i<z;i++){
		    V3 tt = zelle.trans.at(i)+V3(0.5, 0.0, 0.5);
		    tt.x=(tt.x>1)?tt.x-1:tt.x;
		    tt.y=(tt.y>1)?tt.y-1:tt.y;
		    tt.z=(tt.z>1)?tt.z-1:tt.z;
		    zelle.symmops.append(zelle.symmops.at(i));
		    zelle.trans.append(tt);
                    if (!zelle.x4sym.isEmpty()){
                    zelle.x4sym.append(zelle.x4sym.at(i));
                    zelle.x4.append(zelle.x4.at(i));
                    zelle.x4tr.append(zelle.x4tr.at(i));
                    }
		  }
		  break;
	  case 'C' :
		  for (int i=0; i<z;i++){
		    V3 tt = zelle.trans.at(i)+V3(0.5, 0.5, 0.0);
		    tt.x=(tt.x>1)?tt.x-1:tt.x;
		    tt.y=(tt.y>1)?tt.y-1:tt.y;
		    tt.z=(tt.z>1)?tt.z-1:tt.z;
		    zelle.symmops.append(zelle.symmops.at(i));
		    zelle.trans.append(tt);
                    if (!zelle.x4sym.isEmpty()){
                    zelle.x4sym.append(zelle.x4sym.at(i));
                    zelle.x4.append(zelle.x4.at(i));
                    zelle.x4tr.append(zelle.x4tr.at(i));
                    }
		  }
		  break;
	  case 'F' :
		  for (int i=0; i<z;i++){
		    V3 tt = zelle.trans.at(i)+V3(0.0, 0.5, 0.5);
		    tt.x=(tt.x>1)?tt.x-1:tt.x;
		    tt.y=(tt.y>1)?tt.y-1:tt.y;
		    tt.z=(tt.z>1)?tt.z-1:tt.z;
		    zelle.symmops.append(zelle.symmops.at(i));
		    zelle.trans.append(tt);
		    tt = zelle.trans.at(i)+V3(0.5, 0.0, 0.5);
		    tt.x=(tt.x>1)?tt.x-1:tt.x;
		    tt.y=(tt.y>1)?tt.y-1:tt.y;
		    tt.z=(tt.z>1)?tt.z-1:tt.z;
		    zelle.symmops.append(zelle.symmops.at(i));
		    zelle.trans.append(tt);
		    tt = zelle.trans.at(i)+V3(0.5, 0.5, 0.0);
		    tt.x=(tt.x>1)?tt.x-1:tt.x;
		    tt.y=(tt.y>1)?tt.y-1:tt.y;
		    tt.z=(tt.z>1)?tt.z-1:tt.z;
		    zelle.symmops.append(zelle.symmops.at(i));
		    zelle.trans.append(tt);
                    if (!zelle.x4sym.isEmpty()){
                    zelle.x4sym.append(zelle.x4sym.at(i));
                    zelle.x4.append(zelle.x4.at(i));
                    zelle.x4tr.append(zelle.x4tr.at(i));
                    }
		  }
		  break;
	  case 'I' :
		  for (int i=0; i<z;i++){
		    V3 tt = zelle.trans.at(i)+V3(0.5, 0.5, 0.5);
		    tt.x=(tt.x>1)?tt.x-1:tt.x;
		    tt.y=(tt.y>1)?tt.y-1:tt.y;
		    tt.z=(tt.z>1)?tt.z-1:tt.z;
		    zelle.symmops.append(zelle.symmops.at(i));
		    zelle.trans.append(tt);
                    if (!zelle.x4sym.isEmpty()){
                    zelle.x4sym.append(zelle.x4sym.at(i));
                    zelle.x4.append(zelle.x4.at(i));
                    zelle.x4tr.append(zelle.x4tr.at(i));
                    }
		  }
		  break;
	  case 'R' :
		  for (int i=0; i<z;i++){
		    V3 tt = zelle.trans.at(i)+V3(2.0/3.0, 1.0/3.0, 1.0/3.0);
		    tt.x=(tt.x>1)?tt.x-1:tt.x;
		    tt.y=(tt.y>1)?tt.y-1:tt.y;
		    tt.z=(tt.z>1)?tt.z-1:tt.z;
		    zelle.symmops.append(zelle.symmops.at(i));
		    zelle.trans.append(tt);
		    tt = zelle.trans.at(i)+V3(1.0/3.0, 2.0/3.0, 2.0/3.0);
		    tt.x=(tt.x>1)?tt.x-1:tt.x;
		    tt.y=(tt.y>1)?tt.y-1:tt.y;
		    tt.z=(tt.z>1)?tt.z-1:tt.z;
		    zelle.symmops.append(zelle.symmops.at(i));
		    zelle.trans.append(tt);
                    if (!zelle.x4sym.isEmpty()){
                    zelle.x4sym.append(zelle.x4sym.at(i));
                    zelle.x4.append(zelle.x4.at(i));
                    zelle.x4tr.append(zelle.x4tr.at(i));
                    }
		  }
		  break;
	  case 'P' :break;  
  //        case 'X' ://jana superspace group centering
  //                  break;
	  default: return false;
  }
  return true;
}

void molekul::setup_zelle(){  
  const double g2r=180.0/M_PI;
  zelle.phi=  sqrt(1-(cos(zelle.al/g2r)*cos(zelle.al/g2r))-
		  (cos(zelle.be/g2r)*cos(zelle.be/g2r))-(cos(zelle.ga/g2r)*cos(zelle.ga/g2r))
		  +2*cos(zelle.al/g2r)*cos(zelle.be/g2r)*cos(zelle.ga/g2r));
  zelle.V = zelle.a*zelle.b*zelle.c*zelle.phi;
  zelle.as=zelle.c*zelle.b*sin(zelle.al/g2r)/zelle.V;
  zelle.bs=zelle.c*zelle.a*sin(zelle.be/g2r)/zelle.V;
  zelle.cs=zelle.a*zelle.b*sin(zelle.ga/g2r)/zelle.V;
  const double tau=zelle.c*((cos(zelle.al/g2r)-cos(zelle.be/g2r)*cos(zelle.ga/g2r))/sin(zelle.ga/g2r));
  zelle.o1.m11=zelle.o[0][0] =zelle.as*zelle.a;
  zelle.o1.m12=zelle.o[0][1] = 0.0;
  zelle.o1.m13=zelle.o[0][2] = 0.0;
  zelle.o1.m21=zelle.o[1][0] = zelle.bs*zelle.b*cos(zelle.ga/g2r);
  zelle.o1.m22=zelle.o[1][1] = zelle.bs*zelle.b*sin(zelle.ga/g2r);
  zelle.o1.m23=zelle.o[1][2] = 0.0;
  zelle.o1.m31=zelle.o[2][0] = zelle.cs*zelle.c* cos(zelle.be/g2r);
  zelle.o1.m32=zelle.o[2][1] = zelle.cs*tau;
  zelle.o1.m33=zelle.o[2][2] = zelle.cs*zelle.c* zelle.phi / sin(zelle.ga /g2r);
}
void molekul::Uf2Uo(const Matrix x, Matrix & y) {
 y=(zelle.o1*x)*transponse(zelle.o1);
}

Modulat Modulat::applySymm(Matrix sym3d, V3 trans3d, V3 x4sym, int x4,double x4trans){
  Modulat that=*this;
  Modulat *newatom=new Modulat(that);
  newatom->frac0=(sym3d*frac0)+trans3d;
//  printf("applySymm %d = %d %d = %d %d = %d\n",newatom->wo,newatom->os.size(),newatom->wp,newatom->possin.size(),newatom->wt,newatom->usin.size());
  for (int i=0; i<wp;i++){
  //  printf("%sfrac o sin %g %g %g cos %g %g %g\n",atomname,possin[i].x,possin[i].y,possin[i].z,poscos[i].x,poscos[i].y,poscos[i].z);
  newatom->possin[i]=possin[i]*sym3d;
  newatom->poscos[i]=poscos[i]*sym3d;
//    printf("%sfrac wp%d sin %g %g %g cos %g %g %g %p\n",atomname,i,newatom->possin[i].x,newatom->possin[i].y,newatom->possin[i].z,newatom->poscos[i].x,newatom->poscos[i].y,newatom->poscos[i].z,newatom);
  }
  newatom->uf0=(sym3d*uf0)*transponse(sym3d);
  for (int i=0; i<wt; i++){
  newatom->usin[i]=(sym3d*usin[i])*transponse(sym3d);
  newatom->ucos[i]=(sym3d*ucos[i])*transponse(sym3d);
  }
  newatom->x4sym=x4sym;
  newatom->x4=x4;
  newatom->x4trans=x4trans;
  return *newatom;
}
const V3 Modulat::kart(const double t){
  V3 p=frac(t);
  V3 y;
  
  y.x = p.x * mol->zelle.f2c.m11 + p.y * mol->zelle.f2c.m12 + p.z * mol->zelle.f2c.m13;
  y.y = p.x * mol->zelle.f2c.m21 + p.y * mol->zelle.f2c.m22 + p.z * mol->zelle.f2c.m23;
  y.z = p.x * mol->zelle.f2c.m31 + p.y * mol->zelle.f2c.m32 + p.z * mol->zelle.f2c.m33;
  return y;
}
const V3 Modulat::frac(const double t){
  V3 p=frac0;
// printf("%sfrac x4sy %g %g %g x4 %d x4tr %g q %g %g %g \n",atomname,x4sym.x,x4sym.y,x4sym.z,x4,x4trans,mol->zelle.qvec.x,mol->zelle.qvec.y,mol->zelle.qvec.z);
  double X4=t+(mol->zelle.qvec*frac0);
//  printf("%sfrac X4 %g frac0 %g %g %g t%g\n",atomname,X4,frac0.x,frac0.y,frac0.z,t);
  X4=(x4sym*frac0)+x4*X4+x4trans;
//  printf("%sfrac X4 %g\n",atomname,X4);
  switch (sp) {
  case 0:
  for (int i=0; i<wp;i++){
  //  printf("%sfrac sin %g %g %g cos %g %g %g\n",atomname,possin[i].x,possin[i].y,possin[i].z,poscos[i].x,poscos[i].y,poscos[i].z);
    p+=possin[i]*sin(2*M_PI*(i+1)*X4);
    p+=poscos[i]*cos(2*M_PI*(i+1)*X4);
  }
  break;
  case 1://sawtooth
//not yet
      break;
  }
 // printf("%sfrac %g %g %g\n",atomname,p.x,p.y,p.z);
  return p;
}

Matrix Modulat::uf(double t){
  Matrix m=uf0;
  if ((m.m22==0.0)&&(m.m33==0.0)) {
      m.m33=m.m22=m.m11;
      m.m12=m.m13=m.m23=0.0;
  }
//  if (OrdZahl==0){ printf("%-9s %g %g %g %g %g %g\n",atomname,m.m11, m.m22, m.m33, m.m12, m.m13, m.m23); }
  double X4=t+(mol->zelle.qvec*frac0);
  X4=(x4sym*frac0)+x4*X4+x4trans;
  double cs,sn;
  for (int i=0; i<wt;i++){
    sn=sin(2*M_PI*(i+1)*X4);
    cs=cos(2*M_PI*(i+1)*X4);

    m.m11+=usin[i].m11*sn;
    m.m22+=usin[i].m22*sn;
    m.m33+=usin[i].m33*sn;
    m.m12+=usin[i].m12*sn;
    m.m13+=usin[i].m13*sn;
    m.m23+=usin[i].m23*sn;

    m.m11+=ucos[i].m11*cs;
    m.m22+=ucos[i].m22*cs;
    m.m33+=ucos[i].m33*cs;
    m.m12+=ucos[i].m12*cs;
    m.m13+=ucos[i].m13*cs;
    m.m23+=ucos[i].m23*cs;
  }
  m.m21=m.m12;
  m.m31=m.m13;
  m.m32=m.m23;
  return m;
}

Matrix Modulat::u(double t){
  Matrix m=uf(t);
  //Matrix n=(mol->zelle.o1*m)*transponse(mol->zelle.o1);
  Matrix n=(transponse(mol->zelle.o1)*m)*(mol->zelle.o1);
  return n;
}

void Modulat::errorMsg(QString msg){
  qDebug()<<msg;
}
    INP Modulat::toINP(double t){
    INP atom;
    strcpy(atom.atomname,atomname);
    atom.lflag=atom.icor1=atom.icor2=atom.nax=atom.nay1=atom.nay2=atom.na3=0;
    atom.jtf=2;
    atom.atomtype=1;
    atom.noofkappa=atom.lmax=atom.isym=atom.ichcon=0;
    atom.amul=amul;
    atom.imul=imul;
    atom.frac=frac(t);
    atom.kart=kart(t);
    atom.OrdZahl=OrdZahl;
    atom.part=atom.resiNr=0;  
    atom.sg=sg;
    atom.molindex=molindex;
    atom.screenX=screenX;
    atom.screenY=screenY;
    return atom;
    }

/*
 *
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
  double peakHeight;
  int molindex;
  int sg;
  double screenX,screenY;
  uint GLname;
};*/ 
