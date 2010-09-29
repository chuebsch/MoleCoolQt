#include "molekul.h"
#include "eacDlg.h"

extern void __RotateCS( double c, double s, double& X, double& Y );
extern void glTranslateL( const double dx, const double dy, const double dz );
extern void glRotateL( const double dang, const double x, const double y, const double z );


EacGLW::EacGLW(){
  extern molekul mol;
  // m.adpwall=mol.adpwall;
  //printf("%d fff %d\n",m.adpwall,mol.adpwall);
  m.initDir();//"");
  mol.copyAcol(m.Acol,m.arad,m.aStyle);
  INP xda;
  strcpy(xda.atomname,"H");
  xda.kart.x=0;
  xda.kart.y=0;
  xda.kart.z=0;
  xda.u.m11=0.2;
  xda.u.m22=0.1;
  xda.u.m33=0.05;
  xda.u.m13=0.001;
  xda.u.m12=0.0;
  xda.u.m23=0.001;
  xda.OrdZahl=0;
  xd.append(xda);

}
void EacGLW::draw(){
  static double rtx=2.0;  
  glRotateL(rtx,1.0,0.0,0.0);
  glRotateL(rtx,0.0,1.0,0.0);
  rtx+=2.0;
    glPushMatrix();
    glScaled( 50, 50, 50 );
    m.adp=1;
    m.intern=1;
    m.atoms(xd,70);
    m.intern=0;
    glEnable(GL_BLEND);
    m.atoms(xd,70);
    glDisable(GL_BLEND);
    glColor3f(0,0,0);
    glClear( GL_DEPTH_BUFFER_BIT);
    if (!(m.aStyle[m.Get_OZ(xd.at(0).atomname)]&ATOM_STYLE_NOLABEL)) renderText( xd.at(0).kart.x,xd.at(0).kart.y,xd.at(0).kart.z, xd.at(0).atomname,myFont);
    glPopMatrix();
}
void EacGLW::initializeGL(){
  glEnable(GL_LINE_SMOOTH);  
  glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
  
  QTimer *timer = new QTimer(this);
  connect( timer, SIGNAL(timeout()),
	   this, SLOT(updateGL()) );
    // Jede Sekunde updateTime() aufrufen
  timer->start(20);
  
  //glEnable(GL_POLYGON_SMOOTH);   
  myFont= QFont("Arial", 24, -1, false);  
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

  m.adpwall=bindTexture(QImage(QString(":/images/adpwall.png")),GL_TEXTURE_2D);
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
void EacGLW::changeAtom(int OZ){
  strcpy(xd[0].atomname,pse[OZ].toLatin1());
  xd[0].OrdZahl=OZ;
  updateGL();
}
void EacGLW::colorDLG(){
  QColor bgcolor;    
  bgcolor=QColorDialog::getColor(QColor((int)(m.Acol[m.Get_OZ(xd.at(0).atomname)][0]*255),(int)(m.Acol[m.Get_OZ(xd.at(0).atomname)][1]*255),(int)(m.Acol[m.Get_OZ(xd.at(0).atomname)][2]*255),(int)(m.Acol[m.Get_OZ(xd.at(0).atomname)][3]*255)), this);  
  if (bgcolor.isValid()) {
    qreal bgRed,bgGreen,bgBlue,bgAlpha;
    bgcolor.getRgbF(&bgRed,&bgGreen,&bgBlue,&bgAlpha);
    m.Acol[m.Get_OZ(xd.at(0).atomname)][0]=(GLclampf)bgRed;
    m.Acol[m.Get_OZ(xd.at(0).atomname)][1]=(GLclampf)bgGreen;
    m.Acol[m.Get_OZ(xd.at(0).atomname)][2]=(GLclampf)bgBlue;
    m.Acol[m.Get_OZ(xd.at(0).atomname)][3]=(GLclampf)0.5;
  }
}
void EacGLW::wallA(int st){
  if (st) m.aStyle[m.Get_OZ(xd.at(0).atomname)]|=ATOM_STYLE_WALLS;
  else if (m.aStyle[m.Get_OZ(xd.at(0).atomname)]&ATOM_STYLE_WALLS) m.aStyle[m.Get_OZ(xd.at(0).atomname)]-=ATOM_STYLE_WALLS;
  //printf("%d\n",m.aStyle[m.Get_OZ(xd.atomname)]);
  /*
  qDebug()<<"wallA"<<m.aStyle[m.Get_OZ(xd.atomname)]
	  <<(m.aStyle[m.Get_OZ(xd.atomname)]&ATOM_STYLE_WALLS)
	  <<(m.aStyle[m.Get_OZ(xd.atomname)]&ATOM_STYLE_RINGS)
	  <<(m.aStyle[m.Get_OZ(xd.atomname)]&ATOM_STYLE_SPHERE)
	  <<(m.aStyle[m.Get_OZ(xd.atomname)]&ATOM_STYLE_SOLID);*/
}
void EacGLW::ringA(int st){
  if (st==Qt::PartiallyChecked) m.aStyle[m.Get_OZ(xd.at(0).atomname)]|=ATOM_STYLE_WHITERING;
  else if (m.aStyle[m.Get_OZ(xd.at(0).atomname)]&ATOM_STYLE_WHITERING) m.aStyle[m.Get_OZ(xd.at(0).atomname)]-=ATOM_STYLE_WHITERING;
  if (st) m.aStyle[m.Get_OZ(xd.at(0).atomname)]|=ATOM_STYLE_RINGS;
  else if (m.aStyle[m.Get_OZ(xd.at(0).atomname)]&ATOM_STYLE_RINGS) m.aStyle[m.Get_OZ(xd.at(0).atomname)]-=ATOM_STYLE_RINGS;
  //printf("%d\n",m.aStyle[m.Get_OZ(xd.atomname)]);
  /*
qDebug()<<"ringA"<<m.aStyle[m.Get_OZ(xd.atomname)]
	  <<(m.aStyle[m.Get_OZ(xd.atomname)]&ATOM_STYLE_WALLS)
	  <<(m.aStyle[m.Get_OZ(xd.atomname)]&ATOM_STYLE_RINGS)
	  <<(m.aStyle[m.Get_OZ(xd.atomname)]&ATOM_STYLE_SPHERE)
	  <<(m.aStyle[m.Get_OZ(xd.atomname)]&ATOM_STYLE_SOLID);*/
}
void EacGLW::spheA(int st){
  if (st) m.aStyle[m.Get_OZ(xd.at(0).atomname)]|=ATOM_STYLE_SPHERE;
  else if (m.aStyle[m.Get_OZ(xd.at(0).atomname)]&ATOM_STYLE_SPHERE) m.aStyle[m.Get_OZ(xd.at(0).atomname)]-=ATOM_STYLE_SPHERE;
  //printf("%d %d\n",m.aStyle[m.Get_OZ(xd.atomname)],m.lod);
  /*
qDebug()<<"spheA"<<m.aStyle[m.Get_OZ(xd.atomname)]
	  <<(m.aStyle[m.Get_OZ(xd.atomname)]&ATOM_STYLE_WALLS)
	  <<(m.aStyle[m.Get_OZ(xd.atomname)]&ATOM_STYLE_RINGS)
	  <<(m.aStyle[m.Get_OZ(xd.atomname)]&ATOM_STYLE_SPHERE)
	  <<(m.aStyle[m.Get_OZ(xd.atomname)]&ATOM_STYLE_SOLID);*/
}
void EacGLW::soldA(int st){
  if (st) {
    m.aStyle[m.Get_OZ(xd.at(0).atomname)]|=ATOM_STYLE_SOLID;
    //    m.aStyle[m.Get_OZ(xd.atomname)]=(m.aStyle[m.Get_OZ(xd.atomname)]&ATOM_STYLE_SOLID)?(m.aStyle[m.Get_OZ(xd.atomname)]^ATOM_STYLE_WALLS):m.aStyle[m.Get_OZ(xd.atomname)];
  }
  else if (m.aStyle[m.Get_OZ(xd.at(0).atomname)]&ATOM_STYLE_SOLID) m.aStyle[m.Get_OZ(xd.at(0).atomname)]-=ATOM_STYLE_SOLID;
  /*
  qDebug()<<"soldA"<<m.aStyle[m.Get_OZ(xd.atomname)]
	  <<(m.aStyle[m.Get_OZ(xd.atomname)]&ATOM_STYLE_WALLS)
	  <<(m.aStyle[m.Get_OZ(xd.atomname)]&ATOM_STYLE_RINGS)
	  <<(m.aStyle[m.Get_OZ(xd.atomname)]&ATOM_STYLE_SPHERE)
	  <<(m.aStyle[m.Get_OZ(xd.atomname)]&ATOM_STYLE_SOLID);*/
}

void EacGLW::whitA(int st){
  if (st) {
    m.aStyle[m.Get_OZ(xd.at(0).atomname)]|=ATOM_STYLE_WHITERING;
  }
  else if (m.aStyle[m.Get_OZ(xd.at(0).atomname)]&ATOM_STYLE_WHITERING) m.aStyle[m.Get_OZ(xd.at(0).atomname)]-=ATOM_STYLE_WHITERING;
}

void EacGLW::nolaA(int st){
  if (st) {
    m.aStyle[m.Get_OZ(xd.at(0).atomname)]|=ATOM_STYLE_NOLABEL;
  }
  else if (m.aStyle[m.Get_OZ(xd.at(0).atomname)]&ATOM_STYLE_NOLABEL) m.aStyle[m.Get_OZ(xd.at(0).atomname)]-=ATOM_STYLE_NOLABEL;
}

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
  extern molekul mol;
  eacKRAD->setText(QString::number(mol.Kovalenz_Radien[OZ]/100.0));
  eacRAD->setText(QString::number(mol.Kovalenz_Radien[OZ]/250.0));
  //  walbx->setCheckState((mol.aStyle[OZ]&ATOM_STYLE_WALLS)?Qt::Checked:Qt::Unchecked);
  // qDebug()<<"updateLEs"<<mol.aStyle[OZ];                                                       
  rinbx->setCheckState((mol.aStyle[OZ]&ATOM_STYLE_RINGS)?((mol.aStyle[OZ]&ATOM_STYLE_WHITERING)?Qt::PartiallyChecked :Qt::Checked):Qt::Unchecked);

  sphbx->setCheckState((mol.aStyle[OZ]&ATOM_STYLE_SPHERE)?Qt::Checked:Qt::Unchecked);
  sldbx->setCheckState((mol.aStyle[OZ]&ATOM_STYLE_SOLID)?Qt::Checked:Qt::Unchecked);  
  walbx->setCheckState((mol.aStyle[OZ]&ATOM_STYLE_WALLS)?Qt::Checked:Qt::Unchecked);
  nlabx->setCheckState((mol.aStyle[OZ]&ATOM_STYLE_NOLABEL)?Qt::Checked:Qt::Unchecked);

  //  qDebug()<<"UpdateLEs"<<mol.aStyle[OZ];
  update();
}
EacDlg::EacDlg(){
  setWindowTitle("Edit Atom Parameters");
  eacGLW = new EacGLW();  
  eacGLO = new QGridLayout; 
  eacCB = new QComboBox;
  eacRAD = new QLineEdit;
  eacKRAD = new QLineEdit;
  extern molekul mol;
  QLabel *smbl =new QLabel("Element symbol");
  QLabel *rad =new QLabel("Sphere radius");
  QLabel *krad =new QLabel("Covalent radius");
//  QLabel *stwal =new QLabel("Draw intersecting planes");
//  QLabel *strin =new QLabel("Draw outer lines");
//  QLabel *stsph =new QLabel("Draw ellipsoid surface");  
//  QLabel *stsld =new QLabel("Solid ellipsoid");
//  QLabel *stnla =new QLabel("No label!");
//  stnla->setAlignment(Qt::AlignRight);
//  smbl->setAlignment(Qt::AlignRight);
//  rad->setAlignment(Qt::AlignRight);
//  krad->setAlignment(Qt::AlignRight);
//  stwal->setAlignment(Qt::AlignRight);
//  strin->setWhatsThis("If this check box is half selected <font color=gray><b>white</b></font> <b>outer lines</b> will be drawn around the ellipsoid which looks nice when the atom has a dark color.<br> If this is fully selected <b>black outer lines</b> will be drawn around the ellipsoid.");
//  strin->setAlignment(Qt::AlignRight);
//  stsph->setAlignment(Qt::AlignRight);
//  stsld->setAlignment(Qt::AlignRight);
  walbx = new QCheckBox("Draw intersecting planes");
  rinbx = new QCheckBox("Draw outer lines");
  rinbx->setTristate();
  sphbx = new QCheckBox("Draw ellipsoid surface");
  sldbx = new QCheckBox("Solid ellipsoid");
  nlabx = new QCheckBox("No label!");
  walbx->setCheckState ( Qt::Checked);
  rinbx->setCheckState ( Qt::Checked);
  rinbx->setWhatsThis("If this check box is half selected <font color=gray><b>white</b></font> <b>outer lines</b> will be drawn around the ellipsoid which looks nice when the atom has a dark color.<br> If this is fully selected <b>black outer lines</b> will be drawn around the ellipsoid.");
  sphbx->setCheckState ( Qt::Checked);
  sldbx->setCheckState ( Qt::Unchecked);
  nlabx->setCheckState ( Qt::Unchecked);
  //connect(sldbx,SIGNAL(stateChanged(int)),this,SLOT(updateLEs(int)));
  eacGLW->pse << "H" << "He" << "Li" << "Be" << "B" << "C" << "N" << "O" << "F" << "Ne" << "Na" << "Mg" << "Al" << "Si" << "P" << "S" << "Cl" << "Ar" << 
    "K" << "Ca" << "Sc" << "Ti" << "V" << "Cr" << "Mn" << "Fe" << "Co" << "Ni" << "Cu" << "Zn" << "Ga" << "Ge" << "As" << "Se" << "Br" << "Kr" << 
    "Rb" << "Sr" << "Y" << "Zr" << "Nb" << "Mo" << "Tc" << "Ru" << "Rh" << "Pd" << "Ag" << "Cd" << "In" << "Sn" << "Sb" << "Te" << "J" << "Xe" << 
    "Cs" << "Ba" <<  "La" << "Ce" << "Pr" << "Nd" << "Pm" << "Sm" << "Eu" << "Gd" << "Tb" << "Dy" << "Ho" << "Er" << "Tm" << "Yb" << "Lu" << 
    "Hf" << "Ta" << "W" << "Re" << "Os" << "Ir" << "Pt" << "Au" << "Hg" << "Tl" << "Pb" << "Bi" << "Po" << "At" << "Rn" << "Fr" << "Ra" << 
    "Ac" << "Th" << "Pa" << "U" << "Np" << "Pu" << "Am" << "Cm" << "Bk" << "Cf" << "Es" << "Fm" << "Md" << "No" << "Lr" << "Ku" << "Ha" << "Rf" << "Ns" << "Hs" << "Mt";
    eacCB->addItems(eacGLW->pse);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QPushButton *colorbut =new QPushButton("Color");
    
    eacKRAD->setText(QString::number(mol.Kovalenz_Radien[0]/100.0));
    eacRAD->setText(QString::number(mol.Kovalenz_Radien[0]/250.0));
    eacGLO->addWidget(eacGLW,0,0,5,6);
    eacGLO->addWidget(smbl,0,7);
    eacGLO->addWidget(rad,1,7);
    eacGLO->addWidget(krad,2,7);
    eacGLO->addWidget(eacCB,0,6);
    eacGLO->addWidget(eacRAD,1,6);
    eacGLO->addWidget(eacKRAD,2,6);
    eacGLO->addWidget(colorbut,3,7);
//    eacGLO->addWidget(stnla,4,7);
    eacGLO->addWidget(nlabx,4,6);
//    eacGLO->addWidget(stwal,5,1);
    eacGLO->addWidget(walbx,5,0);
//    eacGLO->addWidget(strin,5,3);
    eacGLO->addWidget(rinbx,5,2);
//    eacGLO->addWidget(stsph,5,5);
    eacGLO->addWidget(sphbx,5,4);
//    eacGLO->addWidget(stsld,5,7);
    eacGLO->addWidget(sldbx,5,6);
    eacGLO->addWidget(buttonBox,6,0,1,8);
    setLayout(eacGLO);
    connect(colorbut,SIGNAL(pressed()),eacGLW,SLOT(colorDLG()));
    connect(eacCB,SIGNAL(currentIndexChanged(int)), eacGLW, SLOT(changeAtom(int)));
    connect(eacCB,SIGNAL(currentIndexChanged(int)), this, SLOT(updateLEs(int)));
    updateLEs(0);
    connect(walbx,SIGNAL(stateChanged(int)),eacGLW,SLOT(wallA(int)));

    connect(nlabx,SIGNAL(stateChanged(int)),eacGLW,SLOT(nolaA(int)));
    connect(walbx,SIGNAL(stateChanged(int)),this,SLOT(checkSolid()));
    connect(rinbx,SIGNAL(stateChanged(int)),eacGLW,SLOT(ringA(int)));
    connect(sphbx,SIGNAL(stateChanged(int)),eacGLW,SLOT(spheA(int)));
    connect(sphbx,SIGNAL(stateChanged(int)),this,SLOT(checkBall()));
    connect(sldbx,SIGNAL(stateChanged(int)),eacGLW,SLOT(soldA(int)));
    connect(sldbx,SIGNAL(stateChanged(int)),this,SLOT(checkWall()));
    //  eacGLW->wallA(walbx->checkState());
    //eacGLW->spheA(sphbx->checkState());
    //eacGLW->ringA(rinbx->checkState());
    //eacGLW->soldA(sldbx->checkState());
}
void EacDlg::accept(){
  int i=eacGLW->m.Get_OZ(eacGLW->xd.at(0).atomname);
  extern molekul mol;
  mol.Acol[i][0]=eacGLW->m.Acol[i][0];
  mol.Acol[i][1]=eacGLW->m.Acol[i][1];
  mol.Acol[i][2]=eacGLW->m.Acol[i][2];
  mol.Acol[i][3]=eacGLW->m.Acol[i][3];
  mol.arad[i]=eacRAD->text().toDouble();
  int stt=0;
  if (sphbx->checkState())stt|=ATOM_STYLE_SPHERE;
  if (rinbx->checkState())stt|=ATOM_STYLE_RINGS;
  if (rinbx->checkState()==Qt::PartiallyChecked)stt|=ATOM_STYLE_WHITERING;
  if (walbx->checkState())stt|=ATOM_STYLE_WALLS;
  if (sldbx->checkState())stt|=ATOM_STYLE_SOLID;
  if (nlabx->checkState())stt|=ATOM_STYLE_NOLABEL;
  mol.aStyle[i]=stt;
  mol.Kovalenz_Radien[i]=(int)(eacKRAD->text().toDouble()*100);
  done(QDialog::Accepted);
}
