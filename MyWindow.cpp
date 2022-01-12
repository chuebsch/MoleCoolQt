#include "MyWindow.h"
#include "cubeGL.h"
#include "highlighter.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "doxd.h"
#include "molekul.h"
#include "invariomDlg.h"
#include "eacDlg.h"
#include "gradDlg.h"
#include "molisoStartDlg.h"
#include "ewaldsphere.h"
#include <locale.h>
int rev=615;
int atmax,smx,dummax,egal;
V3 atom1Pos,atom2Pos,atom3Pos;
QList<INP> xdinp,oxd,asymmUnit;
QList<Modulat> masymmUnit,matoms;
molekul mol;
bool XPRTHKL=false; 
int hatlokale=0;
int pdfOnAtom=-1;
int curentPhase=1;
FILE *polifile=NULL;
const double g2r=180.0/M_PI;
bool symmsalabymm = false; 
const double twologtwo=2.0*log(2.0);
double cfac=1.0;//(M_PI*M_PI*M_PI*8)/6;
double dfac=1.0;//(M_PI*M_PI*M_PI*M_PI*16)/24;
double fl(double x,double y,double z){
	double a,b,c;
  const double g2r=180.0/M_PI;
	a=(mol.zelle.ga==90.0)?0.0:2.0*x*y*mol.zelle.a*mol.zelle.b*cos(mol.zelle.ga/g2r);
	b=(mol.zelle.be==90.0)?0.0:2.0*x*z*mol.zelle.a*mol.zelle.c*cos(mol.zelle.be/g2r);
	c=(mol.zelle.al==90.0)?0.0:2.0*y*z*mol.zelle.b*mol.zelle.c*cos(mol.zelle.al/g2r);
  double erg=sqrt(x*x*mol.zelle.a*mol.zelle.a+
		  y*y*mol.zelle.b*mol.zelle.b+
		  z*z*mol.zelle.c*mol.zelle.c+
		  a+b+c);
  return erg;
}

void MyWindow::setup_zelle(){

//  packAct->setVisible(true);
/*
  double
          cs_al=(mol.zelle.al==90)?0:cos(mol.zelle.al/g2r),
          cs_be=(mol.zelle.be==90)?0:cos(mol.zelle.be/g2r),
          cs_ga=(mol.zelle.ga==90)?0:cos(mol.zelle.ga/g2r),

          sn_al=(mol.zelle.al==90)?1:sin(mol.zelle.al/g2r),
          sn_be=(mol.zelle.be==90)?1:sin(mol.zelle.be/g2r),
          sn_ga=(mol.zelle.ga==90)?1:sin(mol.zelle.ga/g2r);

  mol.zelle.phi=  sqrt(1-(cs_al*cs_al)-(cs_be*cs_be)-(cs_ga*cs_ga) + 2*cs_al*cs_be*cs_ga);
  mol.zelle.V = mol.zelle.a*mol.zelle.b*mol.zelle.c*mol.zelle.phi;
  mol.zelle.as=mol.zelle.c*mol.zelle.b*sn_al/mol.zelle.V;
  mol.zelle.bs=mol.zelle.c*mol.zelle.a*sn_be/mol.zelle.V;
  mol.zelle.cs=mol.zelle.a*mol.zelle.b*sn_ga/mol.zelle.V;
  const double tau=mol.zelle.c*((cs_al-cs_be*cs_ga)/sn_ga);
  mol.zelle.o1.m11=mol.zelle.o[0][0] = mol.zelle.as*mol.zelle.a;
  mol.zelle.o1.m12=mol.zelle.o[0][1] = 0.0;
  mol.zelle.o1.m13=mol.zelle.o[0][2] = 0.0;
  mol.zelle.o1.m21=mol.zelle.o[1][0] = mol.zelle.bs*mol.zelle.b*cs_ga;
  mol.zelle.o1.m22=mol.zelle.o[1][1] = mol.zelle.bs*mol.zelle.b*sn_ga;
  mol.zelle.o1.m23=mol.zelle.o[1][2] = 0.0;
  mol.zelle.o1.m31=mol.zelle.o[2][0] = mol.zelle.cs*mol.zelle.c* cs_be;
  mol.zelle.o1.m32=mol.zelle.o[2][1] = mol.zelle.cs*tau;
  mol.zelle.o1.m33=mol.zelle.o[2][2] = mol.zelle.cs*mol.zelle.c* mol.zelle.phi / sin(mol.zelle.ga /g2r);
  mol.zelle.f2c.m11 = mol.zelle.a;
  mol.zelle.f2c.m21 = 0.0;
  mol.zelle.f2c.m31 = 0.0;
  mol.zelle.f2c.m12 = mol.zelle.b * cs_ga;
  mol.zelle.f2c.m22 = mol.zelle.b * sn_ga;
  mol.zelle.f2c.m32 = 0.0;
  mol.zelle.f2c.m13 = mol.zelle.c * cs_be;
  mol.zelle.f2c.m23 = tau;
  mol.zelle.f2c.m33 = mol.zelle.c * mol.zelle.phi / sn_ga;// */
  mol.setup_zelle();
 // qDebug()<< mol.zelle.o1.m11<<mol.zelle.o1.m12<<mol.zelle.o1.m13<<"\n"<<mol.zelle.o1.m21<< mol.zelle.o1.m22<<mol.zelle.o1.m23<<"\n"<<mol.zelle.o1.m31<< mol.zelle.o1.m32<<mol.zelle.o1.m33;
}
char *egals;
QProgressBar *balken;
#define iabs(a) (a>0)?(a):(-a) 

MyWindow::MyWindow( QMainWindow *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags)  {

  QMainWindow::setDockOptions(QMainWindow::AnimatedDocks|QMainWindow::AllowTabbedDocks|QMainWindow::ForceTabbedDocks|QMainWindow::VerticalTabs);
  //  DOCK
  ffmpegedt=NULL; 
  mp4file=NULL;
  printf("{%s}\n", setlocale(LC_ALL,"C"));//suse11.3 braucht das
  filtered=0;
  speedSldr = new QSlider (Qt::Horizontal);
  speedSldr->setMaximum(128);
  speedSldr->setMinimum(1);
  speedSldr->setValue(63);
  speedSldr->hide();
  /////
  dock = new QDockWidget("Information Sidebar",this);
  dock->setAllowedAreas(Qt::AllDockWidgetAreas);  
  infoKanal = new QTextBrowser(dock);
  infoKanal->setLineWrapMode(QTextEdit::NoWrap);
  dock->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetClosable);
  dock->setWidget(infoKanal);
  addDockWidget(Qt::LeftDockWidgetArea, dock);

  dock2 = new QDockWidget("Hydrogen Bond Sidebar",this);
  dock->setAllowedAreas(Qt::AllDockWidgetAreas);  
  hbKanal  = new QTextBrowser(dock2);
  hbKanal->setLineWrapMode(QTextEdit::NoWrap);
  dock2->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetClosable);
  dock2->setWidget(hbKanal);
  addDockWidget(Qt::LeftDockWidgetArea, dock2);

  QMainWindow::tabifyDockWidget (dock2,dock);
  dock2->hide();
  // DOCK
  atom1Pos=V3(0,0,0);
  atom2Pos=V3(0,0,0);
  atom3Pos=V3(0,0,0);
  atmax=smx=dummax=0;
  maxResi=0;
  mol.loadSettings();
  mol.einstellung = new QSettings( QSettings::IniFormat, QSettings::UserScope ,"Christian_B._Huebschle","MoleCoolQt" );
  mol.einstellung->beginGroup("Version 0.1");
  double vang=27.0;
  if (mol.einstellung->contains("ViewAngle")){
    vang=mol.einstellung->value("ViewAngle",27.0).toDouble();
  }
  cubeGL =new CubeGL(this);
  cubeGL->bas=0;
  mol.vorobas=0;
  scalePic=1.0;
  cubeGL->scalePicNow=1.0;
  cubeGL->setMinimumSize(200,100);
  createmoliso = new QAction(tr("Create isosurface"),this);
  createmoliso->setWhatsThis("<img src=\":/images/createmoliso.png\"> Creates one or\
 more iso-surfaces which could be mapped by a color gradient.");
  connect(createmoliso, SIGNAL (triggered ()),
          this, SLOT(genMoliso()));
  createmoliso->setIcon(QIcon(":/images/createmoliso.png"));

  noMoliso = new QAction( QIcon(":/images/nomoliso.png"),"Delete MOLecular ISOsurface",this);
  connect(noMoliso, SIGNAL (triggered ()),
          this, SLOT(destroyMoliso()));


  createmoliso->setVisible(true);
  noMoliso->setVisible(false);
  showface = new QAction(tr("Toggle MolIso-surface"),this);
  showface->setIcon(QIcon(":/images/moliso_molecool_32x32.png"));
  showface->setShortcut(tr("I"));
  showface->setCheckable ( true );
  showface->setVisible(false);
  connect(showface, SIGNAL (toggled (bool)),
	  cubeGL, SLOT(setIsosurface(bool)));

  showLeg = new QAction(tr("Toggle MolIso-legend"),this);
  showLeg->setIcon(QIcon(":/images/legende.png"));
  showLeg->setShortcut(tr("Shift+L"));
  showLeg->setCheckable ( true );
  showLeg->setVisible(false);
  connect(showLeg, SIGNAL (toggled (bool)),
	  cubeGL, SLOT(setLegend(bool)));

  movLeg = new QAction(tr("Move MolIso-legend"),this);
  movLeg->setIcon(QIcon(":/images/move-legend.png"));
  movLeg->setShortcut(tr("Ctrl+M"));
  movLeg->setCheckable ( true );
  movLeg->setVisible(false);
  connect(movLeg, SIGNAL (toggled(bool)),
	  cubeGL, SLOT(setLegendMovable(bool)));

  mildir = new QAction(tr("Toggle legend direction"),this);
  mildir->setIcon(QIcon(":/images/dreh-legende.png"));
  mildir->setCheckable ( true );
  mildir->setShortcut(tr("Shift+M"));
  mildir->setChecked( true );
  mildir->setVisible(false);
  connect(mildir, SIGNAL(toggled(bool)),
	  cubeGL, SLOT(toggleMolisoLegendDirection()));

  QMenu *stereoMenu = new QMenu(tr("&3D Stereo"), this);
  QAction *zm_stereo = stereoMenu->addAction("Zalman Monitor 3D");
  QAction *pe_stereo = stereoMenu->addAction("Parallel Eye Side by Side Stereo");
  QAction *xe_stereo = stereoMenu->addAction("Crossed Eye Side by Side Stereo");
  QAction *no_stereo = stereoMenu->addAction("Stereo Mode off");
  connect(zm_stereo,SIGNAL(triggered()),cubeGL,SLOT(zalman()));
  connect(pe_stereo,SIGNAL(triggered()),cubeGL,SLOT(parallel()));
  connect(xe_stereo,SIGNAL(triggered()),cubeGL,SLOT(crosseye()));
  connect(no_stereo,SIGNAL(triggered()),cubeGL,SLOT(nostereo()));
  QActionGroup *stereoGroup = new QActionGroup(this); 
  stereoGroup->addAction(zm_stereo);
  stereoGroup->addAction(pe_stereo);
  stereoGroup->addAction(xe_stereo);
  stereoGroup->addAction(no_stereo);
  zm_stereo->setCheckable(true);
  pe_stereo->setCheckable(true);
  xe_stereo->setCheckable(true);
  no_stereo->setCheckable(true);

  no_stereo->setChecked(true);



  sfacMenu= new QMenu("Allowed Polyeders",this);
  sfacMenu->setIcon(QIcon(":/images/poly.png"));
  QMenu *MolIsoMenu = new QMenu(tr("&MolIso"), this);  
  MolIsoMenu->addAction(createmoliso);
  MolIsoMenu->addAction(showface);
  MolIsoMenu->addAction(showLeg);
  MolIsoMenu->addAction(movLeg);
  MolIsoMenu->addAction(mildir);
  MolIsoMenu->addSeparator();
  MolIsoMenu->addAction(noMoliso);

  QMenu *fileMenu = new QMenu(tr("&File"), this);  
  QMenu *workMenu = new QMenu(tr("&Move"), this);
  QMenu *viewMenu = new QMenu(tr("&View"), this);
  QMenu *dialogMenu = new QMenu(tr("S&ettings"), this);
  invariomMenu = new QMenu(tr("In&varioms"), this);
  QMenu *helpMenu = new QMenu(tr("&Help"), this);

  xdSetupAct = new QAction(tr("Modify XD local coordinate systems"),this);
  xdMenu     = new QMenu(tr("&XDstuff"), this);
  xdSetupAct->setCheckable ( true );
  xdSetupAct->setChecked ( false );
  xdSetupAct->setEnabled(false);
  xdSetupAct->setIcon(QIcon(":/images/xdlc.png") );
  xdSetupAct->setStatusTip("Modify XD local coordinate systems and symmetry-refinement-key codes");
  xdSetupAct->setWhatsThis("<img src=\":/images/xdlc.png\"><p>Click here to <b>change local coordinate</b> systems in a dialog. MoleCoolQt checks for symmetry elements along the axes of the local coordinate systems. Refinement <b>key-codes fitting to these local symmetry</b> will be written into <i>xd.mas</i>. <i>xd.mas</i> and <i>xd.res</i> will be uptated to the new choosen local coodinate system. <b>Backup files</b> will be generated.");
  xdRestoreAct = new QAction(tr("Undo modification of XD-files"),this);
  xdRestoreAct->setStatusTip("Undo the moifications done by 'Modify XD local coordinate systems'-dialog");
  QAction *xdEditAct = new QAction(tr("Edit xd.mas"),this);
  QAction *resinpAct = new QAction(tr("Copy xd.res -> xd.inp"),this);
  QAction *xdlsmAct  = new QAction(tr("Run xdlsm"),this);
  QAction *xdpropAct = new QAction(tr("Run xdprop"),this);
  QAction *xdEnvAct  = new QAction(tr("Change XD enviroment variables"),this);
  QAction *tidyCPSAct = new QAction(tr("Tidy up xd_rho.cps"),this);
  connect(tidyCPSAct, SIGNAL (triggered ()),
	  this, SLOT(tidyCPS()));
  connect(xdEditAct, SIGNAL (triggered ()),
	  this, SLOT(editXDmas()));
  connect(xdRestoreAct, SIGNAL (triggered ()),
	  this, SLOT(restoreXDfiles()));
  connect(resinpAct, SIGNAL (triggered ()),
	  this, SLOT(resinp()));
  connect(xdpropAct, SIGNAL (triggered ()),
	  this, SLOT(xdprop()));
  connect(xdlsmAct, SIGNAL (triggered ()),
	  this, SLOT(xdlsm()));
  connect(xdSetupAct, SIGNAL(toggled(bool)),
	  cubeGL, SLOT(toggXDSetup(bool)));
  connect(xdEnvAct, SIGNAL (triggered ()),
	  this, SLOT(xdenv()));
  xdMenu->addAction(xdSetupAct); 
  xdMenu->addAction(xdRestoreAct); 
  xdMenu->addAction(xdEditAct); 
  xdMenu->addAction("Get CPs from xd_pro.out (bubble search)",this, SLOT(bubblebub()));
  {
    QAction *a=xdMenu->addAction("Edit xd_part.aux ",this,SLOT(editPartAux()));
  a->setData(0);
  }
  xdMenu->addAction(resinpAct); 
  xdMenu->addAction(xdlsmAct); 
  xdMenu->addAction(xdpropAct); 
  xdMenu->addAction(tidyCPSAct); 
  xdMenu->addAction("Show Reciprocal Space",this,SLOT(rezi()));
  xdMenu->addSeparator();
  xdMenu->addAction("Find all uniq peaks in Fo-Fc map > iso surface value.",this,SLOT(addMoreQPeaks())) ;
//  xdMenu->addAction("Calculate pdf of an atom.",this,SLOT(pdfDlg()));
//  xdMenu->addAction("Calculate pdf of all atoms.",this,SLOT(pdfDlg2()));
  xdMenu->addAction("PDB export (check xd_part.aux)!",this,SLOT(pdbOutput()));
  xdMenu->addAction(xdEnvAct);

  xdMenu->setEnabled(false);

  depthCueingAct = new  QAction(tr("Depth Cueing"),this);
  depthCueingAct->setCheckable ( true );
  depthCueingAct->setChecked ( false );
  connect(depthCueingAct, SIGNAL (toggled(bool)),
	  cubeGL, SLOT(setDepthCueing(bool)));

  cubeGL->toggXDSetup(false);
  seReAct=new QAction(QIcon(":/images/sere.png"),"Select Residues",this);
  seReAct->setStatusTip("Residues are highlighted by clicking on an atom, you can scroll different residues with the mouse wheel");
  seReAct->setEnabled(false);
  seReAct->setCheckable(true);
  seReAct->setChecked(false);
  connect(seReAct, SIGNAL (toggled(bool)),
	  cubeGL, SLOT(togglReSe(bool)));
  ModulationMenu = new QMenu("Modulation");
  ModulationMenu->setEnabled(false);
  QAction *MIA,*dgm;
  seReAct->setVisible(false);
  menuBar()->addMenu(fileMenu);
  menuBar()->addMenu(workMenu);   
  menuBar()->addMenu(viewMenu);
  dgm=menuBar()->addMenu(dialogMenu);
  menuBar()->addMenu(invariomMenu);
  menuBar()->addMenu(xdMenu);
  menuBar()->addMenu(ModulationMenu);
  MIA=menuBar()->addMenu(MolIsoMenu);
  menuBar()->addMenu(stereoMenu);
  menuBar()->addMenu(helpMenu);

  QToolButton *homeMe = new QToolButton(this);
  homeMe->setIcon(QIcon(":images/home.png"));
  homeMe->setText("home");
  homeMe->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

  connect(homeMe,SIGNAL(clicked()),cubeGL,SLOT(homeXY()));



  aboutAct = helpMenu->addAction(QIcon(":/images/icon1.png"),
					 tr("&About MoleCoolQt"), this, SLOT(about() ),
					 QKeySequence(tr("Ctrl+~", "Help|About MoleCoolQt")));

  for (int i = 0; i < MaxRecentFiles; ++i) {
    recentFileActs[i] = new QAction(this);
    recentFileActs[i]->setVisible(false);
    connect(recentFileActs[i], SIGNAL(triggered()),
	    this, SLOT(openRecentFile()));
  }

  QAction *wasistdasAct    = new QAction(tr("What is this?"),this);
 // QAction *printAct        = new QAction(tr("Print"),this);
  QAction *zoomInAct       = new QAction(tr("Zoom in"), this);
  QAction *zoomOutAct      = new QAction(tr("Zoom out"), this);
  QAction *rotLeftAct      = new QAction(tr("Rotate left"), this);
  QAction *rotRightAct     = new QAction(tr("Rotate right"), this);
  QAction *rotUpAct        = new QAction(tr("Rotate upwards"), this);
  QAction *rotDownAct      = new QAction(tr("Rotate downwards"), this);
  QAction *fontSizeUpAct   = new QAction(tr("Increase label size"), this);
  QAction *fontSizeDownAct = new QAction(tr("Decrease label size"), this);
  atomClickAct    = new QAction(tr("Picking mode"), this );
  QAction *mveRightAct     = new QAction(tr("Move right"), this );
  QAction *mveUpAct        = new QAction(tr("Move up"), this );
  QAction *mveDownAct      = new QAction(tr("Move down"), this );
  QAction *mveLeftAct      = new QAction(tr("Move left"), this );
  togAtom         = new QAction(tr("Toggle atoms"), this);
  togBond         = new QAction(tr("Toggle bonds"), this);
  togLabel        = new QAction(tr("Toggle labels"), this);
  QAction *togStat         = new QAction(tr("Toggle status bar"), this);
  QAction *movLabAct       = new QAction(tr("Label move mode"),this);
  QAction *homeLabAct      = new QAction(tr("Home lapel positions"),this);
  QAction *editAtomAct     = new QAction(tr("Edit atom parameters"),this);
  togUnit                  = new QAction(tr("Toggle unit cell"), this);
//  togLuft                  = new QAction(tr("Toggle helices"), this);
  togElli                  = new QAction(tr("Toggle ADP's"), this);
  togHBond        = new QAction(tr("Toggle hydrogen bonds"), this);
  QAction *backColor       = new QAction(tr("Change background color"), this);
  QAction *labelColor      = new QAction(tr("Change label color"), this);
  QAction *labelFont       = new QAction(tr("Change label font"), this);  
  back_Grad       = new QAction(tr("Toggle background gradient"), this);
  donTGrow       = new QAction(tr("Do not Auto-Grow on loading"), this);
  QAction *scalePicAct     = new QAction(tr("Change screen shot scale factor"), this);
  QAction *changeProbAct   = new QAction(tr("Change ADP probability"),this);
  changeProbAct->setData(0);
  QAction *matrixAct       = new QAction(tr("Apply old matrix"),this);
  QAction *invExpAct       = new QAction(tr("Export 'Invariome.in'"),this);
  QAction *invEdiAct       = new QAction(tr("Edit invariomnames"),this);
  QAction *invLoadAct      = new QAction(tr("Load a local invariom data base"),this);
  filterAct                = new QAction(tr("Launch a filter dialog"),this);
  unfilterAct              = new QAction(tr("Unfilter"),this);
  ydlStartAct =              new QAction(tr("Start rotation"),this);
  ydlStopAct =               new QAction(tr("Stop rotation"),this);
  togAxen =                  new QAction(tr("Toggle local coordinate systems"), this);
  act2 = fileMenu->addAction(QIcon(":/images/folder_page_white.png"),
			     tr("&Open..."), this, SLOT(openFile() ),
			     QKeySequence(tr("Ctrl+O", "File|Open")));
  act2->setWhatsThis("Click here to <b>load</b> a structure. Currently <b>MoleCoolQt</b> understands the following file formats:"
		     "<p><b> XD </b>(*.res, *.inp, *.mas) "
		     "<br><b> SHELX </b>(*.ins, *.res) "
		     "<br><b> Gaussian COM </b>(*.com) "
		     "<br><b> Crystal Information File </b>(*.cif) &Dagger;"
		     "<br><b> PDB </b>(*.pdb, *.ent) &Dagger;"
		     "<p>&Dagger;: Implemetation is still experimental!");
 
  act1 =fileMenu->addAction(QIcon(":/images/diskette.png"),
			    tr("&Save screenshot"),this,SLOT(saveScene()),
			    QKeySequence(tr("S","File|Save")));
  act1->setWhatsThis("Click here to <b>save a screenshot</b> of your stucture. Currently <b>MoleCoolQt</b> can save <b>JPG BMP PPM</b> and <b>PNG</b> files. "
		     "You can scale the resolution of the stored images by a factor. This factor can be chaged in the 'Settings' menu. "
		     "A scale factor of 1 result in a image in screen resulution. ");
/*  printAct->setStatusTip("Print on the printer");
  printAct->setShortcut(tr("Ctrl+P"));
  printAct->setIcon(QIcon(":/images/printer.png"));
  printAct->setWhatsThis("Click here to <b>print</b> your stucture on your printer. The printed picture is scaled to fit the page."
			 "You can chage the orientation in the print dialog.");*/
  wasistdasAct->setStatusTip("Shows some explanations to some buttons.");  
  wasistdasAct->setShortcut(tr("Shift+F1"));		   
  wasistdasAct->setIcon(QIcon(":/images/whatsthis.png"));
  wasistdasAct->setWhatsThis("Click here to get some <b>helping</b> hints about the functionality of some of the Buttons."
			     " Please click again here and then on the Button you are interested in.");
  //fileMenu->addAction(printAct);
  helpMenu->addAction(wasistdasAct);
  fileMenu->addSeparator();
  for (int i = 0; i < MaxRecentFiles; ++i)
    fileMenu->addAction(recentFileActs[i]);
  fileMenu->addSeparator();
  fileMenu->addAction("Export Fourier Maps",this,SLOT(exportFMaps()));
  fileMenu->addAction(createmoliso);
  fileMenu->addAction(noMoliso);
  cntrPlot = fileMenu->addAction(QIcon(":images/cont.png"),"Create Contour Plot",this, SLOT(contourPlot()));
  ldipAct = fileMenu->addAction(QIcon(":images/dipole.png"),tr("Load &Dipole moments"),this,SLOT(openDipoleFile()),QKeySequence(tr("D", "File|Load &Dipole moments"))); 
  ldipAct->setWhatsThis("<img src=\":images/dipole.png\"> This loads dipole moments from a file.<br>\
The first three numbers in a row are taken as xyz of the dipole vector.<br>\
You can also specify acolor as RGB after ## or as in HTML after color= in &quot;&quot; here is a example:<br>\
<pre>8.0 -4.0 3.0 \nxd_abbild:  px =      11.0      py =       0.5       pz =       9.2   ## 0.5 0.5 0.0\ngaussian: 6.8986     1.7407     6.1785        color=\"red\"\nInvtoolpx =       7.5  py =      -1.1  pz =       5.6     color=\"#ed0ed0\"\n6.8986     1.7407     6.1785 origin={-4.0,-4.0,-4.0}</pre>");

  nodipAct = fileMenu->addAction(QIcon(":images/nodipole.png"),tr("Remove Dipole moments"),this,SLOT(removeDipoleMoments())); 
  nodipAct->setVisible ( false);
  fileMenu->addAction("export visible atoms etc",this,SLOT(exportVisible()));
  fileMenu->addSeparator();
  act3 = fileMenu->addAction(QIcon(":/images/cancel.png"), tr("&Quit"),
			     this, SLOT(close()),
			     QKeySequence(tr("Q", "File|Quit")) );


  act2->setStatusTip(tr("Opens structure files"));
  act1->setStatusTip(QString(tr("Saves a screen shot in %1 fold screen resolution").arg(scalePic)));
  act3->setStatusTip(tr("Quit program"));
  act3->setWhatsThis("Click here to exit <b>MoleCoolQt</b>. <p> The window size and position, "
		     "label and background color and the recently opened files list are stored. "
		     "The location depends on your operating system: "
		     "<p><b>Windows</b>: C:\\Documents and Settings\\User Name\\Application Data\\Christian_B.Huebschle\\MoleCoolQt.ini"
		     "<p><b>Linux</b>: $HOME/.config/Christian_B._Huebschle/MoleCoolQt.conf"   );
  
  unfilterAct->setStatusTip(tr("Reset the structure to the original. "));
  unfilterAct->setIcon(QIcon(":/images/unfilter.png") );
  unfilterAct->setVisible(false);

  back_Grad->setStatusTip("toggle background gradient");
  back_Grad->setCheckable ( true );
  back_Grad->setChecked ( true );    
  back_Grad->setShortcut(Qt::Key_F4);
  donTGrow->setCheckable ( true );
  donTGrow->setChecked ( false );  
  cubeGL->back_grad=true;
  QMenu *packMenu=new QMenu("Pack");
  QAction *dntpck=packMenu->addAction("Only asymmetric unit (AU)");
  grdpckAct=packMenu->addAction(QString("Pack shortest distances and complete"));
  QAction *molpck=packMenu->addAction("Complete molecules");
  QAction *cctpck=packMenu->addAction("Pack inside unit cell");
  QAction *ccmpck=packMenu->addAction("Pack inside unit cell and complete AU");
  cubeGL->changeGDAct =packMenu->addAction(tr("Search radius is %1 Ang.  Change it").arg(mol.gd));
  connect(cubeGL->changeGDAct,SIGNAL(triggered()),this,SLOT(changeGD()));
  cctpck->setIcon(QIcon(":/images/unitcell.png"));
  molpck->setIcon(QIcon(":/images/compmol.png"));
  ccmpck->setIcon(QIcon(":/images/unitcellplus.png"));
  QSignalMapper *signalMapper = new QSignalMapper(this);
  signalMapper->setMapping(dntpck,  0);
  signalMapper->setMapping(grdpckAct,  1);
  signalMapper->setMapping(molpck,  6);
  signalMapper->setMapping(cctpck,  3);
  signalMapper->setMapping(ccmpck,  4);
  connect(dntpck,SIGNAL(triggered()),signalMapper,SLOT(map()));
  connect(molpck,SIGNAL(triggered()),signalMapper,SLOT(map()));
  connect(cctpck,SIGNAL(triggered()),signalMapper,SLOT(map()));
  connect(ccmpck,SIGNAL(triggered()),signalMapper,SLOT(map()));
  connect(grdpckAct,SIGNAL(triggered()),signalMapper,SLOT(map()));
  connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(setPackArt2(int)));
  QAction *boxpck=packMenu->addAction("Pack inside given limits",signalMapper,SLOT(map()));
  signalMapper->setMapping(boxpck,  7);
  menuBar()->insertMenu(dgm, packMenu);
//  packAct = new QAction(QIcon(":/images/compmol.png"),tr("Show pack dialog "),this);
//  packAct->setStatusTip("Pack the molecule in the unit cell");
//  packAct->setMenu(packMenu);
//  packAct->setVisible(false);
//  connect(packAct,SIGNAL(triggered()),this,SLOT(showPackDlg()));
  filterAct->setIcon(QIcon(":/images/filter.png") );

  filterAct->setWhatsThis("<img src=\":/images/filter.png\"><p>Click here to <b>filter</b> some of the atoms out of the structure."
			  " In the filter dialog you can filter atoms by regular expressions or by index, atomic number, "
			  "residue and nuber of neighbor atoms.");

  zoomInAct->setStatusTip(tr("Enlarge molecule view")); 
  zoomInAct->setShortcut(Qt::Key_PageUp);
  zoomInAct->setIcon(QIcon(":/images/zoomin.png") );
  
  zoomOutAct->setStatusTip(tr("Shrink molecule view") );
  zoomOutAct->setShortcut(Qt::Key_PageDown);
  zoomOutAct->setIcon(QIcon(":/images/zoomout.png"));

  rotLeftAct->setShortcut(tr("Alt+Left"));
  rotLeftAct->setStatusTip(tr("Rotate molecule around Y-axis"));
  rotLeftAct->setIcon(QIcon(":/images/rotateleft.png") );

  rotRightAct->setShortcut(tr("Alt+Right"));
  rotRightAct->setStatusTip(tr("Rotate molecule around Y-axis") );
  rotRightAct->setIcon(QIcon(":/images/rotateright.png") );

  rotUpAct->setShortcut(tr("Alt+Up"));
  rotUpAct->setStatusTip(tr("Rotate molecule around X-axis") );
  rotUpAct->setIcon(QIcon(":/images/rotateup.png") );

  rotDownAct->setShortcut(tr("Alt+Down"));
  rotDownAct->setStatusTip(tr("Rotate molecule around X-axis") );
  rotDownAct->setIcon(QIcon(":/images/rotatedown.png") );
    
  fontSizeUpAct->setShortcut(Qt::Key_Plus);
  fontSizeUpAct->setStatusTip(tr("Increase the font size of the Labels.") );
  fontSizeUpAct->setIcon(QIcon(":/images/fontsizeup.png") );

  fontSizeDownAct->setShortcut(Qt::Key_Minus);
  fontSizeDownAct->setStatusTip(tr("Decrease the font size of the Labels.") );
  fontSizeDownAct->setIcon(QIcon(":/images/fontsizedown.png") );

  atomClickAct->setStatusTip(tr("Show Messages when clicking on an Atom"));
  atomClickAct->setIcon(QIcon(":/images/clickatoms.png"));
  atomClickAct->setCheckable ( true );
  atomClickAct->setChecked (  true);  
  atomClickAct->setWhatsThis("<img src=\":/images/clickatoms.png\"><p>Click here to toggle <b>interactive messages</b>."
			     " If this is checked you receive an message dialog when you click on an atom."
			     " In this message dialog <b>distances, angles and torsion angles</b> to atoms clicked before are displayed.");
  mveLeftAct ->setShortcut(tr("Ctrl+Left"));    
  mveLeftAct->setIcon(QIcon(":/images/moveleft.png") );

  mveDownAct ->setShortcut(tr("Ctrl+Down"));
  mveDownAct->setIcon(QIcon(":/images/movedown.png") );

  mveUpAct   ->setShortcut(tr("Ctrl+Up"));
  mveUpAct->setIcon(QIcon(":/images/moveup.png") );

  mveRightAct->setShortcut(tr("Ctrl+Right"));
  mveRightAct->setIcon(QIcon(":/images/moveright.png") );

  ydlStartAct->setShortcut(tr("Shift+Y"));
  ydlStartAct->setWhatsThis("This starts to rotate the stucture <b>continously</b>."
"<p>Every <b>33/1000 sek</b> the structure is moved <b>2&deg;</b> clockwise around the Y-axis."
"<p>You can stop the rotation by pressing the <b>SPACE</b>-Key on the keyboard. ");
  ydlStartAct->setIcon(QIcon(":/images/everrotateleft.png") );
  
  ydlStopAct->setEnabled(false);
  ydlStopAct->setShortcut(Qt::Key_Space);
  ydlStopAct->setIcon(QIcon(":/images/neverrotateleft.png") );

  togBond->setShortcut(tr("B"));
  togBond->setCheckable ( true );
  togBond->setIcon(QIcon(":/images/bonds.png") );
  togBond->setWhatsThis("<img src=\":/images/bonds.png\"><p>Click here to toggle the visualization of the <b>bonds</b> on and off." 
			" If this is checked bonds are visualized as sticks.");
  QMenu *bondMenu = new QMenu("change bond style");
  noBondAct = bondMenu->addAction("no bonds");
  noBondAct->setCheckable ( true );
  togBond->setChecked ( true );
  bondMenu->addSeparator();
  connect(noBondAct , SIGNAL(toggled (bool) ),
	   cubeGL, SLOT(setNoBond(bool) ));
  connect(noBondAct , SIGNAL(toggled (bool) ),
	   this, SLOT(syncBondActs() ));
  connect(togBond , SIGNAL(toggled (bool) ),
	   this, SLOT(syncBondActs() ));

  QAction *atomColoredAct = bondMenu->addAction("bonds colored like atoms");
  atomColoredAct ->setCheckable ( true );
  singleColorAct = bondMenu->addAction("single colored bonds");
  singleColorAct->setCheckable ( true );
  QActionGroup *bondStyleGroup= new QActionGroup(this);
  bondStyleGroup->addAction(atomColoredAct);
  bondStyleGroup->addAction(singleColorAct);
  atomColoredAct->setChecked(true);
  connect(singleColorAct,SIGNAL(toggled(bool)),
		  this,SLOT(toggleSingleColorBond(bool)));
  bondMenu->addSeparator();
  QAction *changeBondColorAct= bondMenu->addAction("change bond color");
  connect(changeBondColorAct,SIGNAL(triggered()),
		  cubeGL,SLOT(changeBondColor()));
  QAction *changeBondStrengthAct= bondMenu->addAction("change bond strength");
  connect(changeBondStrengthAct,SIGNAL(triggered()),
		  this,SLOT(changeBondStrength()));
  bondMenu->addAction("load coordinative bond list",cubeGL,SLOT(loadCbondList()));
  bondMenu->addAction("save coordinative bond list",cubeGL,SLOT(saveCbondList()));
  togBond->setMenu(bondMenu);

  togLabel->setShortcut(tr("L"));
  togLabel->setCheckable ( true );
  togLabel->setChecked ( true );
  togLabel->setIcon(QIcon(":/images/labels.png") );
  togLabel->setWhatsThis("<img src=\":/images/labels.png\"><p>Click here to toggle the visualization of the <b>labels</b> on and off." );

  movLabAct->setCheckable ( true );													 
  movLabAct->setChecked ( false);													 
  movLabAct->setIcon(QIcon(":/images/movelabel.png") );											 
  movLabAct->setWhatsThis("<img src=\":/images/movelabel.png\"><p>Click here to toggle the moveable label mode." );

  
  homeLabAct->setIcon(QIcon(":/images/homelabel.png") );											 
  homeLabAct->setWhatsThis("<img src=\":/images/homelabel.png\"><p>Click here to move labels back to the atoms." );

  togStat->setCheckable ( true );
  togStat->setChecked ( true );
  
  togAxen->setShortcut(tr("X"));
  togAxen->setCheckable ( true );
  togAxen->setChecked ( cubeGL->drawAx );
  togAxen->setIcon(QIcon(":/images/axen.png"));
  togAxen->setWhatsThis("<img src=\":/images/axen.png\"><p>Click here to toggle the visualization of the <b>local coordinate systems</b> on and off. " 
			"If this is checked local coordinate systems are visualized as colored lines. "
			"<b><font color=red>X</font></b>-direction is <b><font color=red>red</font></b>,  "
			"<b><font color=green>Y</font></b>-direction is  <b><font color=green>green</font></b>, "
			"<b><font color=blue>Z</font></b>-direction is  <b><font color=blue>blue</font></b>.");
  
  invExpAct->setIcon(QIcon(":/images/expoinva.png"));
  invExpAct->setStatusTip(tr("Export Invariomnames into 'Invariome.in'")); 
  invExpAct->setWhatsThis("<img src=\":/images/expoinva.png\"><p>If you click here you can <b>export</b> you modified <b>Invariomnames</b> into a file called 'Invarome.in' which can be used for <b>InvariomTool</b>");
  invEdiAct->setIcon(QIcon(":/images/editinva.png"));
  invEdiAct->setStatusTip(tr("Modify Invariomnames interactively")); 
  invEdiAct->setWhatsThis("<img src=\":/images/editinva.png\"><p>If this is selected you can modify <b>Invariomnames</b> by clicking on the atoms");

  invEdiAct->setCheckable ( true );
  invEdiAct->setChecked ( false );
  togUnit->setShortcut(tr("Z"));
  togUnit->setCheckable ( true );
  togUnit->setChecked ( true );
  togUnit->setIcon(QIcon(":/images/unitcell.png"));
  togUnit->setWhatsThis("<img src=\":/images/unitcell.png\"><p>Click here to toggle the visualization of the <b>unit cell</b> on and off. " 
			"If this is checked the unitcell is visualized by colored lines. "
			"<b><font color=red>a</font></b>-direction is  <b><font color=red>red</font></b>, "
			"<b><font color=green>b</font></b>-direction is  <b><font color=green>green</font></b>."			
			"<b><font color=blue>c</font></b>-direction is <b><font color=blue>blue</font></b>,  "
			);


/*
  togLuft->setShortcut(tr("#"));
  togLuft->setCheckable ( true );
  togLuft->setChecked ( true );
  togLuft->setIcon(QIcon(":/images/helix.png"));
  togLuft->setWhatsThis("<img src=\":/images/helix.png\"><p>Click here to toggle the visualization of the <b>cartoon like peptide chain visualizations</b>"
			" on and off. If this is checked the pepdide chain is visualized as a band or a line. In &alpha;-helix regions the chain "
			"is represented as a band. in all other regions the represetation is a line. The chain is colored as a "
		     "<b><font color=red>r</font><font color=orange>a</font><font color=yellow>i</font><font color=green>n</font><font color=cyan>b</font>"
			"<font color=blue>o</font><font color=violet>w</font></b> from "
			"<b><font color=red>red</font></b> to <b><font color=violet>violet</font></b>" );
*/
  togElli->setShortcut(tr("E"));
  togElli->setCheckable ( true );
  togElli->setChecked ( true );
  togElli->setIcon(QIcon(":/images/adps.png"));
  togElli->setWhatsThis("<img src=\":/images/adps.png\"><p>Click here to toggle the visualization style of the <b>atoms</b>."
			" If this is checked the atoms are visualized as ellipsoids if the structure provides thermal information.");
  QMenu *elliMenu = new QMenu("change ADP probability",this);
  elliMenu->setIcon(QIcon(":/images/adps.png"));
  noElli    = elliMenu->addAction("no ellipsoids");
  elliMenu->addSeparator ();
  QAction *tenp     = elliMenu->addAction("10%");
  QAction *thirtyp  = elliMenu->addAction("30%");
  QAction *fiftyp   = elliMenu->addAction("50%");
  QAction *seventyp = elliMenu->addAction("70%");
  QAction *ninetyp  = elliMenu->addAction("90%");
  tidl=NULL;
  
  noElli  ->setCheckable(true);    
  tenp    ->setCheckable(true);    
  thirtyp ->setCheckable(true);
  fiftyp  ->setCheckable(true);
  seventyp->setCheckable(true);
  ninetyp ->setCheckable(true);

  tenp    ->setData(10);
  thirtyp ->setData(30);
  fiftyp  ->setData(50);
  seventyp->setData(70);
  ninetyp ->setData(90);

  connect( noElli, SIGNAL(toggled(bool)),
	   this, SLOT(toggleElli(bool)));
  connect(tenp,SIGNAL(triggered()),
	  this,SLOT(changeADP()));
  connect(thirtyp,SIGNAL(triggered()),
	  this,SLOT(changeADP()));
  connect(fiftyp,SIGNAL(triggered()),
	  this,SLOT(changeADP()));
  connect(seventyp,SIGNAL(triggered()),
	  this,SLOT(changeADP()));
  connect(ninetyp,SIGNAL(triggered()),
	  this,SLOT(changeADP()));
  
  QActionGroup *probabilityGroup= new QActionGroup(this);
  probabilityGroup->addAction(tenp);
  probabilityGroup->addAction(thirtyp);
  probabilityGroup->addAction(fiftyp);
  probabilityGroup->addAction(seventyp);
  probabilityGroup->addAction(ninetyp);
  fiftyp->setChecked ( true );
  
  togElli->setMenu(elliMenu);

  togAtom->setShortcut(tr("A"));
  togAtom->setCheckable ( true );
  togAtom->setChecked ( true );
  togAtom->setIcon(QIcon(":/images/atoms.png") );
  togAtom->setWhatsThis("<img src=\":/images/atoms.png\"><p>Click here to toggle the visualization of the <b>atoms</b> on and off." 
			" If this is checked atoms are visualized as spheres or "
			" ellipsoids depending on the ADP <img src=\":/images/adps.png\"> selection.");
  QMenu *atomMenu = new QMenu("change atom style");
  noAtomAct= atomMenu->addAction("no atoms");
  noAtomAct->setCheckable ( true );
  atomMenu->addSeparator();
  QAction *tubiAct= atomMenu->addAction("toggle tubes / balls");
  tubiAct->setCheckable ( true );
  connect(tubiAct,SIGNAL(toggled(bool)),
		  this, SLOT(toggleTubes(bool)));
  connect( noAtomAct, SIGNAL(toggled (bool) ),
	   cubeGL, SLOT(setNoAtom(bool) ));
  connect(noAtomAct , SIGNAL(toggled (bool) ),
	   this, SLOT(syncAtomActs() ));
  connect(togAtom , SIGNAL(toggled (bool) ),
	   this, SLOT(syncAtomActs() ));
  atomMenu->addAction(noElli);
  togAtom->setMenu(atomMenu);

  togHBond->setShortcut(tr("H"));
  togHBond->setCheckable ( true );
  togHBond->setChecked ( true );
  togHBond->setIcon(QIcon(":/images/hbrs.png"));
  togHBond->setWhatsThis("<img src=\":/images/hbrs.png\"><p>Click here to toggle the visualization of <b>hydrogen bonds</b>."
			" If this is checked hydrogen bonds are visualized as stipled yellow sticks.");

  QAction *sma =new QAction("show mat",this);
  sma->setShortcut(tr("~"));
  connect(sma,SIGNAL(triggered()),cubeGL,SLOT(showMatrix()));
// */
  matrixAct->setShortcut(tr("="));
  matrixAct->setStatusTip("Rotates, scales and moves the molecule in the same position as on the last sceen shot taken.");
  QAction *viewAlong010= new QAction("View along a",this) ;
  QAction *viewAlong100= new QAction("View along b",this) ;
  QAction *viewAlong001= new QAction("View along c",this) ;

  editAtomAct->setIcon(QIcon(":images/eac.png"));
  editAtomAct->setShortcut(tr("Shift+A"));
  editAtomAct->setWhatsThis("<img src=\":images/eac.png\"><p>Here you can change the labeling, ellipsoid style, color and kovalent radius of a type of atoms.");
  editAtomAct->setStatusTip("Change the labeling, ellipsoid style, color and kovalent radius of a type of atoms.");

  backColor->setShortcut(tr("Shift+B"));
  labelColor->setShortcut(tr("Shift+T"));
  labelFont->setShortcut(tr("Shift+F"));
  scalePicAct->setShortcut(tr("Ctrl+S"));
createRenameWgd();
  connect(viewAlong100,SIGNAL(triggered ()),
		  cubeGL,SLOT(along100()));
  connect(viewAlong010,SIGNAL(triggered ()),
		  cubeGL,SLOT(along010()));
  connect(viewAlong001,SIGNAL(triggered ()),
		  cubeGL,SLOT(along001()));
  connect(filterAct, SIGNAL (triggered ()),
	  this, SLOT(sortFilterDlg()));

  connect(editAtomAct, SIGNAL (triggered ()),
	  this, SLOT(editAtomColor()));
  connect(unfilterAct, SIGNAL (triggered ()),
	  this, SLOT(unFilter()));
  connect(scalePicAct, SIGNAL (triggered ()),
	  this, SLOT(changeScalePic()));
  connect( zoomInAct, SIGNAL(triggered()),
	   this, SLOT(zoomIn() ) );
  connect( zoomOutAct, SIGNAL(triggered()),
	   this, SLOT(zoomOut() ) );
  connect( rotLeftAct, SIGNAL(triggered()),
	   this, SLOT(rotLeft() ) );
  connect( rotRightAct, SIGNAL(triggered()),
	   this, SLOT(rotRight() ) );
  connect( rotUpAct, SIGNAL(triggered()),
	   this, SLOT(rotUp() ) );
  connect( rotDownAct, SIGNAL(triggered()),
	   this, SLOT(rotDown() ) );
  connect( fontSizeUpAct, SIGNAL(triggered()),
	   cubeGL, SLOT(fontSizeUp() ) );
  connect( fontSizeDownAct, SIGNAL(triggered()),
	   cubeGL, SLOT(fontSizeDown() ) );
  connect( atomClickAct, SIGNAL(toggled(bool)),
	   cubeGL, SLOT(setAtomsClickable(bool)));
  connect( back_Grad, SIGNAL(toggled (bool) ),
	   cubeGL, SLOT(togglBGG(bool ) ));
  connect( donTGrow, SIGNAL(toggled (bool) ),
	   cubeGL, SLOT(togglGrow(bool ) ));
  connect( mveLeftAct, SIGNAL(triggered()),
	   this, SLOT(moveL()));
  connect( mveRightAct, SIGNAL(triggered () ),
	   this, SLOT(moveR()  ));
  connect( mveUpAct, SIGNAL(triggered () ),
	   this, SLOT(moveU()  ));
  connect( mveDownAct, SIGNAL(triggered () ),
	   this, SLOT(moveD()  ));
  connect( togAtom, SIGNAL(toggled (bool) ),
	   cubeGL, SLOT(setAtom(bool) ));
  connect( togBond, SIGNAL(toggled (bool) ),
	   cubeGL, SLOT(setBond(bool) ));
  connect( togLabel, SIGNAL(toggled(bool) ),
	   cubeGL, SLOT(setLabels(bool) ));
  connect(ydlStartAct,SIGNAL(triggered()),
	  this,SLOT(idleMoveStart()));
  connect(ydlStopAct,SIGNAL(triggered()),
	  this,SLOT(idleMoveStop()));
  connect( togAxen, SIGNAL(toggled(bool)),
	   cubeGL, SLOT(setAxes(bool)));
  connect( togUnit, SIGNAL(toggled (bool) ),
	   cubeGL, SLOT(setUnitCell(bool) ));
  //connect( togLuft, SIGNAL(toggled(bool) ),
    //   cubeGL, SLOT(setHelices(bool) ));
  connect( togElli, SIGNAL(toggled(bool)),
	   cubeGL, SLOT(setEllipsoid(bool)));
  connect( togElli, SIGNAL(toggled(bool)),
	   this, SLOT(toggleNoElli(bool)));
  connect( movLabAct, SIGNAL(toggled(bool)),
	   cubeGL, SLOT(setLabelsMovable(bool)));
  connect( homeLabAct, SIGNAL(triggered () ),
	   cubeGL, SLOT(homeLabels() ));
  connect( togHBond, SIGNAL(toggled(bool)),
	   cubeGL, SLOT(setHydrogenBond(bool)));
  connect(backColor, SIGNAL (triggered ()),
	  cubeGL, SLOT(changeBColor()));
  connect(labelColor, SIGNAL (triggered ()),
	  cubeGL, SLOT(changeTColor()));
  connect(labelFont, SIGNAL (triggered ()),
	  cubeGL, SLOT(setFont()));
  connect(changeProbAct,SIGNAL(triggered()),
	  this,SLOT(changeADP()));
  connect(togStat,SIGNAL(triggered()),
	  this,SLOT(togglSt()));
  connect(wasistdasAct,SIGNAL(triggered()),
	  this,SLOT(wasistdas()));
//  connect(printAct,SIGNAL(triggered()),
//	  this,SLOT(printScene()));
  connect(matrixAct, SIGNAL (triggered ()),
	  cubeGL, SLOT(setMatrix()));
  connect(invExpAct, SIGNAL (triggered ()),
	  cubeGL, SLOT(invariomExport()));
  connect(invEdiAct, SIGNAL (toggled(bool)),
	  cubeGL, SLOT(toggInvEdit(bool)));
  connect(invLoadAct, SIGNAL (triggered ()),
	  cubeGL, SLOT(loadDataBase()));
  connect(cubeGL, SIGNAL (reloadXD ()),
	  this, SLOT(reloadXDfiles()));
  connect(cubeGL, SIGNAL (reloadFile()),
	  this, SLOT(reloadFiles()));
  workMenu->addAction(zoomInAct  );
  workMenu->addAction(zoomOutAct );
  workMenu->addAction(rotLeftAct );
  workMenu->addAction(rotRightAct);
  workMenu->addAction(rotUpAct );
  workMenu->addAction(rotDownAct);
  workMenu->addAction("rotate clockwise",this,SLOT(rotCw()));
  workMenu->addAction("rotate counterclockwise",this,SLOT(rotCcw()));
  workMenu->addAction(mveLeftAct );
  workMenu->addAction(mveRightAct);
  workMenu->addAction(mveUpAct   );
  workMenu->addAction(mveDownAct );
  workMenu->addAction(ydlStartAct);
  workMenu->addAction(ydlStopAct);  
  workMenu->addAction("Make a rotation-movie file",this,SLOT(makeRotMovi()));
  
  tMovieStartAct=ModulationMenu->addAction("Start t movie",this ,SLOT(tMovieStart())); 
  tMovieStopAct=ModulationMenu->addAction("Stop t movie",this,SLOT(tMovieStop()));  
  tMovieStopAct->setShortcut(tr("t"));
  tMovieStartAct->setShortcut(tr("t"));
  tMovieStopAct->setEnabled(false);
  tMovieStartAct->setEnabled(false);
  {
  QAction *a=exportShelxAtTvalueAct=ModulationMenu->addAction("Export Shelx.res file at current t0 value",this,SLOT(exportShelxAtTvalue()));  
  a->setShortcut(tr("Alt+t"));
  a->setEnabled(false);
  }
  ModulationMenu->addAction("create a t-movie mp4 file",this,SLOT(makeTMovi()));
  cubeGL->lissajous=ModulationMenu->addAction("Show Lissajous traces",cubeGL,SLOT(updateGL()));
  cubeGL->lissajous->setCheckable(true);
  cubeGL->lissajous->setChecked(false);
  ModulationMenu->addAction("Change Lissajous-trace-thicknes",this,SLOT(lisisfat()));
  cubeGL->lissajousGay=ModulationMenu->addAction("rainbow color map t-value on Lissajous traces",cubeGL,SLOT(updateGL()));
  cubeGL->lissajousGay->setCheckable(true);
  cubeGL->lissajousGay->setChecked(false);
  cubeGL->lissaADP=ModulationMenu->addAction("ADP axes on Lissajous traces",cubeGL,SLOT(updateGL()));
  cubeGL->lissaADP->setCheckable(true);
  cubeGL->lissaADP->setChecked(true);

  workMenu->addAction(fontSizeUpAct);
  workMenu->addAction(fontSizeDownAct);
  workMenu->addAction(matrixAct);
  workMenu->addAction(viewAlong010);
  workMenu->addAction(viewAlong100);
  workMenu->addAction(viewAlong001);
  workMenu->addAction(sma);
  workMenu->setTearOffEnabled(true);
  viewMenu->addAction(atomClickAct);
  viewMenu->addAction(togAtom);
  viewMenu->addAction(togBond);
  viewMenu->addAction(togLabel);
  viewMenu->addAction(togAxen);
  viewMenu->addAction(togUnit);
  viewMenu->addAction(togElli);
  {
    QAction *a=
  viewMenu->addAction("Create voronoi polyeders of asymetric unit",this,SLOT(makeVoro()));
    a->setData(-1);
  }
  cubeGL->showPolys=viewMenu->addAction(QIcon(":/images/poly.png"),"toggle Polyeder",cubeGL,SLOT(updateGL()));
  cubeGL->showPolys->setCheckable(true);
  cubeGL->showPolys->setChecked(false);
  cubeGL->showPolys->setMenu(sfacMenu);


  viewMenu->addAction(togHBond);
//  viewMenu->addAction(togLuft);
  viewMenu->addAction(depthCueingAct);
  viewMenu->addSeparator();
  viewMenu->addAction(dock->toggleViewAction());
  viewMenu->addAction(togStat);
  viewMenu->addAction(matrixAct);
  viewMenu->addAction(viewAlong010);
  viewMenu->addAction(viewAlong100);
  viewMenu->addAction(viewAlong001);
  noMessBox=new QAction("Don't show warning messages",this);
  dialogMenu->addAction(noMessBox);
  noMessBox->setCheckable(true);
  noMessBox->setChecked(true);
  dialogMenu->addAction(backColor);
  dialogMenu->addAction(back_Grad);
  dialogMenu->addAction(donTGrow);
  dialogMenu->addAction(labelColor);
  dialogMenu->addAction(labelFont);
  dialogMenu->addAction(scalePicAct);
  dialogMenu->addAction("Change perspective view angle",this,SLOT(changeViewAngle()));
  dialogMenu->addAction(changeProbAct);
  dialogMenu->addAction(editAtomAct);
  dialogMenu->addAction(filterAct);
  dialogMenu->addAction(unfilterAct);
  QAction *mapAct=dialogMenu->addAction(QIcon(":/images/mapcontrol.png"),"Open map control",this,SLOT(openMapControl()));
  dialogMenu->addSeparator();
  dialogMenu->addAction("Save current toggle states",cubeGL,SLOT(saveMISettings()));
  dialogMenu->addAction("Load toggle states",cubeGL,SLOT(loadMISettings()));
  dialogMenu->addAction(cubeGL->chicken);
  dialogMenu->addAction(cubeGL->quickRot);
  cubeGL->Istda=invariomMenu->addAction("No Database loaded yet");
  invariomMenu->addAction(invExpAct);
  invariomMenu->addAction(invEdiAct);
  invariomMenu->addAction(invLoadAct);
  invariomMenu->addAction("Load latest (D95++3df3pd) Invariom-Data-Base from internet",this , SLOT(howOldIsTheLatesDataBaseI()));
  invariomMenu->addAction("Load latest (def2TZVP) Invariom-Data-Base from internet",this , SLOT(howOldIsTheLatesDataBaseII()));
    
  (void*) statusBar ();
  sLabel = new QLabel;
  sLabel->setFrameStyle(QFrame::Panel  );
  sLabel->setLineWidth(2);
  sLabel->setText("MolecoolQt a viewing program for molecular structures");

  balken = new QProgressBar(this);
  balken->hide();
  statusBar()->addPermanentWidget(balken);
  modScale = new QDoubleSpinBox(this);
  modScale->setMaximum(50.0);
  modScale->setMinimum(0.01);
  modScale->setValue(1.0);
  modScale->setSingleStep(0.2);
  modScale->setPrefix("modulation scale");
  modScale->setMinimumWidth(190);
  modScale->setSuffix(" ");
  modScale->setDecimals(2);
  modScale->hide();
  connect(modScale,SIGNAL(valueChanged(double )),this ,SLOT(changeModScal(double)));
  statusBar()->addPermanentWidget(speedSldr);
  statusBar()->addPermanentWidget(modScale);
  statusBar()->addPermanentWidget(sLabel);
  statusBar()->addPermanentWidget(cubeGL->enviButt);

  
  statusBar()->setWhatsThis("This is the status bar. You can hide it in the View menu by unchecking 'toggle Status bar'.");
  
  connect(indexSpin,SIGNAL(valueChanged(int )),this,SLOT(changeIndexofLabel(int ))); 
  phaseSpin =  new QSpinBox();
  phaseSpin->setMinimum(1);
  phaseSpin->setMaximum(1);
  phaseSpin->setPrefix("Phase #") ;
  phaseSpin->setValue(curentPhase);
  phaseSpin->hide();
  connect(phaseSpin,SIGNAL(valueChanged(int )),this,SLOT(currentPhaseChanged(int )));
  statusBar()->addPermanentWidget(phaseSpin);
  statusBar()->addPermanentWidget(homeMe);
  /*time = new QLCDNumber;
  time->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  time->setLineWidth(1);
  time->setMinimumSize(200,33); 
  time->setSegmentStyle(QLCDNumber::Flat);
  statusBar()->addPermanentWidget(time);
  QTimer *timer = new QTimer(this);
  connect( timer, SIGNAL(timeout()),
	   this, SLOT(updateTime()) );
    // Jede Sekunde updateTime() aufrufen
  timer->start(1000);
  updateTime();*/
  
  QAction *a;
  QToolBar *tb= new QToolBar("Atom Selection Tools",this);
  tb->setIconSize(QSize(23,23));

          searchBar = new QToolBar(tr("Search"),this);
          addToolBar(Qt::BottomToolBarArea,searchBar);
          searchBar->setIconSize(QSize(23,23));
          searchBar->setMovable(true);
          searchAtomEdit = new QLineEdit(this);
          searchAtomEdit->setMaxLength(20);
          searchAtomCancel = searchBar->addAction(QIcon(":/images/cancel.png"),"hide Search", searchBar , SLOT(hide()));

          searchBar->addWidget(searchAtomEdit);
          searchBar->addAction(QIcon(":/images/search.png"),"search",this,SLOT(findAtoms()));
          connect(searchAtomEdit,SIGNAL(returnPressed ()),this,SLOT(findAtoms()));

          searchAtomButton=tb->addAction(QIcon(":/images/search.png"),"Search",searchBar,SLOT(show()));
          searchAtomButton->setShortcut( QKeySequence(tr("Ctrl+F")));
          connect(searchAtomButton,SIGNAL(triggered()),searchAtomEdit,SLOT(setFocus()));

          searchBar->hide();
          connect(searchBar,SIGNAL(visibilityChanged(bool )),this,SLOT(hideSearchAtomButton(bool )));

  a=hFilter=tb->addAction("Hide hydrogen atoms",this, SLOT(filterHydrogens(bool) ));
  hFilter->setCheckable ( true );
  hFilter->setChecked (  true );
  hFilter->setToolTip("Filter hydrogens out");
  a->setVisible(false);

  a=cubeGL->rCenter =tb->addAction("Rotation Center",cubeGL,SLOT( rotCenter()));
  a->setVisible(false);
  a=dFilter=tb->addAction("Hide non atoms",this, SLOT(filterNonAtoms(bool) ));
  a->setCheckable ( true );
  a->setChecked (  true );
  a->setVisible(false);
  tb->addAction(atomClickAct);
  a=ignorant=tb->addAction(QIcon(":/images/clickatoms.png"),"Hover highlighting",cubeGL, SLOT(togglMOAI(bool) ));
  ignorant->setCheckable ( true );
  ignorant->setChecked (  true );
  ignorant->setStatusTip("Toggle mouse-over-atom identification"); 
 
  cubeGL->invertMouseZoom = new QCheckBox("invert mouse zoom");
 
  cubeGL->minus = new QCheckBox("switch eyes");
  cubeGL->minus->hide();
  cubeGL->minus->setShortcut(tr("W"));
  cubeGL->minus->setChecked(true);
  connect(cubeGL->minus,SIGNAL(stateChanged(int)),cubeGL,SLOT(updateGL()));
  tb->addWidget(cubeGL->minus);
  tb->addWidget(cubeGL->invertMouseZoom);
  tb->setAllowedAreas(Qt::BottomToolBarArea);
  tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  cubeGL->addBond=a=tb->addAction("connect selected atoms",cubeGL,SLOT(connectSelection()));
  a->setVisible(false);
  cubeGL->killBond=a=tb->addAction("disconnect selected atoms",cubeGL,SLOT(disConnectSelection()));
  a->setVisible(false);
  cubeGL->addCoordi=a=tb->addAction("coordinative bond",cubeGL,SLOT(coordinativeBonds()));
  a->setVisible(false);
  cubeGL->delCoordi=a=tb->addAction("remove all coordinative bonds",cubeGL,SLOT(delCoordinative()));
  a->setVisible(false);
  cubeGL->clearSelection=a=tb->addAction("Deselect atoms",cubeGL,SLOT(disSelection()));
  a->setVisible(false);
  cubeGL->hidSelected=a=tb->addAction("[hide selected]",cubeGL,SLOT(hidSelection()));
  a->setVisible(false);
  cubeGL->hidUnSelected=a=tb->addAction("]hide not selected[",cubeGL,SLOT(hidUnSelection()));
  a->setVisible(false);
  cubeGL->showall=a=tb->addAction("show hidden",cubeGL,SLOT(unHide()));
  a->setVisible(false);
  cubeGL->centerSelection=a=tb->addAction("Center selected atoms",cubeGL,SLOT(updateGL()));
  a->setCheckable(true);
  a->setChecked(false);
  a->setVisible(false);
  addToolBar(Qt::BottomToolBarArea,tb);
      
  toolFile = addToolBar(tr("File"));
  toolFile->setIconSize(QSize(23,23));
  toolFile->setAllowedAreas(Qt::AllToolBarAreas );
  toolFile->addAction(act2);
  toolFile->addAction(createmoliso);
  toolFile->addAction(noMoliso);
  toolFile->addAction(act1);
 // toolFile->addAction(printAct);
  toolFile->addAction(wasistdasAct);
  toolFile->addAction(act3);
  toolFile->addAction(ldipAct);
  toolFile->addAction(nodipAct);
  toolFile->addAction(cntrPlot);

  toolFile->setWhatsThis("This is the file tool bar. You can move it to any window border if you want."
			 " By clicking right on the menu or tool bar region, you can toggel the tool bars.");

  toolSettings = addToolBar(tr("Settings"));
  toolSettings->setIconSize(QSize(23,23));
  toolSettings->setMovable(true);
  toolSettings->addAction(editAtomAct);
  toolSettings->addAction(mapAct);
  toolSettings->addAction(filterAct);
  toolSettings->addAction(unfilterAct);
  toolSettings->addAction(xdSetupAct);
  toolSettings->addAction(invEdiAct);
  toolSettings->addAction(invExpAct);
  toolSettings->addAction(movLabAct);
  toolSettings->addAction(homeLabAct);
  toolSettings->addAction(seReAct);
  toolSettings->addAction(movLeg);
  toolSettings->addAction(mildir);
//  toolSettings->addAction(packAct);

  toolMove = addToolBar(tr("Move"));
  toolMove->setIconSize(QSize(23,23));
  toolMove->setMovable(true);
  toolMove->addAction(zoomInAct);
  toolMove->addAction(zoomOutAct);
  toolMove->addAction(rotLeftAct);
  toolMove->addAction(rotRightAct);
  toolMove->addAction(rotUpAct);
  toolMove->addAction(rotDownAct);
  toolMove->addAction(mveLeftAct ); 
  toolMove->addAction(mveRightAct); 
  toolMove->addAction(mveUpAct   ); 
  toolMove->addAction(mveDownAct ); 
  toolMove->addAction(ydlStartAct); 
  toolMove->addAction(ydlStopAct);  
  toolMove->addAction(fontSizeUpAct);
  toolMove->addAction(fontSizeDownAct);
  toolMove->setWhatsThis("This is the <b>move tool bar</b>. You can move it to any window border if you want."
			 " By clicking right on the menu or tool bar region, you can toggel the tool bars."
			 "<table width=100% border=1 cellspacing=0 cellpadding=2>"
 "<tr><th>Icon</th><th>Short Cut</th><th>Function</th><th></tr>"
 "<tr><td><img src=\":/images/zoomin.png\"</td><td>Page Up</td><td>Zoom In</td><td>Enlarge the structure.</td></tr>"
 "<tr><td><img src=\":/images/zoomout.png\"</td><td>Page Down</td><td>Zoom Out</td><td>Minimize the stucture.</td></tr>"
 "<tr><td><img src=\":/images/rotateleft.png\"</td><td>Left</td><td>Rotate Left</td><td>Rotates the structure clockwise around the Y-axis.</td></tr>"
 "<tr><td><img src=\":/images/rotateright.png\"</td><td>Right</td><td>Rotate Right</td><td>Rotates the structure counterclockwise around the Y-axis.</td></tr>"
 "<tr><td><img src=\":/images/rotateup.png\"</td><td>Up</td><td>Rotate Up</td><td>Rotates the structure clockwise around the X-axis.</td></tr>"
 "<tr><td><img src=\":/images/rotatedown.png\"</td><td>Down</td><td>Rotate Down</td><td>Rotates the structure counterclockwise around the X-axis</td></tr>"
 "<tr><td><img src=\":/images/moveleft.png\"></td>   <td>Ctrl+Left</td> <td>Move Left</td>          <td></td></tr> "
 "<tr><td><img src=\":/images/moveright.png\"></td>  <td>Ctrl+Right</td><td>Move Right</td>         <td></td></tr> "
 "<tr><td><img src=\":/images/moveup.png\"></td>     <td>Ctrl+Up</td>   <td>Move Up</td>            <td></td></tr> "
 "<tr><td><img src=\":/images/movedown.png\"></td>   <td>Crtl+Down</td> <td>Move Down</td>          <td></td></tr> "
 "<tr><td><img src=\":/images/everrotateleft.png\"</td><td>Shift+Y</td><td>Start Rotation</td><td>Starts to rotate the structure clockwise around the Y-axis.</td></tr> "
 "<tr><td><img src=\":/images/neverrotateleft.png\"</td><td>Space</td><td>Stop Rotation</td><td>Stops to rotate the structure clockwise around the Y-axis.</td></tr> "
 "<tr><td><img src=\":/images/fontsizeup.png\"</td>  <td>+</td>         <td>Increase Label Size</td><td></td></tr> "
 "<tr><td><img src=\":/images/fontsizedown.png\"</td><td>-</td>         <td>Decrease Label Size</td><td></td></tr> "
 "</table>" );
  toolMove->hide();
  toolView=addToolBar(tr("View"));
  toolView->setIconSize(QSize(23,23));
  toolView->setMovable(true);
  toolView->addAction(togAtom);
  toolView->addAction(togElli);
  toolView->addAction(togBond);
  toolView->addAction(togLabel);
  toolView->addAction(togAxen);
  toolView->addAction(togUnit);
  toolView->addAction(cubeGL->showPolys);
  toolView->addAction(togHBond);
  toolView->addAction(showface);
  toolView->addAction(showLeg);

  fmcq = new FourMCQ(&mol,cubeGL,toolView);
  /////////////
  {
  QDialogButtonBox *buttonBoxMC = new QDialogButtonBox( QDialogButtonBox::Apply | QDialogButtonBox::Cancel);
  QPushButton *applyMC = buttonBoxMC->button(QDialogButtonBox::Apply);
  QPushButton *jnkButton=buttonBoxMC->addButton("fractal dim. analysis",QDialogButtonBox::ApplyRole);
  QLabel *wl= new QLabel("Factor to downweight weak data");
  QLabel *pl= new QLabel("Map precision: ");
  QLabel *dl= new QLabel("Fo-Fc map: ");
  QLabel *d12= new QLabel("F1-F2 map: ");
  QLabel *ol= new QLabel("F-observed map: ");
  QLabel *rl= new QLabel("Map radius: ");
  QLabel *sl= new QLabel("Map truncation type: ");
  QLabel *lw= new QLabel("Line width: ");
  QLabel *lt= new QLabel("Line transparency: ");
  md = new QDialog(this);
  md->setWindowTitle("Map Control");
  connect(jnkButton, SIGNAL(clicked()),this, SLOT(jnk()));
  connect(applyMC , SIGNAL(clicked()), this , SLOT(controlMap()));
  connect(applyMC , SIGNAL(clicked()), this , SLOT(openMapControl()));
  connect(buttonBoxMC, SIGNAL(rejected()), md, SLOT(hide()));

  mapprec = new QDoubleSpinBox();
  mapprec->setMinimum(1.1);
  mapprec->setMaximum(8.0);
  mapprec->setValue(fmcq->rr);
  mapprec->setSingleStep(0.1);

  lineTrans = new QDoubleSpinBox();
  lineTrans->setMinimum(0.1);
  lineTrans->setMaximum(1.0);
  lineTrans->setValue(fmcq->lintrans);
  lineTrans->setSingleStep(0.1);

  lineWidth = new QDoubleSpinBox();
  lineWidth->setMinimum(0.5);
  lineWidth->setMaximum(4.0);
  lineWidth->setValue(fmcq->linwidth);
  lineWidth->setSingleStep(0.1);

  weak = new QDoubleSpinBox();
  weak->setMinimum(0.1);
  weak->setMaximum(8.0);
  weak->setValue(fmcq->rw);
  weak->setSingleStep(0.1);

  difmaps = new QDoubleSpinBox();
  difmaps->setDecimals(4);
  difmaps->setMinimum(0);
  difmaps->setMaximum(94);
  difmaps->setValue(fabs(fmcq->iso[1]));
  difmaps->setSingleStep(fabs(fmcq->iso[1])/10.0); 
  difmaps->setSuffix("e/A^3");

  fomaps = new QDoubleSpinBox();
  fomaps->setMinimum(0);
  fomaps->setMaximum(94);
  fomaps->setValue(fmcq->iso[0]);
  fomaps->setSingleStep(fmcq->iso[0]/10.0);
  fomaps->setSuffix("e/A^3");
  
  f12maps = new QDoubleSpinBox();
  f12maps->setDecimals(4);
  f12maps->setMinimum(0);
  f12maps->setMaximum(94);
  f12maps->setValue(fabs(fmcq->iso[2]));
  f12maps->setSingleStep(fabs(fmcq->iso[2])/10.0);
  f12maps->setSuffix("e/A^3");

  maprad = new QDoubleSpinBox();
  maprad->setValue(fmcq->map_radius);
  maprad->setMinimum(0);
  maprad->setMaximum(25.0);
  maprad->setSuffix("A");
  maprad->setSingleStep(0.3);
  mapSchnitt=  new QComboBox();
  mapSchnitt->addItem("one complete unit cell",0);
  mapSchnitt->addItem("sphere around rotation center",1);
  mapSchnitt->addItem("1.41A around visible atoms or peaks",2);
  mapSchnitt->setCurrentIndex(fmcq->maptrunc);
  QGridLayout *mdl = new QGridLayout();
  mdl->addWidget(mapprec,0,1);
  mdl->addWidget(weak,1,1);
  mdl->addWidget(difmaps,2,1);
  mdl->addWidget(fomaps,3,1);
  mdl->addWidget(f12maps,4,1);
  mdl->addWidget(maprad,5,1);
  mdl->addWidget(mapSchnitt,6,1);
  mdl->addWidget(lineTrans,7,1);
  mdl->addWidget(lineWidth,8,1);
  mdl->addWidget(pl,0,0);
  mdl->addWidget(wl,1,0);
  mdl->addWidget(dl,2,0);
  mdl->addWidget(ol,3,0);
  mdl->addWidget(d12,4,0);
  mdl->addWidget(rl,5,0);
  mdl->addWidget(sl,6,0);
  mdl->addWidget(lt,7,0);
  mdl->addWidget(lw,8,0);
  mdl->addWidget(buttonBoxMC,10,0,3,3);
  md->setLayout(mdl);
  md->setModal (false);
  fmcq->doMaps = new QCheckBox("Calculate Maps");
  fmcq->doMaps->setChecked(true);
  connect(fmcq->doMaps,SIGNAL(toggled(bool)),this , SLOT(doMapsNow(bool)));
//  fmcq->doMaps->hide();
  tb->addWidget(fmcq->doMaps);
  md->hide();
  }
  /////////////
  chargeGroups =  new QComboBox(this);
  connect(chargeGroups,SIGNAL(currentIndexChanged ( int )), this, SLOT(filterGroup(int)));
  chargeGroups->addItem("Charge Group 1 (all)",0);
  toolView->addWidget(chargeGroups);
  chargeGroups->setVisible(false);
  chargeGroups->setEnabled(false);
  toolView->setWhatsThis("This is the view tool bar. You can move it to any window border if you want."
                         " By clicking right on the menu or tool bar region, you can toggel the toolbars.");

  noMessBox->setChecked(mol.einstellung->value("NoMessageBoxesPopup",true).toBool());
  dirName = mol.einstellung->value("lastFile").toString();

  QStringList files = mol.einstellung->value("recentFileList").toStringList();
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MyWindow *mainWin = qobject_cast<MyWindow *>(widget);
    if (mainWin)
      mainWin->updateRecentFileActions();
  }
  QSize size =
    mol.einstellung->value("size", sizeHint()).toSize();
  QPoint pos =
    mol.einstellung->value("position").toPoint();
  tubiAct->setChecked(mol.tubifiedAtoms);
  singleColorAct->setChecked(mol.bondColorStyle);

  cubeGL->tCR= (float) mol.einstellung->value("text_color_red"  ).toDouble();
  cubeGL->tCG= (float) mol.einstellung->value("text_color_green").toDouble();
  cubeGL->tCB= (float) mol.einstellung->value("text_color_blue" ).toDouble();
  cubeGL->tCA= (float) mol.einstellung->value("text_color_alpha").toDouble();
  cubeGL->bgCR=(float) mol.einstellung->value("background_color_red"  ).toDouble();
  cubeGL->bgCG=(float) mol.einstellung->value("background_color_green").toDouble();
  cubeGL->bgCB=(float) mol.einstellung->value("background_color_blue" ).toDouble();
  cubeGL->bgCA=(float) mol.einstellung->value("background_color_alpha").toDouble();
  cubeGL->checkTC();
/*
  cubeGL-> MM[0]= (float)  mol.einstellung->value("Matrix00").toDouble();
  cubeGL-> MM[1]= (float)  mol.einstellung->value("Matrix01").toDouble();
  cubeGL-> MM[2]= (float)  mol.einstellung->value("Matrix02").toDouble();
  cubeGL-> MM[3]= (float)  mol.einstellung->value("Matrix03").toDouble();
  cubeGL-> MM[4]= (float)  mol.einstellung->value("Matrix04").toDouble();
  cubeGL-> MM[5]= (float)  mol.einstellung->value("Matrix05").toDouble();
  cubeGL-> MM[6]= (float)  mol.einstellung->value("Matrix06").toDouble();
  cubeGL-> MM[7]= (float)  mol.einstellung->value("Matrix07").toDouble();
  cubeGL-> MM[8]= (float)  mol.einstellung->value("Matrix08").toDouble();
  cubeGL-> MM[9]= (float)  mol.einstellung->value("Matrix09").toDouble();
  cubeGL->MM[10]= (float)  mol.einstellung->value("Matrix10").toDouble();
  cubeGL->MM[11]= (float)  mol.einstellung->value("Matrix11").toDouble();
  cubeGL->MM[12]= (float)  mol.einstellung->value("Matrix12").toDouble();
  cubeGL->MM[13]= (float)  mol.einstellung->value("Matrix13").toDouble();
  cubeGL->MM[14]= (float)  mol.einstellung->value("Matrix14").toDouble();
  cubeGL->MM[15]= (float)  mol.einstellung->value("Matrix15").toDouble();*/
  QVariant variant;
  if (mol.einstellung->contains("LabelFont")){
    variant=mol.einstellung->value("LabelFont");
    cubeGL->myFont=variant.value<QFont>();}
  else{
    cubeGL->myFont=QFont("Arial",24, -1, false);
  }
  if (mol.einstellung->contains("MolisoLegendFont")){
    variant=mol.einstellung->value("MolisoLegendFont");
    cubeGL->MLegendFont=variant.value<QFont>();}
  else{
    cubeGL->MLegendFont=QFont("Arial",14, -1, false);
  }
  float mtest=
	  cubeGL->MM[0]+
	  cubeGL->MM[1]+
	  cubeGL->MM[2]+
	  cubeGL->MM[3]+
	  cubeGL->MM[4]+
	  cubeGL->MM[6]+
	  cubeGL->MM[7]+
	  cubeGL->MM[8]+
	  cubeGL->MM[9]+
	  cubeGL->MM[10]+
	  cubeGL->MM[11]+
	  cubeGL->MM[12]+
	  cubeGL->MM[13]+
	  cubeGL->MM[14]+
	  cubeGL->MM[15];
  if (mtest==0){
   cubeGL->MM[0]=1.0;
   cubeGL->MM[1]=0.0;
   cubeGL->MM[2]=0.0;
   cubeGL->MM[3]=0.0;
   cubeGL->MM[4]=0.0;
   cubeGL->MM[5]=1.0;
   cubeGL->MM[6]=0.0;
   cubeGL->MM[7]=0.0;
   cubeGL->MM[8]=0.0;
   cubeGL->MM[9]=0.0;
   cubeGL->MM[10]=1.0;
   cubeGL->MM[11]=0.0;
   cubeGL->MM[12]=0.0;
   cubeGL->MM[13]=0.0;
   cubeGL->MM[14]=-200.0;
   cubeGL->MM[15]=1.0;
  }
  cubeGL->invertMouseZoom->setChecked( mol.einstellung->value("InvertMouseZoom").toBool());
  bool b;
  if (mol.einstellung->contains("BackgroundGradient")){
    back_Grad->setChecked(b=mol.einstellung->value("BackgroundGradient").toBool());
    cubeGL->togglBGG(b);
  }
  if (mol.einstellung->contains("UnitCell")){
    togUnit->setChecked(b=mol.einstellung->value("UnitCell").toBool());
    cubeGL->setUnitCell(b);
  }
  mol.einstellung->endGroup();
  move( pos);
  isomax=isomin=666.666;
  if( size.isNull() )
    resize(640, 480);
  else
    resize(size);
  setCentralWidget(cubeGL);
  cubeGL->setWhatsThis("This the <b>OpenGL</b> window. The visualized structure can be <b>rotated</b> by holding down the <b>left mouse button</b> or "
		       "by clicking on one of these buttons:<br> "
		       "<img src=\":/images/rotateleft.png\"> "
		       "<img src=\":/images/rotateright.png\"> "
		       "<img src=\":/images/rotateup.png\"> "
		       "<img src=\":/images/rotatedown.png\">. "
		       "<p>It can be <b>moved</b> by holding down the <b>middle mouse button</b> or "
		       "by clicking on one of these buttons:<br> "
		       "<img src=\":/images/moveleft.png\"> "
		       "<img src=\":/images/moveright.png\"> "
		       "<img src=\":/images/moveup.png\"> "
		       "<img src=\":/images/movedown.png\">. "
		       "<p>The structure can be <b>scaled</b> by holding down the "
		       "<b>right mouse button</b> and moving up and down or "
		       "by clicking on one of these buttons:<br> "
		       "<img src=\":/images/zoomin.png\"> "
		       "<img src=\":/images/zoomout.png\">. "
		       "<p>The Label size could be altered by using the <b> mouse wheel</b> or "
		       "by clicking on one of these buttons:<br> "
		       "<img src=\":/images/fontsizeup.png\"> "
		       "<img src=\":/images/fontsizedown.png\"> "
		       "<p> Moving the mouse over an atom will show its name in the status bar.");
  connect(cubeGL,SIGNAL(message(const QString&)),this,SLOT(updateStatusBar(const QString&)));
  connect(cubeGL,SIGNAL(bigmessage(const QString&)),this,SLOT(infoKanalNews(const QString&)));
  connect(fmcq,SIGNAL(bigmessage(const QString&)),this,SLOT(infoKanalNews(const QString&)));
#ifdef __x86_64__
  QString bitvers=QString("64 bit version");
#else
  QString bitvers=QString("32 bit version");
#endif

  setWindowTitle(QString("MoleCoolQt-Revision %1 %2").arg(rev).arg(bitvers));
  fastrun=false;
  int argc=QCoreApplication::arguments().size();
  if (argc>1){
    cubeGL->updateGL();
    QString fnam="";
    QString isonam="";
    QString mapnam="";
    int argui=1;
    double gd=-1;
    for (int i=argui;i<argc;i++){
        if ((isonam.isEmpty())&&((QCoreApplication::arguments().at(i).contains(".grd")) ||
          (QCoreApplication::arguments().at(i).contains(".cube")) ||
          (QCoreApplication::arguments().at(i).contains(".raw")) ||
          (QCoreApplication::arguments().at(i).contains(".m81")))) {isonam=QCoreApplication::arguments().at(i);}
        if ((!isonam.isEmpty())&&((QCoreApplication::arguments().at(i).contains(".grd")) ||
          (QCoreApplication::arguments().at(i).contains(".cube")) ||
          (QCoreApplication::arguments().at(i).contains(".raw")) ||
          (QCoreApplication::arguments().at(i).contains(".m81")))) {mapnam=QCoreApplication::arguments().at(i);
        if (mapnam==isonam)mapnam="";
        }

    if ((QCoreApplication::arguments().at(i).contains(".res")) ||
      (QCoreApplication::arguments().at(i).contains(".pdb")) ||
      (QCoreApplication::arguments().at(i).contains(".ent")) ||
      (QCoreApplication::arguments().at(i).contains(".mas")) ||
      (QCoreApplication::arguments().at(i).contains(".pth")) ||
      (QCoreApplication::arguments().at(i).contains(".vasp")) ||
      (QCoreApplication::arguments().at(i).contains(".BayMEM")) ||
      (QCoreApplication::arguments().at(i).contains(".inp")) ||
      (QCoreApplication::arguments().at(i).contains(QRegExp(".\\d\\d$"))) ||
      (QCoreApplication::arguments().at(i).contains(QRegExp(".m\\d\\d$"))) ||
      (QCoreApplication::arguments().at(i).contains(".ins")) ||
      (QCoreApplication::arguments().at(i).contains(".par")) ||
      (QCoreApplication::arguments().at(i).contains(".com")) ||
      (QCoreApplication::arguments().at(i).contains(".xyz")) ||
      (QCoreApplication::arguments().at(i).contains(".fchk",Qt::CaseInsensitive)) ||
      (QCoreApplication::arguments().at(i).contains(".cif")) ) fnam= QCoreApplication::arguments().at(i);
      if (QCoreApplication::arguments().at(i).contains("-GrowDist")) {
	i++;	
	if (i<=argc) gd=QCoreApplication::arguments().at(i).toDouble(); else gd=-1;
      }
      if (QCoreApplication::arguments().at(i).contains("-ifix")) {
        i++;
        if (i<=argc) isomin = QCoreApplication::arguments().at(i).toDouble(); else isomin = 666.666; 
        i++;
        if (i<=argc) isomax = QCoreApplication::arguments().at(i).toDouble(); else isomax = 666.666; 
      }
      if (QCoreApplication::arguments().at(i).contains("-scale")) {
        i++;
        if (i<=argc) scalePic = QCoreApplication::arguments().at(i).toDouble(); else scalePic= 1.0;
      }
      if (QCoreApplication::arguments().at(i).contains("-pt")){
        i++;
        if (i<=argc) { polifile=fopen(QCoreApplication::arguments().at(i).toStdString().c_str(),"at"); }
      }
      if ((QCoreApplication::arguments().at(i)=="-voxel")) {
        fmcq->voxelstr = QCoreApplication::arguments().at(i+1); 
       // qDebug() <<fmcq->voxelstr;
      }
      if (QCoreApplication::arguments().at(i).contains("-f")){
//	**33##
	fmcq->doMaps->setChecked(false); 
	donTGrow->setChecked(true);
	fastrun=true;
      }
      if (QCoreApplication::arguments().at(i).contains("-pdf")){
      i++;
      if (i<=argc)pdfOnAtom=QCoreApplication::arguments().at(i).toInt();else i--;
      }
      if (QCoreApplication::arguments().at(i).contains("-whatsymm")){symmsalabymm =true; }
      if (QCoreApplication::arguments().at(i).contains("-xdhkl")) {XPRTHKL=true;}
      if (QCoreApplication::arguments().at(i).contains("-HADist")) {
	i++;	
	if (i<=argc) mol.HAMax=QCoreApplication::arguments().at(i).toDouble(); 
      }
      if (QCoreApplication::arguments().at(i).contains("-HAAngle")) {
	i++;	
	if (i<=argc) mol.HAWink=QCoreApplication::arguments().at(i).toDouble();
      }
      if ((QCoreApplication::arguments().at(i)=="-viewangle")) vang = QCoreApplication::arguments().at(i+1).toDouble();
    }
    if(!fnam.isEmpty()){
    fnam.replace("\\","/");
      if (!fnam.contains("/")){
	fnam.prepend("/");
	fnam.prepend(QDir::currentPath());
      }
      if (isonam.isEmpty())loadFile(fnam,gd);
      update();

    }

    if(!mapnam.isEmpty()){
    mapnam.replace("\\","/");
      if (!mapnam.contains("/")){
    mapnam.prepend("/");
    mapnam.prepend(QDir::currentPath());
      }
    }
    if(!isonam.isEmpty()){
    isonam.replace("\\","/");
      if (!isonam.contains("/")){
    isonam.prepend("/");
    isonam.prepend(QDir::currentPath());
      }
      int chk=0;
      chk=(!mapnam.isEmpty()&1);
      chk|=(!fnam.isEmpty()&1)<<3;
      genMoliso(isonam,mapnam,"","",chk,fnam);//fnam,gd);

    }

  if (vang>0.0) cubeGL->setViewAngle(vang);
  }
  net = new QNetworkAccessManager(this);
  QString Pfad = mol.einstellung->fileName();
  Pfad=Pfad.section('/',0,-2);
  Pfad.append("/DABA.txt");
  if (QFileInfo(Pfad).exists()) cubeGL->loadDataBase(Pfad);
  Pfad=mol.einstellung->fileName(); Pfad=Pfad.section('/',0,-2); Pfad.append("/hirsh"); if (QFileInfo(Pfad).exists()) { QFile f(Pfad); bool da=false; f.open(QIODevice::ReadOnly); da=QByteArray::fromHex("0d42798c8e9015b9abcb1049c776dae8")==QCryptographicHash::hash (f.readAll(), QCryptographicHash::Md5 ); f.close(); if (da) { QMenu *hirshMenu = new QMenu("Hirshfeld"); hirshMenu->addAction("Hirshfeld", this, SLOT(openHirsh())); menuBar()->insertMenu(MIA,hirshMenu); } }

  cubeGL->setViewAngle(vang);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MyWindow::openHirsh(){
  hirshfeld = new Hirshfeld(&mol, &asymmUnit, &cubeGL->selectedAtoms, &dirName);

  connect(hirshfeld,SIGNAL(doface()), this ,SLOT(directHirsh()));
  hirshfeld->exec();
}

void MyWindow::openMapControl(){
  weak->setValue(fmcq->rw);
  mapprec->setValue(fmcq->rr);
  difmaps->setValue(fabs(fmcq->iso[1]));
  difmaps->setSingleStep(fabs(fmcq->iso[1])/10.0);
  f12maps->setValue(fabs(fmcq->iso[2]));
  f12maps->setSingleStep(fabs(fmcq->iso[2])/10.0);
  lineTrans->setValue(fmcq->lintrans);
  lineWidth->setValue(fmcq->linwidth);
  fomaps->setValue(fmcq->iso[0]);
  fomaps->setSingleStep(fmcq->iso[0]/10.0);
  maprad->setValue(fmcq->map_radius);
  if (fmcq->datfo6!=NULL) fmcq->maptrunc =0;
  mapSchnitt->setCurrentIndex(fmcq->maptrunc);
  mapSchnitt->setEnabled(fmcq->datfo6==NULL);
  md->show();
}

void MyWindow::exportShelxAtTvalue(){
 QString selectedFilter="SHELX .res file (*.res)";
 bool ok;
 double t=QInputDialog::getDouble(this, "Enter t0 value", "Enter t0 value",cubeGL->tvalue,  0.0, (mol.ccc.isEmpty())?1.0:9999.0, 3, &ok) ;
 if (!ok) t=fmod(fabs(cubeGL->tvalue),1.0);
  QString fileName = QFileDialog::getSaveFileName(this,
      QString(tr("Save a shelx.res file at t0 = %1").arg(cubeGL->tvalue)), saveName,"SHELX .res file (*.res)",&selectedFilter,QFileDialog::DontUseNativeDialog );
  if (!fileName.isEmpty()){
    QFile res(fileName);
    res.open(QIODevice::WriteOnly|QIODevice::Text);
    res.write(QString("TITL exported from:\nREM %1\nREM by MoleCoolQt at t0 = %2\n").arg(dirName).arg(t).toLatin1());
    if (mol.zelle.commensurate){
    res.write(QString("CELL %1 %2 %3 %4 %5 %6 %7\nZERR 1.0 0.0 0.0 0.0 0.0 0.0 0.0\nLATT -1\n")
        .arg(mol.zelle.lambda)
        .arg(mol.zelle.a*mol.zelle.commen.x)
        .arg(mol.zelle.b*mol.zelle.commen.y)
        .arg(mol.zelle.c*mol.zelle.commen.z)
        .arg(mol.zelle.al)
        .arg(mol.zelle.be)
        .arg(mol.zelle.ga).toLatin1());
    }else{
    res.write(QString("CELL %1 %2 %3 %4 %5 %6 %7\nZERR 1.0 0.0 0.0 0.0 0.0 0.0 0.0\nLATT -1\n")
        .arg(mol.zelle.lambda)
        .arg(mol.zelle.a)
        .arg(mol.zelle.b)
        .arg(mol.zelle.c)
        .arg(mol.zelle.al)
        .arg(mol.zelle.be)
        .arg(mol.zelle.ga).toLatin1());
    }
    QString aty,anu,s;
    QMap<int,int> sft;
    int j=1;
    for (int i=0; i<masymmUnit.size(); i++){
      s=QString("%1 ").arg(mol.pse(masymmUnit.at(i).OrdZahl));

      if (!aty.contains(s)){
      aty.append(s);
      anu.append("1 ");
      sft[masymmUnit.at(i).OrdZahl]=j++;
      }
    }
    res.write(QString("SFAC %1\nUNIT %2\nFVAR 1.0\n").arg(aty).arg(anu).toLatin1());
    Matrix u;//,uc;
    V3 p;
    double occ;
    for (int i=0; i<matoms.size(); i++){
      occ=matoms[i].occupancy(t);
      if (occ<0.1) continue;
      p=matoms[i].frac(t);
      u=matoms[i].uf(t);
      QString nam =QString(matoms[i].atomname);
      QString ps=mol.pse(matoms[i].OrdZahl);
      nam=QString("%1%2").arg(ps).arg(i+1,4-ps.length(),36,QLatin1Char('0'));
      res.write(QString("%1 %2 %3 %4 %5 %6 %7 %8 =\n    %9 %10 %11 %12\n")
          .arg(nam)
          .arg(sft.value(matoms[i].OrdZahl))
          .arg((mol.zelle.commensurate)?p.x/mol.zelle.commen.x:p.x,10,'f')
          .arg((mol.zelle.commensurate)?p.y/mol.zelle.commen.y:p.y,10,'f')
          .arg((mol.zelle.commensurate)?p.z/mol.zelle.commen.z:p.z,10,'f')
          .arg(matoms[i].amul*occ,10,'f')
          .arg(u.m11,10,'f')
          .arg(u.m22,10,'f')
          .arg(u.m33,10,'f')
          .arg(u.m23,10,'f')
          .arg(u.m13,10,'f')
          .arg(u.m12,10,'f').toLatin1());
     // res.write(QString("rem x4=%1 \n").arg(matoms[i].x4).toLatin1()); 
    //  uc=matoms[i].u(t);
     /* res.write(QString("rem %1 %2 %3 %4 %5 %6 !uc\n")
          .arg(uc.m11)
          .arg(uc.m22)
          .arg(uc.m33)
          .arg(uc.m23)
          .arg(uc.m13)
          .arg(uc.m12).toLatin1());
      u=matoms[i].uf(t);
      res.write(QString("rem %1 %2 %3 %4 %5 %6 !uf\n")
          .arg(u.m11)
          .arg(u.m22)
          .arg(u.m33)
          .arg(u.m23)
          .arg(u.m13)
          .arg(u.m12).toLatin1());*/

    }
    res.write("HKLF 0\n");
    res.close();
  }
}

void MyWindow::doMapsNow(bool b){
  if (!b) {
    fmcq->killmaps(); 
    cubeGL->updateGL();
    return;
  } else {
    QString fouName;
    fouName=dirName;
    bool good=true;
    if (fouName.contains(QRegExp("m\\d\\d$"))) {
      fouName.chop(3);
      fouName.append("m80");
      if (cubeGL->isModulated) good=fmcq->loadDimensionm80AndPerform(fouName.toStdString().c_str());
      else good=fmcq->loadm80AndPerform(fouName.toStdString().c_str(),false);
      if (!good){
        infoKanalNews(QString("<font color=red>Could not load %1!</font><br>").arg(fouName));
        fmcq->deleteLists();
      }
    }else{
      fouName.chop(3);
      fouName.append("fou");
      if (!fmcq->loadFouAndPerform(fouName.toStdString().c_str(),false)){
        infoKanalNews(QString("<font color=red>Could not load %1!</font><br>").arg(fouName));
        fmcq->deleteLists();
      }
    }  
    cubeGL->updateGL();
  }
} 

void MyWindow::controlMap(){
  fmcq->maptrunc=mapSchnitt->currentIndex (); 
  if ((fmcq->rw!=weak->value())||(fmcq->rr!=mapprec->value())){
    fmcq->rr =  mapprec->value();  
    fmcq->rw = weak->value();

    QString fouName;
    if (dirName.contains(QRegExp("m\\d\\d$"))){
      fouName=dirName;
      fouName.chop(3);
      fouName.append("m80");
      bool good=true;
      if (cubeGL->isModulated) good=fmcq->loadDimensionm80AndPerform(fouName.toStdString().c_str());
      else good=fmcq->loadm80AndPerform(fouName.toStdString().c_str(),false);
      if (!good){
        infoKanalNews(QString("<font color=red>Could not load %1!</font><br>").arg(fouName));
        fmcq->deleteLists();
      }
    }else{
      fouName=dirName;
      fouName.chop(3);
      fouName.append("fou");
      //qDebug()<<fouName;
      if (!fmcq->loadFouAndPerform(fouName.toStdString().c_str(),false)){
        infoKanalNews(QString("<font color=red>Could not load %1!</font><br>").arg(fouName));
        fmcq->deleteLists();

      }
    }
  }    

  fmcq->lintrans = lineTrans->value();
  fmcq->linwidth = lineWidth->value();
  fmcq->map_radius = maprad->value();
  fmcq->iso[0] = fomaps->value();
  fmcq->iso[1] = -difmaps->value();
  fmcq->iso[2] = -f12maps->value();
  fmcq->inimap();
  cubeGL->updateGL();
}
void MyWindow::jnk(){
  fmcq->maptrunc=mapSchnitt->currentIndex ();  
  fmcq->rr =  mapprec->value();
  fmcq->rw = weak->value();

  QString fouName;
  if (dirName.contains(QRegExp("m\\d\\d$"))){
      fouName=dirName;
      fouName.chop(3);
      fouName.append("m80");
      if (!fmcq->loadm80AndPerform(fouName.toStdString().c_str(),false)){
        infoKanalNews(QString("<font color=red>Could not load %1!</font><br>").arg(fouName));
        fmcq->deleteLists();

      }
  }else{
      fouName=dirName;
      fouName.chop(3);
      fouName.append("fou");
      fprintf(stderr,"%s jnk %p\n",fouName.toStdString().c_str(),fmcq);
      if (!fmcq->loadFouAndPerform(fouName.toStdString().c_str(),false,4)){
        infoKanalNews(QString("<font color=red>Could not load %1 for jnk!</font><br>").arg(fouName));
        fmcq->deleteLists();

      }
  }


  fmcq->lintrans = lineTrans->value();
  fmcq->linwidth = lineWidth->value();
  fmcq->map_radius = maprad->value();
  fmcq->iso[0] = fomaps->value();
  fmcq->iso[1] = -difmaps->value();
  fmcq->iso[2] = -f12maps->value();
  fmcq->jnk();
  cubeGL->updateGL();
}

void MyWindow::addMoreQPeaks(){
  if (fmcq->n5==0) return;
   
  INP newAtom;
  newAtom.part=0;
  newAtom.sg=0;
  newAtom.OrdZahl=-3;
  for (int i=0; i<asymmUnit.size(); i++){
    if (asymmUnit.at(i).OrdZahl!=-3)continue;
    asymmUnit.removeAt(i);
    i--;
  }
  //htest.ownStyle=false;
  //htest.style=3;
  //htest.rad=0.2;
  //htest.color=QColor("seagreen");
  float x,y,z,u,v,w,p,q,r,t;
  int ipeak=0;
  for (int k=0;k<fmcq->n3;k++){
    for (int j=0;j<fmcq->n2;j++){
      for (int i=0;i<fmcq->n1;i++){
	t = fmcq->datfo_fc[i+ fmcq->n1*(j+k*fmcq->n2)];
	x = t -  fmcq->datfo_fc[((i+1)%fmcq->n1) + fmcq->n1*(j+k*fmcq->n2)];
	y = t -  fmcq->datfo_fc[i+ fmcq->n1*(((j+1)%fmcq->n2)+k*fmcq->n2)];
	z = t -  fmcq->datfo_fc[i+ fmcq->n1*(j+((k+1)%fmcq->n3)*fmcq->n2)];
	u = t -  fmcq->datfo_fc[((i+fmcq->n1-1)%fmcq->n1)+ fmcq->n1*(j+k*fmcq->n2)];
	v = t -  fmcq->datfo_fc[i+ fmcq->n1*(((j+fmcq->n2-1)%fmcq->n2)+k*fmcq->n2)];
	w = t -  fmcq->datfo_fc[i+ fmcq->n1*(j+((k+fmcq->n2-1)%fmcq->n3)*fmcq->n2)];
	if (fmin(x,fmin(y,fmin(z,fmin(u,fmin(v,w)))))<0) continue;
	p=0.5*(u-x)/(u+x);
	q=0.5*(v-y)/(v+y);
	r=0.5*(w-z)/(w+z);
	t=t+0.25*(p*(u-x)+q*(v-y)+r*(w-z));
	if (t<(fabs(fmcq->iso[1])*1.02)) continue;
	newAtom.peakHeight=t;  
	newAtom.frac.x = ((double) i + p +0.5)/fmcq->n1; 
	newAtom.frac.y = ((double) j + q +0.5)/fmcq->n2;
	newAtom.frac.z = ((double) k + r +0.5)/fmcq->n3;
	mol.frac2kart( newAtom.frac, newAtom.kart);
	if (!finite(newAtom.frac.x)||!finite(newAtom.frac.y)||!finite(newAtom.frac.z))continue;
	ipeak++;
//	newAtom.atomname=QString("Q%1").arg(ipeak,3,36,QChar('0'));
	sprintf(newAtom.atomname,"PK%d",ipeak);
	asymmUnit.append(newAtom);

      }
    }
  }
  smx+=ipeak;
  cubeGL->pause=true;
  growSymm(6);
  for (int i=0; i<smx; i++){
    mol.frac2kart(asymmUnit[i].frac,asymmUnit[i].kart);
  }
  
  for (int i=0; i<asymmUnit.size(); i++){
    for (int j=i+1; j<asymmUnit.size(); j++){
      if ((asymmUnit.at(i).OrdZahl!=-3)||(asymmUnit.at(j).OrdZahl!=-3)) continue;
      if (j==i)continue;
      double dd=0;
      if ((dd=Distance(asymmUnit.at(i).kart,asymmUnit.at(j).kart))<0.1) {
	asymmUnit.removeAt(j);j--;
      }
    }
  }

  QList<INP> pks;
  for (int i=0; i<asymmUnit.size(); i++){
    if (asymmUnit.at(i).OrdZahl!=-3)continue;
    pks.append(asymmUnit[i]);
    asymmUnit.removeAt(i);
    i--;
  }
  qSort(pks.begin(),pks.end());
  for (int i=0; i<pks.size(); i++) sprintf(pks[i].atomname,"Q%d",i+1);
  mol.pmax=pks.first().peakHeight;
  mol.pmin=pks.last().peakHeight;
  asymmUnit+=pks;

  QFile o;
  o.setFileName("mcq_peaklist.txt");
  if (o.open(QIODevice::WriteOnly|QIODevice::Text)){
o.write(QString("(%1 < %2) >%3 %4\n").arg( mol.pmin).arg(mol.pmax).arg(fmcq->iso[1]).arg(fabs(fmcq->iso[1])).toLatin1());
    for (int i=0; i<asymmUnit.size(); i++){
      if (asymmUnit.at(i).OrdZahl!=-3)continue;
      o.write(QString("%1 frac: %2 %3 %4  height: %5  cart: %6 %7 %8\n")
		      .arg(asymmUnit.at(i).atomname,-8)
		      .arg(asymmUnit.at(i).frac.x,12,'f',7)
		      .arg(asymmUnit.at(i).frac.y,12,'f',7)
		      .arg(asymmUnit.at(i).frac.z,12,'f',7)
		      .arg(asymmUnit.at(i).peakHeight,9,'f',5)
		      .arg(asymmUnit.at(i).kart.x,12,'f',7)
		      .arg(asymmUnit.at(i).kart.y,12,'f',7)
		      .arg(asymmUnit.at(i).kart.z,12,'f',7).toLatin1());

    }
  o.close();
  }
  smx=asymmUnit.size();
  growSymm(6);
  cubeGL->pause=false;
  cubeGL->updateGL();
}

void MyWindow::currentPhaseChanged(int p){
//  qDebug()<<"Phase change!"<<curentPhase<<p;
  curentPhase=p;
//  qDebug()<<curentPhase;
  loadFile(dirName,mol.gd);
//  qDebug()<<"Phase changed!"<<curentPhase<<p;

}

void MyWindow::createRenameWgd(){
  ///RENAME DOCK
  renamDock = new QDockWidget("Rename Mode");
  renamDock->setObjectName("Rename");
  enterRenameMode = renamDock->toggleViewAction ();
  connect(enterRenameMode,SIGNAL(triggered(bool)),this,SLOT(renamUpdate(bool)));
  connect(renamDock,SIGNAL(visibilityChanged(bool)),this,SLOT(renamUpdate(bool)));
  renamDock->hide();
  renamDock->setFeatures(
		  QDockWidget::DockWidgetMovable|
		  QDockWidget::DockWidgetFloatable|
		  QDockWidget::DockWidgetClosable);

  QGroupBox *renameBox = new QGroupBox();
  QVBoxLayout *rnla= new QVBoxLayout();
  QLabel *nliL = new QLabel("You are in 'rename mode' now.<br> Next Label is:");
  nliL->setFont(QFont("Helvetica",18,QFont::Bold));
  rnla->addWidget(nliL);
  nextNameL = new QLabel("<font color=\"red\">C(1)</font>");
  nextNameL->setFont(QFont("Helvetica",14,QFont::Bold));
  rnla->addWidget(nextNameL);
  labelSuffix="";
  nextLabel="C(1)";
  labelIndex=1;
  labelPSE="C";
  sfacBox = new QGroupBox("Scattering factors");
  sfacBox->setFixedSize(110*52,70);
  QHBoxLayout *sfacla = new QHBoxLayout();

  dsm= new QSignalMapper(this);
  sfacla->addWidget(pserbt[0] = new QRadioButton("??"));
  dsm->setMapping(pserbt[0],"?");
  pserbt[0]->hide();
  connect(pserbt[0], SIGNAL(clicked()), dsm, SLOT(map()));
  for (int i=1; i<109; i++){
    sfacla->addWidget(pserbt[i] = new QRadioButton(mol.pse(i-1)));
    dsm->setMapping(pserbt[i],mol.pse(i-1));
    connect(pserbt[i], SIGNAL(clicked()), dsm, SLOT(map()));
    pserbt[i]->hide();
  }
  connect(dsm, SIGNAL(mapped(const QString &)), this, SLOT(changeElemetofLabel(const QString &)));

  sfacBox->setLayout(sfacla);
  QScrollArea *qsa =new QScrollArea();
  qsa->setWidget(sfacBox);
  qsa->setMaximumHeight(100);
  rnla->addWidget(qsa);
  PSEWidget *psew = new PSEWidget(this);
  psew->selection=0;
  psew->setVisible(false);
  psew->setMaximumHeight(200);
  rnla->addWidget(psew);
  QPushButton *willpse = new QPushButton("more scattering factors");
  willpse->setStyleSheet(
		  QString(
			  "QPushButton {"
			  "border: 1px solid #000000;"
			  "border-radius: 9px;"
			  "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 0.5 %3, stop: 1 %2);"
			  "}"
			  "QPushButton:hover {"
			  "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %2, stop: 0.5 %1, stop: 1 %2);"
			  "}"
			  "QPushButton:flat {"
			  "    border: none; /* no border for a flat push button */"
			  "}"
			 ).arg("#dedede").arg("#eeeeee").arg("#aeaeae"));

  connect(willpse,SIGNAL(clicked()),psew,SLOT(show()));
  QTimer *hidwillpse=new QTimer();
  hidwillpse->setSingleShot(true);
  hidwillpse->setInterval(100);

  connect(willpse,SIGNAL(clicked()),hidwillpse,SLOT(start()));
  connect(hidwillpse,SIGNAL(timeout()),willpse,SLOT(hide()));
  connect(psew,SIGNAL(pseSelected(int)),psew,SLOT(hide()));
  connect(psew,SIGNAL(pseSelected(int)),willpse,SLOT(show()));
  connect(psew,SIGNAL(pseSelected(int)),this,SLOT(addNewScatteringFactor(int)));

  rnla->addWidget(willpse);
  QHBoxLayout *indexla = new QHBoxLayout();
  QLabel *indexL = new QLabel("Number");
  indexSpin = new QSpinBox();
  indexSpin->setMinimum(-1);
  indexSpin->setMaximum(999);
  indexSpin->setValue(labelIndex);
  sufixBox = new QComboBox();
  QLabel *sufixL =new QLabel("Suffix");
  QStringList alpha;
  alpha<<"'"<<""<<"A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"G"<<"H"<<"I"<<"I"<<"K"
	  <<"L"<<"M"<<"N"<<"O"<<"P"<<"Q"<<"R"<<"S"<<"T"<<"U"<<"V"<<"W"<<"X"<<"Y"<<"Z"
	  <<"A'"<<"B'"<<"C'"<<"D'"<<"E'"<<"F'"<<"G'"<<"H'"<<"I'"<<"J'"<<"K'"<<"L'"<<"M'"
	  <<"N'"<<"O'"<<"P'"<<"Q'"<<"R'"<<"S'"<<"T'"<<"U'"<<"V'"<<"W'"<<"X'"<<"Y'"<<"Z'";
  sufixBox->addItems(alpha);
  sufixBox->setCurrentIndex(1);
  sufixBox->setEditable(true);
  connect(sufixBox,SIGNAL( currentIndexChanged ( const QString &)),this,SLOT(changeSuffixofLabel( const QString & )));
  indexla->addWidget(indexL);
  indexla->addWidget(indexSpin);
  indexla->addWidget(sufixL);
  indexla->addWidget(sufixBox);
  rnla->addLayout(indexla);
  renameBox->setLayout(rnla);
  renamDock->setWidget(renameBox);
  addDockWidget(Qt::RightDockWidgetArea, renamDock);
  ///RENAME DOCK

}

void MyWindow::changeElemetofLabel(const QString &pse){
  labelPSE=pse;
  bool inuse=isLabelInUse();
  nextNameL->setText(QString("<font size=\"+19\" color=\"%2\">%1</font>%3")
		  .arg(nextLabel)
		  .arg((inuse)?"red":"green")
		  .arg((inuse)?"<font size=\"-19\"><br>Label is already in use.</font>":""));
  cubeGL->rn.Label=nextLabel;
  cubeGL->rn.an=mol.Get_OZ(labelPSE);
  update();
}

void MyWindow::changeIndexofLabel(int i){
  labelIndex=i;
  bool inuse=isLabelInUse();
  nextNameL->setText(QString("<font size=\"+19\" color=\"%2\">%1</font>%3")
		  .arg(nextLabel)
		  .arg((inuse)?"red":"green")
		  .arg((inuse)?"<font size=\"-19\"><br>Label is already in use.</font>":""));
  cubeGL->rn.Label=nextLabel;
  cubeGL->rn.an=mol.Get_OZ(labelPSE);
  update();
}

void MyWindow::changeSuffixofLabel(const QString &fix){
  labelSuffix=fix;
  bool inuse=isLabelInUse();
  nextNameL->setText(QString("<font size=\"+19\" color=\"%2\">%1</font>%3")
		  .arg(nextLabel)
		  .arg((inuse)?"red":"green")
		  .arg((inuse)?"<font size=\"-19\"><br>Label is already in use.</font>":""));
  cubeGL->rn.Label=nextLabel;
  cubeGL->rn.an=mol.Get_OZ(labelPSE);
  update();
}

void MyWindow::updateLabel(){
  indexSpin->setValue(labelIndex);
  bool inuse=isLabelInUse();
  nextNameL->setText(QString("<font size=\"+19\" color=\"%2\">%1</font>%3")
		  .arg(nextLabel)
		  .arg((inuse)?"red":"green")
		  .arg((inuse)?"<font size=\"-19\"><br>Label is already in use.</font>":""));

  cubeGL->rn.Label=nextLabel;
  cubeGL->rn.an=mol.Get_OZ(labelPSE);
  update();
}

bool MyWindow::isLabelInUse(){
  if (labelIndex<0) nextLabel=QString("%1(%2)").arg(labelPSE).arg(labelSuffix);
  else nextLabel=QString("%1(%2%3)").arg(labelPSE).arg(labelIndex).arg(labelSuffix);
  QStringList alab;
  for (int i =0; i<asymmUnit.size(); i++){
    alab.append(asymmUnit.at(i).atomname);
  }
  QString nn=nextLabel;
  bool inu=(alab.contains(nn,Qt::CaseInsensitive));
  if (enterRenameMode->isChecked()){
      cubeGL->rename=!inu;
      cubeGL->toggXDSetup(!inu);
  }
  return inu;
}

void MyWindow::addNewScatteringFactor(int oz){
  qDebug()<<mol.pse(oz)<<oz<<"This does not work! (Yet)"; 
  printf("geht (noch) nicht!\n"); 
}

void MyWindow::renamUpdate(bool vis){
  enterRenameMode->setChecked(vis);
  enterRenameMode->setText((vis)?"Exit Rename Mode":"Rename Mode");
  cubeGL->rename=false;
  printf("renamUpdate %d\n",vis);
  cubeGL->toggXDSetup(xdSetupAct->isChecked());
  if (vis) {
    updateLabel();
    renamDock->show();
    renamDock->raise();
//    printf("sollte\n");
  }//else {     printf("sollte nicht\n");   }

}

void MyWindow::findAtoms(){
  cubeGL->disSelection();
  QString find = searchAtomEdit->text();
  for (int i=0; i<xdinp.size();i++){
    if (find == QString(xdinp.at(i).atomname)) cubeGL->selectedAtoms.append(xdinp.at(i));
  }  
  cubeGL->centerSelection->setChecked ( true);
  cubeGL->updateBondActions();
  cubeGL->updateGL();
}

void MyWindow::brwsCont(){
  QString selectedFilter;
  cubeGL->moliso->contMapFile->setText(QFileDialog::getOpenFileName(this, tr("Open Contour-grid file "), "",
        "Jana2006 m81-Files (*.m81);;"
        "BayMEM binary file (*.raw);;"
        "General volumetric data (*.?ed);;"
        "GAUSSIAN Cube-Files (*.cube *.cub *.rho_r_3);;" ,&selectedFilter,QFileDialog::DontUseNativeDialog ));
}

void MyWindow::brwsEPS(){
  QString selectedFilter;
  cubeGL->moliso->contEPSFile->setText(QFileDialog::getSaveFileName(this, tr("Save Contour-Encapsulated PostScript file "), "",
        "Encapsulated PostScript-Files (*.eps);;"
        ,&selectedFilter,QFileDialog::DontUseNativeDialog ));
  if (!cubeGL->moliso->contEPSFile->text().endsWith(".eps",Qt::CaseInsensitive))
    cubeGL->moliso->contEPSFile->setText(cubeGL->moliso->contEPSFile->text()+".eps");
}

void MyWindow::contourPlot(){
  if (xdinp.isEmpty()) return;
  if (cubeGL->moliso==NULL){
  cubeGL->moliso = new MolIso();
  cubeGL->moliso->fixmax=isomax;
  cubeGL->moliso->fixmin=isomin;
  connect(cubeGL->moliso,SIGNAL(bigmessage(const QString &)),this,SLOT(infoKanalNews(const QString&)));
  cubeGL->moliso->L=cubeGL->L;
  cubeGL->moliso->orig=Vector3(0.0,0.0,0.0);
  }

  QDialog *contDlg     = new QDialog(this);
  QLabel *contMapFileL = new QLabel("3D-Map or Grid-File",contDlg);
  QPushButton *brwsCF  = new QPushButton("Browse",contDlg);
  connect(brwsCF,SIGNAL(pressed()),this,SLOT(brwsCont()));
  cubeGL->moliso->contMapFile  = new QLineEdit(contDlg);
  
  QLabel *contEPSFileL = new QLabel("Write Encapsulated Postscript file",contDlg);
  QPushButton *brwsEPS  = new QPushButton("Browse",contDlg);
  connect(brwsEPS,SIGNAL(pressed()),this,SLOT(brwsEPS()));
  cubeGL->moliso->contEPSFile  = new QLineEdit(contDlg);

  QSettings settings(QSettings::IniFormat,  QSettings::UserScope ,"Christian_B._Huebschle", "MoleCoolQt" );
  settings.beginGroup("Version 0.1");    
  cubeGL->moliso->contMapFile->setText(settings.value("iso_grid_name").toString());
  settings.endGroup();
  QLabel *planeAtomDefL = new QLabel("Define plane by:");
  int at1=0,at2=1,at3=2;
  if (cubeGL->selectedAtoms.size()>2){
    at1=cubeGL->selectedAtoms.at(0).GLname;
    at2=cubeGL->selectedAtoms.at(1).GLname;
    at3=cubeGL->selectedAtoms.at(2).GLname;
  }
  QLineEdit *planeAtomDef = new QLineEdit(QString("%1 %2 %3")
      .arg(xdinp.at(at1).atomname)
      .arg(xdinp.at(at2).atomname)
      .arg(xdinp.at(at3).atomname));
  planeAtomDef->setReadOnly(true);
  planeAtomDef->setEnabled(false);
  /*
  cubeGL->moliso->contMapFile->setReadOnly(true);
  cubeGL->moliso->contMapFile->setEnabled(false);
  */
  cubeGL->moliso->contourValueEdit = new QLineEdit(contDlg);
  cubeGL->moliso->contourValueEdit->setText("-1.0 0.0 1.0");
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), contDlg, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), contDlg, SLOT(reject()));  
  QPushButton *edo  = new QPushButton("uniform contours",contDlg);
  cStepBx = new QDoubleSpinBox(contDlg);
  cStepBx->setPrefix("Step size: ");
  cStepBx->setMinimum(0.001);
  cStepBx->setDecimals(3);
  cStepBx->setValue(0.1);
  cStepBx->setSingleStep(0.05);
  cubeGL->moliso->cScopeBx = new QDoubleSpinBox(contDlg);
  cubeGL->moliso->cScopeBx->setMinimum(0.0);
  cubeGL->moliso->cScopeBx->setDecimals(1);
  cubeGL->moliso->cScopeBx->setSingleStep(0.5);
  QPushButton *eAIM = new QPushButton("+AIM contours",contDlg);
  connect(edo ,SIGNAL(released ()),this,SLOT(everyPointOne()));
  connect(eAIM,SIGNAL(released ()),this,SLOT(everyAIM()));
  QLabel *hint = new QLabel("<div style=color:#0000aa; style=white-space:normal; style=background-color:"
      "#f8f5cd; style=font-size:large;><b><u>Hint:</u></b>"
      "Select 3 atoms before using this feature in order to define a plane.</div>");
  hint->setWordWrap(true);
  cubeGL->moliso->centerIsOn = new QComboBox(contDlg);
  cubeGL->moliso->centerIsOn ->addItem("first atom",0);
  cubeGL->moliso->centerIsOn ->addItem("second atom",1);
  cubeGL->moliso->centerIsOn ->addItem("third atom",2);
  cubeGL->moliso->centerIsOn ->addItem("center of 3 atoms",3);

  cubeGL->moliso->aspectRatios = new QComboBox(contDlg);
  cubeGL->moliso->aspectRatios->addItem("4:3",0);
  cubeGL->moliso->aspectRatios->addItem("16:9",1);
  cubeGL->moliso->aspectRatios->addItem("1:1",2);
  cubeGL->moliso->aspectRatios->setCurrentIndex(0);

  QGridLayout *gl = new QGridLayout(contDlg);
  gl->addWidget(contMapFileL, 0, 0, 1, 1);
  gl->addWidget(cubeGL->moliso->contMapFile, 0, 1, 1, 3);
  gl->addWidget(brwsCF, 0, 4, 1, 1);
  gl->addWidget(planeAtomDefL, 1, 0, 1, 1);
  gl->addWidget(planeAtomDef, 1, 1, 1, 1);
  gl->addWidget(new QLabel("Center on",contDlg), 1, 3, 1, 1);
  gl->addWidget(cubeGL->moliso->centerIsOn, 1, 4, 1, 1);
  gl->addWidget(hint,2,0,1,5);
  gl->addWidget(new QLabel("Contour values",contDlg), 3, 0, 1, 4);
  gl->addWidget(cubeGL->moliso->contourValueEdit, 4, 0, 1, 5);
  gl->addWidget(cStepBx ,5,0,1,1);
  gl->addWidget(edo,5,1,1,1);
  gl->addWidget(eAIM,5,2,1,1);
  gl->addWidget(new QLabel("Scope (0 means now restriction)"),6,0,1,1);
  gl->addWidget(cubeGL->moliso->cScopeBx,6,1,1,1);
  gl->addWidget(new QLabel("Aspect ratio"),6,3,1,1);
  gl->addWidget(cubeGL->moliso->aspectRatios,6,4,1,1);

  gl->addWidget(contEPSFileL, 7, 0, 1, 1);
  gl->addWidget(cubeGL->moliso->contEPSFile, 7, 1, 1, 3);
  gl->addWidget(brwsEPS, 7, 4, 1, 1);
  gl->addWidget(buttonBox, 10, 0, 1, 5);

  if (contDlg->exec()==QDialog::Accepted){
    cubeGL->moliso->makePlane(cubeGL->cont,at1,at2,at3);
    cubeGL->selectedAtoms.clear();
    /*showLeg->setChecked ( true );
    showLeg->setVisible(true);
    cubeGL->setLegend(true);*/
    cubeGL->updateGL();
  }

}

void MyWindow::everyPointOne(){
  QString cfs;
  if (cStepBx->value()==0) return;
  double val=-100*cStepBx->value();
  double end=-val;  
  while (val<end) {
    cfs.append(QString("%1 ").arg(val));
    val+=cStepBx->value();
  }
  cubeGL->moliso->contourValueEdit->setText(cfs);
  update();
}

void MyWindow::everyAIM(){
  double val=0.001;
//  double step=fabs(0.001)/5.0f;
  int i=-3;
  QString cfs;
  while (val<1e9) {
    cfs.append(QString("%1 ").arg(val));//1
    val+=1*pow(10,i);
    cfs.append(QString("%1 ").arg(val));//2
    val+=2*pow(10,i);
    cfs.append(QString("%1 ").arg(val));//4
    val+=2*pow(10,i);
    cfs.append(QString("%1 ").arg(val));//6
    val+=2*pow(10,i);
    cfs.append(QString("%1 ").arg(val));//8
    val+=2*pow(10,i);
    
    i++;
  }

  cubeGL->moliso->contourValueEdit->setText(cfs);
  update();

}

void MyWindow::genMoliso(QString isoname, QString mapname, QString lfcename, QString sfcename, int check, QString adpname) {

  atmax=0;
  smx=0;
  int fileType=0;
  xdinp.clear();
  asymmUnit.clear();
  cubeGL->pause=true;
  cubeGL->negpdf=true;
  fmcq->deleteLists();
  fmcq->doMaps->hide();
  cubeGL->fofcact->setVisible(false);
  cubeGL->foact->setVisible(false);
  cubeGL->f1f2act->setVisible(false);
  fmcq->n1 = fmcq->n2 = fmcq->n3 = fmcq->n4 = fmcq->n5=0;
  if (cubeGL->moliso){
    delete cubeGL->moliso;
    cubeGL->moliso=NULL;
  }
  cubeGL->moliso = new MolIso();
  cubeGL->moliso->fixmax=isomax;
  cubeGL->moliso->fixmin=isomin;
  connect(cubeGL->moliso,SIGNAL(bigmessage(const QString &)),this,SLOT(infoKanalNews(const QString&)));
  cubeGL->moliso->L=cubeGL->L;
  cubeGL->moliso->orig=Vector3(0.0,0.0,0.0);
  QString isof,mapf,lfaceFile,sfaceFile,adpName;
  MolisoStartDlg *msgdlg = new MolisoStartDlg(this,isoname,mapname,lfcename,sfcename,check,adpname);
  if (msgdlg->exec()==QDialog::Accepted) {
    isof=msgdlg->iso_grid_name;
    mapf=msgdlg->map_grid_name;
    lfaceFile=msgdlg->load_face_name;
    sfaceFile=msgdlg->save_face_name;
    adpName=msgdlg->adp_struct_name;
    mol.initDir();
  }
  else {
    delete(cubeGL->moliso);
    cubeGL->moliso=NULL;    
    cubeGL->pause=false;
    return;
  }
  GradDlg *grdlg = new GradDlg(cubeGL->moliso,mapf );
  //dock->hide();
  //dock2->hide();
 // cubeGL->setVisible(false);
  if (grdlg->exec()!=QDialog::Accepted){
      cubeGL->setVisible ( true );
      dock->show();
      dock2->show();
      delete(cubeGL->moliso);
      cubeGL->moliso=NULL;
      cubeGL->pause=false;
      return;
    }
  cubeGL->moliso->calcextrema=grdlg->cmmcp->isChecked();
  delete grdlg;

  dock3 = new QDockWidget("Moliso control area",this);
  dock3->setAllowedAreas(Qt::AllDockWidgetAreas);
  strikesSldr = new QSlider(Qt::Horizontal);
  strikesSldr->setMaximum(510);
  strikesSldr->setMinimum(127);
  strikesSldr->setValue(496);
  swidthSldr = new QSlider(Qt::Horizontal);
  swidthSldr->setMaximum(128);
  swidthSldr->setMinimum(1);
  swidthSldr->setValue(1);
  zebraBox = new QCheckBox("Show contour belts");
  zebraBox->setChecked(false);
  zebraBox->setShortcut(tr("F8"));
  QGroupBox *zebraZwinger= new QGroupBox();
  QVBoxLayout *zla= new QVBoxLayout();
  zla->addWidget(zebraBox);
  zla->addWidget(new QLabel("Number of contour belts:"));
  zla->addWidget(strikesSldr);
  zla->addWidget(new QLabel("Contour belts width:"));
  zla->addWidget(swidthSldr);
  mt = new QCheckBox("transparence");
  mt->setChecked(true);
  mt->setShortcut(tr("Alt+T"));
  cubeGL->togglMolisoTransparence(true);
  zla->addWidget(mt);
  legendSize = new QSlider(Qt::Horizontal);
  legendSize->setValue(30);
  legendSize->setMaximum(100);
  cullNone = new QRadioButton("No culling");
  cullBack = new QRadioButton("Back face culling");
  cullFront = new QRadioButton("Front face culling");
  cullNone->setChecked(true);
  cubeGL->noneCull(true);
  connect(cullNone,SIGNAL(toggled(bool)),cubeGL,SLOT(noneCull(bool)));
  connect(cullBack,SIGNAL(toggled(bool)),cubeGL,SLOT(backCull(bool)));
  connect(cullFront,SIGNAL(toggled(bool)),cubeGL,SLOT(frontCull(bool)));
  zla->addWidget(cullNone);
  zla->addWidget(cullBack);
  zla->addWidget(cullFront);
  QToolBar *tb = new QToolBar("Moliso toolbar",zebraZwinger);
  tb->setOrientation(Qt::Vertical);
  tb->setIconSize(QSize(16,16));
  tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  tb->addAction(showface);
  tb->addAction(showLeg);
  tb->addAction(movLeg);
  tb->addAction(mildir);
  zla->addWidget(tb);
  mclmox =new QCheckBox("Monochrome legend text");
  mclmox->setChecked(false);
  connect(mclmox,SIGNAL(toggled(bool)),cubeGL,SLOT(togglMonoChrom(bool)));

  zla->addWidget(mclmox);
  zla->addWidget(new QLabel("Scale legend"));
  zla->addWidget(legendSize);
  QPushButton *savset = new QPushButton("Save current settings");
  connect(savset,SIGNAL(pressed()),cubeGL,SLOT(saveMISettings()));
  QPushButton *lodset = new QPushButton("Load settings");
  connect(lodset,SIGNAL(pressed()),cubeGL,SLOT(loadMISettings()));
  mlf = new QFontComboBox();
  mlf->setCurrentFont(cubeGL->MLegendFont);
  connect(mlf,SIGNAL(currentFontChanged(QFont)),cubeGL,SLOT(setMLFont(QFont)));
  fos = new QSpinBox();
  fos->setMinimum(4);
  fos->setMaximum(272);
  fos->setValue(cubeGL->MLegendFont.pointSize());
  connect(fos,SIGNAL(valueChanged(int)),cubeGL,SLOT(setMLFontSize(int)));
  QPushButton *expWFobj= new QPushButton("Export surfaces *.obj");
  connect(expWFobj,SIGNAL(pressed()),  cubeGL->moliso,SLOT(exportObj()));
  QPushButton *expSTL= new QPushButton("Export surfaces *.stl");
  connect(expSTL,SIGNAL(pressed()),  cubeGL->moliso,SLOT(exportSTL()));
  zla->addWidget(mlf);
  zla->addWidget(fos);
  zla->addWidget(savset);
  zla->addWidget(lodset);
  zla->addWidget(expWFobj);
  zla->addWidget(expSTL);
  zla->addStretch(999);
  connect(cubeGL,SIGNAL(mconf()),this,SLOT(syncMconf()));
  cubeGL->togglContours(false);
  cubeGL->scaleLegend(30);
  cubeGL->setContourCnt(496);
  cubeGL->setContourWidth(1);
  connect(mt,SIGNAL(toggled(bool)),cubeGL,SLOT(togglMolisoTransparence(bool)));
  connect(strikesSldr,SIGNAL(valueChanged(int)),cubeGL,SLOT(setContourCnt(int)));
  connect(swidthSldr,SIGNAL(valueChanged(int)),cubeGL,SLOT(setContourWidth(int)));
  connect(legendSize,SIGNAL(valueChanged(int)),cubeGL,SLOT(scaleLegend(int)));
  connect(zebraBox,SIGNAL(toggled(bool)),cubeGL,SLOT(togglContours(bool)));
  zebraZwinger->setLayout(zla);
  dock3->setWidget(zebraZwinger);
  dock3->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetClosable);
  addDockWidget(Qt::LeftDockWidgetArea, dock3);
  cubeGL->moliso->readXDGridHeader(isof,fileType);
  if (fileType==-666) {
  destroyMoliso();
  return;
  }
  mol.adp=0;
  if (fileType==81) mol.adp=1;
  cubeGL->L = 100.0/dimension(asymmUnit);
  cubeGL->moliso->L=cubeGL->L;
  if (smx>7){
  atom1Pos=asymmUnit[0].kart;
  atom2Pos=asymmUnit[1].kart;
  atom3Pos=asymmUnit[2].kart;
  //printf("%s %s %s \n",asymmUnit[0].atomname, asymmUnit[1].atomname, asymmUnit[2].atomname);
  }
//  printf("%10g %10g %10g   %10g %10g %10g   %10g %10g %10g\n",atom1Pos.x,atom1Pos.y,atom1Pos.z,atom2Pos.x,atom2Pos.y,atom2Pos.z,atom3Pos.x,atom3Pos.y,atom3Pos.z);


  if ((adpName.isEmpty())||(adpName.contains('!'))){
    togElli->setVisible ( false );
    toggleElli(false);
    cubeGL->setEllipsoidNoUpdate(false);
    cubeGL->drawUz=false;
    george=false;
    seReAct->setEnabled(false);
    seReAct->setVisible(false);
    xdSetupAct->setEnabled(false);
    cubeGL->toggXDSetup(false);
    xdSetupAct->setChecked ( false );
    xdMenu->setEnabled(false);
 //   togLuft->setVisible(false);
    togAxen->setEnabled (true );
    togUnit->setEnabled (true );
//    packAct->setVisible(false);
    mol.zelle.a=1.0;
    mol.zelle.b=1.0;
    mol.zelle.c=1.0;
    mol.zelle.al=90.0;
    mol.zelle.be=90.0;
    mol.zelle.ga=90.0;
    xdinp=asymmUnit;
    cubeGL->resetENV();
    initLists(xdinp);
    printf("orig %d %f %f %f\n",cubeGL->moliso->cubeiso,cubeGL->moliso->orig.x,cubeGL->moliso->orig.y,cubeGL->moliso->orig.z);
  }
  else {
    statusBar()->showMessage(tr("loading sructure coordinates...") );
    updateStatusBar();
    setCursor(Qt::BusyCursor);
    printf("A\n");
    loadFile(adpName);
    cubeGL->moliso->L=cubeGL->L;
    printf("B\n");
    printf("orig %d %f %f %f\n",cubeGL->moliso->cubeiso,cubeGL->moliso->orig.x,cubeGL->moliso->orig.y,cubeGL->moliso->orig.z);

  }
  if ((lfaceFile.isEmpty())||(lfaceFile.contains('!'))) {
    if (sfaceFile.contains('!'))sfaceFile.clear();
    setCursor(Qt::BusyCursor);
    statusBar()->showMessage(tr("calculating surfaces") );
//    printf("C\n");
    cubeGL->moliso->createSurface(isof,mapf,sfaceFile,fileType);
//    printf("D\n");
    statusBar()->showMessage(tr("surfaces calculated" ) );
    lfaceFile=sfaceFile;
  }
  if (cubeGL->moliso->mibas) glDeleteLists(cubeGL->moliso->mibas,6);
  cubeGL->moliso->mibas=glGenLists(6);
  statusBar()->showMessage(tr("loading surfaces...") );
  //qDebug()<<cubeGL->moliso->mibas<<cubeGL->foubas[0];
  if ((lfaceFile!=sfaceFile)&&!lfaceFile.isEmpty()&&(!lfaceFile.contains('!'))) {
     cubeGL->moliso->loadMI(lfaceFile,true);
  }
  else cubeGL->moliso->loadMI(lfaceFile,false);
//  cubeGL->moliso->makePlane(cubeGL->cont,0,1,2);
  /*
  cubeGL->moliso->findContour(cubeGL->cont,0.00f);
  cubeGL->moliso->findContour(cubeGL->cont,0.10f);
  cubeGL->moliso->findContour(cubeGL->cont,0.20f);
  cubeGL->moliso->findContour(cubeGL->cont,0.30f);
  cubeGL->moliso->findContour(cubeGL->cont,0.40f);
  cubeGL->moliso->findContour(cubeGL->cont,0.50f);
  cubeGL->moliso->findContour(cubeGL->cont,0.60f);
  cubeGL->moliso->findContour(cubeGL->cont,0.70f);
  cubeGL->moliso->findContour(cubeGL->cont,0.80f);
  cubeGL->moliso->findContour(cubeGL->cont,0.90f);
  cubeGL->moliso->findContour(cubeGL->cont,1.00f);
  cubeGL->moliso->findContour(cubeGL->cont,-0.10f);
  cubeGL->moliso->findContour(cubeGL->cont,-0.20f);
  cubeGL->moliso->findContour(cubeGL->cont,-0.30f);
  cubeGL->moliso->findContour(cubeGL->cont,-0.40f);
  cubeGL->moliso->findContour(cubeGL->cont,-0.50f);
  cubeGL->moliso->findContour(cubeGL->cont,-0.60f);
  cubeGL->moliso->findContour(cubeGL->cont,-0.70f);
  cubeGL->moliso->findContour(cubeGL->cont,-0.80f);
  cubeGL->moliso->findContour(cubeGL->cont,-0.90f);
  cubeGL->moliso->findContour(cubeGL->cont,-1.00f);
  printf("Contour has %dlines.\n",cubeGL->cont.size());*/
  if (cubeGL->moliso->calcextrema) il();
  updateStatusBar();
  statusBar()->showMessage(tr("surfaces loaded") );
  cubeGL->MIS=true;
  cubeGL->MILe=true;
  showface->setChecked ( true );
  showface->setVisible(true);
  showLeg->setChecked ( true );
  showLeg->setVisible(true);
  movLeg->setChecked ( false );
  movLeg->setVisible(true);
  mildir->setChecked ( true  );
  mildir->setVisible(true);
  setCursor(Qt::ArrowCursor);
  //dock->hide();
  //dock2->hide();
  cubeGL->setVisible ( true );
  cubeGL->updateGL();
  QMainWindow::tabifyDockWidget (dock2,dock3);
  QMainWindow::tabifyDockWidget (dock2,dock);
  createmoliso->setVisible(false);
  noMoliso->setVisible(true);  
  cubeGL->pause=false;
}

void MyWindow::syncMconf(){
  legendSize->setValue((int)( cubeGL->vangle/cubeGL->milsize ));
  if ((strikesSldr!=NULL) && (strikesSldr->isVisible())) 
    strikesSldr->setValue(512-cubeGL->cdens);
  if (swidthSldr!=NULL)  swidthSldr->setValue((cubeGL->cwid+1)*256/cubeGL->cdens);   
  if (zebraBox!=NULL)  zebraBox->setChecked(cubeGL->zebra);
    mt->setChecked(cubeGL->molisoTransparence);
  togAxen->setChecked(cubeGL->drawAx );
  togUnit->setChecked(cubeGL->drawUz );
  back_Grad->setChecked(cubeGL->back_grad );
  togAtom->setChecked(cubeGL->drawAt );
  togElli->setChecked(cubeGL->elli );
  togBond->setChecked(cubeGL->drawBo );
  togLabel->setChecked(cubeGL->drawLa );
  togHBond->setChecked(cubeGL->drawHb );
 // togLuft->setChecked(cubeGL->Luftschlange );
  depthCueingAct->setChecked(cubeGL->depthCueing );
  mclmox->setChecked(cubeGL->monochrom );
    switch (cubeGL->faceCull){
	case 0: cullNone->setChecked(true);break;
	case 1:	cullBack->setChecked(true);break;
	case 2: cullFront->setChecked(true);break;
    }
    fos->setValue(cubeGL->MLegendFont.pointSize());
    mlf->setCurrentFont(cubeGL->MLegendFont);
    update();
}

void MyWindow::destroyMoliso(){
    atom1Pos=V3(0,0,0);
    atom2Pos=V3(0,0,0);
    atom3Pos=V3(0,0,0);
  atmax=0;
  smx=0;
  xdinp.clear();
  mol.wombats.clear();
  asymmUnit.clear();
  mol.entknoten();
  cubeGL->cont.clear();
  glDeleteLists(cubeGL->moliso->mibas,6);
  if (cubeGL->bas) glDeleteLists(cubeGL->bas,10);
  if (cubeGL->moliso){
      if (cubeGL->moliso->faceFile.contains("qt_temp")) QFile::remove (cubeGL->moliso->faceFile);
    delete cubeGL->moliso;
    cubeGL->moliso=NULL;
  }
  cubeGL->rotze=-1;
filtered=0;
oxd.clear();
unfilterAct->setVisible(false);
  dFilter->setVisible(false);
  hFilter->setVisible(false);
  cubeGL->MIS=false;
  cubeGL->MILe=false;
  showface->setChecked ( false );
  showface->setVisible(false);
  showLeg->setChecked ( false );
  showLeg->setVisible(false);
  movLeg->setChecked ( false );
  movLeg->setVisible(false);
  mildir->setChecked ( false  );
  mildir->setVisible(false);
  setCursor(Qt::ArrowCursor);
  atomClickAct->setChecked ( true );
  atomClickAct->setVisible ( true );
  ignorant->setChecked ( true );
  ignorant->setVisible ( true );
  cubeGL->setVisible ( true );
  cubeGL->updateGL();
  delete dock3;
  dock->show();
  dock2->show();
  createmoliso->setVisible(true);
  noMoliso->setVisible(false);
}

void MyWindow::infoKanalNews(const QString& s){
  QString alt = infoKanal->toHtml();
  alt.append(s);
  infoKanal->setHtml(alt);
  QTextCursor c = infoKanal->textCursor();
  c.movePosition(QTextCursor::End);
  infoKanal->setTextCursor(c);
  infoKanal->ensureCursorVisible();
  dock->show();
  dock->raise();
}

void MyWindow::tidyCPS(QString cpsp){
  QFile cps(cpsp);
  cps.open(QIODevice::ReadOnly);
  QString cpsTextall=cps.readAll();
  QStringList cpsText=cpsTextall.split("\n");
  QStringList items;
  QList<V3> points;
  QString line;
  for (int i=0; i<cpsText.size();i++){
    line=cpsText.at(i);
    items=line.split(" ",QString::SkipEmptyParts);
    if (items.size()>5){
      if (items.at(4).contains(QRegExp("[A-Za-z])"))){
	V3 pt;
	pt.x=items.at(5).toDouble();
	pt.y=items.at(6).toDouble();
	pt.z=items.at(7).toDouble();
	double dm=10,d;
	for (int j=0; j<points.size(); j++){
	  d=sqrt(Distance(points.at(j),pt));
	  dm=(dm<d)?dm:d;
	}
	if (dm <0.05) {
	  cpsText.removeAt(i);
          i--;
	}
	else points.append(pt);
      }else if (items.at(3).contains(QRegExp("[A-Za-z])"))){
	V3 pt;
	pt.x=items.at(4).toDouble();
	pt.y=items.at(5).toDouble();
	pt.z=items.at(6).toDouble();
	double dm=10,d;
	for (int j=0; j<points.size(); j++){
	  d=sqrt(Distance(points.at(j),pt));
	  dm=(dm<d)?dm:d;
	}
	if (dm <0.05) {cpsText.removeAt(i);i--;}
	else 
	  points.append(pt);
      }
    }
  }
  cps.remove();
  cps.open(QIODevice::WriteOnly);
  cps.write(cpsText.join("\n").toLatin1(),cpsText.join("\n").length());
  cps.close();
}

void MyWindow::tidyCPS(){
  QFile cps("xd_rho.cps");
  cps.open(QIODevice::ReadOnly);
  QString cpsTextall=cps.readAll();
  QStringList cpsText=cpsTextall.split("\n");
  QStringList items;
  QList<V3> points;
  QString line;
  for (int i=0; i<cpsText.size();i++){
    line=cpsText.at(i);
    items=line.split(" ",QString::SkipEmptyParts);
    if (items.size()>5){
      if (items.at(4)=="Rho") {
	V3 pt;
	pt.x=items.at(5).toDouble();
	pt.y=items.at(6).toDouble();
	pt.z=items.at(7).toDouble();
	double dm=10,d;
	for (int j=0; j<points.size(); j++){
	  d=sqrt(Distance(points.at(j),pt));
	  dm=(dm<d)?dm:d;
	}
	if (dm <0.05) {
	  cpsText.removeAt(i);
          i--;
	}
	else points.append(pt);
      }else if (items.at(3)=="Rho") {
	V3 pt;
	pt.x=items.at(4).toDouble();
	pt.y=items.at(5).toDouble();
	pt.z=items.at(6).toDouble();
	double dm=10,d;
	for (int j=0; j<points.size(); j++){
	  d=sqrt(Distance(points.at(j),pt));
	  dm=(dm<d)?dm:d;
	}
	if (dm <0.05) {cpsText.removeAt(i);i--;}
	else 
	  points.append(pt);
      }
    }
  }
  cps.remove();
  cps.open(QIODevice::WriteOnly);
  cps.write(cpsText.join("\n").toLatin1(),cpsText.join("\n").length());
  cps.close();
}

void MyWindow::someThingToRestore(){
  QDir work=QDir(QDir::current());  
  QStringList filter;
  filter << "*res_MCQsave*";  
  QStringList resfs;
  resfs=work.entryList(filter,QDir::Files,QDir::Time);   
  bool wasda=(resfs.size()>0);
  xdRestoreAct->setEnabled(wasda);
}

void MyWindow::xdenv(){
  Doxd *doXD = new Doxd();
  doXD->presentDialog();
}

void MyWindow::xdprop(){
  Doxd *doXD = new Doxd();
  doXD->runXdProp(CID);
}

void MyWindow::xdlsm(){
  Doxd *doXD = new Doxd();
  connect(doXD, SIGNAL (reloadXD ()),
	  this, SLOT(reloadXDfiles()));
  doXD->runXdLsm(CID);
}

void MyWindow::resinp(){
  QFile res("xd.res");
  QFile inp("xd.inp");
  inp.remove();
  res.copy("xd.inp");
}

void MyWindow::setRO(){
  QTextCursor c=editor-> textCursor();
  int spalte=c.columnNumber();
  int zeile=c.blockNumber();  
  QString t=c.block().text();
  int min = t.lastIndexOf(" is_part ");
  min=(min>0)?min+9:15;
  editor->setReadOnly((spalte<min));
  if ((spalte<min)) {
    c.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,fabs(min-spalte));
    if (c.isNull()) {
      c=editor-> textCursor();
       c.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
       c.movePosition(QTextCursor::EndOfLine,QTextCursor::MoveAnchor);
       c.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,zeile-1);
    }
    editor->setTextCursor(c);
  }
  editor->setOverwriteMode(true); 
}

void MyWindow::editPartAux(){
  GLdouble MM[16];
  glGetDoublev(GL_MODELVIEW_MATRIX,MM);
  int roz=cubeGL->rotze;
  QAction *action = qobject_cast<QAction *>(sender());
  int index=0;
  if (action){
  index=action->data().toInt();
  }
  QFile aux("xd_part.aux");
  editor = new QTextEdit;
  connect(editor,SIGNAL(cursorPositionChanged ()), this  ,SLOT(setRO()));
  search = new QLineEdit;
  search->setMinimumSize(QSize(150, 0));
  QHBoxLayout *slt=new QHBoxLayout();
  QLabel sela("Search:");
  QToolButton prev;
  prev.setIcon(QIcon(":/images/moveleft.png"));
  prev.setText("Previous");
  prev.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  prev.setAutoRaise(true);
  QToolButton next;
  next.setIcon(QIcon(":/images/moveright.png"));
  next.setText("Next");
  next.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  next.setAutoRaise(true);
  slt->setSpacing(6);
  slt->setMargin(0);
  slt->addWidget(&sela);
  slt->addWidget(search);
  slt->addWidget(&prev);
  slt->addWidget(&next);
  slt->addStretch(10);
  connect(search, SIGNAL(textChanged(const QString&)),this,SLOT(findText()));
  connect(&next, SIGNAL(clicked() ), this, SLOT(findNext()));
  connect(&prev, SIGNAL(clicked() ), this, SLOT(findPrev()));
  QFont font;
  font.setFamily("Courier");
  font.setFixedPitch(true);
  font.setPointSize(10);  
  editor->setFont(font);
  Highlighter *highlighter = new Highlighter(editor->document());
  if (aux.open(QIODevice::ReadOnly|QIODevice::Text)){
    editor->setPlainText(aux.readAll());
    aux.close();
  }
  else{
    QString txt;
    for (int i=0; i<asymmUnit.size(); i++){
      txt.append(QString("%1 is_part %2\n").arg(asymmUnit.at(i).atomname).arg(asymmUnit.at(i).part));
    }
   editor->setPlainText(txt);   
  }
  QTextCursor c=editor-> textCursor();
  c.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
  c.movePosition(QTextCursor::EndOfLine,QTextCursor::MoveAnchor);
  c.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,index);
  editor->setTextCursor(c);

  QDialog *xdpartauxdlg =new QDialog(this);
  xdpartauxdlg->setMinimumSize(900,300);  
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
  buttonBox->clearFocus ();
  buttonBox->setFocusPolicy(Qt::NoFocus);
  connect(buttonBox, SIGNAL(accepted()), xdpartauxdlg, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), xdpartauxdlg, SLOT(reject()));  
  QVBoxLayout sss;
  sss.addWidget(editor);
  sss.addLayout(slt);
  sss.addWidget(buttonBox);
  xdpartauxdlg->setLayout(&sss);
  xdpartauxdlg->setWindowTitle("xd_part.aux editor");
  if(QDialog::Accepted==xdpartauxdlg->exec()){
    aux.open(QIODevice::WriteOnly|QIODevice::Text);
    aux.write(editor->toPlainText().toLatin1(),editor->toPlainText().length());
    aux.close();
    delete highlighter;
    delete editor;
    reloadFiles();
  }else
    if (editor->document()->isModified()){
  if ((QMessageBox::question(this,"Save changes to xd_part.aux?","Save changes to xd_part.aux?",QMessageBox::Save|QMessageBox::Discard)==QMessageBox::Save)){
    aux.open(QIODevice::WriteOnly);
    aux.write(editor->toPlainText().toLatin1(),editor->toPlainText().length());
    aux.close();
    delete highlighter;
    delete editor;
    reloadFiles();
  }
  } 
  
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd(MM);
  cubeGL->rotze=roz;
  cubeGL->updateGL();
}

void MyWindow::editXDmas(){
  QFile mas("xd.mas");
  mas.open(QIODevice::ReadOnly|QIODevice::Text);
  editor = new QTextEdit;
  search = new QLineEdit;
  search->setMinimumSize(QSize(150, 0));
  QHBoxLayout *slt=new QHBoxLayout();
  QLabel sela("Search:");
  QToolButton prev;
  prev.setIcon(QIcon(":/images/moveleft.png"));
  prev.setText("Previous");
  prev.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  prev.setAutoRaise(true);
  QToolButton next;
  next.setIcon(QIcon(":/images/moveright.png"));
  next.setText("Next");
  next.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  next.setAutoRaise(true);
  slt->setSpacing(6);
  slt->setMargin(0);
  slt->addWidget(&sela);
  slt->addWidget(search);
  slt->addWidget(&prev);
  slt->addWidget(&next);
  slt->addStretch(10);
  connect(search, SIGNAL(textChanged(const QString&)),this,SLOT(findText()));
  connect(&next, SIGNAL(clicked() ), this, SLOT(findNext()));
  connect(&prev, SIGNAL(clicked() ), this, SLOT(findPrev()));
  QFont font;
  font.setFamily("Courier");
  font.setFixedPitch(true);
  font.setPointSize(10);  
  editor->setFont(font);
  Highlighter *highlighter = new Highlighter(editor->document());
  editor->setPlainText(mas.readAll());
  mas.close();
  QDialog *xdmasdlg =new QDialog(this);
  xdmasdlg->setMinimumSize(900,300);  
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
  buttonBox->clearFocus ();
  buttonBox->setFocusPolicy(Qt::NoFocus);
  connect(buttonBox, SIGNAL(accepted()), xdmasdlg, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), xdmasdlg, SLOT(reject()));  
  QVBoxLayout sss;
  sss.addWidget(editor);
  sss.addLayout(slt);
  sss.addWidget(buttonBox);
  xdmasdlg->setLayout(&sss);
  xdmasdlg->setWindowTitle("xd.mas editor");
  search->setFocus();
  if(QDialog::Accepted==xdmasdlg->exec()){
    mas.open(QIODevice::WriteOnly|QIODevice::Text);
    mas.write(editor->toPlainText().toLatin1(),editor->toPlainText().length());
    mas.close();
//    delete editor;
  }else
    if (editor->document()->isModified()){
  if ((QMessageBox::question(this,"Save changes to xd.mas?","Save changes to xd.mas?",QMessageBox::Save|QMessageBox::Discard)==QMessageBox::Save)){
    mas.open(QIODevice::WriteOnly|QIODevice::Text);
    mas.write(editor->toPlainText().toLatin1(),editor->toPlainText().length());
    mas.close();
  //  delete editor;
  }
  } 
  delete highlighter;
  delete editor;
}

void MyWindow::findNext(){
  searchchanged(false,false);
}

void MyWindow::findPrev(){
  searchchanged(false,true);
}

void MyWindow::findText(){
  searchchanged(true,false);
}

void MyWindow::searchchanged(bool current,bool back){
  QString searchString = search->text();
  QPalette p = search->palette();
  p.setBrush(QPalette::Active,QPalette::Base,Qt::white);
  QTextDocument *document = editor->document();
  QTextCursor cursor = editor->textCursor();
  if (!searchString.isEmpty()){
    if (cursor.hasSelection())
      cursor.setPosition((current|back)?cursor.anchor():cursor.position(), QTextCursor::MoveAnchor);
    QTextDocument::FindFlags options;
    if (back) options|=QTextDocument::FindBackward;
    cursor=document->find(searchString,cursor,options);
    if (cursor.isNull()){
      QTextCursor ac(document);
      ac.movePosition(options & QTextDocument::FindBackward
		      ? QTextCursor::End : QTextCursor::Start);
      cursor = document->find(searchString, ac,options);
      if (cursor.isNull()){	  
	p.setBrush(QPalette::Active,QPalette::Base,QColor(205,100,100));
      }
    }      
  }
  else cursor.movePosition(QTextCursor::Start);
  editor->setTextCursor(cursor);
  search->setPalette(p);
}

void MyWindow::reloadXDfiles(){
  someThingToRestore();
  loadFile(QDir::currentPath()+"/xd.mas",mol.gd);
  updateLabel();
}

void MyWindow::reloadFiles(){
  loadFile(dirName,mol.gd);
}

void MyWindow::restoreXDfiles(){
  QDir work=QDir(QDir::current());  
  QStringList filter;
  filter << "*res_MCQsave*";  
  QStringList resfs,masfs;
  resfs=work.entryList(filter,QDir::Files,QDir::Time); 
  filter.clear();
  filter << "*mas_MCQsave*";  
  masfs=work.entryList(filter,QDir::Files,QDir::Time); 
  if (masfs.size()){
    QString comps=masfs.at(0);
    QString masnam=masfs.at(0),resnam=resfs.at(0);
    comps.remove(0,6);
    if (resfs.at(0).endsWith(comps)) {
      QFile resf(resnam);
      QFile masf(masnam);
      QFile::remove("xd.mas"); 
      QFile::remove("xd.res");       
      resf.copy("xd.res");
      masf.copy("xd.mas");
      resf.remove();
      masf.remove();
      loadFile(QDir::currentPath()+"/xd.res",mol.gd);
    }
  }  
  someThingToRestore();
}

void MyWindow::showADPvalues(){

  int ind=0;
  QAction *action = qobject_cast<QAction *>(sender());
  if (action!=NULL){
      ind=action->data().toInt();
      ind=qMax(ind,0);
      ind=qMin(ind,xdinp.size()-1);
  }
QMessageBox::information(this,xdinp.at(ind).atomname,QString("U11 = %1 U22 = %2 U33 = %3\nU23 = %4 U13 = %5 U12 = %6\nU11c = %7 U22c = %8 U33c = %9\nU23c = %10 U13c = %11 U12c = %12\n%13\n%14")
                         .arg(xdinp.at(ind).uf.m11)
                         .arg(xdinp.at(ind).uf.m22)
                         .arg(xdinp.at(ind).uf.m33)
                         .arg(xdinp.at(ind).uf.m23)
                         .arg(xdinp.at(ind).uf.m13)
                         .arg(xdinp.at(ind).uf.m12)

                         .arg(xdinp.at(ind).u.m11)
                         .arg(xdinp.at(ind).u.m22)
                         .arg(xdinp.at(ind).u.m33)
                         .arg(xdinp.at(ind).u.m23)
                         .arg(xdinp.at(ind).u.m13)
                         .arg(xdinp.at(ind).u.m12)
.arg((xdinp.at(ind).jtf>2)?QString("c111 = %1 c112 = %2 c113 = %3 c122 = %4 c123 = %5\nc133 = %6 c222 = %7 c223 = %8 c233 = %9 c333 =%10")
                         .arg(xdinp.at(ind).c111)
                         .arg(xdinp.at(ind).c112)
                         .arg(xdinp.at(ind).c113)
                         .arg(xdinp.at(ind).c122)
                         .arg(xdinp.at(ind).c123)
                         .arg(xdinp.at(ind).c133)
                         .arg(xdinp.at(ind).c222)
                         .arg(xdinp.at(ind).c223)
                         .arg(xdinp.at(ind).c233)
                         .arg(xdinp.at(ind).c333)
                         :"")
.arg((xdinp.at(ind).jtf>3)?QString("d1111 = %1 d1112 = %2 d1113 = %3 d1122 = %4 d1123 = %5\nd1133 = %6 d1222 = %7 d1223 = %8 d1233 = %9 d1333 = %10\nd2222 = %11 d2223 = %12 d2233 = %13 d2333 = %14 d3333 = %15 ")
                          .arg(xdinp.at(ind).d1111)
                          .arg(xdinp.at(ind).d1112)
                          .arg(xdinp.at(ind).d1113)
                          .arg(xdinp.at(ind).d1122)
                          .arg(xdinp.at(ind).d1123)
                          .arg(xdinp.at(ind).d1133)
                          .arg(xdinp.at(ind).d1222)
                          .arg(xdinp.at(ind).d1223)
                          .arg(xdinp.at(ind).d1233)
                          .arg(xdinp.at(ind).d1333)
                          .arg(xdinp.at(ind).d2222)
                          .arg(xdinp.at(ind).d2223)
                          .arg(xdinp.at(ind).d2233)
                          .arg(xdinp.at(ind).d2333)
                          .arg(xdinp.at(ind).d3333)
                        :" ")
                                  );
}

void MyWindow::about(){  
#ifdef __x86_64__
  QString has64bit=QString("64 bit Version");
#else
  QString has64bit=QString("32 bit Version");
#endif
  QString openGLVersion=QString("OpenGL Version %1").arg((char *)glGetString(GL_VERSION));

  QMessageBox::about(this,tr("About MoleCoolQt"),
          tr("<b>MoleCoolQt is written by Dr. Christian B. H&uuml;bschle, University of G&ouml;ttingen, Germany; now: University of Bayreuth</b><p>"
		 "<a href=\"http://www.molecoolqt.de\">MoleCoolQt site</a><p>"
		 "If you find bugs, typos or have any suggestions then contact me under <a href=\"mailto:chuebsch@moliso.de\">chuebsch@moliso.de</a>"
                 "<p> This is Revision %1 <br> The Version of Qt used is: %2.<br>%3<p>%4")
                     .arg(rev).arg(qVersion ()).arg(openGLVersion).arg(has64bit));

}

void MyWindow::idleMoveStart(){
  ydlStopAct->setEnabled(true);
  ydlStartAct->setEnabled(false);
  speedSldr->show();
  idl =new QTimer(this);
  connect( idl, SIGNAL(timeout()),
	   this, SLOT(rotRight()) );
  idl->start(3);

}

void MyWindow::tMovieStart(){
  tMovieStartAct->setEnabled(false);
  tMovieStopAct->setEnabled(true);
//  speedSldr->show();
  tidl = new QTimer(this);
  connect( tidl, SIGNAL(timeout()),
	   this, SLOT(incT()) );
  tidl->start((int)cubeGL->twait);
}

void MyWindow::incT(){
  if (mol.ccc.isEmpty()) {
    cubeGL->tvalue=fmod(cubeGL->tvalue+cubeGL->tstep,1.0);
    statusBar()->showMessage(QString("t0 = %1").arg(cubeGL->tvalue));
  }else{
    cubeGL->tvalue+=cubeGL->tstep;
    cubeGL->tvalue=fmod(cubeGL->tvalue,1.0);
    if (mol.ccc.size()==1) statusBar()->showMessage(QString("t1  = %1  t2 = %2").arg(cubeGL->tvalue).arg(cubeGL->tvalue*mol.ccc.at(0).tfactor));
    if (mol.ccc.size()==2) statusBar()->showMessage(QString("t1  = %1  t2 = %2  t3 = %3")
      .arg(cubeGL->tvalue)
      .arg(cubeGL->tvalue*mol.ccc.at(0).tfactor)
      .arg(cubeGL->tvalue*mol.ccc.at(1).tfactor));
      }
//  speedSldr->setValue((int)(cubeGL->tvalue*128));

  bool keepInCell=false;
//  qDebug()<<matoms.size();
  if (keepInCell){
    for  (int i=0; i<matoms.size();i++){
      double occ=matoms[i].occupancy(cubeGL->tvalue);
      if (occ<0.1) continue;
      V3 frc=matoms[i].frac(cubeGL->tvalue);
      V3 lfoor=floor3(frc);
      lfoor*=-1.0;
      V3 nv=V3(0,0,0);
      if (nv==lfoor);else{
        Modulat m=matoms[i];
        V3 f,f1;
        do {
        m=matoms[i].applySymm(identity,lfoor,matoms[i].x4sym,matoms[i].x4,matoms[i].x4trans);
        f1=m.frac(cubeGL->tvalue);
        f=floor3(f1);
        lfoor=lfoor-f;
        printf("%f %f %f  (%f %f %f) (%f %f %f)\n",f1.x,f1.y,f1.z,f.x,f.y,f.z,lfoor.x,lfoor.y,lfoor.z); 
        }while ((nv!=f)&&(nv!=lfoor));
        matoms.replace(i,m); 
        mol.bonds_made=false;
        V3 nfrc=matoms[i].frac(cubeGL->tvalue);
        printf("%f %f %f => %f %f %f  (%f %f %f)\n",frc.x,frc.y,frc.z,nfrc.x,nfrc.y,nfrc.z,lfoor.x,lfoor.y,lfoor.z); 
      }
    }  
  }
  cubeGL->emitnewt();
  cubeGL->updateGL(); 
}

void MyWindow::tMovieStop(){
  tMovieStartAct->setEnabled(true);
  tMovieStopAct->setEnabled(false);
//  speedSldr->hide();
  if (tidl!=NULL)tidl->stop();
  infoKanalNews(QString("t0-movie stopped at t0 = %1").arg(cubeGL->tvalue));
  delete tidl;
  tidl=NULL;

}

void MyWindow::wasistdas(){  
  QWhatsThis::enterWhatsThisMode();

}

void MyWindow::togglSt(){
  bool vis =statusBar()->isVisible();
  if (vis) statusBar()->hide();
  else statusBar()->show();
  update();
}

void MyWindow::idleMoveStop(){  
  ydlStopAct->setEnabled(false);
  ydlStartAct->setEnabled(true);
  speedSldr->hide();
  idl->stop();
  delete idl;
}

void MyWindow::changeScalePic(){
  bool ok;
  double d=QInputDialog::getDouble(this,"Set a scale factor for sreen shot resolution","Scale factor",scalePic,0.1,4.0,2,&ok);
  if (ok) scalePic=d;
  act1->setStatusTip(QString(tr("Saves a screen shot in %1 fold screen resolution").arg(scalePic)));
}

void MyWindow::changeViewAngle(){
  bool ok;
  double d=QInputDialog::getDouble(this,"Set perspective view angle", "View angle ",cubeGL->viewAngle(),0.1,160.0,2,&ok);
  if (ok) {
  cubeGL->setViewAngle(d);
  }    
}

void MyWindow::changeADP(){
  bool ok;
  int ad,d=50;
  QAction *action = qobject_cast<QAction *>(sender());
  if (action) d=action->data().toInt();
#if (QT_VERSION >= 0x040500)
  if (d==0) d=QInputDialog::getInt(this, "Set ADP probability",tr("Percentage:"), mol.proba, 10, 90, 20, &ok);
#else
  if (d==0) d=QInputDialog::getInteger(this, "Set ADP probability",tr("Percentage:"), mol.proba, 10, 90, 20, &ok);
#endif
  else mol.proba=d;
  if (ok) {
    if ((d==10)||(d==30)||(d==50)||(d==70)||(d==90))
    mol.proba=d;
    else qDebug()<<"The value of ADP probability you have chosen is not allowed, please select 10, 30, 50, 70 or 90 %. Thank you!";
  }
  if (cubeGL->bas&&atmax){
    ad=mol.adp;
    glDeleteLists(cubeGL->bas,1);
    glNewList(cubeGL->bas, GL_COMPILE );{                          //ATOME
      glPushMatrix();{
	glScaled(cubeGL-> L,cubeGL-> L,cubeGL-> L );
	mol.adp=1;
	mol.intern=1;
	mol.atoms(xdinp,mol.proba);
      }glPopMatrix();    
    }glEndList(); 

    glDeleteLists(cubeGL->bas+7,1);
    glNewList(cubeGL->bas+7, GL_COMPILE );{                          //ATOME
      glPushMatrix();{
	glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
	mol.adp=1;
	mol.intern=0;
	mol.atoms(xdinp,mol.proba);
      }glPopMatrix();    
    }glEndList();  
    mol.adp=ad;
    update();
  }
  cubeGL->updateGL();
}

void MyWindow::changeBondStrength(){
bool ok;
double d=mol.bondStrength*100.0;
d=QInputDialog::getDouble(this,"Change Bond Strength","thicknes in pm:" ,mol.bondStrength*100.0,1.00,90,4,&ok);
if (ok) {mol.bondStrength=d/100.0;
 if (!xdinp.isEmpty()) initLists(xdinp);
update();
}
}

void MyWindow::saveScene(){
  glGetDoublev(GL_MODELVIEW_MATRIX,cubeGL->MM);

  QList<QByteArray> supo = QImageWriter::supportedImageFormats ();
  QString formats;
  QString selectedFilter;
  for (int i=0; i<supo.size(); i++)
    if ((!QString(supo.at(i)).contains("ps",Qt::CaseInsensitive)) &&
		    (!QString(supo.at(i)).contains(QRegExp("[A-Z]")))){
      QString str = QString("%1-file (*.%1)%2").arg(QString(supo.at(i))).arg(((i+1)<supo.size())?";;":"");
      formats+=str;
      QString endung=saveName;
      int  qq= endung.lastIndexOf(".");
      if (qq>0) {
        qq=endung.length()- qq;
        endung=endung.right(qq);
        if (str.contains(endung)){
        selectedFilter=QString("%1-file (*.%1)").arg(QString(supo.at(i)));
        }
      }
      
    }
  QString fileName = QFileDialog::getSaveFileName(this,
  QString(tr("Save a screen shot in %1 fold screen resolution").arg(scalePic)), saveName,formats,&selectedFilter,QFileDialog::DontUseNativeDialog );
/*						  "Joint Photographic Experts Group - file (*.jpg);;"
						  "Windows Bitmap - file (*.bmp);;"
						  "Portable Network Graphics - file (*.png);;"
                                                  "Portable Pixmap - file (*.ppm)" );*/
  int 
    a = int(cubeGL->_win_width*scalePic+0.5),
    b = int(cubeGL->_win_height*scalePic+0.5),
    c = int(cubeGL->myFont.pointSize ()*scalePic+0.5),
    g = int(cubeGL->MLegendFont.pointSize ()*scalePic+0.5),
    d = cubeGL->_win_width,
    e = cubeGL->_win_height,
    f = cubeGL->myFont.pointSize (),
    h = cubeGL->MLegendFont.pointSize ();
  cubeGL->scalePicNow=scalePic;
  if (!fileName.isEmpty()){
    QString endung=selectedFilter.section('*',1,1).section(')',0,0);
    if (!fileName.endsWith(endung)) fileName.append(endung);
    printf("!%s! %s\n",endung.toStdString().c_str(),fileName.toStdString().c_str());fflush(stdout);
    cubeGL->myFont.setPointSize(c);
    cubeGL->MLegendFont.setPointSize(g);
    cubeGL->noWaitLabel=true;
    cubeGL->paparazi=true;
    QPixmap   map = cubeGL->renderPixmap(a,b);
    cubeGL->paparazi=false;
    map.save(fileName);/*
    if (fileName.endsWith(".bmp")) map.save(fileName,"BMP",-1);
    if (fileName.endsWith(".png")) map.save(fileName,"PNG",80);
    if (fileName.endsWith(".ppm")) map.save(fileName,"PPM",-1);
    if (fileName.endsWith(".jpg")) map.save(fileName,"JPG",80);  */
    saveName=fileName;
  }
  cubeGL->myFont.setPointSize(f);
  cubeGL->MLegendFont.setPointSize(h);
  cubeGL->_win_width=d;
  cubeGL->_win_height=e;
  cubeGL->noWaitLabel=cubeGL->quickRot->isChecked();

  cubeGL->scalePicNow=1.0;
  cubeGL->updateGL();
}

/*
void MyWindow::printScene(){
  glGetDoublev(GL_MODELVIEW_MATRIX,cubeGL->MM);
  QPrinter printer(QPrinter::HighResolution);
  QPrintDialog pd(&printer,this);
  if (pd.exec()==QDialog::Accepted){
    QRect qr=printer.pageRect ();
    double XS = (double)qr.width()/cubeGL->_win_width;
    double YS = (double)qr.height()/cubeGL->_win_height;  
    XS=(XS>YS)?YS:XS;
    int 
      a=int(cubeGL->_win_width*XS+0.5),
      b=int(cubeGL->_win_height*XS+0.5),
      c=int(cubeGL->myFont.pointSize ()*XS+0.5),
      d=cubeGL->_win_width,
      e=cubeGL->_win_height,
      f=cubeGL->myFont.pointSize ();
    cubeGL->myFont.setPointSize(c);
    QPixmap   map=cubeGL->renderPixmap(a,b);  
    QPainter painter(&printer);
    painter.drawPixmap(0,0,map);  
    cubeGL->myFont.setPointSize(f);
    cubeGL->_win_width=d;
    cubeGL->_win_height=e;
    cubeGL->updateGL();
  }
}
*/

double &Cjkl(INP &a,int j, int k, int l){
int pro=j*k*l;
switch (pro){
case 1:return a.c111;
case 2:return a.c112;
case 3:return a.c113;
case 4:return a.c122;
case 6:return a.c123;
case 8:return a.c222;
case 9:return a.c133;
case 12:return a.c223;
case 18:return a.c233;
case 27:return a.c333;
}
return a.c111; //this will never happen!
}

double &Djklm(INP &a,int j, int k, int l, int m){
  /*
   m jklm  product 
   1 1111  1
   4 1112  2
   4 1113  3
   6 1122  4
  12 1123  6
   4 1222  8
   6 1223  9
  12 1223 12
   1 2222 16
  12 1233 18
   4 2223 24
   4 1333 27
   6 2233 36
   4 2333 54
   1 3333 81
   * */
int pro=j*k*l*m;
switch (pro){
case 1 :return a.d1111;
case 2 :return a.d1112;
case 3 :return a.d1113;
case 4 :return a.d1122;
case 6 :return a.d1123;
case 8 :return a.d1222;
case 9 :return a.d1223;
case 12:return a.d1223;
case 16:return a.d2222;
case 18:return a.d1233;
case 24:return a.d2223;
case 27:return a.d1333;
case 36:return a.d2233;
case 54:return a.d2333;
case 81:return a.d3333;
}
return a.d1111; //this will never happen!
}

void tensmul(INP &atom){
  if (atom.jtf<3) return;
  int flag[3][3][3]={
    {{1,1,1},{0,1,1},{0,0,1}},
    {{0,0,0},{0,1,1},{0,0,1}},
    {{0,0,0},{0,0,0},{0,0,1}}};
                     
  double t[3][3][3]; 
  for (int u=0; u<3;u++){
    for (int v=0; v<3;v++){
      for (int w=0; w<3;w++){
	//if (flag[u][v][w]) {
	  t[u][v][w]=0;
	  for (int i=0; i<3;i++){
	    for (int j=0; j<3;j++){
	      for (int k=0; k<3;k++){
		t[u][v][w]+=Cjkl(atom,i+1,j+1,k+1)*mol.zelle.o[i][u]*mol.zelle.o[j][v]*mol.zelle.o[k][w];
	      }
	    }
	  }
	//}
      }
    }
  }
  for (int u=0; u<3;u++){
    for (int v=0; v<3;v++){
      for (int w=0; w<3;w++){
	if (flag[u][v][w]) {
	  Cjkl(atom,u+1,v+1,w+1)=t[u][v][w];
	}
      }
    }
  }	      
	      
  if (atom.jtf<4)return;
  double s[3][3][3][3];
  int flag2[3][3][3][3]={
    {
      {{1,1,1},{0,1,1},{0,0,1}},
      {{0,0,0},{0,1,1},{0,0,1}},
      {{0,0,0},{0,0,0},{0,0,1}}},
    {
      {{0,0,0},{0,0,0},{0,0,0}},
      {{0,0,0},{0,1,1},{0,0,1}},
      {{0,0,0},{0,0,0},{0,0,1}}},
   {
      {{0,0,0},{0,0,0},{0,0,0}},
      {{0,0,0},{0,0,0},{0,0,0}},
      {{0,0,0},{0,0,0},{0,0,1}}}
  };
                     
  for (int u=0; u<3;u++){
    for (int v=0; v<3;v++){
      for (int w=0; w<3;w++){
	for (int q=0; q<3;q++){
	  if (flag2[u][v][w][q]){
	    s[u][v][w][q]=0;
	    for (int i=0; i<3;i++){
	      for (int j=0; j<3;j++){
		for (int k=0; k<3;k++){
		  for (int l=0; l<3;l++){
		    s[u][v][w][q]+=Djklm(atom,i+1,j+1,k+1,l+1)*mol.zelle.o[i][u]*mol.zelle.o[j][v]*mol.zelle.o[k][w]*mol.zelle.o[l][q];
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
  for (int u=0; u<3;u++){
    for (int v=0; v<3;v++){
      for (int w=0; w<3;w++){
	for (int q=0; q<3;q++){
	  if (flag2[u][v][w][q]){
	    Djklm(atom,u+1,v+1,w+1,q+1)=s[u][v][w][q];
	  }
	}
      }
    }
  }
}

void tensmul(INP &atom,Matrix om){
  double o[3][3]={
    {om.m11,om.m12,om.m13},
    {om.m21,om.m22,om.m23},
    {om.m31,om.m32,om.m33}};

  if (atom.jtf<3) return;
  int flag[3][3][3]={
    {{1,1,1},{0,1,1},{0,0,1}},
    {{0,0,0},{0,1,1},{0,0,1}},
    {{0,0,0},{0,0,0},{0,0,1}}};
             printf("mat\n%f %f %f\n%f %f %f\n%f %f %f\n"
			     ,o[0][0],o[0][1],o[0][2]        
			     ,o[1][0],o[1][1],o[1][2]        
			     ,o[2][0],o[2][1],o[2][2]);        
  double t[3][3][3]; 
  for (int u=0; u<3;u++){
    for (int v=0; v<3;v++){
      for (int w=0; w<3;w++){
	if (flag[u][v][w]) {
	  t[u][v][w]=0;
	  for (int i=0; i<3;i++){
	    for (int j=0; j<3;j++){
	      for (int k=0; k<3;k++){
		t[u][v][w]+=Cjkl(atom,i+1,j+1,k+1)*o[i][u]*o[j][v]*o[k][w];
	      }
	    }
	  }
	}
      }
    }
  }
  for (int u=0; u<3;u++){
    for (int v=0; v<3;v++){
      for (int w=0; w<3;w++){
	if (flag[u][v][w]) {
	  Cjkl(atom,u+1,v+1,w+1)=t[u][v][w];
	}
      }
    }
  }	      
  if (atom.jtf<4)return;
  double s[3][3][3][3];
  int flag2[3][3][3][3]={
    {
      {{1,1,1},{0,1,1},{0,0,1}},
      {{0,0,0},{0,1,1},{0,0,1}},
      {{0,0,0},{0,0,0},{0,0,1}}},
    {
      {{0,0,0},{0,0,0},{0,0,0}},
      {{0,0,0},{0,1,1},{0,0,1}},
      {{0,0,0},{0,0,0},{0,0,1}}},
   {
      {{0,0,0},{0,0,0},{0,0,0}},
      {{0,0,0},{0,0,0},{0,0,0}},
      {{0,0,0},{0,0,0},{0,0,1}}}
  };
                     
  for (int u=0; u<3;u++){
    for (int v=0; v<3;v++){
      for (int w=0; w<3;w++){
	for (int q=0; q<3;q++){
	  if (flag2[u][v][w][q]){
	    s[u][v][w][q]=0;
	    for (int i=0; i<3;i++){
	      for (int j=0; j<3;j++){
		for (int k=0; k<3;k++){
		  for (int l=0; l<3;l++){
		    s[u][v][w][q]+=Djklm(atom,i+1,j+1,k+1,l+1)*o[i][u]*o[j][v]*o[k][w]*o[l][q];
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
  for (int u=0; u<3;u++){
    for (int v=0; v<3;v++){
      for (int w=0; w<3;w++){
	for (int q=0; q<3;q++){
	  if (flag2[u][v][w][q]){
	    Djklm(atom,u+1,v+1,w+1,q+1)=s[u][v][w][q];
	  }
	}
      }
    }
  }
}
/*
void Uf2Uo(const Matrix x, Matrix & y) {
  Matrix n=Matrix(mol.zelle.as,0,0,0,mol.zelle.bs,0,0,0,mol.zelle.cs);
  Matrix m=(n*x)*n;
  y=(mol.zelle.f2c*m)*transponse(mol.zelle.f2c);
}
*/
double MyWindow::pdf2(INP atom, V3 pos){
  double p=0;
  Matrix U=atom.u;
  double DI=1.0/determinant(U);
  Matrix UI=inverse(U);
  V3 X=pos-atom.kart;
  const double base=DI/8*M_PI*M_PI*M_PI;
  double ponent=-1.0*((X*UI)*X);
 // printf("PDF?? D%g D**(-1)%g x=%g \n",D,DI,ponent);
  p=sqrt(base)*exp(ponent/2.0);
  return p;
}

void Usym (Matrix x,Matrix sym, Matrix & y){
  y=(sym*x)*transponse(sym);
}
//double xs=0,ys=0,zs=0;//GLOBAL def
double MyWindow::dimension(QList<INP> xnp ){
  double max=0,gg=0;
  int nimanda=0;
  for (int i=0;i<xnp.size();i++)
    for (int j=i+1;j<xnp.size();j++){
      if ((xnp[j].OrdZahl>=0)&&(xnp[j].OrdZahl>=0)) {
	gg=Distance(xnp[i].kart,xnp[j].kart);
	nimanda++;
      }
      max=(max<gg)?gg:max;
    }
  if (!nimanda){ 
    for (int i=0;i<xnp.size();i++)
      for (int j=i+1;j<xnp.size();j++){
	nimanda++;
	gg=Distance(xnp[i].kart,xnp[j].kart);
	max=(max<gg)?gg:max;
      }
  }
  if (!nimanda) return 10.0;
  return sqrt(max);
}

double MyWindow::mdimension(QList<Modulat> xnp ){
  double max=0,gg=0;
  int nimanda=0;
//  qDebug()<<"mdimension..."<<xnp.size();
  for (int i=0;i<xnp.size();i++)
    for (int j=i+1;j<xnp.size();j++){
      if ((xnp.at(j).OrdZahl>=0)&&(xnp.at(j).OrdZahl>=0)) {
	gg=Distance(xnp[i].kart(cubeGL->tvalue),xnp[j].kart(cubeGL->tvalue));
//        V3 p=xnp[i].kart(cubeGL->tvalue);
    //    printf("%f %f %f %f \n",gg,p.x,p.y,p.z );
	nimanda++;
      }
      max=(max<gg)?gg:max;
    }
  if (!nimanda){ 
    for (int i=0;i<xnp.size();i++)
      for (int j=i+1;j<xnp.size();j++){
	nimanda++;
	gg=Distance(xnp[i].kart(cubeGL->tvalue),xnp[j].kart(cubeGL->tvalue));
	max=(max<gg)?gg:max;
      }
  }
  if (!nimanda) return 10.0;
//  printf("maxlen %g\n",max);
  return sqrt(max);
}
void MyWindow::editAtomColor(){
  extern molekul mol;
  EacDlg eac(&mol);
  eac.exec();
    mol.bonds_made=0;
    mol.knopf_made=0;
    cubeGL->resetENV();
    if (!xdinp.isEmpty())initLists(xdinp);
    cubeGL->updateGL();
    //reloadFiles();

}

void MyWindow::il(){
  if (!xdinp.isEmpty())initLists(xdinp);
  cubeGL->updateGL();
}
void MyWindow::rezi(){
  extern molekul mol;
  EwaldSphereDlg e(dirName,&mol);
  e.exec();
}

void MyWindow::load_fchk(QString fileName){
  asymmUnit.clear();
  cubeGL->setVisible ( false );
  char PSE_Symbol[109][3] = {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar",
			     "K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr",
                 "Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe",
			     "Cs","Ba", "La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu",
			     "Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra",
			     "Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No","Lr","Ku","Ha","Rf","Ns","Hs","Mt"};
  QFile fchk(fileName);
  mol.initDir();
  fchk.open(QIODevice::ReadOnly|QIODevice::Text);
  QString line;
  QStringList tok;
  INP newAtom;
  newAtom.part=0;
  newAtom.sg=0;
  bool fertig=false;
  line=QString(fchk.readLine(90));
  strncpy(CID,line.toStdString().c_str(),79);
//  printf("CID %s\n",CID);
  while ((!fchk.atEnd())&&(!fertig)) {
    line=QString(fchk.readLine(90));
//    printf(" line ||%s||\n",line.toStdString().c_str());
    if (line.contains("Atomic numbers")) {
      atmax=(line.split(" ",QString::SkipEmptyParts).at(4)).toInt();
      for (int i=0; i<atmax;){
	line=QString(fchk.readLine(90));
	tok=line.split(" ",QString::SkipEmptyParts);
	for (int j=0;j<tok.size();j++) {
	  newAtom.OrdZahl=tok.at(j).toInt()-1;
	  sprintf(newAtom.atomname,"%s%d",PSE_Symbol[newAtom.OrdZahl],i+1);
	  asymmUnit.append(newAtom);
	  i++;
	}
      }
    }
    if (line.contains("Dipole Moment")){
      line=QString(fchk.readLine(90));
      QStringList zeiger=line.split(QRegExp("[^\\-\\.0-9]+"),QString::SkipEmptyParts);
      V3 dipl=V3(zeiger.at(0).toDouble(),zeiger.at(1).toDouble(),zeiger.at(2).toDouble());
      V3 org=V3(666.0,666.0,666.0);
      cubeGL->pole.append(dipl);
      cubeGL->poleOrg.append(org);
      fertig=true;
    }
    if (line.contains("Current cartesian coordinates")) {
      for (int i=0; i<atmax*3;){
	line=QString(fchk.readLine(90));
	tok=line.split(" ",QString::SkipEmptyParts);
	for (int j=0;j<tok.size();j++) {
	  switch (i%3){
	  case 0: asymmUnit[i/3].kart.x=tok.at(j).toDouble();break;
	  case 1: asymmUnit[i/3].kart.y=tok.at(j).toDouble();break;
	  case 2: asymmUnit[i/3].kart.z=tok.at(j).toDouble();break;
	  }	
	  i++;
	}
      }
    }
  }
    if ((cubeGL->pole.isEmpty())){

      nodipAct->setVisible ( false);
      ldipAct->setVisible ( true);
    }else{
      QString content="";
      for (int i=0; i<cubeGL->pole.size(); i++){ 
	if (cubeGL->farben.size()<=i) cubeGL->farben.append(QColor(QColor::colorNames().at(i)));
	content.append(QString("<tr><td><font color=\"%5\">Dipole_%1</font></td><td>%2</td><td>%3</td><td>%4</td></tr>")
			.arg(i).arg(cubeGL->pole.at(i).x,16,'f',4).arg(cubeGL->pole.at(i).y,16,'f',4).arg(cubeGL->pole.at(i).z,16,'f',4).arg(cubeGL->farben.at(i).name()));
      }
      infoKanalNews(QString("Dipole Moments:<table><tr><th>Dipole</th><th>X</th><th>Y</th><th>Z</th><tr>%1</table>").arg(content));
      nodipAct->setVisible ( true);
      ldipAct->setVisible ( false);
    }
  smx=atmax;
  dummax=0;
  for (int i=0; i<atmax;i++){
    if (asymmUnit[i].OrdZahl!=-1){
      asymmUnit[i].kart.x*=0.529177;
      asymmUnit[i].kart.y*=0.529177;
      asymmUnit[i].kart.z*=0.529177;
      asymmUnit[i].frac=asymmUnit[i].kart;
      //xs+=asymmUnit[i].kart.x;
      //ys+=asymmUnit[i].kart.y;
      //zs+=asymmUnit[i].kart.z;
    }
  }
  /*xs/=(smx);ys/=(smx);zs/=(smx);
  for (int i=0; i<smx; i++){
    asymmUnit[i].kart.x-=xs;
    asymmUnit[i].kart.y-=ys;
    asymmUnit[i].kart.z-=zs;  
  }*/
//  packAct->setVisible(false);
  mol.zelle.a=1.0;
  mol.zelle.b=1.0;
  mol.zelle.c=1.0;
  mol.zelle.al=90.0;
  mol.zelle.be=90.0;
  mol.zelle.ga=90.0;
  mol.adp=0;
  xdinp=asymmUnit;
  double dim=dimension(xdinp);
  if ((Norm(atom1Pos)==0)&&(Norm(atom2Pos)==0)) cubeGL->L=100.0/dim;
  /*if (mol.nListe>2) {
    free(mol.vL);
    mol.vL=NULL;
    mol.nListe=0;
  }*/
  cubeGL->resetENV();
  
  cubeGL->specialFragment1=true;
  mol.countMols(xdinp);
  initLists(xdinp);
  cubeGL->setVisible ( true );
}

void MyWindow::load_MoPro(QString fileName) {
    INP newAtom;
    QList<INP> dummys;
    newAtom.part=0;
    newAtom.sg=0;
    someThingToRestore();
    george=false;
    strcpy(CID,"MoProFile");
    xdSetupAct->setEnabled(false);
    xdMenu->setEnabled(false);
    mol.zelle.symmops.clear();
    mol.zelle.trans.clear();
    seReAct->setEnabled(true);
    seReAct->setVisible(true);
    QFile mpf(fileName);
    QStringList axstr,ato1,ato2,ato3;
    QList<int> resNr;
//    bool habzell=false;
    if (mpf.open(QIODevice::ReadOnly)){
        QStringList tok;
        QStringList all = QString(mpf.readAll()).split("\n",QString::SkipEmptyParts);
        mpf.close();
        for (int li=0; li<all.size();li++){
            if (all.at(li).contains("!")) all[li]=all.at(li).split("!",QString::SkipEmptyParts).at(0);
            all[li]=all.at(li).trimmed();
            if (all.at(li).isEmpty()) all.removeAt(li);
        }
        for (int li=0; li<all.size();li++){
            tok.clear();
            tok=all.at(li).split(" ",QString::SkipEmptyParts);
            if (tok.size()){
                if ((tok.size()>7)&&(tok.at(0).toUpper()=="CELL")) {
                    mol.zelle.a  = tok.at(1).toDouble();
                    mol.zelle.b  = tok.at(2).toDouble();
                    mol.zelle.c  = tok.at(3).toDouble();
                    mol.zelle.al = tok.at(4).toDouble();
                    mol.zelle.be = tok.at(5).toDouble();
                    mol.zelle.ga = tok.at(6).toDouble();
                    mol.zelle.lambda=tok.at(7).toDouble();
                    //habzell=true;
                    setup_zelle();
                }
                if ((tok.size()>2 )&&(tok.at(0).toUpper()=="SYMM")){
                    int nsym = tok.at(1).toInt();
                    char gitt;
                    gitt=tok.at(2).toLatin1()[0];
                    for (int k=li+1; k<(li+1+nsym); k++){
                        mol.decodeSymmCard(all.at(k));
                    }
                    li+=nsym;
                    mol.applyLatticeCentro(gitt,((tok.size()>3)&&(tok.at(3).toUpper().startsWith("CENTRO"))));
                }
                if ((tok.size()> 1)&&(tok.at(0).toUpper()=="DUMMY")){
                    dummax = tok.at(1).toInt();
                    for (int k=li+1; k<(li+1+dummax); k++){
                        if (all.at(k).startsWith("DUM")) {
                            tok.clear();
                            tok=all.at(k).split(" ",QString::SkipEmptyParts);
                            newAtom.frac.x=tok.at(1).toDouble();
                            newAtom.frac.y=tok.at(2).toDouble();
                            newAtom.frac.z=tok.at(3).toDouble();
                            if (tok.size()>3) strncpy(newAtom.atomname,tok.at(4).toStdString().c_str(),38);
                            newAtom.OrdZahl=-1;
                            dummys.append(newAtom);
                        } else {li++;k--;}
                    }

                }
                if ((tok.size()> 1)&&(tok.at(0).toUpper()=="KAPPA")){
                    int nz = tok.at(1).toInt();
                    li+=nz;
                }
                if ((tok.size()> 1)&&(tok.at(0).toUpper()=="ATOMS")){
                    atmax= tok.at(1).toInt();
                    for (int k=0;k<atmax;k++){
                        tok.clear();
                        tok=all.at(++li).split(" ",QString::SkipEmptyParts);
                        if ((tok.size()!=11)&&(tok.at(0)!="ATOM")) {
                            qDebug()<<"Read Error in MoPro file!"<<tok.size()<<tok.at(0);
                            exit(33);
                        }
                        strncpy(newAtom.atomname,QString("%1@%2%3").arg(tok.at(2)).arg(tok.at(4)).arg(tok.at(3)).toStdString().c_str(),38);
                        newAtom.frac.x=tok.at(5).toDouble();
                        newAtom.frac.y=tok.at(6).toDouble();
                        newAtom.frac.z=tok.at(7).toDouble();
			newAtom.amul=tok.at(8).toDouble();
			newAtom.imul=tok.at(9).toInt();
                        newAtom.nay1=tok.at(1).toInt();
                        resNr.append(tok.at(4).toInt());
                        maxResi=qMax(tok.at(4).toInt(),maxResi);
                        newAtom.OrdZahl=mol.Get_OZ(tok.at(10));
                        tok.clear();
                        tok=all.at(++li).split(" ",QString::SkipEmptyParts);
                        axstr.append(tok.at(0));
                        ato1.append(tok.at(1));
                        ato2.append(tok.at(2));
                        ato3.append(tok.at(3));//kommt nur bei 3ZX so vor
                        tok.clear();
                        tok=all.at(++li).split(" ",QString::SkipEmptyParts);
                        if ((tok.size()>6)&&(tok.at(0)=="UANI")) {
                            newAtom.uf.m11=tok.at(1).toDouble();
                            newAtom.uf.m22=tok.at(2).toDouble();
                            newAtom.uf.m33=tok.at(3).toDouble();
                            newAtom.uf.m12=newAtom.uf.m21=tok.at(4).toDouble();
                            newAtom.uf.m13=newAtom.uf.m31=tok.at(5).toDouble();
                            newAtom.uf.m23=newAtom.uf.m32=tok.at(6).toDouble();

                            mol.Uf2Uo(newAtom.uf,newAtom.u);/*
                            printf("%9f %9f %9f %9f %9f %9f\n",
                                   newAtom.u.m11,
                                   newAtom.u.m22,
                                   newAtom.u.m33,
                                   newAtom.u.m12,
                                   newAtom.u.m13,
                                   newAtom.u.m23);
                            printf("<%9f> <%9f> <%9f> <%9f> <%9f> <%9f>\n",
                                   newAtom.uf.m11,
                                   newAtom.uf.m22,
                                   newAtom.uf.m33,
                                   newAtom.uf.m12,
                                   newAtom.uf.m13,
                                   newAtom.uf.m23);// */
                        }else if ((tok.size()>1)&&(tok.at(0)=="UISO")) {
                            newAtom.uf.m11=newAtom.uf.m22=newAtom.uf.m33=tok.at(1).toDouble();
                            newAtom.uf.m12=newAtom.uf.m13=newAtom.uf.m23=0;
                            newAtom.uf.m21=newAtom.uf.m31=newAtom.uf.m32=0;
                            newAtom.u.m11=newAtom.u.m22=newAtom.u.m33=newAtom.uf.m11;
                            newAtom.u.m12=newAtom.u.m13=newAtom.u.m23=0;
                            newAtom.u.m21=newAtom.u.m31=newAtom.u.m32=0;
                        }
                        else {
                            newAtom.uf.m11=0.08;
                            newAtom.uf.m22=0.04;
                            newAtom.uf.m33=0.02;
                            newAtom.uf.m12=0;
                            newAtom.uf.m13=0;
                            newAtom.uf.m23=0;
                        }
                        asymmUnit.append(newAtom);
                        if (k+1<atmax) {
                            while (!all.at(li).startsWith("ATOM")) li++;
                            li--;
                        }
                    }
                }
                if ((tok.size()> 1)&&(tok.at(0).toUpper()=="")){}
                if ((tok.size()> 1)&&(tok.at(0).toUpper()=="")){}
            }
        }
    }
    for (int i=0; i<asymmUnit.size(); i++) {
    int gendum=0;
    if ((axstr.at(i).startsWith("-"))) asymmUnit[i].lflag=-1;else asymmUnit[i].lflag= 1;
       if      ((axstr.at(i)=="XY") || (axstr.at(i)=="-XY"))  {asymmUnit[i].icor1=1; asymmUnit[i].icor2=2;}
       else if ((axstr.at(i)=="XZ") || (axstr.at(i)=="-XZ"))  {asymmUnit[i].icor1=1; asymmUnit[i].icor2=3;}
       else if ((axstr.at(i)=="YZ") || (axstr.at(i)=="-YZ"))  {asymmUnit[i].icor1=2; asymmUnit[i].icor2=3;}
       else if ((axstr.at(i)=="YX") || (axstr.at(i)=="-YX"))  {asymmUnit[i].icor1=2; asymmUnit[i].icor2=1;}
       else if ((axstr.at(i)=="ZX") || (axstr.at(i)=="-ZX"))  {asymmUnit[i].icor1=3; asymmUnit[i].icor2=1;}
       else if ((axstr.at(i)=="ZY") || (axstr.at(i)=="-ZY"))  {asymmUnit[i].icor1=3; asymmUnit[i].icor2=2;}
       else if ((axstr.at(i)=="bXY")|| (axstr.at(i)=="-bXY")) {asymmUnit[i].icor1=1; asymmUnit[i].icor2=2;gendum=1;}
       else if ((axstr.at(i)=="bXZ")|| (axstr.at(i)=="-bXZ")) {asymmUnit[i].icor1=1; asymmUnit[i].icor2=3;gendum=1;}
       else if ((axstr.at(i)=="bYZ")|| (axstr.at(i)=="-bYZ")) {asymmUnit[i].icor1=2; asymmUnit[i].icor2=1;gendum=1;}
       else if ((axstr.at(i)=="bZX")|| (axstr.at(i)=="-bZX")) {asymmUnit[i].icor1=3; asymmUnit[i].icor2=1;gendum=1;}
       else if ((axstr.at(i)=="bZY")|| (axstr.at(i)=="-bZY")) {asymmUnit[i].icor1=3; asymmUnit[i].icor2=2;gendum=1;}
       else if ((axstr.at(i)=="3Zb")|| (axstr.at(i)=="-3Zb")) {asymmUnit[i].icor1=3; asymmUnit[i].icor2=1;gendum=4;}
       else if ((axstr.at(i)=="3bZ")|| (axstr.at(i)=="-3bZ")) {asymmUnit[i].icor1=2; asymmUnit[i].icor2=1;gendum=5;}
       else if ((axstr.at(i)=="3ZX")|| (axstr.at(i)=="-3ZX")) {asymmUnit[i].icor1=3; asymmUnit[i].icor2=1;gendum=3;}
       else if ((axstr.at(i)=="3ZY")|| (axstr.at(i)=="-3ZY")) {asymmUnit[i].icor1=3; asymmUnit[i].icor2=2;gendum=3;}
       if (gendum>1){//wir haben eine dritte richtung
            if (ato3.at(i).contains(QRegExp("[A-Za-z]+"))){
             if (ato3.at(i).contains("_")){
               int rn=ato3.at(i).section("_",0,0).toInt();
               QString fit=QString("%1@").arg(ato3.at(i).section("_",1,1));
               for (int j=0;j<resNr.size();j++){
                asymmUnit[i].na3=j+1;
                if ((rn==resNr.at(j))&&(QString(asymmUnit.at(j).atomname).startsWith(fit))) break;
               }
             }else{
               for (int j=0;j<resNr.size();j++){
                 asymmUnit[i].na3=j+1;
                 if ((resNr.at(i)==resNr.at(j))&&(QString(asymmUnit.at(j).atomname).startsWith(QString("%1@").arg(ato3.at(i))))) break;
               }
             }
            }else {
             asymmUnit[i].na3=ato3.at(i).toInt();
            }
       }
       if (ato1.at(i).contains(QRegExp("[A-Za-z]+"))){
       if (ato1.at(i).contains("_")){
       int rn=ato1.at(i).section("_",0,0).toInt();
       QString fit=QString("%1@").arg(ato1.at(i).section("_",1,1));
       for (int j=0;j<resNr.size();j++){
           asymmUnit[i].nax=j+1;
           if ((rn==resNr.at(j))&&(QString(asymmUnit.at(j).atomname).startsWith(fit))) break;
          }
       }else{
       for (int j=0;j<resNr.size();j++){
           asymmUnit[i].nax=j+1;
           if ((resNr.at(i)==resNr.at(j))&&(QString(asymmUnit.at(j).atomname).startsWith(QString("%1@").arg(ato1.at(i))))) break;
          }

       }
       }else {
       asymmUnit[i].nax=ato1.at(i).toInt();
       }
       if (ato2.at(i).contains(QRegExp("[A-Za-z]+"))){
       if (ato2.at(i).contains("_")){
       int rn=ato2.at(i).section("_",0,0).toInt();
       QString fit=QString("%1@").arg(ato2.at(i).section("_",1,1));
       for (int j=0;j<resNr.size();j++){
           asymmUnit[i].nay2=j+1;
           if ((rn==resNr.at(j))&&(QString(asymmUnit.at(j).atomname).startsWith(fit))) break;
          }
       }else{
       for (int j=0;j<resNr.size();j++){
           asymmUnit[i].nay2=j+1;
           if ((resNr.at(i)==resNr.at(j))&&(QString(asymmUnit.at(j).atomname).startsWith(QString("%1@").arg(ato2.at(i))))) break;
          }

       }
       }else {
       asymmUnit[i].nay2=ato2.at(i).toInt();
       }
if (gendum==1){
V3 mitte,rechts,links;
mol.frac2kart(asymmUnit.at(asymmUnit.at(i).nax-1).frac,rechts);
mol.frac2kart(asymmUnit.at(asymmUnit.at(i).nay1-1).frac,mitte);
mol.frac2kart(asymmUnit.at(asymmUnit.at(i).nay2-1).frac,links);
newAtom.kart=mitte+0.5*Normalize(((rechts-mitte)+(links-mitte)));
mol.kart2frac(newAtom.kart,newAtom.frac);
newAtom.OrdZahl=-1;
strncpy(newAtom.atomname,QString("DUM%1!%2").arg(dummax).arg(axstr.at(i)).toStdString().c_str(),38);
asymmUnit[i].nax=dummax+atmax+1;
dummax++;
dummys.append(newAtom);
}
if (gendum==3){
//Wie bekmmt man jetzt raus welche drei atome benutzt werden sollen?
V3 mitte, arm1, arm2, arm3;
mol.frac2kart(asymmUnit.at(asymmUnit.at(i).nax-1).frac,arm1);
mol.frac2kart(asymmUnit.at(asymmUnit.at(i).nay1-1).frac,mitte);
mol.frac2kart(asymmUnit.at(asymmUnit.at(i).nay2-1).frac,arm2);
mol.frac2kart(asymmUnit.at(asymmUnit.at(i).na3-1).frac,arm3);
newAtom.kart=mitte+0.5*Normalize(((arm1-mitte)+(arm2-mitte)+(arm3-mitte)));
mol.kart2frac(newAtom.kart,newAtom.frac);
newAtom.OrdZahl=-1;
strncpy(newAtom.atomname,QString("DUM%1!%2").arg(dummax).arg(axstr.at(i)).toStdString().c_str(),38);
asymmUnit[i].nax=dummax+atmax+1;
dummax++;
dummys.append(newAtom);

}
if (gendum==4){
V3 mitte, arm2, arm3;
mol.frac2kart(asymmUnit.at(asymmUnit.at(i).nay2-1).frac,arm2);
mol.frac2kart(asymmUnit.at(asymmUnit.at(i).nay1-1).frac,mitte);
mol.frac2kart(asymmUnit.at(asymmUnit.at(i).na3-1).frac,arm3);
newAtom.kart=mitte+0.5*Normalize(((arm2-mitte)+(arm3-mitte)));
mol.kart2frac(newAtom.kart,newAtom.frac);
newAtom.OrdZahl=-1;
strncpy(newAtom.atomname,QString("DUM%1!%2").arg(dummax).arg(axstr.at(i)).toStdString().c_str(),38);
asymmUnit[i].nay2=dummax+atmax+1;
dummax++;
dummys.append(newAtom);

}
if (gendum==5){//3bZ
V3 mitte, arm1, arm2, arm3;
mol.frac2kart(asymmUnit.at(asymmUnit.at(i).nax-1).frac,arm1);
mol.frac2kart(asymmUnit.at(asymmUnit.at(i).nay1-1).frac,mitte);
mol.frac2kart(asymmUnit.at(asymmUnit.at(i).na3-1).frac,arm3);
newAtom.kart=mitte+0.5*Normalize(((arm1-mitte)+(arm3-mitte)));
mol.kart2frac(newAtom.kart,newAtom.frac);
newAtom.OrdZahl=-1;
strncpy(newAtom.atomname,QString("DUM%1!%2").arg(dummax).arg(axstr.at(i)).toStdString().c_str(),38);
asymmUnit[i].nax=dummax+atmax+1;
dummax++;
dummys.append(newAtom);

}
    }
    for (int i=0; i<dummys.size();i++)
        asymmUnit.append(dummys.at(i));
    smx=atmax+dummax;
    growSymm(6);
}

void MyWindow::load_Jana(QString fileName){
  mol.zelle.symmops.clear();
  mol.zelle.trans.clear();
  mol.ccc.clear();
  cubeGL->isModulated=false;
  sprintf(CID,"no title");
  QString fileBase,m40n,m50n,fouName;
  fileBase=fileName;
  mol.dimensions=3; 
  mol.ncomp=1; 
  if  (fileBase.contains(QRegExp(".m\\d\\d$"))) fileBase.chop(4);
  m40n=fileBase+(".m40");
  m50n=fileBase+(".m50");
  fouName=fileBase+(".m80");
  QFile m50(m50n);
  if (!m50.open(QIODevice::ReadOnly|QIODevice::Text)  ) {qDebug()<< "can't open "<<m50n; return;}
  QStringList tok;
  QStringList all = QString(m50.readAll()).split("\n",QString::SkipEmptyParts);
  m50.close();
  //printf("m50 read\n");
  char gitt='P'; 
  int phcnt=0;
  int lrot=1,nmol1=0,nmol2=0,nmol3=0;
  QList<int>  nAtm1, nPos1,nAtm2, nPos2,nAtm3, nPos3;
  JanaMolecule *currentMolekule=NULL;
  int ncomposite=1,ccmp=0;;
  QString Wmats[2];
  QStringList atypen;
  QList<V3>lavec;
  
  QList<double> lavec4;
  for (int li=0; li<all.size();li++){
    tok.clear();
    tok=all.at(li).split(" ",QString::SkipEmptyParts);
    if (tok.size()){
//      qDebug()<<tok.at(0).toUpper();
      if ((tok.size()>1)&&(tok.at(0).toUpper()=="PHASE")) {
        phcnt++;
        if (curentPhase != phcnt) continue;
        strcpy(CID,tok.at(1).toStdString().c_str());
      }
      if (((!phcnt)||((phcnt)&&(curentPhase==phcnt)))&&(tok.size()>1)&&(tok.at(0).toUpper()=="TITLE")) {
        QString cid=all.at(li);
        cid.remove("title",Qt::CaseInsensitive); 
        strcpy(CID,cid.toStdString().c_str());
      }
      if (((!phcnt)||((phcnt)&&(curentPhase==phcnt)))&&(tok.size()>6)&&(tok.at(0).toUpper()=="CELL")) {
        mol.zelle.a  = tok.at(1).toDouble();
        mol.zelle.b  = tok.at(2).toDouble();
        mol.zelle.c  = tok.at(3).toDouble();
        mol.zelle.al = tok.at(4).toDouble();
        mol.zelle.be = tok.at(5).toDouble();
        mol.zelle.ga = tok.at(6).toDouble();
  //      qDebug()<<mol.zelle.a<<mol.zelle.b<<mol.zelle.c<<mol.zelle.al<<mol.zelle.be<<mol.zelle.ga;
        //        mol.zelle.lambda=tok.at(7).toDouble();
        //habzell=true;
        setup_zelle();
      }
      if (((!phcnt)||((phcnt)&&(curentPhase==phcnt)))&&(tok.size()>1 )&&(tok.at(0).toUpper()=="LATTICE")){
        gitt=tok.at(1).toUpper()[0].toLatin1();
//        qDebug()<<"gitter "<<gitt;
      }
      if (((!phcnt)||((phcnt)&&(curentPhase==phcnt)))&&(tok.size()>3 )&&(tok.at(0).toUpper()=="LATTVEC")){
       V3 v=V3(0,0,0);
        v.x  = tok.at(1).toDouble();
        v.y  = tok.at(2).toDouble();
        v.z  = tok.at(3).toDouble();
        if (tok.size()>4) lavec4.append(tok.at(4).toDouble());
        lavec.append(v);
      }
      if (((!phcnt)||((phcnt)&&(curentPhase==phcnt)))&&(tok.size()>3 )&&(tok.at(0).toUpper()=="QR")){
        mol.zelle.qr.x=tok.at(1).toDouble();
        mol.zelle.qr.y=tok.at(2).toDouble();
        mol.zelle.qr.z=tok.at(3).toDouble();
      }
      if (((!phcnt)||((phcnt)&&(curentPhase==phcnt)))&&(tok.size()>3 )&&(tok.at(0).toUpper()=="QI")){
        mol.zelle.qi.x=tok.at(1).toDouble();
        mol.zelle.qi.y=tok.at(2).toDouble();
        mol.zelle.qi.z=tok.at(3).toDouble();
 }
      if (((!phcnt)||((phcnt)&&(curentPhase==phcnt)))&&(tok.size()>1 )&&(tok.at(0).toUpper()=="ATOM")){
        atypen.append(tok.at(1));
       // qDebug()<<"ATOMS"<<tok;
      }
      if (((!phcnt)||((phcnt)&&(curentPhase==phcnt)))&&(tok.size()>1 )&&(tok.at(0).toUpper()=="NDIM")){
          mol.dimensions=tok.at(1).toInt();
          if ((tok.size()>3 )&&(tok.at(2).toUpper()=="NCOMP"))
                ncomposite = mol.ncomp = tok.at(3).toInt();
                
          printf("Dimensions in m50: %d\nComposites: %d\n", mol.dimensions,mol.ncomp );
      }

      if (((!phcnt)||((phcnt)&&(curentPhase==phcnt)))&&(tok.size()>3 )&&(tok.at(0).toUpper()=="COMMEN")){
        mol.zelle.commensurate=true;
        mol.zelle.commen.x=tok.at(1).toInt();
        mol.zelle.commen.y=tok.at(2).toInt();
        mol.zelle.commen.z=tok.at(3).toInt();
 }
      if (((!phcnt)||((phcnt)&&(curentPhase==phcnt)))&&(tok.size()>1 )&&(tok.at(0).toUpper()=="TZERO")){
        cubeGL->tvalue=tok.at(1).toDouble();
       // qDebug()<<"ATOMS"<<tok;
      }
      if (((!phcnt)||((phcnt)&&(curentPhase==phcnt)))&&(mol.dimensions==4)&&(tok.at(0).toUpper()=="WMATRIX")){
        Wmats[ccmp++]=all.at(li+1)+all.at(li+2)+all.at(li+3)+all.at(li+4);
        li+=4;
//       qDebug()<<"Wnu"<< Wmats[ccmp-1]; 
      }
      if (((!phcnt)||((phcnt)&&(curentPhase==phcnt)))&&(tok.size()>2 )&&(tok.at(0).toUpper()=="SYMMETRY")){
        QString s=all.at(li).toUpper();
        s.remove("SYMMETRY");
        s=s.trimmed();
        s.replace(' ',',');
        if (!s.contains("Y")){
		s.replace("X1","X"); 
		s.replace("X2","Y"); 
		s.replace("X3","Z"); 
		s.replace("X4","R"); 
		s.replace("X5","S"); 
		s.replace("X6","T"); 
        //qDebug()<<s;
        }
        mol.decodeSymmCard(s);
//        qDebug()<<s;
      }
      if (tok.at(0).toUpper()=="END") break;   
    }
  }
  all.clear();
  if (mol.dimensions==4){
      QDialog *modulDlg=new QDialog(this);
      modulDlg->setWindowTitle("Loding a modulated structure...");
      QRadioButton *rad1_ = new QRadioButton("Load as avarage structure",modulDlg);
      QRadioButton *rad2_ = new QRadioButton("Load as modulated structure",modulDlg);
      rad2_->setChecked(true);
      QDoubleSpinBox *tzeroBox= new QDoubleSpinBox(modulDlg);
      tzeroBox->setMaximum(0);
      tzeroBox->setMaximum(1);
      tzeroBox->setSingleStep(0.01);
      tzeroBox->setValue(cubeGL->tvalue);
      tzeroBox->setDecimals(3);
      QDoubleSpinBox *tstepBox= new QDoubleSpinBox(modulDlg);
      tstepBox->setMinimum(0.001);
      QSpinBox *tfpsBox= new QSpinBox(modulDlg);
      tfpsBox->setMinimum(1);
      tfpsBox->setMaximum(60);
      tfpsBox->setValue(25);
      if (mol.zelle.commensurate) {
        mol.zelle.qvec=mol.zelle.qr+mol.zelle.qi;
        qDebug()<<mol.zelle.qvec.x<< mol.zelle.qvec.y<<mol.zelle.qvec.z <<mol.zelle.commen.x<<mol.zelle.commen.y<<mol.zelle.commen.z;
        mol.zelle.qvec.x = round(mol.zelle.qvec.x*mol.zelle.commen.x)/mol.zelle.commen.x;
        mol.zelle.qvec.y = round(mol.zelle.qvec.y*mol.zelle.commen.y)/mol.zelle.commen.y;
        mol.zelle.qvec.z = round(mol.zelle.qvec.z*mol.zelle.commen.z)/mol.zelle.commen.z;
        qDebug()<<mol.zelle.qvec.x<< mol.zelle.qvec.y<<mol.zelle.qvec.z <<mol.zelle.commen.x<<mol.zelle.commen.y<<mol.zelle.commen.z;
        double qrr = sqrt(Norm(mol.zelle.qvec));
         tstepBox->setDecimals(11);
         tstepBox->setSingleStep(0.000000001);
         tstepBox->setValue(qrr);
      }
      else{
      tstepBox->setValue(0.01);
      tstepBox->setSingleStep(0.001);
      tstepBox->setMaximum(0.999);
      tstepBox->setDecimals(3);

      }
      mol.bondsBetweenSGs=new QCheckBox("Draw bonds between symmetry groups",modulDlg);
      mol.bondsBetweenSGs->setChecked(true);
      mol.bondsBetweenSGs->setToolTip("molecular structures are redered faster, if this is checked. No bonds are drawn between symmetry releated molecules.");
      QDialogButtonBox *bbx = new    QDialogButtonBox(QDialogButtonBox::Open,Qt::Horizontal,modulDlg);

      connect(bbx, SIGNAL(accepted()), modulDlg, SLOT(accept()));
      connect(bbx, SIGNAL(rejected()), modulDlg, SLOT(reject()));
      QLabel *tzl,*tstl;

      QGridLayout *glt= new QGridLayout(modulDlg);
      glt->addWidget(rad1_,1,0,1,1);
      glt->addWidget(rad2_,1,1,1,1);

      glt->addWidget(tzeroBox,2,1,1,1);
      tzl= new QLabel("t0 = ",modulDlg);
      glt->addWidget(tzl,2,0,1,1);

      glt->addWidget(tstepBox,3,1,1,1);
      tstl= new QLabel("t steps for movie = ",modulDlg);
      glt->addWidget(tstl,3,0,1,1);
      glt->addWidget(new QLabel("Maximal t-steps per second"),4,0,1,1);
      glt->addWidget(tfpsBox,4,1,1,1);
      glt->addWidget(mol.bondsBetweenSGs,5,0,1,1);
      glt->addWidget(bbx,10,0,1,3);
      if (modulDlg->exec()==QDialog::Accepted){
          cubeGL->tvalue=tzeroBox->value();
          cubeGL->tstep =tstepBox->value();
          cubeGL->twait =1000.0/ tfpsBox->value();

          cubeGL->isModulated=rad2_->isChecked();
      }
    if (cubeGL->isModulated){
        ModulationMenu->setEnabled(true);
        modScale->show();

    mol.zelle.qvec=mol.zelle.qr+mol.zelle.qi;
    if (mol.zelle.commensurate){
      mol.zelle.qvec.x = round(mol.zelle.qvec.x*mol.zelle.commen.x)/mol.zelle.commen.x;
      mol.zelle.qvec.y = round(mol.zelle.qvec.y*mol.zelle.commen.y)/mol.zelle.commen.y;
      mol.zelle.qvec.z = round(mol.zelle.qvec.z*mol.zelle.commen.z)/mol.zelle.commen.z;
    }
    printf("\nQr%g %g %g\nQi%g %g %g\nQv%g %g %g\n",
        mol.zelle.qr.x,
        mol.zelle.qr.y,
        mol.zelle.qr.z,
        mol.zelle.qi.x,
        mol.zelle.qi.y,
        mol.zelle.qi.z,
        mol.zelle.qvec.x,
        mol.zelle.qvec.y,
        mol.zelle.qvec.z);
  tMovieStartAct->setEnabled(true);
  exportShelxAtTvalueAct->setEnabled(true);
    }
  }
  if (phcnt){
  phaseSpin->setMaximum(phcnt);
  phaseSpin->show();
  }else curentPhase=1;
  //qDebug()<<atypen;
  QFile m40(m40n);
  if (!m40.open(QIODevice::ReadOnly|QIODevice::Text)) {qDebug()<< "can't open "<<m40n; return;}
  all = QString(m40.readAll()).split("\n",QString::SkipEmptyParts);
  m40.close();
  //printf("m40 read\n");
  int na=0,na1=0,na2=0,na3=0,iread;
  INP newAtom;
  newAtom.part=0;
  newAtom.sg=0;
  someThingToRestore();
  george=true;
  QMap<int,QString> axstr;
  QMap<int,QString> atox;
  QMap<int,int> axtokcnt;
  QMap<QString,int> atloc;
  int skip=0,cmdli=0;
  bool commands=false;
  for (int li=0; li<all.size();li++){
   if (all.at(li).contains("s.u. block")) break; 
    if (all.at(li).contains("commands")) commands=true;
    if (all.at(li).startsWith("end")) {li++;commands=false;cmdli=li;}
    if (commands) continue;
   // qDebug()<<li<<all.at(li); 
    tok.clear();
    tok=all.at(li).split(" ",QString::SkipEmptyParts);
    if (tok.size()){
      if ((li-cmdli)<(curentPhase-1)) {
        skip+=tok.at(0).toInt();
        printf("I will skip the first %d atoms\n",skip);
      }
      if ((li-cmdli)==(curentPhase-1)) {
        na1 = tok.at(0).toInt();
        lrot = tok.last().toInt();
        nmol1=tok.at(1).toInt();
//        if (nmol1>0) qDebug()<<"This version does not support Jana-molecules yet!"<<na1<<lrot<<nmol1;
        if ((mol.ncomp>1)&&(tok.size()>2)) {
          na2 = tok.at(2).toInt();
          nmol2=tok.at(3).toInt();
          if (nmol2>0) qDebug()<<"This version does not support Jana-molecules yet!";
        }
        if ((mol.ncomp>2)&&(tok.size()>4)) {
          na3 = tok.at(4).toInt();
          nmol3=tok.at(5).toInt();
          if (nmol3>0) qDebug()<<"This version does not support Jana-molecules yet!";
        }
        na=na1+na2+na3;
//      qDebug()<< "I will read in "<<na<<li<<cmdli<<curentPhase-1<<all.at(li);
      }
      if (((li-cmdli)<=(curentPhase+nmol1-1))&&(nmol1)&&(tok.size()==2)) {
          nAtm1.append(tok.at(0).toInt());
          nPos1.append(tok.at(1).toInt());
          //qDebug()<<__LINE__<<"molecule 1 has "<<nAtm1<<" atoms and exists on "<<nPos1<<" positions";
      }
      if (((li-cmdli)<=(curentPhase+nmol2+nmol1-1))&&(nmol2)&&(tok.size()==2)) {
          nAtm2.append(tok.at(0).toInt());
          nPos2.append(tok.at(1).toInt());
      }
      if (((li-cmdli)<=(curentPhase+nmol3+nmol2+nmol1-1))&&(nmol3)&&(tok.size()==2)) {
          nAtm3.append(tok.at(0).toInt());
          nPos3.append(tok.at(1).toInt());
      }
//      if ((li)&&(!na)) return;
//      qDebug()<<all.at(li).contains(QRegExp("^[A-z]+"))<<all.at(li)<<na<<asymmUnit.size();
      //if (all.at(li).contains(QRegExp("^[A-z]+")))qDebug()<<(masymmUnit.size()<na)<<(asymmUnit.size()<na)<<na<<asymmUnit.size()<<masymmUnit.size();
      if ((masymmUnit.size()<na)&&(asymmUnit.size()<na)&&(all.at(li).contains(QRegExp("^[A-z]+")))){
        //qDebug()<<all.at(li)<<skip<<na; 
        printf("%d\n",__LINE__);
        if (skip>0) skip--;
        else{
        //C1        1  1     1.000000 0.199051 0.122184 0.071881    
        //123456789012345678901234567890123456789012345678901234567890
        //000000000111111111122222222223333333333444444444455555555556
        //Sm        1  2     0.250000 0.000000 0.000000 0.000106      000  0  1  0
        newAtom.lmax=-1;
        int so,sp,st,wo,wp,wt;
        for (int i=0; i<40; i++)newAtom.atomname[i]='\0';
        if ((all.at(li).length()>16)&&(all.at(li)[16]!=' ')) 
        iread=sscanf(all.at(li).toStdString().c_str(),"%8c%3d%3d%3d %9lf%9lf%9lf%9lf",newAtom.atomname,&newAtom.atomtype,&newAtom.jtf, &newAtom.lmax, &newAtom.amul, &newAtom.frac.x, &newAtom.frac.y, &newAtom.frac.z);
        else iread=sscanf(all.at(li).toStdString().c_str(),"%8c%3d%3d    %9lf%9lf%9lf%9lf     %1d%1d%1d%3d%3d%3d",newAtom.atomname,&newAtom.atomtype,&newAtom.jtf,  &newAtom.amul, &newAtom.frac.x, &newAtom.frac.y, &newAtom.frac.z,&so,&sp,&st,&wo,&wp,&wt);
        if (iread<13) {so=0;sp=0;st=0;wo=0;wp=0;wt=0;}
        for (int i=8; i>0; i--) {if (newAtom.atomname[i]==' ')newAtom.atomname[i]='\0'; else break;}
//        printf("so%d sp%d st%d wo%d wp%d wt%d\n",so,sp,st,wo,wp,wt);
   //     qDebug()<<all.at(li)<<all.at(li)[16];
   //     printf("'11111111222333444 555555555666666666777777777888888888'\n");
//        printf("%3ditems read. jtf:%3d lmx:%3d %-10s %4d %12.6f%12.6f%12.6f%12.6f\n",iread,newAtom.jtf,newAtom.lmax,newAtom.atomname,newAtom.atomtype,newAtom.frac.x,newAtom.frac.y,newAtom.frac.z,newAtom.amul);
        if (newAtom.jtf>0) {
          while ((all.at(li)[0]!=' ')&&(all.at(li)[0]!='-'))li++;//sometimes there are lines starting with the atomname
          sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf%9lf%9lf%9lf%9lf",
              &newAtom.uf.m11,
              &newAtom.uf.m22,
              &newAtom.uf.m33,
              &newAtom.uf.m12,
              &newAtom.uf.m13,
              &newAtom.uf.m23);
          newAtom.uf.m21=newAtom.uf.m12;
          newAtom.uf.m31=newAtom.uf.m13;
          newAtom.uf.m32=newAtom.uf.m23;
         //  qDebug()<<newAtom.uf.m11<<newAtom.uf.m22<<newAtom.uf.m33<<newAtom.uf.m12<<newAtom.uf.m13<<newAtom.uf.m23;
        }

        if (newAtom.jtf>2) {
          li++;
          sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf%9lf%9lf%9lf%9lf",
              &newAtom.c111,
              &newAtom.c112,
              &newAtom.c113,
              &newAtom.c122,
              &newAtom.c123,
              &newAtom.c133);

          li++;
          sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf%9lf%9lf",
              &newAtom.c222,
              &newAtom.c223,
              &newAtom.c233,
              &newAtom.c333);
         // qDebug()<<newAtom.c111<<newAtom.c112<<newAtom.c113<<newAtom.c122<<newAtom.c123<<newAtom.c133<<newAtom.c222<<newAtom.c223<<newAtom.c233<<newAtom.c333;
//*
          newAtom.c111/=cfac;//mol.zelle.as*mol.zelle.as*mol.zelle.as*
          newAtom.c222/=cfac;//mol.zelle.bs*mol.zelle.bs*mol.zelle.bs*
          newAtom.c333/=cfac;//mol.zelle.cs*mol.zelle.cs*mol.zelle.cs*
          newAtom.c112/=cfac;//mol.zelle.as*mol.zelle.as*mol.zelle.bs*
          newAtom.c122/=cfac;//mol.zelle.as*mol.zelle.bs*mol.zelle.bs*
          newAtom.c113/=cfac;//mol.zelle.as*mol.zelle.as*mol.zelle.cs*
          newAtom.c133/=cfac;//mol.zelle.as*mol.zelle.cs*mol.zelle.cs*
          newAtom.c223/=cfac;//mol.zelle.bs*mol.zelle.bs*mol.zelle.cs*
          newAtom.c233/=cfac;//mol.zelle.bs*mol.zelle.cs*mol.zelle.cs*
          newAtom.c123/=cfac;//mol.zelle.as*mol.zelle.bs*mol.zelle.cs*
 // */
        }

        if (newAtom.jtf>3) {
          li++;
          sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf%9lf%9lf%9lf%9lf",
              &newAtom.d1111,
              &newAtom.d1112,
              &newAtom.d1113,
              &newAtom.d1122,
              &newAtom.d1123,
              &newAtom.d1133);

          li++;
          sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf%9lf%9lf%9lf%9lf",
              &newAtom.d1222,
              &newAtom.d1223,
              &newAtom.d1233,
              &newAtom.d1333,
              &newAtom.d2222,
              &newAtom.d2223);

          li++;
          sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf%9lf",
              &newAtom.d2233,
              &newAtom.d2333,
              &newAtom.d3333);
/*
          newAtom.d1111/= mol.zelle.as * mol.zelle.as * mol.zelle.as * mol.zelle.as * dfac;////Ujklm's whanted
          newAtom.d2222/= mol.zelle.bs * mol.zelle.bs * mol.zelle.bs * mol.zelle.bs * dfac;//
          newAtom.d3333/= mol.zelle.cs * mol.zelle.cs * mol.zelle.cs * mol.zelle.cs * dfac;//
          newAtom.d1112/= mol.zelle.as * mol.zelle.as * mol.zelle.as * mol.zelle.bs * dfac;//
          newAtom.d1222/= mol.zelle.as * mol.zelle.bs * mol.zelle.bs * mol.zelle.bs * dfac;//
          newAtom.d1113/= mol.zelle.as * mol.zelle.as * mol.zelle.as * mol.zelle.cs * dfac;//
          newAtom.d1333/= mol.zelle.as * mol.zelle.cs * mol.zelle.cs * mol.zelle.cs * dfac;//
          newAtom.d2223/= mol.zelle.bs * mol.zelle.bs * mol.zelle.bs * mol.zelle.cs * dfac;//
          newAtom.d2333/= mol.zelle.bs * mol.zelle.cs * mol.zelle.cs * mol.zelle.cs * dfac;//
          newAtom.d1122/= mol.zelle.as * mol.zelle.as * mol.zelle.bs * mol.zelle.bs * dfac;//
          newAtom.d1133/= mol.zelle.as * mol.zelle.as * mol.zelle.cs * mol.zelle.cs * dfac;//
          newAtom.d2233/= mol.zelle.bs * mol.zelle.bs * mol.zelle.cs * mol.zelle.cs * dfac;//
          newAtom.d1123/= mol.zelle.as * mol.zelle.as * mol.zelle.bs * mol.zelle.cs * dfac;//
          newAtom.d1223/= mol.zelle.as * mol.zelle.bs * mol.zelle.bs * mol.zelle.cs * dfac;//
          newAtom.d1233/= mol.zelle.as * mol.zelle.bs * mol.zelle.cs * mol.zelle.cs * dfac;//
//  */
        }
        if ((cubeGL->isModulated)&&(mol.dimensions==4)){
          Modulat *modat=new Modulat(wo,wp,wt,so,sp,st);
          modat->mol=&mol;
          strcpy(modat->atomname,newAtom.atomname);
          modat->frac0=newAtom.frac;
          modat->part=0;
          modat->amul=newAtom.amul;
          modat->uf0=newAtom.uf;
          modat->jtf=newAtom.jtf;
          modat->sg=0;
        if (wo){
          li++;
          double o,os,oc;
          char polynomString[60];
          sscanf(all.at(li).toStdString().c_str(),"%9lf%s",&o,polynomString);
          printf("Polynom String:'%s'\n",polynomString);
          if (0==strncmp(polynomString,"Ortho",20)) modat->setPolyType(1);
          if (0==strncmp(polynomString,"Legendre",20)) modat->setPolyType(2);
          if (0==strncmp(polynomString,"XHarm",20)) modat->setPolyType(3);
          for (int ok=0; ok<wo;ok++){
          li++;
          sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf",&os,&oc);
          modat->setWaveOccPar(ok,o,os,oc);
          }
        }
        if (wp){ 
          for (int pk=0; pk<wp;pk++){       
          li++;
          double xs,ys,zs,
                 xc,yc,zc;
          sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf%9lf%9lf%9lf%9lf",
              &xs,&ys,&zs,&xc,&yc,&zc);
          printf("pk %d %f %f %f %f %f %f\n",pk,xs,ys,zs,xc,yc,zc);
          //qDebug()<<modat->debugme();
          modat->setWavePosPar(pk,xs,ys,zs,xc,yc,zc);
          }
        }
        if (wt){ 
          for (int tk=0; tk<wt;tk++){       
          li++;
          double u11s,u22s,u33s, u12s,u13s,u23s;
          double u11c,u22c,u33c, u12c,u13c,u23c;
          sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf%9lf%9lf%9lf%9lf",
              &u11s,&u22s,&u33s, &u12s,&u13s,&u23s);
          li++;
          sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf%9lf%9lf%9lf%9lf",
              &u11c,&u22c,&u33c, &u12c,&u13c,&u23c);
          modat->setWaveTemPar(tk,u11s,u22s,u33s, u12s,u13s,u23s,u11c,u22c,u33c, u12c,u13c,u23c);
          }
        }
        //qDebug()<<modat->debugwp();
 /*       V3 testpos=modat->frac(0.0);
        printf ("%-8s %9.6f %9.6f %9.6f at t=0.0\n",modat->atomname,testpos.x,testpos.y,testpos.z);
        testpos=modat->kart(0.0);
        printf ("kart%-8s %9.6f %9.6f %9.6f at t=0.0\n",modat->atomname,testpos.x,testpos.y,testpos.z);
        testpos=modat->frac(0.25);
        printf ("%-8s %9.6f %9.6f %9.6f at t=0.25\n",modat->atomname,testpos.x,testpos.y,testpos.z);
        testpos=modat->frac(0.5);
        printf ("%-8s %9.6f %9.6f %9.6f at t=0.5\n",modat->atomname,testpos.x,testpos.y,testpos.z);
        testpos=modat->frac(0.75);
        printf ("%-8s %9.6f %9.6f %9.6f at t=0.75\n",modat->atomname,testpos.x,testpos.y,testpos.z);// */
        modat->OrdZahl=mol.Get_OZ(atypen.at(newAtom.atomtype-1));
        if (newAtom.amul==0) modat->OrdZahl=-1;
        atloc[modat->atomname]=masymmUnit.size();
        ccmp=3;
        if ((na2+na1)>masymmUnit.size()) ccmp=2;
        if (na1>masymmUnit.size()) ccmp=1;
        printf("this is in composite %d\n",ccmp);
        modat->iamcomp=ccmp;
        masymmUnit.append(*modat);
        //masymmUnit.last().plotT();
        }
// H2 ^ H3                    H3                          xz  m
//1234567890123456789012345678901234567890123456789012345678901234567890
//         1         2         3         4         5         6         7
        else{
        newAtom.OrdZahl=mol.Get_OZ(atypen.at(newAtom.atomtype-1));
        if (newAtom.amul==0) newAtom.OrdZahl=-1;
        atloc[newAtom.atomname]=asymmUnit.size();
        asymmUnit.append(newAtom);
  printf("line %d\n",__LINE__);
      }
        }
      }
 // printf("line %d\n",__LINE__);

      if ((currentMolekule!=NULL)&&(currentMolekule->atoms.size()<currentMolekule->na())&&(all.at(li).contains(QRegExp("^[A-z]+")))){
        printf("Molekule definition!\n");
       /* qDebug()
          <<currentMolekule->atoms.size()
          <<currentMolekule->na()
          <<all.at(li)
          <<__LINE__;// */
       int so,sp,st,wo,wp,wt;
       ccmp=1;
       sscanf(all.at(li).toStdString().c_str(),"%*8s%*3d%*3d    %*9f%*9f%*9f%*9f     %1d%1d%1d%3d%3d%3d",
                           &so,&sp,&st,&wo,&wp,&wt);
       if (!cubeGL->isModulated) {
         wo=wp=wt=so=sp=st=0;
       }
       Modulat *molat=new Modulat(wo,wp,wt,so,sp,st);
       molat->mol=&mol;
       molat->iamcomp=ccmp;
       iread=sscanf(all.at(li).toStdString().c_str(),"%8s%3d%3d    %9lf%9lf%9lf%9lf     ",molat->atomname,&molat->OrdZahl,
                    &molat->jtf,  &molat->amul, &molat->frac0.x, &molat->frac0.y, &molat->frac0.z);
       molat->OrdZahl=mol.Get_OZ(atypen.at(molat->OrdZahl-1));
       li++;
       sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf%9lf%9lf%9lf%9lf",
           &molat->uf0.m11,
           &molat->uf0.m22,
           &molat->uf0.m33,
           &molat->uf0.m12,
           &molat->uf0.m13,
           &molat->uf0.m23);
       //printf("OK%d\n",__LINE__);
       if (wo){
         li++;
         double o,os,oc;
         char polynomString[60];
         sscanf(all.at(li).toStdString().c_str(),"%9lf%s",&o,polynomString);
         printf("Polynom String:'%s'\n",polynomString);
         if (0==strncmp(polynomString,"Ortho",20)) molat->setPolyType(1);
         if (0==strncmp(polynomString,"Legendre",20)) molat->setPolyType(2);
         if (0==strncmp(polynomString,"XHarm",20)) molat->setPolyType(3);
         for (int ok=0; ok<wo;ok++){
         li++;
         sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf",&os,&oc);
         molat->setWaveOccPar(ok,o,os,oc);
         }
       }
       if (wp){
         for (int pk=0; pk<wp;pk++){
         li++;
         double xs,ys,zs,
                xc,yc,zc;
         sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf%9lf%9lf%9lf%9lf",
             &xs,&ys,&zs,&xc,&yc,&zc);
         printf("pk %d %f %f %f %f %f %f\n",pk,xs,ys,zs,xc,yc,zc);
         //qDebug()<<modat->debugme();
         molat->setWavePosPar(pk,xs,ys,zs,xc,yc,zc);
         }
       }
       if (wt){
         for (int tk=0; tk<wt;tk++){
         li++;
         double u11s,u22s,u33s, u12s,u13s,u23s;
         double u11c,u22c,u33c, u12c,u13c,u23c;
         sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf%9lf%9lf%9lf%9lf",
             &u11s,&u22s,&u33s, &u12s,&u13s,&u23s);
         li++;
         sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf%9lf%9lf%9lf%9lf",
             &u11c,&u22c,&u33c, &u12c,&u13c,&u23c);
         molat->setWaveTemPar(tk,u11s,u22s,u33s, u12s,u13s,u23s,u11c,u22c,u33c, u12c,u13c,u23c);
         }
       }
       currentMolekule->atoms.append(*molat);
       //printf("OK%d\n",__LINE__);
       //qDebug()<<currentMolekule->atoms.size();
      }
      if ((currentMolekule!=NULL)&&(all.at(li).contains("pos#"))&&(currentMolekule->positions.size()<currentMolekule->np())){
        //qDebug()<<all.at(li)<<"line->"<<__LINE__<<currentMolekule->positions.size()<<currentMolekule->np();
        int so,sp,st,wo,wp,wt;
        int iread=sscanf(all.at(li).toStdString().c_str(),"%*8s%*3d%*3d%*f %1d%1d%1d%3d%3d%3d",&so,&sp,&st,&wo,&wp,&wt);
        if (iread<6) {so=0;sp=0;st=0;wo=0;wp=0;wt=0;}

        currentMolekule->positions.append(JmPos(tok.at(0),tok.at(1).toInt(),tok.at(2).toInt(),tok.at(3).toDouble(),
            so,sp,st,wo,wp,wt));
        li++;
        tok.clear();
        all[li].replace("-"," -");
        tok=all.at(li).split(" ",QString::SkipEmptyParts);
        //qDebug()<<iread<<so<<sp<<st<<wo<<wp<<wt<<all.at(li)<<__LINE__<<tok.size();
        currentMolekule->positions.last().phi=tok.at(0).toDouble();
        currentMolekule->positions.last().chi=tok.at(1).toDouble();
        currentMolekule->positions.last().psi=tok.at(2).toDouble();
        currentMolekule->positions.last().trans.x=tok.at(3).toDouble();
        currentMolekule->positions.last().trans.y=tok.at(4).toDouble();
        currentMolekule->positions.last().trans.z=tok.at(5).toDouble();
        li++;
        for (int ri=0; ri<currentMolekule->positions.last().rotty; ri++){
          //1st read mol orient
          //2nd read act orient 
          printf("xx%d\n",li);
          li++;
        }
        bool use_tls = false;
        for (int iii=0; iii<currentMolekule->atoms.size();iii++) if (currentMolekule->atoms.at(iii).jtf==0) use_tls = true;
        if (use_tls){//use TLS
          const char nn[21][4]={"T11","T22","T33","T12","T13","T23","L11","L22","L33","L12","L13","L23","S11","S21","S31","S12","S22","S23","S13","S23","S33"};
          tok.clear();
          tok=all.at(li).split(" ",QString::SkipEmptyParts);
          for (int itls=0;itls<21; itls+=6){
          sscanf(all.at(li++).toStdString().c_str(),"%9lf%9lf%9lf%9lf%9lf%9lf",
              &currentMolekule->positions.last().tls[itls],
              &currentMolekule->positions.last().tls[itls+1],
              &currentMolekule->positions.last().tls[itls+2],
              &currentMolekule->positions.last().tls[itls+3],
              &currentMolekule->positions.last().tls[itls+4],
              &currentMolekule->positions.last().tls[itls+5]);

          }
          li--;
          for (int itls=0;itls<21; itls++)
           printf("%s %9.6f \n",nn[itls],currentMolekule->positions.last().tls[itls]); 
        }
        if (wo){
          li++;
          double o,os,oc;
          char polynomString[60];
          sscanf(all.at(li).toStdString().c_str(),"%9lf%s",&o,polynomString);
          printf("#Polynom String:'%s'\n",polynomString);
          if (0==strncmp(polynomString,"Ortho",20)) {
           // modat->setPolyType(1);
          }
          if (0==strncmp(polynomString,"Legendre",20)) {
           // modat->setPolyType(2);
          }
          if (0==strncmp(polynomString,"XHarm",20)) {
           // modat->setPolyType(3);
          }
          for (int ok=0; ok<wo;ok++){
          li++;
          sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf",&os,&oc);
          //modat->setWaveOccPar(ok,o,os,oc);
          }
        }
        if (wp){ 
          for (int pk=0; pk<wp;pk++){       
          li++;
          double xs,ys,zs,
                 xc,yc,zc;
          sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf%9lf%9lf%9lf%9lf",
              &xs,&ys,&zs,&xc,&yc,&zc);
          printf("#pk %d %f %f %f %f %f %f\n",pk,xs,ys,zs,xc,yc,zc);
          //modat->setWavePosPar(pk,xs,ys,zs,xc,yc,zc);
          }
        }
        if (wt){ 
          for (int tk=0; tk<wt;tk++){       
          li++;
          double u11s,u22s,u33s, u12s,u13s,u23s;
          double u11c,u22c,u33c, u12c,u13c,u23c;
          sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf%9lf%9lf%9lf%9lf",
              &u11s,&u22s,&u33s, &u12s,&u13s,&u23s);
          li++;
          sscanf(all.at(li).toStdString().c_str(),"%9lf%9lf%9lf%9lf%9lf%9lf",
              &u11c,&u22c,&u33c, &u12c,&u13c,&u23c);
          //modat->setWaveTemPar(tk,u11s,u22s,u33s, u12s,u13s,u23s,u11c,u22c,u33c, u12c,u13c,u23c);
          }
        }
          //qDebug()<<currentMolekule->positions.last().aimol<<"LINE::"<<__LINE__<<use_tls<<wo<<wp<<wt<<all.at(li);
          li--;
          if (currentMolekule->positions.size()==currentMolekule->np()){ 
            //printf("X %d\n",__LINE__);
            QList<Modulat> ma;
            currentMolekule->printpos(0.0,ma);
            //printf("X =%d\n",ma.size());
            if ((!cubeGL->isModulated)) {
              QList<INP> molek;
              for (int mii=0; mii<ma.size(); mii++){
                molek.append(ma[mii].toINP(0.0));
                
              }
              asymmUnit.append(molek);
            }else{
            masymmUnit.append(ma);
            }
          }
          continue;
          //        currentMolekule->positions.last().
        }
      if (((currentMolekule!=NULL)&&(currentMolekule->positions.size()==currentMolekule->np()))||(currentMolekule==NULL)) if (((masymmUnit.size()==na)||(asymmUnit.size()==na))&&((tok.size()==4)||(tok.size()==6))&&(all.at(li).contains(QRegExp("^[A-z]+")))){
         // qDebug()<<all.at(li);
//anaMolecule(QString name,int ir,QString pg,QString M,int na,int np,int lr)
          //qDebug()<<"line: "<<__LINE__<<all.at(li)<<nmol1<<tok.at(0)<<tok.at(1).toInt()<<tok.at(2)<<tok.at(3)<<nAtm1.at(0)<<nPos1.at(0)<<lrot;
          currentMolekule = new JanaMolecule(tok.at(0),tok.at(1).toInt(),tok.at(2),tok.at(3),nAtm1.at(0),nPos1.at(0),lrot);
      }
      if ((asymmUnit.size()<=na)&&(all.at(li).contains(QRegExp("^ [A-z]+")))&&(tok.size()>1)){
          axstr[asymmUnit.size()-1]=tok.at(tok.size()-2);
          axtokcnt[asymmUnit.size()-1]=tok.size()-2;
          for (int xi=0; xi<tok.size()-2;xi++){
              atox[(asymmUnit.size()-1)*6+xi]=tok.at(xi);
          }
      }
    }
  }
  if (axstr.contains(0)){
      cubeGL->drawAx=true;
      hatlokale=1;
      george=false;

  int ndum=0;
  for (int i=0;i<asymmUnit.size();i++){
      if (axstr.value(i)=="xy"){asymmUnit[i].icor1=1;asymmUnit[i].icor2=2;asymmUnit[i].lflag=1;}
      else if (axstr.value(i)=="xz"){asymmUnit[i].icor1=1;asymmUnit[i].icor2=3;asymmUnit[i].lflag=1;}
      else if (axstr.value(i)=="yx"){asymmUnit[i].icor1=2;asymmUnit[i].icor2=1;asymmUnit[i].lflag=1;}
      else if (axstr.value(i)=="yz"){asymmUnit[i].icor1=2;asymmUnit[i].icor2=3;asymmUnit[i].lflag=1;}
      else if (axstr.value(i)=="zx"){asymmUnit[i].icor1=3;asymmUnit[i].icor2=1;asymmUnit[i].lflag=1;}
      else if (axstr.value(i)=="zy"){asymmUnit[i].icor1=3;asymmUnit[i].icor2=2;asymmUnit[i].lflag=1;}
      else if (axstr.value(i)=="xy-"){asymmUnit[i].icor1=1;asymmUnit[i].icor2=2;asymmUnit[i].lflag=-1;}
      else if (axstr.value(i)=="xz-"){asymmUnit[i].icor1=1;asymmUnit[i].icor2=3;asymmUnit[i].lflag=-1;}
      else if (axstr.value(i)=="yx-"){asymmUnit[i].icor1=2;asymmUnit[i].icor2=1;asymmUnit[i].lflag=-1;}
      else if (axstr.value(i)=="yz-"){asymmUnit[i].icor1=2;asymmUnit[i].icor2=3;asymmUnit[i].lflag=-1;}
      else if (axstr.value(i)=="zx-"){asymmUnit[i].icor1=3;asymmUnit[i].icor2=1;asymmUnit[i].lflag=-1;}
      else if (axstr.value(i)=="zy-"){asymmUnit[i].icor1=3;asymmUnit[i].icor2=2;asymmUnit[i].lflag=-1;}
      if (axtokcnt.value(i)==2){
          asymmUnit[i].nax=atloc.value(atox.value(i*6))+1;
          asymmUnit[i].nay1=i+1;
          asymmUnit[i].nay2=atloc.value(atox.value(i*6+1))+1;

      }else{
          if (atox.value(i*6+1)=="^"){
              sprintf(newAtom.atomname,"DUM0%d",ndum++);
              newAtom.frac=asymmUnit[atloc.value(atox.value(i*6))].frac+asymmUnit[atloc.value(atox.value(i*6+2))].frac;
              newAtom.frac*=0.5;
              newAtom.OrdZahl=-1;
              asymmUnit.append(newAtom);
              asymmUnit[i].nax=asymmUnit.size();
              asymmUnit[i].nay1=i+1;
              asymmUnit[i].nay2=atloc.value(atox.value(i*6+3))+1;
          }

          if (atox.value(i*6+2)=="^"){
              sprintf(newAtom.atomname,"DUM0%d",ndum++);
              newAtom.frac=asymmUnit[atloc.value(atox.value(i*6+1))].frac+asymmUnit[atloc.value(atox.value(i*6+3))].frac;
              newAtom.frac*=0.5;
              newAtom.OrdZahl=-1;
              asymmUnit.append(newAtom);
              asymmUnit[i].nax=atloc.value(atox.value(i*6+3))+1;
              asymmUnit[i].nay1=i+1;
              asymmUnit[i].nay2=asymmUnit.size();
          }
      }
     /* printf("%s [%s] 1:%s 2:%s 3:%s 4:%s 5:%s 6:%s\n",asymmUnit.at(i).atomname,axstr.value(i).toStdString().c_str(),
             atox.value(i*6).toStdString().c_str(),
             atox.value(i*6+1).toStdString().c_str(),
             atox.value(i*6+2).toStdString().c_str(),
             atox.value(i*6+3).toStdString().c_str(),
             atox.value(i*6+4).toStdString().c_str(),
             atox.value(i*6+5).toStdString().c_str());*/
  }
  }
  if (lavec.isEmpty()){
//    printf("line %d %c\n",__LINE__,gitt);
    mol.applyLatticeCentro(gitt,false);
 //   printf("line %d %c\n",__LINE__,gitt);
    mol.applyLatticeCentro(gitt,false);
  }else{
    int z=mol.zelle.symuncent=mol.zelle.symmops.size();
    for (int i=0; i<z;i++){
      for (int j=0;j<lavec.size();j++){
        if (fabs(Norm(lavec.at(j)))<0.01) continue;
        V3 tt = mol.zelle.trans.at(i)+lavec.at(j);
        tt.x=(tt.x>1)?tt.x-1:tt.x;
        tt.y=(tt.y>1)?tt.y-1:tt.y;
        tt.z=(tt.z>1)?tt.z-1:tt.z;
        mol.zelle.symmops.append(mol.zelle.symmops.at(i));
        mol.zelle.trans.append(tt);
        if (cubeGL->isModulated){
        mol.zelle.x4sym.append(mol.zelle.x4sym.at(i));
        mol.zelle.x4.append(mol.zelle.x4.at(i));
        double tr4=mol.zelle.x4tr.at(i);
        if (lavec4.size()>j) tr4=fmod(99+tr4+lavec4.at(j),1.0);
        mol.zelle.x4tr.append(tr4);
        }
      }
    }
  }
  if ((mol.dimensions==4)&& (ncomposite>1)){
  for (int i=0; i<ncomposite-1;i++){
//    fprintf(stderr,"ok\n");4302
    tok.clear();
    tok=Wmats[i].split(" ",QString::SkipEmptyParts);
  //  qDebug()<<tok.size()<<i;
    if (tok.size()!=16) continue;
    mol.ccc.append(Composite(
      tok.at( 0).toDouble(),
      tok.at( 1).toDouble(),
      tok.at( 2).toDouble(),
      tok.at( 3).toDouble(),
      tok.at( 4).toDouble(),
      tok.at( 5).toDouble(),
      tok.at( 6).toDouble(),
      tok.at( 7).toDouble(),
      tok.at( 8).toDouble(),
      tok.at( 9).toDouble(),
      tok.at(10).toDouble(),
      tok.at(11).toDouble(),
      tok.at(12).toDouble(),
      tok.at(13).toDouble(),
      tok.at(14).toDouble(),
      tok.at(15).toDouble(),mol.zelle));
//    fprintf(stderr,"2ok\n");
  }
  }
//    fprintf(stderr,"3ok\n");
 // printf("line %d\n",__LINE__);
  for (int i=0;i<asymmUnit.size();i++) { 
//    mol.frac2kart(asymmUnit[i].frac,asymmUnit[i].kart);
//    printf("%-10s %4d %12.6f%12.6f%12.6f\n",asymmUnit[i].atomname,asymmUnit[i].OrdZahl,asymmUnit[i].kart.x,asymmUnit[i].kart.y,asymmUnit[i].kart.z);
    if ((asymmUnit[i].uf.m22==0.0)&&(asymmUnit[i].uf.m33==0.0)){
      asymmUnit[i].u.m11=asymmUnit[i].u.m22=asymmUnit[i].u.m33=asymmUnit[i].uf.m11;
      asymmUnit[i].u.m12=asymmUnit[i].u.m13=asymmUnit[i].u.m23=asymmUnit[i].u.m21=asymmUnit[i].u.m31=asymmUnit[i].u.m32=0.0;}
    else mol.Uf2Uo(asymmUnit[i].uf,asymmUnit[i].u);
//  printf("Ucart%12.6f%12.6f%12.6f%12.6f%12.6f%12.6f\n",asymmUnit[i].u.m11,asymmUnit[i].u.m22,asymmUnit[i].u.m33, asymmUnit[i].u.m12, asymmUnit[i].u.m13,asymmUnit[i].u.m23);
  }
  //fprintf(stderr,"%d\n",__LINE__);
  growSymm(6);
  //fprintf(stderr,"%d\n",__LINE__);
  fmcq->curentPhase=curentPhase;
  bool good=true;
//  printf("line %d\n",__LINE__);
  if (cubeGL->isModulated) good=fmcq->loadDimensionm80AndPerform(fouName.toStdString().c_str(),true);
  else good=fmcq->loadm80AndPerform(fouName.toStdString().c_str(),true);
  if (!good){
    infoKanalNews(QString("<font color=red>Could not load %1!</font><br>").arg(fouName));
    fmcq->deleteLists();
  }
  fmcq->doMaps->show();
  //xdMenu->setEnabled(true);
}


void MyWindow::howOldIsTheLatesDataBaseI(){
  connect(net, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinishedI(QNetworkReply*)));
  reply = net->get(QNetworkRequest(QUrl("http://www.molecoolqt.de/latestDABA.php")));
}

void MyWindow::howOldIsTheLatesDataBaseII(){
  connect(net, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinishedII(QNetworkReply*)));
  reply = net->get(QNetworkRequest(QUrl("http://www.molecoolqt.de/latestDABA2.php")));
}


void MyWindow::replyFinishedI(QNetworkReply* antwort){
  QString a=antwort->readAll();
  QDateTime t=  QDateTime::fromString (a,Qt::ISODate);
  int d=t.daysTo(QDateTime::currentDateTime ());
  invariomMenu->addAction(QString("The latest (D95++3df3pd) database on the web is %1 days old.").arg(d));
  //QMessageBox::information(this,"DA DA DIE DABA DIE IS DA!",QString("Die neuste Datenbank ist %1 alt.").arg(d)); 
  antwort->close();
  antwort->deleteLater();
  disconnect(net,SIGNAL(finished(QNetworkReply*)),0,0);
  printf("days %d\n",d);
  if ((d>0)&&(d<3650)) {
    connect(net, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished2(QNetworkReply*)));
    reply = net->get(QNetworkRequest(QUrl("http://www.molecoolqt.de/loadDABA.php")));
  }
}
void MyWindow::replyFinishedII(QNetworkReply* antwort){
  QString a=antwort->readAll();
  QDateTime t=  QDateTime::fromString (a,Qt::ISODate);
  int d=t.daysTo(QDateTime::currentDateTime ());
  invariomMenu->addAction(QString("The latest (def2TZVP) database on the web is %1 days old.").arg(d));
  //QMessageBox::information(this,"DA DA DIE DABA DIE IS DA!",QString("Die neuste Datenbank ist %1 alt.").arg(d)); 
  antwort->close();
  antwort->deleteLater();
  disconnect(net,SIGNAL(finished(QNetworkReply*)),0,0);
  printf("days %d\n",d);
  if ((d>0)&&(d<3650)) {
    connect(net, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished3(QNetworkReply*)));
    reply = net->get(QNetworkRequest(QUrl("http://www.molecoolqt.de/loadDABA2.php")));
  }
}

void MyWindow::replyFinished2(QNetworkReply* antwort){
  QString a=antwort->readAll();
  //
  QString Pfad = mol.einstellung->fileName();
  Pfad=Pfad.section('/',0,-2);
  Pfad.append("/DABA.txt");
  QFile base(Pfad);
  if (base.open(QIODevice::WriteOnly | QIODevice::Text)){
    base.write(a.toLatin1());
    base.close();
  } 
  antwort->close();
  antwort->deleteLater();
  cubeGL->loadDataBase(Pfad);
  disconnect(net,SIGNAL(finished(QNetworkReply*)),0,0);
}

void MyWindow::replyFinished3(QNetworkReply* antwort){
  QString a=antwort->readAll();
  //
  QString Pfad = mol.einstellung->fileName();
  Pfad=Pfad.section('/',0,-2);
  Pfad.append("/DABA_def2TZVP.txt");
  QFile base(Pfad);
  if (base.open(QIODevice::WriteOnly | QIODevice::Text)){
    base.write(a.toLatin1());
    base.close();
  } 
  antwort->close();
  antwort->deleteLater();
  cubeGL->loadDataBase(Pfad);
  disconnect(net,SIGNAL(finished(QNetworkReply*)),0,0);
}
//$filename="/user/birger/NEUE_Datenbank/ToolZ/DABA.txt";
//
//void MyWindow::listCP(){

//}

void MyWindow::load_BayMEM(QString fileName) {
  INP newAtom;
  george=true;
  mol.pmin=10000;
  mol.pmax=-10000;
//  printf("load BayMEM  %d\n",__LINE__);
  newAtom.part=0;
  newAtom.sg=0;
  newAtom.lam1=0.0;
  newAtom.lam2=0.0;
  newAtom.lam3=0.0;
  newAtom.peakHeight=0.0;
  newAtom.cptype=0;
  someThingToRestore();
  mol.zelle.symmops.clear();
  mol.zelle.trans.clear();
  V3 nl(0,0,0);
  mol.initDir();
  QString coo; 
  cubeGL->drawAx=false;
  hatlokale=0;
  QList<V3>lavec;
 
//  printf("loadxd %d\n",__LINE__);fflush(stdout);
    QFile bm(fileName);
    bm.open(QIODevice::ReadOnly|QIODevice::Text);
    QStringList bml=QString(bm.readAll()).split('\n');
    bm.close();
   // qDebug()<<fileName;
    bool bmat=false;
    bool symo=false;
    bool centros=false;
    bool centr=false;
    for (int i=0; i<bml.size();i++){
    if (bml.at(i).startsWith("endprioratoms"))bmat=false;
    if (bmat){
      asymmUnit.append(newAtom); 
      int k=asymmUnit.size()-1;
      char typ[3];
      asymmUnit[k].jtf=2;
      int succ=
      sscanf(bml.at(i).toStdString().c_str(),"%s%s%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf",
	     asymmUnit[k].atomname,
	     typ,
             &asymmUnit[k].amul,
	     &asymmUnit[k].frac.x,
	     &asymmUnit[k].frac.y,
             &asymmUnit[k].frac.z,
             &asymmUnit[k].uf.m11,
             &asymmUnit[k].uf.m22,
             &asymmUnit[k].uf.m33,
             &asymmUnit[k].uf.m21,//21 == 12
             &asymmUnit[k].uf.m31,//31 == 13
             &asymmUnit[k].uf.m32);//32 == 23 
  //    printf("successfully read items: %d\n",succ);
      if (succ!=12) {
             asymmUnit[k].uf.m22=
             asymmUnit[k].uf.m33=
             asymmUnit[k].uf.m21=//21 == 12
             asymmUnit[k].uf.m31=//31 == 13
             asymmUnit[k].uf.m32=0.0;//32 == 23 
             asymmUnit[k].jtf=1;
      }
      asymmUnit[k].uf.m23=asymmUnit[k].uf.m32;
      asymmUnit[k].uf.m13=asymmUnit[k].uf.m31;
      asymmUnit[k].uf.m12=asymmUnit[k].uf.m21;
      asymmUnit[k].OrdZahl=(asymmUnit[k].amul==0.0)?-1:mol.Get_OZ(typ);

      asymmUnit[k].sg=0;
      printf("%-10s[%d] %d %9.6f%9.6f%9.6f %9.6f%9.6f%9.6f%9.6f%9.6f%9.6f\n",asymmUnit[k].atomname,k,asymmUnit[k].OrdZahl,asymmUnit[k].frac.x,asymmUnit[k].frac.y,asymmUnit[k].frac.z,
             asymmUnit[k].uf.m11,
             asymmUnit[k].uf.m22,
             asymmUnit[k].uf.m33,
             asymmUnit[k].uf.m21,//21 == 12
             asymmUnit[k].uf.m31,//31 == 13
             asymmUnit[k].uf.m32);//32 == 23 
    }
    if (bml.at(i).startsWith("prioratoms"))bmat=true;
    if (bml.at(i).startsWith("outputbase")){
      coo=bml.at(i).section(' ',1,1);
    }
    if (bml.at(i).startsWith("cell")){
      sscanf(bml.at(i).toStdString().c_str(),"cell %lf%lf%lf%lf%lf%lf",&mol.zelle.a,&mol.zelle.b,&mol.zelle.c,&mol.zelle.al,&mol.zelle.be,&mol.zelle.ga);
      setup_zelle();
    }
    if (bml.at(i).startsWith("title")) sscanf(bml.at(i).toStdString().c_str(),"title %s",CID);
    if (bml.at(i).startsWith("endcenters")){
      centr=false;
     // printf("endcenters\n");
    }
    if (bml.at(i).startsWith("endsymmetry")){
      symo=false;
      mol.applyLatticeCentro('P',false);
      for (int si=0; si<mol.zelle.symmops.size(); si++){
        double det=determinant(mol.zelle.symmops.at(si));
        printf("\n%9g%9g%9g  %9g\n%9g%9g%9g  %9g\n%9g%9g%9g  %9g det: %9g\n",
            mol.zelle.symmops.at(si).m11,
            mol.zelle.symmops.at(si).m12,
            mol.zelle.symmops.at(si).m13, mol.zelle.trans.at(si).x,
            mol.zelle.symmops.at(si).m21,
            mol.zelle.symmops.at(si).m22,
            mol.zelle.symmops.at(si).m23, mol.zelle.trans.at(si).y,
            mol.zelle.symmops.at(si).m31,
            mol.zelle.symmops.at(si).m32,
            mol.zelle.symmops.at(si).m33, mol.zelle.trans.at(si).z,det

            );
      }
    }
    if (symo){
        QString s=bml.at(i).toUpper();
        s=s.trimmed();
        s.replace(QRegExp("\\s+"),",");
        if (!s.contains("Y")){
		s.replace("X1","X"); 
		s.replace("X2","Y"); 
		s.replace("X3","Z"); 
		s.replace("X4","R"); 
		s.replace("X5","S"); 
		s.replace("X6","T"); 
     //   qDebug()<<"symm"<<s;
        }
        mol.decodeSymmCard(s);
    }
    if (centr){
   //   qDebug()<<bml.at(i);
      QStringList tok=bml.at(i).split(" ",QString::SkipEmptyParts);
      V3 v=V3(0,0,0);
      if (tok.at(0).contains('/')) {
        QStringList bruch = tok.at(0).split("/",QString::SkipEmptyParts);
        v.x = bruch.at(0).toDouble()/bruch.at(1).toDouble();
      }else
      v.x  = tok.at(0).toDouble();
      if (tok.at(1).contains('/')) {
        QStringList bruch = tok.at(1).split("/",QString::SkipEmptyParts);
        v.y = bruch.at(0).toDouble()/bruch.at(1).toDouble();
      }else
      v.y  = tok.at(1).toDouble();
      if (tok.at(2).contains('/')) {
        QStringList bruch = tok.at(2).split("/",QString::SkipEmptyParts);
        v.z = bruch.at(0).toDouble()/bruch.at(1).toDouble();
      }else
      v.z  = tok.at(2).toDouble();
      lavec.append(v);

    }
    if (bml.at(i).startsWith("symmetry"))symo=true;
    if (bml.at(i).startsWith("centro ")) {
      centros = bml.at(i).contains("yes");
    }
    if (bml.at(i).startsWith("centers")){
  //    printf("centers\n");
      centr=true;}
    }
//    qDebug()<<CID<<asymmUnit.size()<<mol.zelle.trans.size()<<mol.zelle.symmops.size()<<mol.zelle.a<<mol.zelle.b<<mol.zelle.c<<mol.zelle.al<<mol.zelle.be<<mol.zelle.ga;
    if (!coo.isEmpty()){
      coo.append(".coo");
      QFile coof(coo);
      coof.open(QIODevice::ReadOnly|QIODevice::Text);
      QStringList all=QString(coof.readAll()).split('\n');
      coof.close();
      QStringList tok;
      int maxi=1;
      for (int ci=0; ci<all.size();ci++){
        if (all.at(ci).startsWith('#')) continue;
        tok=all.at(ci).split(QRegExp("\\s+"),QString::SkipEmptyParts);
        if (tok.size()==4){
        sprintf(newAtom.atomname,"Q%d",maxi++);
        newAtom.frac.x=tok.at(0).toDouble();
        newAtom.frac.y=tok.at(1).toDouble();
        newAtom.frac.z=tok.at(2).toDouble();
	newAtom.OrdZahl=-3;
	newAtom.peakHeight=tok.at(3).toDouble();

        mol.pmin=fmin(mol.pmin,newAtom.peakHeight);
        mol.pmax=fmax(mol.pmax,newAtom.peakHeight);
        mol.frac2kart(newAtom.frac,newAtom.kart);
        asymmUnit.append(newAtom);
        int k=asymmUnit.size()-1;
        printf("%-10s[%d] %d %9.6f%9.6f%9.6f\n",asymmUnit[k].atomname,k,asymmUnit[k].OrdZahl,asymmUnit[k].frac.x,asymmUnit[k].frac.y,asymmUnit[k].frac.z);
        }
        if (tok.size()==9){
        sprintf(newAtom.atomname,"Q%d",maxi++);
        newAtom.frac.x=tok.at(0).toDouble();
        newAtom.frac.y=tok.at(1).toDouble();
        newAtom.frac.z=tok.at(2).toDouble();
	newAtom.OrdZahl=-3;
	newAtom.peakHeight=tok.at(7).toDouble();

        mol.pmin=fmin(mol.pmin,newAtom.peakHeight);
        mol.pmax=fmax(mol.pmax,newAtom.peakHeight);
        mol.frac2kart(newAtom.frac,newAtom.kart);
        asymmUnit.append(newAtom);
        int k=asymmUnit.size()-1;
        printf("%-10s[%d] %d %9.6f%9.6f%9.6f\n",asymmUnit[k].atomname,k,asymmUnit[k].OrdZahl,asymmUnit[k].frac.x,asymmUnit[k].frac.y,asymmUnit[k].frac.z);
        }else
        if (tok.size()==8){
        //0.835532    0.209343   -0.004174    (3,-1)       0.0321       -0.5782       -0.4247        0.4156
        sprintf(newAtom.atomname,"CP%d",maxi++);
        newAtom.frac.x=tok.at(0).toDouble();
        newAtom.frac.y=tok.at(1).toDouble();
        newAtom.frac.z=tok.at(2).toDouble();
	newAtom.OrdZahl=-2;
        
        newAtom.cptype=tok.at(3).section(QRegExp("[)(,]+"),2,2).toInt()+3;
        newAtom.lam1 = tok.at(5).toDouble();
        newAtom.lam2 = tok.at(6).toDouble();
        newAtom.lam3 = tok.at(7).toDouble();
	newAtom.peakHeight=tok.at(4).toDouble();
        mol.frac2kart(newAtom.frac,newAtom.kart);
        asymmUnit.append(newAtom);
        int k=asymmUnit.size()-1;
        printf("%-10s[%d] %d %9.6f%9.6f%9.6f %12.8f\n",asymmUnit[k].atomname,k,asymmUnit[k].OrdZahl,asymmUnit[k].frac.x,asymmUnit[k].frac.y,asymmUnit[k].frac.z,asymmUnit[k].peakHeight);
        }
      }
    }
    dummax=0;
  mol.zelle.centro=centros;
//  qDebug()<<mol.zelle.centro<<centros;
  smx=atmax=asymmUnit.size();
  if (lavec.size()>1){
    int z=mol.zelle.symuncent=mol.zelle.symmops.size();
    for (int i=0; i<z;i++){
      for (int j=0;j<lavec.size();j++){
        if (fabs(Norm(lavec.at(j)))<0.01) continue;
        V3 tt = mol.zelle.trans.at(i)+lavec.at(j);
        tt.x=(tt.x>1)?tt.x-1:tt.x;
        tt.y=(tt.y>1)?tt.y-1:tt.y;
        tt.z=(tt.z>1)?tt.z-1:tt.z;
        mol.zelle.symmops.append(mol.zelle.symmops.at(i));
        mol.zelle.trans.append(tt);
//        qDebug()<<"add by center"<<tt.x<<tt.y<<tt.z<<mol.encodeSymm(mol.zelle.symmops.size()-1);
        }
      }
    }
  for (int i=0;i<asymmUnit.size();i++) {
    if (asymmUnit[i].OrdZahl<0) continue;
    if ((asymmUnit[i].uf.m22==0.0)&&(asymmUnit[i].uf.m33==0.0)){
      printf("%-8s \n",asymmUnit[i].atomname);
      asymmUnit[i].u.m11=asymmUnit[i].u.m22=asymmUnit[i].u.m33=asymmUnit[i].uf.m11;
      asymmUnit[i].u.m12=asymmUnit[i].u.m13=asymmUnit[i].u.m23=asymmUnit[i].u.m21=asymmUnit[i].u.m31=asymmUnit[i].u.m32=0.0;
    }
    else {

      mol.Uf2Uo(asymmUnit[i].uf,asymmUnit[i].u);
/*
  printf("%-8s %8.5g %8.5g %8.5g %8.5g %8.5g %8.5g\n",asymmUnit[i].atomname,
		asymmUnit[i].uf.m11,
		asymmUnit[i].uf.m22,
		asymmUnit[i].uf.m33,
		asymmUnit[i].uf.m12,
		asymmUnit[i].uf.m13,
		asymmUnit[i].uf.m23); 
  printf("%-8s %8.5g %8.5g %8.5g %8.5g %8.5g %8.5g\n",asymmUnit[i].atomname,
		asymmUnit[i].u.m11,
		asymmUnit[i].u.m22,
		asymmUnit[i].u.m33,
		asymmUnit[i].u.m12,
		asymmUnit[i].u.m13,
		asymmUnit[i].u.m23);// */
    }
  }

  growSymm(donTGrow->isChecked()?0:6);
//  */
}


void MyWindow::load_xdres(QString fileName) {
  INP newAtom;
//  printf("loadxd %d\n",__LINE__);
  newAtom.part=0;
  newAtom.sg=0;
  someThingToRestore();
  xdSetupAct->setEnabled(true);
  xdMenu->setEnabled(true);
  QString masName;
  masName=fileName;
  masName.chop(3);
  masName.append("mas");

  QString fouName;
  fouName=fileName;
  fouName.chop(3);
  fouName.append("fou");
  mol.zelle.symmops.clear();
  mol.zelle.trans.clear();
  V3 nl(0,0,0);
  mol.zelle.trans.append(nl);
  mol.zelle.symmops.append(Matrix(1,0,0, 0,1,0, 0,0,1));
  QStringList atypen;
//  printf("loadxd %d\n",__LINE__);fflush(stdout);
  {//MAS//
    FILE *mas;
    char line[220];
    strcpy(line,"");

    if (NULL==(mas=fopen(masName.toLocal8Bit(),"r"))) {QMessageBox::critical(this,"Read Error!",QString("read error %1!").arg(masName),QMessageBox::Ok);exit(1);}
    while (line[0]!='C') {egal=fscanf(mas,"%[^\n\r]\n\r",line);if (line[0]=='T') sscanf(line,"TITLE %s",CID);}
    sscanf(line,"CELL %lf%lf%lf%lf%lf%lf",&mol.zelle.a,&mol.zelle.b,&mol.zelle.c,&mol.zelle.al,&mol.zelle.be,&mol.zelle.ga);
    while (line[0]!='W') {egal=fscanf(mas,"%[^\n\r]\n\r",line);}
    sscanf(line,"WAVE %lf",&mol.zelle.lambda);
    setup_zelle();
    while ((!feof(mas))&&(strstr(line,"SYMM")==NULL)) {egal=fscanf(mas,"%[^\n\r]\n\r",line);}
    while (strstr(line,"SYMM")!=NULL){  
      if (!mol.decodeSymmCard(line)){qDebug()<<line<<"Sorry, but I can't understand this SymmCard";exit(-1);}
      egal=fscanf(mas,"%[^\n\r]\n\r",line);
    }
    for (int i=1; i<107; i++)  pserbt[i]->hide();
    rewind(mas);
  char Centr,gitt;
  while (strstr(line,"LATT")==NULL) {egal=fscanf(mas,"%[^\n\r]\n\r",line);}
  sscanf(line,"LATT %c %c",&Centr,&gitt) ;
  mol.applyLatticeCentro(gitt,toupper(Centr)=='C');
//  printf("loadxd %d %d %d  %d %d \n",mol.zelle.trans.size(),mol.zelle.symmops.size(),mol.zelle.x4tr.size(),mol.zelle.x4.size(),mol.zelle.x4sym.size());
  while (strstr(line,"SCAT")==NULL) {egal=fscanf(mas,"%[^\n\r]\n\r",line);}
  while (strstr(line,"END")==NULL){  
    egal=fscanf(mas,"%[^\n\r]\n\r",line);
    if ((isalpha(line[0]))&&((strstr(line,"END")==NULL))) {
      atypen.append(strtok(line," "));
      pserbt[mol.Get_OZ(atypen.last())+1]->show();
      if (mol.Get_OZ(atypen.last())==5) pserbt[mol.Get_OZ(atypen.last())+1]->setChecked(true); 
    }
  }
//  printf("loadxd %d\n",__LINE__);
  sfacBox->setFixedSize((atypen.size()+1)*52,70);
 // printf("loadxd %d %d\n",__LINE__,atypen.size());
  rewind(mas);
 // printf("loadxd %d %s count %d\n",__LINE__,chargeGroups->currentText().toStdString().c_str(),chargeGroups->count());fflush(stdout);
  if (chargeGroups->count()>1){
   // printf("loadxd %d\n",__LINE__);fflush(stdout);
    chargeGroups->clear();
    chargeGroups->addItem("Charge Group 1 (all)",0);
  }
  //printf("loadxd %d\n",__LINE__);fflush(stdout);
  chargeGroups->setVisible(false);
  chargeGroups->setEnabled(false);
  while (!feof(mas)) {
    egal=fscanf(mas,"%[^\n\r]\n\r",line);
//    printf("%s->[%d]\n",line,egal);fflush(stdout);
    char ll[122],*tok;
    sscanf(line,"%[^!]",ll);  
    if (
    (strstr(ll,"GROUP")!=NULL)&&(line[0]=='G')&&(line[5]>47)&&(line[5]<59)){
      int gnr=0;
      sscanf(ll,"GROUP%d ",&gnr);
      chargeGroups->setVisible(true);
      chargeGroups->setMaxVisibleItems(20);
      chargeGroups->setEnabled(true);
      if (chargeGroups->findData(gnr)==-1) chargeGroups->addItem(QString("Charge Group %1").arg(gnr),gnr);
      for (tok=strtok(ll, " "); tok; tok=strtok(NULL, " ")) {
	if (strstr(tok,"GROUP")==NULL){
//	  printf("%d->%s<-\n",gnr,tok);
	ladungsgruppen[gnr].append(tok);
        }
      }
      //printf("==>%s<== %c %d %d \n",gns,ll[5],(line[5]>47),(line[5]<59));
    }
  }
  fclose(mas);
  }
//  printf("loadxd %d\n",__LINE__);fflush(stdout);
  FILE *adp;
  double XDVERS=0;
  {//RES  
  char line[85]="",dv[20],*dvv;
  int i=0;
  mol.initDir();
  if ((adp=fopen(fileName.toLocal8Bit(),"r"))==NULL) {QMessageBox::critical(this,"Read Error!",QString("read error %1!").arg(fileName),QMessageBox::Ok);exit(2);}  
  i=0;
  if (!fastrun) {
    cubeGL->drawAx=true;
    hatlokale=1;
  }
  while ((!feof(adp))&&(NULL==strstr(line,"Revision"))) {
    egal=fscanf(adp,"%[^\n\r]\n\r",line);
 }
//  printf("loadxd %d\n",__LINE__);fflush(stdout);
  if (NULL!=strstr(line,"Revision"))
  sscanf(line,"! <<< X D PARAMETER FILE >>> $Revision: %lf",&XDVERS); 
  printf("REV:=%g\n",XDVERS);
  rewind(adp);
  while ((line[0]!='U')||(line[1]!='S')||(line[2]!='A')||(line[3]!='G')) {
    egal=fscanf(adp,"%[^\n\r]\n\r",line);
 } 
  ;
  sscanf(line,"%[^0123456789]%d",dv,&atmax);
  dvv=strrchr(line,' ');
  sscanf(dvv,"%d",&dummax);
      while ((i<atmax)&&(!feof(adp))) {
    egal=fscanf(adp,"%[^\n\r]\n\r",line ); 
    if ((line[0]>='A')&&(!((line[0]=='N')&&(line[1]=='a')&&(line[2]=='N'))))  {
      asymmUnit.append(newAtom);
      sscanf(line,"%s %d%d%d%d%d%d%d%d%d%d%d%lf%lf%lf%lf",
	     asymmUnit[i].atomname,
	     &asymmUnit[i].icor1,
	     &asymmUnit[i].icor2,
	     &asymmUnit[i].nax,
	     &asymmUnit[i].nay1,
	     &asymmUnit[i].nay2,
	     &asymmUnit[i].jtf,
	     &asymmUnit[i].atomtype,
	     &asymmUnit[i].noofkappa,
	     &asymmUnit[i].lmax,
	     &asymmUnit[i].isym,
	     &asymmUnit[i].ichcon,
	     &asymmUnit[i].frac.x,
	     &asymmUnit[i].frac.y,
             &asymmUnit[i].frac.z,
	     &asymmUnit[i].amul);
      asymmUnit[i].lflag =(0>asymmUnit[i].nax)?-1:1;
      asymmUnit[i].nax =(0>asymmUnit[i].nax)?-asymmUnit[i].nax :asymmUnit[i].nax ;
//      strcpy(dv,asymmUnit[i].atomname);
//      strtok(dv,"(1234567890+- ");
      asymmUnit[i].OrdZahl=mol.Get_OZ(atypen.at(asymmUnit[i].atomtype-1));
      egal=fscanf(adp,"%lf%lf%lf%lf%lf%lf\n\r",&asymmUnit[i].uf.m11,&asymmUnit[i].uf.m22,&asymmUnit[i].uf.m33,
		      &asymmUnit[i].uf.m21,//21 == 12
		      &asymmUnit[i].uf.m31,//31 == 13
		      &asymmUnit[i].uf.m32);//32 == 23 
      asymmUnit[i].uf.m23=asymmUnit[i].uf.m32;
      asymmUnit[i].uf.m13=asymmUnit[i].uf.m31;
      asymmUnit[i].uf.m12=asymmUnit[i].uf.m21;
      if (asymmUnit[i].jtf>2){ //      printf("%-8s %d\n",asymmUnit[i].atomname,i);
      egal=fscanf(adp,"%lf%lf%lf%lf%lf\n\r",
      &asymmUnit[i].c111,
      &asymmUnit[i].c222,
      &asymmUnit[i].c333,
      &asymmUnit[i].c112,
      &asymmUnit[i].c122);
      egal=fscanf(adp,"%lf%lf%lf%lf%lf\n\r",
      &asymmUnit[i].c113,
      &asymmUnit[i].c133,
      &asymmUnit[i].c223,
      &asymmUnit[i].c233,
      &asymmUnit[i].c123);
      if (XDVERS < 4.105){
	
	asymmUnit[i].c111*=mol.zelle.as*mol.zelle.as*mol.zelle.as;//Ujkl ->cjkl
	asymmUnit[i].c222*=mol.zelle.bs*mol.zelle.bs*mol.zelle.bs;
	asymmUnit[i].c333*=mol.zelle.cs*mol.zelle.cs*mol.zelle.cs;
	asymmUnit[i].c112*=mol.zelle.as*mol.zelle.as*mol.zelle.bs;
	asymmUnit[i].c122*=mol.zelle.as*mol.zelle.bs*mol.zelle.bs;
	asymmUnit[i].c113*=mol.zelle.as*mol.zelle.as*mol.zelle.cs;
	asymmUnit[i].c133*=mol.zelle.as*mol.zelle.cs*mol.zelle.cs;
	asymmUnit[i].c223*=mol.zelle.bs*mol.zelle.bs*mol.zelle.cs;
	asymmUnit[i].c233*=mol.zelle.bs*mol.zelle.cs*mol.zelle.cs;
	asymmUnit[i].c123*=mol.zelle.as*mol.zelle.bs*mol.zelle.cs;
      } else{
	asymmUnit[i].c111/=cfac;//
	asymmUnit[i].c222/=cfac;//
	asymmUnit[i].c333/=cfac;//
	asymmUnit[i].c112/=cfac;//
	asymmUnit[i].c122/=cfac;//
	asymmUnit[i].c113/=cfac;//
	asymmUnit[i].c133/=cfac;//
	asymmUnit[i].c223/=cfac;//
	asymmUnit[i].c233/=cfac;//
	asymmUnit[i].c123/=cfac;//
      }

      }
   if (asymmUnit[i].jtf>3){ 
      egal=fscanf(adp,"%lf%lf%lf%lf%lf\n\r",
      &asymmUnit[i].d1111,
      &asymmUnit[i].d2222,
      &asymmUnit[i].d3333,
      &asymmUnit[i].d1112,
      &asymmUnit[i].d1222);
      egal=fscanf(adp,"%lf%lf%lf%lf%lf\n\r",
      &asymmUnit[i].d1113,
      &asymmUnit[i].d1333,
      &asymmUnit[i].d2223,
      &asymmUnit[i].d2333,
      &asymmUnit[i].d1122);
      egal=fscanf(adp,"%lf%lf%lf%lf%lf\n\r",
      &asymmUnit[i].d1133,
      &asymmUnit[i].d2233,
      &asymmUnit[i].d1123,
      &asymmUnit[i].d1223,
      &asymmUnit[i].d1233);
      if (XDVERS < 4.105){
          asymmUnit[i].d1111*=mol.zelle.as * mol.zelle.as * mol.zelle.as * mol.zelle.as *dfac;////Djklm's whanted
          asymmUnit[i].d2222*=mol.zelle.bs * mol.zelle.bs * mol.zelle.bs * mol.zelle.bs *dfac;//
          asymmUnit[i].d3333*=mol.zelle.cs * mol.zelle.cs * mol.zelle.cs * mol.zelle.cs *dfac;//
          asymmUnit[i].d1112*=mol.zelle.as * mol.zelle.as * mol.zelle.as * mol.zelle.bs *dfac;//
          asymmUnit[i].d1222*=mol.zelle.as * mol.zelle.bs * mol.zelle.bs * mol.zelle.bs *dfac;//
          asymmUnit[i].d1113*=mol.zelle.as * mol.zelle.as * mol.zelle.as * mol.zelle.cs *dfac;//
          asymmUnit[i].d1333*=mol.zelle.as * mol.zelle.cs * mol.zelle.cs * mol.zelle.cs *dfac;//
          asymmUnit[i].d2223*=mol.zelle.bs * mol.zelle.bs * mol.zelle.bs * mol.zelle.cs *dfac;//
          asymmUnit[i].d2333*=mol.zelle.bs * mol.zelle.cs * mol.zelle.cs * mol.zelle.cs *dfac;//
          asymmUnit[i].d1122*=mol.zelle.as * mol.zelle.as * mol.zelle.bs * mol.zelle.bs *dfac;//
          asymmUnit[i].d1133*=mol.zelle.as * mol.zelle.as * mol.zelle.cs * mol.zelle.cs *dfac;//
          asymmUnit[i].d2233*=mol.zelle.bs * mol.zelle.bs * mol.zelle.cs * mol.zelle.cs *dfac;//
          asymmUnit[i].d1123*=mol.zelle.as * mol.zelle.as * mol.zelle.bs * mol.zelle.cs *dfac;//
          asymmUnit[i].d1223*=mol.zelle.as * mol.zelle.bs * mol.zelle.bs * mol.zelle.cs *dfac;//
          asymmUnit[i].d1233*=mol.zelle.as * mol.zelle.bs * mol.zelle.cs * mol.zelle.cs *dfac;//
      } else{
    asymmUnit[i].d1111/=  dfac;////Djklm's whanted
    asymmUnit[i].d2222/=  dfac;//
    asymmUnit[i].d3333/=  dfac;//
    asymmUnit[i].d1112/=  dfac;//
    asymmUnit[i].d1222/=  dfac;//
    asymmUnit[i].d1113/=  dfac;//
    asymmUnit[i].d1333/=  dfac;//
    asymmUnit[i].d2223/=  dfac;//
    asymmUnit[i].d2333/=  dfac;//
    asymmUnit[i].d1122/=  dfac;//
    asymmUnit[i].d1133/=  dfac;//
    asymmUnit[i].d2233/=  dfac;//
    asymmUnit[i].d1123/=  dfac;//
    asymmUnit[i].d1223/=  dfac;//
    asymmUnit[i].d1233/=  dfac;//
      }
   }
   egal=fscanf(adp,"%lf\n\r",&asymmUnit[i].monopol);
   asymmUnit[i].sg=0;
      i++;
    }
  }
  
  rewind(adp);
  while (line[0]!='U') {egal=fscanf(adp,"%[^\n\r]\n\r",line); };
  egal=fscanf(adp,"%[^\n\r]\n\r",line);
  egal=fscanf(adp,"%[^\n\r]\n\r",line);
  for (int j=i;j<dummax+i;j++){
    asymmUnit.append(newAtom);
    sprintf(asymmUnit[j].atomname,"DUM%d",j-i);
    asymmUnit[j].OrdZahl=-1;
    egal=fscanf(adp,"%lf%lf%lf\n\r",
	   &asymmUnit[j].frac.x,
	   &asymmUnit[j].frac.y,
	   &asymmUnit[j].frac.z);
   asymmUnit[j].sg=0;
  }
  fclose(adp);
 
  smx=atmax+dummax;
  }
  {//xd_fft.out
    QString fftName=masName;
    fftName.chop(6);
    fftName.append("xd_fft.out");
  FILE *fft;
  char _line[120],_an[15];
  int ffix=0;
  int maxxp=0,mima=0;
  if (NULL!=(fft=fopen(fftName.toLocal8Bit(),"r"))){
    while ((!feof(fft))&&(NULL==strstr(_line,"HEIGHT"))&&(NULL==strstr(_line,"height"))) {
      egal=fscanf(fft,"%[^\n\r]\n\r",_line);
      if (strstr(_line,"THE PROGRAM WILL LOOK FOR")||strstr(_line,"The program will look for")){
	int maps;
      egal=sscanf(_line,"%*s %*s %*s %*s %*s %d %*s",&maps);
      maxxp+=maps;
      if (mima>0){
      }
      mima++;
      }     
    }
    if (!feof(fft)) {
      egal=fscanf(fft,"%[^\n\r]\n\r",_line);
      egal=fscanf(fft,"%[^\n\r]\n\r",_line);
      while ((!feof(fft))&&(NULL==strstr(_line,"----------")) &&(ffix<maxxp)&&((5==sscanf(_line,"%s %*s %*s %*s %*s %*s %lf %lf %lf %lf"
			,_an
			,&newAtom.frac.x 
			,&newAtom.frac.y 
			,&newAtom.frac.z
			,&newAtom.peakHeight))||
	     (5==sscanf(_line,"%*s %s %*s %lf %lf %lf %lf"
			,_an
			,&newAtom.frac.x
			,&newAtom.frac.y
			,&newAtom.frac.z
                        ,&newAtom.peakHeight)))) {
	strncpy(newAtom.atomname,_an,12);
	newAtom.OrdZahl=-3;
	asymmUnit.append(newAtom);
	ffix++;
	if (!feof(fft))egal=fscanf(fft,"%[^\n\r]\n\r",_line);
      }      
    }
infoKanal->setHtml(QString("%1<font color=green>reading of xd_fft.out is done.</font>").arg(infoKanal->toHtml()));
  }
  smx+=ffix;
  if (fft) fclose(fft);
  }
  {//xd_bubble.spf
    QString spfName=masName;
    spfName.chop(6);
    spfName.append("xd_bubble.spf");
    QFile spff(spfName);
    spff.open(QIODevice::ReadOnly|QIODevice::Text);
    QStringList spfs=QString(spff.readAll()).split('\n');
    for (int i=0; i<spfs.size(); i++){
      if (spfs.at(i).startsWith("Or")){
	QStringList ss=spfs.at(i).split(QRegExp("\\s+"));
	if (ss.size()>3){
	  newAtom.kart.x = ss.at(1).toDouble();
	  newAtom.kart.y = ss.at(2).toDouble();
	  newAtom.kart.z = ss.at(3).toDouble();
	  strcpy(newAtom.atomname,ss.at(0).toLatin1());
	  newAtom.OrdZahl=-2;
	  mol.kart2frac(newAtom.kart,newAtom.frac);
	  asymmUnit.append(newAtom);
	}
      }

    }


  
  }
  loadCPS(masName);
  /*
  {//xd_rho.cps   
    QString cpsName=masName;
    cpsName.chop(6);
    cpsName.append("xd_rho.cps");
    char cptp[10],_line[170]="Rho",dummystr[18];
    FILE *cps;
    int idxx=0,idx;
    if (NULL!=(cps=fopen(cpsName.toLocal8Bit(),"r"))){
      while (((strstr(_line,"GRADRHO"))||((strstr(_line,"Rho"))))&&(!feof(cps))) {egals=fgets(_line,160,cps);idxx++;}
     idxx--;
    printf("idx!! %d\n",idxx);
      rewind(cps);
      for (int i=0;i<idxx;i++){
        char unfg[30];
        egals=fgets(_line,160,cps);
//1   (3,-1)   O(1) -C(1)      Rho     -0.799877   6.307969  13.264604           2.888
//2   (3,-1)   C(1) -N(1)      Rho      3.109743   4.772340   4.761444         2.26238      -19.10129      -16.07322       14.61587        0.59290  2  3
//1   (3,-1)   O(1) -C(1)    GRADRHO    6.982419   3.403954   2.646129         4.62311      -82.41664       -5.90078       41.37913        0.56183  2 43
//2   (3,+1)        -        GRADRHO    7.196546   4.202940   3.389596         4.59766      -82.46656        5.45943       46.93281        0.00000  3 43
	if ((4<sscanf(_line,"%d %s %*19c %lf  %lf  %lf  %lf %*f %*f %*f %*f %*d %*d",&idx,cptp,
		      &newAtom.kart.x,  
		      &newAtom.kart.y,
		      &newAtom.kart.z,
		      &newAtom.peakHeight))||
            (6==sscanf(_line,"%d %s %*19c %lf  %lf  %lf  %lf",&idx,cptp,
		      &newAtom.kart.x,  
		      &newAtom.kart.y,
		      &newAtom.kart.z,
		      &newAtom.peakHeight))||
	    (5==(sscanf(_line,"%d %s %*19c %lf  %lf  %lf",&idx,cptp,
				&newAtom.kart.x,  
				&newAtom.kart.y,
				&newAtom.kart.z)))){
//	printf("line:%s unfg:%s\n",_line,unfg); 
	sprintf(dummystr,"CP%d",idx);
	strncpy(newAtom.atomname,dummystr,18);
	newAtom.OrdZahl=-2;
        mol.kart2frac(newAtom.kart,newAtom.frac);
        newAtom.cptype=QString(cptp).section(QRegExp("[)(,]+"),2,2).toInt()+3;
	asymmUnit.append(newAtom);
        }
      }
      egals=fgets(_line,160,cps);
if (!feof(cps)) 
  for (int i=0; i <idxx;i++) {
    egals=fgets(_line,160,cps);
    sscanf(_line,"%*d %*s %*s %lf %lf %lf",
		    &asymmUnit[smx+i].uf.m21,&asymmUnit[smx+i].uf.m22,&asymmUnit[smx+i].uf.m23);
    egals=fgets(_line,160,cps);                                                       
    sscanf(_line,"%*s %lf %lf %lf",                                                  
		    &asymmUnit[smx+i].uf.m31,&asymmUnit[smx+i].uf.m32,&asymmUnit[smx+i].uf.m33);
    egals=fgets(_line,160,cps);                                                       
    sscanf(_line,"%*s %lf %lf %lf",                                                  
		    &asymmUnit[smx+i].uf.m11,&asymmUnit[smx+i].uf.m12,&asymmUnit[smx+i].uf.m13);
    asymmUnit[smx+i].uf.m11-=asymmUnit[smx+i].kart.x;
    asymmUnit[smx+i].uf.m12-=asymmUnit[smx+i].kart.y;
    asymmUnit[smx+i].uf.m13-=asymmUnit[smx+i].kart.z;
    asymmUnit[smx+i].uf.m21-=asymmUnit[smx+i].kart.x;
    asymmUnit[smx+i].uf.m22-=asymmUnit[smx+i].kart.y;
    asymmUnit[smx+i].uf.m23-=asymmUnit[smx+i].kart.z;
    asymmUnit[smx+i].uf.m31-=asymmUnit[smx+i].kart.x;
    asymmUnit[smx+i].uf.m32-=asymmUnit[smx+i].kart.y;
    asymmUnit[smx+i].uf.m33-=asymmUnit[smx+i].kart.z;
  }
smx+=idxx;
infoKanal->setHtml(QString("%1<font color=green>reading of xd_rho.cps is done.</font>").arg(infoKanal->toHtml()));
    }
    if (cps) fclose(cps);
  }//  */
dummax=smx-atmax;
  for (int i=0;i<asymmUnit.size();i++) {
    if ((asymmUnit[i].uf.m22==0.0)&&(asymmUnit[i].uf.m33==0.0)){
      //printf("%-8s \n",asymmUnit[i].atomname);
      asymmUnit[i].u.m11=asymmUnit[i].u.m22=asymmUnit[i].u.m33=asymmUnit[i].uf.m11;
      asymmUnit[i].u.m12=asymmUnit[i].u.m13=asymmUnit[i].u.m23=asymmUnit[i].u.m21=asymmUnit[i].u.m31=asymmUnit[i].u.m32=0.0;
    }
    else {

      mol.Uf2Uo(asymmUnit[i].uf,asymmUnit[i].u);
/*printf("%-8s %8.5g %8.5g %8.5g %8.5g %8.5g %8.5g\n",asymmUnit[i].atomname,
		asymmUnit[i].u.m11,
		asymmUnit[i].u.m22,
		asymmUnit[i].u.m33,
		asymmUnit[i].u.m12,
		asymmUnit[i].u.m13,
		asymmUnit[i].u.m23);// */
    }
  }

  growSymm(donTGrow->isChecked()?0:6);
/*  for (int i=0;i<asymmUnit.size();i++) {
      printf("%-8s %12.5f %12.5f %12.5f  pdf(0)=%18.9g %18.9g\n",xdinp[i].atomname,
		      xdinp[i].kart.x,
		      xdinp[i].kart.y,
		      xdinp[i].kart.z,
		      pdf2(xdinp[i],xdinp[i].kart),pdf2(xdinp[i],xdinp[i].kart+V3(0.8,0,0)));

  }*/
  if (!fmcq->loadFouAndPerform(fouName.toStdString().c_str())) {
    if (fmcq->doMaps->isChecked()) infoKanalNews(QString("<font color=red>Could not load %1!</font><br>").arg(fouName));
    fmcq->deleteLists();
    fmcq->doMaps->hide();
  }
  fmcq->doMaps->show();
  if (pdfOnAtom!=-1) makePDFGrid(xdinp[pdfOnAtom]);
  pdfOnAtom=-1;
}

void MyWindow::bubblebub(){
  someThingToRestore();
  QString masName=QDir::currentPath()+"/xd.mas";
  bubble2CPS (masName);
  loadFile(masName,mol.gd);
  updateLabel();
}

void MyWindow::bubble2CPS(QString masName){
  QString dn=masName.section('/',0,-2);
  QFile xdout(dn+"/xd_pro.out");
  if (!xdout.open(QIODevice::ReadOnly|QIODevice::Text)) return;
  QStringList lines=QString(xdout.readAll()).split('\n');
  QString line,prop;
//  bool found=false;
  QStringList cptyp;
  QList<double> x,y,z,rho,lap;
  foreach (line,lines){
    if (line.contains("Property f =")) {
      prop = line.section(" ",3,3,QString::SectionSkipEmpty);
//      found=true;
    }
    if (line.contains("critical point")) {
      cptyp.append(line.section(" ",1,1,QString::SectionSkipEmpty));
    }
    if (line.contains(QRegExp(" x\\s+="))){x.append(line.section(" ",2,2,QString::SectionSkipEmpty).toDouble());}
    if (line.contains(QRegExp(" y\\s+="))){y.append(line.section(" ",2,2,QString::SectionSkipEmpty).toDouble());}
    if (line.contains(QRegExp(" z\\s+="))){z.append(line.section(" ",2,2,QString::SectionSkipEmpty).toDouble());}
    if (line.contains(QRegExp(" rho\\s+="))){rho.append(line.section(" ",2,2,QString::SectionSkipEmpty).toDouble());}
//    if (line.contains(QRegExp("|grad rho|\s+="))){x.append(line.section(" ",2,2,QString::SectionSkipEmpty).toDouble());}
    if (line.contains(QRegExp(" laplacian\\s+="))){lap.append(line.section(" ",2,2,QString::SectionSkipEmpty).toDouble());}

  }

 // qDebug()<<x.size()<<y.size()<<z.size()<<rho.size()<<lap.size();
  QString pth=dn+QString("/xd_%1-.cps").arg(prop.toLower());
  FILE *cps=fopen(pth.toStdString().c_str(),"at");
  if (cps!=NULL) {
  for (int i=0; i<x.size();i++){
    fprintf(cps,"%6d%9s        -          %s %11.6f%11.6f%11.6f     %11.5f        0.00000        0.00000        0.00000        0.00000  2  3\n"
        ,i+1
        ,cptyp.at(i).toStdString().c_str()
        ,prop.toStdString().c_str()
        ,x.at(i)
        ,y.at(i)
        ,z.at(i)
        ,rho.at(i)
        );
  }
  fclose(cps);
  }
}

void MyWindow::loadCPS(QString masName){
  INP newAtom;
  newAtom.part=0;
  newAtom.sg=0;
  QString dn=masName.section('/',0,-2);
  QStringList filter;
  filter << "xd_*.cps";
  QDir dir(dn);
  QStringList files = dir.entryList(filter,QDir::Files|QDir::Readable,QDir::Time|QDir::Reversed);
  if (files.isEmpty()) return;
  QString cpsName="";
  if (files.size()==1) cpsName=files.at(0);
  else{
    //dialog
    QMessageBox *box=new QMessageBox(QMessageBox::Question,"Chose CPS file","Please chose a CPS file to open",QMessageBox::Cancel);
    
    //
    for (int i=0; i<files.size(); i++){
      QPushButton *ac=box->addButton(QString("load %1").arg(files.at(i)),QMessageBox::ActionRole);
      ac->setProperty("fil",i);
    }
    box->exec(); 
      QVariant fil=box->clickedButton()->property("fil");
      if (fil.isValid()){ 
        int i = fil.toInt();
      qDebug()<<"I = "<< i;
      if ((i>-1)&&(i<files.size())) cpsName=files.at(i);  
      }
  
  }//else more tan one cps file

  
  QFile cps(cpsName);
  if (!cps.open(QIODevice::ReadOnly|QIODevice::Text)) return;
  QStringList lines=QString(cps.readAll()).split('\n');
  int idx=1;
  for (int i=0; i<lines.size(); i++){
    if ((lines.at(i).size()>63)&&(lines.at(i).at(23)=='-')){
      lines[i].replace(18,11,"           ");
      QStringList tok = lines.at(i).split(QRegExp("\\s+"),QString::SkipEmptyParts);
      if (tok.size()>5){
        newAtom.kart.x=tok.at(3).toDouble();
        newAtom.kart.y=tok.at(4).toDouble();
        newAtom.kart.z=tok.at(5).toDouble();
      }
      if (tok.size()>6){newAtom.peakHeight=tok.at(6).toDouble();}else newAtom.peakHeight=0.0;
      if (tok.size()>8) {
        newAtom.lam1=tok.at(7).toDouble();
        newAtom.lam2=tok.at(8).toDouble();
        newAtom.lam3=tok.at(9).toDouble();
      }
      else {
        newAtom.lam1 = 0.0;
        newAtom.lam2 = 0.0;
        newAtom.lam3 = 0.0;
      }
      strncpy(newAtom.atomname,QString("CP%1").arg(idx++).toStdString().c_str(),20);
      newAtom.OrdZahl=-2;
      mol.kart2frac(newAtom.kart,newAtom.frac);
      newAtom.cptype=tok.at(1).section(QRegExp("[)(,]+"),2,2).toInt()+3;
      asymmUnit.append(newAtom);
    }
  }
  infoKanal->setHtml(QString("%1<font color=green>reading of %2 is done.</font>").arg(infoKanal->toHtml()).arg(cpsName));
  cps.close();
}

double hermite4(V3 w, Matrix q, 
double d1111, double d2222, double d3333, 
double d1112, double d1113, double d1122, 
double d1123, double d1133, double d1222,
double d1223, double d1233, double d1333,
double d2223, double d2233, double d2333
){
//D-tensor : 
//D1111= 0.00000     D2222= 0.00000     D3333= 0.00000
//D1112= 0.00000     D1113= 0.00000     D1122= 0.00000
//D1123= 0.00000     D1133= 0.00000     D1222= 0.00000
//D1223= 0.00000     D1233= 0.00000     D1333= 0.00000
//D2223= 0.00000     D2233= 0.00000     D2333= 0.00000

  double t1,t2,t3,t=0,tz=0;
  double w1=w.x,
	 w2=w.y,
	 w3=w.z,
	 w11=w.x*w.x,
	 w22=w.y*w.y,
	 w33=w.z*w.z;
  double hjklm[15];
  t1    = w11*w11;
  t2    = 6.*w11*q.m11;
  t3    = 3.*q.m11*q.m11;
  hjklm[0] = t1-t2+t3;
  t     = hjklm[0]*d1111;
  t1    = w22*w22;
  t2    = 6.*w22*q.m22;
  t3    = 3.*q.m22*q.m22;
  hjklm[1] = t1-t2+t3;
  t     += hjklm[1]*d2222;
  t1    = w33*w33;
  t2    = 6.*w33*q.m33;
  t3    = 3.*q.m33*q.m33;
  hjklm[2] = t1-t2+t3;
  t     += hjklm[2]*d3333;
  t1    = w11*w1*w2;
  t2    = (w11*q.m12*3.+w1*w2*q.m11*3.);
  t3    = 3.*q.m11*q.m12;
  hjklm[3] = t1-t2+t3;
  tz    = hjklm[3]*d1112;
  hjklm[3] = hjklm[3]*4.0;
  t1    = w11*w1*w3;
  t2    = (w11*q.m13*3.+w1*w3*q.m11*3.);
  t3    = 3.*q.m11*q.m13;
  hjklm[4] = t1-t2+t3;
  tz    += hjklm[4]*d1113;
  hjklm[4] =hjklm[4]*4.0;
  t1    = w22*w2*w1;
  t2    = (w22*q.m12*3.+w2*w1*q.m22*3.);
  t3    = 3.*q.m22*q.m12;
  hjklm[8] = t1-t2+t3;
  tz    += hjklm[8]*d1222;
  hjklm[8] = hjklm[8]*4.0;
  t1    = w33*w3*w1;
  t2    = (w33*q.m13*3.+w1*w3*q.m33*3.);
  t3    = 3.*q.m33*q.m13;
  hjklm[11]= t1-t2+t3;
  tz    += hjklm[11]*d1333;
  hjklm[11]= hjklm[11]*4.0;
  t1    = w22*w2*w3;
  t2    = (w22*q.m23*3.+w2*w3*q.m22*3.);
  t3    = 3*q.m22*q.m23;
  hjklm[12]= t1-t2+t3;
  tz    += hjklm[12]*d2223;
  hjklm[12]= hjklm[12]*4.0;
  t1    = w33*w3*w2;
  t2    = (w33*q.m23*3.+w2*w3*q.m33*3.);
  t3    = 3.*q.m33*q.m23;
  hjklm[14]= t1-t2+t3;
  tz    += hjklm[14]*d2333;
  hjklm[14]= hjklm[14]*4.0;
  t     += tz*4.0;
  tz    = 0.0;
  t1    = w11*w22;
  t2    = (w11*q.m22+4.*w1*w2*q.m12+w22*q.m11);
  t3    = q.m11*q.m22+2.*q.m12*q.m12;
  hjklm[5] = t1-t2+t3;
  tz    = hjklm[5]*d1122;
  t1    = w11*w33;
  t2    = (w11*q.m33+4.*w1*w3*q.m13+w33*q.m11);
  t3    = q.m11*q.m33+2.*q.m13*q.m13;
  hjklm[7] = t1-t2+t3;
  tz    += hjklm[7]*d1133;
  hjklm[7] = hjklm[7]*6.0;
  t1    = w33*w22;
  t2    = (w33*q.m22+4.*w3*w2*q.m23+w22*q.m33);
  t3    = q.m33*q.m22+2.*q.m23*q.m23;
  hjklm[13]= t1-t2+t3;
  tz    += hjklm[13]*d2233;
  hjklm[13]= hjklm[13]*6.0;
  t     += tz*6.0;
  tz    = 0.0;
  t1    = w22*w3*w1;
  t2    = (w22*q.m13+2.*w2*w3*q.m12+2.*w2*w1*q.m23 + w3*w1*q.m22);
  t3    = q.m22*q.m13+2.*q.m23*q.m12;
  hjklm[9]= t1-t2+t3;
  tz    = hjklm[9]*d1223;
  hjklm[9]= hjklm[9]*12.0;
  t1    = w1*w2*w33;
  t2    = (w1*w2*q.m33+w2*w3*q.m13*2.+w33*q.m12 + 2.*w1*w3*q.m23);
  t3    = q.m12*q.m33+2.*q.m13*q.m23;
  hjklm[10]= t1-t2+t3;
  tz    += hjklm[10]*d1233;
  hjklm[10]= hjklm[10]*12.0;
  t1    = w11*w2*w3;
  t2    = (w11*q.m23+2.*w1*w2*q.m13+2.*w1*w3*q.m12 + w2*w3*q.m11);
  t3    = q.m11*q.m23+2.*q.m12*q.m13;
  hjklm[6] = t1-t2+t3;
  tz    += hjklm[6]*d1123;
  hjklm[6] = hjklm[6]*12.0;
  return  (t + tz*12.0)* 1.E-4;
}

double hermite3(V3 w, Matrix q,
		double c111,double c222,double c333,
		double c112,double c122,double c113,
		double c133,double c223,double c233,
		double c123){
  double t2,t=0,tz=0;
  double w1=w.x,
	 w2=w.y,
	 w3=w.z,
	 w11=w.x*w.x,
	 w22=w.y*w.y,
	 w33=w.z*w.z;
  t2 = 3.*w1*q.m11;
  t  = (w11*w1-t2)*c111;
  t2 = 3.*w2*q.m22;
  t  += (w22*w2-t2)*c222;
  t2 = 3.*w3*q.m33;
  t  += (w33*w3-t2)*c333;
  t2 = (2.*w1*q.m12+w2*q.m11);
  tz = (w11*w2-t2)*c112;
  t2 = (w1*q.m22+2.*w2*q.m12);
  tz += (w22*w1-t2)*c122;
  t2 = (2.*w1*q.m13+w3*q.m11);
  tz += (w11*w3-t2)*c113;
  t2 = (w1*q.m33+2.*w3*q.m13);
  tz += (w1*w33-t2)*c133;
  t2 = (2.*w2*q.m23+w3*q.m22);
  tz += (w22*w3-t2)*c223;
  t2 = (w2*q.m33+2.*w3*q.m23);
  tz += (w2*w33-t2)*c233;
  t  += tz*3.0;
  t2 = (w1*q.m23+w2*q.m13+w3*q.m12);
  tz = (w1*w2*w3-t2)*c123*6.0;
  return (t + tz)* 1.E-3;
  //CUM3  = CUM3 * 1.E-3
}

void MyWindow::pdfDlg(){
    int ind=0;
  QAction *action = qobject_cast<QAction *>(sender());
  if (action!=NULL){
      ind=action->data().toInt();
      ind=qMax(ind,0);
      ind=qMin(ind,xdinp.size()-1);
  }
  QDialog *dlg=new QDialog(this);
  dlg->setWindowTitle("Probability Density Function");
  QComboBox *atomBx = new QComboBox(this);
  for (int i=0; i<xdinp.size();i++){
  if (xdinp[i].OrdZahl<0)continue;
  if (xdinp[i].sg) continue;
  atomBx->addItem(QString(xdinp[i].atomname),i);
  }
  atomBx->setCurrentIndex(ind);
  QDoubleSpinBox *proba=new QDoubleSpinBox(this);
  proba->setMinimum(1);
  proba->setMaximum(99);
  proba->setDecimals(0);
  proba->setValue(50);
  QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));  
  QCheckBox *sec =new QCheckBox("Second order");
  sec->setChecked(true);
  QCheckBox *third =new QCheckBox("Third order");
  third->setChecked(true); 
  QCheckBox *fourth =new QCheckBox("Fourth order");
  fourth->setChecked(true);
  QCheckBox *coarse =new QCheckBox("Use a coarse grid");
  coarse->setChecked(false);
  QCheckBox *mapping =new QCheckBox("Map harmonic p.d.f. value on iso surface");
  mapping->setChecked(false);
  QCheckBox *minus99 =new QCheckBox("Draw -99% contour if p.d.f. is negative");
  minus99->setChecked(true);

  QLabel *l = new QLabel("Probability in %");
  QGridLayout *la= new QGridLayout();
  la->addWidget(atomBx,1,1,1,2);
  la->addWidget(l,2,1);
  la->addWidget(proba,2,2,1,2);
  la->addWidget(sec,3,1);
  la->addWidget(third,3,2);
  la->addWidget(fourth,3,3);
  la->addWidget(coarse,4,1);
  la->addWidget(mapping,4,2);
  la->addWidget(minus99,4,3);
  la->addWidget(buttonBox,10,1,1,3);
  dlg->setLayout(la);
  if(dlg->exec()==QDialog::Accepted){
      infoKanalNews("<b>Running p.d.f. via FFT please be patient!</b>");
      update();

    int i=atomBx->itemData(atomBx->currentIndex()).toInt();
    int options=(coarse->isChecked())?1:0;
    options|=(sec->isChecked())?2:0;
    options|=(third->isChecked())?4:0;
    options|=(fourth->isChecked())?8:0;
    options|=(mapping->isChecked())?16:0;
    options|=(minus99->isChecked())?32:0;
    printf("Atom i=%d probability %g options %d %g %g\n",i,proba->value(),options,kuhs(3,xdinp[i]),kuhs(4,xdinp[i]));
    fmcq->PDFbyFFT(i,options,proba->value());
    //loadFile(dirName);
    //cubeGL->moliso->L=cubeGL->L;
    showface->setChecked ( true );
    showface->setVisible(true);
    showLeg->setChecked ( true );
    showLeg->setVisible(true);
    movLeg->setChecked ( false );
    movLeg->setVisible(true);
    mildir->setChecked ( true  );
    mildir->setVisible(true);
    setCursor(Qt::ArrowCursor);
    //dock->hide();
    dock2->hide();
    cubeGL->setVisible ( true );
    createmoliso->setVisible(false);
    noMoliso->setVisible(true);  
    cubeGL->pause=false;
    cubeGL->updateGL();
    dock3 = new QDockWidget("Moliso control area",this);
    dock3->setAllowedAreas(Qt::AllDockWidgetAreas);
    QGroupBox *zebraZwinger= new QGroupBox();
    QVBoxLayout *zla= new QVBoxLayout();
    mt = new QCheckBox("transparence");
    mt->setChecked(true);
    mt->setShortcut(tr("Alt+T"));
    cubeGL->togglMolisoTransparence(true);
    zla->addWidget(mt);
    strikesSldr = new QSlider(Qt::Horizontal);
    strikesSldr->setMaximum(510);
    strikesSldr->setMinimum(127);
    strikesSldr->setValue(496);
    strikesSldr->setVisible(false);
    swidthSldr = new QSlider(Qt::Horizontal);
    swidthSldr->setMaximum(128);
    swidthSldr->setMinimum(1);
    swidthSldr->setValue(1);
    swidthSldr->setVisible(false);
    zebraBox = new QCheckBox("Show contour belts");
    zebraBox->setChecked(false);
  zebraBox->setShortcut(tr("F8"));
  zebraBox->setVisible(false);
  legendSize = new QSlider(Qt::Horizontal);
  legendSize->setValue(30);
  legendSize->setMaximum(100);
  cullNone = new QRadioButton("No culling");
  cullBack = new QRadioButton("Back face culling");
  cullFront = new QRadioButton("Front face culling");
  cullNone->setChecked(true);
  cubeGL->noneCull(true);
  connect(cullNone,SIGNAL(toggled(bool)),cubeGL,SLOT(noneCull(bool)));
  connect(cullBack,SIGNAL(toggled(bool)),cubeGL,SLOT(backCull(bool)));
  connect(cullFront,SIGNAL(toggled(bool)),cubeGL,SLOT(frontCull(bool)));
  zla->addWidget(cullNone);
  zla->addWidget(cullBack);
  zla->addWidget(cullFront);
  QToolBar *tb = new QToolBar("Moliso toolbar",zebraZwinger);
  tb->setOrientation(Qt::Vertical);
  tb->setIconSize(QSize(16,16));
  tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  tb->addAction(showface);
  tb->addAction(showLeg);
  tb->addAction(movLeg);
  tb->addAction(mildir);
  zla->addWidget(tb);
  mclmox =new QCheckBox("Monochrome legend text");
  mclmox->setChecked(false);
  connect(mclmox,SIGNAL(toggled(bool)),cubeGL,SLOT(togglMonoChrom(bool)));

  zla->addWidget(mclmox);
  zla->addWidget(new QLabel("Scale legend"));
  zla->addWidget(legendSize);
  QPushButton *savset = new QPushButton("Save current settings");
  connect(savset,SIGNAL(pressed()),cubeGL,SLOT(saveMISettings()));
  QPushButton *lodset = new QPushButton("Load settings");
  connect(lodset,SIGNAL(pressed()),cubeGL,SLOT(loadMISettings()));
  mlf = new QFontComboBox();
  mlf->setCurrentFont(cubeGL->MLegendFont);
  connect(mlf,SIGNAL(currentFontChanged(QFont)),cubeGL,SLOT(setMLFont(QFont)));
  fos = new QSpinBox();
  fos->setMinimum(4);
  fos->setMaximum(272);
  fos->setValue(cubeGL->MLegendFont.pointSize());
  connect(fos,SIGNAL(valueChanged(int)),cubeGL,SLOT(setMLFontSize(int)));
  zla->addWidget(mlf);
  zla->addWidget(fos);
  zla->addWidget(savset);
  zla->addWidget(lodset);
  zla->addStretch(999);
  connect(cubeGL,SIGNAL(mconf()),this,SLOT(syncMconf()));
  cubeGL->togglContours(false);
  cubeGL->scaleLegend(30);
  cubeGL->setContourCnt(496);
  cubeGL->setContourWidth(1);
  connect(mt,SIGNAL(toggled(bool)),cubeGL,SLOT(togglMolisoTransparence(bool)));
  connect(legendSize,SIGNAL(valueChanged(int)),cubeGL,SLOT(scaleLegend(int)));
  zebraZwinger->setLayout(zla);
  dock3->setWidget(zebraZwinger);
  dock3->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetClosable);
  addDockWidget(Qt::LeftDockWidgetArea, dock3);
  QMainWindow::tabifyDockWidget (dock2,dock3);
  QMainWindow::tabifyDockWidget (dock2,dock);
    //makePDFGrid(xdinp[i],proba->value(),sec->isChecked(),third->isChecked(),fourth->isChecked());
  }

  delete dlg;
}

void MyWindow::pdfDlg2(){

  QDialog *dlg=new QDialog(this);
  dlg->setWindowTitle("Probability Density Function");
  QDoubleSpinBox *proba=new QDoubleSpinBox(this);
  proba->setMinimum(1);
  proba->setMaximum(99);
  proba->setDecimals(0);
  proba->setValue(50);
  QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));  
  QCheckBox *sec =new QCheckBox("Second order");
  sec->setChecked(true);
  QCheckBox *third =new QCheckBox("Third order");
  third->setChecked(true); 
  QCheckBox *fourth =new QCheckBox("Fourth order");
  fourth->setChecked(true);
  QLabel *l = new QLabel("Probability in %");
  QGridLayout *la= new QGridLayout();
  la->addWidget(l,2,1);
  la->addWidget(proba,2,2,1,2);
  la->addWidget(sec,3,1);
  la->addWidget(third,3,2);
  la->addWidget(fourth,3,3);
  la->addWidget(buttonBox,4,1,1,3);
  dlg->setLayout(la);
  if(dlg->exec()==QDialog::Accepted){
    makePDFGrids(proba->value(),sec->isChecked(),third->isChecked(),fourth->isChecked());
  }

  delete dlg;
}

void MyWindow::directHirsh(){
  printf("MyWindow::directHirsh\n");
  cubeGL->pause=true;
  if (cubeGL->moliso){
    glDeleteLists(cubeGL->moliso->mibas,6);
    if (cubeGL->moliso){
      delete cubeGL->moliso;
      cubeGL->moliso=NULL;
    }
    delete dock3;
    dock->show();
    dock2->show();
    createmoliso->setVisible(true);
    noMoliso->setVisible(false);
  }
  cubeGL->moliso = new MolIso();
  cubeGL->moliso->L=cubeGL->L;
  printf("?\n");
  cubeGL->moliso->fixmin=666.666;
  cubeGL->moliso->fixmax=666.666;
  printf("?\n");
  hirshfeld->forMolIso(cubeGL->moliso);
  cubeGL->MIS=true;
  cubeGL->MILe=true;
  showface->setChecked ( true );
  showface->setVisible(true);
  showLeg->setChecked ( true );
  showLeg->setVisible(true);
  movLeg->setChecked ( false );
  movLeg->setVisible(true);
  mildir->setChecked ( true  );
  mildir->setVisible(true);
  setCursor(Qt::ArrowCursor);
  dock->hide();
  dock2->hide();
  cubeGL->setVisible ( true );
  createmoliso->setVisible(false);
  noMoliso->setVisible(true);  
  cubeGL->pause=false;
  cubeGL->updateGL();
  dock3 = new QDockWidget("Moliso control area",this);
  dock3->setAllowedAreas(Qt::AllDockWidgetAreas);
  QGroupBox *zebraZwinger= new QGroupBox();
  QVBoxLayout *zla= new QVBoxLayout();
  mt = new QCheckBox("transparence");
  mt->setChecked(true);
  mt->setShortcut(tr("T"));
  cubeGL->togglMolisoTransparence(true);
  zla->addWidget(mt);
  strikesSldr = new QSlider(Qt::Horizontal);
  strikesSldr->setMaximum(510);
  strikesSldr->setMinimum(127);
  strikesSldr->setValue(496);
  strikesSldr->setVisible(false);
  swidthSldr = new QSlider(Qt::Horizontal);
  swidthSldr->setMaximum(128);
  swidthSldr->setMinimum(1);
  swidthSldr->setValue(1);
  swidthSldr->setVisible(false);
  zebraBox = new QCheckBox("Show contour belts");
  zebraBox->setChecked(false);
  zebraBox->setShortcut(tr("F8"));
  zebraBox->setVisible(false);
  legendSize = new QSlider(Qt::Horizontal);
  legendSize->setValue(30);
  legendSize->setMaximum(100);
  cullNone = new QRadioButton("No culling");
  cullBack = new QRadioButton("Back face culling");
  cullFront = new QRadioButton("Front face culling");
  cullNone->setChecked(true);
  cubeGL->noneCull(true);
  connect(cullNone,SIGNAL(toggled(bool)),cubeGL,SLOT(noneCull(bool)));
  connect(cullBack,SIGNAL(toggled(bool)),cubeGL,SLOT(backCull(bool)));
  connect(cullFront,SIGNAL(toggled(bool)),cubeGL,SLOT(frontCull(bool)));
  zla->addWidget(cullNone);
  zla->addWidget(cullBack);
  zla->addWidget(cullFront);
  QToolBar *tb = new QToolBar("Moliso toolbar",zebraZwinger);
  tb->setOrientation(Qt::Vertical);
  tb->setIconSize(QSize(16,16));
  tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  tb->addAction(showface);
  tb->addAction(showLeg);
  tb->addAction(movLeg);
  tb->addAction(mildir);
  zla->addWidget(tb);
  mclmox =new QCheckBox("Monochrome legend text");
  mclmox->setChecked(false);
  connect(mclmox,SIGNAL(toggled(bool)),cubeGL,SLOT(togglMonoChrom(bool)));

  zla->addWidget(mclmox);
  zla->addWidget(new QLabel("Scale legend"));
  zla->addWidget(legendSize);
  QPushButton *savset = new QPushButton("Save current settings");
  connect(savset,SIGNAL(pressed()),cubeGL,SLOT(saveMISettings()));
  QPushButton *lodset = new QPushButton("Load settings");
  connect(lodset,SIGNAL(pressed()),cubeGL,SLOT(loadMISettings()));
  mlf = new QFontComboBox();
  mlf->setCurrentFont(cubeGL->MLegendFont);
  connect(mlf,SIGNAL(currentFontChanged(QFont)),cubeGL,SLOT(setMLFont(QFont)));
  fos = new QSpinBox();
  fos->setMinimum(4);
  fos->setMaximum(272);
  fos->setValue(cubeGL->MLegendFont.pointSize());
  connect(fos,SIGNAL(valueChanged(int)),cubeGL,SLOT(setMLFontSize(int)));
  zla->addWidget(mlf);
  zla->addWidget(fos);
  zla->addWidget(savset);
  zla->addWidget(lodset);
  QPushButton *expWFobj= new QPushButton("Export surfaces");
  connect(expWFobj,SIGNAL(pressed()),  cubeGL->moliso,SLOT(exportObj()));
  QPushButton *expSTL= new QPushButton("Export surfaces *.stl");
  connect(expSTL,SIGNAL(pressed()),  cubeGL->moliso,SLOT(exportSTL()));
  zla->addWidget(expWFobj);
  zla->addWidget(expSTL);
  zla->addStretch(999);
  connect(cubeGL,SIGNAL(mconf()),this,SLOT(syncMconf()));
  cubeGL->togglContours(false);
  cubeGL->scaleLegend(30);
  cubeGL->setContourCnt(496);
  cubeGL->setContourWidth(1);
  cubeGL->selectedAtoms.clear();
  connect(mt,SIGNAL(toggled(bool)),cubeGL,SLOT(togglMolisoTransparence(bool)));
  connect(legendSize,SIGNAL(valueChanged(int)),cubeGL,SLOT(scaleLegend(int)));
  zebraZwinger->setLayout(zla);
  dock3->setWidget(zebraZwinger);
  dock3->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetClosable);
  addDockWidget(Qt::LeftDockWidgetArea, dock3);
  QMainWindow::tabifyDockWidget (dock2,dock3);
  QMainWindow::tabifyDockWidget (dock2,dock);
///DIRECT HIRSH
}

void MyWindow::makePDFGrids(double proba,bool c2,bool c3,bool c4){
  QString afac("testpdf2.face");
  QString fac("testpdf1.face");
  QString nfac("testpdf3.face");
  int breite=91;
  int z=0;
  if (QFile::exists(fac)) QFile::remove(fac);
  if (QFile::exists(nfac)) QFile::remove(nfac);
  if (QFile::exists(afac)) QFile::remove(afac);
  if (cubeGL->moliso){
    glDeleteLists(cubeGL->moliso->mibas,6);
    if (cubeGL->moliso){
      delete cubeGL->moliso;
      cubeGL->moliso=NULL;
    }
    delete dock3;
    dock->show();
    dock2->show();
    createmoliso->setVisible(true);
    noMoliso->setVisible(false);
  }
  cubeGL->moliso = new MolIso();
  cubeGL->moliso->L=cubeGL->L;
  cubeGL->moliso->lineNr=0;
  QProgressDialog progress(QString("Calculating PDF"),QString(),0,breite*breite*breite*asymmUnit.size());
  for (int ix=0; ix<asymmUnit.size(); ix++){
    if (asymmUnit.at(ix).OrdZahl<0)continue;
    if (asymmUnit.at(ix).jtf<2)continue;
  int ppp=(int)(proba);
  double piso=0;
  double p=0;
  Matrix U=asymmUnit.at(ix).u;//kartesian U
  Matrix UI=inverse(U);
  double DI=determinant(UI);
  double base=sqrt(DI)/sqrt((8*M_PI*M_PI*M_PI));
  switch (ppp){
	  case 1 :   piso=-0.5*(0.3389*0.3389);break;
	  case 2 :   piso=-0.5*(0.4299*0.4299);break;
	  case 3 :   piso=-0.5*(0.4951*0.4951);break;
	  case 4 :   piso=-0.5*(0.5479*0.5479);break;
	  case 5 :   piso=-0.5*(0.5932*0.5932);break;
	  case 6 :   piso=-0.5*(0.6334*0.6334);break;
	  case 7 :   piso=-0.5*(0.6699*0.6699);break;
	  case 8 :   piso=-0.5*(0.7035*0.7035);break;
	  case 9 :   piso=-0.5*(0.7349*0.7349);break;
	  case 10:   piso=-0.5*(0.7644*0.7644);break;
	  case 11:   piso=-0.5*(0.7924*0.7924);break;
	  case 12:   piso=-0.5*(0.8192*0.8192);break;
	  case 13:   piso=-0.5*(0.8447*0.8447);break;
	  case 14:   piso=-0.5*(0.8694*0.8694);break;
	  case 15:   piso=-0.5*(0.8932*0.8932);break;
	  case 16:   piso=-0.5*(0.9162*0.9162);break;
	  case 17:   piso=-0.5*(0.9386*0.9386);break;
	  case 18:   piso=-0.5*(0.9605*0.9605);break;
	  case 19:   piso=-0.5*(0.9818*0.9818);break;
	  case 20:   piso=-0.5*(1.0026*1.0026);break;
	  case 21:   piso=-0.5*(1.0230*1.0230);break;
	  case 22:   piso=-0.5*(1.0430*1.0430);break;
	  case 23:   piso=-0.5*(1.0627*1.0627);break;
	  case 24:   piso=-0.5*(1.0821*1.0821);break;
	  case 25:   piso=-0.5*(1.1012*1.1012);break;
	  case 26:   piso=-0.5*(1.1200*1.1200);break;
	  case 27:   piso=-0.5*(1.1386*1.1386);break;
	  case 28:   piso=-0.5*(1.1570*1.1570);break;
	  case 29:   piso=-0.5*(1.1751*1.1751);break;
	  case 30:   piso=-0.5*(1.1932*1.1932);break;
	  case 31:   piso=-0.5*(1.2110*1.2110);break;
	  case 32:   piso=-0.5*(1.2288*1.2288);break;
	  case 33:   piso=-0.5*(1.2464*1.2464);break;
	  case 34:   piso=-0.5*(1.2638*1.2638);break;
	  case 35:   piso=-0.5*(1.2812*1.2812);break;
	  case 36:   piso=-0.5*(1.2985*1.2985);break;
	  case 37:   piso=-0.5*(1.3158*1.3158);break;
	  case 38:   piso=-0.5*(1.3330*1.3330);break;
	  case 39:   piso=-0.5*(1.3501*1.3501);break;
	  case 40:   piso=-0.5*(1.3672*1.3672);break;
	  case 41:   piso=-0.5*(1.3842*1.3842);break;
	  case 42:   piso=-0.5*(1.4013*1.4013);break;
	  case 43:   piso=-0.5*(1.4183*1.4183);break;
	  case 44:   piso=-0.5*(1.4354*1.4354);break;
	  case 45:   piso=-0.5*(1.4524*1.4524);break;
	  case 46:   piso=-0.5*(1.4695*1.4695);break;
	  case 47:   piso=-0.5*(1.4866*1.4866);break;
	  case 48:   piso=-0.5*(1.5037*1.5037);break;
	  case 49:   piso=-0.5*(1.5209*1.5209);break;
	  case 50:   piso=-0.5*(1.5382*1.5382);break;
	  case 51:   piso=-0.5*(1.5555*1.5555);break;
	  case 52:   piso=-0.5*(1.5729*1.5729);break;
	  case 53:   piso=-0.5*(1.5904*1.5904);break;
	  case 54:   piso=-0.5*(1.6080*1.6080);break;
	  case 55:   piso=-0.5*(1.6257*1.6257);break;
	  case 56:   piso=-0.5*(1.6436*1.6436);break;
	  case 57:   piso=-0.5*(1.6616*1.6616);break;
	  case 58:   piso=-0.5*(1.6797*1.6797);break;
	  case 59:   piso=-0.5*(1.6980*1.6980);break;
	  case 60:   piso=-0.5*(1.7164*1.7164);break;
	  case 61:   piso=-0.5*(1.7351*1.7351);break;
	  case 62:   piso=-0.5*(1.7540*1.7540);break;
	  case 63:   piso=-0.5*(1.7730*1.7730);break;
	  case 64:   piso=-0.5*(1.7924*1.7924);break;
	  case 65:   piso=-0.5*(1.8119*1.8119);break;
	  case 66:   piso=-0.5*(1.8318*1.8318);break;
	  case 67:   piso=-0.5*(1.8519*1.8519);break;
	  case 68:   piso=-0.5*(1.8724*1.8724);break;
	  case 69:   piso=-0.5*(1.8932*1.8932);break;
	  case 70:   piso=-0.5*(1.9144*1.9144);break;
	  case 71:   piso=-0.5*(1.9360*1.9360);break;
	  case 72:   piso=-0.5*(1.9580*1.9580);break;
	  case 73:   piso=-0.5*(1.9804*1.9804);break;
	  case 74:   piso=-0.5*(2.0034*2.0034);break;
	  case 75:   piso=-0.5*(2.0269*2.0269);break;
	  case 76:   piso=-0.5*(2.0510*2.0510);break;
	  case 77:   piso=-0.5*(2.0757*2.0757);break;
	  case 78:   piso=-0.5*(2.1012*2.1012);break;
	  case 79:   piso=-0.5*(2.1274*2.1274);break;
	  case 80:   piso=-0.5*(2.1544*2.1544);break;
	  case 81:   piso=-0.5*(2.1824*2.1824);break;
	  case 82:   piso=-0.5*(2.2114*2.2114);break;
	  case 83:   piso=-0.5*(2.2416*2.2416);break;
	  case 84:   piso=-0.5*(2.2730*2.2730);break;
	  case 85:   piso=-0.5*(2.3059*2.3059);break;
	  case 86:   piso=-0.5*(2.3404*2.3404);break;
	  case 87:   piso=-0.5*(2.3767*2.3767);break;
	  case 88:   piso=-0.5*(2.4153*2.4153);break;
	  case 89:   piso=-0.5*(2.4563*2.4563);break;
	  case 90:   piso=-0.5*(2.5003*2.5003);break;
	  case 91:   piso=-0.5*(2.5478*2.5478);break;
	  case 92:   piso=-0.5*(2.5997*2.5997);break;
	  case 93:   piso=-0.5*(2.6571*2.6571);break;
	  case 94:   piso=-0.5*(2.7216*2.7216);break;
	  case 95:   piso=-0.5*(2.7955*2.7955);break;
	  case 96:   piso=-0.5*(2.8829*2.8829);break;
	  case 97:   piso=-0.5*(2.9912*2.9912);break;
	  case 98:   piso=-0.5*(3.1365*3.1365);break;
	  case 99:   piso=-0.5*(3.3682*3.3682);break;
  }
  piso=base*exp(piso);
  printf("Piso= %g %s %d %g\n",piso,xdinp.at(ix).atomname,ppp,proba);

  if (QFile::exists(fac)) {
 //   qDebug()<<afac<<fac;
    if (QFile::exists(afac)) QFile::remove(afac);
    QFile::copy(fac,afac);
  }
  tensmul(asymmUnit[ix]);

  V3 X=V3(1,0,0);
  V3 ev=V3(1,1,1);
//  Matrix evk=
	  mol.jacobi(U,ev);

  //printf("%g %g %g  \n",ev.x,ev.y,ev.z);
  double maxdim=2*sqrt(fmax(ev.x,fmax(ev.y,ev.z)))*6;

  double pt;//p10,p90,p50,
  double ponent;
  //p50=base*exp(-1.18302962);
  //p10=base*exp(-0.29215368);
  //p90=base*exp(-3.12575004);
  double df=maxdim/breite;
  cubeGL->moliso->orig=Vector3(xdinp.at(ix).kart.x,xdinp.at(ix).kart.y,xdinp.at(ix).kart.z);
/*  printf("%g %g %g   %g %g %g  %s\n",cubeGL->moliso->orig.x, cubeGL->moliso->orig.y, cubeGL->moliso->orig.z,
		  xdinp.at(ix).kart.x,xdinp.at(ix).kart.y,xdinp.at(ix).kart.z,
		  xdinp.at(ix).atomname);// */
  cubeGL->moliso->x_dim=Vector3(df,0,0);
  cubeGL->moliso->y_dim=Vector3(0,df,0);
  cubeGL->moliso->z_dim=Vector3(0,0,df);
  cubeGL->moliso->breite=cubeGL->moliso->hoehe=cubeGL->moliso->tiefe=breite;
  cubeGL->moliso->data.clear();
  cubeGL->moliso->mdata.clear();
//  double cfact=6/(M_PI*M_PI*M_PI*8);
//  tensmul(atom,roma);
  
/*  printf(
		  "u111 %14.7g u222 %14.7g u333 %14.7g \n"
		  "u112 %14.7g u122 %14.7g u113 %14.7g \n"
		  "u133 %14.7g u223 %14.7g u233 %14.7g \n"
	          "u123 %14.7g \n",
		  atom.c111,atom.c222,atom.c333,
		  atom.c112,atom.c122,atom.c113,
		  atom.c133,atom.c223,atom.c233,
		  atom.c123);
  printf(
		  "c111 %14.7g c222 %14.7g c333 %14.7g \n"
		  "c112 %14.7g c122 %14.7g c113 %14.7g \n"
		  "c133 %14.7g c223 %14.7g c233 %14.7g \n"
	          "c123 %14.7g \n",
		  atom.c111*mol.zelle.as*mol.zelle.as*mol.zelle.as,
		  atom.c222*mol.zelle.bs*mol.zelle.bs*mol.zelle.bs,
		  atom.c333*mol.zelle.cs*mol.zelle.cs*mol.zelle.cs,
		  atom.c112*mol.zelle.as*mol.zelle.as*mol.zelle.bs,
		  atom.c122*mol.zelle.as*mol.zelle.bs*mol.zelle.bs,
		  atom.c113*mol.zelle.as*mol.zelle.as*mol.zelle.cs,
		  atom.c133*mol.zelle.as*mol.zelle.cs*mol.zelle.cs,
		  atom.c223*mol.zelle.bs*mol.zelle.bs*mol.zelle.cs,
		  atom.c233*mol.zelle.bs*mol.zelle.cs*mol.zelle.cs,
		  atom.c123*mol.zelle.as*mol.zelle.bs*mol.zelle.cs);
// */
  double third=0,fourth=0,tmax=0,tmin=0;
  V3 t=V3(((breite-1)/-2.0)*df,((breite-1)/-2.0)*df,((breite-1)/-2.0)*df);
//  QString txt;
  for (int k=0; k<breite; k++)
      for (int j=0; j<breite; j++)
          for (int i=0; i<breite; i++){
      X=V3(i*df,j*df,k*df)+t;
      ponent=((X*UI)*X)/-2.0;
      V3 w=(X*UI);

      p=base*exp(ponent);
      if ((c3)&&(asymmUnit.at(ix).jtf>2)) third= p*hermite3(w,UI,
                      asymmUnit.at(ix).c111,asymmUnit.at(ix).c222,asymmUnit.at(ix).c333,
                      asymmUnit.at(ix).c112,asymmUnit.at(ix).c122,asymmUnit.at(ix).c113,
                      asymmUnit.at(ix).c133,asymmUnit.at(ix).c223,asymmUnit.at(ix).c233,
                      asymmUnit.at(ix).c123);
      if ((c4)&&(asymmUnit.at(ix).jtf==4)) fourth= p*hermite4(w,UI,
		       asymmUnit.at(ix).d1111,asymmUnit.at(ix).d2222,asymmUnit.at(ix).d3333,
		       asymmUnit.at(ix).d1112,asymmUnit.at(ix).d1113,asymmUnit.at(ix).d1122,
		       asymmUnit.at(ix).d1123,asymmUnit.at(ix).d1133,asymmUnit.at(ix).d1222,
                       asymmUnit.at(ix).d1223,asymmUnit.at(ix).d1233,asymmUnit.at(ix).d1333,
		       asymmUnit.at(ix).d2223,asymmUnit.at(ix).d2233,asymmUnit.at(ix).d2333);
      tmin=fmin(tmin,third);
      tmax=fmax(tmax,third);
      pt=p*((c2?1:0) + third/6.0 + fourth/24.0);
      cubeGL->moliso->data.append(pt);
//      txt.append(QString("%1").arg(pt,13,'E',5));
      z++;
  //    if (!(z%6))txt.append("\n");
      if (!(z%(breite*breite*breite/100))) progress.setValue(z);
  //    if (!(z%(breite*breite*breite/100))) {printf(">");fflush(stdout);}
  }
  progress.setValue(z);
  cubeGL->moliso->iso_level=(c2)?piso:0.01;
  cubeGL->moliso->createSurface(fac,(c2)?proba:0.01,0.01,false,true);
  if ((QFile::exists(afac))&&(QFile::exists(fac))){
//    qDebug() << afac  << fac << nfac<< "addface!";
    addfaze(afac,fac,nfac);
//    QMessageBox::information(this,afac,nfac);
    QFile::remove(fac);
    QFile::copy(nfac,fac);
  }
  }

  cubeGL->pause=true;
  if (cubeGL->moliso->mibas) glDeleteLists(cubeGL->moliso->mibas,6);
  cubeGL->moliso->mibas=glGenLists(6);
  statusBar()->showMessage(tr("loading surfaces...") );
  //qDebug()<<cubeGL->moliso->mibas<<cubeGL->foubas[0];
  //printf("L= %g   %g %g %g %s\n",cubeGL->moliso->L, cubeGL->moliso->orig.x, cubeGL->moliso->orig.y, cubeGL->moliso->orig.z,fac.toStdString().c_str());

  cubeGL->moliso->min=-98.999;
  cubeGL->moliso->max= 2.0*(proba-cubeGL->moliso->min)+cubeGL->moliso->min;
  cubeGL->moliso->loadMI(fac,false);
  updateStatusBar();
  statusBar()->showMessage(tr("surfaces loaded") );
  cubeGL->MIS=true;
  cubeGL->MILe=true;
  showface->setChecked ( true );
  showface->setVisible(true);
  showLeg->setChecked ( true );
  showLeg->setVisible(true);
  movLeg->setChecked ( false );
  movLeg->setVisible(true);
  mildir->setChecked ( true  );
  mildir->setVisible(true);
  setCursor(Qt::ArrowCursor);
  dock->hide();
  dock2->hide();
  cubeGL->setVisible ( true );
  createmoliso->setVisible(false);
  noMoliso->setVisible(true);  
  cubeGL->pause=false;
  cubeGL->updateGL();
  dock3 = new QDockWidget("Moliso control area",this);
  dock3->setAllowedAreas(Qt::AllDockWidgetAreas);
  QGroupBox *zebraZwinger= new QGroupBox();
  QVBoxLayout *zla= new QVBoxLayout();
  mt = new QCheckBox("transparence");
  mt->setChecked(true);
  mt->setShortcut(tr("T"));
  cubeGL->togglMolisoTransparence(true);
  zla->addWidget(mt);
  strikesSldr = new QSlider(Qt::Horizontal);
  strikesSldr->setMaximum(510);
  strikesSldr->setMinimum(127);
  strikesSldr->setValue(496);
  strikesSldr->setVisible(false);
  swidthSldr = new QSlider(Qt::Horizontal);
  swidthSldr->setMaximum(128);
  swidthSldr->setMinimum(1);
  swidthSldr->setValue(1);
  swidthSldr->setVisible(false);
  zebraBox = new QCheckBox("Show contour belts");
  zebraBox->setChecked(false);
  zebraBox->setShortcut(tr("F8"));
  zebraBox->setVisible(false);
  legendSize = new QSlider(Qt::Horizontal);
  legendSize->setValue(30);
  legendSize->setMaximum(100);
  cullNone = new QRadioButton("No culling");
  cullBack = new QRadioButton("Back face culling");
  cullFront = new QRadioButton("Front face culling");
  cullNone->setChecked(true);
  cubeGL->noneCull(true);
  connect(cullNone,SIGNAL(toggled(bool)),cubeGL,SLOT(noneCull(bool)));
  connect(cullBack,SIGNAL(toggled(bool)),cubeGL,SLOT(backCull(bool)));
  connect(cullFront,SIGNAL(toggled(bool)),cubeGL,SLOT(frontCull(bool)));
  zla->addWidget(cullNone);
  zla->addWidget(cullBack);
  zla->addWidget(cullFront);
  QToolBar *tb = new QToolBar("Moliso toolbar",zebraZwinger);
  tb->setOrientation(Qt::Vertical);
  tb->setIconSize(QSize(16,16));
  tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  tb->addAction(showface);
  tb->addAction(showLeg);
  tb->addAction(movLeg);
  tb->addAction(mildir);
  zla->addWidget(tb);
  mclmox =new QCheckBox("Monochrome legend text");
  mclmox->setChecked(false);
  connect(mclmox,SIGNAL(toggled(bool)),cubeGL,SLOT(togglMonoChrom(bool)));

  zla->addWidget(mclmox);
  zla->addWidget(new QLabel("Scale legend"));
  zla->addWidget(legendSize);
  QPushButton *savset = new QPushButton("Save current settings");
  connect(savset,SIGNAL(pressed()),cubeGL,SLOT(saveMISettings()));
  QPushButton *lodset = new QPushButton("Load settings");
  connect(lodset,SIGNAL(pressed()),cubeGL,SLOT(loadMISettings()));
  mlf = new QFontComboBox();
  mlf->setCurrentFont(cubeGL->MLegendFont);
  connect(mlf,SIGNAL(currentFontChanged(QFont)),cubeGL,SLOT(setMLFont(QFont)));
  fos = new QSpinBox();
  fos->setMinimum(4);
  fos->setMaximum(272);
  fos->setValue(cubeGL->MLegendFont.pointSize());
  connect(fos,SIGNAL(valueChanged(int)),cubeGL,SLOT(setMLFontSize(int)));
  zla->addWidget(mlf);
  zla->addWidget(fos);
  zla->addWidget(savset);
  zla->addWidget(lodset);
  zla->addStretch(999);
  connect(cubeGL,SIGNAL(mconf()),this,SLOT(syncMconf()));
  cubeGL->togglContours(false);
  cubeGL->scaleLegend(30);
  cubeGL->setContourCnt(496);
  cubeGL->setContourWidth(1);
  connect(mt,SIGNAL(toggled(bool)),cubeGL,SLOT(togglMolisoTransparence(bool)));
  connect(legendSize,SIGNAL(valueChanged(int)),cubeGL,SLOT(scaleLegend(int)));
  zebraZwinger->setLayout(zla);
  dock3->setWidget(zebraZwinger);
  dock3->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetClosable);
  addDockWidget(Qt::LeftDockWidgetArea, dock3);
  QMainWindow::tabifyDockWidget (dock2,dock3);
  QMainWindow::tabifyDockWidget (dock2,dock);
}  

void MyWindow::setfaze1(){
  faze1->setText(QFileDialog::getOpenFileName (this,"open Moliso surfaces file A"));
}
void MyWindow::setfaze2(){
  faze2->setText(QFileDialog::getOpenFileName (this,"open Moliso surfaces file B"));
}
void MyWindow::setfaze3(){
  faze3->setText(QFileDialog::getSaveFileName (this,"save Moliso surfaces file A+B"));
}

void MyWindow::addFazeDlg(){
  QDialog *dlg=new QDialog(this);
  QGridLayout *l=new QGridLayout(dlg);
  QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));  
  faze1 = new QLineEdit();
  QPushButton *f1brs = new QPushButton("Browse");
  connect(f1brs,SIGNAL(pressed()),this,SLOT(setfaze1())); 
  faze2 = new QLineEdit();
  QPushButton *f2brs = new QPushButton("Browse");
  connect(f2brs,SIGNAL(pressed()),this,SLOT(setfaze2())); 
  faze3 = new QLineEdit();
  QPushButton *f3brs = new QPushButton("Browse");
  connect(f3brs,SIGNAL(pressed()),this,SLOT(setfaze3())); 
  l->addWidget(new QLabel("input surface file A"), 0,0,1,1);
  l->addWidget(faze1,0,1,1,18);
  l->addWidget(f1brs,0,19,1,1);
  l->addWidget(new QLabel("input surface file B"), 1,0,1,1);
  l->addWidget(faze2,1,1,1,18);
  l->addWidget(f2brs,1,19,1,1);
  l->addWidget(new QLabel("output surface file A+B"), 2,0,1,1);
  l->addWidget(faze3,2,1,1,18);
  l->addWidget(f3brs,2,19,1,1);

  l->addWidget(buttonBox,20,0,1,20);
  bool ok=true;
  do{
  if(dlg->exec()==QDialog::Accepted) {
    if ((!faze1->text().isEmpty())&& (!faze2->text().isEmpty())&& (!faze3->text().isEmpty())) {
      addfaze(faze1->text(),faze2->text(),faze3->text());
      ok=true;
    }
    else ok=false;
  }
  }while (!ok);
}

void MyWindow::addfaze(const QString afac,const QString fac,const QString nface){
  QFile ONE(afac);
  QFile TWO(fac);
  QFile of(nface);
  bool ok=false;
  ok=ONE.open(QIODevice::ReadOnly|QIODevice::Text);
  if (!ok) {qDebug()<< "File open problem!"; return;}
  ok=TWO.open(QIODevice::ReadOnly|QIODevice::Text);
  if (!ok) {qDebug()<< "File open problem!"; return;}
  ok=of.open(QIODevice::WriteOnly|QIODevice::Text);
  if (!ok) {qDebug()<< "File open problem!"; return;}
  QStringList one=QString(ONE.readAll()).split('\n');
  QStringList two=QString(TWO.readAll()).split('\n');
  ONE.close();
  TWO.close();
  int offset1=one.at(0).toInt();
  int offset2=two.at(0).toInt();
   //qDebug()<<offset1<<offset2<<one.size()<<two.size()<<offset1+offset2<<one.size()+two.size();
  of.write(QString("%1\n").arg(offset1+offset2).toLatin1());
  for (int i=1;i<=offset1;i++){
    of.write(QString("%1\n").arg(one.at(i)).toLatin1());
  }
//  qDebug()<<"OK1";
  for (int i=1;i<=offset2;i++){
    of.write(QString("%1\n").arg(two.at(i)).toLatin1());
  }
 // qDebug()<<"OK2";
  for (int i=offset1+1;i<one.size();i++){
    of.write(QString("%1\n").arg(one.at(i)).toLatin1());
  }
 // qDebug()<<"OK3";
  for (int i=offset2+1;i<two.size();i++){
    QStringList numbers = two.at(i).split(' ',QString::SkipEmptyParts);
  //  QList<int> numbers = two.at(i).split(' ',QString::SkipEmptyParts);
    for (int k=0; k<numbers.size(); k++) {
      of.write(QString("%1 ").arg(numbers.at(k).toInt()+offset1).toLatin1());
    }
    of.write("\n");
  }
  of.close();
  //qDebug()<<"OK4";
}

void MyWindow::makePDFGrid(INP atom, double proba,bool c2,bool c3,bool c4){
  int ppp=(int)(proba);
  double piso=0;
  pdfOnAtom=-1;
  double p=0;
  Matrix U=atom.u;//kartesian U
  Matrix UI=inverse(U);
  double DI=determinant(UI);
  double base=sqrt(DI)/sqrt((8*M_PI*M_PI*M_PI));
  //base=sqrt(base);
//  base=1.0;
  double di=determinant(U);
  double BASE=sqrt(1.0/(di*8.0*M_PI*M_PI*M_PI));
  printf("base? %g %g %g  di %g %g  %d %d %d\n",base,BASE,base-BASE,DI,di,c2,c3,c4);
  switch (ppp){
	  case 1 :   piso=-0.5*(0.3389*0.3389);break;
	  case 2 :   piso=-0.5*(0.4299*0.4299);break;
	  case 3 :   piso=-0.5*(0.4951*0.4951);break;
	  case 4 :   piso=-0.5*(0.5479*0.5479);break;
	  case 5 :   piso=-0.5*(0.5932*0.5932);break;
	  case 6 :   piso=-0.5*(0.6334*0.6334);break;
	  case 7 :   piso=-0.5*(0.6699*0.6699);break;
	  case 8 :   piso=-0.5*(0.7035*0.7035);break;
	  case 9 :   piso=-0.5*(0.7349*0.7349);break;
	  case 10:   piso=-0.5*(0.7644*0.7644);break;
	  case 11:   piso=-0.5*(0.7924*0.7924);break;
	  case 12:   piso=-0.5*(0.8192*0.8192);break;
	  case 13:   piso=-0.5*(0.8447*0.8447);break;
	  case 14:   piso=-0.5*(0.8694*0.8694);break;
	  case 15:   piso=-0.5*(0.8932*0.8932);break;
	  case 16:   piso=-0.5*(0.9162*0.9162);break;
	  case 17:   piso=-0.5*(0.9386*0.9386);break;
	  case 18:   piso=-0.5*(0.9605*0.9605);break;
	  case 19:   piso=-0.5*(0.9818*0.9818);break;
	  case 20:   piso=-0.5*(1.0026*1.0026);break;
	  case 21:   piso=-0.5*(1.0230*1.0230);break;
	  case 22:   piso=-0.5*(1.0430*1.0430);break;
	  case 23:   piso=-0.5*(1.0627*1.0627);break;
	  case 24:   piso=-0.5*(1.0821*1.0821);break;
	  case 25:   piso=-0.5*(1.1012*1.1012);break;
	  case 26:   piso=-0.5*(1.1200*1.1200);break;
	  case 27:   piso=-0.5*(1.1386*1.1386);break;
	  case 28:   piso=-0.5*(1.1570*1.1570);break;
	  case 29:   piso=-0.5*(1.1751*1.1751);break;
	  case 30:   piso=-0.5*(1.1932*1.1932);break;
	  case 31:   piso=-0.5*(1.2110*1.2110);break;
	  case 32:   piso=-0.5*(1.2288*1.2288);break;
	  case 33:   piso=-0.5*(1.2464*1.2464);break;
	  case 34:   piso=-0.5*(1.2638*1.2638);break;
	  case 35:   piso=-0.5*(1.2812*1.2812);break;
	  case 36:   piso=-0.5*(1.2985*1.2985);break;
	  case 37:   piso=-0.5*(1.3158*1.3158);break;
	  case 38:   piso=-0.5*(1.3330*1.3330);break;
	  case 39:   piso=-0.5*(1.3501*1.3501);break;
	  case 40:   piso=-0.5*(1.3672*1.3672);break;
	  case 41:   piso=-0.5*(1.3842*1.3842);break;
	  case 42:   piso=-0.5*(1.4013*1.4013);break;
	  case 43:   piso=-0.5*(1.4183*1.4183);break;
	  case 44:   piso=-0.5*(1.4354*1.4354);break;
	  case 45:   piso=-0.5*(1.4524*1.4524);break;
	  case 46:   piso=-0.5*(1.4695*1.4695);break;
	  case 47:   piso=-0.5*(1.4866*1.4866);break;
	  case 48:   piso=-0.5*(1.5037*1.5037);break;
	  case 49:   piso=-0.5*(1.5209*1.5209);break;
	  case 50:   piso=-0.5*(1.5382*1.5382);break;
	  case 51:   piso=-0.5*(1.5555*1.5555);break;
	  case 52:   piso=-0.5*(1.5729*1.5729);break;
	  case 53:   piso=-0.5*(1.5904*1.5904);break;
	  case 54:   piso=-0.5*(1.6080*1.6080);break;
	  case 55:   piso=-0.5*(1.6257*1.6257);break;
	  case 56:   piso=-0.5*(1.6436*1.6436);break;
	  case 57:   piso=-0.5*(1.6616*1.6616);break;
	  case 58:   piso=-0.5*(1.6797*1.6797);break;
	  case 59:   piso=-0.5*(1.6980*1.6980);break;
	  case 60:   piso=-0.5*(1.7164*1.7164);break;
	  case 61:   piso=-0.5*(1.7351*1.7351);break;
	  case 62:   piso=-0.5*(1.7540*1.7540);break;
	  case 63:   piso=-0.5*(1.7730*1.7730);break;
	  case 64:   piso=-0.5*(1.7924*1.7924);break;
	  case 65:   piso=-0.5*(1.8119*1.8119);break;
	  case 66:   piso=-0.5*(1.8318*1.8318);break;
	  case 67:   piso=-0.5*(1.8519*1.8519);break;
	  case 68:   piso=-0.5*(1.8724*1.8724);break;
	  case 69:   piso=-0.5*(1.8932*1.8932);break;
	  case 70:   piso=-0.5*(1.9144*1.9144);break;
	  case 71:   piso=-0.5*(1.9360*1.9360);break;
	  case 72:   piso=-0.5*(1.9580*1.9580);break;
	  case 73:   piso=-0.5*(1.9804*1.9804);break;
	  case 74:   piso=-0.5*(2.0034*2.0034);break;
	  case 75:   piso=-0.5*(2.0269*2.0269);break;
	  case 76:   piso=-0.5*(2.0510*2.0510);break;
	  case 77:   piso=-0.5*(2.0757*2.0757);break;
	  case 78:   piso=-0.5*(2.1012*2.1012);break;
	  case 79:   piso=-0.5*(2.1274*2.1274);break;
	  case 80:   piso=-0.5*(2.1544*2.1544);break;
	  case 81:   piso=-0.5*(2.1824*2.1824);break;
	  case 82:   piso=-0.5*(2.2114*2.2114);break;
	  case 83:   piso=-0.5*(2.2416*2.2416);break;
	  case 84:   piso=-0.5*(2.2730*2.2730);break;
	  case 85:   piso=-0.5*(2.3059*2.3059);break;
	  case 86:   piso=-0.5*(2.3404*2.3404);break;
	  case 87:   piso=-0.5*(2.3767*2.3767);break;
	  case 88:   piso=-0.5*(2.4153*2.4153);break;
	  case 89:   piso=-0.5*(2.4563*2.4563);break;
	  case 90:   piso=-0.5*(2.5003*2.5003);break;
	  case 91:   piso=-0.5*(2.5478*2.5478);break;
	  case 92:   piso=-0.5*(2.5997*2.5997);break;
	  case 93:   piso=-0.5*(2.6571*2.6571);break;
	  case 94:   piso=-0.5*(2.7216*2.7216);break;
	  case 95:   piso=-0.5*(2.7955*2.7955);break;
	  case 96:   piso=-0.5*(2.8829*2.8829);break;
	  case 97:   piso=-0.5*(2.9912*2.9912);break;
	  case 98:   piso=-0.5*(3.1365*3.1365);break;
	  case 99:   piso=-0.5*(3.3682*3.3682);break;
  }
  printf("bas %g iso %g exp(iso) %g\n",base,piso,exp(piso));
  piso=base*exp(piso);
  printf("Piso= %g\n",piso);
  QString fac("testpdf.face");
  printf("b%9.4g%9.4g%9.4g%9.4g%9.4g%9.4g%9.4g%9.4g%9.4g%9.4g\n",atom.c111,atom.c112,atom.c113,atom.c122,atom.c123,atom.c133,atom.c222,atom.c223,atom.c233,atom.c333);
  tensmul(atom);
  printf("a%9.4g%9.4g%9.4g%9.4g%9.4g%9.4g%9.4g%9.4g%9.4g%9.4g\n",atom.c111,atom.c112,atom.c113,atom.c122,atom.c123,atom.c133,atom.c222,atom.c223,atom.c233,atom.c333);

  if (cubeGL->moliso){
    glDeleteLists(cubeGL->moliso->mibas,6);
    if (cubeGL->moliso){
      delete cubeGL->moliso;
      cubeGL->moliso=NULL;
    }
    delete dock3;
    dock->show();
    dock2->show();
    createmoliso->setVisible(true);
    noMoliso->setVisible(false);
  }
  cubeGL->moliso = new MolIso();
  cubeGL->moliso->L=cubeGL->L;
  cubeGL->moliso->lineNr=0;
  V3 X=V3(1,0,0);
  V3 ev=V3(1,1,1);
//  Matrix evk=
	  mol.jacobi(U,ev);

  //printf("%g %g %g  \n",ev.x,ev.y,ev.z);
  double maxdim=2*sqrt(fmax(ev.x,fmax(ev.y,ev.z)))*6;

  double pt;//p10,p90,p50,
  double ponent;
  //p50=base*exp(-1.18302962);
  //p10=base*exp(-0.29215368);
  //p90=base*exp(-3.12575004);
  int breite=181;
  double df=maxdim/breite;
  cubeGL->moliso->orig=Vector3(atom.kart.x,atom.kart.y,atom.kart.z);
  cubeGL->moliso->x_dim=Vector3(df,0,0);
  cubeGL->moliso->y_dim=Vector3(0,df,0);
  cubeGL->moliso->z_dim=Vector3(0,0,df);
  cubeGL->moliso->breite=cubeGL->moliso->hoehe=cubeGL->moliso->tiefe=breite;
  cubeGL->moliso->data.clear();
  cubeGL->moliso->mdata.clear();
  int z=0;
//  double cfact=6/(M_PI*M_PI*M_PI*8);
//  tensmul(atom,roma);
  
/*  printf(
		  "u111 %14.7g u222 %14.7g u333 %14.7g \n"
		  "u112 %14.7g u122 %14.7g u113 %14.7g \n"
		  "u133 %14.7g u223 %14.7g u233 %14.7g \n"
	          "u123 %14.7g \n",
		  atom.c111,atom.c222,atom.c333,
		  atom.c112,atom.c122,atom.c113,
		  atom.c133,atom.c223,atom.c233,
		  atom.c123);
  printf(
		  "c111 %14.7g c222 %14.7g c333 %14.7g \n"
		  "c112 %14.7g c122 %14.7g c113 %14.7g \n"
		  "c133 %14.7g c223 %14.7g c233 %14.7g \n"
	          "c123 %14.7g \n",
		  atom.c111*mol.zelle.as*mol.zelle.as*mol.zelle.as,
		  atom.c222*mol.zelle.bs*mol.zelle.bs*mol.zelle.bs,
		  atom.c333*mol.zelle.cs*mol.zelle.cs*mol.zelle.cs,
		  atom.c112*mol.zelle.as*mol.zelle.as*mol.zelle.bs,
		  atom.c122*mol.zelle.as*mol.zelle.bs*mol.zelle.bs,
		  atom.c113*mol.zelle.as*mol.zelle.as*mol.zelle.cs,
		  atom.c133*mol.zelle.as*mol.zelle.cs*mol.zelle.cs,
		  atom.c223*mol.zelle.bs*mol.zelle.bs*mol.zelle.cs,
		  atom.c233*mol.zelle.bs*mol.zelle.cs*mol.zelle.cs,
		  atom.c123*mol.zelle.as*mol.zelle.bs*mol.zelle.cs);
// */
  double third=0,fourth=0,tmax=0,tmin=0;
  V3 t=V3(((breite-1)/-2.0)*df,((breite-1)/-2.0)*df,((breite-1)/-2.0)*df);
//  QString txt;
  QProgressDialog progress(QString("Calculating PDF of %1").arg(atom.atomname),QString(),0,breite*breite*breite);
  double pmin=1.0e99,pmax=-1.0e99;
  for (int k=0; k<breite; k++)
      for (int j=0; j<breite; j++)
          for (int i=0; i<breite; i++){
      X=V3(i*df,j*df,k*df)+t;
      ponent=((X*UI)*X)/-2.0;
      V3 w=(X*UI);

      p=base*exp(ponent);
      if ((c3)&&(atom.jtf>2)) third= p*hermite3(w,UI,
                      atom.c111/(mol.zelle.as*mol.zelle.as*mol.zelle.as),
                      atom.c222/(mol.zelle.bs*mol.zelle.bs*mol.zelle.bs),
                      atom.c333/(mol.zelle.cs*mol.zelle.cs*mol.zelle.cs),
                      atom.c112/(mol.zelle.as*mol.zelle.as*mol.zelle.bs),
                      atom.c122/(mol.zelle.as*mol.zelle.bs*mol.zelle.bs),
                      atom.c113/(mol.zelle.as*mol.zelle.as*mol.zelle.cs),
                      atom.c133/(mol.zelle.as*mol.zelle.cs*mol.zelle.cs),
                      atom.c223/(mol.zelle.bs*mol.zelle.bs*mol.zelle.cs),
                      atom.c233/(mol.zelle.bs*mol.zelle.cs*mol.zelle.cs),
                      atom.c123/(mol.zelle.as*mol.zelle.bs*mol.zelle.cs));
      if ((c4)&&(atom.jtf==4)) fourth= p*hermite4(w,UI,
		       atom.d1111/(mol.zelle.as * mol.zelle.as * mol.zelle.as * mol.zelle.as),
                       atom.d2222/(mol.zelle.bs * mol.zelle.bs * mol.zelle.bs * mol.zelle.bs),
                       atom.d3333/(mol.zelle.cs * mol.zelle.cs * mol.zelle.cs * mol.zelle.cs),
		       atom.d1112/(mol.zelle.as * mol.zelle.as * mol.zelle.as * mol.zelle.bs),
                       atom.d1113/(mol.zelle.as * mol.zelle.bs * mol.zelle.bs * mol.zelle.bs),
                       atom.d1122/(mol.zelle.as * mol.zelle.as * mol.zelle.as * mol.zelle.cs),
		       atom.d1123/(mol.zelle.as * mol.zelle.cs * mol.zelle.cs * mol.zelle.cs),
                       atom.d1133/(mol.zelle.bs * mol.zelle.bs * mol.zelle.bs * mol.zelle.cs),
                       atom.d1222/(mol.zelle.bs * mol.zelle.cs * mol.zelle.cs * mol.zelle.cs),
                       atom.d1223/(mol.zelle.as * mol.zelle.as * mol.zelle.bs * mol.zelle.bs),
                       atom.d1233/(mol.zelle.as * mol.zelle.as * mol.zelle.cs * mol.zelle.cs),
                       atom.d1333/(mol.zelle.bs * mol.zelle.bs * mol.zelle.cs * mol.zelle.cs),
		       atom.d2223/(mol.zelle.as * mol.zelle.as * mol.zelle.bs * mol.zelle.cs),
                       atom.d2233/(mol.zelle.as * mol.zelle.bs * mol.zelle.bs * mol.zelle.cs),
                       atom.d2333/(mol.zelle.as * mol.zelle.bs * mol.zelle.cs * mol.zelle.cs));
      tmin=fmin(tmin,third);
      tmax=fmax(tmax,third);
      pt=p*((c2?1:0) + third/6.0 + fourth/24.0);
      pmin=fmin(pmin,pt);
      pmax=fmax(pmax,pt);
      cubeGL->moliso->data.append(pt);
//      txt.append(QString("%1").arg(pt,13,'E',5));
      z++;
  //    if (!(z%6))txt.append("\n");
      if (!(z%(breite*breite*breite/100))) progress.setValue(z);
  //    if (!(z%(breite*breite*breite/100))) {printf(">");fflush(stdout);}
  }
  progress.setValue(z);
  printf("pmin %g pmax %g\n",pmin,pmax);
  cubeGL->moliso->iso_level=(c2)?piso:0.01;
  cubeGL->moliso->createSurface(fac,(c2)?proba:0.01,0.01,false,true);
  
  cubeGL->pause=true;
  if (cubeGL->moliso->mibas) glDeleteLists(cubeGL->moliso->mibas,6);
  cubeGL->moliso->mibas=glGenLists(6);
  statusBar()->showMessage(tr("loading surfaces...") );
  //qDebug()<<cubeGL->moliso->mibas<<cubeGL->foubas[0];
//  printf("L= %g   %g %g %g %s\n",cubeGL->moliso->L, cubeGL->moliso->orig.x, cubeGL->moliso->orig.y, cubeGL->moliso->orig.z,fac.toStdString().c_str());
  if (c2) {
    printf("%g %g %g\n",cubeGL->moliso->min,cubeGL->moliso->max,piso);
    cubeGL->moliso->max=(proba+0.005)*2;
    cubeGL->moliso->min=-0.01;
    printf("%g %g %g\n",cubeGL->moliso->min,cubeGL->moliso->max,piso);
    cubeGL->negpdf=!c2;
    printf("cubeGL->negpdf %s \n",(cubeGL->negpdf)?"true":"false");
  }

  cubeGL->moliso->loadMI(fac,false,!c2);
  updateStatusBar();
  statusBar()->showMessage(tr("surfaces loaded") );
  cubeGL->MIS=true;
  cubeGL->MILe=true;
  showface->setChecked ( true );
  showface->setVisible(true);
  showLeg->setChecked ( true );
  showLeg->setVisible(true);
  movLeg->setChecked ( false );
  movLeg->setVisible(true);
  mildir->setChecked ( true  );
  mildir->setVisible(true);
  setCursor(Qt::ArrowCursor);
  dock->hide();
  dock2->hide();
  cubeGL->setVisible ( true );
  createmoliso->setVisible(false);
  noMoliso->setVisible(true);  
  cubeGL->pause=false;
  cubeGL->updateGL();
  dock3 = new QDockWidget("Moliso control area",this);
  dock3->setAllowedAreas(Qt::AllDockWidgetAreas);
  QGroupBox *zebraZwinger= new QGroupBox();
  QVBoxLayout *zla= new QVBoxLayout();
  mt = new QCheckBox("transparence");
  mt->setChecked(true);
  mt->setShortcut(tr("T"));
  cubeGL->togglMolisoTransparence(true);
  zla->addWidget(mt);
  strikesSldr = new QSlider(Qt::Horizontal);
  strikesSldr->setMaximum(510);
  strikesSldr->setMinimum(127);
  strikesSldr->setValue(496);
  strikesSldr->setVisible(false);
  swidthSldr = new QSlider(Qt::Horizontal);
  swidthSldr->setMaximum(128);
  swidthSldr->setMinimum(1);
  swidthSldr->setValue(1);
  swidthSldr->setVisible(false);
  zebraBox = new QCheckBox("Show contour belts");
  zebraBox->setChecked(true);
  zebraBox->setShortcut(tr("F8"));
  zebraBox->setVisible(false);
  legendSize = new QSlider(Qt::Horizontal);
  legendSize->setValue(30);
  legendSize->setMaximum(100);
  cullNone = new QRadioButton("No culling");
  cullBack = new QRadioButton("Back face culling");
  cullFront = new QRadioButton("Front face culling");
  cullNone->setChecked(true);
  cubeGL->noneCull(true);
  connect(cullNone,SIGNAL(toggled(bool)),cubeGL,SLOT(noneCull(bool)));
  connect(cullBack,SIGNAL(toggled(bool)),cubeGL,SLOT(backCull(bool)));
  connect(cullFront,SIGNAL(toggled(bool)),cubeGL,SLOT(frontCull(bool)));
  zla->addWidget(cullNone);
  zla->addWidget(cullBack);
  zla->addWidget(cullFront);
  QToolBar *tb = new QToolBar("Moliso toolbar",zebraZwinger);
  tb->setOrientation(Qt::Vertical);
  tb->setIconSize(QSize(16,16));
  tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  tb->addAction(showface);
  tb->addAction(showLeg);
  tb->addAction(movLeg);
  tb->addAction(mildir);
  zla->addWidget(tb);
  mclmox =new QCheckBox("Monochrome legend text");
  mclmox->setChecked(false);
  connect(mclmox,SIGNAL(toggled(bool)),cubeGL,SLOT(togglMonoChrom(bool)));

  zla->addWidget(mclmox);
  zla->addWidget(new QLabel("Scale legend"));
  zla->addWidget(legendSize);
  QPushButton *savset = new QPushButton("Save current settings");
  connect(savset,SIGNAL(pressed()),cubeGL,SLOT(saveMISettings()));
  QPushButton *lodset = new QPushButton("Load settings");
  connect(lodset,SIGNAL(pressed()),cubeGL,SLOT(loadMISettings()));
  mlf = new QFontComboBox();
  mlf->setCurrentFont(cubeGL->MLegendFont);
  connect(mlf,SIGNAL(currentFontChanged(QFont)),cubeGL,SLOT(setMLFont(QFont)));
  fos = new QSpinBox();
  fos->setMinimum(4);
  fos->setMaximum(272);
  fos->setValue(cubeGL->MLegendFont.pointSize());
  connect(fos,SIGNAL(valueChanged(int)),cubeGL,SLOT(setMLFontSize(int)));
  zla->addWidget(mlf);
  zla->addWidget(fos);
  zla->addWidget(savset);
  zla->addWidget(lodset);
  zla->addStretch(999);
  connect(cubeGL,SIGNAL(mconf()),this,SLOT(syncMconf()));
  cubeGL->togglContours(false);
  cubeGL->scaleLegend(30);
  cubeGL->setContourCnt(496);
  cubeGL->setContourWidth(1);
  connect(mt,SIGNAL(toggled(bool)),cubeGL,SLOT(togglMolisoTransparence(bool)));
  connect(legendSize,SIGNAL(valueChanged(int)),cubeGL,SLOT(scaleLegend(int)));
  zebraZwinger->setLayout(zla);
  dock3->setWidget(zebraZwinger);
  dock3->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetClosable);
  addDockWidget(Qt::LeftDockWidgetArea, dock3);
  QMainWindow::tabifyDockWidget (dock2,dock3);
  QMainWindow::tabifyDockWidget (dock2,dock);

}
//----------------------------------S H E L D R I C K -----------------------------------------
//----------------------------------S H E L D R I C K -----------------------------------------
//----------------------------------S H E L D R I C K -----------------------------------------
double getNum(double v,double fv[20],double uiso){
  if (v<0.0)    {return (-v*uiso);}
  if (v<10.0)   {return (v);}
  if (v>=110.0) {return (v-110.0)*fv[10];}
  if (v>=100.0) {return (v-100.0)*fv[9];}
  if (v>=90.0)  {return (v-90.0)*fv[8];}
  if (v>=80.0)  {return (v-80.0)*fv[7];}
  if (v>=70.0)  {return (v-70.0)*fv[6];}
  if (v>=60.0)  {return (v-60.0)*fv[5];}
  if (v>=50.0)  {return (v-50.0)*fv[4];}
  if (v>=40.0)  {return (v-40.0)*fv[3];}
  if (v>=30.0)  {return (v-30.0)*fv[2];}
  if (v>=20.0)  {return (v-20.0)*fv[1];}
  if (v>=10.0)  {return (v-10.0);}  
  return 0.0;
}
int part=0;
int isacommand(char command[8]){
  const char bfl[108][5]={
    "ACTA", "AFIX", "MPLA", "ANIS", "BASF", "BIND", "BLOC", "BOND", "BUMP", "CELL", "CGLS", "CHIV", "CONF", "CONN", "DAMP", "DANG", //0-15
    "DEFS", "DELU", "DFIX", "DISP", "EADP", "EGEN", "END",  "EQIV", "ESEL", "EXTI", "EXYZ", "FEND", "FLAT", "FMAP", "FRAG", "FREE", //16-31
    "FVAR", "GRID", "HFIX", "HKLF", "HOPE", "HTAB", "INIT", "ISOR", "LAST", "LATT", "LAUE", "LIST", "L.S.", "MERG", "MOLE", "MORE", //32-47
    "MOVE", "NCSY", "OMIT", "PART", "PATT", "PHAN", "PHAS", "PLAN", "PSEE", "REM",  "RESI", "RTAB", "SADI", "SAME", "SFAC", "SHEL", //48-63
    "SIMU", "SIZE", "SPEC", "SPIN", "STIR", "SUMP", "SWAT", "SYMM", "TEMP", "TEXP", "TIME", "TITL", "TREF", "TWIN", "UNIT", "VECT", //64-79
    "WPDB", "WGHT", "ZERR", "XNPD", "REST", "CHAN", "RIGU", "FLAP", "RNUM", "SOCC", "PRIG", "WIGL", "RANG", "TANG", "ADDA", "STAG",
    "ATOM", "HETA", "SCAL", "ABIN", "ANSC", "ANSR", "NOTR", "NEUT", "TWST", "BEDE", "LONE"
  };
  int i=0;
  for (size_t j=0; j<strlen(command); j++) command[j] = toupper( command[j] );
  while ((i<108)&&(strcmp(command,bfl[i]))) i++;
  if (i==108) i=-1;
  return i+1;
}

void MyWindow::load_sheldrick(QString fileName){
  char *fn;
  mol.pmin=10000;
  mol.pmax=-10000;
  INP newAtom;
  newAtom.part=0;
  newAtom.sg=0;
  strcpy(newAtom.ami3,"");
  newAtom.resiNr=0;
  george=true;
  QByteArray BAfn=fileName.toLocal8Bit ();
  fn=BAfn.data();
  char dn[1024], *ddd=strrchr(fn,'/');
  int dlen=ddd-fn;
  strncpy(dn,fn,dlen);
  dn[dlen]='\0';
  maxResi=0;
  mol.initDir();
  FILE *adp,*aadp=NULL,*incf=NULL; 
  part=0;
  char line[85],line2[85],llin[172],dv[50],dvv[50],command[8],ext[85],Ami3[5];
  int acnt=0,sorte=0,gitter=0,bfl,sftr[85],resNr=-1;
  int sf,pda=0;
  double fvar[20],Uiso=0.05; 
  dummax=0; 
  atmax=1; 
  mol.zelle.symmops.clear();
  mol.zelle.trans.clear();
  V3 nl(0,0,0);
  sf=0;
  mol.zelle.trans.append(nl);
  mol.zelle.symmops.append(Matrix(1,0,0, 0,1,0, 0,0,1));
  if ((adp=fopen(fn,"r"))==NULL) {qDebug("Can't open %s!!!\n",fn);exit(2);}
  while (!feof(adp)) { 
    egals=fgets(line,83,adp);
    if (line[0]==' ') continue;//if there is a = continue line this should have been handeld before
    if (line[0]=='\n') continue;
    if (strchr(line,'!')){
      char *idx=strchr(line,'!'); 
          //printf("------ZN> %ld %c\n",idx-line,line[idx-line]);
      if (line[0]=='L') line[idx-line]=' ';
      else line[idx-line]='\0';
    }
    command[0]='\0';
    if (line[0]=='+'){
      char incpfad[80];
      sscanf(line,"+%s",incpfad);
      QString ipf=QString("%1/%2").arg(dn).arg(incpfad);
      if (QFileInfo(ipf).exists()){
         incf=fopen(ipf.toLocal8Bit().data(),"r");
         if (incf!=NULL) {
         aadp=adp;
         adp=incf;
         egals=fgets(line,83,adp);
//         qDebug()<<"included"<<ipf;
         }
      }

    }

    sscanf(line,"%[a-zA-Z.] %[^\n]",command,ext); 
    bfl=isacommand(command);
//    printf("%d [%s][%s]%s",bfl,command,ext,line);
    if (bfl) {
      if (bfl==10) {
        sscanf(ext,"%lf %lf%lf%lf%lf%lf%lf\n",&mol.zelle.lambda,&mol.zelle.a,&mol.zelle.b,&mol.zelle.c,&mol.zelle.al,&mol.zelle.be,&mol.zelle.ga);
	setup_zelle();
      }
      if (bfl==52) {
	part=atoi(ext);
      }
      if (bfl==63) {
	//SFAC 
	if (strchr(line,'=')) {
	  char *idx=strchr(line,'='); 
          //printf("------ZN> %ld %c\n",idx-line,line[idx-line]);
	  line[idx-line]=' ';
          egals=fgets(line2,82,adp);
	} 
	else line2[0]='\0';
	sprintf(llin,"%s%s",line,line2);
	char tch[83],*tok;
	strcpy(tch,ext);
	for (tok=strtok(tch, " \n\t\r"); tok; tok=strtok(NULL, " \n\t\r")) {      
	  if (strlen(tok)>1) tok[1]=tolower(tok[1]); 
	//  printf("[%s]%d",tok,sf);
          if (isalpha(tok[0])){
            sftr[sf]=mol.Get_OZ(tok);
            sf++;
          }
	}
	//printf("\n");
      }
      if (bfl==33){ 
	int fvi=0; 
	char ch[83],*tok; 
	strcpy(ch,ext);
	for (tok=strtok(ch, " "); tok; tok=strtok(NULL, " ")) {      
          fvar[fvi]=atof(tok);
	  fvi++;
	}
      }
      if (bfl==59){
	if (2==sscanf(ext,"%d %s",&resNr,Ami3)){
	  if (strlen(Ami3)==3) {Ami3[1]=tolower(Ami3[1]);Ami3[2]=tolower(Ami3[2]);}
	}else {resNr=-1;strcpy(Ami3,"");}
        maxResi=(resNr>maxResi)?resNr:maxResi;
      }
      if (bfl==42) {
        sscanf(ext,"%d",&gitter);
      }
      if (bfl==76) {sprintf(CID,"%s",ext);}
      if (bfl==72) {
         if (!mol.decodeSymmCard(ext)){qDebug()<<ext<<"Sorry don't understand Symmcard!";exit(-1);}
      }
    }
    else {
      newAtom.lflag=1;
      if (0<sscanf(line,"%s %d",dvv,&sorte)){
	if (strchr(line,'=')) {
	  char *idx=strchr(line,'='); 
          //printf("------ZN> %ld %c\n",idx-line,line[idx-line]);
	  line[idx-line]=' ';
          egals=fgets(line2,82,adp);
	} 
	else line2[0]='\0';
	sprintf(llin,"%s%s",line,line2);
	if (line2[0]){
	  sscanf(llin,"%s %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
		 newAtom.atomname,
		 &newAtom.atomtype,
		 &newAtom.frac.x,
		 &newAtom.frac.y,
		 &newAtom.frac.z,
		 &newAtom.amul,
		 &newAtom.uf.m11,
		 &newAtom.uf.m22,
		 &newAtom.uf.m33,
		 &newAtom.uf.m32,//32 ==23
		 &newAtom.uf.m31,//31 == 13 
		 &newAtom.uf.m21);//21 == 12
	  newAtom.amul=getNum(newAtom.amul,fvar,Uiso);
	  newAtom.uf.m23=newAtom.uf.m32;
	  newAtom.uf.m13=newAtom.uf.m31;
	  newAtom.uf.m12=newAtom.uf.m21;
	  Uiso=sqrt(newAtom.uf.m11*newAtom.uf.m11+
		    newAtom.uf.m22*newAtom.uf.m22+
		    newAtom.uf.m33*newAtom.uf.m33);
          newAtom.jtf=2;
	}else {
          newAtom.jtf=1;
	  pda=sscanf(line,"%s %d %lf %lf %lf %lf %lf %lf",
			 newAtom.atomname,
			 &newAtom.atomtype,
			 &newAtom.frac.x,
			 &newAtom.frac.y,
			 &newAtom.frac.z,
                         &newAtom.amul,
			 &newAtom.uf.m11,
                         &newAtom.peakHeight);
	  if (pda==8){
	    mol.pmin=fmin(mol.pmin,newAtom.peakHeight);
	    mol.pmax=fmax(mol.pmax,newAtom.peakHeight);
            //printf("%s %f %f\n",line,mol.pmin,mol.pmax);
	  }

	  newAtom.amul=getNum(newAtom.amul,fvar,Uiso);

	  newAtom.uf.m11=getNum(newAtom.uf.m11,fvar,Uiso);
	  newAtom.uf.m33=newAtom.uf.m22=
	  newAtom.uf.m31=newAtom.uf.m13=
	  newAtom.uf.m32=newAtom.uf.m23=
	  newAtom.uf.m12=newAtom.uf.m21=0.00;
	}
	if (resNr>-1) {
	  seReAct->setEnabled(true);
	  seReAct->setVisible(true);
	  sprintf(dv,"%s@%d%s",newAtom.atomname,resNr,Ami3);
	  newAtom.resiNr=resNr;
	  strcpy(newAtom.shortname,newAtom.atomname);
	  strncpy(newAtom.ami3,Ami3,3);
	  strcpy(newAtom.atomname,dv);	
	}
        newAtom.part=part;
        newAtom.OrdZahl=(newAtom.atomname[0]=='Q')?-1:sftr[newAtom.atomtype-1];
        if ((pda==7)&&(newAtom.atomname[0]=='L')) newAtom.OrdZahl=-2;
	if (!strcmp(dv,"Q")) dummax++;
	else atmax++;
//        printf("pda=%d[%s] %d atom{%s}\n",pda,line,newAtom.OrdZahl,newAtom.atomname);
	asymmUnit.append(newAtom);
	acnt++;
      }
    }
    if ((feof(adp))&&(aadp)) {
      fclose(adp);
      adp=aadp; 
      aadp=NULL;
      incf=NULL;
    }
  }
  for (int k=0; k<asymmUnit.size();k++){
      asymmUnit[k].frac.x=(asymmUnit[k].frac.x>10)?asymmUnit[k].frac.x-10.0:asymmUnit[k].frac.x;
      asymmUnit[k].frac.y=(asymmUnit[k].frac.y>10)?asymmUnit[k].frac.y-10.0:asymmUnit[k].frac.y;
      asymmUnit[k].frac.z=(asymmUnit[k].frac.z>10)?asymmUnit[k].frac.z-10.0:asymmUnit[k].frac.z;
    }
  char lattice='P';
  switch (gitter){
  case -1:
  case 1:
     break;
  case -2:
  case 2: lattice='I';break;
  case -3:
  case 3: lattice='R';break;
  case -4:
  case 4: lattice='F';break;
  case -5:
  case 5: lattice='A';break;
  case -6:
  case 6: lattice='B';break;
  case -7:
  case 7: lattice='C';break;
  }
  mol.applyLatticeCentro(lattice,gitter>0);
  atmax--; 
  smx=atmax+dummax; 

  for (int i=0;i<asymmUnit.size();i++) {
    if ((asymmUnit[i].uf.m22==0.0)&&(asymmUnit[i].uf.m33==0.0)){
      asymmUnit[i].u.m11=asymmUnit[i].u.m22=asymmUnit[i].u.m33=asymmUnit[i].uf.m11;
      asymmUnit[i].u.m12=asymmUnit[i].u.m13=asymmUnit[i].u.m23=asymmUnit[i].u.m21=asymmUnit[i].u.m31=asymmUnit[i].u.m32=0.0;}
    else mol.Uf2Uo(asymmUnit[i].uf,asymmUnit[i].u);
  }
  growSymm(6);
  //printf(">>%f %f\n",mol.pmin,mol.pmax);
}//shelx

void MyWindow::load_vasp(QString fileName){
//0V  Cl O                                 
//1   1.00000000000000     
//2     7.6217866652000641    0.0304636550647089    0.0002434230125215
//3     0.0238050005825655    6.6140422338289397    0.0025969675531448
//4     0.0003355324171574    0.0062253676579804   15.6401045195907553
//5   V    Cl   O 
//6    16    16    16
//7Direct
//8  0.1263639249449441  0.3762904341218621  0.0592434646581860
  hatlokale=0;
  george=true;
  QFile vasp(fileName);
  vasp.open(QIODevice::ReadOnly);
  QString all=vasp.readAll();
  QStringList lines=all.split('\n',QString::KeepEmptyParts);
//  qDebug()<<fileName; 
  if (lines.size()<9) return;
  strcpy(CID,lines.at(0).toStdString().c_str());
//  qDebug()<<CID<<lines.size();
  double scale=1.0;
  scale=lines.at(1).toDouble();
  V3 a,b,c;
  QStringList tok;
  tok=lines.at(2).split(' ',QString::SkipEmptyParts);
//  qDebug()<<tok.size()<<tok;
  if (tok.size()!=3) return; 
  a.x=tok.at(0).toDouble()*scale;
  a.y=tok.at(1).toDouble()*scale;
  a.z=tok.at(2).toDouble()*scale;
  
  tok=lines.at(3).split(' ',QString::SkipEmptyParts);
  if (tok.size()!=3) return; 
  b.x=tok.at(0).toDouble()*scale;
  b.y=tok.at(1).toDouble()*scale;
  b.z=tok.at(2).toDouble()*scale;
  
  tok=lines.at(4).split(' ',QString::SkipEmptyParts);
  if (tok.size()!=3) return; 
  c.x=tok.at(0).toDouble()*scale;
  c.y=tok.at(1).toDouble()*scale;
  c.z=tok.at(2).toDouble()*scale;
  xdinp.clear();
  asymmUnit.clear();
  mol.zelle.symmops.clear();
  mol.zelle.trans.clear();

  Matrix mg=Matrix(a,b,c);
  mol.zelle.a=sqrt(Norm(a));
  mol.zelle.b=sqrt(Norm(b));
  mol.zelle.c=sqrt(Norm(c));
  mol.zelle.al=mol.winkel(b,c);
  mol.zelle.be=mol.winkel(a,c);
  mol.zelle.ga=mol.winkel(a,b);
  mol.zelle.V=determinant(mg);
  mol.zelle.f2c=mg;
  qDebug()<<mol.zelle.a<<mol.zelle.b<<mol.zelle.c<<mol.zelle.al<<mol.zelle.be<<mol.zelle.ga<<mol.zelle.V;
//  qDebug()<<determinant(mg);
  mol.initDir();
 // mol.adp=0;  
  QStringList elements;
  QList<int> unit,su; 
  elements=lines.at(5).split(' ',QString::SkipEmptyParts);
  tok=lines.at(6).split(' ',QString::SkipEmptyParts);
  int na=0;
  for (int i=0; i<tok.size(); i++){
    su.append(na);
    unit.append(na+=tok.at(i).toInt());
  }
  su.append(na);
 // qDebug()<<na;
  int csfac=-1;
  INP newAtom;
  newAtom.part=0;
  newAtom.sg=0;
  newAtom.amul=1.0;
  newAtom.jtf=1;
  newAtom.OrdZahl=-4;
  newAtom.uf=Matrix(0.01,0,0, 0,0.01,0, 0,0,0.01);
  newAtom.u=Matrix(0.01,0,0, 0,0.01,0, 0,0,0.01);
  for (int i=8; i<na+8;i++){
  tok=lines.at(i).split(' ',QString::SkipEmptyParts);
  int j=i-8;

  for (int k=0; k<unit.size(); k++){
    if ((j>=su.at(k))&&(j<su.at(k+1))) csfac=k;
  }
//  V3 frac=V3(tok.at(0).toDouble(),tok.at(1).toDouble(),tok.at(2).toDouble());
//  V3 cart1=frac.x*a+frac.y*b+frac.z*c;
//  V3 cart2=mg*frac;
//  qDebug()<<csfac<<j<<elements.at(csfac)<< cart1.x<< cart1.y<<cart1.z << cart2.x<< cart2.y<<cart2.z ;
  newAtom.frac=V3(tok.at(0).toDouble(),tok.at(1).toDouble(),tok.at(2).toDouble());
  newAtom.kart=mg*newAtom.frac;
  newAtom.OrdZahl=mol.Get_OZ(elements.at(csfac)); 
  sprintf(newAtom.atomname,"%s",QString("%1%2").arg(elements.at(csfac)).arg(j-su.at(csfac)+1).toStdString().c_str()); 
  asymmUnit.append(newAtom);


  }
  mol.decodeSymmCard("x, y, z");
  growSymm(0);



   


}

//--------------------------------------------------------------------------------------------
// C  I  F  CIF
void MyWindow::cifcard(const QString v){
static int cif_celler=0;
const int cifa=1;
const int cifb=2;
const int cifc=4;
const int cifal=8;
const int cifbe=16;
const int cifga=32;
bool ok;
double val=v.section(QRegExp("\\W+"),1,2).toDouble(&ok);
QString card=v.section(QRegExp("\\W+"),0,0);
if (ok){
if (card=="_cell_length_a") {mol.zelle.a=val; cif_celler|=cifa; }
else if (card=="_cell_length_b") {mol.zelle.b=val; cif_celler|=cifb; }
else if (card=="_cell_length_c") {mol.zelle.c=val; cif_celler|=cifc; }
else if (card=="_cell_angle_alpha") {mol.zelle.al=val; cif_celler|=cifal; }
else if (card=="_cell_angle_beta") {mol.zelle.be=val; cif_celler|=cifbe; }
else if (card=="_cell_angle_gamma") {mol.zelle.ga=val; cif_celler|=cifga;}
if (cif_celler==63) {setup_zelle();cif_celler=0;}
}
}

QList<int> anisIndex;
void cifloop(const QString tag,const QString value,int loopat){
  static int ola=-1;
  if (tag=="_symmetry_equiv_pos_as_xyz") {mol.decodeSymmCard(value);return;}
  if (tag=="_space_group_symop_operation_xyz") {mol.decodeSymmCard(value);return;}
  INP newAtom;
  newAtom.part=0;
  newAtom.sg=0;
  newAtom.OrdZahl=-4;
  newAtom.uf=Matrix(0,0,0, 0,0,0, 0,0,0);
  QString v=value;
  if (tag=="_atom_site_fract_x") {if (ola!=loopat) asymmUnit.append(newAtom); v.remove(QRegExp("\\(\\d+\\)")); asymmUnit[loopat].frac.x=v.toDouble(); ola=loopat; return;}
  if (tag=="_atom_site_fract_y") {if (ola!=loopat) asymmUnit.append(newAtom); v.remove(QRegExp("\\(\\d+\\)")); asymmUnit[loopat].frac.y=v.toDouble(); ola=loopat; return;}
  if (tag=="_atom_site_fract_z") {if (ola!=loopat) asymmUnit.append(newAtom); v.remove(QRegExp("\\(\\d+\\)")); asymmUnit[loopat].frac.z=v.toDouble(); ola=loopat; return;}
  if (tag=="_atom_site_label") {if (ola!=loopat) asymmUnit.append(newAtom); sprintf(asymmUnit[loopat].atomname,"%s",v.toStdString().c_str()); ola=loopat; return;}
  if (tag=="_atom_site_disorder_group") {if (v=="?") return; v.remove(QRegExp("\\(\\d+\\)")); v.replace(".","0");asymmUnit[loopat].part=v.toInt();return;}
  if (tag=="_atom_site_adp_type") {if (v=="Uani") anisIndex.append(loopat);return;}
  if (tag=="_atom_site_type_symbol") {
      if (ola!=loopat) asymmUnit.append(newAtom);
      asymmUnit[loopat].OrdZahl=mol.Get_OZ(v.toStdString().c_str());
      ola=loopat;
      return;}
  if (tag=="_atom_site_aniso_label") {
    if (v=="?") return;
      if (asymmUnit[anisIndex.at(loopat)].atomname!=v) qDebug()<<"Serious Problem!"<<asymmUnit[loopat].atomname<<v;
      ola=loopat; return;}
  if (tag=="_atom_site_aniso_U_11") {if (v=="?") return;mol.adp=1;v.remove(QRegExp("\\(\\d+\\)"));asymmUnit[anisIndex.at(loopat)].uf.m11=v.toDouble();return;}
  if (tag=="_atom_site_aniso_U_22") {if (v=="?") return;mol.adp=1;v.remove(QRegExp("\\(\\d+\\)"));asymmUnit[anisIndex.at(loopat)].uf.m22=v.toDouble();return;}
  if (tag=="_atom_site_aniso_U_33") {if (v=="?") return;mol.adp=1;v.remove(QRegExp("\\(\\d+\\)"));asymmUnit[anisIndex.at(loopat)].uf.m33=v.toDouble();return;}
  if (tag=="_atom_site_aniso_U_12") {if (v=="?") return;mol.adp=1;v.remove(QRegExp("\\(\\d+\\)"));asymmUnit[anisIndex.at(loopat)].uf.m21=asymmUnit[anisIndex.at(loopat)].uf.m12=v.toDouble();return;}
  if (tag=="_atom_site_aniso_U_13") {if (v=="?") return;mol.adp=1;v.remove(QRegExp("\\(\\d+\\)"));asymmUnit[anisIndex.at(loopat)].uf.m31=asymmUnit[anisIndex.at(loopat)].uf.m13=v.toDouble();return;}
  if (tag=="_atom_site_aniso_U_23") {if (v=="?") return;mol.adp=1;v.remove(QRegExp("\\(\\d+\\)"));asymmUnit[anisIndex.at(loopat)].uf.m32=asymmUnit[anisIndex.at(loopat)].uf.m23=v.toDouble();return;}
  if (tag=="_atom_site_U_iso_or_equiv") {if (ola!=loopat) asymmUnit.append(newAtom); v.remove(QRegExp("\\(\\d+\\)")); asymmUnit[loopat].uf.m11=v.toDouble(); ola=loopat; return;}
  if (tag=="_atom_local_axes_atom_label"){hatlokale=1;if (asymmUnit[loopat].atomname!=v) qDebug()<<"Serious Problem!"; ola=loopat; return;}
  if (tag=="_atom_local_axes_atom0"){for (int w=0; w<asymmUnit.size();w++) if (v==asymmUnit.at(w).atomname) asymmUnit[loopat].nax=w+1; return;}
  if (tag=="_atom_local_axes_ax1"){if (v=="X") asymmUnit[loopat].icor1=1;if (v=="Y") asymmUnit[loopat].icor1=2;  if (v=="Z") asymmUnit[loopat].icor1=3; return;}
  if (tag=="_atom_local_axes_atom1"){for (int w=0; w<asymmUnit.size();w++) if (v==asymmUnit.at(w).atomname) asymmUnit[loopat].nay1=w+1; return;}
  if (tag=="_atom_local_axes_atom2"){for (int w=0; w<asymmUnit.size();w++) if (v==asymmUnit.at(w).atomname) asymmUnit[loopat].nay2=w+1; return;}
  if (tag=="_atom_local_axes_ax2"){if (v=="X") asymmUnit[loopat].icor2=1;if (v=="Y") asymmUnit[loopat].icor2=2;  if (v=="Z") asymmUnit[loopat].icor2=3; 
    return;}


}

void delBlank(char *in){
  char s[82];
  int j=0,b=0;
  for (unsigned int i=0;i<=strlen(in);i++){
    if ((b)||in[i]!=' ') {s[j++]=in[i];b=1;}
  }
  in=strcpy(in,s);
}

void MyWindow::load_cif(QString fileName) {
  atmax=0;
  smx=0;
  xdinp.clear();
  anisIndex.clear();
  asymmUnit.clear();
  mol.zelle.symmops.clear();
  mol.zelle.trans.clear();
  QFile ciff(fileName);
  ciff.open(QIODevice::ReadOnly);
  QString all=ciff.readAll();
  all.remove('\r');
  all.remove("\\\\'");
  all.remove(QRegExp("#[^\n]*"));
  QStringList quotes;
  quotes=all.split(QRegExp("\n;"));
  for (int i=1;i < quotes.size(); i+=2){
    quotes[i].prepend("\n;");
    quotes[i].append("\n;");
    all.replace(quotes[i]," ?\n");
  }
  quotes.clear();
  quotes = all.split("'");
  for (int i=1;i < quotes.size(); i+=2){
    quotes[i].prepend("'");
    quotes[i].append("'");
    QString after=quotes[i];
    after.replace(QRegExp("\\s+"),"");
    all.replace(quotes[i],after);
  }
  QStringList lines=all.split('\n',QString::SkipEmptyParts);
  QStringList loopings;
  for (int i =0; i<lines.size();i++){
    if (lines.at(i).trimmed()=="loop_") {
      if (i<lines.size()) i++;
      loopings.clear();
      while (lines.at(i).trimmed().startsWith("_")) {
	loopings.append(lines.at(i).trimmed());
	if (i<lines.size()) i++;
      }
    }
    int loopat=0;
    while ((!loopings.isEmpty())&&(i<lines.size())){
      if ((lines.at(i).trimmed().startsWith("_"))||(lines.at(i).trimmed().isEmpty())||(lines.at(i).trimmed()=="loop_")) loopings.clear();
      else {
	QStringList items;
	QString str;
	while ((items.size()<loopings.size())&&(i<lines.size())) {
	  str+=" "+lines.at(i);
	  items = str.split(QRegExp("\\s"),QString::SkipEmptyParts);
	  if (items.size()<loopings.size()) {if (i<lines.size()) i++;}
	}
	for (int j=0; j<items.size(); j++){
	  cifloop(loopings.at(j),items.at(j),loopat);
	}
	if (i<lines.size()) i++;
	else loopings.clear();
	loopat++;
      }
      if ((i<lines.size())&&(lines.at(i).trimmed()=="loop_")) {i--;break;}
    }
    if (i<lines.size()) cifcard(lines.at(i).trimmed());
  }
//  printf("atoms = %d \n",asymmUnit.size());
  for (int i=0;i<asymmUnit.size();i++) {
    asymmUnit[i].lflag=1;
    if (asymmUnit[i].OrdZahl==-4) asymmUnit[i].OrdZahl=mol.Get_OZ(QString(asymmUnit[i].atomname));
    if ((asymmUnit[i].uf.m22==0.0)&&(asymmUnit[i].uf.m33==0.0)){
      asymmUnit[i].u.m11=asymmUnit[i].u.m22=asymmUnit[i].u.m33=asymmUnit[i].uf.m11;
      asymmUnit[i].u.m12=asymmUnit[i].u.m13=asymmUnit[i].u.m23=asymmUnit[i].u.m21=asymmUnit[i].u.m31=asymmUnit[i].u.m32=0.0;}
    else mol.Uf2Uo(asymmUnit[i].uf,asymmUnit[i].u);
  }
 if (hatlokale) {
   cubeGL->drawAx=true;
   george=false;
 }
  growSymm(6);
/*  if (hatlokale){
    cubeGL->drawAx=true;
    for (int i=0;i<asymmUnit.size();i++) {
      if (asymmUnit[i].OrdZahl>-1){
	xdinp[i].ax1=xdinp[asymmUnit[i].nax-1].kart-xdinp[i].kart;
	xdinp[i].ax1=Normalize(xdinp[i].ax1);
	xdinp[i].ax2=xdinp[asymmUnit[i].nay2-1].kart-xdinp[asymmUnit[i].nay1-1].kart;
	xdinp[i].ax2=Normalize(xdinp[i].ax2);
	xdinp[i].ax3= xdinp[i].ax1 %xdinp[i].ax2;
	xdinp[i].ax3=Normalize(xdinp[i].ax3);
	xdinp[i].ax2= xdinp[i].ax3 %xdinp[i].ax1;
	xdinp[i].ax2=Normalize(xdinp[i].ax2);
      }
    }
  }*/
}
double MyWindow::kuhs(int n,INP atom){
double k=(sqrt(2.0*n*twologtwo))/(sqrt(2.0*M_PI*ueq(atom.uf)));
k/=2*M_PI;
return k;
}

double MyWindow::ueq(const Matrix m){//1111
  double erg=0;
  erg+=m.m11*mol.zelle.as*mol.zelle.a*mol.zelle.a*mol.zelle.as;
  erg+=m.m12*mol.zelle.as*mol.zelle.a*mol.zelle.b*mol.zelle.bs;
  erg+=m.m13*mol.zelle.as*mol.zelle.a*mol.zelle.c*mol.zelle.cs;
  erg+=m.m21*mol.zelle.bs*mol.zelle.b*mol.zelle.a*mol.zelle.as;
  erg+=m.m22*mol.zelle.bs*mol.zelle.b*mol.zelle.b*mol.zelle.bs;
  erg+=m.m23*mol.zelle.bs*mol.zelle.b*mol.zelle.c*mol.zelle.cs;
  erg+=m.m31*mol.zelle.cs*mol.zelle.c*mol.zelle.a*mol.zelle.as;
  erg+=m.m32*mol.zelle.cs*mol.zelle.c*mol.zelle.b*mol.zelle.bs;
  erg+=m.m33*mol.zelle.cs*mol.zelle.c*mol.zelle.c*mol.zelle.cs;
  erg*=1/3.0;
  return erg;
}

void MyWindow::pdbOutput(){
  QMap<double,INP> xdpdb;
  QString selectedFilter="Protein-Data-Base file (*.pdb)";
  QString fileName = QFileDialog::getSaveFileName(this,
  QString("try to export a PDB file"), saveName,"Protein-Data-Base file (*.pdb)",&selectedFilter,QFileDialog::DontUseNativeDialog );
  if (fileName.isEmpty()) return;
QFile pdb(fileName);
pdb.open(QIODevice::WriteOnly|QIODevice::Text);
pdb.write(QString("CRYST1%1%2%3%4%5%6           \n")
		.arg(mol.zelle.a,9,'f',3) 
		.arg(mol.zelle.b,9,'f',3) 
		.arg(mol.zelle.c,9,'f',3) 
		.arg(mol.zelle.al,7,'f',2) 
		.arg(mol.zelle.be,7,'f',2) 
		.arg(mol.zelle.ga,7,'f',2) 
		.toLatin1()); 

  double phi; 
  const double g2r=180.0/M_PI;
  Matrix u;   /*Cholesky decomposition of theReal space Metric tensor Wird fÃÂÃÂ¼r die Umrechnung von fraktionellen in kartesischen Korrdinaten benÃÂÃÂ¶tigt.*/ 
  phi=sqrt(1-pow(cos(mol.zelle.al/g2r),2.0)-pow(cos(mol.zelle.be/g2r),2.0)-pow(cos(mol.zelle.ga/g2r),2.0)+2.0*cos(mol.zelle.al/g2r)*cos(mol.zelle.be/g2r)*cos(mol.zelle.ga/g2r));
  u.m11 = 1.0/mol.zelle.a;
  u.m21 = 0.0;
  u.m31 = 0.0;
  u.m12 = -1.0/(mol.zelle.a * tan(mol.zelle.ga/g2r));
  u.m22 = 1.0/(mol.zelle.b * sin(mol.zelle.ga/g2r));
  u.m32 = 0.0;
  u.m13 = (cos(mol.zelle.al/g2r)*cos(mol.zelle.ga/g2r)-cos(mol.zelle.be/g2r))/(mol.zelle.a*phi*sin(mol.zelle.ga/g2r));
  u.m23 = (cos(mol.zelle.be/g2r)*cos(mol.zelle.ga/g2r)-cos(mol.zelle.al/g2r))/(mol.zelle.b*phi*sin(mol.zelle.ga/g2r));
  u.m33 = sin(mol.zelle.ga/g2r)/(mol.zelle.c*phi);
  pdb.write(QString("SCALE1    %1%2%3         0.00000\n")
		  .arg(u.m11,10,'f',6)
		  .arg(u.m12,10,'f',6)
		  .arg(u.m13,10,'f',6)
		  .toLatin1());
  pdb.write(QString("SCALE2    %1%2%3         0.00000\n")
		  .arg(u.m21,10,'f',6)
		  .arg(u.m22,10,'f',6)
		  .arg(u.m23,10,'f',6)
		  .toLatin1());
  pdb.write(QString("SCALE3    %1%2%3         0.00000\n")
		  .arg(u.m31,10,'f',6)
		  .arg(u.m32,10,'f',6)
		  .arg(u.m33,10,'f',6)
		  .toLatin1());
  QMap<int,QString> parts;
  int x=10;
  for (int i=0; i<asymmUnit.size();i++){
      xdpdb.insertMulti(asymmUnit.at(i).resiNr*asymmUnit.size()*10+i/10.0,asymmUnit.at(i));
      if (asymmUnit.at(i).part){
          if (!parts.contains(asymmUnit.at(i).part))
          parts.insertMulti(asymmUnit.at(i).part,QString::number(x++,36).toUpper());
      if (x>35) qDebug()<<"to many different parts!";
      }
  }
  QList<INP> AsymmUnit=xdpdb.values();
      for (int i=0; i<AsymmUnit.size();i++){
    if (AsymmUnit[i].OrdZahl<0) continue;
    mol.frac2kart(AsymmUnit[i].frac,AsymmUnit[i].kart);
    QString enam=AsymmUnit.at(i).shortname;
    enam.truncate(4);
    pdb.write(QString("ATOM  %1 %2%3%4 %5%6%7   %8%9%10%11%12           %13\n")
		    .arg(i+1,5)
            .arg(enam,-4)
            .arg((AsymmUnit.at(i).part)?parts.value(AsymmUnit.at(i).part):QChar(' '),1)
           .arg(AsymmUnit.at(i).ami3,3)
		   .arg(" ") //chain id
           .arg(AsymmUnit.at(i).resiNr,4)
		   .arg(" ")
           .arg(AsymmUnit.at(i).kart.x,8,'f',3)
           .arg(AsymmUnit.at(i).kart.y,8,'f',3)
           .arg(AsymmUnit.at(i).kart.z,8,'f',3)
           .arg(AsymmUnit.at(i).amul,6,'f',3)
           .arg(ueq(AsymmUnit.at(i).u)*8*M_PI*M_PI,6,'f',2)
           .arg(mol.pse(AsymmUnit.at(i).OrdZahl))
		  .toLatin1());
    if (AsymmUnit.at(i).jtf>1)
    pdb.write(QString("ANISOU%1 %2%3%4 %5%6%7 %8%9%10%11%12%13       %14\n")
		    .arg(i+1,5)//1
            .arg(enam,-4)//2
            .arg((AsymmUnit.at(i).part)?parts.value(AsymmUnit.at(i).part):QChar(' '),1)
           .arg(AsymmUnit.at(i).ami3,3)//4
		   .arg(" ") //5 chain id
           .arg(AsymmUnit.at(i).resiNr,4)//6
		   .arg(" ")//??
           .arg(AsymmUnit.at(i).u.m11*10000,7,'f',0)//8
           .arg(AsymmUnit.at(i).u.m22*10000,7,'f',0)//9
           .arg(AsymmUnit.at(i).u.m33*10000,7,'f',0)//10
           .arg(AsymmUnit.at(i).u.m12*10000,7,'f',0)//11
           .arg(AsymmUnit.at(i).u.m13*10000,7,'f',0)//12
           .arg(AsymmUnit.at(i).u.m23*10000,7,'f',0)//13
           .arg(mol.pse(AsymmUnit.at(i).OrdZahl))//14
          .toLatin1());
  } 
  pdb.close();
}

int pdbatom(char* line,int *idx,char *nam, char *ami3, char * chid, int *rn, double *x, double *y, double *z, double *b,char *dum){
  char dummy[80];
  int i,j=0;
  for (i=6; i<11;i++) dummy[j++]=line[i];
  dummy[j]='\0';
  *idx=atoi(dummy);
  j=0;

  for (i=12; i<16;i++) dummy[j++]=line[i];
  dummy[j]='\0';
  strcpy(nam,dummy);
  j=0;

  for (i=17; i<20;i++) dummy[j++]=line[i];
  dummy[j]='\0';
  strcpy(ami3,dummy);
  j=0;

  *chid=line[21];

  for (i=22; i<26;i++) dummy[j++]=line[i];
  dummy[j]='\0';
  *rn=atoi(dummy);
  j=0;

  for (i=30; i<38;i++) dummy[j++]=line[i];
  dummy[j]='\0';
  *x=atof(dummy);
  j=0;

  for (i=38; i<46;i++) dummy[j++]=line[i];
  dummy[j]='\0';
  *y=atof(dummy);
  j=0;

  for (i=46; i<54;i++) dummy[j++]=line[i];
  dummy[j]='\0';
  *z=atof(dummy);
  j=0;

  for (i=60; i<66;i++) dummy[j++]=line[i];
  dummy[j]='\0';
  *b=atof(dummy);
  j=0;

  for (i=76; i<78;i++) dummy[j++]=line[i];
  dummy[j]='\0';
  strcpy(dum,dummy);
  j=0;
  return 9;
}

/*
   COLUMNS DATA TYPE FIELD DEFINITION
   -------------------------------------------------------------
   1 - 6 Record name "ATOM "
   7 - 11 Integer serial Atom serial number.
   13 - 16 Atom name Atom name.
   17 Character altLoc Alternate location indicator.
   18 - 20 Residue name resName Residue name.
   22 Character chainID Chain identifier.
   23 - 26 Integer resSeq Residue sequence number.
   27 AChar iCode Code for insertion of residues.
   31 - 38 Real(8.3) x Orthogonal coordinates for X in Angstroms
   39 - 46 Real(8.3) y Orthogonal coordinates for Y in Angstroms
   47 - 54 Real(8.3) z Orthogonal coordinates for Z in Angstroms
   55 - 60 Real(6.2) occupancy Occupancy.
   61 - 66 Real(6.2) tempFactor Temperature factor.
   77 - 78 LString(2) element Element symbol, right-justified.
   79 - 80 LString(2) charge Charge on the atom.
   */

void MyWindow::load_pdb(QString fileName){

  FILE *adp; 
  char line[1024],Ami3[5],chId,dummy[80],dummy2[80];
  int i=0,resNr;
  QByteArray BAfn=fileName.toLocal8Bit ();
  char * fn=BAfn.data();
  char dn[1024], *ddd=strrchr(fn,'/');
  int dlen=ddd-fn;
  strncpy(dn,fn,dlen);
  dn[dlen]='\0';
  mol.initDir();
  atmax=0;
  smx=0;
  INP newAtom;
  newAtom.part=0;
  if ((adp=fopen(fn,"r"))==NULL) {fprintf(stderr,"Can't open %s!!!\n",fn);exit(2);}
  while (!feof(adp)) {
    egals=fgets(line,80,adp);
    if (strstr(line,"CRYST1")){
      sscanf(line,"CRYST1 %lf %lf %lf %lf %lf %lf",&mol.zelle.a,&mol.zelle.b,&mol.zelle.c,&mol.zelle.al,&mol.zelle.be,&mol.zelle.ga);
//      printf ("CELL %f %f %f %f %f %f\n",mol.zelle.a,mol.zelle.b,mol.zelle.c,mol.zelle.al,mol.zelle.be,mol.zelle.ga);
      setup_zelle();
    }

    if (strstr(line,"TITLE")){
      strcpy(CID,line+10);
      while (CID[strlen(CID)-1]==' ') {CID[strlen(CID)-1]='\0';}
    }
    if ((line[0]=='A')&&(strstr(line,"ATOM "))){ 
      pdbatom(line,
		      &i,
		      dummy,Ami3,&chId,&resNr, 
		      &newAtom.kart.x,
		      &newAtom.kart.y,
		      &newAtom.kart.z,
		      &newAtom.u.m11,dummy2);

      if (dummy2[0]==' ') {dummy2[0]=dummy2[1];dummy2[1]='\0';}
      if (line[16]!=' ') newAtom.part=(line[16]-'A')+1;
      else newAtom.part=0;
      dummy[4]='\0';
      Ami3[4]='\0';
      sprintf(newAtom.atomname,"%s@%d%s",dummy,resNr,Ami3); 
      maxResi=(maxResi>resNr)?maxResi:resNr;
      mol.delBlanks(newAtom.atomname);
      strncpy(dummy,dummy2,2);
      newAtom.OrdZahl=mol.Get_OZ(dummy);
      if (newAtom.OrdZahl<0) { strncpy(dummy,newAtom.atomname,1);newAtom.OrdZahl=mol.Get_OZ(dummy);}
      mol.kart2frac(newAtom.kart,newAtom.frac);
      asymmUnit.append(newAtom);
   //   printf("%d %-12s %f %f %f\n",atmax,newAtom.atomname,newAtom.kart.x,newAtom.kart.y,newAtom.kart.z);
      atmax++;
    }
    if ((line[0]=='H')&&(strstr(line,"HETATM "))) { 
      int ttt=pdbatom(line,
		      &i, 
		      dummy,
		      Ami3,&chId,&resNr,  
		      &newAtom.kart.x,
		      &newAtom.kart.y,
		      &newAtom.kart.z,
		      &newAtom.u.m11,dummy2);
      if (dummy2[0]==' ') {dummy2[0]=dummy2[1];dummy2[1]='\0';}
      /*
	 COLUMNS DATA TYPE FIELD DEFINITION
	 -----------------------------------------------------------------------
	 1 - 6 Record name "HETATM"
	 7 - 11 Integer serial Atom serial number.
	 13 - 16 Atom name Atom name.
	 17 Character altLoc Alternate location indicator.
	 18 - 20 Residue name resName Residue name.
	 22 Character chainID Chain identifier.
	 23 - 26 Integer resSeq Residue sequence number.
	 27 AChar iCode Code for insertion of residues.
	 31 - 38 Real(8.3) x Orthogonal coordinates for X.
	 39 - 46 Real(8.3) y Orthogonal coordinates for Y.
	 47 - 54 Real(8.3) z Orthogonal coordinates for Z.
	 55 - 60 Real(6.2) occupancy Occupancy.
	 61 - 66 Real(6.2) tempFactor Temperature factor.
	 77 - 78 LString(2) element Element symbol; right-justified.
	 79 - 80 LString(2) charge Charge on the atom.
	 */
      //HETATM  972 3HMD PNI    26       1.453   8.392  80.001  1.00 14.87           H

      if (ttt<9) {
	QMessageBox::critical(this,"Read Error!",QString(tr("read error near atom %1, only %2 tokens fit!").arg(i).arg(ttt)),QMessageBox::Ok);exit(23);}
      dummy[4]='\0'; 
      Ami3[3]='\0';  
      sprintf(newAtom.atomname,"%s@%d%s",dummy,resNr,Ami3);
      mol.delBlanks(newAtom.atomname); 
      dummy[0]='\0'; 
      dummy[1]='\0'; 
      dummy[2]='\0'; 
      strncpy(dummy,dummy2,2); 
      newAtom.OrdZahl=mol.Get_OZ(dummy);
      mol.kart2frac(newAtom.kart,newAtom.frac);
      asymmUnit.append(newAtom);
//      printf("%d %-12s %f %f %f HETATM\n",atmax,newAtom.atomname,newAtom.kart.x,newAtom.kart.y,newAtom.kart.z);
      atmax++; 
    }
  } 
  smx=atmax;
  /*
  for (int i=0; i<smx; i++){
    if (asymmUnit[i].OrdZahl!=-1){
      xs+=asymmUnit[i].kart.x;
      ys+=asymmUnit[i].kart.y;
      zs+=asymmUnit[i].kart.z;
    }
  }
  xs/=(smx);ys/=(smx);zs/=(smx);
  for (int i=0; i<smx; i++){
    asymmUnit[i].kart.x-=xs;
    asymmUnit[i].kart.y-=ys;
    asymmUnit[i].kart.z-=zs;    
  }
*/
  for (int i=0;i<asymmUnit.size();i++) {
    asymmUnit[i].u.m22=asymmUnit[i].u.m33=asymmUnit[i].u.m11=asymmUnit[i].u.m11/(8*M_PI*M_PI);
    asymmUnit[i].u.m12=asymmUnit[i].u.m13=asymmUnit[i].u.m23=asymmUnit[i].u.m21=asymmUnit[i].u.m31=asymmUnit[i].u.m32=0.00001;
  }

   cubeGL->drawAx=false;
   george=true ;
   seReAct->setEnabled(true);
   seReAct->setVisible(true);
  growSymm(0);
}

void MyWindow::load_xyz(QString fileName){
  cubeGL->setVisible ( false);

  mol.adp=0;
  INP newAtom;
  newAtom.part=0;
  newAtom.resiNr=0;
  mol.initDir();
  mol.adp=0;  
  QFile xyz(fileName);
  xyz.open(QIODevice::ReadOnly|QIODevice::Text);
  QString all= QString( xyz.readAll());
  all.remove(QRegExp("[\\[\\],]+"));
  QStringList lines = all.split('\n');
  xyz.close();
  for (int i = 0 ;  i < lines.size(); i++){
    QStringList tok = lines.at(i).split(QRegExp("\\s+"));
    if (tok.size() == 4){
//  qDebug()<<tok<<asymmUnit.size()<<newAtom.OrdZahl;
      strcpy(newAtom.atomname,tok.at(0).toStdString().c_str());
     newAtom.kart.x=tok.at(1).toDouble();
     newAtom.kart.y=tok.at(2).toDouble();
     newAtom.kart.z =tok.at(3).toDouble();
     newAtom.OrdZahl=mol.Get_OZ(tok.at(0));
     asymmUnit.append(newAtom);
  }
  else if (tok.size() == 10){
     strcpy(newAtom.atomname,tok.at(0).toStdString().c_str());
     newAtom.kart.x=tok.at(1).toDouble();
     newAtom.kart.y=tok.at(2).toDouble();
     newAtom.kart.z=tok.at(3).toDouble();
     //m11, m21, m31, m12, m22, m32, m13, m23, m33
    
     mol.adp=1;
     newAtom.u.m11=tok.at(4).toDouble();
     newAtom.u.m22=tok.at(5).toDouble();
     newAtom.u.m33=tok.at(6).toDouble();
     newAtom.u.m12=newAtom.u.m21=tok.at(7).toDouble();
     newAtom.u.m13=newAtom.u.m31=tok.at(8).toDouble();
     newAtom.u.m23=newAtom.u.m32=tok.at(9).toDouble();
     newAtom.OrdZahl=mol.Get_OZ(tok.at(0).section(QRegExp("[^A-Za-z]+"),0,0));
     asymmUnit.append(newAtom);

  }
  else {newAtom.part++;newAtom.resiNr++;}
}
//for (int i = 0 ;  i <asymmUnit.size();i++) qDebug()<<asymmUnit.at(i).atomname<< asymmUnit.at(i).OrdZahl<< asymmUnit.at(i).part<<  asymmUnit.at(i).resiNr;
xdinp=asymmUnit;
double dim=dimension(xdinp);
if ((Norm(atom1Pos)==0)&&(Norm(atom2Pos)==0)) cubeGL->L=100.0/dim;
  /*if (mol.nListe>2) {
    free(mol.vL);
    mol.vL=NULL;
    mol.nListe=0;
  }*/
  cubeGL->resetENV();
  initLists(xdinp);
  cubeGL->setVisible ( true );
}

void MyWindow::load_gaus(QString fileName){

  cubeGL->setVisible ( false );
  const double g2r=180.0/M_PI;
  char *fn;
  QByteArray BAfn=fileName.toLocal8Bit ();
  fn=BAfn.data();
  bool  keineAbkuerzung=false;
  char dn[1024], *ddd=strrchr(fn,'/');
  int dlen=ddd-fn;
  strncpy(dn,fn,dlen);
  dn[dlen]='\0';
  INP newAtom;
  newAtom.part=0;
  mol.initDir();
  mol.adp=0;  
  FILE *adp;
  char line[182]="start",dvv[50],*dvvv;
  double B[250],A[250],D[250],dd,emcp,sp,cp;
  V3 h1=V3(0,0,0),h2=V3(0,0,0),h3=V3(0,0,0);
  int b[250],a[250],d[250];
  int i=0,charge,spin,k=0,cartesian=0,ai=0,bi=0,di=0;
  char bname[250][50],aname[250][50],dname[250][50];
  dummax=0;
  if ((adp=fopen(fn,"r"))==NULL) {fprintf(stderr,"Can't open %s!!!\n",fn);exit(2);}
  atmax=0;smx=0;
  while ((strcmp(line,"\n"))&&(!feof(adp))){
    egals=fgets(line,181,adp);
  }
  strcpy(line,"titel");
  while ((strcmp(line,"\n"))&&(!feof(adp))){
    egals=fgets(line,181,adp);
    if (strcmp(line,"\n")) sscanf(line,"%[^\n]\n",CID);
  }
  strcpy(line,"cordi");
  egals=fgets(line,181,adp);
  sscanf(line,"%d %d",&charge,&spin);
  while ((strcmp(line,"\n"))&&(!feof(adp))){
    egals=fgets(line,181,adp);
    if ((NULL==strstr(line,"Variables:"))&&(strcmp(line,"\n"))&&(strcmp(line,"\r\n"))) {
      if ((i==0)&&(sscanf(line,"%[A-Za-z] %lf %lf %lf",
				      newAtom.atomname,&newAtom.kart.x,&newAtom.kart.y,&newAtom.kart.z)>3)) {
	cartesian=1;
	newAtom.OrdZahl=mol.Get_OZ(newAtom.atomname);
//	sprintf(newAtom.atomname,"%s(%d)",newAtom.atomname,atmax+dummax);
        char numbstr[20];
        sprintf(numbstr,"(%d)",atmax+dummax);
        strncat(newAtom.atomname,numbstr,6);
	if (newAtom.OrdZahl>-1) atmax++; else dummax++;
	asymmUnit.append(newAtom);
      }
      else {
	k=sscanf(line,"%s %d %s %d %s %d %s",newAtom.atomname,&b[bi],bname[bi],&a[ai],aname[ai],&d[di],dname[di]);
	switch (k) {
		case 7: di++;
//                        __attribute__ ((fallthrough));
		case 5: ai++;
//                        __attribute__ ((fallthrough));
		case 3: bi++;
 //                       __attribute__ ((fallthrough));
		case 1: i++;
//			asymmUnit.append(newAtom);
		default: ;
	}
	asymmUnit[i-1].OrdZahl=mol.Get_OZ(asymmUnit[i-1].atomname);
	if (asymmUnit[i-1].OrdZahl>-1) atmax++; else dummax++;
	sprintf(asymmUnit[i-1].atomname,"%s(%d)",asymmUnit[i-1].atomname,i);
	if (i>249) {fprintf(stderr,"Com-file reading accepts not more than 249 atoms!!\n");exit(0);}
      }
    }else {
      strcpy(line,"\n");
    }
  }
  for (int jo=0;jo<di;jo++)
    if (QString(bname[jo]).contains(QRegExp("[A-Za-z]"))&&
		    QString(aname[jo]).contains(QRegExp("[A-Za-z]"))&&
		    QString(dname[jo]).contains(QRegExp("[A-Za-z]"))) {
      keineAbkuerzung=true;
    }
  if (!cartesian) {
    strcpy(line,"cordi");
    if (!keineAbkuerzung) {
      for (int j=0;j<(atmax+dummax);j++){
	B[j]=atof(bname[j]);
	A[j-1]=atof(aname[j-1]);
	D[j-2]=atof(dname[j-2]);
      }
    }else
      while ((strcmp(line,"\n")&&(strcmp(line,"\r\n")))&&(!feof(adp))){
	egals=fgets(line,181,adp);
	if (strcmp(line,"\n")){
	  dvvv=strtok(line," =:");
	  strcpy(dvv,dvvv);
	  dvvv=strtok(NULL," =:");
	  dd=atof(dvvv);
	  delBlank(dvv);
	  int bn=0,an=0,dn=0;
	  while ((bn<bi)&& (strcmp(dvv,bname[bn]))) {bn++;}
	  if (bn<bi) B[bn]=dd;
	  while ((an<ai)&& (strcmp(dvv,aname[an]))) an++;
	  if (an<ai) A[an]=dd;
	  while ((dn<di)&& (strcmp(dvv,dname[dn]))) dn++;
	  if (dn<di) D[dn]=dd;
	}
      }
    strcpy(line,"cordi");
    while ((strcmp(line,"\n"))&&(!feof(adp))){
      egals=fgets(line,181,adp);
    }
    smx=atmax+dummax;
    asymmUnit[0].kart.x=0.0; 
    asymmUnit[0].kart.y=0.0;
    asymmUnit[0].kart.z=0.0;
    asymmUnit[1].kart.x=B[0];
    asymmUnit[1].kart.y=0.0;
    asymmUnit[1].kart.z=0.0;
    for (int j=2;j<smx;j++){
      h2=asymmUnit[j].kart=(asymmUnit[a[j-2]-1].kart-asymmUnit[b[j-1]-1].kart);
      asymmUnit[j].kart=Normalize(asymmUnit[j].kart);
      asymmUnit[j].kart*=B[j-1];
      if (j==2) {h1.x=0.0;h1.y=1.0;h1.z=0.0;}
      else
      {
	h1=(asymmUnit[d[j-3]-1].kart-asymmUnit[b[j-1]-1].kart);
	h1=Normalize(h1);
      }
      h2=Normalize(h2);
      h3=h1%h2;
      h3*=-1.0;
      h3=Normalize(h3);
      emcp=(1-cos(A[j-2]/g2r));
      sp=sin(A[j-2]/g2r);
      cp=cos(A[j-2]/g2r);  
      h1.x=(
		      (cp+h3.x*h3.x*emcp)*         asymmUnit[j].kart.x+
		      (-h3.z*sp+h3.x*h3.y*emcp)*  asymmUnit[j].kart.y+
		      (h3.y*sp+h3.x*h3.z*emcp)*   asymmUnit[j].kart.z);

      h1.y=( 
		      (h3.z*sp+h3.x*h3.y*emcp)*   asymmUnit[j].kart.x+
		      (cp+h3.y*h3.y*emcp)*	  asymmUnit[j].kart.y+
		      (-h3.x*sp+h3.y*h3.z*emcp)*  asymmUnit[j].kart.z);    

      h1.z=(
		      (-h3.y*sp+h3.x*h3.z*emcp)*  asymmUnit[j].kart.x+
		      (h3.x*sp+h3.y*h3.z*emcp)*   asymmUnit[j].kart.y+
		      (cp+h3.z*h3.z*emcp)*         asymmUnit[j].kart.z);

      if (j==2){
	asymmUnit[j].kart=h1;      
      }else {

	emcp=(1-cos(D[j-3]/g2r));
	sp=sin(D[j-3]/g2r);
	cp=cos(D[j-3]/g2r);  
	asymmUnit[j].kart.x=(
			(cp+h2.x*h2.x*emcp)*         h1.x+
			(-h2.z*sp+h2.x*h2.y*emcp)*  h1.y+
			(h2.y*sp+h2.x*h2.z*emcp)*   h1.z);

	asymmUnit[j].kart.y=( 
			(h2.z*sp+h2.x*h2.y*emcp)*   h1.x+
			(cp+h2.y*h2.y*emcp)*	  h1.y+
			(-h2.x*sp+h2.y*h2.z*emcp)*  h1.z);    

	asymmUnit[j].kart.z=(
			(-h2.y*sp+h2.x*h2.z*emcp)*  h1.x+
			(h2.x*sp+h2.y*h2.z*emcp)*   h1.y+
			(cp+h2.z*h2.z*emcp)*         h1.z);
      }

      asymmUnit[j].kart+=asymmUnit[b[j-1]-1].kart;



    }
  }else{ 
    smx=atmax+dummax;//mol.nListe=0;
  }
/*
  for (int i=0;i<smx;i++)
    if (asymmUnit[i].OrdZahl>-1){
      xs+=asymmUnit[i].kart.x;
      ys+=asymmUnit[i].kart.y;
      zs+=asymmUnit[i].kart.z;
    }
  xs/=(double)smx;
  ys/=(double)smx;
  zs/=(double)smx; 
  for (int i=0;i<smx;i++){
    asymmUnit[i].kart.x-=xs;
    asymmUnit[i].kart.y-=ys;
    asymmUnit[i].kart.z-=zs;
  }
*/
  fclose(adp);

//  packAct->setVisible(false);
  mol.zelle.a=1.0;
  mol.zelle.b=1.0;
  mol.zelle.c=1.0;
  mol.zelle.al=90.0;
  mol.zelle.be=90.0;
  mol.zelle.ga=90.0;
  smx=atmax+dummax;

  xdinp=asymmUnit;
  double dim=dimension(xdinp);
  if ((Norm(atom1Pos)==0)&&(Norm(atom2Pos)==0)) cubeGL->L=100.0/dim;
  /*if (mol.nListe>2) {
    free(mol.vL);
    mol.vL=NULL;
    mol.nListe=0;
  }*/
  cubeGL->resetENV();
  initLists(xdinp);
  cubeGL->setVisible ( true );
}

void MyWindow::openFile() {
  QString fileName;
  cubeGL->drawUz=true;
  cubeGL->drawAx=false;
  removeDipoleMoments();
  QString filwas=dirName.section('.',-1);
  QString selectedFilter=(filwas.startsWith('m'))?QString("Jana-Files (*.m*)"):QString();
  fileName = QFileDialog::getOpenFileName(this, tr("Open stucture file "), dirName,
		  "XD-Files (*.res *.inp *.mas);;"
		  "XD-Path-Files (*.pth);;"
                  "BayMEM-Input-Files (*.BayMEM);;"
		  "SHELX-Files (*.res *.ins);;"
                  "MoPro-Files (*.0* *.1* *.2* *.par);;"
		  "Gaussian COM-Files (*.com);;"
		  "Gaussian FChk-Files (*.fchk);;"
		  "CIF-Files (*.cif);;"
                  "Jana-Files (*.m*);;"
                  "XYZ-Files (*.xyz);;"
                  "VASP-Files (*.vasp);;"
		  "Protein Data Base file (*.pdb *.ent);;",&selectedFilter,QFileDialog::DontUseNativeDialog ); 
  if (!fileName.isEmpty()) {
    loadFile(fileName);
  }

  update();
}

void MyWindow::sortFilterDlg(){
  sfdlg=new QDialog(this);
  model = new QStandardItemModel(0, (maxResi)?8:6);
  model->setHeaderData(0, Qt::Horizontal,  ("Atom name"));
  model->setHeaderData(1, Qt::Horizontal,  ("Index"));
  model->setHeaderData(2, Qt::Horizontal,  ("Uiso"));
  model->setHeaderData(3, Qt::Horizontal,  ("Atomic number"));
  model->setHeaderData(4, Qt::Horizontal,  ("1st neighbors"));
  model->setHeaderData(5, Qt::Horizontal,  ("2nd neighbors"));
  if (maxResi){
    model->setHeaderData(6, Qt::Horizontal, ("Residue type"));  
    model->setHeaderData(7, Qt::Horizontal, ("Residue number"));
  }
  for (int i=0;i<smx;i++){
    model->insertRow(0);
    QString *an = new QString(xdinp[i].atomname);
    model->setData(model->index(0, 0), an->left(an->indexOf('@')));
    model->setData(model->index(0, 1), i);
    model->setData(model->index(0, 2), sqrt(xdinp[i].u.m11*xdinp[i].u.m11+xdinp[i].u.m22*xdinp[i].u.m22+xdinp[i].u.m33*xdinp[i].u.m33));//ueberpruefen Uiso formel
    model->setData(model->index(0, 3), (xdinp[i].OrdZahl>-1)?xdinp[i].OrdZahl+1:xdinp[i].OrdZahl);
    model->setData(model->index(0, 4), xdinp[i].firstNeighbors);
    model->setData(model->index(0, 5), xdinp[i].secondNeighbors);
    if (maxResi){
      QString *rs = new QString(xdinp[i].atomname);
      QString rsn=rs->mid(1+rs->lastIndexOf('@'));
      bool ok;
      int len= rsn.length()-rsn.indexOf(QRegExp("\\D+",Qt::CaseInsensitive,QRegExp::RegExp));
      rsn.chop(len);
      model->setData(model->index(0, 6),rs->right(len));
      model->setData(model->index(0, 7),rsn.toInt(&ok,10));
    }
  }
  proxyModel = new MySortFilterProxyModel();
  proxyModel->setDynamicSortFilter(true);
  proxyView = new QTreeView();
  proxyView->setRootIsDecorated(false);
  proxyView->setAlternatingRowColors(true);
  proxyView->setModel(proxyModel);
  proxyView->setSortingEnabled(true);
  proxyView->sortByColumn(1, Qt::AscendingOrder);
  proxyModel->setSourceModel(model);
  proxyView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  proxyView->setWhatsThis("This is a list of the filtered atoms. The Atoms in this Listed will be displayed after pressing the 'OK'-Button. All atom which are not in the list will be deleted.");

  QPushButton *okBut= new QPushButton ("OK");
  okBut->setDefault(true);
  QPushButton *cancelBut= new QPushButton (QIcon(":/images/cancel.png"),"Cancel",sfdlg);

  QLabel fromLabel("from:");
  QLabel toLabel("to:");
  toLabel.setAlignment(Qt::AlignRight);
  QLabel rgLabel("Patern:");
  regExpEdit= new QLineEdit("");
  regExpEdit->setWhatsThis("You can edit here <b>Perl</b>-like regular expressions.<p>The regular expressions work on the flieds 'Atom name' and 'Residue type'."
		  "<p>For more information about regular expressions see:<a href=\"http://www.regular-expressions.info/\">http://www.regular-expressions.info/</a>");
  fromEdit=new QLineEdit("0");
  fromEdit->setWhatsThis("Enter here the <b>minumum</b> number for the selectet field. The result includes the given value.");
  qv = new QDoubleValidator(-1.0E+99,1.0E+99,7,this);
  fromEdit->setValidator(qv);
  toEdit=new QLineEdit("1000000");
  toEdit->setWhatsThis("Enter here the <b>maximum</b> number for the selectet field.  The result includes the given value.");
  toEdit->setValidator(qv);
  filterColumnComboBox = new QComboBox;
  filterColumnComboBox->addItem(("Index"),1);		
  filterColumnComboBox->addItem(("Uiso"),2);		
  filterColumnComboBox->addItem(("Atomic number"),3);	 
  filterColumnComboBox->addItem(("1st neighbors"),4);	 
  filterColumnComboBox->addItem(("2nd neighbors"),5);	 
  if (maxResi)filterColumnComboBox->addItem( ("Residue number"),7);              
  filterColumnComboBox->setWhatsThis("This is a selector for the field to which the 'from:' and 'to:' values are applied.");
  QGridLayout *proxyLayout = new QGridLayout();
  proxyLayout->addWidget(filterColumnComboBox, 0,9);

  proxyLayout->addWidget(&rgLabel,   0, 0);
  proxyLayout->addWidget(regExpEdit, 0, 1,1,1);
  proxyLayout->addWidget(&fromLabel, 0, 10,1,1);
  proxyLayout->addWidget(fromEdit,   0, 11);
  proxyLayout->addWidget(&toLabel,   0, 13);
  proxyLayout->addWidget(toEdit,     0, 14);  
  proxyLayout->addWidget(proxyView,  1, 0,1,15);
  QGridLayout *butLayout = new QGridLayout();;
  butLayout->addWidget(cancelBut,  0, 13); 
  butLayout->addWidget(okBut,      0, 14); 

  QGroupBox *proxyGroupBox = new QGroupBox(("Sorted/Filtered Model"));
  proxyGroupBox->setLayout(proxyLayout);

  QVBoxLayout *mainLayout = new QVBoxLayout();
  mainLayout->addWidget(proxyGroupBox);
  mainLayout->insertLayout(-1,butLayout);
  sfdlg->setLayout(mainLayout);


  connect(fromEdit, SIGNAL(textChanged(const QString &)),
		  this, SLOT(valFilterChanged()));;
  connect(toEdit, SIGNAL(textChanged(const QString &)),
		  this, SLOT(valFilterChanged()));
  connect(filterColumnComboBox, SIGNAL(currentIndexChanged(int)),
		  this, SLOT(valFilterChanged()));
  connect(regExpEdit, SIGNAL(textChanged(const QString &)),
		  this, SLOT(textFilterChanged()));
  connect(cancelBut,SIGNAL(pressed()),
		  sfdlg,SLOT(close()));

  connect(okBut,SIGNAL(pressed()),
		  this,SLOT(filterXD()));
  textFilterChanged();
  valFilterChanged();
  sfdlg->setWindowTitle(("Custom Sort/Filter Model"));
  sfdlg->resize(800, 450);
  sfdlg->exec();
}

void MyWindow::unFilter(){
  cubeGL->disSelection();
  if (filtered){
    hFilter->setText("Hide hydrogen atoms");
    hFilter->setToolTip("Filter hydrogens out");
    hFilter->setChecked(true);
    dFilter->setText("Hide non atoms");
    dFilter->setToolTip("Filter dummys, peaks, holes and critical points out.");
    dFilter->setChecked(true);
    cubeGL->resetENV();
    if (oxd.size()){
      xdinp= oxd;
      oxd.clear();}
/*    for (int i = 0; i<xdinp.size(); i++)
      printf("%-10s %3d %9.5f %9.5f %9.5f jtf%d uc:%9.5f %9.5f %9.5f %9.5f %9.5f %9.5f \n",
          xdinp.at(i).atomname,
          xdinp.at(i).OrdZahl,
          xdinp.at(i).kart.x,
          xdinp.at(i).kart.y,
          xdinp.at(i).kart.z,
          xdinp.at(i).jtf,
          xdinp.at(i).u.m11,
          xdinp.at(i).u.m22,
          xdinp.at(i).u.m33,
          xdinp.at(i).u.m23,
          xdinp.at(i).u.m13,
          xdinp.at(i).u.m12
          );*/
    mol.bonds_made=0;
      mol.knopf_made=0;
      initLists( xdinp);
      filtered=0;
      unfilterAct->setVisible(false);
      cubeGL->updateGL();
  }
}

void MyWindow::filterXD(){

  unfilterAct->setVisible(true);
  if (!filtered){
    oxd.clear();
    oxd=xdinp;
  }
  QList<INP> fltrXdinp;
  for (int i=0;((i<xdinp.size())&&(proxyModel->data(proxyModel->index(i, 3)).toInt()!=0));i++){
    fltrXdinp.append(xdinp[proxyModel->data(proxyModel->index(i, 1)).toInt()]);
  }
  xdinp.clear();
  xdinp=fltrXdinp;
  mol.bonds_made=0;
  mol.knopf_made=0;
  initLists( fltrXdinp);
  filtered=1;
  sfdlg->close();
  update();
  cubeGL->updateGL();
}

void MyWindow::filterGroup(int index){
  if (index<0)return;
  fprintf(stderr,"filter! %d\n",index);
  if (!index) {
    unFilter();
  return;
  }
  unfilterAct->setVisible(true);
  if (!filtered){
    oxd.clear();
    oxd = xdinp;
  }
  else xdinp = oxd;
int idx=chargeGroups->itemData(index).toInt();
  QList<INP> fltrXdinp;
  for (int i=0;i<xdinp.size();i++){
    if (ladungsgruppen[idx].contains(xdinp[i].atomname)) {
      fltrXdinp.append(xdinp[i]);
//      printf("%d %d %s\n",index,i,xdinp[i].atomname);
    }
  }
  xdinp.clear();
  xdinp=fltrXdinp;
  mol.bonds_made=0;
  mol.knopf_made=0;
  initLists( fltrXdinp);
  filtered=1;
  update();
  cubeGL->updateGL();

}

void MyWindow::filterHydrogens(bool b){
  if (b) {
    unFilter();
    return;
  }
  cubeGL->disSelection();
  hFilter->setText("Show all objects");
  hFilter->setToolTip("Restrore all objects");
  unfilterAct->setVisible(true);
  if (!filtered){
    oxd.clear();
    oxd=xdinp;
  }
  QList<INP> fltrXdinp;
  for (int i=0;i<xdinp.size();i++){
    if (xdinp[i].OrdZahl!=0) fltrXdinp.append(xdinp[i]);
  }
  xdinp.clear();
  xdinp=fltrXdinp;
  mol.bonds_made=0;
  mol.knopf_made=0;
  initLists( fltrXdinp);
  filtered=1;
  update();
  cubeGL->updateGL();
}

void MyWindow::filterNonAtoms(bool b){
  cubeGL->disSelection();
  if (b) {
    unFilter();
    return;
  }
  dFilter->setText("Show all objects");
  dFilter->setToolTip("Restrore all objects");
  unfilterAct->setVisible(true);
  if (!filtered){
    oxd.clear();
    oxd=xdinp;
  }
  QList<INP> fltrXdinp;
  for (int i=0;i<xdinp.size();i++){
    if (xdinp[i].OrdZahl>=0) fltrXdinp.append(xdinp[i]);
  }
  xdinp.clear();
  xdinp=fltrXdinp;
  mol.bonds_made=0;
  mol.knopf_made=0;
  initLists( fltrXdinp);
  filtered=1;
  update();
  cubeGL->updateGL();
}

void MyWindow::filterSelectedAtoms(){
  if (cubeGL->selectedAtoms.isEmpty())return;
    unfilterAct->setVisible(true);
    if (!filtered){
      oxd=xdinp;
    }
    qDebug()<<"ok"<<__LINE__<<xdinp.size()<<cubeGL->selectedAtoms.size();
    if (xdinp.isEmpty())return;
    QList<INP> fltrXdinp;
//    qDebug()<<"ok"<<__LINE__;
    for (int i=0;i<xdinp.size();i++){
      bool issel=false;
      for (int j=0;j<cubeGL->selectedAtoms.size();j++) 
        if (cubeGL->selectedAtoms.at(j).kart==xdinp.at(i).kart) {
        issel=true;
   //     qDebug()<<cubeGL->selectedAtoms.at(j).atomname;
      }
      if (!issel) fltrXdinp.append(xdinp[i]);
    }
  //  qDebug()<<"ok"<<__LINE__;
    xdinp.clear();
    cubeGL->selectedAtoms.clear();
    xdinp=fltrXdinp;
    mol.bonds_made=0;
    mol.knopf_made=0;
   // qDebug()<<"ok"<<__LINE__;
    initLists( fltrXdinp);
   // qDebug()<<"ok"<<__LINE__;
    filtered=1;
    update();
    qDebug()<<"ok"<<__LINE__;
    cubeGL->updateGL();
    qDebug()<<"ok"<<__LINE__;
}

void MyWindow::filterThisAtom(){
  if (cubeGL->expandatom>-1){
    unfilterAct->setVisible(true);
    if (!filtered){
      oxd=xdinp;
    }
    QList<INP> fltrXdinp;
    for (int i=0;i<xdinp.size();i++){
      if (cubeGL->expandatom!=i) fltrXdinp.append(xdinp[i]);
    }
    xdinp.clear();
    xdinp=fltrXdinp;
    mol.bonds_made=0;
    mol.knopf_made=0;
    initLists( fltrXdinp);
    filtered=1;
    update();
    cubeGL->updateGL();
  }
}

void MyWindow::filterThisFragment(){
  if (cubeGL->expandatom>-1){
    unfilterAct->setVisible(true);
    if (!filtered){
      oxd=xdinp;
    }
    //double laen,SUCHRAD;
    QList<INP> mussweg;
    QList<int> mwi;
    int fragment=xdinp[cubeGL->expandatom].molindex;
    printf("Filter Wochen\n");
    mussweg.append(xdinp[cubeGL->expandatom]);
    mwi.append(cubeGL->expandatom);
    for (int j=0; j<mussweg.size();j++){
      for (int i=0;i<xdinp.size();i++){
	if (mwi.contains(i)) continue;
/*	laen=fl( 
			xdinp[i].frac.x-mussweg[j].frac.x, 
			xdinp[i].frac.y-mussweg[j].frac.y, 
			xdinp[i].frac.z-mussweg[j].frac.z);
	SUCHRAD=((
				mol.Kovalenz_Radien[xdinp[i].OrdZahl]+ 
				mol.Kovalenz_Radien[mussweg[j].OrdZahl]) -
			(0.08*fabs((double)mol.ElNeg[xdinp[i].OrdZahl] -mol.ElNeg[mussweg[j].OrdZahl])))*0.012;
	if ((laen>0.2)&&(laen<SUCHRAD))*/
	if (fragment==xdinp[i].molindex)
	{

	 // mussweg.append(xdinp[i]);
	  mwi.append(i);
	}
      }
    }
    QList<INP> fltrXdinp;
    for (int i=0;i<xdinp.size();i++){
      if (!mwi.contains(i)) fltrXdinp.append(xdinp[i]);
    }
    xdinp.clear();
    xdinp=fltrXdinp;
    mol.bonds_made=0;
    mol.knopf_made=0;
    initLists( fltrXdinp);
    filtered=1;
    update();
    cubeGL->updateGL();
  }
}

void MyWindow::filterDistant(){
  if (cubeGL->expandatom>-1){
    unfilterAct->setVisible(true);
    if (!filtered){
      oxd=xdinp;
    }
    V3 ego=xdinp[cubeGL->expandatom].frac;
    QList<INP> fltrXdinp;
    for (int i=0;i<xdinp.size();i++){
      if (4.0>fl(xdinp[i].frac.x-ego.x,xdinp[i].frac.y-ego.y,xdinp[i].frac.z-ego.z))fltrXdinp.append(xdinp[i]);
    }
    xdinp.clear();
    xdinp=fltrXdinp;
    mol.bonds_made=0;
    mol.knopf_made=0;
    initLists( fltrXdinp);
    filtered=1;
    update();
    cubeGL->updateGL();
  }
}

void MyWindow::showCoordinatesOfThis(){
  if ((cubeGL->expandatom>-1)&&(cubeGL->expandatom<xdinp.size())){
    int i=cubeGL->expandatom;
    printf("fractional coordinates of %s:\n%18.7f%18.7f%18.7f\n",
        xdinp[i].atomname,
        xdinp[i].frac.x,
        xdinp[i].frac.y,
        xdinp[i].frac.z);
    printf("Cartesian coordinates of %s:\n%18.7f%18.7f%18.7f\n",
        xdinp[i].atomname,
        xdinp[i].kart.x,
        xdinp[i].kart.y,
        xdinp[i].kart.z);
  }
}

void MyWindow::selectThisFragment(){
  if (cubeGL->expandatom>-1){
    //double laen,SUCHRAD;
    QList<int> mwi;
    cubeGL->selectedAtoms.clear();
    int fragment=xdinp[cubeGL->expandatom].molindex;
    printf("Filter Wochen\n");
    mwi.append(cubeGL->expandatom);
    cubeGL->selectedAtoms.append(xdinp[cubeGL->expandatom]);
      for (int i=0;i<xdinp.size();i++){
	if (mwi.contains(i)) continue;
/*	laen=fl( 
			xdinp[i].frac.x-mussweg[j].frac.x, 
			xdinp[i].frac.y-mussweg[j].frac.y, 
			xdinp[i].frac.z-mussweg[j].frac.z);
	SUCHRAD=((
				mol.Kovalenz_Radien[xdinp[i].OrdZahl]+ 
				mol.Kovalenz_Radien[mussweg[j].OrdZahl]) -
			(0.08*fabs((double)mol.ElNeg[xdinp[i].OrdZahl] -mol.ElNeg[mussweg[j].OrdZahl])))*0.012;
	if ((laen>0.2)&&(laen<SUCHRAD))*/
	if (fragment==xdinp[i].molindex)
	{

	 // mussweg.append(xdinp[i]);
	  mwi.append(i);
          cubeGL->selectedAtoms.append(xdinp[i]);
	}
      }
    update();
    cubeGL->updateGL();
  }
}

void MyWindow::filterOtherFragments(){
  if (cubeGL->expandatom>-1){
    unfilterAct->setVisible(true);
    if (!filtered){
      oxd=xdinp;
    }
    double laen,SUCHRAD;
    QList<INP> bleibtda;
    QList<int> mwi;
    bleibtda.append(xdinp[cubeGL->expandatom]);
    mwi.append(cubeGL->expandatom);
    for (int j=0; j<bleibtda.size();j++){
      for (int i=0;i<xdinp.size();i++){
	if (mwi.contains(i)) continue;
	if ((xdinp[i].OrdZahl<0)||(bleibtda[j].OrdZahl<0)) continue;
	laen=fl( 
			xdinp[i].frac.x-bleibtda[j].frac.x, 
			xdinp[i].frac.y-bleibtda[j].frac.y, 
			xdinp[i].frac.z-bleibtda[j].frac.z);
	SUCHRAD=((
			mol.Kovalenz_Radien[xdinp[i].OrdZahl]+ 
			mol.Kovalenz_Radien[bleibtda[j].OrdZahl]) -
			(0.08*fabs((double)mol.ElNeg[xdinp[i].OrdZahl] -mol.ElNeg[bleibtda[j].OrdZahl])))*0.012;
	if ((laen>0.2)&&(laen<SUCHRAD)){
	  bleibtda.append(xdinp[i]);
	  mwi.append(i);
	}
      }
    }
    QList<INP> fltrXdinp;
    for (int i=0;i<xdinp.size();i++){
      if (mwi.contains(i)) fltrXdinp.append(xdinp[i]);
    }
    xdinp.clear();
    xdinp=fltrXdinp;
    mol.bonds_made=0;
    mol.knopf_made=0;
    initLists( fltrXdinp);
    filtered=1;
    update();
    cubeGL->updateGL();
  }
}

void MyWindow::textFilterChanged() {
  QRegExp regExp(regExpEdit->text(), Qt::CaseInsensitive, QRegExp::RegExp);
  proxyModel->setFilterRegExp(regExp);
}

void MyWindow::valFilterChanged() {
  bool ok;
  proxyModel->setMyIndex(filterColumnComboBox->itemData(filterColumnComboBox->currentIndex()).toInt(&ok));
  proxyModel->setFilterMinimum(fromEdit->text().toDouble());
  proxyModel->setFilterMaximum(toEdit->text().toDouble());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MySortFilterProxyModel::MySortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {
  myIndex=1;
}

void MySortFilterProxyModel::setFilterMinimum(const double &val) {
  minVal = val;
  invalidateFilter();
}

void MySortFilterProxyModel::setMyIndex(const int &idx) {
  myIndex = idx;
  invalidateFilter();
}

void MySortFilterProxyModel::setFilterMaximum(const double &val) {
  maxVal = val;
  invalidateFilter();
}

bool MySortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
  QVariant leftData = sourceModel()->data(left);
  QVariant rightData = sourceModel()->data(right);

  if (leftData.type() == QVariant::String) {
    return leftData.toString() < rightData.toString();
  } else {
    return leftData.toDouble() < rightData.toDouble();
  }
}

bool MySortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
  QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
  QModelIndex index1 = sourceModel()->index(sourceRow, 6, sourceParent);
  QModelIndex index2 = sourceModel()->index(sourceRow, myIndex, sourceParent);
  QString s=sourceModel()->data(index0).toString();
  s+=" ";
  s+=sourceModel()->data(index1).toString();
  bool textMatch=s.contains(filterRegExp());
  return textMatch && valInRange(sourceModel()->data(index2).toDouble());
}

bool MySortFilterProxyModel::valInRange(const double &val) const {
  return (  val >= minVal)&& ( val <= maxVal);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MyWindow::setPackArt(int pa){
  if (pa==1) mol.gd=gdedit->value();
//  packdlg->accept();
  mol.bonds_made=0;
  mol.knopf_made=0;
  growSymm(pa);
  cubeGL->updateGL();
}

void MyWindow::setPackArt2(int pa){
  mol.bonds_made=0;
  mol.knopf_made=0;
  growSymm(pa);
  cubeGL->updateGL();
}

void MyWindow::setGD(double gd){
  mol.gd=gd;
  cubeGL->changeGDAct->setText(tr("Search radius is %1 Ang.  Change it").arg(mol.gd));
}

void MyWindow::changeGD(){
  bool ok;
  double GD = QInputDialog::getDouble(this,
		  tr("Grow distance"),
		  tr("Search radius"),
		  mol.gd,0.0,4.0,2,&ok);
  if (ok) mol.gd=GD;
  cubeGL->changeGDAct->setText(tr("Search radius is %1 Ang.  Change it").arg(mol.gd));
}
/*
void MyWindow::showPackDlg(){
  packdlg = new QDialog(this);
  QToolButton *molpck,*cctpck,*ccmpck,*grdpck;
  QPushButton *dntpck;
  gdedit=new QDoubleSpinBox();
  gdedit->setMinimum(0.0);
  gdedit->setMaximum(4.0);
  gdedit->setSingleStep(0.1);
  gdedit->setDecimals(3);
  gdedit->setSuffix(" Ang");
  gdedit->setValue(mol.gd);
  connect(gdedit,SIGNAL(valueChanged(double)),this,SLOT(setGD(double)));
  dntpck = new QPushButton();
  molpck = new QToolButton();
  cctpck = new QToolButton();
  ccmpck = new QToolButton();
  grdpck = new QToolButton();
  cctpck->setIcon(QIcon(":/images/unitcell.png"));
  molpck->setIcon(QIcon(":/images/compmol.png"));
  ccmpck->setIcon(QIcon(":/images/unitcellplus.png"));
  cctpck->setIconSize(QSize(32,32));
  ccmpck->setIconSize(QSize(32,32));
  molpck->setIconSize(QSize(32,32));
  molpck->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  cctpck->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  ccmpck->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  grdpck->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  dntpck->setDefault (true);
  dntpck->setText("Do not pack!");
  molpck->setText("Complete molecules");
  cctpck->setText("Pack inside unit cell");
  ccmpck->setText("Pack inside unit cell and complete");
  grdpck->setText("Pack shortest distances and complete");
  signalMapper = new QSignalMapper(this);
  signalMapper->disconnect();
  signalMapper->setMapping(dntpck,  0);
  signalMapper->setMapping(grdpck,  1);
  signalMapper->setMapping(molpck,  6);
  signalMapper->setMapping(cctpck,  3);
  signalMapper->setMapping(ccmpck,  4);
  connect(dntpck,SIGNAL(clicked()),signalMapper,SLOT(map()));
  connect(molpck,SIGNAL(clicked()),signalMapper,SLOT(map()));
  connect(cctpck,SIGNAL(clicked()),signalMapper,SLOT(map()));
  connect(ccmpck,SIGNAL(clicked()),signalMapper,SLOT(map()));
  connect(grdpck,SIGNAL(clicked()),signalMapper,SLOT(map()));
  connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(setPackArt(int)));
  QGridLayout sss;
  sss.addWidget(dntpck,4,2,Qt::AlignRight);
  sss.addWidget(molpck,0,2,Qt::AlignRight);
  sss.addWidget(ccmpck,1,2,Qt::AlignRight);
  sss.addWidget(cctpck,2,2,Qt::AlignRight);
  sss.addWidget(grdpck,3,2,Qt::AlignRight);
  sss.addWidget(gdedit,3,0);
  packdlg->setLayout(&sss);
  packdlg->setWindowTitle("Apply Symetry");
  packdlg->exec();
}
*/
void MyWindow::loadFile(QString fileName,double GD){//empty
//  printf("loadFILe %d\n",__LINE__);
  cubeGL->pause=true;
  modScale->hide();
  ModulationMenu->setEnabled(false);
  cubeGL->rename=false;
  hatlokale=0;
  tMovieStop();
//  for (int i=0;  mol.ombats.size(); i++) mol.wombats[i].pth.clear();
  mol.wombats.clear();
  mol.zelle.qr=mol.zelle.qi=mol.zelle.qvec=V3(0,0,0),
  exportShelxAtTvalueAct->setEnabled(false);
  mol.zelle.commensurate=false;
  tMovieStartAct->setEnabled(false);
  phaseSpin->hide();
  infoKanal->clear();
  fmcq->killmaps();
  QDir directory(fileName);
  bool same =false;
  {
  QString s1=directory.canonicalPath();
  QString s2=dirName;
  //printf("alife\n");
  if (s1.size()>4)  s1.chop(3); 
  if (s2.size()>4)  s2.chop(3); 
  //qDebug()<<s1<<s2;
  same=(s1==s2);
 // printf("blife\n");
  }
  dirName=directory.canonicalPath();
  fileName=dirName;
  cubeGL->afilename=fileName.section("/",-1);
  if (!same) cubeGL->rotze=-1;
  if (cubeGL->afilename.contains("."))
    cubeGL->afilename=cubeGL->afilename.section(".",-2,-2);
  infoKanal->setHtml(QString("<strong>File name:</strong><br> %1<hr>").arg(fileName));
  if (GD<0) GD=2.3;
  mol.gd=GD;
  
  cubeGL->changeGDAct->setText(tr("Search radius is %1 Ang.  Change it").arg(mol.gd));
  atmax=0;
  smx=0;
  xdinp.clear();
  QList<INP> miat=asymmUnit;
  asymmUnit.clear();
  masymmUnit.clear();
  matoms.clear();
  cubeGL->specialFragment1=false;
  mol.zelle.symmops.clear();
  mol.zelle.trans.clear();
  mol.zelle.x4sym.clear();
  mol.zelle.x4.clear();
  mol.zelle.x4tr.clear();
//  printf("loadFILe %d\n",__LINE__);
  george=false;
  if (!same) seReAct->setEnabled(false);
  if (!same) seReAct->setVisible(false);
  if (!same) xdMenu->setEnabled(false);
  fck=false;
 // if (!same) togLuft->setVisible(false);
  if (!same) togAxen->setEnabled (true );
  if (!same) togUnit->setEnabled (true );
  QDir::setCurrent ( fileName.left(fileName.lastIndexOf("/") ))  ;
  statusBar()->showMessage(QString(tr("Loading %1").arg(fileName)) );
  if (fileName.endsWith(".vasp", Qt::CaseInsensitive)) {
    load_vasp(fileName);
  }
  if (fileName.endsWith(".BayMEM", Qt::CaseInsensitive)) {
   // qDebug()<<fileName<<fileName.endsWith(".BayMEM");
    load_BayMEM(fileName);
  }
  if (((fileName.endsWith(".ini",Qt::CaseInsensitive)))||((fileName.endsWith(".dat",Qt::CaseInsensitive)))) return;
  if (fileName.endsWith(".mas", Qt::CaseInsensitive)){
    fileName.chop(4);
    fileName.append(".res");
    if (!QFile::exists ( fileName)) {
    fileName.chop(4);
    fileName.append(".inp");
    }
  }
  if ((fileName.endsWith(".pdb", Qt::CaseInsensitive))||(fileName.endsWith(".ent", Qt::CaseInsensitive))) load_pdb(fileName);
  if (fileName.endsWith(".fchk", Qt::CaseInsensitive)) {
    load_fchk(fileName);
    fck=true;  
    cubeGL->drawAx=false;
    cubeGL->drawUz=false;
    mol.adp=0;
    togAxen->setEnabled (false );
    togUnit->setEnabled (false );
  }
  if (fileName.endsWith(".pth", Qt::CaseInsensitive)) {
    
    mol.readXDPath(fileName);
//    packAct->setVisible(false);
    mol.zelle.a=1.0;
    mol.zelle.b=1.0;
    mol.zelle.c=1.0;
    mol.zelle.al=90.0;
    mol.zelle.be=90.0;
    mol.zelle.ga=90.0;
    mol.adp=0;
    V3 shift=V3(0.,0.,0.);
    if ((!miat.isEmpty())&&(cubeGL->moliso!=NULL)) {
    for (int miii=0; miii<miat.size();miii++)
      for (int piii=0; piii<asymmUnit.size(); piii++){
      if (!strcmp(miat.at(miii).atomname,asymmUnit.at(piii).atomname)){
      shift=miat.at(miii).kart-asymmUnit.at(piii).kart;
      break;
      }
      }
    }
    for (int pii=0; pii<asymmUnit.size();pii++) asymmUnit[pii].kart+= shift;
    for (int wb=0; wb<mol.wombats.size();wb++)
      for (int pb=0; pb< mol.wombats.at(wb).pth.size(); pb++) mol.wombats[wb].pth[pb]+=shift;
    xdinp=asymmUnit;
    double dim=dimension(xdinp);
    if ((Norm(atom1Pos)==0)&&(Norm(atom2Pos)==0)) 
      cubeGL->L=100.0/dim;
    
    /*if (mol.nListe>2) {
      free(mol.vL);
      mol.vL=NULL;
      mol.nListe=0;
      }*/

    togElli->setVisible ( false );
    toggleElli(false);
    cubeGL->setEllipsoidNoUpdate(false);
    cubeGL->resetENV();
    initLists(xdinp);
    cubeGL->setVisible ( true );
    cubeGL->drawAx=false;
    cubeGL->drawUz=false;
    mol.adp=0;
    togAxen->setEnabled (false );
    togUnit->setEnabled (false );
  }
  if ((fileName.endsWith(".com",Qt::CaseInsensitive))) {
    load_gaus(fileName);
    cubeGL->drawAx=false;
    cubeGL->drawUz=false;
    mol.adp=0;
    togAxen->setEnabled (false );
    togUnit->setEnabled (false );
  }
  if ((fileName.endsWith(".xyz",Qt::CaseInsensitive))) {
    load_xyz(fileName);
    cubeGL->drawAx=false;
    cubeGL->drawUz=false;
    togAxen->setEnabled (false );
    togUnit->setEnabled (false );
  }
  if (fileName.contains(QRegExp(".m\\d\\d$",Qt::CaseInsensitive))){
    cubeGL->setEllipsoidNoUpdate( true );
    togElli->setChecked ( true );
    togElli->setVisible ( true );
    mol.adp=1;
    cubeGL->drawAx=false;
    cubeGL->drawUz=true;
    load_Jana(fileName);
    if ((asymmUnit.size()<1)&&(masymmUnit.isEmpty())) {
      cubeGL->pause=false;
      atmax=0;
      smx=0;
      xdinp.clear();
      asymmUnit.clear();
      george=false;
      infoKanal->setHtml(QString("<strong>File name:</strong><br> %1<hr><strong>No atoms found in structure.</strong>").arg(fileName));
      return;
    }
    togUnit->setEnabled (true );
    togAxen->setChecked ( cubeGL->drawAx );
  }
  else if ((fileName.endsWith(".par"))||(fileName.contains(QRegExp(".\\d\\d$")))){
      cubeGL->setEllipsoidNoUpdate( true );
      togElli->setChecked ( true );
      togElli->setVisible ( true );
      cubeGL->drawUz=true;
      cubeGL->drawAx=true;
      hatlokale=1;
      togAxen->setEnabled (true );
      togAxen->setChecked ( cubeGL->drawAx );
      load_MoPro(fileName);
  }
  if ((fileName.endsWith(".cif",Qt::CaseInsensitive))) {
    george=true;
    cubeGL->drawAx=false;
    load_cif(fileName);
    cubeGL->drawUz=true;
    togUnit->setEnabled (true );
    togAxen->setChecked ( cubeGL->drawAx );
  }
  if ((fileName.endsWith(".res", Qt::CaseInsensitive))||(fileName.endsWith(".ins", Qt::CaseInsensitive))||(fileName.endsWith(".inp", Qt::CaseInsensitive))) {	
    char test[250];
    FILE *xxx;
    if (!same) mol.adp=1;
    if (!same) cubeGL->setEllipsoidNoUpdate( true );
    if (!same) togElli->setChecked ( true );
    if (!same) togElli->setVisible ( true );
    if (!same) cubeGL->drawUz=true;
    if ((xxx=fopen(fileName.toLocal8Bit().data(),"r"))==NULL) {fprintf(stderr,"Can't open %s!!!\n",fileName.toLocal8Bit().data());exit(2);}      
    do{
    egal=fscanf(xxx,"%[^\n\r]\n\r",test);
    printf("test %s\n",test);
    } while (test[0]=='!');
    fclose(xxx);
  //printf("loadFILe %d %d\n",__LINE__,strcmp(test,"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
    if (strstr(test,"XDPARFILE")==NULL){ 
      load_sheldrick(fileName);}
    else    
      load_xdres(fileName);    
    togAxen->setChecked ( cubeGL->drawAx );
//  printf("loadFILe %d\n",__LINE__);
  }
  updateStatusBar();
  QStringList aSymmList;
  for (int i=0;i<asymmUnit.size();i++) {
    aSymmList.append(asymmUnit.at(i).atomname);
  }
  QCompleter *cc = new QCompleter(aSymmList,this);
  cc->setCaseSensitivity(Qt::CaseInsensitive);
  searchAtomEdit->setCompleter(cc);
  mol.sfac.clear();
  for (int i=0;i<asymmUnit.size();i++) {
      if ((!mol.sfac.contains(asymmUnit.at(i).OrdZahl))&&(asymmUnit.at(i).OrdZahl>=0))
          mol.sfac.append(asymmUnit.at(i).OrdZahl);
  }
  sfacMenu->clear();
  {
  QAction *a=sfacMenu->addAction("Polyeder color mode",this,SLOT(polyColorIng(bool)));
  a->setCheckable(true);
  a->setChecked(false);
  }
  sfacMenu->addSeparator();
  for (int i=0; i<mol.sfac.size();i++){
      QAction *a=sfacMenu->addAction(mol.pse(mol.sfac.at(i)),this,SLOT(allowPolyeder()));
      a->setCheckable(true);
      a->setChecked(mol.allowedPolyeders.value(mol.sfac.at(i),true));
      a->setData(mol.sfac.at(i));
  }
  mol.multiplicity(asymmUnit);
  statusBar()->showMessage(tr("File succesfully loaded.") );
  // Zuletzt geffnete File setzen
  if (mol.einstellung->group()!="Version 0.1")mol.einstellung->beginGroup("Version 0.1");
  mol.einstellung->setValue("lastFile", fileName );
  QStringList files = mol.einstellung->value("recentFileList").toStringList();
  files.removeAll(fileName);//alle identischen "fileName" aus der Liste entfernen
  files.prepend(fileName);// und dann vorne anfuegen...
  while (files.size() > MaxRecentFiles)
    files.removeLast();//hinten abschneiden was lnger als MaxRecentFiles ist
  mol.einstellung->setValue("recentFileList", files);
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MyWindow *mainWin = qobject_cast<MyWindow *>(widget);
    if (mainWin)
      mainWin->updateRecentFileActions();
  }
  
  if (mol.einstellung->contains("UnitCell")){
    bool b;
    togUnit->setChecked(b=mol.einstellung->value("UnitCell").toBool());
    cubeGL->setUnitCell(b);
  }
  mol.einstellung->endGroup();
  filtered=0;
//  printf("loadFILe %d\n",__LINE__);
  if (symmsalabymm) mol.whatSymm();
  cubeGL->pause=false;
  cubeGL->updateGL();
}

void MyWindow::setFFMPEGEXE(){
   mol.ffmpegexe= QFileDialog::getOpenFileName(this,"path to ffmpeg.exe","ffmpeg.exe","",NULL,QFileDialog::DontUseNativeDialog );
   if (ffmpegedt!=NULL) ffmpegedt->setText(mol.ffmpegexe);
}

void MyWindow::setMP4(){
   QString s = QFileDialog::getSaveFileName(this,"save mp4 file","test.mp4","Movie file (*.mp4)",NULL,QFileDialog::DontUseNativeDialog );
   if (mp4file!=NULL) mp4file->setText(s);
}

void MyWindow::updateOutput(){
  if (runffmpeg->pid()) {
    brs->show();
    QString s=brs->toPlainText();
    s.append(runffmpeg->readAllStandardOutput());
    s.append(runffmpeg->readAllStandardError());
    brs->setPlainText(s);
  }else{
    brs->show();
    QString s=brs->toPlainText();
    s.append(runffmpeg->readAllStandardOutput());
    s.append(runffmpeg->readAllStandardError());
    brs->setPlainText(s);
    QDir d=QDir::current();

    QStringList clips;
    clips.append("molisoclip*");
    clips  = d.entryList(clips);
    for (int i=0; i<clips.size();i++) QFile::remove(clips.at(i));
    QFile::remove("mcqwatermark.png");
    qint64 size = 0;
    QFile myFile(fffn);
    if (myFile.open(QIODevice::ReadOnly)){
      size = myFile.size();  //when file does open.
      myFile.close();
    }
    int kMGT=0;
    double siz=(double)size;
    while (siz>1024){
     siz/=1024;
     kMGT++;
    }
    QString byte="B";
    switch (kMGT){ 
      case 1: byte.prepend("k"); break;
      case 2: byte.prepend("M"); break;
      case 3: byte.prepend("G"); break;
      case 4: byte.prepend("T"); break;
    }
    QString fss=QString("%1 %2").arg(siz, 0, 'f', 2).arg(byte);

    ffl->setText(QString("finished! %1 has %2").arg(fffn).arg(fss));
    bar->hide();
    closeFF->show();
  }
}

void MyWindow::makeTMovi(){
  QDialog *tdlg = new QDialog(this);
  QGridLayout *l = new QGridLayout(tdlg);
  QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), tdlg, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), tdlg, SLOT(reject()));  
  ffmpegedt = new QLineEdit(mol.ffmpegexe);
  QPushButton *ffbrs = new QPushButton("Browse");
  connect(ffbrs,SIGNAL(pressed()),this,SLOT(setFFMPEGEXE()));
  QLabel *ffmpegtxt =new QLabel("Path to ffmpeg.exe <br>(You need ffmpeg.exe. You can get it from here <a href=\"https://ffmpeg.org/download.html\">https://ffmpeg.org/download.html</a> )");
  ffmpegtxt->setOpenExternalLinks(true);
  QRadioButton *hd720 = new QRadioButton("1280x720",tdlg);
  QRadioButton *hd1080 = new QRadioButton("1920x1080",tdlg);
  hd1080->setChecked(true);
  mp4file = new QLineEdit("test.mp4");
  QPushButton *mp4brs = new QPushButton("Browse");
  connect(mp4brs,SIGNAL(pressed()),this,SLOT(setMP4())); 
  QSpinBox *dura= new QSpinBox(tdlg);
  dura->setMinimum(5);
  dura->setMaximum(300);
  dura->setValue(10);
  dura->setSuffix("s");
  QSpinBox *fps= new QSpinBox(tdlg);
  fps->setMinimum(12);
  fps->setMaximum(60);
  fps->setValue(30);

  l->addWidget(ffmpegtxt,0,0,1,20);
  l->addWidget(ffmpegedt,1,0,1,18);
  l->addWidget(ffbrs,1,19,1,1);
  l->addWidget(hd720,2,0,1,5);
  l->addWidget(hd1080,2,6,1,5);
  l->addWidget(new QLabel("Duration"),3,0,1,1);
  l->addWidget(dura,3,1,1,1);
  l->addWidget(new QLabel("Frames per second (FPS)"),4,0,1,1);
  l->addWidget(fps,4,1,1,1);
  l->addWidget(new QLabel("mp4 file name"),5,0,1,1);
  l->addWidget(mp4file,5,1,1,18);
  l->addWidget(mp4brs,5,19,1,1);

  l->addWidget(buttonBox,20,0,1,20);
  if(tdlg->exec()==QDialog::Accepted) {
    if (mol.ffmpegexe.isEmpty()||mp4file->text().isEmpty()) return;
    QString fileName=mp4file->text();
    if (QFile::exists(fileName)) {
      QString n=fileName;
      QString re=QString("_%1_.mp4").arg(QDateTime::currentMSecsSinceEpoch (),0,36);
      n.replace(QRegExp(".mp4$"),re);
      QFile::rename(fileName,n);
    }
    int d,e,f,h;
    bool hd=(hd720->isChecked()) ;
    int frameRate=fps->value();
    int frames = dura->value()*frameRate;
    int wi = (hd) ? 1280 : 1920;
    int hi = (hd) ? 720  : 1080;
    d = cubeGL->_win_width;
    e = cubeGL->_win_height;
    f = cubeGL->myFont.pointSize ();
    h = cubeGL->MLegendFont.pointSize ();
    printf("%p\n",cubeGL);
    QDialog *q =new QDialog(this);
    q->setWindowTitle("Wait! t-Movie file creation in progress");
    bar = new QProgressBar(q);
    bar->setMinimum(0);
    bar->setMaximum(frames);
    QGridLayout *g= new QGridLayout(q);
    g->addWidget(bar,0,0);
    ffl=new QLabel(QString("Please wait until t-movie is created!! %1 frames will be created and joined to a mp4 movie file.").arg(frames));
    g->addWidget(ffl,1,0);
    brs=new QTextBrowser(q);
    QTextCharFormat fmt;
    brs->setFontFamily("Courier");
    QPalette p = brs->palette();
    p.setBrush(QPalette::Active,QPalette::Base,QColor("#242424"));
    brs->setPalette (p );
    fmt.setFontFamily("Courier");
    fmt.setBackground(QColor("#242424"));
    fmt.setForeground(QColor("#DDDD00"));
    fmt.setFontWeight(50);
    brs->setCurrentCharFormat(fmt); 
    brs->setWordWrapMode(QTextOption::WordWrap);
    brs->setPlainText("====================\n Output of ffmpeg: \n====================\n\n");
  
    brs->hide();
    g->addWidget(brs,2,0);
    closeFF = new QPushButton("close", q);
    g->addWidget(closeFF,3,0);
    connect(closeFF,SIGNAL(released()),q,SLOT(close()));
    closeFF->hide();
        
    q->show();
    double stepssi= 1.0/frames;
    QPixmap   map;
    printf("stepsize=%f frames %d\n",stepssi,frames);
    for (int i=0; i<frames; i++){
      ffl->setText(QString("Please wait until t-movie is created!! %1 of %2").arg(i).arg(frames));
      bar->setValue(i);
      char fname[255] ;
    //  statusBar()->showMessage(tr("create pic #%1 of %2").arg(i+1).arg(frames) );
      sprintf(fname, "molisoclip%04d.png", i);
      cubeGL->noWaitLabel=true;
      cubeGL->paparazi=true;
      glGetDoublev(GL_MODELVIEW_MATRIX,cubeGL->MM);
      map = cubeGL->renderPixmap(wi,hi);
      cubeGL->paparazi=false;
      map.save(fname);
      cubeGL->tvalue=fmod(cubeGL->tvalue+stepssi,1.0);
     // cubeGL->updateGL();
      q->update();
      q->repaint();
    }
        if (!fileName.isEmpty()) {
          QPixmap watermark = QIcon(":/images/logo.png").pixmap(200, 34);
          watermark.save("mcqwatermark.png");
        QStringList al;
        al <<"-r"<< QString("%1").arg(frameRate)<< "-f"<< "image2"<< "-s"<< QString("%1").arg((hd)?"1280x720":"1920x1080")
           <<"-start_number"<< "0"<< "-i"<< "molisoclip%04d.png"<< 
           "-i"<< "mcqwatermark.png"<< "-filter_complex"<< QString("[0:v][1:v] overlay=%1:%2").arg(wi-230).arg(hi-40)<<           
           "-vframes"<< QString("%1").arg(frames)<< "-codec:v"<< "libx264" <<"-crf" <<"25"<< "-pix_fmt"<< "yuv420p"<< fileName;
           
        runffmpeg = new QProcess(this);
        QDir dn=QDir(QDir::current());  
        runffmpeg->setWorkingDirectory(QDir::current().absolutePath());                           // updateXLput()
        connect(runffmpeg,SIGNAL(readyReadStandardOutput ()),this,SLOT(updateOutput()));
        connect(runffmpeg,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(updateOutput()));
        connect(runffmpeg,SIGNAL(error(QProcess::ProcessError)),this,SLOT(updateOutput())); 

        runffmpeg->start(mol.ffmpegexe,al);
        fffn=fileName;
        updateOutput();

        }
    cubeGL->myFont.setPointSize(f);
    cubeGL->MLegendFont.setPointSize(h);
    cubeGL->_win_width=d;
    cubeGL->_win_height=e;
    cubeGL->noWaitLabel=cubeGL->quickRot->isChecked();
   //q->close();
   // filmFred = new TMovieThread(frames,wi,hi,d,e,f,h,cubeGL,fileName,hd720->isChecked());
    //connect(filmFred,SIGNAL(finished ()),q,SLOT(close()));
   // filmFred->start();
  } 
}

void MyWindow::makeRotMovi(){
  QDialog *tdlg = new QDialog(this);
  QGridLayout *l = new QGridLayout(tdlg);
  QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), tdlg, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), tdlg, SLOT(reject()));  
  ffmpegedt = new QLineEdit(mol.ffmpegexe);
  QPushButton *ffbrs = new QPushButton("Browse");
  connect(ffbrs,SIGNAL(pressed()),this,SLOT(setFFMPEGEXE()));
  QLabel *ffmpegtxt =new QLabel("Path to ffmpeg.exe <br>(You need ffmpeg.exe. You can get it from here <a href=\"https://ffmpeg.org/download.html\">https://ffmpeg.org/download.html</a> )");
  ffmpegtxt->setOpenExternalLinks(true);
  QRadioButton *hd720 = new QRadioButton("1280x720",tdlg);
  QRadioButton *hd1080 = new QRadioButton("1920x1080",tdlg);
  hd1080->setChecked(true);
  mp4file = new QLineEdit("test.mp4");
  QPushButton *mp4brs = new QPushButton("Browse");
  connect(mp4brs,SIGNAL(pressed()),this,SLOT(setMP4())); 
  QSpinBox *dura= new QSpinBox(tdlg);
  dura->setMinimum(5);
  dura->setMaximum(300);
  dura->setValue(10);
  dura->setSuffix("s");
  QSpinBox *fps= new QSpinBox(tdlg);
  fps->setMinimum(12);
  fps->setMaximum(60);
  fps->setValue(30);

  l->addWidget(ffmpegtxt,0,0,1,20);
  l->addWidget(ffmpegedt,1,0,1,18);
  l->addWidget(ffbrs,1,19,1,1);
  l->addWidget(hd720,2,0,1,5);
  l->addWidget(hd1080,2,6,1,5);
  l->addWidget(new QLabel("Duration"),3,0,1,1);
  l->addWidget(dura,3,1,1,1);
  l->addWidget(new QLabel("Frames per second (FPS)"),4,0,1,1);
  l->addWidget(fps,4,1,1,1);
  l->addWidget(new QLabel("mp4 file name"),5,0,1,1);
  l->addWidget(mp4file,5,1,1,18);
  l->addWidget(mp4brs,5,19,1,1);

  l->addWidget(buttonBox,20,0,1,20);
  if(tdlg->exec()==QDialog::Accepted) {
    if (mol.ffmpegexe.isEmpty()||mp4file->text().isEmpty()) return;
    QString fileName=mp4file->text();
    if (QFile::exists(fileName)) {
      QString n=fileName;
      QString re=QString("_%1_.mp4").arg(QDateTime::currentMSecsSinceEpoch (),0,36);
      n.replace(QRegExp(".mp4$"),re);
      QFile::rename(fileName,n);
    }
    int d,e,f,h;
    bool hd=(hd720->isChecked()) ;
    int frameRate=fps->value();
    int frames = dura->value()*frameRate;
    int wi = (hd) ? 1280 : 1920;
    int hi = (hd) ? 720  : 1080;
    d = cubeGL->_win_width;
    e = cubeGL->_win_height;
    f = cubeGL->myFont.pointSize ();
    h = cubeGL->MLegendFont.pointSize ();
    printf("%p\n",cubeGL);
    QDialog *q =new QDialog(this);
    q->setWindowTitle("Wait! Movie file creation in progress");
    bar = new QProgressBar(q);
    bar->setMinimum(0);
    bar->setMaximum(frames);
    QGridLayout *g= new QGridLayout(q);
    g->addWidget(bar,0,0);
    ffl=new QLabel(QString("Please wait until movie is created!! %1 frames will be created and joined to a mp4 movie file.").arg(frames));
    g->addWidget(ffl,1,0);
    brs=new QTextBrowser(q);
    QTextCharFormat fmt;
    brs->setFontFamily("Courier");
    QPalette p = brs->palette();
    p.setBrush(QPalette::Active,QPalette::Base,QColor("#242424"));
    brs->setPalette (p );
    fmt.setFontFamily("Courier");
    fmt.setBackground(QColor("#242424"));
    fmt.setForeground(QColor("#DDDD00"));
    fmt.setFontWeight(50);
    brs->setCurrentCharFormat(fmt); 
    brs->setWordWrapMode(QTextOption::WordWrap);
    brs->setPlainText("====================\n Output of ffmpeg: \n====================\n\n");
  
    brs->hide();
    g->addWidget(brs,2,0);
    closeFF = new QPushButton("close", q);
    g->addWidget(closeFF,3,0);
    connect(closeFF,SIGNAL(released()),q,SLOT(close()));
    closeFF->hide();
        
    q->show();
    double stepssi= 1.0/frames;
    QPixmap   map;
    printf("stepsize=%f frames %d\n",stepssi,frames);
    for (int i=0; i<frames; i++){
      ffl->setText(QString("Please wait until movie is created!! %1 of %2").arg(i).arg(frames));
      bar->setValue(i);
      char fname[255] ;
    //  statusBar()->showMessage(tr("create pic #%1 of %2").arg(i+1).arg(frames) );
      sprintf(fname, "molisoclip%04d.png", i);
      cubeGL->noWaitLabel=true;
      cubeGL->paparazi=true;
      glGetDoublev(GL_MODELVIEW_MATRIX,cubeGL->MM);
      map = cubeGL->renderPixmap(wi,hi);
      cubeGL->paparazi=false;
      map.save(fname);
      cubeGL->rotate(360.0/frames,0.0,1.0,0.0);
     // cubeGL->updateGL();
      q->update();
      q->repaint();
    }
        if (!fileName.isEmpty()) {
          QPixmap watermark = QIcon(":/images/logo.png").pixmap(200, 34);
          watermark.save("mcqwatermark.png");
        QStringList al;
        al <<"-r"<< QString("%1").arg(frameRate)<< "-f"<< "image2"<< "-s"<< QString("%1").arg((hd)?"1280x720":"1920x1080")
           <<"-start_number"<< "0"<< "-i"<< "molisoclip%04d.png"<< 
           "-i"<< "mcqwatermark.png"<< "-filter_complex"<< QString("[0:v][1:v] overlay=%1:%2").arg(wi-230).arg(hi-40)<<           
           "-vframes"<< QString("%1").arg(frames)<< "-codec:v"<< "libx264" <<"-crf" <<"25"<< "-pix_fmt"<< "yuv420p"<< fileName;
           
        runffmpeg = new QProcess(this);
        QDir dn=QDir(QDir::current());  
        runffmpeg->setWorkingDirectory(QDir::current().absolutePath());                           // updateXLput()
        connect(runffmpeg,SIGNAL(readyReadStandardOutput ()),this,SLOT(updateOutput()));
        connect(runffmpeg,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(updateOutput()));
        connect(runffmpeg,SIGNAL(error(QProcess::ProcessError)),this,SLOT(updateOutput())); 

        runffmpeg->start(mol.ffmpegexe,al);
        fffn=fileName;
        updateOutput();

        }
    cubeGL->myFont.setPointSize(f);
    cubeGL->MLegendFont.setPointSize(h);
    cubeGL->_win_width=d;
    cubeGL->_win_height=e;
    cubeGL->noWaitLabel=cubeGL->quickRot->isChecked();
   //q->close();
   // filmFred = new TMovieThread(frames,wi,hi,d,e,f,h,cubeGL,fileName,hd720->isChecked());
    //connect(filmFred,SIGNAL(finished ()),q,SLOT(close()));
   // filmFred->start();
  } 
}

void MyWindow::openDipoleFile() {
  QString fileName;
  QString selectedFilter;
  fileName = QFileDialog::getOpenFileName(this, tr("Dipole Moments File "), dirName,
		  "Dipole.in (*.in);;"
		  "Any (*);;",&selectedFilter,QFileDialog::DontUseNativeDialog  );
  if (!fileName.isEmpty()) {
    loadDipoleMoments(fileName);
  }    
  update();
}

void MyWindow::allowPolyeder(){
    QAction *action = qobject_cast<QAction *>(sender());
    int index=0;
    if (action)
    index=action->data().toInt();
    else return;
    if (mol.allowedPolyeders.contains(index))
    mol.allowedPolyeders[index]=!mol.allowedPolyeders.value(index);
    else mol.allowedPolyeders[index]=false;
    cubeGL->updateGL();

}

void MyWindow::removeDipoleMoments(){
  cubeGL->pole.clear();
  cubeGL->farben.clear();
  nodipAct->setVisible ( false);
  ldipAct->setVisible ( true);
  cubeGL->updateGL();
}

void MyWindow::loadDipoleMoments(QString fileName){
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  //                                                    DIPOLE                                                                      //
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//    
  QFile DIPF(fileName);
  if (DIPF.open(QIODevice::ReadOnly)){
    QStringList zeiger,zeilen=QString(DIPF.readAll()).split("\n",QString::SkipEmptyParts);
    for(int i=0; i<zeilen.size ();i++){
      zeiger=zeilen.at(i).split(QRegExp("[^\\-\\.0-9]+"),QString::SkipEmptyParts);
      V3 dipl,org=V3(666,666,666);
      QColor color;
      if (zeilen.at(i).contains("(")) dipl=V3(zeiger.at(0).toDouble(),zeiger.at(2).toDouble(),zeiger.at(4).toDouble());
      else dipl=V3(zeiger.at(0).toDouble(),zeiger.at(1).toDouble(),zeiger.at(2).toDouble());
      if (zeilen.at(i).contains("##")) {
	dipl=V3(zeiger.at(0).toDouble(),zeiger.at(1).toDouble(),zeiger.at(2).toDouble());
	color.setRgbF(zeiger.at(3).toDouble(),zeiger.at(4).toDouble(),zeiger.at(5).toDouble());
	cubeGL->farben.append(color);
      }
      if (zeilen.at(i).contains(QRegExp("origin="))){
          QString aname= zeilen.at(i).split(QRegExp("\\{|\\}"),QString::SkipEmptyParts).at(1);
          QStringList oo= aname.split(QRegExp("[^\\-\\.0-9]+"),QString::SkipEmptyParts);
          if (oo.size()>2) {
              org=V3(oo.at(0).toDouble(),oo.at(1).toDouble(),oo.at(2).toDouble());
          }
      }
      if (zeilen.at(i).contains(QRegExp("color="))){
	QString aname= zeilen.at(i).split(QRegExp("\""),QString::SkipEmptyParts).at(1);
	color.setNamedColor(aname);
	cubeGL->farben.append(color);
      }      
      cubeGL->pole.append(dipl);
      cubeGL->poleOrg.append(org);
    }
    DIPF.close();
  }
  if ((cubeGL->pole.isEmpty())){

    nodipAct->setVisible ( false);
    ldipAct->setVisible ( true);
  }else{
    QString content="";
    for (int i=0; i<cubeGL->pole.size(); i++){ 
      if (cubeGL->farben.size()<=i) cubeGL->farben.append(QColor(QColor::colorNames().at(i)));
      content.append(QString("<tr><td><font color=\"%5\">Dipole_%1</font></td><td>%2</td><td>%3</td><td>%4</td></tr>")
		      .arg(i).arg(cubeGL->pole.at(i).x,16,'f',4).arg(cubeGL->pole.at(i).y,16,'f',4).arg(cubeGL->pole.at(i).z,16,'f',4).arg(cubeGL->farben.at(i).name()));
    }
    infoKanalNews(QString("Dipole Moments:<table><tr><th>Dipole</th><th>X</th><th>Y</th><th>Z</th><tr>%1</table>").arg(content));
    nodipAct->setVisible ( true);
    ldipAct->setVisible ( false);
  }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  //                                                    DIPOLE                                                                      //
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  update();
}

void  MyWindow::exportFMaps(){
  QString al;
  const double a0 = 0.52917720859;
  int ina=0;
  for (int i=0; i<asymmUnit.size();i++){
    if (asymmUnit[i].OrdZahl<0) continue;
    ina++;
  }
  for (int i=0; i<asymmUnit.size();i++){
    if (asymmUnit[i].OrdZahl<0) continue;
    mol.frac2kart(asymmUnit[i].frac,asymmUnit[i].kart);
    al.append(QString("\n%1%2%3%4%5").arg(asymmUnit[i].OrdZahl+1,5).arg(asymmUnit[i].OrdZahl+1.0,12,'f',6).arg(asymmUnit[i].kart.x/a0,12,'f',6).arg(asymmUnit[i].kart.y/a0,12,'f',6).arg(asymmUnit[i].kart.z/a0,12,'f',6));
  }
  fmcq->exportMaps(ina, dirName.toStdString().c_str(), al.toStdString().c_str());

}

void MyWindow::initLists(QList<INP> xd){
  dFilter->setVisible(true);
  hFilter->setVisible(true);
 bool singleColorBonds_state = mol.bondColorStyle;
 bool tubifiedAtoms_state=mol.tubifiedAtoms;
  int adpstate=mol.adp;
  int mx=xd.size();
  statusBar()->showMessage(tr("Draw Atoms.") );	
//qDebug()<<"0";

  for (int j=0;j<mx;j++)               
    xd[j].labPos=xd[j].kart;
  if (!cubeGL->bas) cubeGL->bas=glGenLists(10);


//qDebug()<<"1";
 glNewList(cubeGL->bas, GL_COMPILE );{                          //ATOME
//qDebug()<<"1";
    glPushMatrix();{
//qDebug()<<"1";
      glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
//qDebug()<<"1";
      mol.intern=1;
//qDebug()<<"1";
      mol.atoms(xd,mol.proba);
//qDebug()<<"1";
    }glPopMatrix();    
//qDebug()<<"1";
  }glEndList();

//Debug()<<"2";


  glNewList(cubeGL->bas+8, GL_COMPILE );{       //bonds in single color
    glPushMatrix();{
      glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
      mol.intern=1;
      mol.bondColorStyle=true;
      mol.bonds(xd);
    }glPopMatrix();    
  }glEndList();

//Debug()<<"3";

  glNewList(cubeGL->bas+7, GL_COMPILE );{                          //ATOME
    glPushMatrix();{
      glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
      mol.intern=0;
      mol.atoms(xd,mol.proba);
    }glPopMatrix();    
  }glEndList();

//Debug()<<"4";

  glNewList(cubeGL->bas+4, GL_COMPILE );{                          //ATOME
    glPushMatrix();{
      glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
      mol.adp=0;
      mol.atoms(xd,50);
    }glPopMatrix();    
  }glEndList();

//Debug()<<"5";
  glNewList(cubeGL->bas+9, GL_COMPILE );{       //Atome fuer tube syle
    glPushMatrix();{
      glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
      mol.intern=1;
      mol.tubifiedAtoms=true;
      mol.atoms(xd,mol.proba);
    }glPopMatrix();    
  }glEndList();
  mol.tubifiedAtoms=false;

//Debug()<<"6";

  glNewList(cubeGL->bas+1, GL_COMPILE );{                          //BONDS
    glPushMatrix();{
      glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
      mol.adp=0;      
      mol.bondColorStyle=false;
      mol.bonds(xd);
    }glPopMatrix();    
  }glEndList();


//Debug()<<"7";
 mol.adp=adpstate;

  if (hatlokale) {

//qDebug()<<"1xyz";
    statusBar()->showMessage(tr("Draw local coordinate systems.") );	
    togAxen->setVisible(true);
    glNewList(cubeGL->bas+2, GL_COMPILE );{                          //Axen
      glDisable( GL_DEPTH_TEST ); 
      glPushMatrix();{
	glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
	mol.axes(xd); 
   // printf("xyz axen\n");
      }glPopMatrix();    
      glEnable( GL_DEPTH_TEST );
    }glEndList();
  }
  else togAxen->setVisible(false);
//  togUnit->setVisible(packAct->isVisible());
//  if (packAct->isVisible()) 
  {

    statusBar()->showMessage(tr("Draw unit cell.") );
    glNewList(cubeGL->bas+3, GL_COMPILE );{                          //Unit Zell
      glPushMatrix();{
	glScaled( cubeGL->L, cubeGL->L, cubeGL->L );           
	mol.UnitZell(); 
      }glPopMatrix();    
    }glEndList();
  }
  if (mol.HAMax!=0.0){

    statusBar()->showMessage(tr("Draw H-bonds.") );	
    glNewList(cubeGL->bas+5, GL_COMPILE );{                          //H...X-Bindungen
      glPushMatrix();{
	glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
	hbKanal->setHtml(mol.h_bonds(xd));
      }glPopMatrix();    
    }glEndList();
    if (!hbKanal->toPlainText().isEmpty()){
      dock2->show();
      dock2->raise();
    }
  }
  /*if (mol.nListe>2){
   // togLuft->setVisible(true);
    statusBar()->showMessage(tr("Draw helices.") );	
    glNewList(cubeGL->bas+6, GL_COMPILE );{        
      glPushMatrix();
      glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
      mol.drawSline(mol.vL,mol.nListe);
      glPopMatrix();
    }glEndList();
  }
  */
  for (int i=0; i<mx; i++)
    xdinp[i]=xd[i];
  smx=atmax=mx;
  mol.tubifiedAtoms=tubifiedAtoms_state;
  mol.bondColorStyle = singleColorBonds_state;
}

void MyWindow::updateRecentFileActions() {
  QSettings einstellung(QSettings::IniFormat,  QSettings::UserScope ,"Christian_B._Huebschle", "MoleCoolQt" );
  einstellung.beginGroup("Version 0.1");
  QStringList files = einstellung.value("recentFileList").toStringList();

  int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

  for (int i = 0; i < numRecentFiles; ++i) {
    QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
    recentFileActs[i]->setText(text);
    recentFileActs[i]->setData(files[i]);
    recentFileActs[i]->setStatusTip(files[i]);
    recentFileActs[i]->setVisible(true);
  }
  for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
    recentFileActs[j]->setVisible(false);


  einstellung.endGroup();
}

QString MyWindow::strippedName(const QString &fullFileName) {
  return QFileInfo(fullFileName).fileName();
} 

void MyWindow::openRecentFile() {
  QAction *action = qobject_cast<QAction *>(sender());
  removeDipoleMoments();
  if (action)
    loadFile(action->data().toString());
}

void MyWindow::zoomIn() {
  cubeGL->zoom(0.1);
}

void MyWindow::zoomOut() {

  cubeGL->zoom(-0.1);
}

void MyWindow::rotLeft() {
  cubeGL->rotY(0.1);
}

void MyWindow::rotRight() {
 double spd=speedSldr->value()/63.0;
  cubeGL->rotY(-0.1*spd);
}

void MyWindow::moveR(){
  cubeGL->moveX(1.1);
}

void MyWindow::moveL(){
  cubeGL->moveX(-1.1);
}

void MyWindow::moveU(){
  cubeGL->moveY(1.1);
}

void MyWindow::moveD(){
  cubeGL->moveY(-1.1);
}

void MyWindow::rotUp() {
  cubeGL->rotX(0.1);
}

void MyWindow::rotCw() {
  cubeGL->rotZ(0.1);
}

void MyWindow::rotCcw() {
  cubeGL->rotZ(-0.1);
}

void MyWindow::rotDown() {
  cubeGL->rotX(-0.1);
}

void MyWindow::toggleElli(bool b){
togElli->setChecked(!b);
}

void MyWindow::toggleNoElli(bool b){
noElli->setChecked(!b);
}

void MyWindow::toggleSingleColorBond(bool b){
  mol.bondColorStyle=b;
  cubeGL->updateGL();
}

void MyWindow::updateStatusBar(const QString& s) {
  statusBar()->showMessage(s);
}

void MyWindow::updateStatusBar() {
  QString label =   QString(tr("%1 @ %2").arg(CID).arg(dirName.mid(dirName.lastIndexOf('/')+1)));//
   sLabel->setText(label);   
   if ((cubeGL->afilename=="xd.res")||(cubeGL->afilename=="xd.mas")||(cubeGL->afilename=="xd.inp"))
     cubeGL->afilename=QString(CID);
   cubeGL->CID=CID;
}

void MyWindow::paintEvent(QPaintEvent *){
   if (cubeGL->stereo_mode==1) cubeGL->minus->setChecked((geometry().y()+cubeGL->y())%2);
}

 void MyWindow::moveEvent(QMoveEvent *) {
   if (cubeGL->stereo_mode==1) cubeGL->minus->setChecked((geometry().y()+cubeGL->y())%2);
}

void MyWindow::updateTime() {
   QDateTime Time = QDateTime::currentDateTime();
   QString text = Time.toString("dd.MM.yyyy hh:mm:ss");
   time->setDigitCount(text.size()+1);
   time->display(text);

}

void MyWindow::closeEvent(QCloseEvent *event)  {

    if (mol.einstellung->group()!="Version 0.1")mol.einstellung->beginGroup("Version 0.1");
    mol.einstellung->setValue("FFMPEG.EXE",mol.ffmpegexe);
    mol.einstellung->setValue("NoMessageBoxesPopup",noMessBox->isChecked());
   mol.einstellung->setValue("size", size() );
   mol.einstellung->setValue("position", pos() );
   mol.einstellung->setValue("ViewAngle",cubeGL->viewAngle());
   mol.einstellung->setValue("text_color_red",cubeGL->tCR);
   mol.einstellung->setValue("text_color_green",cubeGL->tCG);
   mol.einstellung->setValue("text_color_blue",cubeGL->tCB);
   mol.einstellung->setValue("text_color_alpha",cubeGL->tCA);
   mol.einstellung->setValue("background_color_red",cubeGL->bgCR);
   mol.einstellung->setValue("background_color_green",cubeGL->bgCG);
   mol.einstellung->setValue("background_color_blue",cubeGL->bgCB);
   mol.einstellung->setValue("background_color_alpha",cubeGL->bgCA);
   mol.einstellung->setValue("Matrix00",cubeGL-> MM[0]);
   mol.einstellung->setValue("Matrix01",cubeGL-> MM[1]);
   mol.einstellung->setValue("Matrix02",cubeGL-> MM[2]);
   mol.einstellung->setValue("Matrix03",cubeGL-> MM[3]);
   mol.einstellung->setValue("Matrix04",cubeGL-> MM[4]);
   mol.einstellung->setValue("Matrix05",cubeGL-> MM[5]);
   mol.einstellung->setValue("Matrix06",cubeGL-> MM[6]);
   mol.einstellung->setValue("Matrix07",cubeGL-> MM[7]);
   mol.einstellung->setValue("Matrix08",cubeGL-> MM[8]);
   mol.einstellung->setValue("Matrix09",cubeGL-> MM[9]);
   mol.einstellung->setValue("Matrix10",cubeGL->MM[10]);
   mol.einstellung->setValue("Matrix11",cubeGL->MM[11]);
   mol.einstellung->setValue("Matrix12",cubeGL->MM[12]);
   mol.einstellung->setValue("Matrix13",cubeGL->MM[13]);
   mol.einstellung->setValue("Matrix14",cubeGL->MM[14]);
   mol.einstellung->setValue("Matrix15",cubeGL->MM[15]);
   mol.einstellung->setValue("bondColor",mol.bondColor);
   mol.einstellung->setValue("bondStrength", mol.bondStrength);
   mol.einstellung->setValue("tubes",mol.tubifiedAtoms);
   mol.einstellung->setValue("singleColorBonds",mol.bondColorStyle);
   mol.einstellung->setValue("LabelFont",cubeGL->myFont);
   mol.einstellung->setValue("MolisoLegendFont",cubeGL->MLegendFont);
   mol.einstellung->beginWriteArray("AtomColors",107);
   for (int i = 0; i < 107; ++i) {
     mol.einstellung->setArrayIndex(i);
     mol.einstellung->setValue("red",   mol.Acol[i-1][0]);
     mol.einstellung->setValue("green", mol.Acol[i-1][1]);
     mol.einstellung->setValue("blue",  mol.Acol[i-1][2]);
     mol.einstellung->setValue("alpha", mol.Acol[i-1][3]);
   }
   mol.einstellung->endArray();
   mol.einstellung->beginWriteArray("AtomStyles",107);
   for (int i = 0; i < 107; ++i) {
     mol.einstellung->setArrayIndex(i);
     mol.einstellung->setValue("Style", mol.aStyle[i-1]);
   }
   mol.einstellung->endArray();
   mol.einstellung->beginWriteArray("CovaleceRadii",107);
   for (int i = 0; i < 107; ++i) {
     mol.einstellung->setArrayIndex(i);
     mol.einstellung->setValue("Radius", mol.Kovalenz_Radien[i-1]);
   }
   mol.einstellung->endArray();
   mol.einstellung->setValue("InvertMouseZoom",(cubeGL->invertMouseZoom->checkState()==Qt::Checked));
   mol.einstellung->setValue("BackgroundGradient",(back_Grad->isChecked()));//TOGGLE STATES
   mol.einstellung->setValue("UnitCell",(togUnit->isChecked()));//TOGGLE STATES
   mol.einstellung->endGroup();
   if ((cubeGL->moliso)&&(!cubeGL->moliso->faceFile.isEmpty())) {
      destroyMoliso();
   }
   event->accept();
}

void MyWindow::readXDPartAux(){
  QFile aux("xd_part.aux");  
  bool b;
  b= aux.open(QIODevice::ReadOnly);
  if (b){
  QString auxTxt = aux.readAll(); 
  aux.close();
  QStringList lines = auxTxt.split("\n",QString::SkipEmptyParts);
  if ((lines.size()!=atmax+1)&&(lines.size()!=atmax)) return;
  for (int j=0; j < atmax; j++){ 
    if (asymmUnit[j].OrdZahl<0) continue;   
    QStringList token = lines.at(j).split(" ",QString::SkipEmptyParts);
    asymmUnit[j].part=token.at(2).toInt();
    if ((token.size()>7)&&(token.at(3)=="resiNr")){
      if (token.at(4).contains(QRegExp("[^0-9]+") )) qDebug() <<token.at(4) << "must be numeric!"<<lines.at(j);
    asymmUnit[j].resiNr=token.at(4).toInt();

    strncpy(asymmUnit[j].ami3,token.at(5).toStdString().c_str(),4);
    strncpy(asymmUnit[j].shortname,token.at(7).toStdString().c_str(),4);
    }
    else if (token.size()>3) qDebug()<< "Read error in line "<<j<<lines.at(j);
  }
  }
}

void MyWindow::syncBondActs(){
  QAction *action = qobject_cast<QAction *>(sender());
   if (action==noBondAct) togBond  ->setChecked(!noBondAct->isChecked());
   if (action==togBond) noBondAct->setChecked(!togBond->isChecked());
}

void MyWindow::syncAtomActs(){
  QAction *action = qobject_cast<QAction *>(sender());
   if (action==noAtomAct) togAtom  ->setChecked(!noAtomAct->isChecked());
   if (action==togAtom) noAtomAct->setChecked(!togAtom->isChecked());
}

void MyWindow::toggleTubes(bool b){
  if (b) noElli->setChecked(b);
  mol.tubifiedAtoms=b;
  cubeGL->updateGL();
}

void MyWindow::makeXDPartAux(){
  //qDebug()<<"MyWindow::makeXDPartAux";
  if (QFile::exists("xd_part.aux")){
    readXDPartAux();
    return;
  }
  if ((!asymmUnit.isEmpty())&&(atmax)){
    const double TOLERANZ = 0.0005;
    QStringList befehle;
    befehle  <<"ACTA" << "AFIX" << "MPLA" << "ANIS" << "BASF" << "BIND" << "BLOC" << "BOND" 
      << "BUMP" << "CELL" << "CGLS" << "CHIV" << "CONF" << "CONN" << "DAMP" << "DANG" 
      << "DEFS" << "DELU" << "DFIX" << "DISP" << "EADP" << "EGEN" << "END" <<  "EQIV" 
      << "ESEL" << "EXTI" << "EXYZ" << "FEND" << "FLAT" << "FMAP" << "FRAG" << "FREE" 
      << "FVAR" << "GRID" << "HFIX" << "HKLF" << "HOPE" << "HTAB" << "INIT" << "ISOR" 
      << "LAST" << "LATT" << "LAUE" << "LIST" << "L.S." << "MERG" << "MOLE" << "MORE" 
      << "MOVE" << "NCSY" << "OMIT" << "PART" << "PATT" << "PHAN" << "PHAS" << "PLAN" 
      << "PSEE" << "REM" <<  "RESI" << "RTAB" << "SADI" << "SAME" << "SFAC" << "SHEL" 
      << "SIMU" << "SIZE" << "SPEC" << "SPIN" << "STIR" << "SUMP" << "SWAT" << "SYMM" 
      << "TEMP" << "TEXP" << "TIME" << "TITL" << "TREF" << "TWIN" << "UNIT" << "VECT" 
      << "WPDB" << "WGHT" << "ZERR"<<
      "XNPD"<< "REST"<< "CHAN"<< "RIGU"<< "FLAP"<< "RNUM"<< "SOCC"<< "PRIG"<< "WIGL"<< "RANG"<< "TANG"<< "ADDA"<< "STAG"<<
      "ATOM"<< "HETA"<< "SCAL"<< "ABIN"<< "ANSC"<< "ANSR"<< "NOTR"<< "NEUT"<< "TWST";

    QDir work=QDir(QDir::current());  
    QStringList filter;
    filter << "*res"<<"*ins";      
    QStringList shelxf;
    QString line;
    QFile shx;
    int ifi=0;
	for (int j=0; j<atmax; j++){ 
          asymmUnit[j].part=0;
	  strcpy(asymmUnit[j].shortname,"OOPS");
	  strcpy(asymmUnit[j].ami3,"NO");
	  asymmUnit[j].resiNr=666;
          break;
       }
    shelxf=work.entryList(filter,QDir::Files,QDir::Time); 
    do {
      if (!(ifi<shelxf.size())) return;
      shx.setFileName(shelxf.at(ifi++));
      if (shx.open(QIODevice::ReadOnly)) {line=QString(shx.readLine(80));shx.close();}
    }while (line.contains("!!"));
    shx.open(QIODevice::ReadOnly);
    QString shelxTxt = shx.readAll(); 
    shx.close();
    QStringList lines = shelxTxt.split("\n");
    int myPart=0,myResi=0;
    QString ami3,shortname;
    V3 point;
    for (int i=0; i<lines.size();i++){
      QStringList token = lines.at(i).split(" ",QString::SkipEmptyParts);
      if ((token.size()>1)&&( token.at(0).contains("PART",Qt::CaseInsensitive ))) {
	myPart=token.at(1).toInt(); 
      }
      if ((token.size()>1)&&( token.at(0).contains("RESI",Qt::CaseInsensitive ))){
	if ((token.size()>1)&&(token.at(1).toInt()>0)) {
	  myResi=token.at(1).toInt();
	  if (token.size()>2) ami3=token.at(2);
	}else if ((token.size()>2)&&(token.at(2).toInt()>0)) {
	myResi=token.at(2).toInt();
	ami3=token.at(1);
	}
      }
      if ((token.size()>5)&&(!befehle.contains(token.at(0)))){
	shortname = token.at(0);
	point.x=token.at(2).toDouble();
	point.y=token.at(3).toDouble();
	point.z=token.at(4).toDouble();
	for (int j=0; j<atmax; j++){ 
       if (Distance(point,asymmUnit[j].frac)<TOLERANZ) {
          asymmUnit[j].part=myPart;
	  strcpy(asymmUnit[j].shortname,shortname.toStdString().c_str());
	  strcpy(asymmUnit[j].ami3,ami3.toStdString().c_str());
	  asymmUnit[j].resiNr=myResi;
          break;
       }
	}
 }
      if (lines.at(i).contains("=")) i++;
    }
    QFile aux("xd_part.aux");
    QStringList newlines;
    if (aux.open(QIODevice::WriteOnly)){
      for (int i=0; i<atmax; i++){
	if (asymmUnit[i].resiNr!=0)
	newlines.append(QString("%1 is_part %2 resiNr %3 %4 short_name %5")
			.arg(asymmUnit[i].atomname)
			.arg(asymmUnit[i].part)
			.arg(asymmUnit[i].resiNr)
			.arg(asymmUnit[i].ami3)
			.arg(asymmUnit[i].shortname)
			);
	else
	newlines.append(QString("%1 is_part %2").arg(asymmUnit[i].atomname).arg(asymmUnit[i].part));
      }
      aux.write(newlines.join("\n").toLatin1(),newlines.join("\n").toLatin1().size());
      aux.close();
    }
  }
}

void MyWindow::expandAround(){
  if (cubeGL->expandatom>-1) {
    mol.bonds_made=0;
    mol.knopf_made=0;
    if (xdinp.isEmpty()) mgrowSymm(5,cubeGL->expandatom);
    else growSymm(5,cubeGL->expandatom);
    cubeGL->updateGL();
  }
}

void MyWindow::exportVisible(){
//  qDebug()<<"here IT comes soon!"<<xdinp.size();
  QString efn=dirName+"_vis_export.txt";
  FILE *ef=fopen(efn.toLocal8Bit(),"wt");
  for (int j=0;j<xdinp.size();j++){
    fprintf(ef,"%-6s %3d %10.5f %10.5f %10.5f (3,%1d) %10.5f %g\n",
      xdinp.at(j).atomname,
      xdinp.at(j).OrdZahl,
      xdinp.at(j).kart.x,
      xdinp.at(j).kart.y,      
      xdinp.at(j).kart.z,
      xdinp.at(j).cptype-3,
      xdinp.at(j).peakHeight,
      xdinp.at(j).lam1+xdinp.at(j).lam2+xdinp.at(j).lam3);
  }
  fclose(ef);
}

inline int trindex(int i,int j){
  int I=(i>j)?i:j;
  int J=(i<j)?i:j;
  I++;J++;
  return (I*(I+1)/2-I+J-1);
}

void MyWindow::plotSDM(int steps){
  QList<SdmItem> sdm;
  double t=0.0,stepwidth=1.0/(steps);//,oo=0;
  if (masymmUnit.isEmpty()) {
    return;
  }
  SdmItem sdmItem;
  sdmItem.a1=0;
  sdmItem.a2=1;
  sdmItem.sn=0;
  sdmItem.d=666;
  QList<Modulat> mato;
  V3 p,g0;
  printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
  for (int h=-1; h< 2; h++){
    for (int k=-1; k< 2; k++){
      for (int l=-1; l< 2; l++){
        for (int s=0; s<mol.zelle.symmops.size(); s++){
          for (int i=0; i<masymmUnit.size(); i++) {
            Modulat *newAt = NULL;
            if (masymmUnit[i].iamcomp>1)
              newAt = new Modulat(masymmUnit[i].applySymm(
                    mol.ccc[masymmUnit[i].iamcomp-2].nuCell.symmops.at(s),
                    mol.ccc[masymmUnit[i].iamcomp-2].nuCell.trans.at(s)+V3(h,k,l),
                    mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4sym.at(s),
                    mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4.at(s),
                    mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4tr.at(s)));
            else
              newAt = new Modulat(masymmUnit[i].applySymm(
                    mol.zelle.symmops.at(s),
                    mol.zelle.trans.at(s)+V3(h,k,l), 
                    mol.zelle.x4sym.at(s), 
                    mol.zelle.x4.at(s),
                    mol.zelle.x4tr.at(s)));
            bool gibscho=false;
            for(int gbt=0;gbt<mato.size();gbt++){
              if (mato.at(gbt).OrdZahl<0) continue;
              g0=mato[gbt].frac(t);
              p=newAt->frac(t);
              if ((mato[gbt].OrdZahl>-1)&&(fl(p.x-g0.x,p.y-g0.y,p.z-g0.z)<0.2)) gibscho=true; 
            }
            if (!gibscho) { 
              mato.append(*newAt);
              //V3 r=newAt->frac(cubeGL->tvalue);
              //printf("%s %f %f %f  \n",newAt->atomname,r.x,r.y,r.z);
            }
          }
        }
      }
    }
  }
  printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

  for (int i=0; i<steps; i++){
  sdm.clear();
  for (int i=0; i<mato.size(); i++){
    double min=10000.0,dddd;
    for (int j=0; j<=i; j++ ){
      double dk = sqrt(Distance(mato[i].kart(t) , mato[j].kart(t))) ;

      if ((dk>0.01)&&(min>dk)){  
        min=dk;
        sdmItem.d=min;
        sdmItem.a1=i;
        sdmItem.a2=j;
      }
      if ((mato[sdmItem.a1].OrdZahl>-1)&&(mato[sdmItem.a2].OrdZahl>-1)) 
        dddd=(mol.Kovalenz_Radien[mato[sdmItem.a1].OrdZahl]+ mol.Kovalenz_Radien[mato[sdmItem.a2].OrdZahl])*0.012;
      else dddd=0;
      if ((dddd>sdmItem.d)&(mato[sdmItem.a1].OrdZahl==0)&&(mato[sdmItem.a2].OrdZahl==0)) {
        dddd=0;
      }

      if (sdmItem.d<dddd){
        sdmItem.covalent=true;
        if  (sdmItem.sn) printf("%s==%s %g (%g) %d\n",
            mato[sdmItem.a1].atomname, 
            mato[sdmItem.a2].atomname ,
            sdmItem.d,dddd,sdmItem.sn );// */
        sdm.append(sdmItem);
      }else sdmItem.covalent=false;

    }
  }
  qSort(sdm.begin(),sdm.end());
    printf("%12.6f ",t);
    int end=9;
    for (int j=0; (j<sdm.size()) && (j<end);j++) if (sdm.at(j).covalent){ 
     // oo= masymmUnit[sdm.at(j).a1].occupancy(t)* masymmUnit[sdm.at(j).a2].occupancy(t);
      printf("(%4s=%-4s%9.6f[%d_%g%g%g])", mato.at(sdm.at(j).a1).atomname, mato.at(sdm.at(j).a2).atomname, // 
          sdm.at(j).d,
          sdm.at(j).sn,
         -sdm.at(j).floorD.x+5,
         -sdm.at(j).floorD.y+5,
         -sdm.at(j).floorD.z+5

          //oo
          );
    }
    else end++;
    printf("\n");
    t+=stepwidth;

  }
}
void MyWindow::lisisfat(){
    double v;
    bool ok=true;
    v= QInputDialog::getDouble ( this, "Lissajous_line_width", "Lissajous_line_width", mol.Lissajous_line_width, 0.1, 10.0, 1, &ok );
    if (ok) mol.Lissajous_line_width=v;
    cubeGL->updateGL();
}

void MyWindow::mSDM(QStringList &brauchSymm,int packart){
  // George Sheldrick Seminar ideas
  if (masymmUnit.isEmpty()) {
    return;
  }
  printf("starte mSDM %d %d \n",mol.zelle.symmops.size(),masymmUnit.size());
  QTime speedTest;
  speedTest.start();
  double dk,dddd;
  V3 prime,dp,D,floorD;
  QList<SdmItem> sdm;
  QString bs;
  SdmItem sdmItem;
  sdmItem.a1=0;
  sdmItem.a2=1;
  sdmItem.sn=0;

  for (int i=0; i<masymmUnit.size(); i++){
    masymmUnit[i].imul=0;
    for (int j=0; j<=i; j++ ){
      double min=100000;
      for (int n=0;n<mol.zelle.symmops.size();  n++){
	prime=mol.zelle.symmops.at(n) * masymmUnit[i].frac0 + mol.zelle.trans.at(n);
	D=prime - masymmUnit[j].frac0 + V3(0.5,0.5,0.5) ;
	floorD=V3(floor(D.x),floor(D.y),floor(D.z));	 
	dp=D - floorD - V3(0.5,0.5,0.5);
	dk=fl(dp.x,dp.y,dp.z);
	if ((dk>0.01)&&(min>dk)){  
	  min=dk;
	  sdmItem.d=min;
	  sdmItem.floorD=floorD;
	  sdmItem.a1=i;
	  sdmItem.a2=j;
	  sdmItem.sn=n;
	}
	else if ((masymmUnit.at(i).OrdZahl>=0)&&(dk<=0.1)){
	  masymmUnit[i].imul++;
	}
      }
      if ((packart==1)&&(masymmUnit[sdmItem.a1].OrdZahl>-1)&&(masymmUnit[sdmItem.a2].OrdZahl>-1)
      //&&(masymmUnit[sdmItem.a1].part>-1)&&(masymmUnit[sdmItem.a2].part>-1)
      ) dddd=mol.gd;
      else if ((masymmUnit[sdmItem.a1].OrdZahl>-1)&&(masymmUnit[sdmItem.a2].OrdZahl>-1)
     // &&(masymmUnit[sdmItem.a1].part>-1)&&(masymmUnit[sdmItem.a2].part>-1)
      )
	dddd=(mol.Kovalenz_Radien[masymmUnit[sdmItem.a1].OrdZahl]+ mol.Kovalenz_Radien[masymmUnit[sdmItem.a2].OrdZahl])*0.012;
      else dddd=0;
//      if ((masymmUnit[sdmItem.a1].part!=0)&&(masymmUnit[sdmItem.a2].part!=0)&&(masymmUnit[sdmItem.a1].part!=masymmUnit[sdmItem.a2].part)) dddd=0;
      if ((dddd>sdmItem.d)&(masymmUnit[sdmItem.a1].OrdZahl==0)&&(masymmUnit[sdmItem.a2].OrdZahl==0)) {
/*	printf("%s==%s %d %d %g (%g)\n",
			asymmUnit[sdmItem.a1].atomname, 
			asymmUnit[sdmItem.a2].atomname ,
			asymmUnit[sdmItem.a1].part, 
			asymmUnit[sdmItem.a2].part,sdmItem.d,dddd);// */
	dddd=0;
      }

      if (sdmItem.d<dddd){
	sdmItem.covalent=true;
	if  (sdmItem.sn) printf("%s==%s %g (%g) %d\n",
			masymmUnit[sdmItem.a1].atomname, 
			masymmUnit[sdmItem.a2].atomname ,
			sdmItem.d,dddd,sdmItem.sn );// */
      }else sdmItem.covalent=false;

      sdm.append(sdmItem);
    } 
  }
//  qDebug()<<packart<<masymmUnit[0].atomname;
 if (packart!=1){ 
    int someleft=0,nextmol=0,maxmol=1;
    for (int i=0; i<masymmUnit.size();i++) masymmUnit[i].molindex=-1;
    masymmUnit[0].molindex=1;
    do {
      nextmol=0;
      do { 
	someleft=0;
	for  (int i=0; i<sdm.size();i++){
//          if (sdm.at(i).covalent) printf("%s=%s %d %d %d \n",masymmUnit[sdm.at(i).a1].atomname,masymmUnit[sdm.at(i).a2].atomname,masymmUnit[sdm.at(i).a1].molindex,masymmUnit[sdm.at(i).a2].molindex,masymmUnit[sdm.at(i).a1].molindex*masymmUnit[sdm.at(i).a2].molindex);
	  if ((sdm.at(i).covalent)&&(masymmUnit[sdm.at(i).a1].molindex*masymmUnit[sdm.at(i).a2].molindex<0)) {
	    masymmUnit[sdm.at(i).a1].molindex=maxmol;
	    masymmUnit[sdm.at(i).a2].molindex=maxmol;
/*            printf("%s==%s %g %d %d\n",
			masymmUnit[sdmItem.a1].atomname, 
			masymmUnit[sdmItem.a2].atomname ,
			sdmItem.d,maxmol,someleft);// */
	    someleft++;
	   }     
	} 
      }while (someleft);
      for ( int i=0; i<masymmUnit.size();i++) {
	if ((masymmUnit[i].OrdZahl>-1)&&(masymmUnit[i].molindex<0)) {nextmol=i;break;}
      }
      if (nextmol) masymmUnit[nextmol].molindex=(++maxmol);
    }while (nextmol);
    printf ("The asymmetric unit contains %d fragments. SDM is %d\n",maxmol,sdm.size());
    cubeGL->maxmol=maxmol;
  }
  for (int k =0; k<sdm.size();k++){
     if ((packart==1)||sdm.at(k).covalent){
       if (masymmUnit[sdm.at(k).a1].molindex<1) continue;
       for (int n=0;n<mol.zelle.symmops.size();  n++){
	prime=mol.zelle.symmops.at(n) * masymmUnit[sdm.at(k).a1].frac0 + mol.zelle.trans.at(n);
	D=prime - masymmUnit[sdm.at(k).a2].frac0+ V3(0.5,0.5,0.5) ;
	floorD=V3(floor(D.x),floor(D.y),floor(D.z));	 
	dp=D - floorD - V3(0.5,0.5,0.5);
	if ((n==0)&&(V3(0,0,0)==floorD)) continue;
	dk=fl(dp.x,dp.y,dp.z);
	dddd=(sdm.at(k).d+0.02);
	if ((dk>0.01)&&(dddd>=dk)) {
	  //printf("n=%d dk%g %g %s %s \n",n,dk,dddd, masymmUnit[sdm.at(k).a1].atomname,masymmUnit[sdm.at(k).a2].atomname);
	  bs=QString("%1_%2 %3 %4:%5,").arg(n+1).arg(5-(int)floorD.x).arg(5-(int)floorD.y).arg(5-(int)floorD.z).arg(masymmUnit[sdm.at(k).a1].molindex);
          //qDebug()<<bs;
	  if  (!brauchSymm.contains(bs)) {
	    brauchSymm.append(bs);
 	   }
 	 }
      } 
    }
  }

  qSort(sdm.begin(),sdm.end());
/* ??
  QList<int> flags;
  for (int i=0; i<masymmUnit.size(); i++) flags.append((masymmUnit.at(i).OrdZahl==-3)?-1:1);
  for (int i=0; i<sdm.size(); i++)
    if ((flags.at(sdm.at(i).a1)*flags.at(sdm.at(i).a2))==-1) {
      if ((flags.at(sdm.at(i).a1)==-1)&&((asymmUnit.at(sdm.at(i).a1).OrdZahl>-1)||(asymmUnit.at(sdm.at(i).a2).OrdZahl>-1))){
	if ((sdm.at(i).sn==0)&&(sdm.at(i).floorD==V3(0,0,0))) {flags[sdm.at(i).a1]=1;continue;}
	masymmUnit[sdm.at(i).a1].frac = mol.zelle.symmops.at(sdm.at(i).sn) * masymmUnit[sdm.at(i).a1].frac + mol.zelle.trans.at(sdm.at(i).sn) - sdm.at(i).floorD;
	flags[sdm.at(i).a1]=1;
      }
    }
    */
  while (brauchSymm.contains("1_5 5 5:")){
    brauchSymm.removeAt(brauchSymm.indexOf(QRegExp("1_5 5 5:\\d+")));
  }
  printf("mSDM time used: %d milliseconds.\n",speedTest.restart());
}

void MyWindow::SDM(QStringList &brauchSymm,int packart){
  // George Sheldrick Seminar ideas
  if (asymmUnit.isEmpty()) {
    return;
  }
  printf("starte SDM %d %d \n",mol.zelle.symmops.size(),asymmUnit.size());
  QTime speedTest;
  speedTest.start();
  double dk,dddd;
  V3 prime,dp,D,floorD;
  QList<SdmItem> sdm;
  QString bs;
  SdmItem sdmItem;
  sdmItem.a1=0;
  sdmItem.a2=1;
  sdmItem.sn=0;

  for (int i=0; i<asymmUnit.size(); i++){
    asymmUnit[i].imul=0;
    for (int j=0; j<=i; j++ ){
      double min=100000;
      for (int n=0;n<mol.zelle.symmops.size();  n++){
	prime=mol.zelle.symmops.at(n) * asymmUnit[i].frac + mol.zelle.trans.at(n);
	D=prime - asymmUnit[j].frac+ V3(0.5,0.5,0.5) ;
	floorD=V3(floor(D.x),floor(D.y),floor(D.z));	 
	dp=D - floorD - V3(0.5,0.5,0.5);
	dk=fl(dp.x,dp.y,dp.z);
	if ((dk>0.01)&&(min>dk)){  
	  min=dk;
	  sdmItem.d=min;
	  sdmItem.floorD=floorD;
	  sdmItem.a1=i;
	  sdmItem.a2=j;
	  sdmItem.sn=n;
	}
	else if ((asymmUnit.at(i).OrdZahl>=0)&&(dk<=0.1)){
	  asymmUnit[i].imul++;
	}
      }
      if ((packart==1)&&(asymmUnit[sdmItem.a1].OrdZahl>-1)&&(asymmUnit[sdmItem.a2].OrdZahl>-1)
      &&(asymmUnit[sdmItem.a1].part>-1)&&(asymmUnit[sdmItem.a2].part>-1)) dddd=mol.gd;
      else if ((asymmUnit[sdmItem.a1].OrdZahl>-1)&&(asymmUnit[sdmItem.a2].OrdZahl>-1)
      &&(asymmUnit[sdmItem.a1].part>-1)&&(asymmUnit[sdmItem.a2].part>-1))
	dddd=(mol.Kovalenz_Radien[asymmUnit[sdmItem.a1].OrdZahl]+ mol.Kovalenz_Radien[asymmUnit[sdmItem.a2].OrdZahl])*0.012;
      else dddd=0;
      if ((asymmUnit[sdmItem.a1].part!=0)&&(asymmUnit[sdmItem.a2].part!=0)&&(asymmUnit[sdmItem.a1].part!=asymmUnit[sdmItem.a2].part))
	dddd=0;
      if ((dddd>sdmItem.d)&(asymmUnit[sdmItem.a1].OrdZahl==0)&&(asymmUnit[sdmItem.a2].OrdZahl==0)) {
/*	printf("%s==%s %d %d %g (%g)\n",
			asymmUnit[sdmItem.a1].atomname, 
			asymmUnit[sdmItem.a2].atomname ,
			asymmUnit[sdmItem.a1].part, 
			asymmUnit[sdmItem.a2].part,sdmItem.d,dddd);// */
	dddd=0;
      }

      if (sdmItem.d<dddd){
	sdmItem.covalent=true;
/*	if  (sdmItem.sn) printf("%s==%s %d %d %g (%g) \n",
			asymmUnit[sdmItem.a1].atomname, 
			asymmUnit[sdmItem.a2].atomname ,
			asymmUnit[sdmItem.a1].part, 
			asymmUnit[sdmItem.a2].part,sdmItem.d,dddd);// */
      }else sdmItem.covalent=false;
      sdm.append(sdmItem);
    } 
  }

 if (packart!=1){ 
    int someleft=0,nextmol=0,maxmol=1;
    for (int i=0; i<asymmUnit.size();i++) asymmUnit[i].molindex=-1;
    asymmUnit[0].molindex=1;
    do {
      nextmol=0;
      do { 
	someleft=0;
	for  (int i=0; i<sdm.size();i++){
	  if ((sdm.at(i).covalent)&&(asymmUnit[sdm.at(i).a1].molindex*asymmUnit[sdm.at(i).a2].molindex<0)) {
	    asymmUnit[sdm.at(i).a1].molindex=maxmol;
	    asymmUnit[sdm.at(i).a2].molindex=maxmol;
	    someleft++;
	   }     
	} 
      }while (someleft);
      for ( int i=0; i<asymmUnit.size();i++) {
	if ((asymmUnit[i].OrdZahl>-1)&&(asymmUnit[i].molindex<0)) {nextmol=i;break;}
      }
      if (nextmol) asymmUnit[nextmol].molindex=(++maxmol);
    }while (nextmol);
    printf ("The asymmetric unit contains %d fragments. SDM is %d\n",maxmol,sdm.size());
    cubeGL->maxmol=maxmol;
  }
  for (int k =0; k<sdm.size();k++){
     if ((packart==1)||sdm.at(k).covalent){
       if (asymmUnit[sdm.at(k).a1].molindex<1) continue;
       for (int n=0;n<mol.zelle.symmops.size();  n++){
	prime=mol.zelle.symmops.at(n) * asymmUnit[sdm.at(k).a1].frac + mol.zelle.trans.at(n);
	D=prime - asymmUnit[sdm.at(k).a2].frac+ V3(0.5,0.5,0.5) ;
	floorD=V3(floor(D.x),floor(D.y),floor(D.z));	 
	dp=D - floorD - V3(0.5,0.5,0.5);
	if ((n==0)&&(V3(0,0,0)==floorD)) continue;
	dk=fl(dp.x,dp.y,dp.z);
	dddd=(sdm.at(k).d+0.02);
	if ((dk>0.01)&&(dddd>=dk)) {
//	  printf("n=%d dk%g %g %s %s \n",n,dk,dddd, asymmUnit[sdm.at(k).a1].atomname,asymmUnit[sdm.at(k).a2].atomname);
	  bs=QString("%1_%2%3%4:%5,").arg(n+1).arg(5-(int)floorD.x).arg(5-(int)floorD.y).arg(5-(int)floorD.z).arg(asymmUnit[sdm.at(k).a1].molindex);
	  if  (!brauchSymm.contains(bs)) {
	    brauchSymm.append(bs);
 	   }
 	 }
      } 
    }
  }

  qSort(sdm.begin(),sdm.end());
  for (int i=0; i<sdm.size(); i++){
   if ((asymmUnit[sdm.at(i).a1].OrdZahl==-2)&&(asymmUnit[sdm.at(i).a2].OrdZahl>-1)&&(sdm.at(i).d<1.5)){//
//     &&(asymmUnit[sdm.at(i).a1].cptype==0)&&(asymmUnit[sdm.at(i).a1].peakHeight<2.0)){
     bool done=false;
     for (int j=0; j<sdm.size(); j++){
       if ((!done)&&(sdm.at(i).a1==sdm.at(j).a1)&&(sdm.at(j).a2!=sdm.at(i).a2)&&(asymmUnit[sdm.at(j).a2].OrdZahl>-1)&&(sdm.at(j).d<2.5)){
         printf("%6s=%-6s %-6s (3,%d) %9.4f %9.4f %9.4f %6.2f %4.2f \n",
             asymmUnit[sdm.at(i).a2].atomname,
             asymmUnit[sdm.at(j).a2].atomname,
             asymmUnit[sdm.at(i).a1].atomname,
             asymmUnit[sdm.at(i).a1].cptype-3,
             sdm.at(i).d,
             sdm.at(j).d,
             asymmUnit[sdm.at(i).a1].peakHeight,
             asymmUnit[sdm.at(i).a1].lam1+
             asymmUnit[sdm.at(i).a1].lam2+
             asymmUnit[sdm.at(i).a1].lam3,
             asymmUnit[sdm.at(i).a1].lam1/asymmUnit[sdm.at(i).a1].lam2-1.0
             ); 
         done=true;
       }
     }
   }
  }
  QList<int> flags;
  for (int i=0; i<asymmUnit.size(); i++) flags.append((asymmUnit.at(i).OrdZahl==-3)?-1:1);
  for (int i=0; i<sdm.size(); i++)
    if ((flags.at(sdm.at(i).a1)*flags.at(sdm.at(i).a2))==-1) {
      if ((flags.at(sdm.at(i).a1)==-1)&&((asymmUnit.at(sdm.at(i).a1).OrdZahl>-1)||(asymmUnit.at(sdm.at(i).a2).OrdZahl>-1))){
	if ((sdm.at(i).sn==0)&&(sdm.at(i).floorD==V3(0,0,0))) {flags[sdm.at(i).a1]=1;continue;}
	asymmUnit[sdm.at(i).a1].frac = mol.zelle.symmops.at(sdm.at(i).sn) * asymmUnit[sdm.at(i).a1].frac + mol.zelle.trans.at(sdm.at(i).sn) - sdm.at(i).floorD;
	flags[sdm.at(i).a1]=1;
      }
    }
  while (brauchSymm.contains("1_555:")){
    brauchSymm.removeAt(brauchSymm.indexOf(QRegExp("1_555:\\d+")));
  }
  printf("SDM time used: %d milliseconds.\n",speedTest.restart());
}

void MyWindow::makeVoro(){
  int ind=-1;
  QAction *action = qobject_cast<QAction *>(sender());
  if (action!=NULL){
      ind=action->data().toInt();
      ind=qMax(ind,-1);
      ind=qMin(ind,xdinp.size()-1);
  }
  mol.voronoij(asymmUnit,ind); 
  infoKanalNews(mol.voroMsg); 
  cubeGL->updateGL();
}

void MyWindow::mgrowSymm(int packart,int packatom){
  V3 expander=V3(0,0,0);
  QStringList brauchSymm;
  QString  bs;
  mol.bonds_made=0;
  mol.knopf_made=0;
  if (mol.zelle.commensurate){
  xdinp.clear();
  matoms.clear();
    V3 p,g0;
    for (int h=-2; h< mol.zelle.commen.x; h++){
      for (int k=-2; k< mol.zelle.commen.y; k++){
        for (int l=-2; l< mol.zelle.commen.z; l++){
          for (int s=0; s<mol.zelle.symmops.size(); s++){
            for (int i=0; i<masymmUnit.size(); i++) {
              Modulat *newAt = NULL;
              if (masymmUnit[i].iamcomp>1)
                newAt = new Modulat(masymmUnit[i].applySymm(
                      mol.ccc[masymmUnit[i].iamcomp-2].nuCell.symmops.at(s),
                      mol.ccc[masymmUnit[i].iamcomp-2].nuCell.trans.at(s)+V3(h,k,l),
                      mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4sym.at(s),
                      mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4.at(s),
                      mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4tr.at(s)));
              else
                newAt = new Modulat(masymmUnit[i].applySymm(
                      mol.zelle.symmops.at(s),
                      mol.zelle.trans.at(s)+V3(h,k,l), 
                      mol.zelle.x4sym.at(s), 
                      mol.zelle.x4.at(s),
                      mol.zelle.x4tr.at(s)));
              bool gibscho=false;
              for(int gbt=0;gbt<matoms.size();gbt++){
                if (matoms.at(gbt).OrdZahl<0) continue;
                g0=matoms[gbt].frac(cubeGL->tvalue);
                p=newAt->frac(cubeGL->tvalue);
                if ((matoms[gbt].OrdZahl>-1)&&(fl(p.x-g0.x,p.y-g0.y,p.z-g0.z)<0.2)) gibscho=true; 
              }
              if (!gibscho) { 
                matoms.append(*newAt);
                //V3 r=newAt->frac(cubeGL->tvalue);
                //printf("%s %f %f %f  \n",newAt->atomname,r.x,r.y,r.z);
              }
            }
          }
        }
      }
    }
    printf("Commensurate zell has %d atoms\n",matoms.size());
    packart=99; 
  }
  else if (!mol.ccc.isEmpty()){
    V3 p,g0;
  xdinp.clear();
  matoms.clear();
    for (int h=-1; h< 2; h++){
      for (int k=-1; k< 2; k++){
        for (int l=-1; l< 2; l++){
          for (int s=0; s<mol.zelle.symmops.size(); s++){
            for (int i=0; i<masymmUnit.size(); i++) {
              Modulat *newAt = NULL;
              if (masymmUnit[i].iamcomp>1)
                newAt = new Modulat(masymmUnit[i].applySymm(
                      mol.ccc[masymmUnit[i].iamcomp-2].nuCell.symmops.at(s),
                      mol.ccc[masymmUnit[i].iamcomp-2].nuCell.trans.at(s)+V3(h,k,l),
                      mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4sym.at(s),
                      mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4.at(s),
                      mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4tr.at(s)));
              else
                newAt = new Modulat(masymmUnit[i].applySymm(
                      mol.zelle.symmops.at(s),
                      mol.zelle.trans.at(s)+V3(h,k,l), 
                      mol.zelle.x4sym.at(s), 
                      mol.zelle.x4.at(s),
                      mol.zelle.x4tr.at(s)));
              bool gibscho=false;
              for(int gbt=0;gbt<matoms.size();gbt++){
                if (matoms.at(gbt).OrdZahl<0) continue;
                g0=matoms[gbt].frac(cubeGL->tvalue);
                p=newAt->frac(cubeGL->tvalue);
                if ((matoms[gbt].OrdZahl>-1)&&(fl(p.x-g0.x,p.y-g0.y,p.z-g0.z)<0.2)) gibscho=true; 
              }
              if (!gibscho) { 
                matoms.append(*newAt);
                //V3 r=newAt->frac(cubeGL->tvalue);
                //printf("%s %f %f %f  \n",newAt->atomname,r.x,r.y,r.z);
              }
            }
          }
        }
      }
    }

    packart=9;
  }
//  printf("===>>>%d %d\n",packart,packatom);
  if ((packart==5) && (packatom>-1) && (packatom<matoms.size())) {
    expander=matoms[packatom].frac(cubeGL->tvalue);
//    qDebug()<<  matoms[packatom].atomname<< expander.x<< expander.y<< expander.z;
  }
  QString blob=infoKanal->toPlainText();
  // qDebug()<<"blob"<<__LINE__;
  if ((packart==5)&&(blob.contains("'"))){
    QStringList blub=blob.split("'",QString::SkipEmptyParts);
    for (int i=0; i<blub.size(); i++) if ((i%2)==1) brauchSymm.append(QString("%1").arg(blub.at(i)));
  }
  if (packart<9){
  xdinp.clear();
  matoms.clear();
  }
  // qDebug()<<"blob"<<__LINE__;
  if ((packart==1)||(packart==6)){
    if (brauchSymm.isEmpty()) {mSDM(brauchSymm,packart);packart=6;}
    if (brauchSymm.isEmpty()) {packart=0;}
  }
  if (packart==0){
    infoKanal->clear();
    matoms.clear();
    matoms=masymmUnit;
  }
  if (packart==3){//packe und schneide die Zelle
    V3 prime,floorD;
    double dawars=1000.0,dl;
    for (int n=0; n<mol.zelle.symmops.size();n++){
      for (int i=0; i<masymmUnit.size();i++){
        if ((masymmUnit.at(i).OrdZahl>-1)&&(masymmUnit.at(i).molindex>0)){
          if (masymmUnit[i].iamcomp>1) 
            prime=mol.ccc[masymmUnit[i].iamcomp-2].nuCell.symmops.at(n) 
              * masymmUnit[i].frac(cubeGL->tvalue) 
              + mol.ccc[masymmUnit[i].iamcomp-2].nuCell.trans.at(n);
          else            
            prime=mol.zelle.symmops.at(n) * masymmUnit[i].frac(cubeGL->tvalue) + mol.zelle.trans.at(n) ;
          floorD=V3(floor(prime.x),floor(prime.y),floor(prime.z));
          prime=prime -floorD;	  
          //printf("%f %f %f __ %f %f %f \n",prime.x,prime.y,prime.z,floorD.x,floorD.y,floorD.z);
          dawars=1000.0;
          if (!matoms.isEmpty())for (int g=0; g<matoms.size();g++){
            if ((masymmUnit[i].OrdZahl*matoms[g].OrdZahl<0)) continue;
            V3 f=matoms[g].frac(cubeGL->tvalue);
            dl=fl(prime.x-f.x, prime.y-f.y, prime.z-f.z);
            dawars=(dl<dawars)?dl:dawars;
          }
          if (dawars<0.01) continue; // */

          Modulat *newAt = NULL;
          if (masymmUnit[i].iamcomp>1)
            newAt = new Modulat(masymmUnit[i].applySymm(
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.symmops.at(n),
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.trans.at(n)-floorD,
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4sym.at(n),
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4.at(n),
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4tr.at(n)));
          else
            newAt = new Modulat(masymmUnit[i].applySymm(
                  mol.zelle.symmops.at(n),
                  mol.zelle.trans.at(n)-floorD, 
                  mol.zelle.x4sym.at(n), 
                  mol.zelle.x4.at(n),
                  mol.zelle.x4tr.at(n)));
          matoms.append(*newAt);
        }
      }
    }
  }// */
  if (packart==4){//packe die Zelle
    V3 prime=V3(0,0,0),floorD=V3(0,0,0);
    for (int i=0; i<masymmUnit.size();i++){
      if ((masymmUnit.at(i).OrdZahl>-1)&&(masymmUnit.at(i).molindex>0)){
        for (int n=0; n<mol.zelle.symmops.size();n++){
          if (masymmUnit[i].iamcomp>1) 
            prime=mol.ccc[masymmUnit[i].iamcomp-2].nuCell.symmops.at(n) 
              * masymmUnit[i].frac(cubeGL->tvalue) 
              + mol.ccc[masymmUnit[i].iamcomp-2].nuCell.trans.at(n);
          else            
            prime=mol.zelle.symmops.at(n) * masymmUnit[i].frac(cubeGL->tvalue) + mol.zelle.trans.at(n) ;
          floorD=V3(floor(prime.x),floor(prime.y),floor(prime.z));
          bs.clear();
          bs=QString("%1_%2 %3 %4:%5,").arg(n+1).arg(5-(int)floorD.x).arg(5-(int)floorD.y).arg(5-(int)floorD.z)
            .arg(masymmUnit.at(i).molindex);
          if  (!brauchSymm.contains(bs)) {
            brauchSymm.append(bs);
          }
        }
      }//molindex
    }
    infoKanalNews(QString("Used Symmetry:<br>%1").arg(mol.symmcode2human(brauchSymm)));
    int s,h,k,l,gibscho=0,symmgroup;

    for (int j=0;j<brauchSymm.size();j++){
      if (5!=sscanf(brauchSymm[j].toLatin1(),"%d_%d %d %d:%d",&s,&h,&k,&l,&symmgroup))qDebug()<<"Katastrophe, o je!";
      h-=5;
      k-=5;
      l-=5;
      s--;
      for (int i=0;i<masymmUnit.size();i++){
        if ((masymmUnit[i].molindex==symmgroup)&&(masymmUnit[i].OrdZahl>-1)){// newAtom.frac=mol.zelle.symmops[s]*asymmUnit[i].frac+mol.zelle.trans.at(s)+V3(h,k,l);

          Modulat *newAt = NULL;
          if (masymmUnit[i].iamcomp>1)
            newAt = new Modulat(masymmUnit[i].applySymm(
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.symmops.at(s),
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.trans.at(s)+V3(h,k,l),
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4sym.at(s),
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4.at(s),
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4tr.at(s)));
          else
            newAt = new Modulat(masymmUnit[i].applySymm(
                  mol.zelle.symmops.at(s),
                  mol.zelle.trans.at(s)+V3(h,k,l), 
                  mol.zelle.x4sym.at(s), 
                  mol.zelle.x4.at(s),
                  mol.zelle.x4tr.at(s)));
          //     matoms.append(*newAt);
          //Modulat *newAt = new Modulat(masymmUnit[i].applySymm(mol.zelle.symmops.at(s),mol.zelle.trans.at(s)+V3(h,k,l), mol.zelle.x4sym.at(s), mol.zelle.x4.at(s),mol.zelle.x4tr.at(s)));
          sprintf(newAt->atomname,"%s_%d",masymmUnit[i].atomname,j+1);
          newAt->sg=1+j;
          //	  newAtom.OrdZahl = asymmUnit[i].OrdZahl;	  
          newAt->molindex = masymmUnit[i].molindex;
          gibscho=0;
          for(int gbt=0;gbt<matoms.size();gbt++){
            if (matoms.at(gbt).OrdZahl<0) continue;
            V3 n0=newAt->frac(cubeGL->tvalue), g0=matoms[gbt].frac(cubeGL->tvalue);
            if ((matoms[gbt].OrdZahl>-1)&&(fl(n0.x-g0.x,n0.y-g0.y,n0.z-g0.z)<0.2)) gibscho=1; 
          }
          if (!gibscho) { 
            matoms.append(*newAt);
          }
        }
      }
    }
    statusBar()->showMessage(tr("Neighbor search is finished"));

  }//4
  if ((packart==5)&&(packatom>-1)){//expand around given atom
    matoms=masymmUnit;
    V3 prime,floorD,D;
    double dk;
    for (int n=0; n<mol.zelle.symmops.size();n++){
      for (int i=0; i<masymmUnit.size();i++){
        if (masymmUnit[i].iamcomp>1) 
          prime=mol.ccc[masymmUnit[i].iamcomp-2].nuCell.symmops.at(n) 
            * masymmUnit[i].frac(cubeGL->tvalue) 
            + mol.ccc[masymmUnit[i].iamcomp-2].nuCell.trans.at(n);
        else            
          prime=mol.zelle.symmops.at(n) * masymmUnit[i].frac(cubeGL->tvalue) + mol.zelle.trans.at(n);
        D=prime - expander + V3(0.5,0.5,0.5) ;
        floorD=V3(floor(D.x),floor(D.y),floor(D.z));
        if ((n==0)&&(floorD==V3(0,0,0))) {continue;}
        D=D - floorD - V3(0.5,0.5,0.5);
        dk=fl(D.x,D.y,D.z);
        if (dk<mol.gd){
          
      //  printf("floor %f %f %f dk%f  %s\n",floorD.x,floorD.y,floorD.z,dk,masymmUnit.at(i).atomname);
          bs=QString("%1_%2 %3 %4:%5,").arg(n+1).arg(5-(int)floorD.x).arg(5-(int)floorD.y).arg(5-(int)floorD.z).arg(masymmUnit.at(i).molindex);
        //  qDebug()<<bs;
          /*            qDebug()<<bs
                        <<brauchSymm
                        <<brauchSymm.contains(bs)
                        <<packatom
                        <<matoms.at(packatom).atomname
                        <<expander.x
                        <<expander.y
                        <<expander.z
                        <<masymmUnit.at(i).atomname;*/
          if  (!brauchSymm.contains(bs)) {
            brauchSymm.append(bs);
          }
        }
      }
    }
    int s,h,k,l,gibscho=0,symmgroup;
    infoKanalNews(QString("Used Symmetry:<br>%1").arg(mol.symmcode2human(brauchSymm)));
   // qDebug()<<brauchSymm;
    for (int j=0;j<brauchSymm.size();j++){
      //	int ifx=
      sscanf(brauchSymm[j].toLatin1(),"%d_%d %d %d:%d",&s,&h,&k,&l,&symmgroup);
      h-=5;
      k-=5;
      l-=5;
      s--;

//      printf("s%d h%d k%d l%d sg%d\n", s, h, k, l, symmgroup);
      //        printf("fix %d\n",ifx);
      for (int i=0;i<masymmUnit.size();i++) 
        if ((masymmUnit[i].molindex==symmgroup)&&(masymmUnit[i].OrdZahl>-1)){
          //newAtom.frac=mol.zelle.symmops.at(s)*masymmUnit[i].frac+mol.zelle.trans.at(s)+V3(h,k,l);
          //  Modulat *newAt = new Modulat(masymmUnit[i].applySymm(mol.zelle.symmops.at(s),mol.zelle.trans.at(s)+V3(h,k,l), mol.zelle.x4sym.at(s), mol.zelle.x4.at(s),mol.zelle.x4tr.at(s)));
          Modulat *newAt = NULL;
          if (masymmUnit[i].iamcomp>1)
            newAt = new Modulat(masymmUnit[i].applySymm(
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.symmops.at(s),
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.trans.at(s)+V3(h,k,l),
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4sym.at(s),
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4.at(s),
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4tr.at(s)));
          else
            newAt = new Modulat(masymmUnit[i].applySymm(
                  mol.zelle.symmops.at(s),
                  mol.zelle.trans.at(s)+V3(h,k,l), 
                  mol.zelle.x4sym.at(s), 
                  mol.zelle.x4.at(s),
                  mol.zelle.x4tr.at(s)));
          sprintf(newAt->atomname,"%s_%d",masymmUnit[i].atomname,j+1);
          newAt->sg=1+j;
          newAt->molindex=masymmUnit[i].molindex;
          gibscho=0;
          for(int gbt=0;gbt<matoms.size();gbt++){
            if (matoms.at(gbt).OrdZahl<0) continue;
            V3 n0=newAt->frac(cubeGL->tvalue), g0=matoms[gbt].frac(cubeGL->tvalue);
            if ((matoms[gbt].OrdZahl>-1)&&(fl(n0.x-g0.x,n0.y-g0.y,n0.z-g0.z)<0.2)) gibscho=1; 
            //if (fl(newAt->frac0.x-matoms[gbt].frac0.x,newAt->frac0.y-matoms[gbt].frac0.y,newAt->frac0.z-matoms[gbt].frac0.z)<0.2) gibscho=1;
          }
          if (!gibscho) {
            matoms.append(*newAt);
          }
        }
    }
    statusBar()->showMessage(tr("Neighbor search is finished"));
  }  

  if (packart==6){ 
/*        qDebug()<<"hi"<<brauchSymm
          <<mol.zelle.symmops.size()
          <<mol.zelle.trans.size()
          <<mol.zelle.x4sym.size()
          <<mol.zelle.x4.size()<<masymmUnit.size()
          <<mol.zelle.x4tr.size();// */
    int s,h,k,l,gibscho=0,symmgroup;
    matoms=masymmUnit;
    infoKanalNews(QString("Used Symmetry:<br>%1").arg(mol.symmcode2human(brauchSymm)));
    for (int j=0;j<brauchSymm.size();j++){
      balken->setValue(j);
      sscanf(brauchSymm[j].toLatin1(),"%d_%d %d %d:%d",&s,&h,&k,&l,&symmgroup);
      h-=5;
      k-=5;
      l-=5;
      s--;
      for (int i=0;i<masymmUnit.size();i++){ 
        if ((masymmUnit[i].molindex==symmgroup)&&(masymmUnit[i].OrdZahl>-1)){ 
//          V3 r0=masymmUnit[i].frac(cubeGL->tvalue);
  //        printf("##%s %f %f %f cmp %d s%d h%d k%d l%d siz%d siz%d siz%d \n",masymmUnit[i].atomname,r0.x,r0.y,r0.z,masymmUnit[i].iamcomp,s,h,k,l, mol.zelle.symmops.size(),mol.zelle.trans.size(),mol.zelle.x4sym.size()<<mol.zelle.x4.size()<<mol.zelle.x4tr.size());
          //Modulat *newAt = new Modulat(masymmUnit[i].applySymm(mol.zelle.symmops.at(s),mol.zelle.trans.at(s)+V3(h,k,l), mol.zelle.x4sym.at(s), mol.zelle.x4.at(s),mol.zelle.x4tr.at(s)));
          Modulat *newAt = NULL;
          if (masymmUnit[i].iamcomp>1)
            newAt = new Modulat(masymmUnit[i].applySymm(
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.symmops.at(s),
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.trans.at(s)+V3(h,k,l),
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4sym.at(s),
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4.at(s),
                  mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4tr.at(s)));
          else
            if (cubeGL->isModulated) 
            newAt = new Modulat(masymmUnit[i].applySymm(
                  mol.zelle.symmops.at(s),
                  mol.zelle.trans.at(s)+V3(h,k,l), 
                  mol.zelle.x4sym.at(s), 
                  mol.zelle.x4.at(s),
                  mol.zelle.x4tr.at(s)));
            else newAt = new Modulat(masymmUnit[i].applySymm(mol.zelle.symmops.at(s),mol.zelle.trans.at(s)+V3(h,k,l),V3(0,0,0),1,0));
          //printf("newAtom %p\n",newAt);
          sprintf(newAt->atomname,"%s_%d",masymmUnit[i].atomname,j+1);
          //  newAtom.frac=mol.zelle.symmops.at(s)*asymmUnit[i].frac+mol.zelle.trans.at(s)+V3(h,k,l);
          //  newAtom.part=asymmUnit[i].part;
          //  sprintf(newAtom.atomname,"%s_%d",asymmUnit[i].atomname,j+1);
          //newAtom.sg=1+j;
          //  newAtom.OrdZahl=asymmUnit[i].OrdZahl;
          //  newAtom.molindex=asymmUnit[i].molindex;
          gibscho=0;
          for(int gbt=0;gbt<matoms.size();gbt++){
            if (matoms.at(gbt).OrdZahl<0) continue;
            V3 n0=newAt->frac(cubeGL->tvalue), g0=matoms[gbt].frac(cubeGL->tvalue);
            if ((matoms[gbt].OrdZahl>-1)&&(fl(n0.x-g0.x,n0.y-g0.y,n0.z-g0.z)<0.2)) gibscho=1; 
            //            if (fl(newAt->frac0.x-matoms[gbt].frac0.x,newAt->frac0.y-matoms[gbt].frac0.y,newAt->frac0.z-matoms[gbt].frac0.z)<0.2) gibscho=1;
          }
          if (!gibscho) {
            //printf("??\n");
            matoms.append(*newAt);
            //V3 r=newAt->frac(cubeGL->tvalue);
            //printf("%s %f %f %f  \n",newAt->atomname,r.x,r.y,r.z);
          }
        }//molindex
      }
    }
    statusBar()->showMessage(tr("Neighbor search is finished"));
  } //6 
  if (packart==7){
    QDialog *boxpack= new QDialog(this);
    QDoubleSpinBox 
      *aminlim,*amaxlim,
      *bminlim,*bmaxlim,
      *cminlim,*cmaxlim;

    aminlim = new QDoubleSpinBox(boxpack);
    amaxlim = new QDoubleSpinBox(boxpack);
    bminlim = new QDoubleSpinBox(boxpack);
    bmaxlim = new QDoubleSpinBox(boxpack);
    cminlim = new QDoubleSpinBox(boxpack);
    cmaxlim = new QDoubleSpinBox(boxpack);

    aminlim->setMinimum(-10.0);
    amaxlim->setMinimum(0.0);
    bminlim->setMinimum(-10.0);
    bmaxlim->setMinimum(0.0);
    cminlim->setMinimum(-10.0);
    cmaxlim->setMinimum(0.0);

    aminlim->setMaximum(0.0);
    amaxlim->setMaximum(10.0);
    bminlim->setMaximum(0.0);
    bmaxlim->setMaximum(10.0);
    cminlim->setMaximum(0.0);
    cmaxlim->setMaximum(10.0);

    aminlim->setValue(0.0);
    amaxlim->setValue(1.0);
    bminlim->setValue(0.0);
    bmaxlim->setValue(1.0);
    cminlim->setValue(0.0);
    cmaxlim->setValue(1.0);

    QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    // QPushButton *ok = buttonBoxMC->button(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(rejected()), boxpack, SLOT(reject())); 
    connect(buttonBox, SIGNAL(accepted()), boxpack, SLOT(accept())); 

    QGridLayout *g = new QGridLayout(boxpack);
    g->addWidget(new QLabel("pack range along a "),0,0);
    g->addWidget(aminlim,1,0);
    g->addWidget(amaxlim,2,0);
    g->addWidget(new QLabel("pack range along b "),0,1);
    g->addWidget(bminlim,1,1);
    g->addWidget(bmaxlim,2,1);
    g->addWidget(new QLabel("pack range along c "),0,2);
    g->addWidget(cminlim,1,2);
    g->addWidget(cmaxlim,2,2);
    g->addWidget(buttonBox,10,0,1,3);
    if (QDialog::Accepted==boxpack->exec()){

      xdinp.clear();
      matoms.clear();
      V3 p,g0;
            for (int s=0; s<mol.zelle.symmops.size(); s++){
              for (int i=0; i<masymmUnit.size(); i++) {
//                Modulat *newAt = NULL;
                Modulat nat=((masymmUnit[i].iamcomp>1))?
//                if (masymmUnit[i].iamcomp>1)
                  //newAt = new Modulat(masymmUnit[i].applySymm(
                  masymmUnit[i].applySymm(
                        mol.ccc[masymmUnit[i].iamcomp-2].nuCell.symmops.at(s),
                        mol.ccc[masymmUnit[i].iamcomp-2].nuCell.trans.at(s),
                        mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4sym.at(s),
                        mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4.at(s),
                        mol.ccc[masymmUnit[i].iamcomp-2].nuCell.x4tr.at(s))
  //              else
                  //newAt = new Modulat(masymmUnit[i].applySymm(
                  :masymmUnit[i].applySymm(
                        mol.zelle.symmops.at(s),
                        mol.zelle.trans.at(s), 
                        mol.zelle.x4sym.at(s), 
                        mol.zelle.x4.at(s),
                        mol.zelle.x4tr.at(s));
      for (int h=-11; h< 12; h++){
        for (int k=-11; k< 12; k++){
          for (int l=-11; l< 12; l++){
                V3 ppp=nat.frac(cubeGL->tvalue)+V3(h,k,l);
                if ((ppp.x)<aminlim->value()) {continue;}
                if ((ppp.y)<bminlim->value()) {continue;}
                if ((ppp.z)<cminlim->value()) {continue;}
                if ((ppp.x)>amaxlim->value()) {continue;}
                if ((ppp.y)>bmaxlim->value()) {continue;}
                if ((ppp.z)>cmaxlim->value()) {continue;}
                bool gibscho=false;
                for(int gbt=0;gbt<matoms.size();gbt++){
                  if (matoms.at(gbt).OrdZahl<0) {continue;}
                  g0=matoms[gbt].frac(cubeGL->tvalue);
                  //p=newAt->frac(cubeGL->tvalue);
                  if ((matoms[gbt].OrdZahl>-1)&&(fl(ppp.x-g0.x,ppp.y-g0.y,ppp.z-g0.z)<0.2)) gibscho=true; 
                }
                if (!gibscho) { 

//                  matoms.append(*newAt);
                  Modulat neu= nat;
                  neu.frac0+=V3(h,k,l);
                  matoms.append(neu);
                  //V3 r=newAt->frac(cubeGL->tvalue);
                  //printf("%s %f %f %f  \n",newAt->atomname,r.x,r.y,r.z);
                }//gbt
              }//i++
            }//s++
          }//l++
        }//k++
      }//h++
    }//exec
  }//packart==7
  //////////////////

  V3 uz0f,uz1f,uz2f,uz3f,uz4f,uz5f,uz6f,uz7f;
  uz0f.x=0.0;  uz0f.y=0.0;  uz0f.z=0.0;
  uz1f.x=1.0;  uz1f.y=0.0;  uz1f.z=0.0;
  uz2f.x=0.0;  uz2f.y=1.0;  uz2f.z=0.0;
  uz3f.x=0.0;  uz3f.y=0.0;  uz3f.z=1.0;
  uz4f.x=1.0;  uz4f.y=1.0;  uz4f.z=0.0;
  uz5f.x=1.0;  uz5f.y=0.0;  uz5f.z=1.0;
  uz6f.x=0.0;  uz6f.y=1.0;  uz6f.z=1.0;
  uz7f.x=1.0;  uz7f.y=1.0;  uz7f.z=1.0;
  if (cubeGL->moliso!=NULL) cubeGL->moliso->orig=Vector3(0,0,0);
  mol.frac2kart(uz0f,mol.uz0k);
  mol.frac2kart(uz1f,mol.uz1k);
  mol.frac2kart(uz2f,mol.uz2k);
  mol.frac2kart(uz3f,mol.uz3k);
  mol.frac2kart(uz4f,mol.uz4k);
  mol.frac2kart(uz5f,mol.uz5k);
  mol.frac2kart(uz6f,mol.uz6k);
  mol.frac2kart(uz7f,mol.uz7k);
  double dim=mdimension(matoms);
  cubeGL->L=100.0/dim;
  //qDebug()<<dim<<"dim L"<<cubeGL->L;
  cubeGL->bas=0;

  //  cubeGL->resetENV();
  dock->hide();
  dock2->hide();
  cubeGL->setVisible(true);
  dock2->show();
  dock->show();
  dock->setMaximumWidth(width()/5);
  printf("MgrowSymm with packart = %d\n",packart);
}

void MyWindow::growSymm(int packart,int packatom){
  //  qDebug()<<smx<<atmax <<asymmUnit.size();
  if ((asymmUnit.isEmpty())&&(!masymmUnit.isEmpty())){
    //    qDebug()<<packart<<packatom<<"modul grow";
    //fprintf(stderr,"%d\n",__LINE__);
    mgrowSymm(packart,packatom);
    //plotSDM(25);
    //fprintf(stderr,"%d\n",__LINE__);
    return;
  }
  if (packart==0){
    infoKanal->clear();

  }
  QTime speedTest;
  mol.vorobas=0;
  speedTest.start();
  cubeGL->setVisible(false);
  unFilter();
  mol.bonds_made=0;
  mol.knopf_made=0;
  QStringList brauchSymm;
  QString  bs;
  V3 expander=V3(0,0,0);
  if ((packart==5) && (packatom>-1) && (packatom<xdinp.size())) expander=xdinp.at(packatom).frac;
  QString blob=infoKanal->toPlainText();
  if ((packart==5)&&(blob.contains("'"))){
    QStringList blub=blob.split("'",QString::SkipEmptyParts);
    for (int i=0; i<blub.size(); i++) if ((i%2)==1) brauchSymm.append(QString("%1,").arg(blub.at(i)));
  }
  atmax=smx-dummax;
  xdinp.clear();
  if ((packart==1)||(packart==6)){
    if (brauchSymm.isEmpty()) {SDM(brauchSymm,packart);packart=6;}
    if (brauchSymm.isEmpty()) {packart=0;}
  }
  //  printf("growSymm line %d\n",__LINE__);
  if (!george) makeXDPartAux();
  //  printf("growSymm line %d\n",__LINE__);
  xdinp=asymmUnit;
  //matoms=masymmUnit;//*

  //  if (!packart) for (int i=0; i<xdinp.size(); i++) printf("%-9s -->%d<-- %d %d \n",xdinp.at(i).atomname,xdinp.at(i).molindex-asymmUnit.at(i).molindex,xdinp.at(i).molindex,asymmUnit.at(i).molindex);
  INP newAtom;  
  newAtom.sg=1;
  infoKanalNews("Used Symmetry:");
  if (packart) {
    unFilter();
    double SUCHRAD=mol.gd;
    if (packart==4){//packe die Zelle
      V3 prime,floorD;
      for (int i=0; i<asymmUnit.size();i++)
        if ((asymmUnit.at(i).OrdZahl>-1)&&(asymmUnit.at(i).molindex>0)){
          for (int n=0; n<mol.zelle.symmops.size();n++){
            prime=mol.zelle.symmops.at(n) * asymmUnit.at(i).frac + mol.zelle.trans.at(n) ;//+ V3(0.005,0.005,0.005)
            floorD=V3(floor(prime.x),floor(prime.y),floor(prime.z));
            bs.clear();
            bs=QString("%1_%2%3%4:%5,").arg(n+1).arg(5-(int)floorD.x).arg(5-(int)floorD.y).arg(5-(int)floorD.z).arg(asymmUnit.at(i).molindex);
            if  (!brauchSymm.contains(bs)) {
              brauchSymm.append(bs);
            }
          }
        }
      infoKanalNews(QString("Used Symmetry:<br>%1").arg(mol.symmcode2human(brauchSymm)));
      int s,h,k,l,gibscho=0,symmgroup;

      for (int j=0;j<brauchSymm.size();j++){
        if (5!=sscanf(brauchSymm[j].toLatin1(),"%d_%1d%1d%1d:%d",&s,&h,&k,&l,&symmgroup))qDebug()<<"katastrfe";
        h-=5;
        k-=5;
        l-=5;
        s--;
        for (int i=0;i<asymmUnit.size();i++){
          if ((asymmUnit[i].molindex==symmgroup)&&(asymmUnit[i].OrdZahl>-1)) newAtom.frac=mol.zelle.symmops[s]*asymmUnit[i].frac+mol.zelle.trans.at(s)+V3(h,k,l);
          sprintf(newAtom.atomname,"%s_%d",asymmUnit[i].atomname,j+1);
          newAtom.sg=1+j;
          newAtom.OrdZahl = asymmUnit[i].OrdZahl;	  
          newAtom.amul=asymmUnit[i].amul;
          newAtom.molindex = asymmUnit[i].molindex;
          if ((asymmUnit[i].u.m12==0.0)&&(asymmUnit[i].u.m23==0.0)&&(asymmUnit[i].u.m13==0.0)){
            newAtom.u.m11=newAtom.u.m22=newAtom.u.m33=asymmUnit[i].uf.m11;
            newAtom.u.m12=newAtom.u.m13=newAtom.u.m23=newAtom.u.m21=newAtom.u.m31=newAtom.u.m32=0.0;}
          else {
            Usym(asymmUnit[i].uf,mol.zelle.symmops[s],newAtom.uf);
            mol.Uf2Uo(newAtom.uf,newAtom.u);}
          gibscho=0;
          for(int gbt=0;gbt<xdinp.size();gbt++){
            if (xdinp.at(gbt).OrdZahl<0) continue;
            if ((xdinp[gbt].OrdZahl>-1)&&(fl(newAtom.frac.x-xdinp[gbt].frac.x,newAtom.frac.y-xdinp[gbt].frac.y,newAtom.frac.z-xdinp[gbt].frac.z)<0.2)) gibscho=1; 
          }
          if (!gibscho) { 
            xdinp.append(newAtom);
          }
        }
      }
      statusBar()->showMessage(tr("Neighbor search is finished"));

    }
    if (packart==3){//packe und schneide die Zelle
      V3 prime,floorD;
      double dawars=1000.0,dl;
      /*
         for (int i=0; i<asymmUnit.size();i++){
         prime=asymmUnit.at(i).frac;
         if (fabs(asymmUnit.at(i).frac.x)<0.01)  {
         prime.x=1;
         newAtom.part=asymmUnit[i].part;
         newAtom.frac=prime;
         newAtom.OrdZahl=asymmUnit[i].OrdZahl;
         newAtom.molindex=asymmUnit[i].molindex;
         sprintf(newAtom.atomname,"%s'",asymmUnit[i].atomname);
         newAtom.sg=1;
         if ((asymmUnit[i].u.m12==0.0)&&(asymmUnit[i].u.m23==0.0)&&(asymmUnit[i].u.m13==0.0)){
         newAtom.uf.m11=newAtom.u.m11=newAtom.u.m22=newAtom.u.m33=asymmUnit[i].uf.m11;
         newAtom.u.m12=newAtom.u.m13=newAtom.u.m23=newAtom.u.m21=newAtom.u.m31=newAtom.u.m32=0.0;
         }
         else {
         Usym(asymmUnit[i].uf,mol.zelle.symmops[0],newAtom.uf);
         Uf2Uo(newAtom.uf,newAtom.u);
         }
         xdinp.append(newAtom);
         smx++;
         }
         if (fabs(asymmUnit.at(i).frac.y)<0.01)  {
         prime.y=1;
         newAtom.part=asymmUnit[i].part;
         newAtom.frac=prime;
         newAtom.OrdZahl=asymmUnit[i].OrdZahl;
         newAtom.molindex=asymmUnit[i].molindex;
         sprintf(newAtom.atomname,"%s'",asymmUnit[i].atomname);
         newAtom.sg=1;
         if ((asymmUnit[i].u.m12==0.0)&&(asymmUnit[i].u.m23==0.0)&&(asymmUnit[i].u.m13==0.0)){
         newAtom.uf.m11=newAtom.u.m11=newAtom.u.m22=newAtom.u.m33=asymmUnit[i].uf.m11;
         newAtom.u.m12=newAtom.u.m13=newAtom.u.m23=newAtom.u.m21=newAtom.u.m31=newAtom.u.m32=0.0;
         }
         else {
         Usym(asymmUnit[i].uf,mol.zelle.symmops[0],newAtom.uf);
         Uf2Uo(newAtom.uf,newAtom.u);
         }
         xdinp.append(newAtom);
         smx++;
         }
         if (fabs(asymmUnit.at(i).frac.z)<0.01)  {
         prime.z=1;
         newAtom.part=asymmUnit[i].part;
         newAtom.frac=prime;
         newAtom.OrdZahl=asymmUnit[i].OrdZahl;
         newAtom.molindex=asymmUnit[i].molindex;
         sprintf(newAtom.atomname,"%s'",asymmUnit[i].atomname);
         newAtom.sg=1;
         if ((asymmUnit[i].u.m12==0.0)&&(asymmUnit[i].u.m23==0.0)&&(asymmUnit[i].u.m13==0.0)){
         newAtom.uf.m11=newAtom.u.m11=newAtom.u.m22=newAtom.u.m33=asymmUnit[i].uf.m11;
         newAtom.u.m12=newAtom.u.m13=newAtom.u.m23=newAtom.u.m21=newAtom.u.m31=newAtom.u.m32=0.0;
         }
         else {
         Usym(asymmUnit[i].uf,mol.zelle.symmops[0],newAtom.uf);
         Uf2Uo(newAtom.uf,newAtom.u);
         }
         xdinp.append(newAtom);
         smx++;
         }
         }*/
      for (int n=0; n<mol.zelle.symmops.size();n++){
        for (int i=0; i<asymmUnit.size();i++)
          if ((asymmUnit.at(i).OrdZahl>-1)&&(asymmUnit.at(i).molindex>0)){
            prime=mol.zelle.symmops.at(n) * asymmUnit.at(i).frac + mol.zelle.trans.at(n) ;//+ V3(-0.0050,-0.0050,-0.0050)
            floorD=V3(floor(prime.x),floor(prime.y),floor(prime.z));
            prime=prime -floorD;	  
            dawars=1000.0;
            for (int g=0; g<xdinp.size();g++){
              if ((xdinp[i].OrdZahl*xdinp[g].OrdZahl<0)) continue;
              dl=fl(prime.x-xdinp[g].frac.x, prime.y-xdinp[g].frac.y, prime.z-xdinp[g].frac.z);
              dawars=(dl<dawars)?dl:dawars;
            }
            if (dawars<0.01) continue; // */
            bs=QString("%1_%2%3%4:%5,").arg(n+1).arg(5-(int)floorD.x).arg(5-(int)floorD.y).arg(5-(int)floorD.z).arg(asymmUnit.at(i).molindex);
            if  (!brauchSymm.contains(bs)) {
              brauchSymm.append(bs);
            }
            newAtom.part=asymmUnit[i].part;
            newAtom.frac=prime;
            newAtom.OrdZahl=asymmUnit[i].OrdZahl;
            newAtom.molindex=asymmUnit[i].molindex;
            newAtom.amul=asymmUnit[i].amul;
            sprintf(newAtom.atomname,"%s'",asymmUnit[i].atomname);
            newAtom.sg=1;
            if ((asymmUnit[i].u.m12==0.0)&&(asymmUnit[i].u.m23==0.0)&&(asymmUnit[i].u.m13==0.0)){
              newAtom.uf.m11=newAtom.u.m11=newAtom.u.m22=newAtom.u.m33=asymmUnit[i].uf.m11;
              newAtom.u.m12=newAtom.u.m13=newAtom.u.m23=newAtom.u.m21=newAtom.u.m31=newAtom.u.m32=0.0;
            }
            else {
              Usym(asymmUnit[i].uf,mol.zelle.symmops[n],newAtom.uf);
              mol.Uf2Uo(newAtom.uf,newAtom.u);
            }
            //printf("%-10s %9.5f %9.5f %9.5f\n",newAtom.atomname,newAtom.frac.x,newAtom.frac.y,newAtom.frac.z);
            xdinp.append(newAtom);
            smx++;
          }
      }


      char dumstr[500];
      for (int j=0; j<3;j++){
        int asa=xdinp.size();
        for (int i=0;i<asa;i++){
          if (fabs(xdinp.at(i).frac.x)<0.05) {
            newAtom.frac=xdinp.at(i).frac;
            newAtom.frac.x+=1.0;
            newAtom.sg=1;
            newAtom.part=xdinp[i].part;
            newAtom.OrdZahl=xdinp[i].OrdZahl;
            newAtom.molindex=xdinp[i].molindex;
            strcpy(dumstr,xdinp[i].atomname);
            sprintf(newAtom.atomname,"%s'",strtok(dumstr,"'"));
            dawars=1000.0;
            prime=newAtom.frac;
            for (int g=0; g<xdinp.size();g++){
              if ((xdinp[i].OrdZahl*xdinp[g].OrdZahl<0)) continue;
              dl=fl(prime.x-xdinp[g].frac.x, prime.y-xdinp[g].frac.y, prime.z-xdinp[g].frac.z);
              dawars=(dl<dawars)?dl:dawars;
            }
            if (dawars>0.01) {
              xdinp.append(newAtom);
              //printf("%-10s %9.5f %9.5f %9.5f\n",newAtom.atomname, newAtom.frac.x,newAtom.frac.y,newAtom.frac.z);
              smx++;
            }
          }
          if (fabs(xdinp.at(i).frac.y)<0.05) {
            newAtom.frac=xdinp.at(i).frac;
            newAtom.frac.y+=1.0;
            newAtom.sg=1;
            newAtom.part=xdinp[i].part;
            newAtom.OrdZahl=xdinp[i].OrdZahl;
            newAtom.molindex=xdinp[i].molindex;
            strcpy(dumstr,xdinp[i].atomname);
            sprintf(newAtom.atomname,"%s'",strtok(dumstr,"'"));
            dawars=1000.0;
            prime=newAtom.frac;
            for (int g=0; g<xdinp.size();g++){
              if ((xdinp[i].OrdZahl*xdinp[g].OrdZahl<0)) continue;
              dl=fl(prime.x-xdinp[g].frac.x, prime.y-xdinp[g].frac.y, prime.z-xdinp[g].frac.z);
              dawars=(dl<dawars)?dl:dawars;
            }
            if (dawars>0.01) {
              xdinp.append(newAtom);
              //printf("%-10s %9.5f %9.5f %9.5f\n",newAtom.atomname, newAtom.frac.x,newAtom.frac.y,newAtom.frac.z);
              smx++;
            }
          }
          if (fabs(xdinp.at(i).frac.z)<0.05) {
            newAtom.frac=xdinp.at(i).frac;
            newAtom.sg=1;
            newAtom.part=xdinp[i].part;
            newAtom.OrdZahl=xdinp[i].OrdZahl;
            newAtom.molindex=xdinp[i].molindex;
            strcpy(dumstr,xdinp[i].atomname);
            sprintf(newAtom.atomname,"%s'",strtok(dumstr,"'"));
            newAtom.frac.z+=1.0;
            dawars=1000.0;
            prime=newAtom.frac;
            for (int g=0; g<xdinp.size();g++){
              if ((xdinp[i].OrdZahl*xdinp[g].OrdZahl<0)) continue;
              dl=fl(prime.x-xdinp[g].frac.x, prime.y-xdinp[g].frac.y, prime.z-xdinp[g].frac.z);
              dawars=(dl<dawars)?dl:dawars;
            }
            if (dawars>0.01){ xdinp.append(newAtom);
              //printf("%-10s %9.5f %9.5f %9.5f\n",newAtom.atomname, newAtom.frac.x,newAtom.frac.y,newAtom.frac.z);
              smx++;}
          }
        }
      }// */
      infoKanalNews(QString("Used Symmetry:<br>%1").arg(mol.symmcode2human(brauchSymm)));
      printf("%d\n",smx);
    } 
    if ((packart==5)&&(packatom>-1)){//expand around given atom
      V3 prime,floorD,D;
      double dk;
      for (int n=0; n<mol.zelle.symmops.size();n++){
        for (int i=0; i<asymmUnit.size();i++){
          prime=mol.zelle.symmops.at(n) * asymmUnit.at(i).frac + mol.zelle.trans.at(n);
          D=prime - expander + V3(0.5,0.5,0.5) ;
          floorD=V3(floor(D.x),floor(D.y),floor(D.z));
          if ((n==0)&&(floorD==V3(0,0,0))) {continue;}
          D=D - floorD - V3(0.5,0.5,0.5);
          dk=fl(D.x,D.y,D.z);
          if (dk<SUCHRAD){
            bs=QString("%1_%2%3%4:%5,").arg(n+1).arg(5-(int)floorD.x).arg(5-(int)floorD.y).arg(5-(int)floorD.z).arg(asymmUnit.at(i).molindex);
            if  (!brauchSymm.contains(bs)) {
              brauchSymm.append(bs);
            }
          }
        }
      }
      int s,h,k,l,gibscho=0,symmgroup;
      infoKanalNews(QString("Used Symmetry:<br>%1").arg(mol.symmcode2human(brauchSymm)));
      for (int j=0;j<brauchSymm.size();j++){
        sscanf(brauchSymm[j].toLatin1(),"%d_%1d%1d%1d:%d",&s,&h,&k,&l,&symmgroup);
        h-=5;
        k-=5;
        l-=5;
        s--;
        for (int i=0;i<asymmUnit.size();i++) 
          if ((asymmUnit[i].molindex==symmgroup)&&(asymmUnit[i].OrdZahl>-1)){
            newAtom.frac=mol.zelle.symmops.at(s)*asymmUnit[i].frac+mol.zelle.trans.at(s)+V3(h,k,l);
            sprintf(newAtom.atomname,"%s_%d",asymmUnit[i].atomname,j+1);
            newAtom.sg=1+j;
            newAtom.atomtype=asymmUnit[i].atomtype;
            newAtom.jtf=asymmUnit[i].jtf;
            newAtom.part=asymmUnit[i].part;
            newAtom.amul=asymmUnit[i].amul;
            newAtom.OrdZahl=asymmUnit[i].OrdZahl;
            newAtom.molindex=asymmUnit[i].molindex;
            if (asymmUnit[i].jtf<2){
              newAtom.u.m11=newAtom.u.m22=newAtom.u.m33=asymmUnit[i].uf.m11;
              newAtom.u.m12=newAtom.u.m13=newAtom.u.m23=newAtom.u.m21=newAtom.u.m31=newAtom.u.m32=0.0;}
            else {
              Usym(asymmUnit[i].uf,mol.zelle.symmops[s],newAtom.uf);
              mol.Uf2Uo(newAtom.uf,newAtom.u);}
            /*            qDebug()<<"~~~~~~(o-o)"<<asymmUnit[i].jtf
                          <<asymmUnit[i].uf.m11<<newAtom.uf.m11
                          <<asymmUnit[i].uf.m22<<newAtom.uf.m22
                          <<asymmUnit[i].uf.m33<<newAtom.uf.m33; // */
            gibscho=0;
            for(int gbt=0;gbt<xdinp.size();gbt++){
              if (xdinp.at(gbt).OrdZahl<0) continue;
              if (fl(newAtom.frac.x-xdinp[gbt].frac.x,newAtom.frac.y-xdinp[gbt].frac.y,newAtom.frac.z-xdinp[gbt].frac.z)<0.2) gibscho=1;
            }
            if (!gibscho) {
              xdinp.append(newAtom);
            }
          }
      }
      statusBar()->showMessage(tr("Neighbor search is finished"));
    }  
    if (packart==6){ 
      // printf("growSymm line %d\n",__LINE__);
      int s,h,k,l,gibscho=0,symmgroup;
      balken->setMinimum(0);
      balken->setMaximum(brauchSymm.size());
      balken->show();      
      infoKanalNews(QString("Used Symmetry:<br>%1").arg(mol.symmcode2human(brauchSymm)));
      for (int j=0;j<brauchSymm.size();j++){
        balken->setValue(j);
        sscanf(brauchSymm[j].toLatin1(),"%d_%1d%1d%1d:%d",&s,&h,&k,&l,&symmgroup);
        h-=5;
        k-=5;
        l-=5;
        s--;
        /*
           int psiz=mol.polyeders.size();
           printf("===>%d\n",mol.polyeders.size());
           for (int i=0;i<psiz;i++){
           PolyEder p;
           p.af=mol.zelle.symmops.at(s)*mol.polyeders.at(i).af+mol.zelle.trans.at(s)+V3(h,k,l);
           p.bf=mol.zelle.symmops.at(s)*mol.polyeders.at(i).bf+mol.zelle.trans.at(s)+V3(h,k,l);
           p.cf=mol.zelle.symmops.at(s)*mol.polyeders.at(i).cf+mol.zelle.trans.at(s)+V3(h,k,l);
           gibscho=0;
           for(int gbt=0;gbt<psiz;gbt++)
           if ((fl(p.af.x-mol.polyeders.at(gbt).af.x,p.af.y-mol.polyeders.at(gbt).af.y,p.af.z-mol.polyeders.at(gbt).af.z)+
           fl(p.bf.x-mol.polyeders.at(gbt).bf.x,p.bf.y-mol.polyeders.at(gbt).bf.y,p.bf.z-mol.polyeders.at(gbt).bf.z)+
           fl(p.cf.x-mol.polyeders.at(gbt).af.x,p.cf.y-mol.polyeders.at(gbt).cf.y,p.cf.z-mol.polyeders.at(gbt).cf.z))<0.5) gibscho=1;
           mol.polyeders.append(p);
           }*/
        for (int i=0;i<asymmUnit.size();i++) 
          if ((asymmUnit[i].molindex==symmgroup)&&(asymmUnit[i].OrdZahl>-1)){ 
            newAtom.frac=mol.zelle.symmops.at(s)*asymmUnit[i].frac+mol.zelle.trans.at(s)+V3(h,k,l);
            newAtom.part=asymmUnit[i].part;
            newAtom.amul=asymmUnit[i].amul;
            newAtom.atomtype=asymmUnit[i].atomtype;


            sprintf(newAtom.atomname,"%s_%d",asymmUnit[i].atomname,j+1);
            newAtom.sg=1+j;
            newAtom.OrdZahl=asymmUnit[i].OrdZahl;
            newAtom.molindex=asymmUnit[i].molindex;
            if ((asymmUnit[i].u.m12==0.0 )&&(asymmUnit[i].u.m23==0.0)&&(asymmUnit[i].u.m13==0.0)){
              newAtom.uf=asymmUnit[i].uf;
              newAtom.u.m11=newAtom.u.m22=newAtom.u.m33=asymmUnit[i].uf.m11;
              newAtom.u.m12=newAtom.u.m13=newAtom.u.m23=newAtom.u.m21=newAtom.u.m31=newAtom.u.m32=0.0;}
            else { 
              Usym(asymmUnit[i].uf,mol.zelle.symmops[s],newAtom.uf);
              mol.Uf2Uo(newAtom.uf,newAtom.u);}
            gibscho=0;
            for(int gbt=0;gbt<xdinp.size();gbt++){
              if (xdinp.at(gbt).OrdZahl<0) continue;
              if (fl(newAtom.frac.x-xdinp[gbt].frac.x,newAtom.frac.y-xdinp[gbt].frac.y,newAtom.frac.z-xdinp[gbt].frac.z)<0.2) gibscho=1;
            }
            if (!gibscho) {
              xdinp.append(newAtom);
            }
          }
      }
      statusBar()->showMessage(tr("Neighbor search is finished"));
    }  
    
    if (packart==7){//1
      QDialog *boxpack= new QDialog(this);
      QDoubleSpinBox 
        *aminlim,*amaxlim,
        *bminlim,*bmaxlim,
        *cminlim,*cmaxlim;
      aminlim = new QDoubleSpinBox(boxpack);
      amaxlim = new QDoubleSpinBox(boxpack);
      bminlim = new QDoubleSpinBox(boxpack);
      bmaxlim = new QDoubleSpinBox(boxpack);
      cminlim = new QDoubleSpinBox(boxpack);
      cmaxlim = new QDoubleSpinBox(boxpack);
      aminlim->setMinimum(-10.0);
      amaxlim->setMinimum(0.0);
      bminlim->setMinimum(-10.0);
      bmaxlim->setMinimum(0.0);
      cminlim->setMinimum(-10.0);
      cmaxlim->setMinimum(0.0);
      aminlim->setMaximum(1.0);
      amaxlim->setMaximum(10.0);
      bminlim->setMaximum(1.0);
      bmaxlim->setMaximum(10.0);
      cminlim->setMaximum(1.0);
      cmaxlim->setMaximum(10.0);
      aminlim->setValue(0.0);
      amaxlim->setValue(1.0);
      bminlim->setValue(0.0);
      bmaxlim->setValue(1.0);
      cminlim->setValue(0.0);
      cmaxlim->setValue(1.0);
      QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
      connect(buttonBox, SIGNAL(rejected()), boxpack, SLOT(reject())); 
      connect(buttonBox, SIGNAL(accepted()), boxpack, SLOT(accept())); 
      QGridLayout *g = new QGridLayout(boxpack);
      g->addWidget(new QLabel("pack range along a "),0,0);
      g->addWidget(aminlim,1,0);
      g->addWidget(amaxlim,2,0);
      g->addWidget(new QLabel("pack range along b "),0,1);
      g->addWidget(bminlim,1,1);
      g->addWidget(bmaxlim,2,1);
      g->addWidget(new QLabel("pack range along c "),0,2);
      g->addWidget(cminlim,1,2);
      g->addWidget(cmaxlim,2,2);
      g->addWidget(buttonBox,10,0,1,3);
      if (QDialog::Accepted==boxpack->exec()){
        if (!hatlokale) xdinp.clear();

        V3 prime,floorD;
        double dawars=1000.0,dl;
        for (int n=0; n<mol.zelle.symmops.size();n++){
          for (int i=0; i<asymmUnit.size();i++){
            if ((asymmUnit.at(i).OrdZahl>-1)&&(asymmUnit.at(i).molindex>0)){
              prime=mol.zelle.symmops.at(n) * asymmUnit.at(i).frac + mol.zelle.trans.at(n);
              floorD=V3(floor(prime.x),floor(prime.y),floor(prime.z));
              prime=prime -floorD;	  
              dawars=1000.0;
              for (int g=0; g<xdinp.size();g++){
                if ((asymmUnit[i].OrdZahl*xdinp[g].OrdZahl<0)) continue;
                dl=fl(prime.x-xdinp[g].frac.x, prime.y-xdinp[g].frac.y, prime.z-xdinp[g].frac.z);
                dawars=(dl<dawars)?dl:dawars;
              }
              if (dawars<0.01) continue; 
              for (int hh=-11; hh<12; hh++)
                for (int kk=-11; kk<12; kk++)
                  for (int ll=-11; ll<12; ll++){
                    if ((prime.x+hh)<aminlim->value()) continue;
                    if ((prime.y+kk)<bminlim->value()) continue;
                    if ((prime.z+ll)<cminlim->value()) continue;
                    if ((prime.x+hh)>amaxlim->value()) continue;
                    if ((prime.y+kk)>bmaxlim->value()) continue;
                    if ((prime.z+ll)>cmaxlim->value()) continue;
                    newAtom.part=asymmUnit[i].part;
                    newAtom.frac=prime+V3((double)hh,(double)kk,(double)ll);
                    newAtom.OrdZahl=asymmUnit[i].OrdZahl;
                    newAtom.molindex=asymmUnit[i].molindex;
                    newAtom.amul=asymmUnit[i].amul;
                    sprintf(newAtom.atomname,"%s",asymmUnit[i].atomname);
                    newAtom.sg=1;
                    if ((asymmUnit[i].u.m12==0.0)&&(asymmUnit[i].u.m23==0.0)&&(asymmUnit[i].u.m13==0.0)){
                      newAtom.uf.m11=newAtom.u.m11=newAtom.u.m22=newAtom.u.m33=asymmUnit[i].uf.m11;
                      newAtom.u.m12=newAtom.u.m13=newAtom.u.m23=newAtom.u.m21=newAtom.u.m31=newAtom.u.m32=0.0;
                    }
                    else {
                      Usym(asymmUnit[i].uf,mol.zelle.symmops[n],newAtom.uf);
                      mol.Uf2Uo(newAtom.uf,newAtom.u);
                    }
                    xdinp.append(newAtom);
                  }//ll
            }//packable 
            else if ((asymmUnit.at(i).OrdZahl<0)&&(n==0)) xdinp.append(asymmUnit[i]);//non atoms BCPs, Dummys, etc.
          }//i++
        }//n++
      }//exec
    }//packart==7
  
  }

  smx=xdinp.size();
  balken->hide();      
  for (int i=0; i<smx; i++){
    mol.frac2kart(xdinp[i].frac,xdinp[i].kart);
  }

  //int atoms=0;
  /*xs=0;ys=0,zs=0;
    for (int i=0; i<smx; i++){
    if (xdinp[i].OrdZahl>-1){
    atoms++;
    xs+=xdinp[i].kart.x;
    ys+=xdinp[i].kart.y;
    zs+=xdinp[i].kart.z;
    }
    }*/
  for (int i=0;i<smx;i++){
    if (xdinp[i].OrdZahl<0){
      xdinp[i].u.m11=xdinp[i].u.m22=xdinp[i].u.m33=xdinp[i].u.m12=xdinp[i].u.m13=xdinp[i].u.m23=xdinp[i].u.m21=xdinp[i].u.m31=xdinp[i].u.m32=0.0;
    }
  }

  /*  xs/=(atoms);ys/=(atoms);zs/=(atoms);

      for (int i=0; i<smx; i++){
      xdinp[i].kart.x-=xs;
      xdinp[i].kart.y-=ys;
      xdinp[i].kart.z-=zs;
      }*/
  //  printf("growSymm line %d\n",__LINE__);
  if (!george){
    //  printf("growSymm line %d\n",__LINE__);
    for (int i=0; i<asymmUnit.size() ;i++)
      if (xdinp[i].OrdZahl>-1){
        // printf("%s %d %d %d %s %s\n",asymmUnit.at(i).atomname,asymmUnit[i].nax,asymmUnit[i].nay2,asymmUnit[i].nay1,asymmUnit[asymmUnit[i].nax-1].atomname,asymmUnit[asymmUnit[i].nay2-1].atomname);
        if ((asymmUnit[i].nax<1)||(asymmUnit[i].nax-1>xdinp.size()))qDebug()<<"There is something wrong with the local coordinate systems!"<<asymmUnit[i].atomname;
        if ((asymmUnit[i].nay2<1)||(asymmUnit[i].nay2-1>xdinp.size()))qDebug()<<"There is something wrong with the local coordinate systems!"<<asymmUnit[i].atomname;
        xdinp[i].ax1=xdinp[asymmUnit[i].nax-1].kart-xdinp[i].kart;
        xdinp[i].ax1=Normalize(xdinp[i].ax1);
        xdinp[i].ax2=xdinp[asymmUnit[i].nay2-1].kart-xdinp[i].kart;
        xdinp[i].ax2=Normalize(xdinp[i].ax2);
        xdinp[i].ax3= xdinp[i].ax1 %xdinp[i].ax2;
        xdinp[i].ax3=Normalize(xdinp[i].ax3);
        xdinp[i].ax2= xdinp[i].ax3 %xdinp[i].ax1;
        xdinp[i].ax2=Normalize(xdinp[i].ax2);

        int probe = xdinp[i].icor1-xdinp[i].icor2;
        probe=(probe<0)?(probe+3)%2:probe%2;
        if (probe) xdinp[i].lflag*=-1;
        xdinp[i].ax3*=(double)xdinp[i].lflag;
      }
  }
  //  printf("growSymm line %d\n",__LINE__);
  V3 uz0f,uz1f,uz2f,uz3f,uz4f,uz5f,uz6f,uz7f;
  uz0f.x=0.0;  uz0f.y=0.0;  uz0f.z=0.0;
  uz1f.x=1.0;  uz1f.y=0.0;  uz1f.z=0.0;
  uz2f.x=0.0;  uz2f.y=1.0;  uz2f.z=0.0;
  uz3f.x=0.0;  uz3f.y=0.0;  uz3f.z=1.0;
  uz4f.x=1.0;  uz4f.y=1.0;  uz4f.z=0.0;
  uz5f.x=1.0;  uz5f.y=0.0;  uz5f.z=1.0;
  uz6f.x=0.0;  uz6f.y=1.0;  uz6f.z=1.0;
  uz7f.x=1.0;  uz7f.y=1.0;  uz7f.z=1.0;
  if (cubeGL->moliso!=NULL) cubeGL->moliso->orig=Vector3(0,0,0);
  mol.frac2kart(uz0f,mol.uz0k);
  mol.frac2kart(uz1f,mol.uz1k);
  mol.frac2kart(uz2f,mol.uz2k);
  mol.frac2kart(uz3f,mol.uz3k);
  mol.frac2kart(uz4f,mol.uz4k);
  mol.frac2kart(uz5f,mol.uz5k);
  mol.frac2kart(uz6f,mol.uz6k);
  mol.frac2kart(uz7f,mol.uz7k);
  //printf("%g,%g %g\n",mol.uz4k.x,mol.uz4k.y,mol.uz4k.z);
  if ((Norm(atom1Pos)>0)||(Norm(atom2Pos)>0)){
    Matrix OM;
    V3 r1,r2,r3,t1,t2,t3;
    double l1,l2,kl1,kl2;
    r1=atom1Pos-atom2Pos;
    r2=atom3Pos-atom2Pos;
    l1=Norm(r1);
    l2=Norm(r2);
    r3=r1%r2;
    r2=r3%r1;

    r1=Normalize(r1);
    r2=Normalize(r2);
    r3=Normalize(r3);
    int idx1=0,idx2=1,idx3=2;

    double kl1min=20000,kl2min=20000;
    while (((kl1=fabs(l1-Norm(xdinp[idx2].kart-xdinp[idx1].kart)))>0.002)&&(idx1<atmax)) {
      kl1min=(kl1<kl1min)?kl1:kl1min;
      idx2++;
      idx1=(idx2%atmax)?idx1:idx1+1;
      idx2%=atmax;
    }
    kl1min=(kl1<kl1min)?kl1:kl1min;

    if (kl1>0.002){
      V3 rrr=xdinp[0].kart-atom1Pos;
      if (cubeGL->moliso!=NULL) cubeGL->moliso->orig=Vector3(rrr.x,rrr.y,rrr.z);
      for (int ii=0; ii<xdinp.size(); ii++)
        printf("%-8s %12.5f  %12.5f  %12.5f\n",xdinp.at(ii).atomname,
            xdinp.at(ii).kart.x+cubeGL->moliso->orig.x,
            xdinp.at(ii).kart.y+cubeGL->moliso->orig.y,
            xdinp.at(ii).kart.z+cubeGL->moliso->orig.z);
      printf("Atom1 %12.5f %12.5f %12.5f\n",atom1Pos.x,atom1Pos.y,atom1Pos.z);
      printf("Atom2 %12.5f %12.5f %12.5f\n",atom2Pos.x,atom2Pos.y,atom2Pos.z);
      printf("Atom3 %12.5f %12.5f %12.5f\n",atom3Pos.x,atom3Pos.y,atom3Pos.z);
      qDebug()<<kl1<<"Can not find the first three atoms of the grid-file in file. Please check that these files have to be from the same refinement.";
      exit(0);
    }
    while (((kl2=fabs(l2-Norm(xdinp[idx2].kart-xdinp[idx3].kart)))>0.002)&&(idx3<atmax-1)) {
      kl2min=(kl2<kl2min)?kl2:kl2min;
      idx3++;
      idx3%=atmax;
    }
    if ((kl1>0.002)||(kl2>0.002)) {
      qDebug()<<kl1<<kl2<<"Can not find the first three atoms of the grid-file in file. Please check that these files have to be from the same refinement."<<idx1<<idx2;
      exit(0);
    }
    t1=xdinp[idx1].kart-xdinp[idx2].kart;
    t2=xdinp[idx3].kart-xdinp[idx2].kart;
    t3=t1%t2;

    t2=t3%t1;

    t1=Normalize(t1);
    t2=Normalize(t2);
    t3=Normalize(t3);

    Matrix RR=Matrix(r1,r2,r3);
    Matrix TT=Matrix(t1,t2,t3);

    OM=RR*transponse(TT);
    for(int i=0; i<smx;i++){
      r1=OM*xdinp[i].kart;
      xdinp[i].kart=r1;

      r1=OM*xdinp[i].ax1;
      xdinp[i].ax1=r1;

      r1=OM*xdinp[i].ax2;
      xdinp[i].ax2=r1;

      r1=OM*xdinp[i].ax3;
      xdinp[i].ax3=r1;
    }

    V3 rrr=xdinp[idx1].kart-atom1Pos;
    if (cubeGL->moliso!=NULL) cubeGL->moliso->orig=Vector3(rrr.x,rrr.y,rrr.z);

    for(int i=0; i<smx;i++){
      Usym(xdinp[i].u,OM,xdinp[i].u);
    }

    r1=OM*mol.uz0k;
    mol.uz0k=r1;

    r1=OM*mol.uz1k;
    mol.uz1k=r1;

    r1=OM*mol.uz2k;
    mol.uz2k=r1;

    r1=OM*mol.uz3k;
    mol.uz3k=r1;

    r1=OM*mol.uz4k;
    mol.uz4k=r1;

    r1=OM*mol.uz5k;
    mol.uz5k=r1;

    r1=OM*mol.uz6k;
    mol.uz6k=r1;

    r1=OM*mol.uz7k;
    mol.uz7k=r1;

    //mol.uz0k=mol.uz0k+cubeGL->moliso->orig;
    //mol.uz1k=mol.uz1k+cubeGL->moliso->orig;
    //mol.uz2k=mol.uz2k+cubeGL->moliso->orig;
    //mol.uz3k=mol.uz3k+cubeGL->moliso->orig;
    //mol.uz4k=mol.uz4k+cubeGL->moliso->orig;
    //mol.uz5k=mol.uz5k+cubeGL->moliso->orig;
    //mol.uz6k=mol.uz6k+cubeGL->moliso->orig;
    //mol.uz7k=mol.uz7k+cubeGL->moliso->orig;
  }

  double dim=dimension(xdinp);
  if ((asymmUnit.isEmpty())&&(!masymmUnit.isEmpty()))  dim=mdimension(matoms);
  //  if ((Norm(atom1Pos)==0)&&(Norm(atom2Pos)==0)) cubeGL->L=100.0/dim;
  if (cubeGL->moliso==NULL) cubeGL->L=100.0/dim; 
  //qDebug()<<dim<<cubeGL->L;
  /*if (mol.nListe>2) {
    free(mol.vL);
    mol.vL=NULL;
    mol.nListe=0;
    }*/

  //if ((!fck)&&(!fastrun)) mol.findChains(xdinp);
  /*if (mol.nListe>2) {
    mol.vL=mol.smoothPoints(mol.vL,mol.nListe);
    mol.vL=mol.smoothPoints(mol.vL,mol.nListe);
    mol.vL=mol.smoothPoints(mol.vL,mol.nListe);
    }
    */
  //  printf("growSymm line %d\n",__LINE__);
  cubeGL->resetENV();
  if ((!asymmUnit.isEmpty())&&(masymmUnit.isEmpty())) initLists(xdinp);
  else {cubeGL->bas=0;printf("keine listen! N%d M%d\n",asymmUnit.isEmpty(),masymmUnit.isEmpty());}
  //  printf("growSymm line %d\n",__LINE__);
  dock->hide();
  dock2->hide();
  cubeGL->setVisible(true);
  dock2->show();
  dock->show();
  dock->setMaximumWidth(width()/5);
  printf("growSymm %d milliseconds with packart = %d\n",speedTest.restart(),packart);
  mol.distfix(asymmUnit);
}

void MyWindow::changeModScal(double d){
  mol.modscal=d;
  modScale->setSingleStep(d/5);
  cubeGL->setFocus(Qt::OtherFocusReason);
}

void MyWindow::polyColorIng(bool b){
  mol.polyShapColor=b;
  cubeGL->updateGL();
}
