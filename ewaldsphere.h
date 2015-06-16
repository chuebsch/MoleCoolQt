
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
#ifndef EWALDSPHERE_H
#define EWALDSPHERE_H
#include <QtGui>
#include "molekul.h"
struct Refl{
  int h,k,l,sc;
  double I,sig;
  V3 pos;
  double sintl;
};

class EwaldSphereGLW: public QGLWidget {
Q_OBJECT
  public:
  molekul *mol;//!< pointer to ShelXle's Molecule object.
  int proba;//!< probability level of the ellipsoids.
  EwaldSphereGLW(QString fileName, molekul *externMole,QWidget * parent = 0);
  //EwaldSphereGLW(const MyAtom *atom,Molecule *externMole);
  QFont myFont;//!< a font for the atom labels
  //QStringList pse;
  QTimer *timer;//!< a timer for the molecule rotation
 protected:
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);
 public slots:
 private:
  bool erlaubt(int h, int k, int l);
  QPoint lastPos;
  GLint vp[4];
  double sintl(int h, int k, int l);
  double COSAL, RCOSAL,
         COSBE, RCOSBE,
         COSGA, RCOSGA,
         smin,smax,imax,imin,linesize,pointsize;
  double SINAL;    
  double SINBE;    
  double SINGA; 
  double rc_alpha; 
  double rc_beta; 
  double rc_gamma; 









  V3 xs,ys,zs;
  QList<Refl> reflexe;
  int maxH,maxK,maxL,minH,minK,minL;
  void draw();
};
class EwaldSphereDlg: public QDialog {
Q_OBJECT
  public:
  EwaldSphereDlg(QString fileName,molekul *externMole);
  //EwaldSphereDlg(MyAtom *onlyThisAtom,Molecule *externMole);
  EwaldSphereGLW *eacGLW;//!<The QGLWidget displaying an example molecule.
  public slots:
    void zoneChanged(int z);
    void layerChanged(int l);
  protected slots:
  void accept () ;
  void reject () ;
 private:
  molekul *mol;
  QGridLayout *eacGLO;
};
#endif
