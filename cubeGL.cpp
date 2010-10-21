// beispiele/opengl/mainWindow.cpp
#include <QtGui>
#include <QtOpenGL>
#include <cmath>
#include "cubeGL.h"
#include "invariomDlg.h"  
#include "XDDlg.h"  
#include "inames.h"
V3 mil;

CubeGL::CubeGL(QWidget *parent) : QGLWidget(parent) {
   setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer) );
   atomsClickable=true;
   faceCull=0;
   monochrom=false;
   reSe=false;
   rotze=-1;
   imFokus=-1;
   cdens=16;
   cbas=0;
   stereo_mode=0;
   awidth=cwid=1;
   milsize=1.0;
   molisoTransparence=zebra=true;
   drawAt=drawBo=drawLa=drawHb=true;
   drawAx=drawUz=false;
   moveLab=false;
   moveLeg=false;
   elli=true;
   //texti=0;
   moliso=NULL;
   mlsc=1.0;
   //Tex1FileName="";
   //Tex2FileName="";
   invEditAble=false;
   xdSetupMode=false;
   Luftschlange=true;
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
   mil.x=-2.3;
   mil.y=-0.3;
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
    milsize=size/30.0;
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
//    printf("%d %d %d\n",cwid,width,(cwid+1)*256/cdens);
    updateGL();
}

void CubeGL::togglContours(bool b){
    zebra=b;
    updateGL();
}

void CubeGL::togglMonoChrom(bool b){
 monochrom=b;
 updateGL();
}
void CubeGL::saveMISettings(){
  QString fn=QFileDialog::getSaveFileName(this, tr("Save MoleCoolQt MolIso Settings file "), "MoleCoolQt.moliso.ini",
		                         "MoleCoolQt.moliso.ini (*.ini);;");

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
   mconf.write(QString("Draw helices: %1\r\n").arg(Luftschlange).toLatin1());
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
		                         "MoleCoolQt.moliso.ini (*.ini);;");

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
  Luftschlange = all.section(QRegExp("[:\r]"),41,41).toInt();
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
  setMatrix();
  emit mconf();
  updateGL();
}

void CubeGL::setFont(){
  bool ok;
  myFont = QFontDialog::getFont(&ok, myFont, this,"Change Label Font");
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

 /*
void CubeGL::changeTexture(){
  texti=(texti+1)%3;
  //  QMessageBox::information(this,"Textur number",QString("No. %1").arg(texti),QMessageBox::Ok);  
  updateGL();
}
*/

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
 // updateGL();
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
    "Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","J","Xe",
    "Cs","Ba", "La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu",
    "Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra",
    "Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No","Lr","Ku","Ha","Rf","Ns","Hs","Mt"};
  for (int ix=0;ix<asymmUnit.size();ix++){
    MyAtom invom;
    double soll_abst,gg;
    MyBond bond;
    if (ce.size()==0){
      //      qDebug()<<"lhkmnsdfmfgn,";
      for (int i=0;i<xdinp.size();i++)
	if (xdinp[i].OrdZahl>-1) {
	  invom.Label=xdinp[i].atomname;
	  invom.pos=xdinp[i].kart;
	  invom.an=xdinp[i].OrdZahl;
	  invom.part=xdinp[i].part;
	  invom.Symbol=PSE_Symbol[xdinp[i].OrdZahl];
	  ce.append(invom);
	}
    }
    if ((bondList.size()!=0)) {
      cl=bondList;
      // qDebug()<<bondList.size();
    }
    else {
      for (int i=0;i<ce.size();i++) {
	for (int j=i+1;j<ce.size();j++){
	  //int i=0;
	  if ((ce.at(i).an<0)||(ce.at(j).an<0)) continue;
	  if (ce.at(i).pos==ce.at(j).pos) continue;
	  if ((ce.at(i).part>0)&&(ce.at(j).part>0)&&(ce.at(j).part!=ce.at(i).part)) continue;
	  if ((ce.at(i).an<83)&&(ce.at(j).an<83)&&(ce.at(i).an>=0)&&(ce.at(j).an>=0)){
	    soll_abst=((mol.Kovalenz_Radien[ce.at(i).an]+
				    mol.Kovalenz_Radien[ce.at(j).an])/100.0
			    -(0.08*fabs(((double)mol.ElNeg[ce.at(i).an]
						    -mol.ElNeg[ce.at(j).an])/100.0)));
	    gg=sqrt( Distance(ce.at(i).pos,ce.at(j).pos));
	    if (gg<soll_abst*1.2) {
	      bond.ato1=&ce.at(i);
	      bond.ato2=&ce.at(j);
	      bond.length=gg;
	      bond.chi=soll_abst-gg;
	      bond.order=(bond.chi<0.0847)?1:((bond.chi<0.184)?2:((bond.chi<0.27)?3:4));
	      bond.order=((bond.ato1->Symbol=="H")||(bond.ato2->Symbol=="H"))?1:bond.order;
	      //	      qDebug()<<bond.ato1->Label<<bond.ato1->part<<"---"<<bond.ato2->Label<<bond.ato2->part;
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
    invom.Symbol=PSE_Symbol[xdinp[ix].OrdZahl];
    if (invom.an>-1 ){
      ina=inames::invName(invom,bondList,sel,ix);
    }
  }
  }
}

void CubeGL::toggXDSetup(bool on){
  xdSetupMode=on;
}

void  CubeGL::setLabels(bool on){
  drawLa=on;  
  updateGL();
}

void  CubeGL::setAxes(bool on){
  drawAx=on;  
  updateGL();
}

void CubeGL::setHelices(bool on){
  Luftschlange=on;
  updateGL();
}

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
  //deleteTexture(tex[0]);
  //deleteTexture(tex[1]);
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

//awh!=_win_width*_win_heightawh!=_win_width*_win_heig  qDebug("Texturen geladen%d %d",mol.adpwall,mol.hbtex);
  /*
  if (!Tex1FileName.isEmpty()){
    tex[0] = bindTexture(QImage(Tex1FileName),GL_TEXTURE_2D);     
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  }

  if (!Tex2FileName.isEmpty()){
    tex[1] = bindTexture(QImage(Tex2FileName),GL_TEXTURE_2D);   
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  }
  */
}

/*
void CubeGL::loadTexture() {

  QMessageBox::StandardButton B= QMessageBox::question(this,tr("Backgrond Texture Dialog"),
						       tr("Recent textures: <p>Primary texture:<br><img src=\"%1\" width=\"150\" height=\"150\"><p>Secondary texture:<br><img src=\"%2\" width=\"150\" height=\"150\"> <p><b>Would you like to load a new primary teture?</b>").arg(Tex1FileName).arg(Tex2FileName),QMessageBox::Yes|QMessageBox::No);  
  if (B==QMessageBox::No) return;
  Tex2FileName=Tex1FileName;
  Tex1FileName=QFileDialog::getOpenFileName(this, tr("Open background texture file %1").arg(B),Tex1FileName,
					    "Joint Photographic Experts Group - file (*.jpg);;"
					    "Windows Bitmap - file (*.bmp);;"
					    "Portable Network Graphics - file (*.png);;"
					    "Portable Pixmap - file (*.ppm)" );

  //setupTexture();
  updateGL();

}*/

void CubeGL::changeBColor() {
  qreal bgRed,bgGreen,bgBlue,bgAlpha;
  QColor bgcolor = QColorDialog::getColor(QColor((int)(bgCR*255),(int)(bgCG*255),(int)(bgCB*255),(int)(bgCA*255)), this);  
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
  mol.bondColor=QColorDialog::getColor(mol.bondColor,this);
  updateGL();
}

void CubeGL::changeTColor() {
  qreal bgRed,bgGreen,bgBlue,bgAlpha;
  QColor tcolor = QColorDialog::getColor(QColor((int)(tCR*255),(int)(tCG*255),(int)(tCB*255),(int)(tCA*255)), this);  
  if (tcolor.isValid()) {
    tcolor.getRgbF(&bgRed,&bgGreen,&bgBlue,&bgAlpha);
    tCR=(GLclampf)bgRed;
    tCG=(GLclampf)bgGreen;
    tCB=(GLclampf)bgBlue;
    tCA=(GLclampf)bgAlpha;
  }
}

void CubeGL::initializeGL() {
  glEnable(GL_LINE_SMOOTH);   
  glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
  //glEnable(GL_POLYGON_SMOOTH);   
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

  //glDisable(GL_FOG);
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

   //glClearDepth(1.0f);
   glEnable(GL_DEPTH_TEST );
   glDepthFunc(GL_LEQUAL);
   glEnable(GL_CULL_FACE);
   gluLookAt(0.0, 200, 50 ,   0.0, 0.0, 0.0,   0.0, -100, 400 );
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
   setMouseTracking(true); 

  extern QList<INP> xdinp;
  extern double L;
  extern molekul mol;
  setupTexture();
  //888
  
  //  QFile bld("hirnzirrose");
  // bld.open(QIODevice::Append);
  if (!xdinp.isEmpty()) {
    int adpstate=mol.adp;
    glLoadMatrixd(MM);
   if ((moliso)&&(moliso->mibas)){
      moliso->loadMI(moliso->faceFile); 
   }
   //if (!bas) bas=glGenLists(8);
      glNewList(bas, GL_COMPILE );{                          //ATOME
	glPushMatrix();{
	  glScaled( L, L, L );
	  mol.intern=1;
	  mol.adp=1;
	  mol.atoms(xdinp,mol.proba);
	}glPopMatrix();    
      }glEndList();
      

bool before=  mol.singleColorBonds;
  glNewList(bas+8, GL_COMPILE );{       //bonds in single color
    glPushMatrix();{
      glScaled( L, L, L );
      mol.intern=1;
//      cubeGL->qglColor(mol.bondColor);
      mol.singleColorBonds=true;
      mol.bonds(xdinp);
    }glPopMatrix();
  }glEndList();
  mol.singleColorBonds=before;

      //  bld.write(QString("!!-> %1 %2 ERROR%3 \n").arg(mibas).arg(bas).arg((char*)gluErrorString(glGetError())).toLatin1());
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
      //mol.adp=1;
       mol.tubifiedAtoms=true;
      mol.atoms(xdinp,mol.proba);
    }glPopMatrix();
  }glEndList();
  mol.tubifiedAtoms=before;

      // bld.write(QString("!!-> %1 %2 ERROR%3 \n").arg(mibas).arg(bas).arg((char*)gluErrorString(glGetError())).toLatin1());
      glNewList(bas+1, GL_COMPILE );{                          //BONDS
	glPushMatrix();{
	  glScaled( L, L, L );
	  mol.adp=0;      
	  mol.bonds(xdinp);
	}glPopMatrix();    
      }glEndList();

      if (drawAx) {
	glNewList(bas+2, GL_COMPILE );{                          //Axen
	  //glClear( GL_DEPTH_BUFFER_BIT);
	  glDisable( GL_DEPTH_TEST ); 
	  glPushMatrix();{
	    //      glTranslated( -L/2.0, -L/2.0, -L/2.0 );
	    glScaled( L, L, L );
	    mol.axes(xdinp); 
	  }glPopMatrix();    
	  glEnable( GL_DEPTH_TEST );
	}glEndList();
      }

      // bld.write(QString("!!-> %1 %2 axERROR%3 \n").arg(mibas).arg(bas).arg((char*)gluErrorString(glGetError())).toLatin1());
      glNewList(bas+3, GL_COMPILE );{                          //Unit Zell
	glPushMatrix();{
	  //      glTranslated( -L/2.0, -L/2.0, -L/2.0 );
	  glScaled( L, L, L );           
	  mol.UnitZell(); 
	}glPopMatrix();    
      }glEndList();

      if (mol.HAMax!=0.0){
	glNewList(bas+5, GL_COMPILE );{                          //H...X-Bindungen
	  glPushMatrix();{
	    //      glTranslated( -L/2.0, -L/2.0, -L/2.0 );
	    glScaled( L, L, L );
	    mol.h_bonds(xdinp); 
	  }glPopMatrix();    
	}glEndList();
      }

      // bld.write(QString("!!-> %1 %2 ERROR%3 \n").arg(mibas).arg(bas).arg((char*)gluErrorString(glGetError())).toLatin1());
      if (mol.nListe>2){
	glNewList(bas+6, GL_COMPILE );{        //helices
	  glPushMatrix();
	  glScaled( L, L, L );
	  mol.drawSline(mol.vL,mol.nListe);
	  glPopMatrix();
	}glEndList();
      }

      //      bld.write(QString("-> %1 %2 %3 \n").arg(mibas).arg(bas).arg(ibas).toLatin1());
      //      bld.close();  
    if (iSel){
      //qDebug()<<"before"<<mol.firstHL<<mol.lastHL;
      if (ibas) glDeleteLists(ibas,1);
      ibas=glGenLists(1);    
      //for (int i=0;i<maxResi;i++)
      glNewList(ibas, GL_COMPILE );
      extern QList<INP> xdinp;
      extern double L;
      extern molekul mol;
      mol.intern=1;
       mol.highlightResi(xdinp,mol.firstHL,L,elli);
      glEndList();	    /*
      QFile qf("murx.txt");
      qf.open(QIODevice::Append);
      qf.write(QString("\nmolhl %1 %2 %3 sum %4 sum %5 \n").arg(mol.firstHL).arg(mol.lastHL).arg(mol.smx).arg(summe).arg(mol.lastHL-mol.firstHL).toLatin1());
      qf.close();*/
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
      // printf("initGLed\n");

      //bld.write(QString("initGLed!!-> %1 %2 ERROR%3 \n").arg(mibas).arg(bas).arg((char*)gluErrorString(glGetError())).toLatin1());

      //bld.close();
  }

}

void CubeGL::resizeGL(int width, int height) {
  glViewport(0, 0, _win_width=width, _win_height=height);        
  glGetIntegerv(GL_VIEWPORT, vp);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective( 29.0, (double)_win_width/_win_height, 5.0, 8000.0 );
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
  //  glGetDoublev( GL_PROJECTION_MATRIX, (double*)mat );


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
//  if (stereo_mode==3) minussign=-1;
  if (!stereo_mode){ 
    glGetIntegerv(GL_VIEWPORT, vp);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 29.0, (double)_win_width/_win_height, 5.0, 8000.0 );
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
    //glDisable(0x809D);//  GL_MULTISAMPLE_ARB * / 
    glDisable(0x809D); // GL_MULTISAMPLE_ARB * / 

    //glDisable(GL_STENCIL_TEST);
    glClearStencil(0);
    glColorMask(false,false,false,false);
    glDepthMask(false);
    glClear(GL_STENCIL_BUFFER_BIT);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glLineWidth(1.0);
//    printf("%d %d %d %d \n",viewport[0],viewport[1],viewport[2],viewport[3]);
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
    gluPerspective( 29.0, (double)_win_width/_win_height, 5.0, 8000.0 );
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
//    glTranslateL(0,0,-150);
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
//    glTranslateL(0,0,-150);
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
      gluPerspective( 29.0, (double)(_win_width/2.0)/_win_height, 5.0, 8000.0 );
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      //glTranslateL(0,0,-150);
      glRotateL(1.5*minussign,0,1,0);
      /*  glEnable(GL_STENCIL_TEST);
	  glStencilFunc(GL_EQUAL, 1, 1);
	  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	  glEnable(GL_STENCIL_TEST);// */
      draw();
      glPopMatrix();
      glPopMatrix();

      glPushMatrix();
      //  glViewport(0, 0, _win_width, _win_height);        
      //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      //  glTranslated(0,0,-5);
      glViewport( _win_width / 2 , 0,_win_width / 2,_win_height );        
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective( 29.0, (double)(_win_width/2.0)/_win_height, 5.0, 8000.0 );
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      //glTranslateL(0,0,-150);
      glRotateL(-1.5*minussign,0,1,0);
      /*  glEnable(GL_STENCIL_TEST);
	  glStencilFunc(GL_EQUAL, 0, 1);
	  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	  glEnable(GL_STENCIL_TEST);// */
//    bool warLabel = drawLa;
//    drawLa=false;
      draw();
//    drawLa= warLabel;
      glPopMatrix();
      glPopMatrix();
    }
}

void CubeGL::loadDataBase(){
  QString fileName = QFileDialog::getOpenFileName(this,QString(tr("Open invariom data base.")), "DABA.txt","All files (*)" );
  if (!fileName.isEmpty()){
    QFile daba(fileName);
    //cout<<fileName.toStdString()<<endl;
    daba.open(QIODevice::ReadOnly);
    DABA entry;

    QString line;
    int lineCntr=-1;
    while (!daba.atEnd()){
      line = QString(daba.readLine(150));
      if (line.contains(QRegExp("^R-|S-|=-|[3-8]{1,3}-|[A-Z]{1,1}[a-z]{0,1}[123#@]{1,1}"))) {
        if ((!line.startsWith("KS"))&&(!line.startsWith("!"))) {
          lineCntr=0;
          line.remove(QRegExp("[ \n\r]"));
          dataBase.append(line);
          }
      }
      if ((lineCntr>-1)&&(lineCntr<7)){
      line.remove(QRegExp("[\n\r]"));
      //printf("test %d (%s)\n",lineCntr,line.toStdString().c_str());
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
        case 4: entry.Symmetry=tok.at(1); break;
        case 5: entry.CoordinateSystem=line; break;
        case 6: line.remove("Kappa=");
                tok = line.split(";=",QString::SkipEmptyParts);
                entry.k1=tok.at(0).toDouble();
                entry.k2=tok.at(1).toDouble();
                entry.k3=tok.at(2).toDouble();
                entry.k4=tok.at(3).toDouble();
                entry.k5=tok.at(4).toDouble();
                entry.k6=tok.at(5).toDouble();
        //        printf("%d %d %f %f %f %f %f %f\n",entries.size(),dataBase.size(),entry.k1,entry.k2,entry.k3,entry.k4,entry.k5,entry.k6);
                entries.append(entry);
             break;
        }
        lineCntr++;
      }

    }

  //qDebug()<<entries.size()<<dataBase.size();
  }
}

void CubeGL::resetENV(){
  //  qDebug()<<"reset Environ";
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
  updateGL();
  rCenter->setVisible(false);
}

void CubeGL::mousePressEvent(QMouseEvent *event) {
  lastPos = event->pos();
  //printf("Event %d %d \n",lastPos.x(),lastPos.y());
  static GLuint ppp=0,pp=0,p=0;   
  molekul m;	
  if (event->buttons() & Qt::MidButton){
    GLint hits;
    glSelectBuffer(MAXSELECT, selectBuf);
    (void)glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(~0);
    glPushMatrix();
    if (stereo_mode<2)  glViewport(0, 0, _win_width, _win_height);        
    else glViewport(0, 0, _win_width/2, _win_height);
    glGetIntegerv(GL_VIEWPORT, vp);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (stereo_mode<2) {
      gluPickMatrix(event->x(), _win_height-event->y(), 8, 8, vp);
      gluPerspective( 29.0, (double)_win_width/_win_height, 50.0, 8000.0 );
    }
    else {
      gluPickMatrix(event->x() % (_win_width/2), _win_height-event->y(), 8, 8, vp);
      gluPerspective( 29.0, (double)(_win_width / 2.0)/_win_height, 50.0, 8000.0 );
    }
    glMatrixMode(GL_MODELVIEW);
    glGetDoublev(GL_MODELVIEW_MATRIX,MM);
    draw();
    glPopMatrix();
    hits = glRenderMode(GL_RENDER); 
    extern QList<INP> xdinp;
    //extern double L;
    //extern molekul mol;
    updateGL();
    if (hits <= 0) {      
    }else 
      if ((int)selectBuf[(hits-1)*4+3]<xdinp.size()) {
	GLuint index=selectBuf[(hits-1)*4+3];
	if (index==((GLuint)-1))return;
	rotze=((int)index<xdinp.size())?index:-1;  
	if (rotze>-1) rCenter->setVisible(true);
	updateGL();

      }
  }
  if((reSe | moveLab| invEditAble | atomsClickable| xdSetupMode) && (event->buttons() & Qt::LeftButton)){

//    printf("Test\n");
    GLint hits;
    //QString text[4]={"label","yellow","blue","green"};
    //GLfloat MM[16];
    //glGetDoublev(GL_MODELVIEW_MATRIX,MM); 
    glSelectBuffer(MAXSELECT, selectBuf);
    (void)glRenderMode(GL_SELECT);
    //SelectMode=1;
    glInitNames();
    glPushName(~0);
    //WIN_COL[0]=1.0;
    glPushMatrix();
    if (stereo_mode<2)  glViewport(0, 0, _win_width, _win_height);        
    else glViewport(0, 0, _win_width/2, _win_height);
    glGetIntegerv(GL_VIEWPORT, vp);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPickMatrix(event->x(), _win_height-event->y(), 8, 8, vp);
    if (stereo_mode<2) gluPerspective( 29.0, (double)_win_width/_win_height, 50.0, 8000.0 );
    else gluPerspective( 29.0, (double)(_win_width / 2.0)/_win_height, 50.0, 8000.0 );
    glMatrixMode(GL_MODELVIEW);
    glGetDoublev(GL_MODELVIEW_MATRIX,MM);
    //     glLoadMatrixd(MM);
    draw();
    glPopMatrix();
    hits = glRenderMode(GL_RENDER); 

    //    extern int dummax;
    extern QList<INP> xdinp;
    extern double L;
    extern molekul mol;
    updateGL();
    if (hits <= 0) {      
    }else 
      if ((int)selectBuf[(hits-1)*4+3]<xdinp.size()) {
	GLuint index=selectBuf[(hits-1)*4+3];
//	printf("hits = %d index = %d xdinp.size = %d %d %d %d\n",hits,index,xdinp.size(),p,pp,ppp);
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
	  //invClick=index;
	  {
	    if (ibas) glDeleteLists(ibas,1);
	    ibas=glGenLists(1);    
	    //for (int i=0;i<maxResi;i++)
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
	  //	  QMessageBox::information(this,"Clicking...",
	//  printf("index %d %d\n",index,xdinp.size());
	//  printf("xdinp[index].sg = %d\n",xdinp[index].sg);
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

	    /*	    std::cout<<QString("Geometry:<table><tr><td><b>%1</b></td><td align=\"left\"></td></tr><tr><td><b>%2--%3</b></td><td align=\"left\">%4 &Aring;</td></tr><tr><td><b>%5--%6--%7</b></td><td align=\"left\">%8&deg;</td></tr><tr><td><b>%9--%10--%11--%12 </b></td><td align=\"left\">%13&deg;</td></tr><tr><td>PART</td><td align=\"left\">%14</td></tr><tr><td>Charge Density: </td><td align=\"left\">%15 e &sdot; &Aring;<sup>-3</sub></td></tr><table>")
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
		    .toStdString()<<std::endl;;
		    */


	  }else {
	    emit bigmessage(QString("Geometry:<table><tr><td><b>%1</b> clicked.</td><td align=\"left\"></td></tr><tr><td><b>%2--%3</b></td><td align=\"left\">%4 &Aring;</td></tr><tr><td><b>%5--%6--%7</b></td><td align=\"left\">%8&deg;</td></tr><tr><td><b>%9--%10--%11--%12 </b></td><td align=\"left\">%13&deg;</td></tr><tr><td>PART</td><td align=\"left\">%14</td></tr><table>")
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
			    .arg(xdinp[index].part));
	  }
	  //				   ,QMessageBox::Ok);
	  ppp=pp;
	  pp=p;
	  p=index;
	  if (event->modifiers()==Qt::NoModifier) {
	    selectedAtoms.clear();
	    selectedAtoms.append(xdinp[index]);
	    selectedAtoms.last().GLname=index;
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
	  /*
	  printf("Selected Atoms:\n");
	  for (int i=0; i<selectedAtoms.size();i++) {
	    printf(" %s,",selectedAtoms.at(i).atomname);
	    
	  }
	  printf("\n\n");
	  */ 
	  updateBondActions();
	}

	//888
	if ((xdinp.size())&&((invEditAble)||(xdSetupMode))) {
	  if (xdinp[index].OrdZahl<0)return;
	  int na=0;
	  for (int i=0; (i<xdinp.size())&&(xdinp[i].OrdZahl>=0); i++) na=i;
	  if (((int)index)>na) {
	    //	  printf ("index=%d na%d\n",index,na);
	    return;
	  }
	  //	  printf ("index=%d na%d\n",index,na);
	  static char PSE_Symbol[109][3] = {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar",
	    "K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr",
	    "Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","J","Xe",
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
	      ce.append(invom);
	    }
	  if ((bondList.size()!=0)) {
	    cl=bondList;
	    /*
	       for (int w=0; w<bondList.size(); w++){
	       cout<<bondList.at(w).ato1->Label.toStdString()<<"---"<<bondList.at(w).ato2->Label.toStdString()<< " "<< bondList.at(w).chi<< " "<<bondList.at(w).order<<endl;
	       }
	       cout<<"cl=bondList;"<<endl;
	       */
	  }
	  else {
	    for (int i=0;i<ce.size();i++) {
	      for (int j=i+1;j<ce.size();j++){
		//int i=0;
		if ((ce.at(i).an<0)||(ce.at(j).an<0)) continue;
		if (ce.at(i).pos==ce.at(j).pos) continue;
		if ((ce.at(i).part>0)&&(ce.at(j).part>0)&&(ce.at(j).part!=ce.at(i).part)) continue;
		if ((ce.at(i).an<83)&&(ce.at(j).an<83)&&(ce.at(i).an>=0)&&(ce.at(j).an>=0)){
		  soll_abst=((mol.Kovalenz_Radien[ce.at(i).an]+
					  mol.Kovalenz_Radien[ce.at(j).an])/100.0
				  -(0.08*fabs(((double)mol.ElNeg[ce.at(i).an]
							  -mol.ElNeg[ce.at(j).an])/100.0)));
		  gg=sqrt( Distance(ce.at(i).pos,ce.at(j).pos));
		  if (gg<(soll_abst*1.2)) {
		    bond.ato1=&ce.at(i);
		    bond.ato2=&ce.at(j);
		    bond.length=gg;
		    bond.chi=soll_abst-gg;
		    bond.order=(bond.chi<0.0847)?1:((bond.chi<0.184)?2:((bond.chi<0.27)?3:4));
		    bond.order=((bond.ato2->Symbol=="H")||(bond.ato1->Symbol=="H"))?1:bond.order;
		    //		    qDebug()<<"xx";
		    cl.append(bond);
		  }
		}
	      }
	    }	
	  }
	  if (bondList.size()!=cl.size()) {	    
	    //	    qDebug()<<"bondList=cl"<<cl.size();
	    bondList=cl;
	  }
	  CEnvironment cel;
	  invom.Label=xdinp[index].atomname;
	  invom.pos=xdinp[index].kart;
	  invom.an=xdinp[index].OrdZahl;
	  invom.part=xdinp[index].part;
	  invom.Symbol=PSE_Symbol[xdinp[index].OrdZahl];
	  cel.append(invom);	  
	  for (int i=0;i<xdinp.size();i++){
	    if((xdinp[i].OrdZahl<0)||(xdinp[index].OrdZahl<0)) continue;
	    if ((xdinp[i].part>0)&&(xdinp[index].part>0)&&(xdinp[i].part!=xdinp[index].part)) continue;
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
		invom.Symbol="X";
		cel.append(invom);
	      }
	    }
	  }
	  Connection cll;
	  //printf("Tollllllll: bind %d inv%d ato%d  smx%d  \n",cl.size(),cel.size(),ce.size(),smx);


	  for (int i=0; i<cl.size();i++)
	    for (int k=0; k<cel.size();k++)
	      for (int j=0; j<cel.size();j++){
		if ((cl.at(i).ato1->pos==cel.at(j).pos)&&(cl.at(i).ato2->pos==cel.at(k).pos)){
		  if ((cel.at(0).part>0)&&((cel.at(0).part!=cl.at(i).ato1->part)||(cel.at(0).part!=cl.at(i).ato2->part)));
		  else 
		    cll.append(cl[i]);
		}
	      }
	  if (invEditAble) {
	    invDlg *id=new invDlg(&cel,&cll,dataBase);//TESTXDEDIT
	    id->update();
	    if (id->exec()==QDialog::Rejected)
	      for (int i=0;i<bondList.size();i++){
		bondList[i].order=cl.at(i).order;
	      }
	    //qDebug()<<"test";
	    //qDebug()<<bondList.size()<<bondList[0].order;
	  }
	  if (xdSetupMode){
	    CEnvironment all;	  
	    MyAtom xdall;
	    //	    printf
	 //   qDebug("Here I am atmax=%d  \n",xdinp.size());
	    for (int i=0; i<xdinp.size(); i++){
	      xdall.Label=xdinp[i].atomname;
	      xdall.pos=xdinp[i].kart;
	      xdall.an=xdinp[i].OrdZahl;
	      if (xdinp[i].OrdZahl>-1){
	      xdall.Symbol=PSE_Symbol[xdinp[i].OrdZahl];
	      }else xdall.Symbol="X";
	      all.append(xdall);

	    }

	   //   qDebug()<<"soso"<<xdall.an<<xdall.Symbol;
	    //	    printf("Hi, its me %d %d %d\n",cel.size(),cll.size(),all.size());
	    xdEditDlg *id=new xdEditDlg(&cel,&cll,&all);
	    //	    printf("Hi, its me again\n");
        //qDebug("Here I am atmax=%d  \n",xdinp.size());
	    id->update();
	    if (QDialog::Accepted==id->exec()) emit reloadXD() ;
	  }
	  setMatrix();
	  updateGL();
	  setMatrix();
	  updateGL();
	}	
      }
  }
}

void CubeGL::invariomExport(){  
  extern molekul mol;
  extern QList<INP> xdinp;
  extern QList<INP> asymmUnit;
  static char PSE_Symbol[109][3] = {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar",
    "K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr",
    "Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","J","Xe",
    "Cs","Ba", "La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu",
    "Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra",
    "Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No","Lr","Ku","Ha","Rf","Ns","Hs","Mt"};
  invariomsComplete.clear();
  invariomsUnique.clear();
  knoepfe.clear();
  QDialog *invExportDlg =new QDialog(this);
  invExportDlg->setMinimumSize(400,300);  
  QTextBrowser *browser=new QTextBrowser;
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), invExportDlg, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), invExportDlg, SLOT(reject()));
  QPushButton *exportMoProbutton = new QPushButton("Export Invarioms to MoPro files//not finished yet");
//  exportMoProbutton->setEnabled(false);
  connect(exportMoProbutton,SIGNAL(clicked()),this,SLOT(exportMoProFiles()));
  QString text;
  //  extern Connection bondList;
  for (int ix=0;ix<asymmUnit.size();ix++){
    MyAtom invom;
    double soll_abst,gg;
    MyBond bond;
    if (ce.size()==0){
      //      qDebug()<<"lhkmnsdfmfgn,";
      for (int i=0;i<xdinp.size();i++)
	if (xdinp[i].OrdZahl>-1) {
	  invom.Label=xdinp[i].atomname;
	  invom.pos=xdinp[i].kart;
	  invom.an=xdinp[i].OrdZahl;
	  invom.part=xdinp[i].part;
	  invom.Symbol=PSE_Symbol[xdinp[i].OrdZahl];
	  ce.append(invom);
	}
    }
    if ((bondList.size()!=0)) {
      cl=bondList;
      // qDebug()<<bondList.size();
    }
    else {
      for (int i=0;i<ce.size();i++) {
	for (int j=i+1;j<ce.size();j++){
	  //int i=0;
	  if ((ce.at(i).an<0)||(ce.at(j).an<0)) continue;
	  if (ce.at(i).pos==ce.at(j).pos) continue;
	  if ((ce.at(i).part>0)&&(ce.at(j).part>0)&&(ce.at(j).part!=ce.at(i).part)) continue;
	  if ((ce.at(i).an<83)&&(ce.at(j).an<83)&&(ce.at(i).an>=0)&&(ce.at(j).an>=0)){
	    soll_abst=((mol.Kovalenz_Radien[ce.at(i).an]+
				    mol.Kovalenz_Radien[ce.at(j).an])/100.0
			    -(0.08*fabs(((double)mol.ElNeg[ce.at(i).an]
						    -mol.ElNeg[ce.at(j).an])/100.0)));
	    gg=sqrt( Distance(ce.at(i).pos,ce.at(j).pos));
	    if (gg<soll_abst*1.2) {
	      bond.ato1=&ce.at(i);
	      bond.ato2=&ce.at(j);
	      bond.length=gg;
	      bond.chi=soll_abst-gg;
	      bond.order=(bond.chi<0.0847)?1:((bond.chi<0.184)?2:((bond.chi<0.27)?3:4));
	      bond.order=((bond.ato1->Symbol=="H")||(bond.ato2->Symbol=="H"))?1:bond.order;
	      //	      qDebug()<<bond.ato1->Label<<bond.ato1->part<<"---"<<bond.ato2->Label<<bond.ato2->part;
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
    invom.Symbol=PSE_Symbol[xdinp[ix].OrdZahl];
    if (invom.an>-1 ){
      text+="<b>";
      ina=inames::invName(invom,bondList,sel,ix);      
      //qDebug()<<sel.size()<<knoepfe.size();
      knoepfe.append(sel);
      invariomsComplete.append(ina);
      if (dataBase.contains(ina)) text+="<font color=\"green\">";
      else text+="<font color=\"red\">";
      text+=invom.Label;
      text+=": </font></b>";
      text+=ina;
      text+="<br>";
    }
  }
  browser->setHtml(text);
  invariomsUnique=invariomsComplete;
  invariomsUnique.removeDuplicates();
  QVBoxLayout sss;
  sss.addWidget(browser);
  sss.addWidget(buttonBox);
  sss.addWidget(exportMoProbutton);
  invExportDlg->setLayout(&sss);
  invExportDlg->setWindowTitle("Export these Invaromnames to 'Invariome.in'");
  if (QDialog::Accepted==invExportDlg->exec()){
    QFile in("Invariome.in");
    in.open(QIODevice::WriteOnly);
    in.write(browser->toPlainText().toLatin1(),browser->toPlainText().length());
    in.close();
  }
}

QString CubeGL::inv2moproaxes(int index){
    QStringList axtok;
    QString as1,as2;
    extern molekul mol;
    int j=dataBase.indexOf(invariomsComplete.at(index));
    if (j==-1) {
        //default
      if (knoepfe.at(index).size()>1){
        as1=knoepfe.at(index).at(0).Label;
        as2=knoepfe.at(index).at(1).Label;
        as1.remove(QRegExp("[)(]+"));
        as2.remove(QRegExp("[)(]+"));
        as1.remove(QRegExp("@.+$"));
        as2.remove(QRegExp("@.+$"));
        return QString(" XY  %1 %2").arg(as1).arg(as2);//
      }
    }
    QString erg = "problem ",axcopy=entries.at(j).CoordinateSystem;
    //KS: X:C(4) Y:C(7) AX2:C0.033974 AX1:C0.033938
    axcopy.remove("KS: ");
    axcopy.remove(QRegExp("\\(\\w+\\)"));
    axcopy.replace(" ",":");

    axtok=axcopy.split(":",QString::SkipEmptyParts);
    //qDebug()<<axtok.size()<<axtok<<index<<invariomsComplete.at(index);
    if (axtok.size()>3){
    int at1=mol.Get_OZ(axtok.at(1));
    int at2=mol.Get_OZ(axtok.at(3));
    int ind1=0,ind2=0,r=0;

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
      as1.remove(QRegExp("@.+$"));
      as2.remove(QRegExp("@.+$"));
      erg = QString("%1%2%3  %4 %5")
            .arg("b")
            .arg(axtok.at(0))
            .arg(axtok.at(2))
            .arg(as1)
            .arg(as2);
      return erg;
    }
    for (int k=1; k< knoepfe.at(index).size();k++){//direkte Nachbarn finden
        if (knoepfe.at(index).at(k).an==at1) {ind1=k;continue;}
        if (knoepfe.at(index).at(k).an==at2) {ind2=k;continue;}

    }
    as1=knoepfe.at(index).at(ind1).Label;
    as2=knoepfe.at(index).at(ind2).Label;
    // qDebug()<<ind1<<ind2;
    if (ind1>=ind2){
      while ((r<knoepfe.size())&&(as1!=knoepfe.at(r).at(0).Label)) r++;

      as2=QString("MIST");
      //qDebug()<<as1<<r<<knoepfe.size()<<knoepfe.at(r).at(0).Label;
      for (int k=1; k< knoepfe.at(r).size();k++){
        if ((knoepfe.at(r).at(k).an==at2)&&(knoepfe.at(r).at(k).Label!=as1)) {
          as2=knoepfe.at(r).at(k).Label;
          //qDebug()<<"oo"<<as1<<as2<<r<<k<<knoepfe.at(r).at(0).Label;
          break;
        }
      }
      if (as2=="MIST"){
        //qDebug()<<as2;
        for (int k=1; k< knoepfe.at(r).size();k++){
          if ((knoepfe.at(r).at(k).Label!=as1)) {
            as2=knoepfe.at(r).at(k).Label;
            //qDebug()<<"aa"<<as1<<as2;
            break;
          }
        }
      }
    }
    //qDebug()<<as1<<as2;
    if ((as1==as2)||(knoepfe.at(index).at(0).Label==as1)||(knoepfe.at(index).at(0).Label==as2)){
        QStringList nb,nnb;
        for (int r=0;r<knoepfe.at(index).size();r++) nb.append(knoepfe.at(index).at(r).Label);
        for (int r=0;r<knoepfe.at(ind1).size();r++) nnb.append(knoepfe.at(ind1).at(r).Label);
        //qDebug()<<knoepfe.at(index).at(0).Label<<as1<<as2<<nb<<"!!!!\n"<<nnb;
        for (int k=1; k< knoepfe.at(ind1).size();k++){
        if ((knoepfe.at(ind1).at(k).Label!=knoepfe.at(index).at(0).Label)&&(knoepfe.at(ind1).at(k).Label!=as1)) {
            as2=knoepfe.at(index).at(k).Label;
            //qDebug()<<as1<<as2;
            break;
        }
        }
    }
    //qDebug()<<ind1<<ind2;

    as1.remove(QRegExp("[)(]+"));
    as2.remove(QRegExp("[)(]+"));
    as1.remove(QRegExp("@.+$"));
    as2.remove(QRegExp("@.+$"));
    erg = QString("%1%2%3  %4 %5")
          .arg(" ")
          .arg(axtok.at(0))
          .arg(axtok.at(2))
          .arg(as1)
          .arg(as2);
    //qDebug()<<erg;
    //exit(11);
      }
    return erg;

}

void CubeGL::exportMoProFiles(){
    QDir md;
    extern molekul mol;
    extern QList<INP> asymmUnit;
    extern int dummax;
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
      "Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","J","Xe",
      "Cs","Ba", "La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu",
      "Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra",
      "Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No","Lr","Ku","Ha","Rf","Ns","Hs","Mt"};
    md.mkdir("InvariomTransfer");
    QList<double> corr3;
    double issu=0.0,corr_3;
    int sosu=0,anzahl=0;

    for (int mi =0; mi < maxmol; mi++){
      for (int i=0;i<asymmUnit.size();i++){
        if (asymmUnit.at(i).molindex!=mi) continue;
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
    //qDebug()<<corr3.size()<<maxmol;
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
        /*
        moprofile.write(QString("DUMMY %1\n").arg(dummax).toLatin1());
        if (dummax) for (int i=0; i<asymmUnit.size();i++){
            if (asymmUnit.at(i).OrdZahl==-1)
                moprofile.write(QString("  DUM %1 %2 %3 %4 0\n")
                                .arg(asymmUnit.at(i).frac.x,11,'f',6)
                                .arg(asymmUnit.at(i).frac.y,11,'f',6)
                                .arg(asymmUnit.at(i).frac.z,11,'f',6)
                                .arg(strtok(asymmUnit[i].atomname,"!")).toLatin1());
        }
        */
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
        moprofile.write(QString("\nATOMS  %1\n").arg(asymmUnit.size()-dummax).toLatin1());
        for (int i=0; i<asymmUnit.size();i++){
            if (asymmUnit.at(i).OrdZahl!=-1){
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
                moprofile.write(QString("ATOM %1  %2 %3 %4 %5%6%7  %8 %9 %10\n")
                                .arg(i+1,5)//1
                                .arg(anam,-4)//2
                                .arg(resityp,4)//3
                                .arg(resinr,3)//4
				.arg(asymmUnit.at(i).frac.x,10,'f',6)//5
				.arg(asymmUnit.at(i).frac.y,10,'f',6)//6
				.arg(asymmUnit.at(i).frac.z,10,'f',6)//7
				.arg("1.0000")//8 pop //achtung hier sollten wir populationen haben 
				                      //werden aber noch nicht eingelesen
				.arg(1)//9 occ
				.arg(PSE_Symbol[asymmUnit.at(i).OrdZahl])//10 elem
                                .toLatin1());

                int j = invariomsUnique.indexOf(invariomsComplete.at(i));
                moprofile.write(QString("%1HEX  K%2     V0   M0   Q0\n")
                                .arg(inv2moproaxes(i),-32)//1ax
                                .arg(j)//6kappanr
                                .toLatin1());////hier bin ich gerade */
                if ((asymmUnit.at(i).uf.m12==asymmUnit.at(i).uf.m13)&&
                    (asymmUnit.at(i).uf.m13==asymmUnit.at(i).uf.m23)&&
                    (asymmUnit.at(i).uf.m12==0))
                  moprofile.write(QString("UISO %1\n").arg(asymmUnit.at(i).uf.m11).toLatin1());
                else
                  moprofile.write(QString("UANI %1 %2 %3 %4 %5 %6\n")
                                  .arg(asymmUnit.at(i).uf.m11)
                                  .arg(asymmUnit.at(i).uf.m22)
                                  .arg(asymmUnit.at(i).uf.m33)
                                  .arg(asymmUnit.at(i).uf.m12)
                                  .arg(asymmUnit.at(i).uf.m13)
                                  .arg(asymmUnit.at(i).uf.m23)
                                  .toLatin1());
                int z=dataBase.indexOf(invariomsComplete.at(i));
                if (z>-1){
                  double mv=0;
                  if (asymmUnit.at(i).molindex<corr3.size()) mv= entries.at(z).m0+corr3.at(asymmUnit.at(i).molindex);
                  moprofile.write(QString("%1 %2 %3 %4 %5 %6 %6 %7 %8 %9 %10\n")
                                  .arg(mv,8,'f',5)
                                .arg(entries.at(z).m1,8,'f',5)
                                .arg(entries.at(z).d0,8,'f',5)
                                .arg(entries.at(z).d1p,8,'f',5)
                                .arg(entries.at(z).d1m,8,'f',5)
                                .arg(entries.at(z).q0,8,'f',5)
                                .arg(entries.at(z).q1p,8,'f',5)
                                .arg(entries.at(z).q1m,8,'f',5)
                                .arg(entries.at(z).q2p,8,'f',5)
                                .arg(entries.at(z).q2m,8,'f',5)
                                .toLatin1());
                moprofile.write(QString("%1 %2 %3 %4 %5 %6 %6 %7\n")
                                .arg(entries.at(z).o0,8,'f',5)
                                .arg(entries.at(z).o1p,8,'f',5)
                                .arg(entries.at(z).o1m,8,'f',5)
                                .arg(entries.at(z).o2p,8,'f',5)
                                .arg(entries.at(z).o2m,8,'f',5)
                                .arg(entries.at(z).o3p,8,'f',5)
                                .arg(entries.at(z).o3m,8,'f',5)
                                .toLatin1());
                moprofile.write(QString("%1 %2 %3 %4 %5 %6 %6 %7 %8 %9\n")
                                .arg(entries.at(z).h0,8,'f',5)
                                .arg(entries.at(z).h1p,8,'f',5)
                                .arg(entries.at(z).h1m,8,'f',5)
                                .arg(entries.at(z).h2p,8,'f',5)
                                .arg(entries.at(z).h2m,8,'f',5)
                                .arg(entries.at(z).h3p,8,'f',5)
                                .arg(entries.at(z).h3m,8,'f',5)
                                .arg(entries.at(z).h4p,8,'f',5)
                                .arg(entries.at(z).h4m,8,'f',5)
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
        qDebug()<<"MoPro file succesfully written!";
    }
}


void CubeGL::wheelEvent(QWheelEvent *event){   
  int numDegrees = event->delta() / 8;
  int numSteps = numDegrees / 15;     
  if (iSel){
    if (ibas) glDeleteLists(ibas,1);
    ibas=glGenLists(1);    
    //for (int i=0;i<maxResi;i++)
    glNewList(ibas, GL_COMPILE );
    extern QList<INP> xdinp;
    extern double L;
    extern molekul mol;
    if (numSteps>0)mol.highlightResi(xdinp,mol.firstHL-1,L,elli);
    else mol.highlightResi(xdinp,mol.lastHL+1,L,elli);
    glEndList();	    
    //printf("gut ?%d\n",ibas);
    updateGL();
  }else{
    int d = myFont.pointSize();
    d=(d+numSteps>4)?d+numSteps:d;
    myFont.setPointSize(d);
    updateGL();
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
    GLint hits;
    glSelectBuffer(MAXSELECT, selectBuf);
    (void)glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(~0);
    glPushMatrix();
    if (stereo_mode<2)  glViewport(0, 0, _win_width, _win_height);        
    else glViewport(0, 0, _win_width/2, _win_height);
    glGetIntegerv(GL_VIEWPORT, vp);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (stereo_mode<2) {
      gluPickMatrix(event->x(), _win_height-event->y(), 8, 8, vp);
      gluPerspective( 29.0, (double)_win_width/_win_height, 50.0, 8000.0 );
    }
    else {
      gluPickMatrix(event->x() % (_win_width/2), _win_height-event->y(), 8, 8, vp);
      gluPerspective( 29.0, (double)(_win_width / 2.0)/_win_height, 50.0, 8000.0 );
    }
    glMatrixMode(GL_MODELVIEW);
    draw();
    glPopMatrix();
    hits = glRenderMode(GL_RENDER); 
    if (hits<=0) {expandatom=-1;}else
      if ((int)selectBuf[(hits-1)*4+3]<xdinp.size()) {
	expandatom =selectBuf[(hits-1)*4+3];
	if (expandatom<0) {expandatom=-1;return;}
	//      printf("expandatom %d\n",expandatom);
	//     int bo=cl.at(index).order;
	expandAct.setText(tr("Expand %1 Ang. arround %2.").arg(mol.gd).arg(xdinp.at(expandatom).atomname));
	hideThisAct.setText(tr("Hide %1 ").arg(xdinp.at(expandatom).atomname));
	menu.addAction(&expandAct);
	menu.addAction(&hideThisAct);
	menu.addAction(&hideThisFragment);
	menu.addAction(&hideOtherFragments);

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
  glScaled(1.0+speed*0.1,1.0+speed*0.1,1.0+speed*0.1);  
  updateGL();  
}

void CubeGL::rotY(double speed){
  glRotateL(-20.0*speed,0.0f,1.0f,0.0f);
  updateGL();  
}

void CubeGL::moveY(double speed){
  glTranslateL(0.0,speed,0.0);
  updateGL();  
}

void CubeGL::moveX(double speed){
  glTranslateL(speed,0.0,0.0);
  //  QMessageBox::information(this,"Px move","xmoved",QMessageBox::Ok);
  updateGL();  
}

void CubeGL::rotX(double speed){
  glRotateL(-20.0*speed,1.0f,0.0f,0.0f);
  updateGL();  
}

void CubeGL::fontSizeUp() {
  myFont.setPointSize(myFont.pointSize()+1);
  //  QMessageBox::information(this,"PFUND","Font Geändert",QMessageBox::Ok);
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
  GLfloat dx = GLfloat(
		  event->x() - lastPos.x()) / width();
  GLfloat dy = GLfloat(
		  event->y() - lastPos.y()) / height();
  if ((!moai)&&(!event->buttons())) {
    //        printf("Test\n");
    GLint hits;
    //QString text[4]={"label","yellow","blue","green"};
    //GLfloat MM[16];
    //glGetDoublev(GL_MODELVIEW_MATRIX,MM); 
    glSelectBuffer(MAXSELECT, selectBuf);
    (void)glRenderMode(GL_SELECT);
    //SelectMode=1;
    glInitNames();
    glPushName(~0);
    //WIN_COL[0]=1.0;
    glPushMatrix();
    if (stereo_mode<2)  glViewport(0, 0, _win_width, _win_height);        
    else glViewport(0, 0, _win_width/2, _win_height);
    glGetIntegerv(GL_VIEWPORT, vp);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (stereo_mode<2) {
      gluPickMatrix(event->x(), _win_height-event->y(), 8, 8, vp);
      gluPerspective( 29.0, (double)_win_width/_win_height, 50.0, 8000.0 );
    }
    else {
      gluPickMatrix(event->x() % (_win_width/2), _win_height-event->y(), 8, 8, vp);
      gluPerspective( 29.0, (double)(_win_width / 2.0)/_win_height, 50.0, 8000.0 );
    }
    glMatrixMode(GL_MODELVIEW);
    //     glLoadMatrixd(MM);
    draw();
    glPopMatrix();
    hits = glRenderMode(GL_RENDER);
    extern QList<INP> xdinp;
    updateGL();
    if ((hits <= 0)||(xdinp.size()<(int)selectBuf[(hits-1)*4+3])) {imFokus=-1; emit message("");}
    else {
      imFokus=selectBuf[(hits-1)*4+3];
      //  printf("Test,%d %d\n",imFokus,hits);
      if (imFokus>=0) emit message(xdinp[selectBuf[(hits-1)*4+3]].atomname);
    }
  }
  if (event->buttons() & Qt::MidButton){
    glTranslateL(dx*100.0,-dy*100.0,0);
    updateGL();
  }
  if ((event->buttons() & Qt::LeftButton)) {
    if (moveLab){
      extern QList<INP> xdinp;
      GLdouble ML[16];
      glGetDoublev(GL_MODELVIEW_MATRIX,ML);
      double V2[3],V1[3];
      V1[0]=dx*12.0*mlsc*(double)_win_width/_win_height;
      V1[1]=-dy*12.0*mlsc;
      V1[2]=0;
      V2[0]=ML[0] *V1[0]+ML[1] *V1[1]+ML[2] *V1[2];
      V2[1]=ML[4] *V1[0]+ML[5] *V1[1]+ML[6] *V1[2];
      V2[2]=ML[8] *V1[0]+ML[9] *V1[1]+ML[10]*V1[2];
      //       printf("%f %f %f\n",mlsc,ML[14],(double)_win_width/_win_height);
      //       V2[0]/=V2[3]/width();
      //V2[1]/=V2[3]/height();

      //       xdinp[labToMove].labPos+=v2;
      //qDebug()<<labToMove<<V2[0]<<V2[1]<<V2[2];
      xdinp[labToMove].labPos+=V3(V2[0],V2[1],V2[2]);
    }else if (moveLeg){
      //         qDebug()<<"movLeg"<<moveLeg;
      if (horizont) {
	mil.y+=dx*3.15*(double)_win_width/_win_height;
	mil.x-=dy*3.15;
	//qDebug()<<mlsc<<mlsc*3.15;
	//	 printf("H %f %f \n",mil.x,mil.y);
	/*	 if ((mil.x>1.6)||(mil.x<-1.6)||(mil.y>1.2)||(mil.y<-2.99)) {
		 horizont=false;
	//	   double dm=mil.x;
	mil.x=0;
	mil.y=-1.0;
	}*/
      }else{
	mil.x+=dx*3.15*(double)_win_width/_win_height;
	mil.y-=dy*3.15;
	//	 	 printf("V %f %f \n",mil.x,mil.y);
	/*	 if ((mil.x>2.90)||(mil.x<-2.99)||(mil.y>-0.25)||(mil.y<-1.6)) {
		 horizont=true;
	//	   double dm=mil.x;
	mil.x=0;
	mil.y=-1.0;
	}*/
      }
    }else{
      glRotateL(dy*360.0,1.0f,0.0f,0.0f);
      glRotateL(dx*360.0,0.0f,1.0f,0.0f);
    }
    updateGL();
  }
  else if((event->buttons() & Qt::RightButton)){
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
    mil.x=-2.4;
    mil.y=-0.3;
  }else{
    mil.x=-1.4;
    mil.y=0.1;
  }
  horizont=!horizont;
  updateGL();
}

void CubeGL::setMatrix(){
  glMatrixMode(GL_MODELVIEW);
  //  GLdouble mm[16];
  //  glGetDoublev(GL_MODELVIEW_MATRIX,mm);
  /*  printf("Die Mmatrix ist:\n%9.6f %9.6f %9.6f %9.6f\n%9.6f %9.6f %9.6f %9.6f\n%9.6f %9.6f %9.6f %9.6f\n%9.6f %9.6f %9.6f %9.6f\n",
      mm[0],mm[1],mm[2],mm[3],
      mm[4],mm[5],mm[6],mm[7],
      mm[8],mm[9],mm[10],mm[11],
      mm[12],mm[13],mm[14],mm[15]);*/
  //  GLdouble sum9 = MM[0]+ MM[1]+ MM[2]+  MM[4]+ MM[5]+ MM[6]+   MM[8]+ MM[9]+ MM[10];
  GLdouble det=
	  MM[0]*MM[5]*MM[10] - MM[8]*MM[5]*MM[2]+
	  MM[1]*MM[6]*MM[8]  - MM[9]*MM[6]*MM[0]+
	  MM[2]*MM[4]*MM[9]  - MM[10]*MM[4]*MM[1];
  //  printf("Stetze die Matrix det %f\n",det);
  if ((det>0.1)&&(det<9.0)) glLoadMatrixd(MM);
  else {
    //  printf("else\n"); 
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
  /*  glGetDoublev(GL_MODELVIEW_MATRIX,mm);
      printf("Die neue Mmatrix ist:\n%9.6f %9.6f %9.6f %9.6f\n%9.6f %9.6f %9.6f %9.6f\n%9.6f %9.6f %9.6f %9.6f\n%9.6f %9.6f %9.6f %9.6f\n",
      mm[0],mm[1],mm[2],mm[3],
      mm[4],mm[5],mm[6],mm[7],
      mm[8],mm[9],mm[10],mm[11],
      mm[12],mm[13],mm[14],mm[15]);
      det=
      mm[0]*mm[5]*mm[10] - mm[8]*mm[5]*mm[2]+
      mm[1]*mm[6]*mm[8]  - mm[9]*mm[6]*mm[0]+
      mm[2]*mm[4]*mm[9]  - mm[10]*mm[4]*mm[1];
      glScaled(1.0/det,1.0/det,1.0/det);*/
  //  {int Fehler=glGetError(); if (Fehler!=GL_NO_ERROR) { fprintf (stderr, "3OpenGL Error: %s\n",gluErrorString(Fehler)); } }
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
  stereo_mode++;
  stereo_mode%=4;
  updateGL();
#ifdef Q_WS_MAC
  qDebug()<<QCoreApplication::libraryPaths () <<QImageWriter::supportedImageFormats ();
#endif   
#ifdef _WIN32
  //qDebug()<<QCoreApplication::libraryPaths () <<QImageWriter::supportedImageFormats ();
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
  printf("die Determinante der Drehmatrix ist: %f\n",det);
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
  printf("die Determinante der Drehmatrix ist: %f\n",det);
#endif
}

void CubeGL::along001(){
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
  updateGL();
}

void CubeGL::along010(){
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
  updateGL();

}

void CubeGL::along100(){
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
  updateGL();
}

void CubeGL::draw() {
  if (depthCueing) glEnable(GL_FOG);
  else glDisable(GL_FOG);

  static int Pers;
  extern QList<INP> xdinp;
  extern double L;
  extern molekul mol;
  double gmat[16];
  static double max=0;
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
  V3 ori=V3(0,0,0);///rotation origin
  if ((iSel)&&(ibas)){    
    V3 sumse=V3(0,0,0);
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
    V3 sumse=V3(0,0,0);
    for (int i=0;i<xdinp.size();i++)
      sumse+=xdinp[i].kart;
    sumse*=1.0/xdinp.size();
    double gmat[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, (double*)gmat );
    ori.x=gmat[0] * sumse.x + gmat[4] * sumse.y + gmat[8] * sumse.z;
    ori.y=gmat[1] * sumse.x + gmat[5] * sumse.y + gmat[9] * sumse.z;
    ori.z=gmat[2] * sumse.x + gmat[6] * sumse.y + gmat[10] * sumse.z;    
  }
  if (centerSelection->isChecked()){
    if (selectedAtoms.isEmpty()) centerSelection->setChecked(false);
    else {
      V3 sumse=V3(0,0,0);
      for (int i=0;i<selectedAtoms.size();i++)
	sumse+=selectedAtoms[i].kart;
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
    double gmat[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, (double*)gmat );
    ori.x=gmat[0] * xdinp.at(rotze).kart.x + gmat[4] * xdinp.at(rotze).kart.y + gmat[8] *  xdinp.at(rotze).kart.z;
    ori.y=gmat[1] * xdinp.at(rotze).kart.x + gmat[5] * xdinp.at(rotze).kart.y + gmat[9] *  xdinp.at(rotze).kart.z;
    ori.z=gmat[2] * xdinp.at(rotze).kart.x + gmat[6] * xdinp.at(rotze).kart.y + gmat[10] * xdinp.at(rotze).kart.z;    
  }
  QFont nonAtomFont=QFont(myFont);
  nonAtomFont.setPointSize(myFont.pointSize()/2);
  glPushMatrix();
  if (back_grad&&(rmode==GL_RENDER)){
    double mat[16];
    //  if (texti==0) glBindTexture(GL_TEXTURE_2D, tex[0]);
    //  if (texti==1) glBindTexture(GL_TEXTURE_2D, tex[1]);
    //  if (texti!=2) glEnable(GL_TEXTURE_2D);
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
  }  
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
  glPushMatrix();
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
  if (bas) {
    glDisable(GL_CULL_FACE);
    if (Luftschlange) glCallList(bas+6);      
    glEnable(GL_CULL_FACE);
    if (drawHb) glCallList(bas+5);
    if (cbas) glCallList(cbas);
    if (drawBo) {
      if (mol.singleColorBonds) {
	qglColor(mol.bondColor);
	glCallList(bas+8);
      }
      else glCallList(bas+1);
    }
    if (drawAt) {
      if (elli) {
	glCallList(bas);
	glEnable(GL_BLEND);
	glCallList(bas+7);
	if ((ibas)&&(iSel)) ;
	else glDisable(GL_BLEND);
      }else {
	if (mol.tubifiedAtoms)glCallList(bas+9);
	else glCallList(bas+4);
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
    glEnable(GL_BLEND);
    if (rmode==GL_RENDER){
      if (drawAx) glCallList(bas+2);
      if (drawUz) glCallList(bas+3);    
      if ((MIS)&&(moliso->mibas)) { 
	glDisable(GL_CULL_FACE);
	if (molisoTransparence) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);
	if (zebra){
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
		case 1: {glCallList(moliso->mibas);break;}
		case 2: {glCallList(moliso->mibas+1);break;}
		case 3: {glCallList(moliso->mibas+2);break;}
		case 4: {glCallList(moliso->mibas+3);break;}
		case 5: {glCallList(moliso->mibas+4);break;}
		case 6: {glCallList(moliso->mibas+5);break;}
		default: qDebug()<<"Impossible Orientation!!!";exit(1);
	}      
	//else glCallList(mibas);
	// */
	glDisable(GL_TEXTURE_1D);
	glDisable(   GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);


	if (MILe){
	  //    printf("4ok\n");
	  GLfloat fw;
	  glPushMatrix();
	  double mat[16];
	  glEnable(GL_COLOR_MATERIAL);
	  glGetDoublev( GL_MODELVIEW_MATRIX, (double*)mat );
	  //      printf("4+ok\n");
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

	  for (int i=0;i<6;i++) {
	    fw=(0.2*i*(moliso->max-moliso->min)) + moliso->min;
	    lab = QString::number(fw,'f',4) ;
	    R= FM.boundingRect(lab);
	    if (!monochrom)  moliso->Farbverlauf(fw); 
	    else glColor4f(tCR,tCG,tCB,tCA);
	    glDisable(GL_BLEND);
	    if (horizont) renderText(mil.y+0.33*i*milsize+0.01,mil.x+0.07*milsize,-6.1,lab ,MLegendFont);
	    else{
	      if (mil.x<0) renderText(mil.x+0.07*milsize,mil.y+0.33*i*milsize+0.01,-6.1,lab,MLegendFont);
	      else {//renderText(mil.x-0.05-(R.width()/300.0),mil.y+0.33*i+0.01,-6.1,lab ,MLegendFont);
		renderText(mil.x-(3.5/_win_height *R.width()),mil.y+0.33*i*milsize+0.01,-6.1,lab ,MLegendFont);
	      }
	    }	  
	  }

	  //	printf("%f  %d %d %d %d \n",fw,R.x(),R.y(),R.width(),R.height());
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
	dieDiPole();
	glPopMatrix();
      }
      if ((drawLa)) //glCallList(bas+7);
      {	
	glClear( GL_DEPTH_BUFFER_BIT);
	glPushMatrix();{
	  glScaled(L,L,L);
	  for (int j=0;j<xdinp.size();j++){
	    //	    glLoadName(j);
	    if (imFokus==j) qglColor(Qt::yellow); else  glColor4f(tCR,tCG,tCB,tCA);
	    if (((iSel)&&(mol.firstHL<=j)&&(mol.lastHL>=j))||(!iSel)){
	      if (xdinp[j].OrdZahl<0) renderText( xdinp[j].labPos.x,xdinp[j].labPos.y,xdinp[j].labPos.z, xdinp[j].atomname,nonAtomFont);
	      else 
		if ((imFokus==j)||(!(mol.aStyle[xdinp[j].OrdZahl]&ATOM_STYLE_NOLABEL)))
		  renderText( xdinp[j].labPos.x,xdinp[j].labPos.y,xdinp[j].labPos.z, xdinp[j].atomname,myFont);
	    }
	  }    
	}glPopMatrix();
      }
    } 
  }
  //  printf("%d %d %d\n",bas,ibas,mibas);
  /*
     if ((ibas)&&(iSel)){
     glEnable(GL_COLOR_MATERIAL);
     glCallList(ibas);
     glColor4f(0.6,0.6,0.6,0.3);
     glDisable(GL_COLOR_MATERIAL);
     } else {glEnable(GL_COLOR_MATERIAL);}  
     */
  glPopMatrix();
}

void CubeGL::dieDiPole(){
  int size=pole.size(),fsz=farben.size();
  V3 vec,p,VZ;
  VZ.x=0;VZ.y=0;VZ.z=1;
  QFont nonAtomFont=QFont(myFont);
  nonAtomFont.setPointSize(myFont.pointSize()/2);
  double wink,gg;
  glDisable(   GL_CULL_FACE);
  for (int i=0;  i<size;i++){    	    
    p=pole.at(i);
    vec=molekul::kreuzX(pole.at(i).x,pole.at(i).y,pole.at(i).z,0.0f,0.0f,1.0f);
    wink=molekul::winkel(pole.at(i),VZ);
    gg=sqrt(Norm(p));{
      if (Norm(vec)==0) vec=V3(1.0,0,0);
      glPushMatrix();
      glLoadName((GLuint)-1);
      if (fsz>i)qglColor(farben.at(i)); 
      else glColor3f(0.4,0.3,0.1);
      glRotatef(wink,vec.x,vec.y,vec.z); // drehen
      GLUquadricObj *q = gluNewQuadric();
      gluQuadricNormals(q, GL_SMOOTH); 
      //   printf ("%f\n",gg);
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
  }
  glEnable(   GL_CULL_FACE);
}


void CubeGL::updateBondActions(){
  clearSelection->setVisible(!selectedAtoms.isEmpty());
  centerSelection->setVisible(!selectedAtoms.isEmpty());
  extern molekul mol;
  delCoordi->setVisible(!mol.cBonds.isEmpty());
  //hideNotSelection->setVisible(!selectedAtoms.isEmpty());
  //unhide->setVisible(hiddenThings);
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
  extern double L;
  int idx= mol.bcnt;
  mol.bcnt+=2;
  mol.bd=(bindi*)realloc(mol.bd,sizeof(bindi)*mol.bcnt);
  mol.bd[idx].a=selectedAtoms.at(0).GLname;
  mol.bd[idx].e=selectedAtoms.at(1).GLname;
  idx++;
  mol.bd[idx].e=selectedAtoms.at(0).GLname;
  mol.bd[idx].a=selectedAtoms.at(1).GLname;
  bool vorher=mol.singleColorBonds;
  glNewList(bas+1, GL_COMPILE );{      //BONDS
    glPushMatrix();{
      glScaled( L, L, L );
      mol.adp=0;      
      mol.singleColorBonds=false;
      mol.bonds(xdinp);
    }glPopMatrix();    
  }glEndList();
  glNewList(bas+8, GL_COMPILE );{       //bonds in single color
    glPushMatrix();{
      glScaled( L, L, L );
      mol.intern=1;
      //      cubeGL->qglColor(mol.bondColor);
      mol.singleColorBonds=true;
      mol.bonds(xdinp);
    }glPopMatrix();
  }glEndList();
  mol.singleColorBonds=vorher;
  selectedAtoms.clear();
  updateBondActions();
  updateGL();
}

void CubeGL::disConnectSelection(){
  if (selectedAtoms.size()!=2) return;
  extern molekul mol;
  extern QList<INP> xdinp;
  extern double L;
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
  bool vorher=mol.singleColorBonds;
  glNewList(bas+1, GL_COMPILE );{      //BONDS
    glPushMatrix();{
      glScaled( L, L, L );
      mol.adp=0;      
      mol.singleColorBonds=false;
      mol.bonds(xdinp);
    }glPopMatrix();    
  }glEndList();
  glNewList(bas+8, GL_COMPILE );{       //bonds in single color
    glPushMatrix();{
      glScaled( L, L, L );
      mol.intern=1;
      //      cubeGL->qglColor(mol.bondColor);
      mol.singleColorBonds=true;
      mol.bonds(xdinp);
    }glPopMatrix();
  }glEndList();
  mol.singleColorBonds=vorher;
  selectedAtoms.clear();
  updateBondActions();
  updateGL();

}

void CubeGL::coordinativeBonds(){
  if (selectedAtoms.size()!=2) return;
  bindi bond;
  extern molekul mol;
  extern QList<INP> xdinp;
  extern double L;
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
