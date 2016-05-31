// beispiele/opengl/mainWindow.cpp
#include <QtGui>
#include <QtOpenGL>
#include <cmath>
#include "cubeGL.h"
#include "invariomDlg.h"  
#include "XDDlg.h"  
#include "inames.h"
V3 mil;
CubeGL::CubeGL(QWidget *parent,double vang) : QGLWidget(parent) {
   setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer) );
   atomsClickable=true;
   negpdf=true;
   faceCull=0;
   paparazi=false;
   tvalue=0.0;
   vangle=vang;
   chicken= new QAction("Permanent wireframe mode",this);
   chicken->setCheckable(true);
   chicken->setChecked(false);
   connect(chicken,SIGNAL(toggled(bool)),this,SLOT(updateGL()));
   quickRot= new QAction("No change in view while rotation",this );
   quickRot->setCheckable(true);
   quickRot->setChecked(false);
   connect(quickRot,SIGNAL(toggled(bool)),this,SLOT(toggleNoWaitLabel(bool)));
   growIt=false;
   pause = monochrom = false;
   altemitte=V3(0,0,0);
   reSe=false;
   rotze=-1;
   imFokus=-1;
   cdens=16;
   cbas=0;
   moving = new QTimer(this);
   moving->setSingleShot(true);
   connect(moving,SIGNAL(timeout()),this,SLOT(updateGL()));
   stereo_mode=0;
   awidth=cwid=1;
   milsize=0.03448275862068966*vangle;
   molisoTransparence=true;
   zebra=false;
   drawAt=drawBo=drawLa=drawHb=true;
   drawAx=drawUz=false;
   moveLab=false;
   moveLeg=false;
   elli=true;
   moliso=NULL;
   mlsc=1.0;
   invEditAble=false;
   xdSetupMode=false;
 //  Luftschlange=true;
   horizont=false;
   MIS=false;
   moai=false;
   tCR=tCG=tCB=tCA=1.0;
   bgCR=bgCG=bgCB=bgCA=0.3;
   bas=ibas=iSel=0;
   depthCueing=false;
   glDisable(GL_FOG);
   MM[0]=MM[5]=MM[10]=MM[15]=1.0;
   MM[1]=MM[2]=MM[3]=MM[4]=MM[6]=MM[7]=MM[8]=MM[9]=MM[11]=MM[12]=MM[13]=0.0;
   MM[14]=-200.0;
   mil.x=-0.07931034482758621*vangle;
   mil.y=-0.0103448275862069*vangle;
   noWaitLabel=true;
}

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

void CubeGL::toggleNoWaitLabel(bool b){
  noWaitLabel=b;
  updateGL();
}

void CubeGL::noneCull(bool b){
  if (b) faceCull=0;
  updateGL();
}

void CubeGL::backCull(bool b){
  if (b) faceCull=1;
  updateGL();
}

void CubeGL::frontCull(bool b){
  if (b) faceCull=2;
  updateGL();
}

void CubeGL::scaleLegend(int size){
    milsize=(size/30.0)*0.03448275862068966*vangle;
    //printf("milsize %g\n",milsize);
    updateGL();
}

void CubeGL::togglMolisoTransparence(bool b){
    molisoTransparence=b;
    updateGL();
}

void CubeGL::setContourCnt(int cnt){
    cdens=512-cnt;
    setContourWidth(awidth);
}

void CubeGL::setContourWidth(int width){
    cwid=(cdens*width/256);
    awidth=width;
    updateGL();
}

void CubeGL::togglContours(bool b){
    zebra=b;
    updateGL();
}

void CubeGL::setViewAngle(double ang){
    if ((ang>0.001)&&(ang<160.0)){
      //printf("View angle = %g degrees. %10.8f %f\n",ang,ang/vangle,vangle);
      glScaled(ang/vangle,ang/vangle,ang/vangle);
      vangle=ang;
      milsize=0.03448275862068966*vangle;
      if (!horizont){
        //x -0.708863  y -0.0942207
        mil.x=-0.0788*vangle;
        mil.y=-0.0105*vangle;
      }else{
        //-0.452525  y -0.725207
        mil.x=-0.05*vangle;
        mil.y=-0.08*vangle;
      }
      homeXY();
    }
}

void CubeGL::homeXY(){
  glGetDoublev(GL_MODELVIEW_MATRIX,MM);
  MM[12]=MM[13]=0;
  glLoadMatrixd(MM);
  updateGL();
}

void CubeGL::togglMonoChrom(bool b){
 monochrom=b;
 updateGL();
}
void CubeGL::saveMISettings(){
  QString fn=QFileDialog::getSaveFileName(this, tr("Save MoleCoolQt MolIso Settings file "), "MoleCoolQt.moliso.ini",
		                         "MoleCoolQt.moliso.ini (*.ini);;",&selectedFilter,QFileDialog::DontUseNativeDialog );

   QFile mconf(fn);
   mconf.open(QIODevice::WriteOnly);
   mconf.write("MoleCoolQt-MolIso-Settings\r\n\r\nThis file will be overwritten.\r\nDo not edit!!!\r\n");
   mconf.write(QString("Show contours: %1\r\n").arg(zebra).toLatin1());
   mconf.write(QString("Density of contours: %1\r\n").arg(cdens).toLatin1());
   mconf.write(QString("Contour width: %1\r\n").arg(cwid).toLatin1());
   mconf.write(QString("Isosurface transparency: %1\r\n").arg(molisoTransparence).toLatin1());
   mconf.write(QString("Faceculling mode: %1\r\n").arg(faceCull).toLatin1());
   mconf.write(QString("Show legend: %1\r\n").arg(MILe).toLatin1());
   mconf.write(QString("Legend scale factor: %1\r\n").arg(milsize).toLatin1());
   mconf.write(QString("Legend position: %1,%2\r\n").arg(mil.x).arg(mil.y).toLatin1());
   mconf.write(QString("Legend Orientation: %1\r\n").arg(horizont).toLatin1());
   mconf.write(QString("Legend font: %1\r\n").arg(QVariant(MLegendFont).toString()).toLatin1());
   mconf.write(QString("Draw axes: %1\r\n").arg(drawAx).toLatin1());
   mconf.write(QString("Draw unit cell box: %1\r\n").arg(drawUz).toLatin1());     
   mconf.write(QString("Background gradient: %1\r\n").arg(back_grad).toLatin1());	
   mconf.write(QString("Draw atoms: %1\r\n").arg(drawAt).toLatin1());
   mconf.write(QString("Ellipsoids: %1\r\n").arg(elli).toLatin1());
   mconf.write(QString("Draw bonds: %1\r\n").arg(drawBo).toLatin1());
   mconf.write(QString("Draw labels: %1\r\n").arg(drawLa).toLatin1());
   mconf.write(QString("Draw h-bonds: %1\r\n").arg(drawHb).toLatin1());
   mconf.write(QString("Draw helices: %1\r\n").arg(false).toLatin1());
   mconf.write(QString("Depthcueing: %1\r\n").arg(depthCueing).toLatin1());
   mconf.write(QString("Monochrome legend: %1\r\n").arg(monochrom).toLatin1());     
   GLdouble mm[16];
   glGetDoublev(GL_MODELVIEW_MATRIX,mm);
   mconf.write(QString("Orientation matrix: %1/%2/%3/%4/%5/%6/%7/%8/%9/%10/%11/%12/%13/%14/%15/%16\r\n")
		   .arg(mm[0]).arg(mm[1]).arg(mm[2]).arg(mm[3]).arg( mm[4]).arg(mm[5])
		   .arg(mm[6]).arg(mm[7]).arg( mm[8]).arg(mm[9]).arg(mm[10]).arg(mm[11])
		   .arg( mm[12]).arg(mm[13]).arg(mm[14]).arg(mm[15]).toLatin1());
   mconf.write("End of MoleCoolQt-MolIso-Settings\r\n");
   mconf.close();
}
void CubeGL::loadMISettings(){
  QString fn=QFileDialog::getOpenFileName(this, tr("Open MoleCoolQt MolIso Settings file "), "MoleCoolQt.moliso.ini",
		                         "MoleCoolQt.moliso.ini (*.ini);;",&selectedFilter,QFileDialog::DontUseNativeDialog );

  if (fn.isEmpty()) return;
  QFile miconf(fn);
  miconf.open(QIODevice::ReadOnly);
  QString all=miconf.readAll();
  miconf.close();
  zebra=all.section(QRegExp("[:\r]"),5,5).toInt();
  cdens=all.section(QRegExp("[:\r]"),7,7).toInt();
  cwid=all.section(QRegExp("[:\r]"),9,9).toInt();
  awidth=(cwid+1)*256/cdens;
  molisoTransparence=all.section(QRegExp("[:\r]"),11,11).toInt();
  faceCull=all.section(QRegExp("[:\r]"),13,13).toInt();  
  MILe=all.section(QRegExp("[:\r]"),15,15).toInt();
  milsize=all.section(QRegExp("[:\r]"),17,17).toDouble();
  mil.x=all.section(QRegExp("[:\r]"),19,19).section(",",0,0).toDouble();
  mil.y=all.section(QRegExp("[:\r]"),19,19).section(",",1,1).toDouble();
  horizont=all.section(QRegExp("[:\r]"),21,21).toInt();
  MLegendFont= QVariant(all.section(QRegExp("[:\r]"),23,23)).value<QFont>();
  drawAx = all.section(QRegExp("[:\r]"),25,25).toInt();
  drawUz = all.section(QRegExp("[:\r]"),27,27).toInt();
  back_grad = all.section(QRegExp("[:\r]"),29,29).toInt();
  drawAt = all.section(QRegExp("[:\r]"),31,31).toInt();
  elli = all.section(QRegExp("[:\r]"),33,33).toInt();
  drawBo = all.section(QRegExp("[:\r]"),35,35).toInt();
  drawLa = all.section(QRegExp("[:\r]"),37,37).toInt();
  drawHb = all.section(QRegExp("[:\r]"),39,39).toInt();
 // Luftschlange = all.section(QRegExp("[:\r]"),41,41).toInt();
  depthCueing = all.section(QRegExp("[:\r]"),43,43).toInt();
  monochrom = all.section(QRegExp("[:\r]"),45,45).toInt();
  QString matz = all.section(QRegExp("[:\r]"),47,47);
  MM[0] = matz.section('/',0,0).toDouble();
  MM[1] = matz.section('/',1,1).toDouble(); 
  MM[2] = matz.section('/',2,2).toDouble();
  MM[3] = matz.section('/',3,3).toDouble();
  MM[4] = matz.section('/',4,4).toDouble();
  MM[5] = matz.section('/',5,5).toDouble();
  MM[6] = matz.section('/',6,6).toDouble();
  MM[7] = matz.section('/',7,7).toDouble();
  MM[8] = matz.section('/',8,8).toDouble();
  MM[9] = matz.section('/',9,9).toDouble();
  MM[10]= matz.section('/',10,10).toDouble();
  MM[11]= matz.section('/',11,11).toDouble();
  MM[12]= matz.section('/',12,12).toDouble();
  MM[13]= matz.section('/',13,13).toDouble();
  MM[14]= matz.section('/',14,14).toDouble();
  MM[15]= matz.section('/',15,15).toDouble();
  printf("milsize %g\n",milsize);
  setMatrix();
  emit mconf();
  updateGL();
    printf("milsize %g\n",milsize);
}

void CubeGL::setFont(){
  bool ok;
#if (QT_VERSION >= 0x040500)
  myFont = QFontDialog::getFont(&ok, myFont, this,"Change Label Font",QFontDialog::DontUseNativeDialog);
#else
  myFont = QFontDialog::getFont(&ok, myFont, this,"Change Label Font");
#endif
  updateGL();
}

void CubeGL::setMLFont(QFont fo){
  int size = MLegendFont.pointSize();
  MLegendFont = fo;
  MLegendFont.setPointSize(size);
  updateGL();
}

void CubeGL::setMLFontSize(int size){
  MLegendFont.setPointSize(size);
  updateGL();
}

void CubeGL::setIsosurface(bool on){
  MIS= on;
  updateGL();
}

void CubeGL::setLegend(bool on){
  MILe= on;
  updateGL();
}

void CubeGL::togglMOAI(bool on){
  moai=!on;
  updateGL();
}

void  CubeGL::setNoAtom(bool on){
  drawAt=!on;
  updateGL();
}    

void  CubeGL::setAtom(bool on){
  drawAt=on;
  updateGL();
}    

void CubeGL::setDepthCueing(bool on){
  depthCueing=on;
  if (depthCueing) glEnable(GL_FOG);
  else glDisable(GL_FOG);
  updateGL();
}

void CubeGL::setLabelsMovable(bool on){
  moveLab=on;
  updateGL();
}

void CubeGL::setLegendMovable(bool on){
  moveLeg=on;
  updateGL();
}

void CubeGL::togglReSe(bool on){
  reSe=on;
  if (!reSe) {
    if (ibas) glDeleteLists(ibas,1);
    extern molekul mol;
    mol.firstHL=0;
    mol.lastHL=0;
    ibas=0;
    iSel=0;
    updateGL();
  }
}

void CubeGL::homeLabels(){
  extern QList<INP> xdinp;
  for (int j=0;j<xdinp.size();j++){
    xdinp[j].labPos=xdinp[j].kart;               
  }
  updateGL();
}

void  CubeGL::togglBGG(bool on){
  back_grad=on;
  updateGL();
}

void  CubeGL::setBond(bool on){
  drawBo=on;  
  updateGL();
}

void  CubeGL::setNoBond(bool on){
  drawBo=!on;  
  updateGL();
}

void  CubeGL::setEllipsoidNoUpdate(bool on){
  elli=on;  
}

void  CubeGL::setEllipsoid(bool on){
  elli=on;  
  updateGL();
}

Connection bondList;
void  CubeGL::toggInvEdit(bool on){
  invEditAble=on;   
  if (on) {
  extern molekul mol;
  extern QList<INP> xdinp;
  extern QList<INP> asymmUnit;
  static char PSE_Symbol[109][3] = {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar",
    "K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr",
    "Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe",
    "Cs","Ba", "La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu",
    "Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra",
    "Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No","Lr","Ku","Ha","Rf","Ns","Hs","Mt"};
  for (int ix=0;ix<asymmUnit.size();ix++){
    MyAtom invom;
    double soll_abst,gg;
    MyBond bond;
    if (ce.size()==0){
      for (int i=0;i<xdinp.size();i++)
	if (xdinp[i].OrdZahl>-1) {
	  invom.Label=xdinp[i].atomname;
	  invom.pos=xdinp[i].kart;
	  invom.an=xdinp[i].OrdZahl;
	  invom.part=xdinp[i].part;
	  invom.Symbol=PSE_Symbol[xdinp[i].OrdZahl];
	  invom.sg=xdinp[i].sg;
	  invom.index=i;
	  ce.append(invom);
	}
    }
    if ((bondList.size()!=0)) {
      cl=bondList;
    }
    else {
      for (int i=0;i<ce.size();i++) {
	for (int j=i+1;j<ce.size();j++){
	  if ((ce.at(i).an<0)||(ce.at(j).an<0)) continue;
	  if (ce.at(i).pos==ce.at(j).pos) continue;
	  if ((ce.at(i).part>0)&&(ce.at(j).part>0)&&(ce.at(j).part!=ce.at(i).part)) continue;
	  if (((ce.at(i).part<0)||(ce.at(j).part<0))&&(ce.at(j).part!=ce.at(i).part)) continue;
	  if (((ce.at(i).part<0)||(ce.at(j).part<0))&&(ce.at(j).sg!=ce.at(i).sg)) continue;
	  if ((ce.at(i).an<83)&&(ce.at(j).an<83)&&(ce.at(i).an>=0)&&(ce.at(j).an>=0)){
	    soll_abst=((mol.Kovalenz_Radien[ce.at(i).an]+
				    mol.Kovalenz_Radien[ce.at(j).an])/100.0
			    -(0.08*fabs(((double)mol.ElNeg[ce.at(i).an]
						    -mol.ElNeg[ce.at(j).an])/100.0)));
	    gg=sqrt( Distance(ce.at(i).pos,ce.at(j).pos));
	    if (gg<soll_abst*1.2) {
              printf("O%g %s %s\n",gg,ce.at(i).Label.toStdString().c_str(),ce.at(j).Label.toStdString().c_str());
	      bond.ato1=&ce.at(i);
	      bond.ato2=&ce.at(j);
	      bond.length=gg;
	      bond.chi=soll_abst-gg;
	      bond.order=(bond.chi<0.0847)?1:((bond.chi<0.184)?2:((bond.chi<0.27)?3:4));
	      bond.order=((bond.ato1->Symbol=="H")||(bond.ato2->Symbol=="H"))?1:bond.order;
	      cl.append(bond);
	    }
	  }
	}
      }	
    }
    if (bondList.size()!=cl.size()) {	    
      bondList=cl;
    }
    CEnvironment sel;
    QString ina;
//    printf("ix %d \n",ix);
    invom.Label=xdinp[ix].atomname;
    invom.pos=xdinp[ix].kart;
    invom.an=xdinp[ix].OrdZahl;
    invom.part=xdinp[ix].part;
    invom.Symbol=PSE_Symbol[xdinp[ix].OrdZahl];
    if (invom.an>-1 ){
      ina=inames::invName(invom,bondList,sel,ix);
    }
  }
  }
printf("ok\n");
}

void CubeGL::toggXDSetup(bool on){
  xdSetupMode=on;

//  printf("ist %s\n",(on)?"":"nicht");
}

void  CubeGL::setLabels(bool on){
  drawLa=on;  
  updateGL();
}

void  CubeGL::setAxes(bool on){
  drawAx=on;  
  updateGL();
}
/*
void CubeGL::setHelices(bool on){
  Luftschlange=on;
  updateGL();
}
*/
void  CubeGL::setUnitCell(bool on){
  drawUz=on;  
  updateGL();
}

void  CubeGL::setHydrogenBond(bool on){
  drawHb=on;  
  updateGL();
}

void CubeGL::setupTexture(){
  extern molekul mol;
  deleteTexture(mol.adpwall);
  deleteTexture(mol.hbtex);

  mol.adpwall=bindTexture(QImage(QString(":/images/adpwall.png")),GL_TEXTURE_2D);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  mol.hbtex=bindTexture(QImage(QString(":/images/hbb.png")),GL_TEXTURE_2D);  
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  
}

void CubeGL::changeBColor() {
  qreal bgRed,bgGreen,bgBlue,bgAlpha;
#if (QT_VERSION >= 0x040500)
  QColor bgcolor = QColorDialog::getColor( QColor((int)(bgCR*255),(int)(bgCG*255),(int)(bgCB*255),(int)(bgCA*255)), this,"Change Background Color",QColorDialog::DontUseNativeDialog);
#else
  QColor bgcolor = QColorDialog::getColor( QColor((int)(bgCR*255),(int)(bgCG*255),(int)(bgCB*255),(int)(bgCA*255)), this);
#endif
  if (bgcolor.isValid()) {
    bgcolor.getRgbF(&bgRed,&bgGreen,&bgBlue,&bgAlpha);
    bgCR=(GLclampf)bgRed;
    bgCG=(GLclampf)bgGreen;
    bgCB=(GLclampf)bgBlue;
    bgCA=(GLclampf)bgAlpha;
  }
  GLfloat fgc[4]={bgCR,bgCG,bgCB,1.0};
  glFogfv(GL_FOG_COLOR,fgc);
}

void CubeGL::changeBondColor(){
  extern molekul mol;
#if (QT_VERSION >= 0x040500)
  mol.bondColor=QColorDialog::getColor(mol.bondColor,this,"Change Bond Color",QColorDialog::DontUseNativeDialog);
#else
  mol.bondColor=QColorDialog::getColor(mol.bondColor,this);
#endif
  updateGL();
}

void CubeGL::changeTColor() {
  qreal bgRed,bgGreen,bgBlue,bgAlpha;
#if (QT_VERSION >= 0x040500)
  QColor tcolor = QColorDialog::getColor(QColor((int)(tCR*255),(int)(tCG*255),(int)(tCB*255),(int)(tCA*255)), this,"Change Label Color",QColorDialog::DontUseNativeDialog);
#else
  QColor tcolor = QColorDialog::getColor(QColor((int)(tCR*255),(int)(tCG*255),(int)(tCB*255),(int)(tCA*255)), this);
#endif
  if (tcolor.isValid()) {
    tcolor.getRgbF(&bgRed,&bgGreen,&bgBlue,&bgAlpha);
    tCR=(GLclampf)bgRed;
    tCG=(GLclampf)bgGreen;
    tCB=(GLclampf)bgBlue;
    tCA=(GLclampf)bgAlpha;
  }
}

void CubeGL::initializeGL() {
  moving->stop();
  printf("initializeGL\n");
  glEnable(GL_LINE_SMOOTH);   
  glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
  const GLfloat  position[] = {100.0f, 100.0f,100.0f,0.0f};
  const GLfloat  diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
  const GLfloat  specular[] = { 1.0, 1.0, 1.0, 1.0 };
  const GLfloat  ambient[]  = { 0.5, 0.5, 0.5, 1.0 };

  glLightModeli(  GL_LIGHT_MODEL_LOCAL_VIEWER, 1 );

  glLightfv( GL_LIGHT0, GL_POSITION, position );
  glLightfv( GL_LIGHT0, GL_AMBIENT,  ambient );
  glLightfv( GL_LIGHT0, GL_DIFFUSE,  diffuse );
  glLightfv( GL_LIGHT0, GL_SPECULAR, specular );

  glLightfv( GL_LIGHT1, GL_POSITION, position );
  glLightfv( GL_LIGHT1, GL_DIFFUSE,  diffuse );  
  glLightfv( GL_LIGHT1, GL_AMBIENT,  ambient );

  glLightfv( GL_LIGHT2, GL_DIFFUSE,  diffuse );  

  glFogi(GL_FOG_MODE,GL_LINEAR);
  glFogi(GL_FOG_START,140);
  glFogi(GL_FOG_END,260);
  GLfloat fgc[4]={bgCR,bgCG,bgCB,0.0};
  glFogfv(GL_FOG_COLOR,fgc);
  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );
  glEnable( GL_BLEND);
  glLightModeli(  GL_LIGHT_MODEL_TWO_SIDE, 1);
  glAlphaFunc ( GL_GREATER, 0.01f ) ;
  glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) ;
  const GLfloat  OBJ_SPE[]   = { 0.8, 0.8, 0.8, 1.0 };  
  const GLfloat  OBJ_SHIN    = 127.0;               
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,             OBJ_SPE  );
  glEnable     ( GL_COLOR_MATERIAL ) ;
  glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;    
  glMaterialf(  GL_FRONT_AND_BACK, GL_SHININESS,           OBJ_SHIN );
  glShadeModel( GL_SMOOTH );
  glEnable(GL_NORMALIZE);
  glClearColor(bgCR,bgCG,bgCB,bgCA);

   glEnable(GL_DEPTH_TEST );
   glDepthFunc(GL_LEQUAL);
   glEnable(GL_CULL_FACE);
   gluLookAt(0.0, 200, 50 ,   0.0, 0.0, 0.0,   0.0, -100, 400 );
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
   setMouseTracking(true); 

  extern QList<INP> xdinp;
  extern molekul mol;
  setupTexture();
  //888
    glLoadMatrixd(MM);
   if ((paparazi)&&(moliso)&&(moliso->mibas)){
      moliso->loadMI(moliso->faceFile,false,negpdf); 
   }
//  printf("initializeGL %d\n",__LINE__);
  
//    glLoadMatrixd(MM);
  if (0&&!xdinp.isEmpty()) {
    int adpstate=mol.adp;

      glNewList(bas, GL_COMPILE );{                          //ATOME
	glPushMatrix();{
	  glScaled( L, L, L );
	  mol.intern=1;
	  mol.adp=1;
	  mol.atoms(xdinp,mol.proba);
	}glPopMatrix();    
      }glEndList();
      

bool before=  mol.bondColorStyle;
  glNewList(bas+8, GL_COMPILE );{       //bonds in single color
    glPushMatrix();{
      glScaled( L, L, L );
      mol.intern=1;

      mol.bondColorStyle=true;
      mol.bonds(xdinp);
    }glPopMatrix();
  }glEndList();
  mol.bondColorStyle=before;


      glNewList(bas+7, GL_COMPILE );{                          //ATOME
	glPushMatrix();{
	  glScaled( L, L, L );
	  mol.intern=0;
	  mol.adp=1;
	  mol.atoms(xdinp,mol.proba);
	}glPopMatrix();    
      }glEndList();
      glNewList(bas+4, GL_COMPILE );{                          //ATOME
	glPushMatrix();{
	  glScaled( L, L, L );
	  mol.adp=0;
	  mol.atoms(xdinp,mol.proba);
	}glPopMatrix();    
      }glEndList();

      before=mol.tubifiedAtoms;
  glNewList(bas+9, GL_COMPILE );{       //Atome fuer tube syle
    glPushMatrix();{
      glScaled( L, L, L );
      mol.intern=1;

       mol.tubifiedAtoms=true;
      mol.atoms(xdinp,mol.proba);
    }glPopMatrix();
  }glEndList();
  mol.tubifiedAtoms=before;


      glNewList(bas+1, GL_COMPILE );{                          //BONDS
	glPushMatrix();{
	  glScaled( L, L, L );
	  mol.adp=0;      
	  mol.bonds(xdinp);
	}glPopMatrix();    
      }glEndList();

      if (drawAx) {
	glNewList(bas+2, GL_COMPILE );{                          //Axen

	  glDisable( GL_DEPTH_TEST ); 
	  glPushMatrix();{

	    glScaled( L, L, L );
	    mol.axes(xdinp); 
	  }glPopMatrix();    
	  glEnable( GL_DEPTH_TEST );
	}glEndList();
      }


      glNewList(bas+3, GL_COMPILE );{                          //Unit Zell
	glPushMatrix();{

	  glScaled( L, L, L );           
	  mol.UnitZell(); 
	}glPopMatrix();    
      }glEndList();

      if (mol.HAMax!=0.0){
	glNewList(bas+5, GL_COMPILE );{                          //H...X-Bindungen
	  glPushMatrix();{

	    glScaled( L, L, L );
	    mol.h_bonds(xdinp); 
	  }glPopMatrix();    
	}glEndList();
      }


      /*if (mol.nListe>2){
	glNewList(bas+6, GL_COMPILE );{        //helices
	  glPushMatrix();
	  glScaled( L, L, L );
	  mol.drawSline(mol.vL,mol.nListe);
	  glPopMatrix();
	}glEndList();
      }//*/
    if (iSel){
      if (ibas) glDeleteLists(ibas,1);
      ibas=glGenLists(1);    
      glNewList(ibas, GL_COMPILE );
      extern QList<INP> xdinp;
      extern molekul mol;
      mol.intern=1;
       mol.highlightResi(xdinp,mol.firstHL,L,elli);
      glEndList();	    
    }
    if (cbas){
      glDeleteLists(cbas,1);
      glNewList(cbas, GL_COMPILE );
      glPushMatrix();
      glScaled( L, L, L );
      mol.cbonds(xdinp);
      glPopMatrix();
      glEndList();
    }
      mol.adp=adpstate;      ;
  }

 // printf("initializeGL %d\n",__LINE__);
  if (foubas[0]|foubas[1]|foubas[2]|foubas[3]|foubas[4]){
      emit inimibas();
  }//else printf("dountinit\n");
 // printf("initializeGL %d\n",__LINE__);
  if (!noWaitLabel) moving->start(80);

}

void CubeGL::callList(int list){
//  printf("%d--%d %d\n",__LINE__,list,paparazi);
  extern QList<INP> xdinp;
  extern molekul mol;
    if ((paparazi)&&(moliso)&&(glIsList(moliso->mibas))){
      if (list>=moliso->mibas){glCallList(list);
     // printf("Call List #%d %d\n",list,paparazi);
      }
    }

  int isobase=((moliso)&&(moliso->mibas)&&(glIsList(moliso->mibas)))?list-moliso->mibas:-666;
  if (isobase!=-666){
  //  moliso->loadMI(moliso->faceFile,false,negpdf); 
  
    if (isobase>=0) {
    glCallList(list);
   // printf("call List #%d %d\n",list,paparazi);
  }
  }
  //int foubase=(foubas[0]|foubas[1]|foubas[2]|foubas[3]|foubas[4]);
    int molbase=list-bas;
  if ((paparazi)&&(!xdinp.isEmpty())) {
    int adpstate=mol.adp;
//printf("molbas %d bas %d\n",molbase,bas);

    if ((bas>0)&&(molbase<10)){
      switch (molbase){
        case 0: { //ATOMS
                  glPushMatrix();{
                    glScaled( L, L, L );
                    mol.intern=1;
                    mol.adp=1;
                    mol.atoms(xdinp,mol.proba);
                  }glPopMatrix();    
                }
                break; 

        case 1: {  //BONDS
                  glPushMatrix();{
                    glScaled( L, L, L );
                    mol.adp=0;      
                    mol.bonds(xdinp);
                  }glPopMatrix();    
                }
                break;
        case 2: //local axis
                if (drawAx) {
                  glDisable( GL_DEPTH_TEST ); 
                  glPushMatrix();{

                    glScaled( L, L, L );
                    mol.axes(xdinp); 
                  }glPopMatrix();    
                  glEnable( GL_DEPTH_TEST );
                }
                break;
        case 3: { //unit cell    
                  glPushMatrix();{
                    glScaled( L, L, L );           
                    mol.UnitZell(); 
                  }glPopMatrix();    
                }
                break;
        case 4: {                          //ATOME
                  glPushMatrix();{
                    glScaled( L, L, L );
                    mol.adp=0;
                    mol.atoms(xdinp,mol.proba);
                  }glPopMatrix();    
                }
                break;
        case 5:

                if (mol.HAMax!=0.0){
                  glPushMatrix();{

                    glScaled( L, L, L );
                    mol.h_bonds(xdinp); 
                  }glPopMatrix();    
                }
                break;
        case 7:{
                 bool before=  mol.bondColorStyle;
                 glPushMatrix();{
                   glScaled( L, L, L );
                   mol.intern=0;
                   mol.adp=1;
                   mol.atoms(xdinp,mol.proba);
                 }glPopMatrix();    
                 mol.bondColorStyle=before;
               }
               break;
        case 8:{


                 bool before=  mol.bondColorStyle;
                 glPushMatrix();{
                   glScaled( L, L, L );
                   mol.intern=1;

                   mol.bondColorStyle=true;
                   mol.bonds(xdinp);
                 }glPopMatrix();
                 mol.bondColorStyle=before;
               }
               break;
        case 9:{
                 bool before=mol.tubifiedAtoms;
                 glPushMatrix();{
                   glScaled( L, L, L );
                   mol.intern=1;

                   mol.tubifiedAtoms=true;
                   mol.atoms(xdinp,mol.proba);
                 }glPopMatrix();
                 mol.tubifiedAtoms=before;
               }
               break;
      }

    mol.adp=adpstate;


      /*if (mol.nListe>2){
        glNewList(bas+6, GL_COMPILE );{        //helices
        glPushMatrix();
        glScaled( L, L, L );
        mol.drawSline(mol.vL,mol.nListe);
        glPopMatrix();
        }glEndList();
        }// */
      /*
         if (iSel){
         if (ibas) glDeleteLists(ibas,1);
         ibas=glGenLists(1);    
         glNewList(ibas, GL_COMPILE );
         extern QList<INP> xdinp;
         extern molekul mol;
         mol.intern=1;
         mol.highlightResi(xdinp,mol.firstHL,L,elli);
         glEndList();	    
         }
         if (cbas){
         glDeleteLists(cbas,1);
         glNewList(cbas, GL_COMPILE );
         glPushMatrix();
         glScaled( L, L, L );
         mol.cbonds(xdinp);
         glPopMatrix();
         glEndList();
         }
         mol.adp=adpstate;      ;
         }

         if (foubas[0]|foubas[1]|foubas[2]|foubas[3]|foubas[4]){
         emit inimibas();
         }//else printf("dountinit\n");  */
  }
}else
    if ((bas>0)&&(molbase<10)){
      glCallList(list);
 //     printf("molbas %d bas %d list %d  paparazi %d\n",molbase,bas,list,paparazi);
    }
}

void CubeGL::resizeGL(int width, int height) {
  glViewport(0, 0, _win_width=width, _win_height=height);        
  glGetIntegerv(GL_VIEWPORT, vp);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective( vangle, (double)_win_width/_win_height, 5.0, 8000.0 );

  if (!noWaitLabel) moving->start(80);
}

inline void __RotateCS( double c, double s, double& X, double& Y ) {
  double T = X;
  X = c*X - s*Y;
  Y = s*T + c*Y;
}

void glTranslateL( const double dx, const double dy, const double dz ) {
  double mat[4][4];

  glGetDoublev( GL_MODELVIEW_MATRIX, (double*)mat );
  mat[3][0] += dx;  mat[3][1] += dy;  mat[3][2] += dz;
  glLoadMatrixd((double*)mat);
}

void glRotateL( const double dang, const double x, const double y, const double z ) {
  double mat[4][4];
#ifndef M_PI
#define	M_PI		3.14159265358979323846	/* pi */
#endif

  double s = z;
  s = sin(dang*M_PI/180);
  const double c = cos(dang*M_PI/180);
  glGetDoublev( GL_MODELVIEW_MATRIX, (double*)mat );


  if( x!=0.0 ){
    __RotateCS( c, s, mat[0][1], mat[0][2] );
    __RotateCS( c, s, mat[1][1], mat[1][2] );
    __RotateCS( c, s, mat[2][1], mat[2][2] );
  }else if( y!=0.0 ){
    __RotateCS( c, s, mat[0][2], mat[0][0] );
    __RotateCS( c, s, mat[1][2], mat[1][0] );
    __RotateCS( c, s, mat[2][2], mat[2][0] );
  }else{
    __RotateCS( c, s, mat[0][0], mat[0][1] );
    __RotateCS( c, s, mat[1][0], mat[1][1] );
    __RotateCS( c, s, mat[2][0], mat[2][1] );
  }


  glLoadMatrixd((double*)mat); 

}

void CubeGL::paintGL() {
//  
    int minussign=(minus->isChecked ())?1:-1;
    glClearColor(bgCR,bgCG,bgCB,bgCA);
    GLfloat fgc[4]={bgCR,bgCG,bgCB,0.0};
    glFogfv(GL_FOG_COLOR,fgc);
    glViewport(0, 0, _win_width, _win_height);        
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, _win_width, _win_height);        
    glDisable(GL_STENCIL_TEST);
  if (!stereo_mode){ 
    glGetIntegerv(GL_VIEWPORT, vp);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( vangle, (double)_win_width/_win_height, 5.0, 8000.0 );
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    draw();
    glPopMatrix();
  }else
  if (stereo_mode==1){// stereo zalman
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);

    glPushAttrib(GL_ENABLE_BIT);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0,viewport[2],0,viewport[3],-10.0,10.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.33F,0.33F,0.0F); 
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_NORMALIZE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_DITHER);
    glDisable(GL_BLEND);
    glShadeModel(GL_SMOOTH);
    glDisable(0x809D); // GL_MULTISAMPLE_ARB * / 

    glClearStencil(0);
    glColorMask(false,false,false,false);
    glDepthMask(false);
    glClear(GL_STENCIL_BUFFER_BIT);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glLineWidth(1.0);
    glBegin(GL_LINES);
    int h = viewport[3], w=viewport[2];
    int y;
     for(y=0;y<h;y+=2) {
      glVertex2i(0,y);
      glVertex2i(w,y);
    }
    glEnd();

    glColorMask(true,true,true,true);
    glDepthMask(true);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    //
    glPopAttrib();
  
// */
    glClearColor(bgCR,bgCG,bgCB,bgCA);
    glViewport(0, 0, _win_width, _win_height);        
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, _win_width, _win_height);        
    glGetIntegerv(GL_VIEWPORT, vp);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( vangle, (double)_win_width/_win_height, 5.0, 8000.0 );
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    bool warLabel = drawLa;
    drawLa=false;
    glRotateL(minussign*-1.5,0,1,0);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glEnable(GL_STENCIL_TEST);// */
    draw();
    glPopMatrix();
    drawLa= warLabel;
    glPushMatrix();
    glRotateL(minussign*1.5,0,1,0);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 0, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glEnable(GL_STENCIL_TEST);// */
    draw();
    glPopMatrix();
  }else  
    if (stereo_mode>1) { //stereo_side by side
      glPushMatrix();
      glViewport(0, 0, _win_width/2, _win_height);        
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective( vangle, (double)(_win_width/2.0)/_win_height, 5.0, 8000.0 );
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glRotateL(1.5*minussign,0,1,0);
      draw();
      glPopMatrix();
      glPopMatrix();

      glPushMatrix();
      glViewport( _win_width / 2 , 0,_win_width / 2,_win_height );        
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective( vangle, (double)(_win_width/2.0)/_win_height, 5.0, 8000.0 );
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glRotateL(-1.5*minussign,0,1,0);
      draw();
      glPopMatrix();
      glPopMatrix();
    }
}

void CubeGL::loadDataBase(){
  QString fileName = QFileDialog::getOpenFileName(this,QString(tr("Open invariom data base.")), "DABA.txt","All files (*)" ,&selectedFilter,QFileDialog::DontUseNativeDialog );
  if (!fileName.isEmpty()){
    
    
    QDateTime zeit=QFileInfo(fileName).lastModified(); 
    Istda->setText(QString("Data base loaded was modified %1 days ago").arg(zeit.daysTo(QDateTime::currentDateTime ())));
    QFile daba(fileName);
    daba.open(QIODevice::ReadOnly);

    DABA entry;

    QString line;
    int lineCntr=-1;
    while (!daba.atEnd()){
      line = QString(daba.readLine(150));
      if (line.contains(QRegExp("^R-|S-|=-|[3-8]{1,3}-|[A-Z]{1,1}[a-z]{0,1}[123#@]{0,1}"))) {
        if ((!line.startsWith("KS"))&&(!line.startsWith("!"))&&(!line.contains(QRegExp("[;:]")))) {
          lineCntr=0;
          line.remove(QRegExp("[ \n\r]"));
          dataBase.append(line);
          }
      }
      if ((lineCntr>-1)&&(lineCntr<7)){
      line.remove(QRegExp("[\n\r]"));
        QStringList tok=line.split(" ",QString::SkipEmptyParts);
        switch(lineCntr){
        case 1: if (tok.size()>9) {
             entry.m0=tok.at(0).toDouble();
             entry.m1=tok.at(1).toDouble();
             entry.d1p=tok.at(2).toDouble();
             entry.d1m=tok.at(3).toDouble();
             entry.d0=tok.at(4).toDouble();
             entry.q0=tok.at(5).toDouble();
             entry.q1p=tok.at(6).toDouble();
             entry.q1m=tok.at(7).toDouble();
             entry.q2p=tok.at(8).toDouble();
             entry.q2m=tok.at(9).toDouble();

              } break;
        case 2: if (tok.size()>9) {
             entry.o0=tok.at(0).toDouble();
             entry.o1p=tok.at(1).toDouble();
             entry.o1m=tok.at(2).toDouble();
             entry.o2p=tok.at(3).toDouble();
             entry.o2m=tok.at(4).toDouble();
             entry.o3p=tok.at(5).toDouble();
             entry.o3m=tok.at(6).toDouble();
             entry.h0=tok.at(7).toDouble();
             entry.h1p=tok.at(8).toDouble();
             entry.h1m=tok.at(9).toDouble();

              } break;
        case 3: if (tok.size()>5) {
             entry.h2p=tok.at(0).toDouble();
             entry.h2m=tok.at(1).toDouble();
             entry.h3p=tok.at(2).toDouble();
             entry.h3m=tok.at(3).toDouble();
             entry.h4p=tok.at(4).toDouble();
             entry.h4m=tok.at(5).toDouble();

              } break;
        case 4: entry.Symmetry=tok.at(1).trimmed(); break;
        case 5: entry.CoordinateSystem=line; break;
        case 6: line.remove("Kappa=");
		line.remove(";");
                tok = line.split("=",QString::SkipEmptyParts);
                entry.k1=tok.at(0).toDouble();
                entry.k2=tok.at(1).toDouble();
                entry.k3=tok.at(2).toDouble();
                entry.k4=tok.at(3).toDouble();
                entry.k5=tok.at(4).toDouble();
                entry.k6=tok.at(5).toDouble();
                entries.append(entry);
             break;
        }
        lineCntr++;
      }

    }

  }
}

void CubeGL::loadDataBase(QString fileName){
  if (!fileName.isEmpty()){
    QDateTime zeit=QFileInfo(fileName).lastModified(); 
    Istda->setText(QString("Data base loaded was modified %1 days ago").arg(zeit.daysTo(QDateTime::currentDateTime ())));
    QFile daba(fileName);
    daba.open(QIODevice::ReadOnly);
    DABA entry;

    QString line;
    int lineCntr=-1;
    while (!daba.atEnd()){
      line = QString(daba.readLine(150));
      if (line.contains(QRegExp("^R-|S-|=-|[3-8]{1,3}-|[A-Z]{1,1}[a-z]{0,1}[123#@]{0,1}"))) {
        if ((!line.startsWith("KS"))&&(!line.startsWith("!"))&&(!line.contains(QRegExp("[;:]")))) {
          lineCntr=0;
          line.remove(QRegExp("[ \n\r]"));
          dataBase.append(line);
          }
      }
      if ((lineCntr>-1)&&(lineCntr<7)){
      line.remove(QRegExp("[\n\r]"));
        QStringList tok=line.split(" ",QString::SkipEmptyParts);
        switch(lineCntr){
        case 1: if (tok.size()>9) {
             entry.m0=tok.at(0).toDouble();
             entry.m1=tok.at(1).toDouble();
             entry.d1p=tok.at(2).toDouble();
             entry.d1m=tok.at(3).toDouble();
             entry.d0=tok.at(4).toDouble();
             entry.q0=tok.at(5).toDouble();
             entry.q1p=tok.at(6).toDouble();
             entry.q1m=tok.at(7).toDouble();
             entry.q2p=tok.at(8).toDouble();
             entry.q2m=tok.at(9).toDouble();

              } break;
        case 2: if (tok.size()>9) {
             entry.o0=tok.at(0).toDouble();
             entry.o1p=tok.at(1).toDouble();
             entry.o1m=tok.at(2).toDouble();
             entry.o2p=tok.at(3).toDouble();
             entry.o2m=tok.at(4).toDouble();
             entry.o3p=tok.at(5).toDouble();
             entry.o3m=tok.at(6).toDouble();
             entry.h0=tok.at(7).toDouble();
             entry.h1p=tok.at(8).toDouble();
             entry.h1m=tok.at(9).toDouble();

              } break;
        case 3: if (tok.size()>5) {
             entry.h2p=tok.at(0).toDouble();
             entry.h2m=tok.at(1).toDouble();
             entry.h3p=tok.at(2).toDouble();
             entry.h3m=tok.at(3).toDouble();
             entry.h4p=tok.at(4).toDouble();
             entry.h4m=tok.at(5).toDouble();

              } break;
        case 4: entry.Symmetry=tok.at(1).trimmed(); break;
        case 5: entry.CoordinateSystem=line; break;
        case 6: line.remove("Kappa=");
		line.remove(";");
                tok = line.split("=",QString::SkipEmptyParts);
                entry.k1=tok.at(0).toDouble();
                entry.k2=tok.at(1).toDouble();
                entry.k3=tok.at(2).toDouble();
                entry.k4=tok.at(3).toDouble();
                entry.k5=tok.at(4).toDouble();
                entry.k6=tok.at(5).toDouble();
                entries.append(entry);
             break;
        }
        lineCntr++;
      }

    }

  }
}

void CubeGL::resetENV(){
  ce.clear();
  bondList.clear();
  cl.clear();
  setMatrix();
  updateGL();
  setMatrix();
  updateGL();
}

void CubeGL::rotCenter(){
  rotze=-1;
  homeXY();
  updateGL();
  rCenter->setVisible(false);
}

void CubeGL::mousePressEvent(QMouseEvent *event) {
  double nahda=200.0,da=0;
  int nahdai=-1;
  extern QList<INP> xdinp;
  extern QList<Modulat> matoms;
  //for (int i=0; i<xdinp.size(); i++) printf("%-9s -->%d<--\n",xdinp.at(i).atomname,xdinp.at(i).molindex);
  for (int j=0; j<xdinp.size();j++){
    da=(((xdinp.at(j).screenX-event->x())*( xdinp.at(j).screenX-event->x()))+ 
		    ((xdinp.at(j).screenY-event->y())*( xdinp.at(j).screenY-event->y())));
    nahdai=(da<nahda)?j:nahdai;
    nahda=qMin(nahda,da);
  }
  for (int j=0; j<matoms.size();j++){
    da=(((matoms.at(j).screenX-event->x())*( matoms.at(j).screenX-event->x()))+ 
		    ((matoms.at(j).screenY-event->y())*( matoms.at(j).screenY-event->y())));
    nahdai=(da<nahda)?j:nahdai;
    nahda=qMin(nahda,da);
  }
  lastPos = event->pos();
  static GLuint ppp=0,pp=0,p=0;   
  molekul m;	
  if (event->buttons() & Qt::MidButton){
    if (nahdai<xdinp.size()) {
      GLuint index=nahdai;
      if (index==((GLuint)-1))return;
      rotze=((int)index<xdinp.size())?index:-1;  
      if (rotze>-1) rCenter->setVisible(true);
      homeXY();
      updateGL();

    }
    if (nahdai<matoms.size()) {
      GLuint index=nahdai;
      if (index==((GLuint)-1))return;
      rotze=((int)index<matoms.size())?index:-1;  
      if (rotze>-1) rCenter->setVisible(true);
      homeXY();
      updateGL();

    }
  }
  if((reSe | moveLab| invEditAble | atomsClickable| xdSetupMode) && (event->buttons() & Qt::LeftButton)){

    if (nahdai<xdinp.size()) {
//      extern QList<INP> xdinp;
      extern molekul mol;
	GLuint index=nahdai;
	if (index==((GLuint)-1))return;
	double w=0,dw=0;
	if (((GLuint) ppp)>((GLuint)xdinp.size())) {
	  ppp=pp;
	  pp=p;
	  p=index;
	}
	if (((GLuint) pp)>((GLuint)xdinp.size())) {
	  ppp=index;
	  pp=p;
	  p=index;
	}
	if (((GLuint) p)>((GLuint)xdinp.size())) {
	  ppp=pp;
	  pp=index;
	  p=index;
	}
	if ((pp!=p)&&(pp!=index)&&(p!=index)) {
	  w=m.winkel(xdinp[p].kart-xdinp[pp].kart,
			  xdinp[p].kart-xdinp[index].kart);
	  if ((ppp!=p)&&(ppp!=pp)&&(ppp!=index))
	    dw=m.dieder(xdinp[pp].kart-xdinp[ppp].kart,
			    xdinp[pp].kart-xdinp[p].kart,
			    xdinp[p].kart-xdinp[index].kart);
	}
	if ((reSe)&&(strchr(xdinp[index].atomname,'@'))){
	  iSel=1;
	  {
	    if (ibas) glDeleteLists(ibas,1);
	    ibas=glGenLists(1);    

	    glNewList(ibas, GL_COMPILE );
	    mol.highlightResi(xdinp,index,L,elli);
	    glEndList();	    
	  }
	  updateGL();
	}
	if (moveLab) {
	  labToMove=index;
	}
	if (atomsClickable){
	  if ((xdinp[index].atomname[0]=='Q')||
			  ((xdinp[index].atomname[0]=='H')&&(xdinp[index].atomname[1]=='L')) ||
			  ((xdinp[index].atomname[0]=='P')&&(xdinp[index].atomname[1]=='K'))){
	    emit bigmessage(QString("Geometry:<table><tr><td><b>%1</b></td><td align=\"left\"></td></tr><tr><td><b>%2--%3</b></td><td align=\"left\">%4 &Aring;</td></tr><tr><td><b>%5--%6--%7</b></td><td align=\"left\">%8&deg;</td></tr><tr><td><b>%9--%10--%11--%12 </b></td><td align=\"left\">%13&deg;</td></tr><tr><td>PART</td><td align=\"left\">%14</td></tr><tr><td>Peak Height: </td><td align=\"left\">%15 [%16-%17]</td></tr><table>")
			    .arg((xdinp[index].atomname))                         //1
			    .arg((xdinp[index].atomname))			        //2
			    .arg((xdinp[p].atomname))						//3
			    .arg(sqrt(Distance(xdinp[index].kart,xdinp[p].kart)),0,'f',5) //4
			    .arg((xdinp[pp].atomname))			                        //5
			    .arg((xdinp[p].atomname))			                        //6
			    .arg((xdinp[index].atomname))			        //7
			    .arg(w,0,'f',3)			                                                //8
			    .arg((xdinp[ppp].atomname))			                        //9
			    .arg((xdinp[pp].atomname))			                        //10
			    .arg((xdinp[p].atomname))			                        //11
			    .arg((xdinp[index].atomname))			        //12
			    .arg(dw,0,'f',3)
			    .arg(xdinp[index].part)
			    .arg(xdinp[index].peakHeight,0,'f',3)
			    .arg(mol.pmin,0,'f',3)
			    .arg(mol.pmax,0,'f',3)
			   );
	  }else if  ((xdinp[index].atomname[0]=='C')&&(xdinp[index].atomname[1]=='P')) {
	    emit bigmessage(QString("Geometry:<table><tr><td><b>%1</b></td><td align=\"left\"></td></tr><tr><td><b>%2--%3</b></td><td align=\"left\">%4 &Aring;</td></tr><tr><td><b>%5--%6--%7</b></td><td align=\"left\">%8&deg;</td></tr><tr><td><b>%9--%10--%11--%12 </b></td><td align=\"left\">%13&deg;</td></tr><tr><td>PART</td><td align=\"left\">%14</td></tr><tr><td>Charge Density: </td><td align=\"left\">%15 e &sdot; &Aring;<sup>-3</sub></td></tr><table>")
			    .arg((xdinp[index].atomname))                         //1
			    .arg((xdinp[index].atomname))			        //2
			    .arg((xdinp[p].atomname))						//3
			    .arg(sqrt(Distance(xdinp[index].kart,xdinp[p].kart)),0,'f',5) //4
			    .arg((xdinp[pp].atomname))			                        //5
			    .arg((xdinp[p].atomname))			                        //6
			    .arg((xdinp[index].atomname))			        //7
			    .arg(w,0,'f',3)			                                                //8
			    .arg((xdinp[ppp].atomname))			                        //9
			    .arg((xdinp[pp].atomname))			                        //10
			    .arg((xdinp[p].atomname))			                        //11
			    .arg((xdinp[index].atomname))			        //12
			    .arg(dw,0,'f',3)
			    .arg(xdinp[index].part)
			    .arg(xdinp[index].peakHeight,0,'f',3));



	  }else {
	    emit bigmessage(QString("Geometry:<table><tr><td><b>%1</b> clicked.</td><td align=\"left\"></td></tr><tr><td><b>%2--%3</b></td><td align=\"left\">%4 &Aring;</td></tr><tr><td><b>%5--%6--%7</b></td><td align=\"left\">%8&deg;</td></tr><tr><td><b>%9--%10--%11--%12 </b></td><td align=\"left\">%13&deg;</td></tr><tr><td>PART</td><td align=\"left\">%15</td></tr> <tr><td>Fragment</td><td align=\"left\">%14</td></tr><table>")
			    .arg((xdinp[index].atomname))                         //1
			    .arg((xdinp[index].atomname))			        //2
			    .arg((xdinp[p].atomname))						//3
			    .arg(sqrt(Distance(xdinp[index].kart,xdinp[p].kart)),0,'f',5) //4
			    .arg((xdinp[pp].atomname))			                        //5
			    .arg((xdinp[p].atomname))			                        //6
			    .arg((xdinp[index].atomname))			        //7
			    .arg(w,0,'f',3)			                                                //8
			    .arg((xdinp[ppp].atomname))			                        //9
			    .arg((xdinp[pp].atomname))			                        //10
			    .arg((xdinp[p].atomname))			                        //11
			    .arg((xdinp[index].atomname))			        //12
			    .arg(dw,0,'f',3)
			    .arg(xdinp[index].molindex)
			    .arg(xdinp[index].part));
	  }
	  ppp=pp;
	  pp=p;
	  p=index;
/*	  if (event->modifiers()==Qt::NoModifier) {
	    selectedAtoms.clear();
	    selectedAtoms.append(xdinp[index]);
	    selectedAtoms.last().GLname=index;
	    updateGL();
	  }*/

	    if (event->modifiers()==Qt::NoModifier) {
	      int isschon=-1;
	      for (int i=0; i< selectedAtoms.size();i++){
		isschon=((GLuint)selectedAtoms.at(i).GLname==index)?index:isschon;
	      }
	      selectedAtoms.clear();
	      if (isschon==-1){
		selectedAtoms.append(xdinp[index]);
		selectedAtoms.last().GLname=index;
	      }
	    updateGL();
	    }


	  if (event->modifiers()==Qt::ShiftModifier){
	  GLuint min=xdinp.size(),max=0;
	  for (int i=0;i<selectedAtoms.size();i++) {
	    min=(selectedAtoms.at(i).GLname<min)?selectedAtoms.at(i).GLname:min;
	    max=(selectedAtoms.at(i).GLname>max)?selectedAtoms.at(i).GLname:max;
	  }
	    min=(index<min)?index:min;
	    max=(index>max)?index:max;
	    selectedAtoms.clear();
	    for (GLuint i=min; i<=max; i++){
	    selectedAtoms.append(xdinp[i]);
	    selectedAtoms.last().GLname=i;
	    }
	    updateGL();
	  }
	  if (event->modifiers()==Qt::ControlModifier){
	    selectedAtoms.append(xdinp[index]);
	    selectedAtoms.last().GLname=index;
	    updateGL();
	  }
	  updateBondActions();
	}
	//888
	if ((xdinp.size())&&((invEditAble)||(xdSetupMode))) {
//	  printf("Â¿%d %d %d\n",xdSetupMode,rename,xdinp[index].OrdZahl);
	  if ((!rename)&&(xdinp[index].OrdZahl<0))return;
	  if ((rename)&&(xdinp[index].OrdZahl==-1)) return;
	  if (xdinp[index].sg) return; 
//	  printf("!%d %d %d sg%d\n",xdSetupMode,rename,xdinp[index].OrdZahl,xdinp[index].sg);
	  int na=0;
	  for (int i=0; (i<xdinp.size())&&(xdinp[i].OrdZahl>=0); i++) na++;
	  //if (((int)index)>na) {
	  //  return;
	  //}
	  static char PSE_Symbol[109][3] = {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar",
	    "K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr",
	    "Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe",
	    "Cs","Ba", "La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu",
	    "Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra",
	    "Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No","Lr","Ku","Ha","Rf","Ns","Hs","Mt"};

	  MyAtom invom,orgAtom;
	  double soll_abst,gg;
	  MyBond bond;
	  if (ce.size()==0)
	    for (int i=0;i<xdinp.size();i++){
	      invom.Label=xdinp[i].atomname;
	      invom.pos=xdinp[i].kart;
	      invom.an=xdinp[i].OrdZahl;
	      invom.part=xdinp[i].part;
	      invom.sg=xdinp[i].sg;
	      invom.Symbol=PSE_Symbol[xdinp[i].OrdZahl];
	      invom.index=i;
	      ce.append(invom);
	    }
	  if ((bondList.size()!=0)) {
	    cl=bondList;
	  }
	  else {
	    for (int i=0;i<ce.size();i++) {
	      for (int j=i+1;j<ce.size();j++){
		if ((ce.at(i).an<0)||(ce.at(j).an<0)) continue;
		if (ce.at(i).pos==ce.at(j).pos) continue;
		if ((ce.at(i).part>0)&&(ce.at(j).part>0)&&(ce.at(j).part!=ce.at(i).part)) continue;
		if ((ce.at(i).part<0)&&(ce.at(j).part<0)&&(ce.at(j).part!=ce.at(i).part)) continue;
		if (((ce.at(i).part<0)||(ce.at(j).part<0))&&(ce.at(j).sg!=ce.at(i).sg)) continue;
		if ((ce.at(i).an<83)&&(ce.at(j).an<83)&&(ce.at(i).an>=0)&&(ce.at(j).an>=0)){
		  soll_abst=((mol.Kovalenz_Radien[ce.at(i).an]+
					  mol.Kovalenz_Radien[ce.at(j).an])/100.0
				  -(0.08*fabs(((double)mol.ElNeg[ce.at(i).an]
							  -mol.ElNeg[ce.at(j).an])/100.0)));
		  gg=sqrt( Distance(ce.at(i).pos,ce.at(j).pos));
		  if (gg<(soll_abst*1.2)) {
              printf("X%g %s %s\n",gg,ce.at(i).Label.toStdString().c_str(),ce.at(j).Label.toStdString().c_str());
		    bond.ato1=&ce.at(i);
		    bond.ato2=&ce.at(j);
		    bond.length=gg;
		    bond.chi=soll_abst-gg;
		    bond.order=(bond.chi<0.0847)?1:((bond.chi<0.184)?2:((bond.chi<0.27)?3:4));
		    bond.order=((bond.ato2->Symbol=="H")||(bond.ato1->Symbol=="H"))?1:bond.order;
		    cl.append(bond);
		  }
		}
	      }
	    }	
	  }
	  if (bondList.size()!=cl.size()) {	    
	    bondList=cl;
	  }
	  CEnvironment cel;
	  invom.Label=xdinp[index].atomname;
	  invom.pos=xdinp[index].kart;
	  invom.an=xdinp[index].OrdZahl;
	  invom.part=xdinp[index].part;
	  invom.sg=xdinp[index].sg;
	  invom.Symbol=PSE_Symbol[xdinp[index].OrdZahl];
          orgAtom=invom;

	  if (rename){
	    printf("ich rename\n");
	  invom.Label=rn.Label;
	  invom.an=rn.an;
	  invom.Symbol=PSE_Symbol[rn.an];
	  }
	  cel.append(invom);	  
	  for (int i=0;i<xdinp.size();i++){
	    if((xdinp[i].OrdZahl<0)||(invom.an<0)) continue;
	    if ((xdinp[i].part>0)&&(xdinp[index].part>0)&&(xdinp[i].part!=xdinp[index].part)) continue;
	    if ((xdinp[i].OrdZahl<83)&&(invom.an<83)&&(xdinp[i].OrdZahl>=0)&&(invom.an>=0)){
	      soll_abst=((mol.Kovalenz_Radien[xdinp[i].OrdZahl]+
				      mol.Kovalenz_Radien[invom.an])
			      -(0.08*fabs((double)mol.ElNeg[xdinp[i].OrdZahl]
					      -mol.ElNeg[invom.an])))*1.2;
	      gg=100.0*sqrt( Distance(xdinp[i].kart,xdinp[index].kart));
	      if ((gg<soll_abst)&&(!(xdinp[i].kart==cel[0].pos))){		
		invom.Label=xdinp[i].atomname;
		invom.pos=xdinp[i].kart;
		invom.part=xdinp[i].part;
		invom.an=xdinp[i].OrdZahl;
		invom.sg=xdinp[i].sg;
		invom.Symbol=PSE_Symbol[xdinp[i].OrdZahl];
		cel.append(invom);
	      }
	    }
	  }
	  int end=cel.size();
	  if (invEditAble){
	    int *isInList=(int*)malloc(sizeof(int)*xdinp.size());
	    for (int i=0;i<xdinp.size();i++){
	      isInList[i]=0;
	      for (int j=0; j<end;j++){
		if (cel[j].pos==xdinp[i].kart) isInList[i]=1;
	      }
	    }
	    for (int i=0;i<xdinp.size();i++){
	      for (int j=1; j<end;j++){
		if(xdinp[i].OrdZahl<0) continue;
		if (isInList[i]) continue;
		if ((xdinp[i].part>0)&&(cel.at(j).part>0)&&(cel.at(j).part!=xdinp[i].part)) continue;
		if ((xdinp[i].OrdZahl<83)&&(xdinp[i].OrdZahl>=0)){
		  soll_abst=((mol.Kovalenz_Radien[xdinp[i].OrdZahl]+
					  mol.Kovalenz_Radien[cel.at(j).an])
				  -(0.08*fabs((double)mol.ElNeg[xdinp[i].OrdZahl]
						  -mol.ElNeg[cel.at(j).an])))*1.2;
		  gg=100.0*sqrt( Distance(xdinp[i].kart,cel.at(j).pos));
		  if (gg<soll_abst){				  
		    invom.Label=xdinp[i].atomname;
		    invom.pos=xdinp[i].kart;
		    invom.part=xdinp[i].part;
		    invom.sg=xdinp[i].sg;
		    invom.an=xdinp[i].OrdZahl;
		    invom.Symbol=PSE_Symbol[xdinp[i].OrdZahl];
		    cel.append(invom);
		  }
		}
	      }
	    }
	    free(isInList);
	  }//inv
	  if (xdSetupMode){
	    for (int i=0;i<xdinp.size();i++){
	      if ((xdinp[i].OrdZahl<0)&&(12>Distance(cel.at(0).pos,xdinp[i].kart))){
		invom.Label=xdinp[i].atomname;
		invom.pos=xdinp[i].kart;
		invom.an=-1;
		invom.sg=xdinp[i].sg;
		invom.Symbol="X";
		if (xdinp[i].OrdZahl>-2)cel.append(invom);
	      }
	    }
	  }
	  Connection cll;


	  for (int i=0; i<cl.size();i++)
	    for (int k=0; k<cel.size();k++)
	      for (int j=0; j<cel.size();j++){
		if ((cl.at(i).ato1->pos==cel.at(j).pos)&&(cl.at(i).ato2->pos==cel.at(k).pos)){
		  if ((cel.at(0).part>0)&&((cel.at(0).part!=cl.at(i).ato1->part)||(cel.at(0).part!=cl.at(i).ato2->part))) continue;
		  if ((cel.at(k).part<0)&&(cel.at(j).part<0)&&(cel.at(j).part!=cel.at(k).part)) continue;
		  if (((cel.at(k).part<0)||(cel.at(j).part<0))&&(cel.at(j).sg!=cel.at(k).sg)) continue;
		  cll.append(cl[i]);
		}
	      }
	  if (invEditAble) {
            printf("invDlg %d\n",__LINE__);
	    invDlg *id=new invDlg(&cel,&cll,dataBase);//TESTXDEDIT
	    id->update();
	    if (id->exec()==QDialog::Rejected)
	      for (int i=0;i<bondList.size();i++){
		bondList[i].order=cl.at(i).order;
	      }
	  }
	  if (xdSetupMode){
	    CEnvironment all;	  
	    MyAtom xdall;
	    //	    printf
	    for (int i=0; i<xdinp.size(); i++) {//if (xdinp[i].OrdZahl>-2){
	      xdall.Label=xdinp[i].atomname;
	      xdall.pos=xdinp[i].kart;
	      xdall.an=xdinp[i].OrdZahl;
	      xdall.sg=xdinp[i].sg;
	      if (xdinp[i].OrdZahl>-1){
	      xdall.Symbol=PSE_Symbol[xdinp[i].OrdZahl];
	      }else xdall.Symbol="X";
	      all.append(xdall);

	    }
	    xdEditDlg *id=new xdEditDlg(&cel,&cll,&all,orgAtom);
	    id->update();
	    if (QDialog::Accepted==id->exec()) emit reloadXD() ;
	  }
	  setMatrix();
	  updateGL();
	  setMatrix();
	  updateGL();
	}	
      }
    if (nahdai<matoms.size()) {
//      extern QList<INP> xdinp;
   //   extern molekul mol;
	GLuint index=nahdai;
	if (index==((GLuint)-1))return;
	double w=0,dw=0;
	if (((GLuint) ppp)>((GLuint)matoms.size())) {
	  ppp=pp;
	  pp=p;
	  p=index;
	}
	if (((GLuint) pp)>((GLuint)matoms.size())) {
	  ppp=index;
	  pp=p;
	  p=index;
	}
	if (((GLuint) p)>((GLuint)matoms.size())) {
	  ppp=pp;
	  pp=index;
	  p=index;
	}
	if ((pp!=p)&&(pp!=index)&&(p!=index)) {
	  w=m.winkel(matoms[p].kart(tvalue)-matoms[pp].kart(tvalue),
			  matoms[p].kart(tvalue)-matoms[index].kart(tvalue));
	  if ((ppp!=p)&&(ppp!=pp)&&(ppp!=index))
	    dw=m.dieder(matoms[pp].kart(tvalue)-matoms[ppp].kart(tvalue),
			    matoms[pp].kart(tvalue)-matoms[p].kart(tvalue),
			    matoms[p].kart(tvalue)-matoms[index].kart(tvalue));
	}
	if (atomsClickable){
	  {
	    emit bigmessage(QString("Geometry:<table><tr><td><b>%1</b> clicked at t0 = %15.</td><td align=\"left\"></td></tr><tr><td><b>%2--%3</b></td><td align=\"left\">%4 &Aring;</td></tr><tr><td><b>%5--%6--%7</b></td><td align=\"left\">%8&deg;</td></tr><tr><td><b>%9--%10--%11--%12 </b></td><td align=\"left\">%13&deg;</td></tr><tr><td>Fragment</td><td align=\"left\">%14</td></tr><table>")
			    .arg((matoms[index].atomname))                         //1
			    .arg((matoms[index].atomname))			        //2
			    .arg((matoms[p].atomname))						//3
			    .arg(sqrt(Distance(matoms[index].kart(tvalue),matoms[p].kart(tvalue))),0,'f',5) //4
			    .arg((matoms[pp].atomname))			                        //5
			    .arg((matoms[p].atomname))			                        //6
			    .arg((matoms[index].atomname))			        //7
			    .arg(w,0,'f',3)			                                                //8
			    .arg((matoms[ppp].atomname))			                        //9
			    .arg((matoms[pp].atomname))			                        //10
			    .arg((matoms[p].atomname))			                        //11
			    .arg((matoms[index].atomname))			        //12
			    .arg(dw,0,'f',3)
			    .arg(matoms[index].molindex)
                            .arg(tvalue)
			    );
	  }
	  ppp=pp;
	  pp=p;
	  p=index;
/*	  if (event->modifiers()==Qt::NoModifier) {
	    selectedAtoms.clear();
	    selectedAtoms.append(matoms[index].toINP(tvalue));
	    selectedAtoms.last().GLname=index;
	    updateGL();
	  }*/
	    if (event->modifiers()==Qt::NoModifier) {
	      int isschon=-1;
	      for (int i=0; i< selectedAtoms.size();i++){
		isschon=((GLuint)selectedAtoms.at(i).GLname==index)?index:isschon;
	      }
	      selectedAtoms.clear();
	      if (isschon==-1){
		selectedAtoms.append(matoms[index].toINP(tvalue));
		selectedAtoms.last().GLname=index;
	      }
	    updateGL();
	    }
	  if (event->modifiers()==Qt::ShiftModifier){
	  GLuint min=matoms.size(),max=0;
	  for (int i=0;i<selectedAtoms.size();i++) {
	    min=(selectedAtoms.at(i).GLname<min)?selectedAtoms.at(i).GLname:min;
	    max=(selectedAtoms.at(i).GLname>max)?selectedAtoms.at(i).GLname:max;
	  }
	    min=(index<min)?index:min;
	    max=(index>max)?index:max;
	    selectedAtoms.clear();
	    for (GLuint i=min; i<=max; i++){
	    selectedAtoms.append(matoms[i].toINP(tvalue));
	    selectedAtoms.last().GLname=i;
	    }
	    updateGL();
	  }
	  if (event->modifiers()==Qt::ControlModifier){
	    selectedAtoms.append(matoms[index].toINP(tvalue));
	    selectedAtoms.last().GLname=index;
	    updateGL();
	  }
	  updateBondActions();
	}

      }
  // OOOOOOOOOOOOOOO //
    }
    printf("!\n");
}

/////////
void CubeGL::editInv(const QUrl & link ){
  int index=link.toString().toInt();  
  extern QList<INP> xdinp;
  extern molekul mol;
  if (xdinp[index].OrdZahl<0)return;
  int na=0;
  for (int i=0; (i<xdinp.size())&&(xdinp[i].OrdZahl>=0); i++) na=i;
  if (((int)index)>na) {
    return;
  }
  static char PSE_Symbol[109][3] = {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar",
    "K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr",
    "Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe",
    "Cs","Ba", "La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu",
    "Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra",
    "Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No","Lr","Ku","Ha","Rf","Ns","Hs","Mt"};

  MyAtom invom;
  double soll_abst,gg;
  MyBond bond;
  if (ce.size()==0)
    for (int i=0;i<xdinp.size();i++){
      invom.Label=xdinp[i].atomname;
      invom.pos=xdinp[i].kart;
      invom.an=xdinp[i].OrdZahl;
      invom.part=xdinp[i].part;
      invom.Symbol=PSE_Symbol[xdinp[i].OrdZahl];
      invom.index=i;
      ce.append(invom);
    }
  if ((bondList.size()!=0)) {
    cl=bondList;
  }
  else {
    for (int i=0;i<ce.size();i++) {
      for (int j=i+1;j<ce.size();j++){
	if ((ce.at(i).an<0)||(ce.at(j).an<0)) continue;
	if (ce.at(i).pos==ce.at(j).pos) continue;
	if ((ce.at(i).part>0)&&(ce.at(j).part>0)&&(ce.at(j).part!=ce.at(i).part)) continue;
	  if ((ce.at(i).part<0)&&(ce.at(j).part<0)&&(ce.at(j).part!=ce.at(i).part)) continue;
	  if (((ce.at(i).part<0)||(ce.at(j).part<0))&&(ce.at(j).sg!=ce.at(i).sg)) continue;
	if ((ce.at(i).an<83)&&(ce.at(j).an<83)&&(ce.at(i).an>=0)&&(ce.at(j).an>=0)){
	  soll_abst=((mol.Kovalenz_Radien[ce.at(i).an]+
				  mol.Kovalenz_Radien[ce.at(j).an])/100.0
			  -(0.08*fabs(((double)mol.ElNeg[ce.at(i).an]
						  -mol.ElNeg[ce.at(j).an])/100.0)));
	  gg=sqrt( Distance(ce.at(i).pos,ce.at(j).pos));
	  if (gg<(soll_abst*1.2)) {
              printf("Y%g %s %s\n",gg,ce.at(i).Label.toStdString().c_str(),ce.at(j).Label.toStdString().c_str());
	    bond.ato1=&ce.at(i);
	    bond.ato2=&ce.at(j);
	    bond.length=gg;
	    bond.chi=soll_abst-gg;
	    bond.order=(bond.chi<0.0847)?1:((bond.chi<0.184)?2:((bond.chi<0.27)?3:4));
	    bond.order=((bond.ato2->Symbol=="H")||(bond.ato1->Symbol=="H"))?1:bond.order;
	    cl.append(bond);
	  }
	}
      }
    }	
  }
  if (bondList.size()!=cl.size()) {	    
    bondList=cl;
  }
  CEnvironment cel;
  invom.Label=xdinp[index].atomname;
  invom.pos=xdinp[index].kart;
  invom.an=xdinp[index].OrdZahl;
  invom.part=xdinp[index].part;
  invom.sg=xdinp[index].sg;
  invom.Symbol=PSE_Symbol[xdinp[index].OrdZahl];
  cel.append(invom);	  
  for (int i=0;i<xdinp.size();i++){
    if((xdinp[i].OrdZahl<0)||(xdinp[index].OrdZahl<0)) continue;
    if ((xdinp[i].part>0)&&(xdinp[index].part>0)&&(xdinp[i].part!=xdinp[index].part)) continue;
    //dk
    if ((xdinp[i].OrdZahl<83)&&(xdinp[index].OrdZahl<83)&&(xdinp[i].OrdZahl>=0)&&(xdinp[index].OrdZahl>=0)){
      soll_abst=((mol.Kovalenz_Radien[xdinp[i].OrdZahl]+
			      mol.Kovalenz_Radien[xdinp[index].OrdZahl])
		      -(0.08*fabs((double)mol.ElNeg[xdinp[i].OrdZahl]
				      -mol.ElNeg[xdinp[index].OrdZahl])))*1.2;
      gg=100.0*sqrt( Distance(xdinp[i].kart,xdinp[index].kart));
      if ((gg<soll_abst)&&(!(xdinp[i].kart==cel[0].pos))){		
	invom.Label=xdinp[i].atomname;
	invom.pos=xdinp[i].kart;
	invom.part=xdinp[i].part;
	invom.an=xdinp[i].OrdZahl;
	invom.Symbol=PSE_Symbol[xdinp[i].OrdZahl];
	cel.append(invom);
      }
    }
  }
  int end=cel.size();
    int *isInList=(int*)malloc(sizeof(int)*xdinp.size());
    for (int i=0;i<xdinp.size();i++){
      isInList[i]=0;
      for (int j=0; j<end;j++){
	if (cel[j].pos==xdinp[i].kart) isInList[i]=1;
      }
    }
    for (int i=0;i<xdinp.size();i++){
      for (int j=1; j<end;j++){
	if(xdinp[i].OrdZahl<0) continue;
	if (isInList[i]) continue;
	if ((xdinp[i].part>0)&&(cel.at(j).part>0)&&(cel.at(j).part!=xdinp[i].part)) continue;
	if ((xdinp[i].OrdZahl<83)&&(xdinp[i].OrdZahl>=0)){
	  soll_abst=((mol.Kovalenz_Radien[xdinp[i].OrdZahl]+
				  mol.Kovalenz_Radien[cel.at(j).an])
			  -(0.08*fabs((double)mol.ElNeg[xdinp[i].OrdZahl]
					  -mol.ElNeg[cel.at(j).an])))*1.2;
	  gg=100.0*sqrt( Distance(xdinp[i].kart,cel.at(j).pos));
	  if (gg<soll_abst){				  
	    invom.Label=xdinp[i].atomname;
	    invom.pos=xdinp[i].kart;
	    invom.part=xdinp[i].part;
	    invom.an=xdinp[i].OrdZahl;
	    invom.Symbol=PSE_Symbol[xdinp[i].OrdZahl];
	    cel.append(invom);
	  }
	}
      }
    }
    free(isInList);

    Connection cll;
    for (int i=0; i<cl.size();i++)
      for (int k=0; k<cel.size();k++)
	for (int j=0; j<cel.size();j++){
	  if ((cl.at(i).ato1->pos==cel.at(j).pos)&&(cl.at(i).ato2->pos==cel.at(k).pos)){
	    if ((cel.at(0).part>0)&&((cel.at(0).part!=cl.at(i).ato1->part)||(cel.at(0).part!=cl.at(i).ato2->part)));
	    else 
	      cll.append(cl[i]);
	  }
	}
    invDlg *id=new invDlg(&cel,&cll,dataBase);//TESTXDEDIT
    id->update();
    if (id->exec()==QDialog::Rejected)
      for (int i=0;i<bondList.size();i++){
	bondList[i].order=cl.at(i).order;
      }
}
  /////////


void CubeGL::invariomExport(){  
  extern molekul mol;
  extern QList<INP> xdinp;
  extern QList<INP> asymmUnit;
  static char PSE_Symbol[109][3] = {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar",
    "K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr",
    "Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe",
    "Cs","Ba", "La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu",
    "Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra",
    "Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No","Lr","Ku","Ha","Rf","Ns","Hs","Mt"};
  invariomsComplete.clear();
  invariomsUnique.clear();
  knoepfe.clear();
  QDialog *invExportDlg =new QDialog(this);
  invExportDlg->setMinimumSize(400,300);  
  QTextBrowser *browser=new QTextBrowser;
  browser->setOpenLinks(false);
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), invExportDlg, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), invExportDlg, SLOT(reject()));
  connect(browser,SIGNAL(anchorClicked( const QUrl &)), this ,SLOT(editInv(const QUrl &)));
  connect(browser,SIGNAL(anchorClicked( const QUrl &)), invExportDlg, SLOT(reject()));
  QPushButton *exportMoProbutton = new QPushButton("Export Invarioms to MoPro files");
  QPushButton *exportXDbutton = new QPushButton("Export Invarioms to XD files");
  connect(exportMoProbutton,SIGNAL(clicked()),this,SLOT(exportMoProFiles()));
  connect(exportXDbutton,SIGNAL(clicked()),this,SLOT(exportXDFiles()));
  connect(exportXDbutton,SIGNAL(clicked()),invExportDlg, SLOT(reject()));
  connect(exportMoProbutton,SIGNAL(clicked()),invExportDlg, SLOT(reject()));
  QString text;
  for (int ix=0;ix<asymmUnit.size();ix++){
    MyAtom invom;
    double soll_abst,gg;
    MyBond bond;
    if (ce.size()==0){

      for (int i=0;i<xdinp.size();i++)
	if (xdinp[i].OrdZahl>-1) {
	  invom.Label=xdinp[i].atomname;
	  invom.pos=xdinp[i].kart;
	  invom.an=xdinp[i].OrdZahl;
	  invom.part=xdinp[i].part;
	  invom.Symbol=PSE_Symbol[xdinp[i].OrdZahl];
	  invom.index=i;
	  ce.append(invom);
	}
    }
    if ((bondList.size()!=0)) {
      cl=bondList;

    }
    else {
      for (int i=0;i<ce.size();i++) {
	for (int j=i+1;j<ce.size();j++){

	  if ((ce.at(i).an<0)||(ce.at(j).an<0)) continue;
	  if (ce.at(i).pos==ce.at(j).pos) continue;

	  if ((ce.at(i).an==0)&&(ce.at(j).an==0)) continue;
	  if ((ce.at(i).part<0)&&(ce.at(j).part<0)&&(ce.at(j).Label.contains("_"))) continue;
  	    if ((ce.at(i).part!=0)&&(ce.at(j).part!=0)&&(ce.at(j).part!=ce.at(i).part)) continue;
	  if ((ce.at(i).an<83)&&(ce.at(j).an<83)&&(ce.at(i).an>=0)&&(ce.at(j).an>=0)){
	    soll_abst=((mol.Kovalenz_Radien[ce.at(i).an]+
				    mol.Kovalenz_Radien[ce.at(j).an])/100.0
			    -(0.08*fabs(((double)mol.ElNeg[ce.at(i).an]
						    -mol.ElNeg[ce.at(j).an])/100.0)));
	    gg=sqrt( Distance(ce.at(i).pos,ce.at(j).pos));
	    if (gg<soll_abst*1.2) {
//              printf("Z%g %s %s %g \n",gg,ce.at(i).Label.toStdString().c_str(),ce.at(j).Label.toStdString().c_str(),soll_abst);
	      bond.ato1=&ce.at(i);
	      bond.ato2=&ce.at(j);
	      bond.length=gg;
	      bond.chi=soll_abst-gg;
	      bond.order=(bond.chi<0.0847)?1:((bond.chi<0.184)?2:((bond.chi<0.27)?3:4));
	      bond.order=((bond.ato1->Symbol=="H")||(bond.ato2->Symbol=="H"))?1:bond.order;

	      cl.append(bond);
	    }
	  }
	}
      }	
    }
    if (bondList.size()!=cl.size()) {	    
      bondList=cl;
    }
    CEnvironment sel;
    QString ina;
    invom.Label=xdinp[ix].atomname;
    invom.pos=xdinp[ix].kart;
    invom.an=xdinp[ix].OrdZahl;
    invom.part=xdinp[ix].part;
   if (invom.an>-1) invom.Symbol=PSE_Symbol[xdinp[ix].OrdZahl];
    invom.index=ix;
    if (invom.an>-1 ){
      text+="<b>";
      ina=inames::invName(invom,bondList,sel,ix);      

      knoepfe.append(sel);
      invariomsComplete.append(ina);
      text+=QString("<a href=\"%1\">").arg(ix);
      if (dataBase.contains(ina)) text+="<font color=\"green\">";
      else {
	text+="<font color=\"red\">";
	exportMoProbutton->setEnabled(false);
	exportXDbutton->setEnabled(false);
      }
      text+=invom.Label;
      text+=": </font></a></b>";
      text+=ina;
      text+="<br>";
    }

  }
  browser->setHtml(text);
  invariomsUnique=invariomsComplete;
#if (QT_VERSION >= 0x040500)
  invariomsUnique.removeDuplicates();
#else
  {
    int of=0;
    for (int i=0; i < invariomsUnique.size(); i++){
      do{  
	of= invariomsUnique.lastIndexOf(invariomsUnique.at(i));
	if (of>i) invariomsUnique.removeAt(of);
      }while(of>i);
    }
  }
#endif
  //printf("test4\n");
  QVBoxLayout sss;
  sss.addWidget(browser);
  sss.addWidget(buttonBox);
  sss.addWidget(exportMoProbutton);
  sss.addWidget(exportXDbutton);
  invExportDlg->setLayout(&sss);
  invExportDlg->setWindowTitle("Export these Invaromnames to 'Invariome.in'");
  //printf("test5\n");
  if (QDialog::Accepted==invExportDlg->exec()){
    QFile in("Invariome.in");
    in.open(QIODevice::WriteOnly);
    in.write(browser->toPlainText().toLatin1(),browser->toPlainText().length());
    in.close();
  }
  //printf("test6\n");
}

QString CubeGL::inv2moproaxes(int index){
    QStringList axtok;
    QString as1,as2;
    extern molekul mol;
    extern int dummax;
    extern QList<INP> asymmUnit;
    int j=dataBase.indexOf(invariomsComplete.at(index));
    if (j==-1) {
        //default
      if (knoepfe.at(index).size()>1){
        as1=knoepfe.at(index).at(0).Label;
        as2=knoepfe.at(index).at(1).Label;
        as1.remove(QRegExp("[)(]+"));
        as2.remove(QRegExp("[)(]+"));
      if (as1.contains('@')){
	//wenn es '@' im Namen gibt dann resi und resi-Nr decodieren
	QString anam =as1.section('@',0,0);
	QString resinr =as1.section('@',1,1).section(QRegExp("[A-Za-z]+"),0,0);
	as1=QString("%1_%2").arg(resinr).arg(anam);
      }
      if (as2.contains('@')){
	//wenn es '@' im Namen gibt dann resi und resi-Nr decodieren
	QString anam =as2.section('@',0,0);
	QString resinr =as2.section('@',1,1).section(QRegExp("[A-Za-z]+"),0,0);
	as2=QString("%1_%2").arg(resinr).arg(anam);
      }
        return QString(" XY  %1 %2").arg(as1).arg(as2);//
      }
    }
    QString erg = "problem ",axcopy=entries.at(j).CoordinateSystem;
    //KS: X:C(4) Y:C(7) AX2:C0.033974 AX1:C0.033938
    axcopy.remove("KS: ");
    axcopy.remove(QRegExp("\\(\\w+\\)"));
    axcopy.replace(" ",":");

    axtok=axcopy.split(":",QString::SkipEmptyParts);
    if (axtok.size()>3){
    int at1=mol.Get_OZ(axtok.at(1));
    int at2=mol.Get_OZ(axtok.at(3));
    int ind1=-1,ind2=-1,r=0;

    if ((axcopy.contains("DUM"))&&(entries.at(j).Symmetry=="mm2")) {
      ind1=qMax(at1,at2);//der index der nicht dummy ist
      at1=at2=-1;
      for (int k=1; k< knoepfe.at(index).size();k++){//direkte Nachbarn finden
        if ((at1!=-1)&&(knoepfe.at(index).at(k).an==ind1)) {at2=k;continue;}
        if ((at1==-1)&&(knoepfe.at(index).at(k).an==ind1)) {at1=k;continue;}
      }

      as1=knoepfe.at(index).at(at1).Label;
      as2=knoepfe.at(index).at(at2).Label;

      as1.remove(QRegExp("[)(]+"));
      as2.remove(QRegExp("[)(]+"));

      if (as1.contains('@')){
	//wenn es '@' im Namen gibt dann resi und resi-Nr decodieren
	QString anam =as1.section('@',0,0);
	QString resinr =as1.section('@',1,1).section(QRegExp("[A-Za-z]+"),0,0);
	as1=QString("%1_%2").arg(resinr).arg(anam);
      }
      if (as2.contains('@')){
	//wenn es '@' im Namen gibt dann resi und resi-Nr decodieren
	QString anam =as2.section('@',0,0);
	QString resinr =as2.section('@',1,1).section(QRegExp("[A-Za-z]+"),0,0);
	as2=QString("%1_%2").arg(resinr).arg(anam);
      }
      erg = QString("%1%2%3  %4 %5")
            .arg("b")
            .arg(axtok.at(0))
            .arg(axtok.at(2))
            .arg(as1)
            .arg(as2);
      return erg;
    }
    if ((axcopy.contains("DUM"))&&((entries.at(j).Symmetry=="3")||(entries.at(j).Symmetry=="3m"))&&(knoepfe.at(index).size()>3)) {
      ind1=qMax(at1,at2);//der index der nicht dummy ist
      at1=at2=-1;
      int at3=-1;
      for (int k=1; k< knoepfe.at(index).size();k++){//direkte Nachbarn finden
        if ((at2!=-1)&&(at1!=-1)&&(knoepfe.at(index).at(k).an==ind1)) {at3=k;continue;}
        if ((at2==-1)&&(at1!=-1)&&(knoepfe.at(index).at(k).an==ind1)) {at2=k;continue;}
        if ((at1==-1)&&(knoepfe.at(index).at(k).an==ind1)) {at1=k;continue;}

      }
      as1=knoepfe.at(index).at(at1).Label;
      as2=knoepfe.at(index).at(at2).Label;
      QString as3 = knoepfe.at(index).at(at3).Label;
      as1.remove(QRegExp("[)(]+"));
      as2.remove(QRegExp("[)(]+"));
      as3.remove(QRegExp("[)(]+"));

      if (as1.contains('@')){
	//wenn es '@' im Namen gibt dann resi und resi-Nr decodieren
	QString anam =as1.section('@',0,0);
	QString resinr =as1.section('@',1,1).section(QRegExp("[A-Za-z]+"),0,0);
	as1=QString("%1_%2").arg(resinr).arg(anam);
      }
      if (as2.contains('@')){
	//wenn es '@' im Namen gibt dann resi und resi-Nr decodieren
	QString anam =as2.section('@',0,0);
	QString resinr =as2.section('@',1,1).section(QRegExp("[A-Za-z]+"),0,0);
	as2=QString("%1_%2").arg(resinr).arg(anam);
      }
      if (as3.contains('@')){
	//wenn es '@' im Namen gibt dann resi und resi-Nr decodieren
	QString anam =as3.section('@',0,0);
	QString resinr =as3.section('@',1,1).section(QRegExp("[A-Za-z]+"),0,0);
	as3=QString("%1_%2").arg(resinr).arg(anam);
      }
      erg = QString("%1%2%3  %4 %5 %6")
            .arg("3")
            .arg(axtok.at(0))
            .arg(axtok.at(2))
            .arg(as1)
            .arg(as2)
            .arg(as3);
      return erg;


    }
    
      for (int k=1; k< knoepfe.at(index).size();k++){//direkte Nachbarn finden
	if ((ind1==-1)&&(knoepfe.at(index).at(k).an==at1)) {ind1=knoepfe.at(index).at(k).index;continue;}
	if ((knoepfe.at(index).at(k).an==at2)&&(knoepfe.at(index).at(k).index<(asymmUnit.size()-dummax))) {ind2=knoepfe.at(index).at(k).index;continue;}
      }

      if ((ind2>(asymmUnit.size()-dummax))||(ind2<0)){
	r=ind1;
	for (int k=1; k< knoepfe.at(r).size();k++){
	  if ((knoepfe.at(r).at(k).an==at2)&&(knoepfe.at(r).at(k).index!=ind1)&&(knoepfe.at(r).at(k).index!=index)) {
	    ind2=knoepfe.at(r).at(k).index;
          break;
        }
      }
      if (ind2==-1){
        for (int k=1; k< knoepfe.at(r).size();k++){
          if ((knoepfe.at(r).at(k).index!=ind1)&&(knoepfe.at(r).at(k).index!=index)) {
            ind2=knoepfe.at(r).at(k).index;
            break;
          }
        }
      }
    }
    
    

    as1=asymmUnit.at(ind1).atomname;
    as2=asymmUnit.at(ind2).atomname;
    as1.remove(QRegExp("[)(]+"));
    as2.remove(QRegExp("[)(]+"));

    if (as1.contains('@')){
      //wenn es '@' im Namen gibt dann resi und resi-Nr decodieren
      QString anam =as1.section('@',0,0);
      QString resinr =as1.section('@',1,1).section(QRegExp("[A-Za-z]+"),0,0);
      as1=QString("%1_%2").arg(resinr).arg(anam);
    }
    if (as2.contains('@')){
      //wenn es '@' im Namen gibt dann resi und resi-Nr decodieren
      QString anam =as2.section('@',0,0);
      QString resinr =as2.section('@',1,1).section(QRegExp("[A-Za-z]+"),0,0);
      as2=QString("%1_%2").arg(resinr).arg(anam);
    }
    erg = QString("%1%2%3  %4 %5")
	    .arg(" ")
	    .arg(axtok.at(0))
	    .arg(axtok.at(2))
	    .arg(as1)
	    .arg(as2);
    }
    return erg;

}

QString CubeGL::translateSymm2MP(QString idbs){
  QString mps="";
  idbs= idbs.trimmed();
  if (idbs=="m") mps="mz";
  if (idbs=="mx") mps="mx";
  if (idbs=="my") mps="my";
  if (idbs=="mz") mps="mz";
  if (idbs=="1") mps="";
  if (idbs=="-1") mps="i";
  if (idbs=="2") mps="2z";
  if (idbs=="2x") mps="2x";
  if (idbs=="2y") mps="2y";
  if (idbs=="2z") mps="2z";
  if (idbs=="222") mps="222";
  if (idbs=="mm2") mps="mxmy";
  if (idbs=="mmm") mps="mmm";
  if (idbs=="4/mmm") mps="mmm";
  if (idbs=="3") mps="3z";
  if (idbs=="3m") mps="3m";
  if (idbs=="6") mps="cy";
  if (idbs=="-6m2") mps="3m";
  return mps;
}

double CubeGL::getHDist(int index){
  extern molekul mol;
  int j=dataBase.indexOf(invariomsComplete.at(index));
  if (j<0) return -1.0;
  QString axcopy=entries.at(j).CoordinateSystem;
    //KS: X:C(4) Y:C(7) AX2:C0.033974 AX1:C0.033938
    axcopy.remove("KS: ");
    axcopy.remove(QRegExp("\\(\\w+\\)"));
    axcopy.replace(" ",":");

    QStringList axtok=axcopy.split(":",QString::SkipEmptyParts);
    QString tok=axtok.last();
    int atyp=mol.Get_OZ(tok.remove(QRegExp("[-.0-9]+")));
    double a1_dist=axtok.last().remove(QRegExp("[A-Za-z]+")).toDouble();
    a1_dist=(((37+mol.Kovalenz_Radien[atyp]) / 100.0) - 
		    (0.08 * fabs(((double)220-mol.ElNeg[atyp])/100.0)))-a1_dist;

    return a1_dist;

}

void CubeGL::exportMoProFiles(){
    QDir md;
    exportLabels.clear();
    extern molekul mol;
    extern QList<INP> asymmUnit;
    const int vale[109]= {1 ,//H
     2 ,//He
     1 ,//Li
     2 ,//Be
     3 ,//B
     4 ,//C
     5 ,//N
     6 ,//O
     7 ,//F
     8 ,//Ne
     1 ,//Na
     2 ,//Mg
     3 ,//Al
     4 ,//Si
     5 ,//P
     6 ,//S
     7 ,//Cl
     8 ,//Ar
     1 ,//K
     2 ,//Ca
     3 ,//Sc
     4 ,//Ti
     5 ,//V
     6 ,//Cr
     7 ,//Mn
     8 ,//Fe
     9 ,//Co
     10,//Ni
     11,//Cu
     12,//Zn
     3 ,//Ga
     4 ,//Ge
     5 ,//As
     6 ,//Se
     7 ,//Br
     8 ,//Kr
     1 ,//Rb
     2 ,//Sr
     3 ,//Y
     4 ,//Zr
     5 ,//Nb
     6 ,//Mo
     7 ,//Tc
     8 ,//Ru
     9 ,//Rh
     10,//Pd
     11,//Ag
     12,//Cd
     3 ,//In
     4 ,//Sn
     5 ,//Sb
     6 ,//Te
     7 ,//I
     8 ,//Xe
     1 ,//Cs
     2 ,//Ba
     3 ,//La
     4 ,//Cr
     5 ,//Pr
     6 ,//Nd
     7 ,//Pm
     8 ,//Sm
     9 ,//Eu
     10,//Gd
     11,//Tb
     12,//Dy
     13,//Ho
     14,//Er
     15,//Tm
     16,//Yb
     17,//Lu
     4 ,//Hf
     5 ,//Ta
     6 ,//W
     7 ,//Re
     8 ,//Os
     9 ,//Ir
     10,//Pt
     11,//Au
     12,//Hg
     3 ,//Tl
     4 ,//Pb
     5 ,//Bi
     6 ,//Po
     7 ,//At
     8 ,//Rn
     1 ,//Fr
     2 ,//Ra
     3 ,//Ac
     4 ,//Th
     5 ,//Pa
     6 ,//U
     7 ,//Np
     8 ,//Pu
     9 ,//Am
     10,//Cm
     11,//Bk
     12,//Cf
     13,//Es
     14,//Fm
     15,//Md
     16,//No
     17,//Lr
     4 ,//Rf
     5 ,//Db
     6 ,//Sg
     7 ,//Bh
     8 ,//Hs
     9 };//Mt

    static char PSE_Symbol[109][3] = {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar",
      "K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr",
      "Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe",
      "Cs","Ba", "La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu",
      "Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra",
      "Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No","Lr","Ku","Ha","Rf","Ns","Hs","Mt"};
    md.mkdir("InvariomTransfer");
    QList<double> corr3;
    double issu=0.0,corr_3;
    int sosu=0,anzahl=0;

    for (int mi =0; mi < maxmol; mi++){
      for (int i=0;i<asymmUnit.size();i++){
        if (asymmUnit.at(i).molindex!=(mi+1)) continue;
        if (asymmUnit.at(i).OrdZahl<0) continue;
	exportLabels.append(asymmUnit.at(i).atomname);
        int z=dataBase.indexOf(invariomsComplete.at(i));
        if (z>-1) issu+=entries.at(z).m0;
        else issu+=vale[asymmUnit.at(i).OrdZahl];
        sosu+=vale[asymmUnit.at(i).OrdZahl];
        //if (asymmUnit.at(i).OrdZahl==0)
        anzahl++;
      }
      corr_3=0;
      if (anzahl) corr_3 = (sosu-issu)/anzahl;
      corr3.append(corr_3);
      sosu=0;
      issu=0;
      anzahl=0;
    }
    QFile moprofile(QString("InvariomTransfer/%1.00").arg(afilename));
    QDir moprodir(QString("InvariomTransfer/%1.00").arg(afilename));
    if (moprofile.open(QIODevice::WriteOnly)){
        QString Name=moprodir.canonicalPath();
        Name.chop(3);
        moprofile.write(QString("! MOPRO6.2   %1   %2     %3.00\n! reflection file : %3.Ihkl\n")
                        .arg(QDate::currentDate().toString("dd/MM/yy"))
                        .arg(QTime::currentTime().toString("hh:mm:ss"))
                        .arg(Name).toLatin1());
        moprofile.write(QString("!  RF  factor =  0.000  %        RI  factor =  0.000%\n! wR2F factor =  0.000  %       wR2I factor =  0.000%\n").toLatin1());
        moprofile.write(QString("! g.o.f. =   1.000  Nref=   0  Nfree=      0    0.00<sinT/l< 3.00\n! Created by MoleCoolQt (c) Christian B. Huebschle\n").toLatin1());
        moprofile.write(QString("\nCELL %1 %2 %3 %4 %5 %6 %7\n")
                        .arg(mol.zelle.a)
                        .arg(mol.zelle.b)
                        .arg(mol.zelle.c)
                        .arg(mol.zelle.al)
                        .arg(mol.zelle.be)
                        .arg(mol.zelle.ga)
                        .arg(mol.zelle.lambda).toLatin1());
        moprofile.write(QString("\nSYMM %1 %2 %3\n")
                        .arg(mol.zelle.symuncent)
                        .arg(mol.zelle.lattis)
                        .arg((mol.zelle.centro)?"CENTRO":"").toLatin1());
        for (int i=0; i<mol.zelle.symuncent;i++){
            moprofile.write(mol.encodeSymm(i).toLatin1());
        }
        moprofile.write("\nSCALE  1   1.0000\n\nFMULT     1.00000\n\nUOVER     0.00000   ISO\n\nSOLVT     0.00000    50.00000\n\nEXTIN  0.00000   GAUSSIAN      ISOT  TTYP1      !  G*10^4\n\n");
        moprofile.write("DUMMY 0\n\n");
        
        moprofile.write(QString("\nKAPPA  %1\n").arg(invariomsUnique.size()).toLatin1());
        for (int i=0; i<invariomsUnique.size();i++){
            int j=dataBase.indexOf(invariomsUnique.at(i));
            if (j==-1) {
                moprofile.write("  1.000000 1.000000 ! Not in data base!\n");
                continue;
            }
            moprofile.write(QString("  %1 %2\n")
                            .arg(entries.at(j).k1,0,'f',6)
                            .arg(entries.at(j).k2,0,'f',6)
                            .toLatin1());

        }
        moprofile.write(QString("\nATOMS  %1\n").arg(exportLabels.size()).toLatin1());
        for (int i=0; i<asymmUnit.size();i++){
            if (asymmUnit.at(i).OrdZahl>-1){
                QString anam,resinr,resityp;
                if (QString(asymmUnit.at(i).atomname).contains('@')){
		  //wenn es '@' im Namen gibt dann resi und resi-Nr decodieren
		  anam=QString(asymmUnit.at(i).atomname).section('@',0,0);
		  resinr=QString(asymmUnit.at(i).atomname).section('@',1,1).section(QRegExp("[A-Za-z]+"),0,0);
		  resityp=QString(asymmUnit.at(i).atomname).right(3);// ich hoffe resi typen sind immer 3 lang .... 		
		}
                else {
		  anam=QString(asymmUnit.at(i).atomname);
		  resinr="1";
		  resityp="MOL";
		}
                anam.remove(QRegExp("[)(]+"));
		asymmUnit[i].amul*=asymmUnit.at(i).imul;
                moprofile.write(QString("ATOM %1  %2 %3 %4 %5%6%7  %8 %9 %10\n")
                                .arg(i+1,5)//1
                                .arg(anam,-4)//2
                                .arg(resityp,4)//3
                                .arg(resinr,3)//4
				.arg(asymmUnit.at(i).frac.x,10,'f',6)//5
				.arg(asymmUnit.at(i).frac.y,10,'f',6)//6
				.arg(asymmUnit.at(i).frac.z,10,'f',6)//7
				.arg(asymmUnit.at(i).amul,6,'f',4)//8 pop //achtung hier sollten wir populationen haben 
				                      //werden aber noch nicht eingelesen
				.arg(asymmUnit.at(i).imul)//9 occ
				.arg(PSE_Symbol[asymmUnit.at(i).OrdZahl])//10 elem
                                .toLatin1());

                int j = invariomsUnique.indexOf(invariomsComplete.at(i));
                moprofile.write(QString("%1HEX  K%2     V0   M0   Q0\n")
                                .arg(inv2moproaxes(i),-32)//1ax
                                .arg(j+1)//6kappanr
                                .toLatin1());////hier bin ich gerade */
                if ((asymmUnit.at(i).uf.m12==asymmUnit.at(i).uf.m13)&&
                    (asymmUnit.at(i).uf.m13==asymmUnit.at(i).uf.m23)&&
                    (asymmUnit.at(i).uf.m12==0))
                  moprofile.write(QString("UISO %1\n").arg(asymmUnit.at(i).uf.m11).toLatin1());
                else
                  moprofile.write(QString("UANI %1 %2 %3 %4 %5 %6\n")
                                  .arg(asymmUnit.at(i).uf.m11,8,'f',6)
                                  .arg(asymmUnit.at(i).uf.m22,8,'f',6)
                                  .arg(asymmUnit.at(i).uf.m33,8,'f',6)
                                  .arg(asymmUnit.at(i).uf.m12,8,'f',6)
                                  .arg(asymmUnit.at(i).uf.m13,8,'f',6)
                                  .arg(asymmUnit.at(i).uf.m23,8,'f',6)
                                  .toLatin1());
                int z=dataBase.indexOf(invariomsComplete.at(i));
                if (z>-1){
                  double mv=0;
                  if ((asymmUnit.at(i).molindex-1)<corr3.size()) mv= entries.at(z).m0+corr3.at(asymmUnit.at(i).molindex-1);
                  moprofile.write(QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10\n")
                                  .arg(mv,8,'f',5)
                                .arg(entries.at(z).m1,6,'f',3)
                                .arg(entries.at(z).d1p,6,'f',3)
                                .arg(entries.at(z).d1m,6,'f',3)
                                .arg(entries.at(z).d0,6,'f',3)
                                .arg(entries.at(z).q0,6,'f',3)
                                .arg(entries.at(z).q1p,6,'f',3)
                                .arg(entries.at(z).q1m,6,'f',3)
                                .arg(entries.at(z).q2p,6,'f',3)
                                .arg(entries.at(z).q2m,6,'f',3)
                                .toLatin1());
                moprofile.write(QString("%1 %2 %3 %4 %5 %6 %7\n")
                                .arg(entries.at(z).o0,6,'f',3)
                                .arg(entries.at(z).o1p,6,'f',3)
                                .arg(entries.at(z).o1m,6,'f',3)
                                .arg(entries.at(z).o2p,6,'f',3)
                                .arg(entries.at(z).o2m,6,'f',3)
                                .arg(entries.at(z).o3p,6,'f',3)
                                .arg(-entries.at(z).o3m,6,'f',3)
                                .toLatin1());
                moprofile.write(QString("%1 %2 %3 %4 %5 %6 %7 %8 %9\n")
                                .arg(entries.at(z).h0,6,'f',3)
                                .arg(entries.at(z).h1p,6,'f',3)
                                .arg(entries.at(z).h1m,6,'f',3)
                                .arg(entries.at(z).h2p,6,'f',3)
                                .arg(entries.at(z).h2m,6,'f',3)
                                .arg(entries.at(z).h3p,6,'f',3)
                                .arg(-entries.at(z).h3m,6,'f',3)
                                .arg(entries.at(z).h4p,6,'f',3)
                                .arg(entries.at(z).h4m,6,'f',3)
                                .toLatin1());
              }else{
                moprofile.write("0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0.!not in database!!!\n");
                moprofile.write("0. 0. 0. 0. 0. 0. 0. 0.\n");
                moprofile.write("0. 0. 0. 0. 0. 0. 0. 0. 0. 0.\n");
              }
            }
        }
        moprofile.write("ANHAR 0 3 \n\nSTOP\n");
        moprofile.close();
	moprodir= QDir("InvariomTransfer/CONSTRAIN");

	QFile con("InvariomTransfer/CONSTRAIN");
	if (con.open(QIODevice::WriteOnly)){
	  con.write("! PREP CONS SYMP\n");
	  for (int i=0; i<asymmUnit.size();i++){
	    if (asymmUnit.at(i).OrdZahl!=-1){
	      int z=dataBase.indexOf(invariomsComplete.at(i));
	      if (z>-1){
		QString anam,resinr,symm;
		if (!translateSymm2MP(entries.at(z).Symmetry).isEmpty()){
		  symm=translateSymm2MP(entries.at(z).Symmetry);
		  if (QString(asymmUnit.at(i).atomname).contains('@')){
		    //wenn es '@' im Namen gibt dann resi und resi-Nr decodieren
		    anam=QString(asymmUnit.at(i).atomname).section('@',0,0);
		    resinr=QString(asymmUnit.at(i).atomname).section('@',1,1).section(QRegExp("[A-Za-z]+"),0,0);
		  }
		  else {
		    anam=QString(asymmUnit.at(i).atomname);
		    resinr="1";
		  }
		  anam.remove(QRegExp("[)(]+"));
		  con.write(QString("SYMPLM %1 %2 %3\n")
				  .arg(symm)
				  .arg(anam)
				  .arg(resinr)
				  .toLatin1());

		}
	      }
	    }
	  }
	  con.write("\n! PREP CONS EQUI\n");
	  for (int k=0; k<invariomsComplete.size(); k++){
            int index=-1;
	    if (-1!=(index=invariomsComplete.indexOf(invariomsComplete.at(k),k+1))){
		QString anam,resinr,anam2,resinr2;
		  if (QString(asymmUnit.at(k).atomname).contains('@')){
		    //wenn es '@' im Namen gibt dann resi und resi-Nr decodieren
		    anam=QString(asymmUnit.at(k).atomname).section('@',0,0);
		    resinr=QString(asymmUnit.at(k).atomname).section('@',1,1).section(QRegExp("[A-Za-z]+"),0,0);
		  }
		  else {
		    anam=QString(asymmUnit.at(k).atomname);
		    resinr="1";
		  }
		  anam.remove(QRegExp("[)(]+"));
		  if (QString(asymmUnit.at(index).atomname).contains('@')){
		    //wenn es '@' im Namen gibt dann resi und resi-Nr decodieren
		    anam2=QString(asymmUnit.at(index).atomname).section('@',0,0);
		    resinr2=QString(asymmUnit.at(index).atomname).section('@',1,1).section(QRegExp("[A-Za-z]+"),0,0);
		  }
		  else {
		    anam2=QString(asymmUnit.at(index).atomname);
		    resinr2="1";
		  }
		  anam2.remove(QRegExp("[)(]+"));
		  con.write(QString("AVEPVM %1 %2 %3 %4\n")
				  .arg(anam)
				  .arg(resinr)
				  .arg(anam2)
				  .arg(resinr2)
				  .toStdString().c_str());
	    }
	  
	  }
	  con.close();
	}
	QFile rest("InvariomTransfer/RESTRAIN");
	if (rest.open(QIODevice::WriteOnly)){
	  for (int i=0; i<asymmUnit.size(); i++){
	    if (asymmUnit.at(i).OrdZahl!=0) continue;
		QString anam,resinr,anam2,resinr2;
		  if (knoepfe.at(i).at(1).Label.contains('@')){
		    //wenn es '@' im Namen gibt dann resi und resi-Nr decodieren
		    anam=knoepfe.at(i).at(1).Label.section('@',0,0);
		    resinr=knoepfe.at(i).at(1).Label.section('@',1,1).section(QRegExp("[A-Za-z]+"),0,0);
		  }
		  else {
		    anam=knoepfe.at(i).at(1).Label;
		    resinr="1";
		  }
		  anam.remove(QRegExp("[)(]+"));
		  if (knoepfe.at(i).at(0).Label.contains('@')){
		    //wenn es '@' im Namen gibt dann resi und resi-Nr decodieren
		    anam2=knoepfe.at(i).at(0).Label.section('@',0,0);
		    resinr2=knoepfe.at(i).at(0).Label.section('@',1,1).section(QRegExp("[A-Za-z]+"),0,0);
		  }
		  else {
		    anam2=knoepfe.at(i).at(0).Label;
		    resinr2="1";
		  }
		  anam2.remove(QRegExp("[)(]+"));
		  rest.write(QString("DISTAN %1 %2 %3 %4 %5 0.002\n")
				  .arg(anam,8).arg(resinr,8).arg(anam2,8).arg(resinr2,8).arg(getHDist(i)).toLatin1());
		  rest.write(QString("URATIO %1 %2 %3 %4 %5 0.01! %6\n")
				  .arg(anam,8).arg(resinr,8).arg(anam2,8).arg(resinr2,8).arg(
					  (2==invariomsComplete.at(i).count('h')?"1.5":"1.2")).arg(invariomsComplete.at(i)).toLatin1());
	  }

	  rest.close();
	}

	
        qDebug()<<"MoPro files succesfully written!";
    }
    emit reloadFile();
}

void CubeGL::inv2XDaxes(int index, int maxat){
    QStringList axtok;
    extern molekul mol;
    extern QList<INP> asymmUnit;
    if (asymmUnit.at(index).OrdZahl<0) return; 
    int j=dataBase.indexOf(invariomsComplete.at(index));
    QString axcopy=entries.at(j).CoordinateSystem;
    //KS: X:C(4) Y:C(7) AX2:C0.033974 AX1:C0.033938
    axcopy.remove("KS: ");
    axcopy.remove(QRegExp("\\(\\w+\\)"));
    axcopy.replace(" ",":");

    printf("%s %d %d\n",asymmUnit.at(index).atomname,maxat,exportDummys.size());		
    axtok=axcopy.split(":",QString::SkipEmptyParts);
    if (axtok.size()>3){
      int at1=mol.Get_OZ(axtok.at(1));
      int at2=mol.Get_OZ(axtok.at(3));
      int ind1=-1,ind2=-1,ind3=-1,r=0;//noch viel arbeit ......
      if ((axcopy.contains("DUM"))&&(entries.at(j).Symmetry=="mm2")) {
	r=qMax(at1,at2);//non dummy atomic number
	for (int k=1; k< knoepfe.at(index).size();k++){//direkte Nachbarn finden
	  if ((ind1==-1)&&(knoepfe.at(index).at(k).an==r)) {ind1=knoepfe.at(index).at(k).index;continue;}
	  if ((knoepfe.at(index).at(k).an==r)&&(knoepfe.at(index).at(k).index<(maxat))) {ind2=knoepfe.at(index).at(k).index;continue;}
	}
      if ((ind2>(maxat))||(ind2<0)){//endstaendige O's
	r=ind1;
	for (int k=1; k< knoepfe.at(r).size();k++){
	  if ((knoepfe.at(r).at(k).an==at2)&&(knoepfe.at(r).at(k).index!=ind1)&&(knoepfe.at(r).at(k).index!=index)) {
	    ind2=knoepfe.at(r).at(k).index;
          break;
        }
      }
      if (ind2==-1){
        for (int k=1; k< knoepfe.at(r).size();k++){
          if ((knoepfe.at(r).at(k).index!=ind1)&&(knoepfe.at(r).at(k).index!=index)) {
            ind2=knoepfe.at(r).at(k).index;
            break;
          }
        }
      }
        MyAtom dummy;
	V3 rechts,links,mitte,dc;
	mol.frac2kart( asymmUnit.at(ind1).frac , rechts);
	mol.frac2kart( asymmUnit.at(index).frac, mitte );
	mol.frac2kart( asymmUnit.at(ind2).frac , links );
	dc = mitte + Normalize(((rechts-mitte)%(links-mitte)));
	mol.kart2frac(dc,dummy.pos);
	dummy.Label=QString("DUM%1").arg(exportDummys.size());
	exportLabels.append(dummy.Label);
	dummy.Symbol="DUM";
	dummy.an=-1;
	dummy.index=maxat+exportDummys.size();
	exportDummys.append(dummy);
	asymmUnit[index].nax=dummy.index;
	asymmUnit[index].nay1=index;
	asymmUnit[index].nay2=r;
	asymmUnit[index].icor1= (axtok.at(0)=="X")? 1 : ((axtok.at(0)=="Y")? 2 : 3);
	asymmUnit[index].icor2= (axtok.at(2)=="X")? 1 : ((axtok.at(2)=="Y")? 2 : 3);
      return;
    }// endstaendige O's 
        MyAtom dummy;
	V3 rechts,links,mitte,dc;
	mol.frac2kart( asymmUnit.at(ind1).frac , rechts);
	mol.frac2kart( asymmUnit.at(index).frac, mitte );
	mol.frac2kart( asymmUnit.at(ind2).frac , links );
	dc = mitte + Normalize(((rechts-mitte)+(links-mitte)));
	mol.kart2frac(dc,dummy.pos);
	dummy.Label=QString("DUM%1").arg(exportDummys.size());
	exportLabels.append(dummy.Label);
	dummy.Symbol="DUM";
	dummy.an=-1;
	dummy.index=maxat+exportDummys.size();
	exportDummys.append(dummy);
	asymmUnit[index].nax=dummy.index;
	asymmUnit[index].nay1=index;
	asymmUnit[index].nay2=ind2;
	asymmUnit[index].icor1= (axtok.at(0)=="X")? 1 : ((axtok.at(0)=="Y")? 2 : 3);
	asymmUnit[index].icor2= (axtok.at(2)=="X")? 1 : ((axtok.at(2)=="Y")? 2 : 3);
      return;
      }
      if ((axcopy.contains("DUM"))&&((entries.at(j).Symmetry=="3")||(entries.at(j).Symmetry=="3m"))){
	r=qMax(at1,at2);//non dummy atomic number

	for (int k=1; k< knoepfe.at(index).size();k++){//direkte Nachbarn finden
	  if (ind1==-1) {ind1=knoepfe.at(index).at(k).index;continue;}
	  if (ind2==-1) {ind2=knoepfe.at(index).at(k).index;continue;}
	  if (ind3==-1) {ind3=knoepfe.at(index).at(k).index;continue;}
	}
	V3 mitte, arm1, arm2, arm3, dc;
        MyAtom dummy;
	mol.frac2kart(asymmUnit.at(ind1).frac,arm1);
	mol.frac2kart(asymmUnit.at(index).frac,mitte);
	mol.frac2kart(asymmUnit.at(ind2).frac,arm2);
	mol.frac2kart(asymmUnit.at(ind3).frac,arm3);
	dc=mitte+Normalize(((arm1-mitte)+(arm2-mitte)+(arm3-mitte)));
	mol.kart2frac(dc,dummy.pos);
	dummy.Label=QString("DUM%1").arg(exportDummys.size());
	exportLabels.append(dummy.Label);
	dummy.Symbol="DUM";
	dummy.an=-1;
	dummy.index=maxat+exportDummys.size();
	exportDummys.append(dummy);
	asymmUnit[index].nax=dummy.index;
	asymmUnit[index].nay1=index;
	asymmUnit[index].nay2=ind2;
	asymmUnit[index].icor1= (axtok.at(0)=="X")? 1 : ((axtok.at(0)=="Y")? 2 : 3);
	asymmUnit[index].icor2= (axtok.at(2)=="X")? 1 : ((axtok.at(2)=="Y")? 2 : 3);
      return;
      }
      if ((axcopy.contains("DUM"))) {
        printf("daran liegts: %s %s %s \n",
            axcopy.toStdString().c_str(),
            invariomsComplete.at(index).toStdString().c_str(),
            entries.at(j).Symmetry.toStdString().c_str());
      asymmUnit[index].nax=(index==0)?1:0;
      asymmUnit[index].nay1=index;
      asymmUnit[index].nay2=(index==2)?1:2;
      asymmUnit[index].icor1=  1 ;
      asymmUnit[index].icor2=  2 ;
      return;
//        exit(0);
      }
      for (int k=1; k< knoepfe.at(index).size();k++){//direkte Nachbarn finden
	if ((ind1==-1)&&(knoepfe.at(index).at(k).an==at1)) {ind1=knoepfe.at(index).at(k).index;continue;}
	if ((knoepfe.at(index).at(k).an==at2)&&(knoepfe.at(index).at(k).index<(maxat))) {ind2=knoepfe.at(index).at(k).index;continue;}
      }

      if ((ind2>(maxat))||(ind2<0)){
	r=ind1;
	for (int k=1; k< knoepfe.at(r).size();k++){
	  if ((knoepfe.at(r).at(k).an==at2)&&(knoepfe.at(r).at(k).index!=ind1)&&(knoepfe.at(r).at(k).index!=index)) {
	    ind2=knoepfe.at(r).at(k).index;
          break;
        }
      }
      if (ind2==-1){
        for (int k=1; k< knoepfe.at(r).size();k++){
          if ((knoepfe.at(r).at(k).index!=ind1)&&(knoepfe.at(r).at(k).index!=index)) {
            ind2=knoepfe.at(r).at(k).index;
            break;
          }
        }
      }
    }
      asymmUnit[index].nax=ind1;
      asymmUnit[index].nay1=index;
      asymmUnit[index].nay2=ind2;
      asymmUnit[index].icor1= (axtok.at(0)=="X")? 1 : ((axtok.at(0)=="Y")? 2 : 3);
      asymmUnit[index].icor2= (axtok.at(2)=="X")? 1 : ((axtok.at(2)=="Y")? 2 : 3);

    }
}

QString CubeGL::symm2Key(QString sym){
  if (sym=="m") return "10 110 10011 0110011 100110011";//mz
  if (sym=="mm2") return "10 001 10010 1001000 100100010";
  if (sym=="3") return "10 001 10000 1000011 100001100";
  if (sym=="3m") return "10 001 10000 1000010 100001000";
  if (sym=="-1") return "10 000 11111 0000000 111111111";//-1
  if (sym=="1") return "10 111 11111 1111111 111111111";//1
  if (sym=="6") return "10 001 10000 1000000 100000000";
  if (sym=="mx") return "10 011 10110 1011001 101100110";//mx
  if (sym=="my") return "10 101 11010 1101010 110101010";//my
  if (sym=="mz") return "10 110 10011 0110011 100110011";//mz
  if (sym=="2x") return "10 100 10110 1100110 101100110";//2x
  if (sym=="2y") return "10 010 11010 1010101 110101010";//2y
  if (sym=="2z") return "10 001 10011 1001100 100110011";//2z
  if (sym=="2") return  "10 001 10011 1001100 100110011";
  if (sym=="2mx") return "10 000 10110 1000000 101100110";
  if (sym=="2my") return "10 000 11010 1000000 110101010";
  if (sym=="2mz") return "10 000 10011 0000000 100110011";
  if (sym=="m2m") return "10 000 10010 0000100 100100010";
  if (sym=="2mm") return "10 000 10010 0000000 100100010";
  if (sym=="4") return "10 001 10000 1000000 100000011";
  if (sym=="-4") return "10 000 10000 0001100 100000011";
  if (sym=="4/m") return "10 000 10000 0000000 100000011";
  if (sym=="4mm") return "10 001 10000 1000000 100000010";
  if (sym=="-42m") return "10 000 10000 1001000 100000010";
  if (sym=="422") return "10 001 10000 1000000 100000010";
  if (sym=="4/mmm") return "10 000 10000 0000000 100000010";
  if (sym=="-3") return "10 000 10000 0000000 100001100";
  if (sym=="3mx") return "10 001 10000 1000001 100000100";
  if (sym=="3my") return "10 001 10000 1000010 100001000";
  if (sym=="-1m") return "10 000 10000 0000000 100001000";
  if (sym=="-6m2") return "10 000 10000 0000010 100000000";
  printf( "%s ist nicht in der Liste!\n",sym.toStdString().c_str());
  return "00 000 00000 0000000 000000000";
} 

void CubeGL::exportXDFiles(){
    exportDummys.clear();
    exportLabels.clear();
    QDir md;
    extern molekul mol;
    extern QList<INP> asymmUnit;

    const int vale[109]= {1 ,//H
     2 ,//He
     1 ,//Li
     2 ,//Be
     3 ,//B
     4 ,//C
     5 ,//N
     6 ,//O
     7 ,//F
     8 ,//Ne
     1 ,//Na
     2 ,//Mg
     3 ,//Al
     4 ,//Si
     5 ,//P
     6 ,//S
     7 ,//Cl
     8 ,//Ar
     1 ,//K
     2 ,//Ca
     3 ,//Sc
     4 ,//Ti
     5 ,//V
     6 ,//Cr
     7 ,//Mn
     8 ,//Fe
     9 ,//Co
     10,//Ni
     11,//Cu
     12,//Zn
     3 ,//Ga
     4 ,//Ge
     5 ,//As
     6 ,//Se
     7 ,//Br
     8 ,//Kr
     1 ,//Rb
     2 ,//Sr
     3 ,//Y
     4 ,//Zr
     5 ,//Nb
     6 ,//Mo
     7 ,//Tc
     8 ,//Ru
     9 ,//Rh
     10,//Pd
     11,//Ag
     12,//Cd
     3 ,//In
     4 ,//Sn
     5 ,//Sb
     6 ,//Te
     7 ,//I
     8 ,//Xe
     1 ,//Cs
     2 ,//Ba
     3 ,//La
     4 ,//Cr
     5 ,//Pr
     6 ,//Nd
     7 ,//Pm
     8 ,//Sm
     9 ,//Eu
     10,//Gd
     11,//Tb
     12,//Dy
     13,//Ho
     14,//Er
     15,//Tm
     16,//Yb
     17,//Lu
     4 ,//Hf
     5 ,//Ta
     6 ,//W
     7 ,//Re
     8 ,//Os
     9 ,//Ir
     10,//Pt
     11,//Au
     12,//Hg
     3 ,//Tl
     4 ,//Pb
     5 ,//Bi
     6 ,//Po
     7 ,//At
     8 ,//Rn
     1 ,//Fr
     2 ,//Ra
     3 ,//Ac
     4 ,//Th
     5 ,//Pa
     6 ,//U
     7 ,//Np
     8 ,//Pu
     9 ,//Am
     10,//Cm
     11,//Bk
     12,//Cf
     13,//Es
     14,//Fm
     15,//Md
     16,//No
     17,//Lr
     4 ,//Rf
     5 ,//Db
     6 ,//Sg
     7 ,//Bh
     8 ,//Hs
     9 };//Mt

    static char PSE_Symbol[109][3] = {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar",
      "K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr",
      "Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe",
      "Cs","Ba", "La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu",
      "Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra",
      "Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No","Lr","Ku","Ha","Rf","Ns","Hs","Mt"};

    for (int i=0;i<asymmUnit.size();i++){
      if (asymmUnit.at(i).OrdZahl<0) continue;
      QString as=asymmUnit.at(i).atomname;
      as.remove(QRegExp("[)(]+"));
      if (as.contains('@')){
	//wenn es '@' im Namen gibt dann resi und resi-Nr decodieren
	QString anam =as.section('@',0,0);
	QString resinr =as.section('@',1,1).section(QRegExp("[A-Za-z]+"),0,0);
	as=QString("%1.%2").arg(anam).arg(resinr);
      }
      as.insert(strlen(PSE_Symbol[asymmUnit.at(i).OrdZahl]),"(");
      as.append(")");
      exportLabels.append(as);

    }
    md.mkdir("InvariomTransfer");
    QList<double> corr3;
    double issu=0.0,corr_3;
    int sosu=0,anzahl=0;

    for (int mi =0; mi < maxmol; mi++){
      for (int i=0;i<asymmUnit.size();i++){
        if (asymmUnit.at(i).molindex!=(mi+1)) continue;
        if (asymmUnit.at(i).OrdZahl<0) continue;
        int z=dataBase.indexOf(invariomsComplete.at(i));
        if (z>-1) issu+=entries.at(z).m0;
        else issu+=vale[asymmUnit.at(i).OrdZahl];
        sosu+=vale[asymmUnit.at(i).OrdZahl];
        //if (asymmUnit.at(i).OrdZahl==0)
        anzahl++;
      }
      corr_3=0;
      if (anzahl) corr_3 = (sosu-issu)/anzahl;
      corr3.append(corr_3);
      sosu=0;
      issu=0;
      anzahl=0;
    }
    int ntabl=0;
    QString hama;
    QMap<int,int>itable;
    QMap<int,int>jtable;
    for (int i=0;i<asymmUnit.size();i++){
      if (asymmUnit.at(i).OrdZahl<0) continue;
      inv2XDaxes(i,exportLabels.size()-exportDummys.size());
      if (hama.contains(QString("!%1!").arg(PSE_Symbol[asymmUnit.at(i).OrdZahl]))) continue;
      hama+=QString("!%1!").arg(PSE_Symbol[asymmUnit.at(i).OrdZahl]);
      ntabl++;
      itable[asymmUnit.at(i).OrdZahl]=ntabl;
    }
    QFile inp("InvariomTransfer/xd.inp");
    QString datum=QDate::currentDate().toString("dd-MMM-yy--HH-mm-ss");
    if (inp.exists()) {
      datum=QFileInfo("InvariomTransfer/xd.inp").lastModified().toString("dd-MMM-yy--HH-mm-ss"); 
      inp.rename(QString("InvariomTransfer/xd_%1.inp").arg(datum));
      inp.setFileName("InvariomTransfer/xd.inp");
    }
    CID.truncate(8);
    if (inp.open(QIODevice::WriteOnly)){
      inp.write("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      inp.write(QString("! <<< X D PARAMETER FILE >>> $Revision: 5.34 (Mar 05 2007)$          %1!\n")
		      .arg(QDate::currentDate().toString("dd-MMM-yy")).toLatin1());
      inp.write("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      inp.write("XDPARFILE VERSION  2\n");
      inp.write(QString("   %1  MODEL   4  2  1  0\n").arg(CID,-8).toLatin1());
      inp.write("LIMITS nat  2000 ntx  31 lmx  4 nzz  30 nto  0 nsc  20 ntb  20 nov   2500\n");
      inp.write(QString("USAGE  %1   2  4 %2   0   1   0   1 %3 %4     1   1   1 %5\n")
		      .arg(exportLabels.size()-exportDummys.size())
		      .arg(invariomsUnique.size())
		      .arg(ntabl)
		      .arg(mol.zelle.symuncent)
		      .arg(exportDummys.size()).toLatin1());
       inp.write("  0.000000  0.000000  0.000000  0.000000  0.000000  0.000000  1.000000 0.100E+01\n");
       inp.write(" -2.000000  0.000000  0.000000  0.000000  0.000000  0.333300\n");
       for (int i=0; i<exportDummys.size();i++){
	 inp.write(QString(" %1 %2 %3\n")
			 .arg(exportDummys.at(i).pos.x,14,'f',6)
			 .arg(exportDummys.at(i).pos.y,14,'f',6)
			 .arg(exportDummys.at(i).pos.z,14,'f',6)
			 .toLatin1());
       }
	for (int i=0; i<asymmUnit.size();i++){
	  if (asymmUnit.at(i).OrdZahl>-1){
	    int j = invariomsUnique.indexOf(invariomsComplete.at(i));
//	    format(a8,1x,2i2,i5,2i4,i2,2i3,i2,i3,i4,3f10.6,f7.4)
	    jtable[j]=itable.value(asymmUnit.at(i).OrdZahl);
	    inp.write(QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14 %15 %16\n")
			    .arg(exportLabels.at(i),-9)
			    .arg(asymmUnit.at(i).icor1,1)
			    .arg(asymmUnit.at(i).icor2,1)
			    .arg((asymmUnit.at(i).nax+1)*asymmUnit.at(i).lflag,4)
			    .arg((asymmUnit.at(i).nay1+1),3)
			    .arg((asymmUnit.at(i).nay2+1),3)
			    .arg(((asymmUnit.at(i).uf.m12==asymmUnit.at(i).uf.m13)&&
					    (asymmUnit.at(i).uf.m13==asymmUnit.at(i).uf.m23)&&
					    (asymmUnit.at(i).uf.m12==0))?1:2,1)//jtf
			    .arg(itable.value(asymmUnit.at(i).OrdZahl),2)//itbl
			    .arg(j+1,2)//noofkappa
			    .arg(4,1)//lmax
			    .arg(1,2)//isy 
			    .arg(0,3)//ichcon
			    .arg(asymmUnit.at(i).frac.x,9,'f',6)
			    .arg(asymmUnit.at(i).frac.y,9,'f',6)
			    .arg(asymmUnit.at(i).frac.z,9,'f',6)
			    .arg(asymmUnit.at(i).amul,6,'f',4)//pop
			    .toLatin1());
	    inp.write(QString(" %1 %2 %3 %4 %5 %6\n")
			    .arg(asymmUnit.at(i).uf.m11,9,'f',6)
			    .arg(asymmUnit.at(i).uf.m22,9,'f',6)
			    .arg(asymmUnit.at(i).uf.m33,9,'f',6)
			    .arg(asymmUnit.at(i).uf.m12,9,'f',6)
			    .arg(asymmUnit.at(i).uf.m13,9,'f',6)
			    .arg(asymmUnit.at(i).uf.m23,9,'f',6)		    
			    .toLatin1());
	    int z=dataBase.indexOf(invariomsComplete.at(i));
	    if (z>-1){
	      double mv=0;
	      if ((asymmUnit.at(i).molindex-1)<corr3.size()) 
		mv= asymmUnit.at(i).amul*entries.at(z).m0+corr3.at(asymmUnit.at(i).molindex-1);
	      inp.write(QString(" %1 %2 %3 %4 %5 %6 %7 %8 %9 %10\n")
			      .arg(mv,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).m1,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).d1p,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).d1m,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).d0,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).q0,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).q1p,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).q1m,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).q2p,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).q2m,7,'f',4)
			      .toLatin1());
	      inp.write(QString(" %1 %2 %3 %4 %5 %6 %7 %8 %9 %10\n")
			      .arg(asymmUnit.at(i).amul*entries.at(z).o0,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).o1p,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).o1m,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).o2p,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).o2m,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).o3p,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).o3m,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).h0,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).h1p,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).h1m,7,'f',4)
			      .toLatin1());
	      inp.write(QString(" %1 %2 %3 %4 %5 %6\n")
			      .arg(asymmUnit.at(i).amul*entries.at(z).h2p,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).h2m,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).h3p,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).h3m,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).h4p,7,'f',4)
			      .arg(asymmUnit.at(i).amul*entries.at(z).h4m,7,'f',4)
			      .toLatin1());
	    }
	    else{
	      inp.write("  0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000\n");
	      inp.write("  0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000\n");
	      inp.write("  0.0000 0.0000 0.0000 0.0000 0.0000 0.0000\n");
	    }
	  }
	}
	
        for (int i=0; i<invariomsUnique.size();i++){
            int j=dataBase.indexOf(invariomsUnique.at(i));
            if (j==-1) {
                inp.write("  1  1.000000  1.000000  1.000000  1.000000  1.000000  1.000000\n");
                continue;
            }
            inp.write(QString("  %1 %2 %3 %4 %5 %6 %7\n")
			    .arg(jtable.value(i))
                            .arg(entries.at(j).k1,8,'f',6)
                            .arg(entries.at(j).k2,8,'f',6)
                            .arg(entries.at(j).k3,8,'f',6)
                            .arg(entries.at(j).k4,8,'f',6)
                            .arg(entries.at(j).k5,8,'f',6)
                            .arg(entries.at(j).k6,8,'f',6)
                            .toLatin1());

        }
	inp.write(" 0.0000E+00 0.0000E+00 0.0000E+00  0.0000E+00  0.0000E+00  0.0000E+00 0.0000E+00\n 0.0000E+00\n 0.100000E+01\n");
	inp.close();
    }
    QFile mas("InvariomTransfer/xd.mas");
    if (mas.exists()) {
      datum=QFileInfo("InvariomTransfer/xd.mas").lastModified().toString("dd-MMM-yy--HH-mm-ss"); 
      mas.rename(QString("InvariomTransfer/xd_%1.mas").arg(datum));
      mas.setFileName("InvariomTransfer/xd.mas");
    }
    if (mas.open(QIODevice::WriteOnly)){
      mas.write("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      mas.write(QString("! <<< X D MASTER FILE >>> $Revision: 5.34 $   exported by MoleCoolQt %1!\n")
		      .arg(QDate::currentDate().toString("dd-MMM-yy")).toLatin1());
      mas.write("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      mas.write(QString("TITLE %1\n").arg(CID).toLatin1());
      mas.write(QString("CELL %1 %2 %3 %4 %5 %6 \n")
		      .arg(mol.zelle.a)
		      .arg(mol.zelle.b)
		      .arg(mol.zelle.c)
		      .arg(mol.zelle.al)
		      .arg(mol.zelle.be)
		      .arg(mol.zelle.ga)
		      .toLatin1());
      mas.write(QString("WAVE %1\n")
		      .arg(mol.zelle.lambda).toLatin1());

      mas.write(QString("LATT %1 %2\n")
		      .arg((mol.zelle.centro)?"C":"A")
		      .arg(mol.zelle.lattis)
		      .toLatin1());
      for (int i=1; i<mol.zelle.symuncent;i++){
	mas.write(QString("SYMM %1").arg(mol.encodeSymm(i)).toLatin1());
      }
      mas.write("BANK CR\n");
      mas.write("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      mas.write("MODULE *XDLSM\nSELECT *model 4 2 1 0 based_on F^2 test verbose 1\nSELECT  cycle -10 dampk 1. cmin 0.6 cmax 1. eigcut 1.d-09 convcrit 1.d-05\nSAVE  deriv *lsqmat *cormat\nSOLVE  *inv *diag *cond\n!-------------------------------------------------------------------------------\nSCAT CORE SPHV DEFV   1S  2S  3S  4S  2P  3P  4P  3D  4D  4F  5S  5P  6S  6P  5D  7S  6D  5F  DELF'   DELF''  NSCTL\n");
//      SCAT core sphv defv 1s 2s 3s 4s 2p 3p 4p 3d 4d 4f 5s 5p 6s 6p 5d 7s 6d 5f Df' Df" nsctl
//      C    CHFW CHFW CSZD    2  -2   0   0  -2   0   0   0   0   0   0   0   0   0   0   0   0   0  0.0033  0.0016  0.665

      for (int i=1; i<=ntabl; i++){
	mas.write(QString("%1   CHFW CHFW CSZD  %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14 %15 %16 %17 %18 %19 %20 %21 %22\n")
			.arg(XDBANK[itable.key(i)].pse,-2)
			.arg(XDBANK[itable.key(i)].s1,3)
			.arg(XDBANK[itable.key(i)].s2,3)
			.arg(XDBANK[itable.key(i)].s3,3)
			.arg(XDBANK[itable.key(i)].s4,3)
			.arg(XDBANK[itable.key(i)].p2,3)
			.arg(XDBANK[itable.key(i)].p3,3)
			.arg(XDBANK[itable.key(i)].p4,3)
			.arg(XDBANK[itable.key(i)].d3,3)
			.arg(XDBANK[itable.key(i)].d4,3)
			.arg(XDBANK[itable.key(i)].f4,3)
			.arg(XDBANK[itable.key(i)].s5,3)
			.arg(XDBANK[itable.key(i)].p5,3)
			.arg(XDBANK[itable.key(i)].s6,3)
			.arg(XDBANK[itable.key(i)].p6,3)
			.arg(XDBANK[itable.key(i)].d5,3)
//      SCAT core sphv defv 1s 2s 3s 4s 2p 3p 4p 3d 4d 4f 5s 5p 6s 6p 5d 7s 6d 5f Df' Df" nsctl
			.arg(XDBANK[itable.key(i)].s7,3)
			.arg(XDBANK[itable.key(i)].d6,3)
			.arg(XDBANK[itable.key(i)].f5,3)
			.arg((mol.zelle.lambda-1.1262355<0)?XDBANK[itable.key(i)].fprimMo:XDBANK[itable.key(i)].fprimCu,7,'f',4)
			//1.1262355 ist der Mittelwert aus Cu un Mo. 
			.arg((mol.zelle.lambda-1.1262355<0)?XDBANK[itable.key(i)].fdprimMo:XDBANK[itable.key(i)].fdprimCu,7,'f',4)
			//wenn man ihn von lambda abzieht ist bei positiven Werten die WL naeher bei Cu als bei Mo
			.arg(XDBANK[itable.key(i)].nsctl/10.0,6,'f',3)
			.toLatin1());
      }
      mas.write("END SCAT\n!-------------------------------------------------------------------------------\nATOM     ATOM0    AX1 ATOM1    ATOM2   AX2 R/L TP  TBL KAP LMX SITESYM  CHEMCON\n");
      const char axl[3][2]={"X","Y","Z"};
      for (int i=0; i< asymmUnit.size();i++){
	if (asymmUnit.at(i).OrdZahl>-1){
	  int j = invariomsUnique.indexOf(invariomsComplete.at(i));
	  int z = dataBase.indexOf(invariomsComplete.at(i));
	//N(T)     C(A)      Z  N(T)     H(3)     Y   R   2   1   1   4    NO
	//ATOM     ATOM0    AX1 ATOM1    ATOM2   AX2 R/L TP  TBL KAP LMX SITESYM  CHEMCON
	  mas.write(QString("%1 %2 %3  %4 %5 %6   %7  %8 %9 %10 %11  _%12 %13\n")
			  .arg(exportLabels.at(i),-8)
			  .arg(exportLabels.at(asymmUnit.at(i).nax),-8)
			  .arg(axl[asymmUnit.at(i).icor1-1],2)
			  .arg(exportLabels.at(asymmUnit.at(i).nay1),-8)
			  .arg(exportLabels.at(asymmUnit.at(i).nay2),-8)
			  .arg(axl[asymmUnit.at(i).icor2-1],1)
			  .arg((asymmUnit.at(i).lflag>0)?"R":"L")
			  .arg(((asymmUnit.at(i).uf.m12==asymmUnit.at(i).uf.m13)&&
					  (asymmUnit.at(i).uf.m13==asymmUnit.at(i).uf.m23)&&
					  (asymmUnit.at(i).uf.m12==0))?1:2,2)
			  .arg(itable.value(asymmUnit.at(i).OrdZahl),3)
			  .arg(j+1,3)
			  .arg(4,3)
			  .arg(entries.at(z).Symmetry,-4)
			  .arg(" ")
			  .toLatin1());

	}
      }

      for (int j=0; j<exportDummys.size();j++){
	 mas.write(QString("%1 %2 %3 %4\n")
			 .arg(exportDummys.at(j).Label)
			 .arg(exportDummys.at(j).pos.x,14,'f',6)
			 .arg(exportDummys.at(j).pos.y,14,'f',6)
			 .arg(exportDummys.at(j).pos.z,14,'f',6)
			 .toLatin1());
       }
      mas.write("END ATOM\n!-------------------------------------------------------------------------------\n");
      mas.write("KEEP KAPPA ");
      for (int i=0; i<invariomsUnique.size();i++) {
	if (i%38==37) mas.write("\nKEEP KAPPA ");
	mas.write(QString("%1 ").arg(i+1).toLatin1());
      }
      mas.write("\nKEEP  charge group1\n! KEEP  rigid group1\n");

      for (int i=0; i<asymmUnit.size(); i++){
	if (asymmUnit.at(i).OrdZahl!=0) continue;
	mas.write(QString("RESET BOND %1 %2 %3\n")
			.arg(exportLabels.at(knoepfe.at(i).at(1).index))
			.arg(exportLabels.at(i))
			.arg(getHDist(i))
			.toLatin1());

      }
      mas.write("WEIGHT  -2.0 .0 .0 .0 .0 0.3333\nSKIP  obs 0. 1.d10 *sigobs 3. 1.d06 sinthl 0. 2.\n");
      mas.write("PRINT sinthl .0 2. obs 0. 15. delta 0. 10. *del% 80 100 extcn 80. 100. *abssc\n");
      mas.write("!EXTCN  *iso aniso *type_1 type_2 type_3 distr_g *distr_l msc_0  msc_1\n");
      mas.write("DMSDA  1.1  1.8\n FOUR  fmod1 4 2 0 0  fmod2 -1 2 0 0\n");
      mas.write("!CON  num1 par1/iat1 num2 par2/iat2 ... = num0\n!------------------------------------------------------------------------------\nKEY     XYZ --U2-- ----U3---- ------U4------- M- -D- --Q-- ---O--- ----H----\n");
      for (int i=0; i<asymmUnit.size(); i++){
	if (asymmUnit.at(i).OrdZahl<0)continue;
	if (asymmUnit.at(i).OrdZahl==0)
	  mas.write(QString("%1 000 100000 0000000000 000000000000000 00 000 00000 0000000 000000000 \n")
			.arg(exportLabels.at(i),-7)
			.toLatin1());
	else
	  mas.write(QString("%1 111 111111 0000000000 000000000000000 00 000 00000 0000000 000000000 \n")
			.arg(exportLabels.at(i),-7)
			.toLatin1());
      }
      for (int i=0; i<invariomsUnique.size();i++) {
	mas.write("KAPPA   000000\n");
      }
      mas.write("EXTCN   0000000\nOVTHP   0\nSCALE   1\nEND KEY\n!------------------------------------------------------------------------------\nEND XDLSM\n");
      mas.write("!Please replace the Key block above by the following block for symmetry constrained refinement\nKEY     XYZ --U2-- ----U3---- ------U4------- M- -D- --Q-- ---O--- ----H----\n");
      for (int i=0; i<asymmUnit.size(); i++){
	if (asymmUnit.at(i).OrdZahl<0)continue;
	int z=dataBase.indexOf(invariomsComplete.at(i));
	if (asymmUnit.at(i).OrdZahl==0)
	mas.write(QString("%1 000 100000 0000000000 000000000000000 %2 \n")
			.arg(exportLabels.at(i),-7)
			.arg(symm2Key(entries.at(z).Symmetry))
			.toLatin1());
	else
	mas.write(QString("%1 111 111111 0000000000 000000000000000 %2 \n")
			.arg(exportLabels.at(i),-7)
			.arg(symm2Key(entries.at(z).Symmetry))
			.toLatin1());
      }
      for (int i=0; i<invariomsUnique.size();i++) {
	mas.write("KAPPA   000000\n");
      }
      mas.write("EXTCN   0000000\nOVTHP   0\nSCALE   1\nEND KEY\n!------------------------------------------------------------------------------\nEND XDLSM\n!end of block!");

      mas.close();
      qDebug()<<"XD files succesfully written!";
    }else qDebug()<<"Can not write XD files!";
    emit reloadFile();
}

void CubeGL::wheelEvent(QWheelEvent *event){   
  int numDegrees = event->delta() / 8;
  int numSteps = numDegrees / 15;
if (event->modifiers()==Qt::NoModifier){
  if (iSel){
    if (ibas) glDeleteLists(ibas,1);
    ibas=glGenLists(1);    
    glNewList(ibas, GL_COMPILE );
    extern QList<INP> xdinp;
    extern molekul mol;
    if (numSteps>0)mol.highlightResi(xdinp,mol.firstHL-1,L,elli);
    else mol.highlightResi(xdinp,mol.lastHL+1,L,elli);
    glEndList();	    
    updateGL();
  }else{
    int d = myFont.pointSize();
    d=(d+numSteps>4)?d+numSteps:d;
    myFont.setPointSize(d);
    updateGL();
  }
}else if (event->modifiers()==Qt::ControlModifier){
    emit diffscroll(numSteps,1);

}else if (event->modifiers()==Qt::ShiftModifier){
    emit diffscroll(numSteps,0);
}if (event->modifiers()==Qt::AltModifier){
    emit diffscroll(numSteps,2);
}
  event->accept();
}

void CubeGL::contextMenuEvent(QContextMenuEvent *event) {
  if (atomsClickable){
    extern QList<INP> xdinp;
    extern molekul mol;
    QAction expandAct(tr("mist"),this);
    QAction hideThisAct(tr("mist"),this);
    QAction hideThisFragment("Hide this fragment",this);
    QAction hideOtherFragments("Hide other fragments",this);
    QAction *dntpck=new QAction("only asymmetric unit",this);
    QAction *molpck=new QAction("Complete molecules",this);
    QAction *cctpck=new QAction("Pack inside unit cell",this);
    QAction *ccmpck=new QAction("Pack inside unit cell and complete AU",this);

    QSignalMapper *signalMapper = new QSignalMapper(this);
    signalMapper->setMapping(dntpck,  0);
    signalMapper->setMapping(molpck,  2);
    signalMapper->setMapping(cctpck,  3);
    signalMapper->setMapping(ccmpck,  4);
    connect(dntpck,SIGNAL(triggered()),signalMapper,SLOT(map()));
    connect(molpck,SIGNAL(triggered()),signalMapper,SLOT(map()));
    connect(cctpck,SIGNAL(triggered()),signalMapper,SLOT(map()));
    connect(ccmpck,SIGNAL(triggered()),signalMapper,SLOT(map()));
    connect(signalMapper, SIGNAL(mapped(int)), parent(), SLOT(setPackArt2(int)));
    connect( &expandAct, SIGNAL(triggered () ), parent(), SLOT(expandAround() ));
    connect( &hideThisAct, SIGNAL(triggered () ), parent(), SLOT(filterThisAtom() ));
    connect( &hideThisFragment, SIGNAL(triggered () ), parent(), SLOT(filterThisFragment() ));
    connect( &hideOtherFragments, SIGNAL(triggered () ), parent(), SLOT(filterOtherFragments()));
    QMenu menu(this);
    double nahda=200.0,da=0;
    int nahdai=-1;
    extern QList<INP> xdinp;
    extern QList<Modulat> matoms;
    for (int j=0; j<xdinp.size();j++){
      da=(((xdinp.at(j).screenX-event->x())*( xdinp.at(j).screenX-event->x()))+ 
		      ((xdinp.at(j).screenY-event->y())*( xdinp.at(j).screenY-event->y())));
      nahdai=(da<nahda)?j:nahdai;
      nahda=qMin(nahda,da);
    }
    for (int j=0; j<matoms.size();j++){
      da=(((matoms.at(j).screenX-event->x())*( matoms.at(j).screenX-event->x()))+ 
          ((matoms.at(j).screenY-event->y())*( matoms.at(j).screenY-event->y())));
      nahdai=(da<nahda)?j:nahdai;
      nahda=qMin(nahda,da);
    }

    expandatom=nahdai;
      if (expandatom<xdinp.size()) {
	if (expandatom<0) {expandatom=-1;return;}
	expandAct.setText(tr("Expand %1 Ang. arround %2.").arg(mol.gd).arg(xdinp.at(expandatom).atomname));
	hideThisAct.setText(tr("Hide %1 ").arg(xdinp.at(expandatom).atomname));
	menu.addAction(&expandAct);
	menu.addAction(&hideThisAct);
	menu.addAction(&hideThisFragment);
	menu.addAction(&hideOtherFragments);
    {QAction *a = menu.addAction("Calculate p.d.f. of an atom",parent(),SLOT(pdfDlg()));
    a->setData(expandatom);
    }
    {QAction *a = menu.addAction("Show ADP parameters",parent(),SLOT(showADPvalues()));
    a->setData(expandatom);
    }
    menu.addSeparator();
	menu.addAction(dntpck);
	menu.addAction(molpck);
	menu.addAction(cctpck);
	menu.addAction(ccmpck);
	menu.addAction(changeGDAct);
	QAction *a = menu.addAction("edit xd_part.aux",parent(),SLOT(editPartAux()));
	a->setData(qMax(expandatom,0));
	menu.exec(event->globalPos());
      }
      else if (expandatom<matoms.size()) {
	if (expandatom<0) {expandatom=-1;return;}
	expandAct.setText(tr("Expand %1 Ang. arround %2.").arg(mol.gd).arg(matoms.at(expandatom).atomname));
	menu.addAction(&expandAct);
        menu.addSeparator();
	menu.addAction(dntpck);
	menu.addAction(molpck);
	menu.addAction(cctpck);
	menu.addAction(ccmpck);
	menu.addAction(changeGDAct);
	menu.exec(event->globalPos());
      }
      else{expandatom=-1;}
  }
}

void CubeGL::checkTC(){
  if ((tCR==bgCR)&&
		  (tCG==bgCG)&&
		  (tCB==bgCB)){
    tCR=1.0-bgCR;
    tCG=1.0-bgCG;
    tCB=1.0-bgCB;
    tCA=1.0;
  }
}

void CubeGL::zoom(double speed){
    if (!noWaitLabel)moving->start(80);
  glScaled(1.0+speed*0.1,1.0+speed*0.1,1.0+speed*0.1);  
  updateGL();  
}

void CubeGL::rotY(double speed){
    if (!noWaitLabel)moving->start(80);
  glRotateL(-20.0*speed,0.0f,1.0f,0.0f);
  updateGL();  
}

void CubeGL::moveY(double speed){
    if (!noWaitLabel)moving->start(80);
  glTranslateL(0.0,speed,0.0);
  updateGL();  
}

void CubeGL::moveX(double speed){
    if (!noWaitLabel)moving->start(80);
  glTranslateL(speed,0.0,0.0);
  updateGL();  
}

void CubeGL::rotX(double speed){
    if (!noWaitLabel)moving->start(80);
  glRotateL(-20.0*speed,1.0f,0.0f,0.0f);
  updateGL();  
}

void CubeGL::fontSizeUp() {
  myFont.setPointSize(myFont.pointSize()+1);
  updateGL();
}

void CubeGL::fontSizeDown() {
  int d = myFont.pointSize();
  d=(d>4)?d-1:d;
  myFont.setPointSize(d);
  updateGL();
}

void CubeGL::setAtomsClickable(bool on){
  atomsClickable=on;
}

void CubeGL::mouseMoveEvent(QMouseEvent *event) {
  double nahda=200.0,da=0;
  int nahdai=-1;
  extern QList<INP> xdinp;
  extern QList<Modulat> matoms;
  for (int j=0; j<xdinp.size();j++){
    da=(((xdinp.at(j).screenX-event->x())*( xdinp.at(j).screenX-event->x()))+ 
			((xdinp.at(j).screenY-event->y())*( xdinp.at(j).screenY-event->y())));
    nahdai=(da<nahda)?j:nahdai;
    nahda=qMin(nahda,da);
  }
  for (int j=0; j<matoms.size();j++){
    da=(((matoms.at(j).screenX-event->x())*( matoms.at(j).screenX-event->x()))+ 
		    ((matoms.at(j).screenY-event->y())*( matoms.at(j).screenY-event->y())));
    nahdai=(da<nahda)?j:nahdai;
    nahda=qMin(nahda,da);
  }

  GLfloat dx = GLfloat(
		  event->x() - lastPos.x()) / width();
  GLfloat dy = GLfloat(
		  event->y() - lastPos.y()) / height();
  if ((!moai)&&(!event->buttons())) {
    if (imFokus!=nahdai){
      imFokus=nahdai;
      if ((imFokus>=0)&&(!xdinp.isEmpty())&&(matoms.isEmpty())) emit message(xdinp[imFokus].atomname);
      if ((imFokus>=0)&&(xdinp.isEmpty())&&(!matoms.isEmpty())) emit message(matoms[imFokus].atomname);
      else emit message("");
      updateGL();
    }
  }
  if (event->buttons() & Qt::MidButton){
      if (!noWaitLabel)moving->start(80);
    glTranslateL(dx*vangle*3,-dy*vangle*3,0);
    updateGL();
  }
  if ((event->buttons() & Qt::LeftButton)) {
      if (!noWaitLabel)moving->start(80);
    if ((!xdinp.isEmpty())&&(moveLab)){
      GLdouble ML[16];
      glGetDoublev(GL_MODELVIEW_MATRIX,ML);
      double V2[3],V1[3];
      V1[0]=dx*12.0*mlsc*(double)_win_width/_win_height;
      V1[1]=-dy*12.0*mlsc;
      V1[2]=0;
      V2[0]=ML[0] *V1[0]+ML[1] *V1[1]+ML[2] *V1[2];
      V2[1]=ML[4] *V1[0]+ML[5] *V1[1]+ML[6] *V1[2];
      V2[2]=ML[8] *V1[0]+ML[9] *V1[1]+ML[10]*V1[2];

      xdinp[labToMove].labPos+=V3(V2[0],V2[1],V2[2]);
    }else if (moveLeg){
      double sc41=0.1086206896551724*vangle;
      if (horizont) {
	mil.y+=dx*sc41*(double)_win_width/_win_height;
	mil.x-=dy*sc41;
      }else{
	mil.x+=dx*sc41*(double)_win_width/_win_height;
	mil.y-=dy*sc41;
      }
     //printf("legend x %g  y %g",mil.x,mil.y); 
    }else{
      glRotateL(dy*360.0,1.0f,0.0f,0.0f);
      glRotateL(dx*360.0,0.0f,1.0f,0.0f);
    }
    updateGL();
  }
  else if((event->buttons() & Qt::RightButton)){
      if (!noWaitLabel)moving->start(80);
    if (invertMouseZoom->checkState()!=Qt::Checked){ 
      glScaled(1.0-dy,1.0-dy,1.0-dy);
      mlsc/=1.0-dy;
    }else {
      glScaled(1.0+dy,1.0+dy,1.0+dy);
      mlsc/=1.0+dy;
    }
    updateGL();
  }
  lastPos = event->pos();
}

void CubeGL::toggleMolisoLegendDirection(){
  if (horizont){
    //x -0.708863  y -0.0942207
    mil.x=-0.0788*vangle;
    mil.y=-0.0105*vangle;
  }else{
    //-0.452525  y -0.725207
    mil.x=-0.05*vangle;
    mil.y=-0.08*vangle;
  }

  printf("legend x %g  y %g %s\n",mil.x,mil.y,(!horizont)?"horizontal":"vertical"); 
  horizont=!horizont;
  updateGL();
}

void CubeGL::togglGrow(bool on){
  growIt=on;
}

void CubeGL::setMatrix(){
//#include <float.h>
//qDebug()<<"setMatrix()";  
  glMatrixMode(GL_MODELVIEW);
    GLdouble det=
	  MM[0]*MM[5]*MM[10] - MM[8]*MM[5]*MM[2]+
	  MM[1]*MM[6]*MM[8]  - MM[9]*MM[6]*MM[0]+
	  MM[2]*MM[4]*MM[9]  - MM[10]*MM[4]*MM[1];
  if ((det>=0)&&(det<9990000.0)) glLoadMatrixd(MM);
  else {
    MM[0]=1;
    MM[1]=0;
    MM[2]=0;
    MM[3]=0;
    MM[4]=0;
    MM[5]=1;
    MM[6]=0;
    MM[7]=0;
    MM[8]=0;
    MM[9]=0;
    MM[10]=1;
    MM[11]=0;
    MM[12]=0;
    MM[13]=0;
    MM[14]=-200;
    MM[15]=1;
    glLoadIdentity();
    glLoadMatrixd(MM);
    
  }
    updateGL();
}

void CubeGL::zalman(){
  stereo_mode=1;
  minus->show();
  updateGL();
}

void CubeGL::parallel(){
  stereo_mode=2;
  minus->show();
  minus->setChecked(true);
  updateGL();
}

void CubeGL::crosseye(){ 
  stereo_mode=3;
  minus->show();
  minus->setChecked(false);
  updateGL();
}

void CubeGL::nostereo(){
  stereo_mode=0;
  minus->hide();
  updateGL();
} 

void CubeGL::showMatrix(){
//  stereo_mode++;
//  stereo_mode%=4;
  updateGL();
#ifdef Q_WS_MAC
 // qDebug()<<QCoreApplication::libraryPaths () <<QImageWriter::supportedImageFormats ();
#endif   
#ifdef _WIN32
#else
  QList<QByteArray> supo = QImageWriter::supportedImageFormats ();
  for (int i=0; i<supo.size();i++) printf("%s\n",QString(supo.at(i)).toStdString().c_str());
  printf("Die MMATRIX ist:\n%9.6f %9.6f %9.6f %9.6f\n%9.6f %9.6f %9.6f %9.6f\n%9.6f %9.6f %9.6f %9.6f\n%9.6f %9.6f %9.6f %9.6f\n",
		  MM[0],MM[1],MM[2],MM[3],
		  MM[4],MM[5],MM[6],MM[7],
		  MM[8],MM[9],MM[10],MM[11],
		  MM[12],MM[13],MM[14],MM[15]);
  GLdouble mm[16];
  glGetDoublev(GL_MODELVIEW_MATRIX,mm);
  printf("Die Mmatrix ist:\n%9.6f %9.6f %9.6f %9.6f\n%9.6f %9.6f %9.6f %9.6f\n%9.6f %9.6f %9.6f %9.6f\n%9.6f %9.6f %9.6f %9.6f\n",
		  mm[0],mm[1],mm[2],mm[3],
		  mm[4],mm[5],mm[6],mm[7],
		  mm[8],mm[9],mm[10],mm[11],
		  mm[12],mm[13],mm[14],mm[15]);
  GLdouble det=
	  mm[0]*mm[5]*mm[10] - mm[8]*mm[5]*mm[2]+
	  mm[1]*mm[6]*mm[8]  - mm[9]*mm[6]*mm[0]+
	  mm[2]*mm[4]*mm[9]  - mm[10]*mm[4]*mm[1];
  printf("die Determinante der Drehmatrix ist: %g\n",det);
  glGetDoublev(GL_PROJECTION_MATRIX,mm);
  printf("Die Pmatrix ist:\n%9.6f %9.6f %9.6f %9.6f\n%9.6f %9.6f %9.6f %9.6f\n%9.6f %9.6f %9.6f %9.6f\n%9.6f %9.6f %9.6f %9.6f\n",
		  mm[0],mm[1],mm[2],mm[3],
		  mm[4],mm[5],mm[6],mm[7],
		  mm[8],mm[9],mm[10],mm[11],
		  mm[12],mm[13],mm[14],mm[15]);
  det=
	  mm[0]*mm[5]*mm[10] - mm[8]*mm[5]*mm[2]+
	  mm[1]*mm[6]*mm[8]  - mm[9]*mm[6]*mm[0]+
	  mm[2]*mm[4]*mm[9]  - mm[10]*mm[4]*mm[1];
  printf("die Determinante der Drehmatrix ist: %g\n",det);
#endif
}

void CubeGL::along001(){
  double va=vangle;
  setViewAngle(29.0);
  MM[0]=1.0;
  MM[1]=0.0;
  MM[2]=0.0;
  MM[4]=0.0;
  MM[5]=-1.0;
  MM[6]=0.0;
  MM[8]=0.0;
  MM[9]=0.0;
  MM[10]=-1.0;
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd(MM);
  setViewAngle(va);
}

void CubeGL::along010(){
    double va=vangle;
    setViewAngle(29.0);
  MM[0]=0.0;
  MM[1]=0.0;
  MM[2]=-1.0;
  MM[4]=-1.0;
  MM[5]=0.0;
  MM[6]=0.0;
  MM[8]=0.0;
  MM[9]=1.0;
  MM[10]=0.0;
  glLoadMatrixd(MM);
  setViewAngle(va);
}

void CubeGL::along100(){
    double va=vangle;
    setViewAngle(29.0);
  MM[0]=0.0;
  MM[1]=-1.0;
  MM[2]=0.0;
  MM[4]=0.0;
  MM[5]=0.0;
  MM[6]=-1.0;
  MM[8]=1.0;
  MM[9]=0.0;
  MM[10]=0.0;
  glLoadMatrixd(MM);
  setViewAngle(va);
}

void CubeGL::draw() {
//  printf("%s\n",(pause)?"paused":"drawing");
    if (pause) return;
  if (depthCueing) glEnable(GL_FOG);
  else glDisable(GL_FOG);
  int drawopt=(drawAt&1)|(drawBo<<1)|(drawUz<<2)|(elli<<3)|(drawLa<<4);
  static int Pers;
  extern QList<INP> xdinp;
  extern molekul mol;
  extern QList<Modulat> matoms;
  double gmat[16];
  static double max=0;

  if (mol.polyeders.isEmpty()){
      showPolys->setVisible(false);
      showPolys->setChecked(false);
  }else {
      showPolys->setVisible(true);
  }
  if ((moliso)&&(moliso->mibas)){
    glGetDoublev( GL_MODELVIEW_MATRIX, (double*)gmat );
    max= (fabs(gmat[2])>fabs(gmat[6]))?gmat[2]:gmat[6];
    max=(fabs(max)<fabs(gmat[10]))?gmat[10]:max; 
    if ((max==gmat[2])&&(max>0.0)) Pers=1; else 
      if ((max==gmat[2])&&(max<0.0)) Pers=2; else 
	if ((max==gmat[6])&&(max>0.0)) Pers=3; else 
	  if ((max==gmat[6])&&(max<0.0)) Pers=4; else 
	    if ((max==gmat[10])&&(max>0.0)) Pers=5; else 
	      if ((max==gmat[10])&&(max<0.0)) Pers=6;
  }
  V3 sumse=V3(0,0,0);
  V3 ori=V3(0,0,0);///rotation origin
  if ((iSel)&&(ibas)){    
    if ((mol.firstHL!=0)||(0!=mol.lastHL)){
      for (int i=mol.firstHL;i<=mol.lastHL;i++)
	sumse+=xdinp[i].kart;
      sumse*=1.0/(mol.lastHL-mol.firstHL+1.0);    
      glGetDoublev( GL_MODELVIEW_MATRIX, (double*)gmat );
      ori.x=gmat[0] * sumse.x + gmat[4] * sumse.y + gmat[8] * sumse.z;
      ori.y=gmat[1] * sumse.x + gmat[5] * sumse.y + gmat[9] * sumse.z;
      ori.z=gmat[2] * sumse.x + gmat[6] * sumse.y + gmat[10] * sumse.z;    
    }
    else iSel=0;
  }else {
    if (!matoms.isEmpty()) {
      for (int i=0;i<matoms.size();i++){
    //    printf("%9g %9g %9g\n",sumse.x,sumse.y,sumse.z);
      sumse+=matoms[i].kart(0.0);
      }
    sumse*=1.0/qMax(matoms.size(),1);
    }
    else {
    for (int i=0;i<xdinp.size();i++)
      sumse+=xdinp[i].kart;
    sumse*=1.0/qMax(xdinp.size(),1);
    }
//    qDebug()<<sumse.x<<sumse.y<<sumse.z<<xdinp.size()<<masymmUnit.size();
    double gmat[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, (double*)gmat );
    ori.x=gmat[0] * sumse.x + gmat[4] * sumse.y + gmat[8] * sumse.z;
    ori.y=gmat[1] * sumse.x + gmat[5] * sumse.y + gmat[9] * sumse.z;
    ori.z=gmat[2] * sumse.x + gmat[6] * sumse.y + gmat[10] * sumse.z;    
  }
  if (centerSelection->isChecked()){
    if (selectedAtoms.isEmpty()) centerSelection->setChecked(false);
    else {
        sumse=V3(0,0,0);
        for (int i=0;i<selectedAtoms.size();i++){
            sumse+=selectedAtoms[i].kart;
        //    printf("%s %d %g %g %g\n",selectedAtoms.at(i).atomname,i, selectedAtoms.at(i).kart.x, selectedAtoms.at(i).kart.y, selectedAtoms.at(i).kart.z);
        }
      sumse*=1.0/selectedAtoms.size();

      glGetDoublev( GL_MODELVIEW_MATRIX, (double*)gmat );
      ori.x=gmat[0] * sumse.x + gmat[4] * sumse.y + gmat[8] * sumse.z;
      ori.y=gmat[1] * sumse.x + gmat[5] * sumse.y + gmat[9] * sumse.z;
      ori.z=gmat[2] * sumse.x + gmat[6] * sumse.y + gmat[10] * sumse.z;
    }
  }
  int rmode;
  glGetIntegerv(GL_RENDER_MODE,&rmode);

  if (rotze>-1) {
    sumse=(xdinp.isEmpty())?(matoms.isEmpty())?V3(0,0,0):matoms[rotze].kart(tvalue):xdinp.at(rotze).kart;
//    printf("%s %g %g %g\n",xdinp.at(rotze).atomname, xdinp.at(rotze).kart.x, xdinp.at(rotze).kart.y, xdinp.at(rotze).kart.z);
    double gmat[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, (double*)gmat );
    ori.x=gmat[0] * sumse.x + gmat[4] * sumse.y + gmat[8] *  sumse.z;
    ori.y=gmat[1] * sumse.x + gmat[5] * sumse.y + gmat[9] *  sumse.z;
    ori.z=gmat[2] * sumse.x + gmat[6] * sumse.y + gmat[10] * sumse.z;    
  }
  if ((matoms.isEmpty())&&xdinp.isEmpty()){
    V3 mittemang=V3(0.5,0.5,0.5);
    mol.frac2kart(mittemang,sumse);
    double gmat[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, (double*)gmat );
    ori.x=gmat[0] * sumse.x + gmat[4] * sumse.y + gmat[8] *  sumse.z;
    ori.y=gmat[1] * sumse.x + gmat[5] * sumse.y + gmat[9] *  sumse.z;
    ori.z=gmat[2] * sumse.x + gmat[6] * sumse.y + gmat[10] * sumse.z;    

  }

  QFont nonAtomFont=QFont(myFont);
  nonAtomFont.setPointSize(myFont.pointSize()/2);
  glPushMatrix();
  if (back_grad&&(rmode==GL_RENDER)){ 
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 29, (double)_win_width/_win_height, 5.0, 8000.0 );
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable( GL_LIGHTING ); 
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );
    glBegin(GL_QUADS);
    double xx = ((double) _win_width/_win_height) * 2.0,
	   yy = 1.77777777778;
    glColor4f(1.0,1.0,1.0,0.5);
    //glTexCoord2d(-1,-1);
    glVertex3f(-xx,-yy,-6.0);
    glColor4f(1.0,1.0,1.0,0.5);
    //glTexCoord2d(0,-1);
    glVertex3f( xx,-yy,-6.0);
    //glTexCoord2d(0,0);
    glColor4f(0.3,0.3,0.3,0.7);
    glVertex3f( xx, yy,-6.0);
    glColor4f(0.3,0.3,0.3,0.7);
    //glTexCoord2d(-1,0);
    glVertex3f(-xx, yy,-6.0);
    glEnd();
    glColor4f(1.0,1.0,1.0,0.5);
    if (!matoms.isEmpty())  renderText(-xx*0.85,yy*0.95,-6.9,QString("t0 = %1 %2").arg(tvalue,10,'f',4).arg(mol.bcnt) ,MLegendFont);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(  vangle, (double)_win_width/_win_height, 5.0, 8000.0 );
    glMatrixMode(GL_MODELVIEW);
    glEnable( GL_LIGHTING ); 
    glEnable( GL_DEPTH_TEST ); 
    glPopMatrix();
  }  
/*{
    double mat[16];
    //  else
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnable(GL_COLOR_MATERIAL);
    glGetDoublev( GL_MODELVIEW_MATRIX, (double*)mat );
    glLoadIdentity();
    glDisable( GL_LIGHTING ); 
    glDisable( GL_DEPTH_TEST ); 
    glBegin(GL_QUADS);
    glColor4f(1.0,1.0,1.0,0.5);
    glTexCoord2d(-1,-1);
    glVertex3f(-3,-1.6,-6.0);
    glTexCoord2d(0,-1);
    glVertex3f( 3,-1.6,-6.0);
    glTexCoord2d(0,0);
    glColor4f(0.3,0.3,0.3,0.7);
    glVertex3f( 3, 1.6,-6.0);
    glTexCoord2d(-1,0);
    glVertex3f(-3, 1.6,-6.0);
    glEnd();
    glEnable( GL_LIGHTING ); 
    glEnable( GL_DEPTH_TEST ); 
    glLoadMatrixd(mat);
  }*/  
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
  glPushMatrix();
  if (!(altemitte==sumse)){
//    printf("neuemitte %f %f %f\n",sumse.x,sumse.y,sumse.z);
    if (finite(Norm(sumse))) altemitte=sumse;
    emit neuemitte(altemitte);
  }

  glTranslateL(-L*ori.x,-L*ori.y,-L*ori.z);
  if ((ibas)&&(iSel)&&(rmode==GL_RENDER)){
    glDisable(GL_BLEND);
    glEnable(GL_COLOR_MATERIAL);
    glCallList(ibas);
    glColor4f(0.5,0.5,0.5,0.3);
    glEnable(GL_BLEND);
    glDisable(GL_COLOR_MATERIAL);
  } else {
    glDisable(GL_BLEND);
    glEnable(GL_COLOR_MATERIAL);
  }  
  if ((moving->isActive())) {

      if (drawHb) callList(bas+5);
      if (drawBo) {
     if (mol.bondColorStyle) {
       qglColor(mol.bondColor);
       callList(bas+8);
     }
     else callList(bas+1);
   }
/*
      if (pole.size()>0){
        glPushMatrix();
        glScaled( L, L, L );
        //glTranslateL();

        dieDiPole(sumse);
        glPopMatrix();
      }
      if (drawAx) callList(bas+2);
      if (drawUz) callList(bas+3);
      if (foubas[0]|foubas[1]|foubas[2]|foubas[3]|foubas[4]) {
        if ((MIS)&&(moliso->mibas)) glClear( GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        glDisable(GL_LIGHTING);
        glLineWidth(0.5);
        glEnable(GL_BLEND);
        if (fofcact->isChecked()) {
            glCallList(foubas[0]);
            glCallList(foubas[1]);
        }
        if (foact->isChecked()) glCallList(foubas[2]);
        if (f1f2act->isChecked()){
            glCallList(foubas[3]);
            glCallList(foubas[4]);
        }
        glEnable(GL_LIGHTING);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);*/
      //}
   //qDebug()<<wirbas<<glIsList(wirbas+1)<<glIsList(wirbas);
   //if (glIsList(bas+6)) callList(bas+6);
  }
  else {
  if (bas) {
    glDisable(GL_CULL_FACE);
 //   if (Luftschlange) callList(bas+6);
    glEnable(GL_CULL_FACE);
    if (drawHb) callList(bas+5);
    if (cbas) glCallList(cbas);
    if (drawBo) {
      if (mol.bondColorStyle) {
	qglColor(mol.bondColor);
	callList(bas+8);
      }
      else callList(bas+1);
    }
    if (drawAt) {
      if (elli) {
	callList(bas);
	glEnable(GL_BLEND);
	callList(bas+7);
	if ((ibas)&&(iSel)) ;
	else glDisable(GL_BLEND);
      }else {
	if (mol.tubifiedAtoms)callList(bas+9);
	else callList(bas+4);
      }


    } 
    glEnable(GL_BLEND);
  }else{
  glPushMatrix();
  glEnable(GL_LIGHTING);
  glScaled( L, L, L );
  if (!matoms.isEmpty()) mol.modulated(tvalue,matoms,drawopt,tstep);
//if (drawBo) mol.modulated
  glPopMatrix();
  }
if (!selectedAtoms.isEmpty()){
    int at = mol.tubifiedAtoms;
	int ae = mol.adp;

    glPushMatrix();{
      glScaled( L, L, L );
      mol.tubifiedAtoms=0;
      mol.intern=1;
      mol.adp=0;
      mol.dratom=1;
      mol.atoms(selectedAtoms,50);
      mol.dratom=0;
    }glPopMatrix();
    mol.tubifiedAtoms=at;
    mol.adp=ae;
  }
  }
    if (rmode==GL_RENDER){

      if (!mol.wombats.isEmpty()){
        glDisable(GL_LIGHTING);
        glLineWidth(3.5);
        glEnable(GL_BLEND);
        glColor4f(tCR,tCG,tCB,tCA);
        int cntwmbts=0;
        glPushMatrix();glScaled( L, L, L );
        for (int wb=0;wb<mol.wombats.size();wb++){
          glBegin(GL_LINE_STRIP);
          for (int bp=0; bp<mol.wombats.at(wb).pth.size(); bp++){
            glVertex3d(mol.wombats.at(wb).pth.at(bp).x,mol.wombats.at(wb).pth.at(bp).y,mol.wombats.at(wb).pth.at(bp).z);
            cntwmbts++;
          }
          glEnd();
        }
        glPopMatrix();
        glEnable(GL_LIGHTING);
        //printf("%d wombats\n",cntwmbts);
      }
      if ((bas)&&(drawAx)) callList(bas+2);
      if ((bas)&&(drawUz)) callList(bas+3);
      if ((MIS)&&(moliso->mibas)) { 
	glDisable(GL_CULL_FACE);
        if (moving->isActive()||chicken->isChecked()) {Pers=1; glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);}
	else glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	if (molisoTransparence) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);
	if ((!moving->isActive())&&(zebra)){
	  GLubyte contours[2049];
	  for (int i=0;i<512;i++){
	    contours[4*i]=  ((i%cdens)>cwid)?0xff:0x00;
	    contours[4*i+1]=((i%cdens)>cwid)?0xff:0x00;
	    contours[4*i+2]=((i%cdens)>cwid)?0xff:0x00;
	    contours[4*i+3]=((i%cdens)>cwid)?0xff:0xff;
	  }
	  glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	  glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	  glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	  glTexImage1D( GL_TEXTURE_1D, 0,GL_RGBA, 512, 0,
			  GL_RGBA, GL_UNSIGNED_BYTE, contours );
	  glBindTexture(GL_TEXTURE_1D,contours[0]);
	  glEnable(GL_TEXTURE_1D);
	}
	if (faceCull) glEnable(   GL_CULL_FACE);
	if (faceCull==2) glCullFace(GL_FRONT);
	if (faceCull==1) glCullFace(GL_BACK);
	switch (Pers) {
		case 1: {callList(moliso->mibas);break;}
		case 2: {callList(moliso->mibas+1);break;}
		case 3: {callList(moliso->mibas+2);break;}
		case 4: {callList(moliso->mibas+3);break;}
		case 5: {callList(moliso->mibas+4);break;}
		case 6: {callList(moliso->mibas+5);break;}
		default: qDebug()<<"Impossible Orientation!!!";exit(1);
	}      
	glDisable(GL_TEXTURE_1D);
	glDisable(   GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	if (MILe){
	  GLfloat fw;
	  glPushMatrix();
	  double mat[16];
	  glEnable(GL_COLOR_MATERIAL);
	  glGetDoublev( GL_MODELVIEW_MATRIX, (double*)mat );
	  glLoadIdentity();
	  glDisable( GL_LIGHTING ); 
	  glDisable( GL_DEPTH_TEST ); 
	  if (zebra)  glEnable(GL_TEXTURE_1D);
	  glBegin( GL_TRIANGLE_STRIP );
	  for (int j=0;j<256;j++){	
	    moliso->Farbverlauf((0.0039*j*(moliso->max-moliso->min))+moliso->min);
	    glTexCoord1f(0.0039*j);
	    if (horizont) {
	      glVertex3f(mil.y+0.006640625*j*milsize,mil.x+0.05*milsize,-6.1);
	      glVertex3f(mil.y+0.006640625*j*milsize,mil.x     ,-6.1);
	    }else{
	      glVertex3f(mil.x+0.05*milsize,mil.y+0.006640625*j*milsize,-6.1);
	      glVertex3f(mil.x,mil.y+0.006640625*j*milsize,-6.1);
	    }
	  }
	  glEnd();
	  QRect R;
	  QFontMetrics FM(MLegendFont);
	  QString lab="AAA";
	  R= FM.boundingRect(lab);

	  for (int i=0;i<7;i++) {
	    fw=(0.1666666666666667*i*(moliso->max-moliso->min)) + moliso->min;
            if (moliso->thisIsPDF)
	    lab = QString("%1%2").arg(fw,4,'f',0).arg("%");
            else lab = QString("%1").arg(fw,7,'f',3);
	    R= FM.boundingRect(lab);
	    if (!monochrom)  moliso->Farbverlauf(fw); 
	    else glColor4f(tCR,tCG,tCB,tCA);
	    glDisable(GL_BLEND);
	    if (horizont) renderText(mil.y+0.275*i*milsize+0.00,mil.x+0.07*milsize,-6.1,lab ,MLegendFont);
	    else{
	      if (mil.x<0) renderText(mil.x+0.07*milsize,mil.y+0.275*i*milsize+0.0,-6.1,lab,MLegendFont);
	      else {
		renderText(mil.x-(0.18*vangle/_win_width *R.width()),mil.y+0.275*i*milsize+0.0,-6.1,lab ,MLegendFont);
	      }
	    }	  
	  }

	  glEnable( GL_LIGHTING ); 
	  glEnable( GL_DEPTH_TEST ); 
	  glLoadMatrixd(mat);
	  glDisable(GL_TEXTURE_1D);
	  glPopMatrix();
	}
	glEnable(GL_CULL_FACE);
      }
      // */ 

      if (pole.size()>0){ 
	glPushMatrix();
        glScaled( L, L, L );
    dieDiPole(sumse);
	glPopMatrix();
      }
      if (foubas[0]|foubas[1]|foubas[2]|foubas[3]|foubas[4]) {
        if ((MIS)&&(moliso->mibas)) glClear( GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        glDisable(GL_LIGHTING);
        glLineWidth(0.5);
        glEnable(GL_BLEND);
        if (fofcact->isChecked()) {
            glCallList(foubas[0]);
            glCallList(foubas[1]);
        }
        if (foact->isChecked()) glCallList(foubas[2]);
        if (f1f2act->isChecked()){
            glCallList(foubas[3]);
            glCallList(foubas[4]);
        }
        glEnable(GL_LIGHTING);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      }
      if ((showPolys->isChecked())&&(mol.knopf_made>0)){
         // glClear( GL_DEPTH_BUFFER_BIT);
    glDisable(   GL_CULL_FACE);
    glPushMatrix();
    glScaled(L,L,L);
    mol.draw_polyeders(xdinp);
    glPopMatrix();
    glEnable(   GL_CULL_FACE);
    glEnable(GL_BLEND);
    }
      if (((!moving->isActive())||(!chicken->isChecked()))&&(drawLa)) {
	glClear( GL_DEPTH_BUFFER_BIT);
	glPushMatrix();{
	  glScaled(L,L,L);


	  for (int j=0;j<xdinp.size();j++){

	    if (imFokus==j) qglColor(Qt::yellow); else  glColor4f(tCR,tCG,tCB,tCA);
	    if (((iSel)&&(mol.firstHL<=j)&&(mol.lastHL>=j))||(!iSel)){
	      if (xdinp[j].OrdZahl<0) renderText( xdinp[j].labPos.x,xdinp[j].labPos.y,xdinp[j].labPos.z, xdinp[j].atomname,nonAtomFont);
	      else 
		if ((imFokus==j)||(!(mol.aStyle[xdinp[j].OrdZahl]&ATOM_STYLE_NOLABEL)))
		  renderText( xdinp[j].labPos.x,xdinp[j].labPos.y,xdinp[j].labPos.z, xdinp[j].atomname,myFont);
	    }
	  }    
	  for (int j=0;j<matoms.size();j++){
	    if (imFokus==j) qglColor(Qt::yellow); else  glColor4f(tCR,tCG,tCB,tCA);
            if ((imFokus==j)||(!(mol.aStyle[matoms[j].OrdZahl]&ATOM_STYLE_NOLABEL))){
              V3 lpos=matoms[j].kart(tvalue);
              renderText( lpos.x,lpos.y,lpos.z, matoms[j].atomname,myFont);
            }
          }
	}glPopMatrix();
      }
    {
        glPushMatrix();
        glScaled(L,L,L);{

            GLdouble model[16];
            GLdouble proj[16];
            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);
            glGetDoublev( GL_PROJECTION_MATRIX , (double*)proj );
            glGetDoublev( GL_MODELVIEW_MATRIX, (double*)model );
            for (int j=0;j<xdinp.size();j++){
	      if (((iSel)&&(mol.firstHL<=j)&&(mol.lastHL>=j))||(!iSel)){
		if (!posTo2D(xdinp.at(j).kart,model,proj,viewport, &xdinp[j].screenX, &xdinp[j].screenY))
                {xdinp[j].screenX=-200; xdinp[j].screenY=-200;}
            }
	    }
            for (int j=0;j<matoms.size();j++){
	      if (((iSel)&&(mol.firstHL<=j)&&(mol.lastHL>=j))||(!iSel)){
		if (!posTo2D(matoms[j].kart(tvalue),model,proj,viewport, &matoms[j].screenX, &matoms[j].screenY))
                {matoms[j].screenX=-200; matoms[j].screenY=-200;}
            }
	    }
        }
        glPopMatrix();
    }
    } 
  
    glPopMatrix();
}

void CubeGL::dieDiPole(V3 org){
  int size=pole.size(),fsz=farben.size();
  V3 vec,p,VZ;
  VZ.x=0;VZ.y=0;VZ.z=1;
  QFont nonAtomFont=QFont(myFont);
  nonAtomFont.setPointSize(myFont.pointSize()/2);
  double wink,gg;
  glDisable(   GL_CULL_FACE);
  for (int i=0;  i<size;i++){
      glPushMatrix();
      if (Norm(poleOrg.at(i))>500) glTranslated(org.x, org.y, org.z);
      else glTranslated(poleOrg.at(i).x, poleOrg.at(i).y, poleOrg.at(i).z);
    p=pole.at(i);
    vec=molekul::kreuzX(pole.at(i).x,pole.at(i).y,pole.at(i).z,0.0f,0.0f,1.0f);
    wink=molekul::winkel(pole.at(i),VZ);
    gg=sqrt(Norm(p));{
      if (Norm(vec)==0) vec=V3(1.0,0,0);
      glPushMatrix();

      if (fsz>i)qglColor(farben.at(i)); 
      else glColor3f(0.4,0.3,0.1);
      glRotatef(wink,vec.x,vec.y,vec.z); // drehen
      GLUquadricObj *q = gluNewQuadric();
      gluQuadricNormals(q, GL_SMOOTH); 

      gluCylinder(q, 0.02, 0.02, (float)gg*.9, 8, 50);
      glPopMatrix();
    }
    glPushMatrix();
    glTranslated(p.x*.8,p.y*.8,p.z*.8)  ;
    glRotatef(wink,vec.x,vec.y,vec.z); // drehen
    GLUquadricObj *q = gluNewQuadric();
    GLUquadricObj *q2 = gluNewQuadric();
    gluQuadricNormals(q, GL_SMOOTH); 
    gluQuadricNormals(q2, GL_SMOOTH); 
    if (fsz>i) qglColor(farben.at(i));
    else glColor3f(0.7,0.3,0.1);
    gluCylinder(q, 0.06, 0.0003, gg/5.0, 8, 50);
    gluDisk(q2, 0, 0.06, 8, 1);
    glPopMatrix();
    if (drawLa) renderText( pole.at(i).x,pole.at(i).y,pole.at(i).z,QString("Dipole_%1").arg(i),nonAtomFont);

    glPopMatrix();
  }

  glEnable(   GL_CULL_FACE);
}


void CubeGL::updateBondActions(){
  clearSelection->setVisible(!selectedAtoms.isEmpty());
  centerSelection->setVisible(!selectedAtoms.isEmpty());
  extern molekul mol;
  delCoordi->setVisible(!mol.cBonds.isEmpty());


  if (selectedAtoms.size()!=2){
    addBond->setVisible(false);
    killBond->setVisible(false);
    addCoordi->setVisible(false);
  }
  else{
    int da=0;
    addCoordi->setVisible(true);
    extern molekul mol;
    for (int i=0; i<mol.bcnt;i++){
      if ((((int)selectedAtoms.at(0).GLname==mol.bd[i].a)||
			      ((int)selectedAtoms.at(0).GLname==mol.bd[i].e))&&
		      (((int)selectedAtoms.at(1).GLname==mol.bd[i].a)||
		       ((int)selectedAtoms.at(1).GLname==mol.bd[i].e))) da=i;
    }
    if (da){
      addBond->setVisible(false);
      killBond->setVisible(true);
      killBond->setData(da);
    }
    else{
      addBond->setVisible(true);
      killBond->setVisible(false);
    }
  }
}

void CubeGL::disSelection(){
  selectedAtoms.clear();
  updateBondActions();
  updateGL();
}


void CubeGL::connectSelection(){
  if (selectedAtoms.size()!=2) return;
  extern molekul mol;
  extern QList<INP> xdinp;
  int idx= mol.bcnt;
  mol.bcnt+=2;
  mol.bd=(bindi*)realloc(mol.bd,sizeof(bindi)*mol.bcnt);
  mol.bd[idx].a=selectedAtoms.at(0).GLname;
  mol.bd[idx].e=selectedAtoms.at(1).GLname;
  idx++;
  mol.bd[idx].e=selectedAtoms.at(0).GLname;
  mol.bd[idx].a=selectedAtoms.at(1).GLname;
  bool vorher=mol.bondColorStyle;
  glNewList(bas+1, GL_COMPILE );{      //BONDS
    glPushMatrix();{
      glScaled( L, L, L );
      mol.adp=0;      
      mol.bondColorStyle=false;
      mol.bonds(xdinp);
    }glPopMatrix();    
  }glEndList();
  glNewList(bas+8, GL_COMPILE );{       //bonds in single color
    glPushMatrix();{
      glScaled( L, L, L );
      mol.intern=1;

      mol.bondColorStyle=true;
      mol.bonds(xdinp);
    }glPopMatrix();
  }glEndList();
  mol.bondColorStyle=vorher;
  selectedAtoms.clear();
  updateBondActions();
  updateGL();
}

void CubeGL::disConnectSelection(){
  if (selectedAtoms.size()!=2) return;
  extern molekul mol;
  extern QList<INP> xdinp;
  static bindi *nbd,*abd;
  abd=mol.bd;
  if (NULL==(nbd=(bindi*)malloc(sizeof(bindi)*mol.bcnt-2))) return;
  int j=0;
  for (int i=0; i<mol.bcnt;i++){
      if ((((int)selectedAtoms.at(0).GLname==mol.bd[i].a)||
			      ((int)selectedAtoms.at(0).GLname==mol.bd[i].e))&&
		      (((int)selectedAtoms.at(1).GLname==mol.bd[i].a)||
		       ((int)selectedAtoms.at(1).GLname==mol.bd[i].e))) ; else {
	nbd[j].a=mol.bd[i].a;
	nbd[j].e=mol.bd[i].e;
        j++;
      }  
  }
 mol.bcnt-=2;
 mol.bd = nbd;
 free(abd);
  bool vorher=mol.bondColorStyle;
  glNewList(bas+1, GL_COMPILE );{      //BONDS
    glPushMatrix();{
      glScaled( L, L, L );
      mol.adp=0;      
      mol.bondColorStyle=false;
      mol.bonds(xdinp);
    }glPopMatrix();    
  }glEndList();
  glNewList(bas+8, GL_COMPILE );{       //bonds in single color
    glPushMatrix();{
      glScaled( L, L, L );
      mol.intern=1;

      mol.bondColorStyle=true;
      mol.bonds(xdinp);
    }glPopMatrix();
  }glEndList();
  mol.bondColorStyle=vorher;
  selectedAtoms.clear();
  updateBondActions();
  updateGL();

}

void CubeGL::coordinativeBonds(){
  if (selectedAtoms.size()!=2) return;
  bindi bond;
  extern molekul mol;
  extern QList<INP> xdinp;
  bond.a=selectedAtoms.at(0).GLname;
  bond.e=selectedAtoms.at(1).GLname;
  mol.cBonds.append(bond);
  if (!cbas) cbas=glGenLists(1);
  glNewList(cbas, GL_COMPILE );
  glPushMatrix();
  glScaled( L, L, L );
  mol.cbonds(xdinp);
  glPopMatrix();
  glEndList();
  selectedAtoms.clear();
  updateBondActions();
  updateGL();
}

void CubeGL::delCoordinative(){
  extern molekul mol;
  mol.cBonds.clear();
  glDeleteLists(cbas,1);
  cbas=0;
  selectedAtoms.clear();
  updateBondActions();
  updateGL();
}
