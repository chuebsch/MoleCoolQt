
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
#ifndef EACDLG_H
#define EACDLG_H
#include <QtGui>
#include "molekul.h"
#include "psewidget.h"
/*! \brief EacGLW is the QGLWidget of the Edit Atom Styles Dialog EacDlg. 
 * 
 * EacGLW displays a sp3 like molecule. The 'molecule' consists of a central atom with anisotrop ADP and a neighboring atom without ADP 
 * (displayed as a ball) both are from the same changeable type. Attached to the central atom are also 3 hydrogen atoms. The molecule 
 * rotates continously around its own axis which itself rotates as well. This widget demonstates the look and feel of an atom specific 
 * setting which can be changed in the EacDlg. 3 Q-Peaks are displayed arround the molecule. 
 */

class EacGLW: public QGLWidget {
Q_OBJECT
  public:
  molekul *mol;//!< pointer to ShelXle's Molecule object.
  double exmin,//!< Minimal Q-Peak electron density value of shelXle to be restored when the dialog is closed.
	 exmax;//!< Maximal Q-Peak electron density value of shelXle to be restored when the dialog is closed.
  CEnvironment xd;//!< MyAtom list of the molecule
  Connection bonds;//!< MyBond list of the molecule
  int proba;//!< probability level of the ellipsoids.
  EacGLW( molekul *externMole,QWidget * parent = 0);
  //EacGLW(const MyAtom *atom,Molecule *externMole);
  QFont myFont;//!< a font for the atom labels
  //QStringList pse;
  QTimer *timer;//!< a timer for the molecule rotation
  signals:
      void colorchanged(QColor bgcolor);//!< colorchanged is emitted  by the color dialog colorDLG to change the color of a chemical element. @param bgcolor the new color
 protected:
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();
 public slots:
  void changeAtom(int OZ);//!< changes the atom type of the central atom in OZ @param OZ atomic number (staring from zero) of the new atom
  void colorDLG();//!< changes the color of an atom type
  void wallA(int st);//!< wallA changes the state of the ellipsoid wall. @param st is the checkbox state of the calling QCheckBox.
  void ringA(int st);//!< ringA changes the state of the principal ellipses. @param st is the checkbox state of the calling QCheckBox.
  void spheA(int st);//!< spheA changes the state of the ellipsoid surface. @param st is the checkbox state of the calling QCheckBox. 
  void soldA(int st);//!< soldA changes the transparence state of the ellipsoid surface. @param st is the checkbox state of the calling QCheckBox.
  void nolaA(int st);//!< nolaA changes the state of the atom labels @param st is the checkbox state of the calling QCheckBox.
  void whitA(int st);//!< whitA changes the color of the principal ellipses from black to white. @param st is the checkbox state of the calling QCheckBox.
//  void metalA(int st);//!< metalA changes the state of the bond to without this feature checked on into stipled style @param st is the checkbox state of the calling QCheckBox.
 private:
  Connection connecting(const CEnvironment &atom);
  GLint vp[4];
  void draw();
};
/*! \brief MultiStyle a small dialog called in EacDlg to change the style of multiple elements at once.
 */
class MultiStyle: public QDialog {
Q_OBJECT
  public:
  MultiStyle(molekul *externMole);
  QCheckBox *walbx;  //!<Draw intersecting planes
  QCheckBox *rinbx;  //!<Draw principal ellipses
  QCheckBox *sphbx;  //!<Draw ellipsoid surface
  QCheckBox *sldbx;  //!<Solid ellipsoid
  QCheckBox *nlabx;  //!<No label
  QCheckBox *pcentr; //!<Allowed Polyeder center
 // QCheckBox *metalic;//!<Dashed Bonds to others
  QComboBox *start;  //!<First element from which the style should be changed
  QComboBox *end;    //!<Last element from which the style should be changed

  public slots:
  void checkWall();
  void checkSolid();
  void checkBall();
 private:
  molekul *mol;
};
/*! \brief EacDlg is the Edit Atom Styles Dialog of ShelXle which is used to control various settings of how molecules are drawn.
 */
class EacDlg: public QDialog {
Q_OBJECT
  public:
  EacDlg(molekul *externMole);
  //EacDlg(MyAtom *onlyThisAtom,Molecule *externMole);
  EacGLW *eacGLW;//!<The QGLWidget displaying an example molecule.
  public slots:
  void colorDLG2();//!< opens the bond color dialog.
  void colorchange(QColor bgcolor);//!< changes the color of the current element to bgcolor. Changes the color of the corresponding button.
  void updateLEs(int OZ);//!< when the current element changes the settings of this element are loaded and applied to the EacGLW widget and the PSEWidget. @param OZ atomic number (staring from zero) of the new atom
  void checkWall();//!< when solid is checked then solid is unchecked.
  void checkSolid();//!< when walls are checked solid is unchecked. 
  void checkBall();//!< when ellipsoid surface is unchecked the solid checkbox is diabled. 
  void checkPolyCentr();
  void doMulti();//!< calls MultiStyle dialog
  void unicolor(bool b);//!< the bonds shall have a uniq color. 
  void woADP(bool b);//!< the elemen shall be shown without ADP ellispoids.
  void probaChanged(int ix);//!< The ellipsoid probability is changed to ix.
  void strengthChanged(double d);//!< the bond thicknes is changed to d.
  void QPeakRadChanged(double d);//!< QPeak size is changed to d.
  void CPRadChanged(double d);//!< CP size is changed to d.
  void lodChanged(int d);//!< the level of detail is changed to d.
  void kradChanged(double d);//!< the colvalent radius is changed to d.   
  void radChanged(double d);//!< the ball radius used in ball stick mode is changed to d.
  void exportSettings();//!< export settings to a specified ini file.
  void importSettings();//!< import settings from a specified ini file.
  protected slots:
  void accept () ;
  void reject () ;
 private:
  molekul *mol;
  //MyAtom *otm;
  QDoubleSpinBox *bondStrength;
  QSpinBox *lodSpinBox;
  QComboBox *eacProba;
  PSEWidget *psew;
  QPushButton *colorbut, *colorbut2,*impbut,*expbut;
  QGridLayout *eacGLO;
  //QComboBox *eacCB;
  QDoubleSpinBox *eacRAD;
  QDoubleSpinBox *eacKRAD;

  QCheckBox *pcentr; //!<Allowed Polyeder center
  QCheckBox *walbx;
  QCheckBox *rinbx;
  QCheckBox *sphbx;
  QCheckBox *sldbx;
  QCheckBox *nlabx;
 // QCheckBox *metalic;

  QCheckBox *noadp;
};
#endif
