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
#include <locale.h>
int rev=357;
int atmax,smx,dummax,egal;
V3 atom1Pos,atom2Pos,atom3Pos;
QList<INP> xdinp,oxd,asymmUnit;
molekul mol;
int pdfOnAtom=-1;
const double g2r=180.0/M_PI;
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

  packAct->setVisible(true);
  mol.zelle.phi=  sqrt(1-(cos(mol.zelle.al/g2r)*cos(mol.zelle.al/g2r))-
		  (cos(mol.zelle.be/g2r)*cos(mol.zelle.be/g2r))-(cos(mol.zelle.ga/g2r)*cos(mol.zelle.ga/g2r))
		  +2*cos(mol.zelle.al/g2r)*cos(mol.zelle.be/g2r)*cos(mol.zelle.ga/g2r));
  mol.zelle.V = mol.zelle.a*mol.zelle.b*mol.zelle.c*mol.zelle.phi;
  mol.zelle.as=mol.zelle.c*mol.zelle.b*sin(mol.zelle.al/g2r)/mol.zelle.V;
  mol.zelle.bs=mol.zelle.c*mol.zelle.a*sin(mol.zelle.be/g2r)/mol.zelle.V;
  mol.zelle.cs=mol.zelle.a*mol.zelle.b*sin(mol.zelle.ga/g2r)/mol.zelle.V;
  const double tau=mol.zelle.c*((cos(mol.zelle.al/g2r)-cos(mol.zelle.be/g2r)*cos(mol.zelle.ga/g2r))/sin(mol.zelle.ga/g2r));
  mol.zelle.o1.m11=mol.zelle.o[0][0] =mol.zelle.as*mol.zelle.a;
  mol.zelle.o1.m12=mol.zelle.o[0][1] = 0.0;
  mol.zelle.o1.m13=mol.zelle.o[0][2] = 0.0;
  mol.zelle.o1.m21=mol.zelle.o[1][0] = mol.zelle.bs*mol.zelle.b*cos(mol.zelle.ga/g2r);
  mol.zelle.o1.m22=mol.zelle.o[1][1] = mol.zelle.bs*mol.zelle.b*sin(mol.zelle.ga/g2r);
  mol.zelle.o1.m23=mol.zelle.o[1][2] = 0.0;
  mol.zelle.o1.m31=mol.zelle.o[2][0] = mol.zelle.cs*mol.zelle.c* cos(mol.zelle.be/g2r);
  mol.zelle.o1.m32=mol.zelle.o[2][1] = mol.zelle.cs*tau;
  mol.zelle.o1.m33=mol.zelle.o[2][2] = mol.zelle.cs*mol.zelle.c* mol.zelle.phi / sin(mol.zelle.ga /g2r);
}
char *egals;
QProgressBar *balken;
#define iabs(a) (a>0)?(a):(-a) 

MyWindow::MyWindow( QMainWindow *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags)  {
  QMainWindow::setDockOptions(QMainWindow::AnimatedDocks|QMainWindow::AllowTabbedDocks|QMainWindow::ForceTabbedDocks|QMainWindow::VerticalTabs);
  //  DOCK

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
  cubeGL =new CubeGL;
  cubeGL->bas=0;
  scalePic=1.0;
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
  QMenu *invariomMenu = new QMenu(tr("In&varioms"), this);
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
  {
    QAction *a=xdMenu->addAction("Edit xd_part.aux ",this,SLOT(editPartAux()));
  a->setData(0);
  }
  xdMenu->addAction(resinpAct); 
  xdMenu->addAction(xdlsmAct); 
  xdMenu->addAction(xdpropAct); 
  xdMenu->addAction(tidyCPSAct); 
  xdMenu->addSeparator();
  xdMenu->addAction("Find all uniq peaks in Fo-Fc map > iso surface value.",this,SLOT(addMoreQPeaks())) ;
  xdMenu->addAction("Calculate pdf of an atom.",this,SLOT(pdfDlg()));
  xdMenu->addAction("Calculate pdf of all atoms.",this,SLOT(pdfDlg2()));
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

  seReAct->setVisible(false);
  menuBar()->addMenu(fileMenu);
  menuBar()->addMenu(workMenu);   
  menuBar()->addMenu(viewMenu);
  menuBar()->addMenu(dialogMenu);
  menuBar()->addMenu(invariomMenu);
  menuBar()->addMenu(xdMenu);
  menuBar()->addMenu(MolIsoMenu);
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
  QAction *printAct        = new QAction(tr("Print"),this);
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
  togLuft                  = new QAction(tr("Toggle helices"), this);
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
			    tr("&Save"),this,SLOT(saveScene()),
			    QKeySequence(tr("S","File|Save")));
  act1->setWhatsThis("Click here to <b>save a screenshot</b> of your stucture. Currently <b>MoleCoolQt</b> can save <b>JPG BMP PPM</b> and <b>PNG</b> files. "
		     "You can scale the resolution of the stored images by a factor. This factor can be chaged in the 'Settings' menu. "
		     "A scale factor of 1 result in a image in screen resulution. ");
  printAct->setStatusTip("Print on the printer");
  printAct->setShortcut(tr("Ctrl+P"));
  printAct->setIcon(QIcon(":/images/printer.png"));
  printAct->setWhatsThis("Click here to <b>print</b> your stucture on your printer. The printed picture is scaled to fit the page."
			 "You can chage the orientation in the print dialog.");
  wasistdasAct->setStatusTip("Shows some explanations to some buttons.");  
  wasistdasAct->setShortcut(tr("Shift+F1"));		   
  wasistdasAct->setIcon(QIcon(":/images/whatsthis.png"));
  wasistdasAct->setWhatsThis("Click here to get some <b>helping</b> hints about the functionality of some of the Buttons."
			     " Please click again here and then on the Button you are interested in.");
  fileMenu->addAction(printAct);
  helpMenu->addAction(wasistdasAct);
  fileMenu->addSeparator();
  for (int i = 0; i < MaxRecentFiles; ++i)
    fileMenu->addAction(recentFileActs[i]);
  fileMenu->addSeparator();
  fileMenu->addAction(createmoliso);
  fileMenu->addAction(noMoliso);
  ldipAct = fileMenu->addAction(QIcon(":images/dipole.png"),tr("Load &Dipole moments"),this,SLOT(openDipoleFile()),QKeySequence(tr("D", "File|Load &Dipole moments"))); 
  ldipAct->setWhatsThis("<img src=\":images/dipole.png\"> This loads dipole moments from a file.<br>\
The first three numbers in a row are taken as xyz of the dipole vector.<br>\
You can also specify acolor as RGB after ## or as in HTML after color= in &quot;&quot; here is a example:<br>\
<pre>8.0 -4.0 3.0 \nxd_abbild:  px =      11.0      py =       0.5       pz =       9.2   ## 0.5 0.5 0.0\ngaussian: 6.8986     1.7407     6.1785        color=\"red\"\nInvtoolpx =       7.5  py =      -1.1  pz =       5.6     color=\"#ed0ed0\"\n6.8986     1.7407     6.1785 origin={-4.0,-4.0,-4.0}</pre>");

  nodipAct = fileMenu->addAction(QIcon(":images/nodipole.png"),tr("Remove Dipole moments"),this,SLOT(removeDipoleMoments())); 
  nodipAct->setVisible ( false);
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
  QMenu *packMenu=new QMenu("Pack the moleule");
  QAction *dntpck=packMenu->addAction("Only asymmetric unit");
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
  packAct                  = new QAction(QIcon(":/images/compmol.png"),tr("Show pack dialog "),this);
  packAct->setStatusTip("Pack the molecule in the unit cell");
  packAct->setMenu(packMenu);
  packAct->setVisible(false);
  connect(packAct,SIGNAL(triggered()),this,SLOT(showPackDlg()));
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
/*
  QAction *sma =new QAction(this);
  sma->setShortcut(tr(";"));
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
  connect( togLuft, SIGNAL(toggled(bool) ),
	   cubeGL, SLOT(setHelices(bool) ));
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
  connect(printAct,SIGNAL(triggered()),
	  this,SLOT(printScene()));
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
  workMenu->addAction(mveLeftAct );
  workMenu->addAction(mveRightAct);
  workMenu->addAction(mveUpAct   );
  workMenu->addAction(mveDownAct );
  workMenu->addAction(ydlStartAct);
  workMenu->addAction(ydlStopAct);  
  workMenu->addAction(fontSizeUpAct);
  workMenu->addAction(fontSizeDownAct);
  workMenu->addAction(matrixAct);
  workMenu->addAction(viewAlong010);
  workMenu->addAction(viewAlong100);
  workMenu->addAction(viewAlong001);
  workMenu->setTearOffEnabled(true);
  viewMenu->addAction(atomClickAct);
  viewMenu->addAction(togAtom);
  viewMenu->addAction(togBond);
  viewMenu->addAction(togLabel);
  viewMenu->addAction(togAxen);
  viewMenu->addAction(togUnit);
  viewMenu->addAction(togElli);

  viewMenu->addAction(togHBond);
  viewMenu->addAction(togLuft); 
  viewMenu->addAction(depthCueingAct);
  viewMenu->addSeparator();
  viewMenu->addAction(dock->toggleViewAction());
  viewMenu->addAction(togStat);
  viewMenu->addAction(matrixAct);
  viewMenu->addAction(viewAlong010);
  viewMenu->addAction(viewAlong100);
  viewMenu->addAction(viewAlong001);
  dialogMenu->addAction(backColor);        
  dialogMenu->addAction(back_Grad);
  dialogMenu->addAction(donTGrow);
  dialogMenu->addAction(labelColor);
  dialogMenu->addAction(labelFont);
  dialogMenu->addAction(scalePicAct);
  dialogMenu->addAction(changeProbAct);
  dialogMenu->addAction(editAtomAct);
  dialogMenu->addAction(filterAct);
  dialogMenu->addAction(unfilterAct);
  dialogMenu->addAction(tr("Open map control"),this,SLOT(openMapControl()));
  dialogMenu->addSeparator();
  dialogMenu->addAction("Save current toggel states",cubeGL,SLOT(saveMISettings()));
  dialogMenu->addAction("Load toggle states",cubeGL,SLOT(loadMISettings()));
  cubeGL->Istda=invariomMenu->addAction("No data base loded yet.");
  invariomMenu->addAction(invExpAct);
  invariomMenu->addAction(invEdiAct);
  invariomMenu->addAction(invLoadAct);
  invariomMenu->addAction("Load latest Invariom-Data-Base from internet",this , SLOT(howOldIsTheLatesDataBase()));
    
  (void*) statusBar ();
  sLabel = new QLabel;
  sLabel->setFrameStyle(QFrame::Panel  );
  sLabel->setLineWidth(2);
  sLabel->setText("MolecoolQt a viewing program for molecular strutures");
  balken = new QProgressBar(this);
  balken->hide();
  statusBar()->addPermanentWidget(balken);
  statusBar()->addPermanentWidget(speedSldr);
  statusBar()->addPermanentWidget(sLabel);
  
  statusBar()->setWhatsThis("This is the status bar. You can hide it in the View menu by unchecking 'toggle Status bar'.");
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
  cubeGL->clearSelection=a=tb->addAction("Unselect selected atoms",cubeGL,SLOT(disSelection()));
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
  toolFile->addAction(printAct);
  toolFile->addAction(wasistdasAct);
  toolFile->addAction(act3);
  toolFile->addAction(ldipAct);
  toolFile->addAction(nodipAct);

  toolFile->setWhatsThis("This is the file tool bar. You can move it to any window border if you want."
			 " By clicking right on the menu or tool bar region, you can toggel the tool bars.");

  toolSettings = addToolBar(tr("Settings"));
  toolSettings->setIconSize(QSize(23,23));
  toolSettings->setMovable(true);
  toolSettings->addAction(editAtomAct);
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
  toolSettings->addAction(packAct);

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
  toolView->addAction(togLuft);     
  toolView->addAction(togHBond);
  toolView->addAction(showface);
  toolView->addAction(showLeg);

  fmcq = new FourMCQ(&mol,cubeGL,toolView);
  /////////////
  {
  QDialogButtonBox *buttonBoxMC = new QDialogButtonBox( QDialogButtonBox::Apply | QDialogButtonBox::Cancel);
  QPushButton *applyMC = buttonBoxMC->button(QDialogButtonBox::Apply);
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
  
  searchBar = addToolBar(tr("Search"));
  searchBar->setIconSize(QSize(23,23));
  searchBar->setMovable(true);
  searchAtomEdit = new QLineEdit(this);
  searchAtomEdit->setMaxLength(20);
  searchAtomCancel = searchBar->addAction(QIcon(":/images/cancel.png"),"hide Search", searchBar , SLOT(hide())); 

  searchBar->addWidget(searchAtomEdit);
  searchBar->addAction(QIcon(":/images/search.png"),"search",this,SLOT(findAtoms()));
  connect(searchAtomEdit,SIGNAL(returnPressed ()),this,SLOT(findAtoms())); 
  searchAtomButton = toolView->addAction(QIcon(":/images/search.png"),"Search",searchBar,SLOT(show()));

  searchBar->hide(); 
  connect(searchBar,SIGNAL(visibilityChanged(bool )),this,SLOT(hideSearchAtomButton(bool )));
  chargeGroups->setVisible(false);
  chargeGroups->setEnabled(false);
  toolView->setWhatsThis("This is the view tool bar. You can move it to any window border if you want."
                         " By clicking right on the menu or tool bar region, you can toggel the toolbars.");
  settings = new QSettings( QSettings::IniFormat, QSettings::UserScope ,"Christian_B._Huebschle","MoleCoolQt" );
  settings->beginGroup("Version 0.1");
  dirName = settings->value("lastFile").toString();

  QStringList files = settings->value("recentFileList").toStringList();  
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MyWindow *mainWin = qobject_cast<MyWindow *>(widget);
    if (mainWin)
      mainWin->updateRecentFileActions();
  }

  QSize size =
    settings->value("size", sizeHint()).toSize();
  QPoint pos =
    settings->value("position").toPoint();
  
  mol.tubifiedAtoms=settings->value("tubes").toBool();
  tubiAct->setChecked(mol.tubifiedAtoms);
  mol.singleColorBonds=settings->value("singleColorBonds").toBool();   
  singleColorAct->setChecked(mol.singleColorBonds);
  cubeGL->tCR= (float) settings->value("text_color_red"  ).toDouble();
  cubeGL->tCG= (float) settings->value("text_color_green").toDouble();
  cubeGL->tCB= (float) settings->value("text_color_blue" ).toDouble();
  cubeGL->tCA= (float) settings->value("text_color_alpha").toDouble();
  cubeGL->bgCR=(float) settings->value("background_color_red"  ).toDouble();
  cubeGL->bgCG=(float) settings->value("background_color_green").toDouble();
  cubeGL->bgCB=(float) settings->value("background_color_blue" ).toDouble();
  cubeGL->bgCA=(float) settings->value("background_color_alpha").toDouble();
  cubeGL->checkTC();

  cubeGL-> MM[0]= (float)  settings->value("Matrix00").toDouble();
  cubeGL-> MM[1]= (float)  settings->value("Matrix01").toDouble();
  cubeGL-> MM[2]= (float)  settings->value("Matrix02").toDouble();
  cubeGL-> MM[3]= (float)  settings->value("Matrix03").toDouble();
  cubeGL-> MM[4]= (float)  settings->value("Matrix04").toDouble();
  cubeGL-> MM[5]= (float)  settings->value("Matrix05").toDouble();
  cubeGL-> MM[6]= (float)  settings->value("Matrix06").toDouble();
  cubeGL-> MM[7]= (float)  settings->value("Matrix07").toDouble();
  cubeGL-> MM[8]= (float)  settings->value("Matrix08").toDouble();
  cubeGL-> MM[9]= (float)  settings->value("Matrix09").toDouble();
  cubeGL->MM[10]= (float)  settings->value("Matrix10").toDouble();
  cubeGL->MM[11]= (float)  settings->value("Matrix11").toDouble();
  cubeGL->MM[12]= (float)  settings->value("Matrix12").toDouble();
  cubeGL->MM[13]= (float)  settings->value("Matrix13").toDouble();
  cubeGL->MM[14]= (float)  settings->value("Matrix14").toDouble();
  cubeGL->MM[15]= (float)  settings->value("Matrix15").toDouble();
  QVariant variant;
  if (settings->contains("bondColor")){
  variant = settings->value("bondColor");
  mol.bondColor=variant.value<QColor>();
  }else{mol.bondColor=QColor("silver");} 
  if ((settings->value("bondStrength").toDouble())>0.001) mol.bondStrength=settings->value("bondStrength").toDouble();
  if (settings->contains("LabelFont")){
    variant=settings->value("LabelFont");
    cubeGL->myFont=variant.value<QFont>();}
  else{
    cubeGL->myFont=QFont("Arial",24, -1, false);
  }
  if (settings->contains("MolisoLegendFont")){
    variant=settings->value("MolisoLegendFont");
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

   int acsize = settings->beginReadArray("AtomColors");   
   for (int i = 0; i < acsize; ++i) {
     settings->setArrayIndex(i);
     mol.Acol[i-1][0] = (GLfloat)settings->value("red"   ).toDouble();
     mol.Acol[i-1][1] = (GLfloat)settings->value("green" ).toDouble();
     mol.Acol[i-1][2] = (GLfloat)settings->value("blue"  ).toDouble();
     mol.Acol[i-1][3] = (GLfloat)settings->value("alpha" ).toDouble();
   }
   settings->endArray();
   acsize = settings->beginReadArray("AtomStyles");   
   for (int i = 0; i < acsize; ++i) {
     settings->setArrayIndex(i);
     mol.aStyle[i-1] = settings->value("Style").toInt();
   }
   settings->endArray();
   acsize = settings->beginReadArray("CovaleceRadii");   
   for (int i = 0; i < acsize; ++i) {
     settings->setArrayIndex(i);
     mol.Kovalenz_Radien[i-1] = settings->value("Radius").toInt();
   }
   settings->endArray();
  cubeGL->invertMouseZoom->setChecked( settings->value("InvertMouseZoom").toBool());
  bool b;
  if (settings->contains("BackgroundGradient")){
    back_Grad->setChecked(b=settings->value("BackgroundGradient").toBool());
    cubeGL->togglBGG(b);
  }
  if (settings->contains("UnitCell")){
    togUnit->setChecked(b=settings->value("UnitCell").toBool());
    cubeGL->setUnitCell(b);
  }
  settings->endGroup();
  move( pos);
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
  setWindowTitle(QString("MoleCoolQt-Revision %1 ").arg(rev));
  fastrun=false;
  int argc=QCoreApplication::arguments().size();
  if (argc>1){
    cubeGL->updateGL();
    QString fnam;
    int argui=1;
    double gd=-1;
    for (int i=argui;i<argc;i++){
    if ((QCoreApplication::arguments().at(i).contains(".res")) ||
      (QCoreApplication::arguments().at(i).contains(".pdb")) ||
      (QCoreApplication::arguments().at(i).contains(".ent")) ||
      (QCoreApplication::arguments().at(i).contains(".mas")) ||
      (QCoreApplication::arguments().at(i).contains(".inp")) ||
      (QCoreApplication::arguments().at(i).contains(QRegExp(".\\d\\d$"))) ||
      (QCoreApplication::arguments().at(i).contains(".ins")) ||
      (QCoreApplication::arguments().at(i).contains(".par")) ||
      (QCoreApplication::arguments().at(i).contains(".com")) ||
      (QCoreApplication::arguments().at(i).contains(".fchk",Qt::CaseInsensitive)) ||
      (QCoreApplication::arguments().at(i).contains(".cif")) ) fnam= QCoreApplication::arguments().at(i);
      if (QCoreApplication::arguments().at(i).contains("-GrowDist")) {
	i++;	
	if (i<=argc) gd=QCoreApplication::arguments().at(i).toDouble(); else gd=-1;
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
      if (QCoreApplication::arguments().at(i).contains("-HADist")) {
	i++;	
	if (i<=argc) mol.HAMax=QCoreApplication::arguments().at(i).toDouble(); 
      }
      if (QCoreApplication::arguments().at(i).contains("-HAAngle")) {
	i++;	
	if (i<=argc) mol.HAWink=QCoreApplication::arguments().at(i).toDouble();
      }
    }
    if(!fnam.isEmpty()){
    fnam.replace("\\","/");
      if (!fnam.contains("/")){
	fnam.prepend("/");
	fnam.prepend(QDir::currentPath());
      }
      loadFile(fnam,gd);
      update();

    }

  }
  net = new QNetworkAccessManager(this);
  QString Pfad = settings->fileName();
  Pfad=Pfad.section('/',0,-2);
  Pfad.append("/DABA.txt");
  if (QFileInfo(Pfad).exists()) cubeGL->loadDataBase(Pfad);

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
  mapSchnitt->setCurrentIndex(fmcq->maptrunc);
  md->show();
}

void MyWindow::doMapsNow(bool b){
  if (!b) {
    fmcq->killmaps(); 
    cubeGL->updateGL();
    return;
  } else {
    QString fouName;
    fouName=dirName;
    fouName.chop(3);
    fouName.append("fou");
    if (fmcq->loadFouAndPerform(fouName.toStdString().c_str()),false){
      infoKanalNews(QString("<font color=red>Could not load %1!</font><br>").arg(fouName));
      fmcq->deleteLists();
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
    fouName=dirName;
    fouName.chop(3);
    fouName.append("fou");
    if (fmcq->loadFouAndPerform(fouName.toStdString().c_str()),false){
      infoKanalNews(QString("<font color=red>Could not load %1!</font><br>").arg(fouName));
      fmcq->deleteLists();

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

void MyWindow::createRenameWgd(){
  ///RENAME DOCK
  renamDock = new QDockWidget("Rename Mode");
  renamDock->setObjectName("Rename");
  renamDock->hide();
  enterRenameMode = renamDock->toggleViewAction ();
  connect(enterRenameMode,SIGNAL(triggered(bool)),this,SLOT(renamUpdate(bool)));
  connect(renamDock,SIGNAL(visibilityChanged(bool)),this,SLOT(renamUpdate(bool)));
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
  connect(indexSpin,SIGNAL(valueChanged(int )),this,SLOT(changeIndexofLabel(int ))); 
  sufixBox = new QComboBox();
  QLabel *sufixL =new QLabel("Suffix");
  QStringList alpha;
  alpha<<"'"<<""<<"A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"G"<<"H"<<"I"<<"J"<<"K"
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

void MyWindow::genMoliso() {
  atmax=0;
  smx=0;
  xdinp.clear();
  asymmUnit.clear();
  cubeGL->pause=true;
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
  connect(cubeGL->moliso,SIGNAL(bigmessage(const QString &)),this,SLOT(infoKanalNews(const QString&)));
  cubeGL->moliso->L=cubeGL->L;
  cubeGL->moliso->orig=Vector3(0.0,0.0,0.0);
  QString isof,mapf,lfaceFile,sfaceFile,adpName;
  MolisoStartDlg *msgdlg = new MolisoStartDlg();
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
  GradDlg *grdlg = new GradDlg(cubeGL->moliso );
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
  zebraBox->setChecked(true);
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
  mt->setShortcut(tr("T"));
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
  zla->addWidget(mlf);
  zla->addWidget(fos);
  zla->addWidget(savset);
  zla->addWidget(lodset);
  zla->addStretch(999);
  connect(cubeGL,SIGNAL(mconf()),this,SLOT(syncMconf()));
  cubeGL->togglContours(true);
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
  cubeGL->moliso->readXDGridHeader(isof);
  mol.adp=0;
  cubeGL->L = 100.0/dimension(asymmUnit);
  cubeGL->moliso->L=cubeGL->L;
  if (smx>7){
  atom1Pos=asymmUnit[0].kart;
  atom2Pos=asymmUnit[1].kart;
  atom3Pos=asymmUnit[2].kart;
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
    togLuft->setVisible(false);
    togAxen->setEnabled (true );
    togUnit->setEnabled (true );
  packAct->setVisible(false);
  mol.zelle.a=1.0;
  mol.zelle.b=1.0;
  mol.zelle.c=1.0;
  mol.zelle.al=90.0;
  mol.zelle.be=90.0;
  mol.zelle.ga=90.0;
  xdinp=asymmUnit;
  cubeGL->resetENV();
  initLists(xdinp);
  }
  else {
    statusBar()->showMessage(tr("loading sructure coordinates...") );
  updateStatusBar();
    setCursor(Qt::BusyCursor);
    loadFile(adpName);
    cubeGL->moliso->L=cubeGL->L;

  }
  if ((lfaceFile.isEmpty())||(lfaceFile.contains('!'))) {
    if (sfaceFile.contains('!'))sfaceFile.clear();
    setCursor(Qt::BusyCursor);
    statusBar()->showMessage(tr("calculating surfaces") );
    cubeGL->moliso->createSurface(isof,mapf,sfaceFile);
    statusBar()->showMessage(tr("surfaces calculatied" ) );
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
    legendSize->setValue(cubeGL->milsize*30);
    strikesSldr->setValue(512-cubeGL->cdens);
    swidthSldr->setValue((cubeGL->cwid+1)*256/cubeGL->cdens);   
    zebraBox->setChecked(cubeGL->zebra);
    mt->setChecked(cubeGL->molisoTransparence);
  togAxen->setChecked(cubeGL->drawAx );
  togUnit->setChecked(cubeGL->drawUz );
  back_Grad->setChecked(cubeGL->back_grad );
  togAtom->setChecked(cubeGL->drawAt );
  togElli->setChecked(cubeGL->elli );
  togBond->setChecked(cubeGL->drawBo );
  togLabel->setChecked(cubeGL->drawLa );
  togHBond->setChecked(cubeGL->drawHb );
  togLuft->setChecked(cubeGL->Luftschlange );
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
  asymmUnit.clear();
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
  if(QDialog::Accepted==xdmasdlg->exec()){
    mas.open(QIODevice::WriteOnly|QIODevice::Text);
    mas.write(editor->toPlainText().toLatin1(),editor->toPlainText().length());
    mas.close();
    delete editor;
  }else
    if (editor->document()->isModified()){
  if ((QMessageBox::question(this,"Save changes to xd.mas?","Save changes to xd.mas?",QMessageBox::Save|QMessageBox::Discard)==QMessageBox::Save)){
    mas.open(QIODevice::WriteOnly|QIODevice::Text);
    mas.write(editor->toPlainText().toLatin1(),editor->toPlainText().length());
    mas.close();
    delete editor;
  }
  } 
  delete highlighter;
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

void MyWindow::about(){  
  QString date="$LastChangedDate: 2010-12-01 19:15:18 +0100 (Mi, 01 Dez 2010)$";
  date.remove("LastChangedDate:");
  date.remove("$");
  QString bau_datum=QString(__TIME__ " " __DATE__);
  QString openGLVersion=QString("OpenGL Version %1").arg((char *)glGetString(GL_VERSION));

  QMessageBox::about(this,tr("About MoleCoolQt"),
	      tr("<b>MoleCoolQt is written by Dr. Christian B. H&uuml;bschle, University of G&ouml;ttingen, Germany</b><p>"
		 "Please visit the following web sites: <a href=\"http://wwwuser.gwdg.de/~chuebsc/dittrich/index.html\">"
		 "Dr. Birger Dittrich's Work Group</a> and "
		 "<a href=\"http://www.molecoolqt.de\">MoleCoolQt site</a><p>"
		 "If you find bugs, typos or have any suggestions then contact me under <a href=\"mailto:chuebsc@gwdg.de\">chuebsc@gwdg.de</a>"
                 "<p> This is Revision %1 from: %2 <br> The Version of Qt used is: %3.<br>%4<p>This program was build: %5").arg(rev).arg(date).arg(qVersion ()).arg(openGLVersion).arg(bau_datum));

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

void MyWindow::changeADP(){
  bool ok;
  int ad,d=50;
  QAction *action = qobject_cast<QAction *>(sender());
  if (action) d=action->data().toInt();
  if (d==0) d=QInputDialog::getInteger(this, "Set ADP probability",tr("Percentage:"), mol.proba, 10, 90, 20, &ok);
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
}

void MyWindow::changeBondStrength(){
bool ok;
double d=mol.bondStrength*100.0;
d=QInputDialog::getDouble(this,"Change Bond Strength","thicknes in pm:" ,mol.bondStrength*100.0,1.00,90,4,&ok);
if (ok) {mol.bondStrength=d/100.0;
  initLists(xdinp);
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
		    (!QString(supo.at(i)).contains(QRegExp("[A-Z]"))))
      formats+=QString("%1-file (*.%1)%2").arg(QString(supo.at(i))).arg(((i+1)<supo.size())?";;":"");
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
  if (!fileName.isEmpty()){
    cubeGL->myFont.setPointSize(c);
    cubeGL->MLegendFont.setPointSize(g);
    cubeGL->noWaitLabel=true;
    QPixmap   map = cubeGL->renderPixmap(a,b);
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
  cubeGL->noWaitLabel=false;
  cubeGL->updateGL();
}

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

void Uf2Uo(const Matrix x, Matrix & y) {
 y=(mol.zelle.o1*x)*transponse(mol.zelle.o1);
}

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

void MyWindow::editAtomColor(){
  EacDlg eac;          
  extern molekul mol;
  if ( eac.exec()==QDialog::Accepted){
    mol.bonds_made=0;
    mol.knopf_made=0;
    cubeGL->resetENV();
    initLists(xdinp);
    cubeGL->updateGL();
  }

}

void MyWindow::load_fchk(QString fileName){
  asymmUnit.clear();
  cubeGL->setVisible ( false );
  char PSE_Symbol[109][3] = {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar",
			     "K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr",
			     "Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","J","Xe",
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
  packAct->setVisible(false);
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
  if (mol.nListe>2) {
    free(mol.vL);
    mol.vL=NULL;
    mol.nListe=0;
  }
  cubeGL->resetENV();
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
                    gitt=tok.at(2).toAscii()[0];
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

                            Uf2Uo(newAtom.uf,newAtom.u);/*
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

void MyWindow::howOldIsTheLatesDataBase(){
   connect(net, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
   reply = net->get(QNetworkRequest(QUrl("http://ewald.ac.chemie.uni-goettingen.de/___mole_cool_qt__/latestDABA.php")));
}

void MyWindow::replyFinished(QNetworkReply* antwort){
  QString a=antwort->readAll();
  QDateTime t=  QDateTime::fromString (a,Qt::ISODate);
  int d=t.daysTo(QDateTime::currentDateTime ());
  //QMessageBox::information(this,"DA DA DIE DABA DIE IS DA!",QString("Die neuste Datenbank ist %1 alt.").arg(d)); 
  antwort->close();
  antwort->deleteLater();
  disconnect(net,SIGNAL(finished(QNetworkReply*)),0,0);
  if ((d>1)&&(d<365)) {
    connect(net, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished2(QNetworkReply*)));
    reply = net->get(QNetworkRequest(QUrl("http://ewald.ac.chemie.uni-goettingen.de/___mole_cool_qt__/loadDABA.php")));
  }
}

void MyWindow::replyFinished2(QNetworkReply* antwort){
  QString a=antwort->readAll();
  //
  QString Pfad = settings->fileName();
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
//$filename="/user/birger/NEUE_Datenbank/ToolZ/DABA.txt";
//
void MyWindow::load_xdres(QString fileName) {
  INP newAtom;
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

  while (strstr(line,"SCAT")==NULL) {egal=fscanf(mas,"%[^\n\r]\n\r",line);}
  while (strstr(line,"END")==NULL){  
    egal=fscanf(mas,"%[^\n\r]\n\r",line);
    if ((isalpha(line[0]))&&((strstr(line,"END")==NULL))) {

      atypen.append(strtok(line," "));
      pserbt[mol.Get_OZ(atypen.last())+1]->show();
      if (mol.Get_OZ(atypen.last())==5) pserbt[mol.Get_OZ(atypen.last())+1]->setChecked(true); 
    }
  }
  sfacBox->setFixedSize((atypen.size()+1)*52,70);
  rewind(mas);
  chargeGroups->clear();
  chargeGroups->addItem("Charge Group 1 (all)",0);
  chargeGroups->setVisible(false);
  chargeGroups->setEnabled(false);
  while (!feof(mas)) {
    egal=fscanf(mas,"%[^\n\r]\n\r",line);
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
	if (strstr(tok,"GROUP")==NULL)
//	  printf("%d->%s<-\n",gnr,tok);
	ladungsgruppen[gnr].append(tok);
      }
      //printf("==>%s<== %c %d %d \n",gns,ll[5],(line[5]>47),(line[5]<59));
    }
    
  }
  fclose(mas);
  }
  FILE *adp;
  double XDVERS=0;
  {//RES  
  char line[85]="",dv[20],*dvv;
  int i=0;
  double cfac=1.0;//6/(M_PI*M_PI*M_PI*8);
  mol.initDir();
  if ((adp=fopen(fileName.toLocal8Bit(),"r"))==NULL) {QMessageBox::critical(this,"Read Error!",QString("read error %1!").arg(fileName),QMessageBox::Ok);exit(2);}  
  i=0;
  if (!fastrun) cubeGL->drawAx=true;
  while ((!feof(adp))&&(NULL==strstr(line,"Revision"))) {
    egal=fscanf(adp,"%[^\n\r]\n\r",line);
 }
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
      strcpy(dv,asymmUnit[i].atomname);
      strtok(dv,"(1234567890+- ");
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
	
	asymmUnit[i].c111/=cfac;
	asymmUnit[i].c222/=cfac;
	asymmUnit[i].c333/=cfac;
	asymmUnit[i].c112/=cfac;
	asymmUnit[i].c122/=cfac;
	asymmUnit[i].c113/=cfac;
	asymmUnit[i].c133/=cfac;
	asymmUnit[i].c223/=cfac;
	asymmUnit[i].c233/=cfac;
	asymmUnit[i].c123/=cfac;
      } else{
	asymmUnit[i].c111/=mol.zelle.as*mol.zelle.as*mol.zelle.as*cfac;////Ujkl's whanted
	asymmUnit[i].c222/=mol.zelle.bs*mol.zelle.bs*mol.zelle.bs*cfac;//
	asymmUnit[i].c333/=mol.zelle.cs*mol.zelle.cs*mol.zelle.cs*cfac;//
	asymmUnit[i].c112/=mol.zelle.as*mol.zelle.as*mol.zelle.bs*cfac;//
	asymmUnit[i].c122/=mol.zelle.as*mol.zelle.bs*mol.zelle.bs*cfac;//
	asymmUnit[i].c113/=mol.zelle.as*mol.zelle.as*mol.zelle.cs*cfac;//
	asymmUnit[i].c133/=mol.zelle.as*mol.zelle.cs*mol.zelle.cs*cfac;//
	asymmUnit[i].c223/=mol.zelle.bs*mol.zelle.bs*mol.zelle.cs*cfac;//
	asymmUnit[i].c233/=mol.zelle.bs*mol.zelle.cs*mol.zelle.cs*cfac;//
	asymmUnit[i].c123/=mol.zelle.as*mol.zelle.bs*mol.zelle.cs*cfac;//
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
      } else{
	asymmUnit[i].d1111/= mol.zelle.as * mol.zelle.as * mol.zelle.as * mol.zelle.as;////Ujklm's whanted
	asymmUnit[i].d2222/= mol.zelle.bs * mol.zelle.bs * mol.zelle.bs * mol.zelle.bs;//
	asymmUnit[i].d3333/= mol.zelle.cs * mol.zelle.cs * mol.zelle.cs * mol.zelle.cs;//
	asymmUnit[i].d1112/= mol.zelle.as * mol.zelle.as * mol.zelle.as * mol.zelle.bs;//
	asymmUnit[i].d1222/= mol.zelle.as * mol.zelle.bs * mol.zelle.bs * mol.zelle.bs;//
	asymmUnit[i].d1113/= mol.zelle.as * mol.zelle.as * mol.zelle.as * mol.zelle.cs;//
	asymmUnit[i].d1333/= mol.zelle.as * mol.zelle.cs * mol.zelle.cs * mol.zelle.cs;//
	asymmUnit[i].d2223/= mol.zelle.bs * mol.zelle.bs * mol.zelle.bs * mol.zelle.cs;//
	asymmUnit[i].d2333/= mol.zelle.bs * mol.zelle.cs * mol.zelle.cs * mol.zelle.cs;//
	asymmUnit[i].d1122/= mol.zelle.as * mol.zelle.as * mol.zelle.bs * mol.zelle.bs;//
	asymmUnit[i].d1133/= mol.zelle.as * mol.zelle.as * mol.zelle.cs * mol.zelle.cs;//
	asymmUnit[i].d2233/= mol.zelle.bs * mol.zelle.bs * mol.zelle.cs * mol.zelle.cs;//
	asymmUnit[i].d1123/= mol.zelle.as * mol.zelle.as * mol.zelle.bs * mol.zelle.cs;//
	asymmUnit[i].d1223/= mol.zelle.as * mol.zelle.bs * mol.zelle.bs * mol.zelle.cs;//
	asymmUnit[i].d1233/= mol.zelle.as * mol.zelle.bs * mol.zelle.cs * mol.zelle.cs;//
      }
   }
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
  {//xd_rho.cps   
    QString cpsName=masName;
    cpsName.chop(6);
    cpsName.append("xd_rho.cps");
    char cptp[10],_line[120]="Rho",dummystr[18];
    FILE *cps;
    int idxx=0,idx;
    if (NULL!=(cps=fopen(cpsName.toLocal8Bit(),"r"))){
      while ((strstr(_line,"Rho"))&&(!feof(cps))) {egals=fgets(_line,92,cps);idxx++;}
     idxx--;
      rewind(cps);
      for (int i=0;i<idxx;i++){
        egals=fgets(_line,92,cps);
	if ((6==sscanf(_line,"%d %s %*19c %lf  %lf  %lf  %lf",&idx,cptp,
		      &newAtom.kart.x,  
		      &newAtom.kart.y,
		      &newAtom.kart.z,
		      &newAtom.peakHeight))||
	    (5==(sscanf(_line,"%d %s %*19c %lf  %lf  %lf",&idx,cptp,
				&newAtom.kart.x,  
				&newAtom.kart.y,
				&newAtom.kart.z)))){
	 
	sprintf(dummystr,"CP%d",idx);
	strncpy(newAtom.atomname,dummystr,18);
	newAtom.OrdZahl=-2;
        mol.kart2frac(newAtom.kart,newAtom.frac);
	asymmUnit.append(newAtom);
        }
      }
      egals=fgets(_line,92,cps);
if (!feof(cps)) 
  for (int i=0; i <idxx;i++) {
    egals=fgets(_line,92,cps);
    sscanf(_line,"%*d %*s %*s %lf %lf %lf",
		    &asymmUnit[smx+i].uf.m21,&asymmUnit[smx+i].uf.m22,&asymmUnit[smx+i].uf.m23);
    egals=fgets(_line,92,cps);                                                       
    sscanf(_line,"%*s %lf %lf %lf",                                                  
		    &asymmUnit[smx+i].uf.m31,&asymmUnit[smx+i].uf.m32,&asymmUnit[smx+i].uf.m33);
    egals=fgets(_line,92,cps);                                                       
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
  }
dummax=smx-atmax;
  for (int i=0;i<asymmUnit.size();i++) {
    if ((asymmUnit[i].uf.m22==0.0)&&(asymmUnit[i].uf.m33==0.0)){
      //printf("%-8s \n",asymmUnit[i].atomname);
      asymmUnit[i].u.m11=asymmUnit[i].u.m22=asymmUnit[i].u.m33=asymmUnit[i].uf.m11;
      asymmUnit[i].u.m12=asymmUnit[i].u.m13=asymmUnit[i].u.m23=asymmUnit[i].u.m21=asymmUnit[i].u.m31=asymmUnit[i].u.m32=0.0;
    }
    else {

      Uf2Uo(asymmUnit[i].uf,asymmUnit[i].u);
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

  QDialog *dlg=new QDialog(this);
  dlg->setWindowTitle("Probability Density Function");
  QComboBox *atomBx = new QComboBox(this);
  for (int i=0; i<xdinp.size();i++){
  if (xdinp[i].OrdZahl<0)continue;
  if (xdinp[i].sg) continue;
  atomBx->addItem(QString(xdinp[i].atomname),i);
  }
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
  la->addWidget(atomBx,1,1,1,2);
  la->addWidget(l,2,1);
  la->addWidget(proba,2,2,1,2);
  la->addWidget(sec,3,1);
  la->addWidget(third,3,2);
  la->addWidget(fourth,3,3);
  la->addWidget(buttonBox,4,1,1,3);
  dlg->setLayout(la);
  if(dlg->exec()==QDialog::Accepted){
    makePDFGrid(xdinp[atomBx->itemData(atomBx->currentIndex()).toInt()],proba->value(),sec->isChecked(),third->isChecked(),fourth->isChecked());
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
  cubeGL->moliso->createSurface(fac,(c2)?proba:0.01);
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

void MyWindow::addfaze(const QString afac,const QString fac,const QString nface){
  QFile ONE(afac);
  QFile TWO(fac);
  QFile of(nface);

  ONE.open(QIODevice::ReadOnly|QIODevice::Text);
  TWO.open(QIODevice::ReadOnly|QIODevice::Text);
  of.open(QIODevice::WriteOnly|QIODevice::Text);
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
  printf("Piso= %g\n",piso);
  QString fac("testpdf.face");
  tensmul(atom);

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
  int breite=91;
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
  for (int k=0; k<breite; k++)
      for (int j=0; j<breite; j++)
          for (int i=0; i<breite; i++){
      X=V3(i*df,j*df,k*df)+t;
      ponent=((X*UI)*X)/-2.0;
      V3 w=(X*UI);

      p=base*exp(ponent);
      if ((c3)&&(atom.jtf>2)) third= p*hermite3(w,UI,
                      atom.c111,atom.c222,atom.c333,
                      atom.c112,atom.c122,atom.c113,
                      atom.c133,atom.c223,atom.c233,
                      atom.c123);
      if ((c4)&&(atom.jtf==4)) fourth= p*hermite4(w,UI,
		       atom.d1111,atom.d2222,atom.d3333,
		       atom.d1112,atom.d1113,atom.d1122,
		       atom.d1123,atom.d1133,atom.d1222,
                       atom.d1223,atom.d1233,atom.d1333,
		       atom.d2223,atom.d2233,atom.d2333);
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
  cubeGL->moliso->createSurface(fac,(c2)?proba:0.01);
  
  cubeGL->pause=true;
  if (cubeGL->moliso->mibas) glDeleteLists(cubeGL->moliso->mibas,6);
  cubeGL->moliso->mibas=glGenLists(6);
  statusBar()->showMessage(tr("loading surfaces...") );
  //qDebug()<<cubeGL->moliso->mibas<<cubeGL->foubas[0];
//  printf("L= %g   %g %g %g %s\n",cubeGL->moliso->L, cubeGL->moliso->orig.x, cubeGL->moliso->orig.y, cubeGL->moliso->orig.z,fac.toStdString().c_str());
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
  const char bfl[83][5]={"ACTA", "AFIX", "MPLA", "ANIS", "BASF", "BIND", "BLOC", "BOND", "BUMP", "CELL", "CGLS", "CHIV", "CONF", "CONN", "DAMP", "DANG", //0-15
			 "DEFS", "DELU", "DFIX", "DISP", "EADP", "EGEN", "END",  "EQIV", "ESEL", "EXTI", "EXYZ", "FEND", "FLAT", "FMAP", "FRAG", "FREE", //16-31
			 "FVAR", "GRID", "HFIX", "HKLF", "HOPE", "HTAB", "INIT", "ISOR", "LAST", "LATT", "LAUE", "LIST", "L.S.", "MERG", "MOLE", "MORE", //32-47
			 "MOVE", "NCSY", "OMIT", "PART", "PATT", "PHAN", "PHAS", "PLAN", "PSEE", "REM",  "RESI", "RTAB", "SADI", "SAME", "SFAC", "SHEL", //48-63
			 "SIMU", "SIZE", "SPEC", "SPIN", "STIR", "SUMP", "SWAT", "SYMM", "TEMP", "TEXP", "TIME", "TITL", "TREF", "TWIN", "UNIT", "VECT", //64-79
			 "WPDB", "WGHT", "ZERR"};//80-82
  int i=0;
  for (size_t j=0; j<strlen(command); j++) command[j] = toupper( command[j] );
  while ((i<83)&&(strcmp(command,bfl[i]))) i++;
  if (i==83) i=-1;
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
  FILE *adp; 
  part=0;
  char line[85],line2[85],llin[166],dv[50],dvv[50],command[8],ext[85],Ami3[5];
  int acnt=0,sorte=0,gitter=0,bfl,sftr[85],resNr=-1;
  int sf;
  double fvar[20],Uiso=0.05; 
  dummax=0; 
  atmax=1; 
  mol.zelle.symmops.clear();
  mol.zelle.trans.clear();
  V3 nl(0,0,0);
  mol.zelle.trans.append(nl);
  mol.zelle.symmops.append(Matrix(1,0,0, 0,1,0, 0,0,1));
  if ((adp=fopen(fn,"r"))==NULL) {qDebug("Can't open %s!!!\n",fn);exit(2);}
  while (!feof(adp)) { 
    egals=fgets(line,83,adp);
    command[0]='\0';
    sscanf(line,"%[a-zA-Z.] %[^\n]",command,ext); 
    bfl=isacommand(command);
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
	sf=0;
	char tch[83],*tok;
	strcpy(tch,ext);
	for (tok=strtok(tch, " \n\t\r"); tok; tok=strtok(NULL, " \n\t\r")) {      
	  if (strlen(tok)>1) tok[1]=tolower(tok[1]); 
	  printf("[%s]",tok);
	  sftr[sf]=mol.Get_OZ(tok);
	  sf++;
	}
	printf("\n");
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
	  line[79]=' ' ;
	  line[78]=' ' ;
          egals=fgets(line2,82,adp);
	} 
	else line2[0]='\0';
	sprintf(llin,"%s%s",line,line2);
	if (line2[0]){
	  sscanf(line,"%s %d %lf %lf %lf %lf %lf %lf",
		 newAtom.atomname,
		 &newAtom.atomtype,
		 &newAtom.frac.x,
		 &newAtom.frac.y,
		 &newAtom.frac.z,
		 &newAtom.amul,
		 &newAtom.uf.m11,
		 &newAtom.uf.m22);
	  sscanf(line2,"%lf %lf %lf %lf",
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
	}else {
	  int pda=sscanf(line,"%s %d %lf %lf %lf %*f %lf %lf",
			 newAtom.atomname,
			 &newAtom.atomtype,
			 &newAtom.frac.x,
			 &newAtom.frac.y,
			 &newAtom.frac.z,
			 &newAtom.uf.m11,
                         &newAtom.peakHeight);
	  if (pda==7){
	    mol.pmin=(mol.pmin>newAtom.peakHeight)?newAtom.peakHeight:mol.pmin;
	    mol.pmax=(mol.pmax<newAtom.peakHeight)?newAtom.peakHeight:mol.pmax;
	  }

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
	if (!strcmp(dv,"Q")) dummax++;
	else atmax++;
	asymmUnit.append(newAtom);
	acnt++;
      }
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
    else Uf2Uo(asymmUnit[i].uf,asymmUnit[i].u);
  }
  growSymm(6);
}//shelx

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

int hatlokale=0;
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
  if (tag=="_atom_site_adp_type") {if (v=="Uani") anisIndex.append(loopat);return;}
  if (tag=="_atom_site_type_symbol") {
      if (ola!=loopat) asymmUnit.append(newAtom);
      asymmUnit[loopat].OrdZahl=mol.Get_OZ(v.toStdString().c_str());
      ola=loopat;
      return;}
  if (tag=="_atom_site_aniso_label") {
      if (asymmUnit[anisIndex.at(loopat)].atomname!=v) qDebug()<<"Serious Problem!"<<asymmUnit[loopat].atomname<<v;
      ola=loopat; return;}
  if (tag=="_atom_site_aniso_U_11") {mol.adp=1;v.remove(QRegExp("\\(\\d+\\)"));asymmUnit[anisIndex.at(loopat)].uf.m11=v.toDouble();return;}
  if (tag=="_atom_site_aniso_U_22") {mol.adp=1;v.remove(QRegExp("\\(\\d+\\)"));asymmUnit[anisIndex.at(loopat)].uf.m22=v.toDouble();return;}
  if (tag=="_atom_site_aniso_U_33") {mol.adp=1;v.remove(QRegExp("\\(\\d+\\)"));asymmUnit[anisIndex.at(loopat)].uf.m33=v.toDouble();return;}
  if (tag=="_atom_site_aniso_U_12") {mol.adp=1;v.remove(QRegExp("\\(\\d+\\)"));asymmUnit[anisIndex.at(loopat)].uf.m21=asymmUnit[anisIndex.at(loopat)].uf.m12=v.toDouble();return;}
  if (tag=="_atom_site_aniso_U_13") {mol.adp=1;v.remove(QRegExp("\\(\\d+\\)"));asymmUnit[anisIndex.at(loopat)].uf.m31=asymmUnit[anisIndex.at(loopat)].uf.m13=v.toDouble();return;}
  if (tag=="_atom_site_aniso_U_23") {mol.adp=1;v.remove(QRegExp("\\(\\d+\\)"));asymmUnit[anisIndex.at(loopat)].uf.m32=asymmUnit[anisIndex.at(loopat)].uf.m23=v.toDouble();return;}
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
    else Uf2Uo(asymmUnit[i].uf,asymmUnit[i].u);
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
  Matrix u;   /*Cholesky decomposition of theReal space Metric tensor Wird fÃ¼r die Umrechnung von fraktionellen in kartesischen Korrdinaten benÃ¶tigt.*/ 
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
	sprintf(newAtom.atomname,"%s(%d)",newAtom.atomname,atmax+dummax);
	if (newAtom.OrdZahl>-1) atmax++; else dummax++;
	asymmUnit.append(newAtom);
      }
      else {
	k=sscanf(line,"%s %d %s %d %s %d %s",newAtom.atomname,&b[bi],bname[bi],&a[ai],aname[ai],&d[di],dname[di]);
	switch (k) {
		case 7: di++;
		case 5: ai++;
		case 3: bi++;
		case 1: i++;
			asymmUnit.append(newAtom);
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
    smx=atmax+dummax;mol.nListe=0;
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

  packAct->setVisible(false);
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
  if (mol.nListe>2) {
    free(mol.vL);
    mol.vL=NULL;
    mol.nListe=0;
  }
  cubeGL->resetENV();
  initLists(xdinp);
  cubeGL->setVisible ( true );
}

void MyWindow::openFile() {
  QString fileName;
  cubeGL->drawUz=true;
  cubeGL->drawAx=false;
  removeDipoleMoments();
  QString selectedFilter;
  fileName = QFileDialog::getOpenFileName(this, tr("Open stucture file "), dirName,
		  "XD-Files (*.res *.inp *.mas);;"
		  "SHELX-Files (*.res *.ins);;"
                  "MoPro-Files (*.0* *.1* *.2*);;"
		  "Gaussian COM-Files (*.com);;"
		  "Gaussian FChk-Files (*.fchk);;"
		  "CIF-Files (*.cif);;"
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
  packdlg->accept();
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
  dntpck->setDefault (true);/*
			       molpck->setAutoRaise(true);
			       cctpck->setAutoRaise(true);
			       ccmpck->setAutoRaise(true);
			       grdpck->setAutoRaise(true);*/
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

void MyWindow::loadFile(QString fileName,double GD){//empty
  cubeGL->pause=true;
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
  atmax=0;
  smx=0;
  xdinp.clear();
  asymmUnit.clear();
  george=false;
  if (!same) seReAct->setEnabled(false);
  if (!same) seReAct->setVisible(false);
  if (!same) xdMenu->setEnabled(false);
  fck=false;
  if (!same) togLuft->setVisible(false);
  if (!same) togAxen->setEnabled (true );
  if (!same) togUnit->setEnabled (true );
  QDir::setCurrent ( fileName.left(fileName.lastIndexOf("/") ))  ;
  statusBar()->showMessage(QString(tr("Loading %1").arg(fileName)) );
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
  if ((fileName.endsWith(".com",Qt::CaseInsensitive))) {
    load_gaus(fileName);
    cubeGL->drawAx=false;
    cubeGL->drawUz=false;
    mol.adp=0;
    togAxen->setEnabled (false );
    togUnit->setEnabled (false );
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
    egal=fscanf(xxx,"%[^\n\r]\n\r",test);
    fclose(xxx);
    if (strcmp(test,"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")) 
    { 
      load_sheldrick(fileName);}
    else    
      load_xdres(fileName);    
    togAxen->setChecked ( cubeGL->drawAx );
  }
  if ((fileName.endsWith(".par"))||(fileName.contains(QRegExp(".\\d\\d$")))){
      cubeGL->setEllipsoidNoUpdate( true );
      togElli->setChecked ( true );
      togElli->setVisible ( true );
      cubeGL->drawUz=true;
      cubeGL->drawAx=true;
      togAxen->setEnabled (true );
      togAxen->setChecked ( cubeGL->drawAx );
      load_MoPro(fileName);
  }
  updateStatusBar();
  QStringList aSymmList;
  for (int i=0;i<asymmUnit.size();i++) {
    aSymmList.append(asymmUnit.at(i).atomname);
  }
  QCompleter *cc = new QCompleter(aSymmList,this);
  searchAtomEdit->setCompleter(cc);
  statusBar()->showMessage(tr("File succesfully loaded.") );
  // Zuletzt geffnete File setzen
  settings->beginGroup("Version 0.1");
  settings->setValue("lastFile", fileName );
  QStringList files = settings->value("recentFileList").toStringList();
  files.removeAll(fileName);//alle identischen "fileName" aus der Liste entfernen
  files.prepend(fileName);// und dann vorne anfuegen...
  while (files.size() > MaxRecentFiles)
    files.removeLast();//hinten abschneiden was lnger als MaxRecentFiles ist
  settings->setValue("recentFileList", files);  
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MyWindow *mainWin = qobject_cast<MyWindow *>(widget);
    if (mainWin)
      mainWin->updateRecentFileActions();
  }
  
  if (settings->contains("UnitCell")){
    bool b;
    togUnit->setChecked(b=settings->value("UnitCell").toBool());
    cubeGL->setUnitCell(b);
  }
  settings->endGroup();
  filtered=0;
  cubeGL->pause=false;
  cubeGL->updateGL();
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


void MyWindow::initLists(QList<INP> xd){
  dFilter->setVisible(true);
  hFilter->setVisible(true);
 bool singleColorBonds_state = mol.singleColorBonds;
 bool tubifiedAtoms_state=mol.tubifiedAtoms;
  int adpstate=mol.adp;
  int mx=xd.size();
  statusBar()->showMessage(tr("Draw Atoms.") );	


  for (int j=0;j<mx;j++)               
    xd[j].labPos=xd[j].kart;

  if (cubeGL->bas) glDeleteLists(cubeGL->bas,10);
  cubeGL->bas=glGenLists(10);


  glNewList(cubeGL->bas, GL_COMPILE );{                          //ATOME
    glPushMatrix();{
      glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
      mol.intern=1;
      mol.atoms(xd,mol.proba);
    }glPopMatrix();    
  }glEndList();



  glNewList(cubeGL->bas+8, GL_COMPILE );{       //bonds in single color
    glPushMatrix();{
      glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
      mol.intern=1;
      mol.singleColorBonds=true;
      mol.bonds(xd);
    }glPopMatrix();    
  }glEndList();


  glNewList(cubeGL->bas+7, GL_COMPILE );{                          //ATOME
    glPushMatrix();{
      glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
      mol.intern=0;
      mol.atoms(xd,mol.proba);
    }glPopMatrix();    
  }glEndList();


  glNewList(cubeGL->bas+4, GL_COMPILE );{                          //ATOME
    glPushMatrix();{
      glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
      mol.adp=0;
      mol.atoms(xd,50);
    }glPopMatrix();    
  }glEndList();

  glNewList(cubeGL->bas+9, GL_COMPILE );{       //Atome fuer tube syle
    glPushMatrix();{
      glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
      mol.intern=1;
      mol.tubifiedAtoms=true;
      mol.atoms(xd,mol.proba);
    }glPopMatrix();    
  }glEndList();
  mol.tubifiedAtoms=false;


  glNewList(cubeGL->bas+1, GL_COMPILE );{                          //BONDS
    glPushMatrix();{
      glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
      mol.adp=0;      
      mol.singleColorBonds=false;
      mol.bonds(xd);
    }glPopMatrix();    
  }glEndList();


  mol.adp=adpstate;

  if (cubeGL->drawAx) {

    statusBar()->showMessage(tr("Draw local coordinate systems.") );	
    togAxen->setVisible(true);
    glNewList(cubeGL->bas+2, GL_COMPILE );{                          //Axen
      glDisable( GL_DEPTH_TEST ); 
      glPushMatrix();{
	glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
	mol.axes(xd); 
      }glPopMatrix();    
      glEnable( GL_DEPTH_TEST );
    }glEndList();
  }
  else togAxen->setVisible(false);
  togUnit->setVisible(packAct->isVisible());
  if (packAct->isVisible()) {

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
  if (mol.nListe>2){
    togLuft->setVisible(true);
    statusBar()->showMessage(tr("Draw helices.") );	
    glNewList(cubeGL->bas+6, GL_COMPILE );{        
      glPushMatrix();
      glScaled( cubeGL->L, cubeGL->L, cubeGL->L );
      mol.drawSline(mol.vL,mol.nListe);
      glPopMatrix();
    }glEndList();
  }
  for (int i=0; i<mx; i++)
    xdinp[i]=xd[i];
  smx=atmax=mx;
  mol.tubifiedAtoms=tubifiedAtoms_state;
  mol.singleColorBonds = singleColorBonds_state;
}

void MyWindow::updateRecentFileActions() {
  QSettings settings(QSettings::IniFormat,  QSettings::UserScope ,"Christian_B._Huebschle", "MoleCoolQt" );
  settings.beginGroup("Version 0.1");
  QStringList files = settings.value("recentFileList").toStringList();

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


  settings.endGroup();
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
  mol.singleColorBonds=b;
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
   time->setNumDigits(text.size()+1);
   time->display(text);

}

void MyWindow::closeEvent(QCloseEvent *event)  {
   settings->beginGroup("Version 0.1");
   settings->setValue("size", size() );
   settings->setValue("position", pos() );
   settings->setValue("text_color_red",cubeGL->tCR);
   settings->setValue("text_color_green",cubeGL->tCG);
   settings->setValue("text_color_blue",cubeGL->tCB);
   settings->setValue("text_color_alpha",cubeGL->tCA);
   settings->setValue("background_color_red",cubeGL->bgCR);
   settings->setValue("background_color_green",cubeGL->bgCG);
   settings->setValue("background_color_blue",cubeGL->bgCB);
   settings->setValue("background_color_alpha",cubeGL->bgCA);
   settings->setValue("Matrix00",cubeGL-> MM[0]);
   settings->setValue("Matrix01",cubeGL-> MM[1]);
   settings->setValue("Matrix02",cubeGL-> MM[2]);
   settings->setValue("Matrix03",cubeGL-> MM[3]);
   settings->setValue("Matrix04",cubeGL-> MM[4]);
   settings->setValue("Matrix05",cubeGL-> MM[5]);
   settings->setValue("Matrix06",cubeGL-> MM[6]);
   settings->setValue("Matrix07",cubeGL-> MM[7]);
   settings->setValue("Matrix08",cubeGL-> MM[8]);
   settings->setValue("Matrix09",cubeGL-> MM[9]);
   settings->setValue("Matrix10",cubeGL->MM[10]);
   settings->setValue("Matrix11",cubeGL->MM[11]);
   settings->setValue("Matrix12",cubeGL->MM[12]);
   settings->setValue("Matrix13",cubeGL->MM[13]);
   settings->setValue("Matrix14",cubeGL->MM[14]);
   settings->setValue("Matrix15",cubeGL->MM[15]);
   settings->setValue("bondColor",mol.bondColor);
   settings->setValue("bondStrength", mol.bondStrength);
   settings->setValue("tubes",mol.tubifiedAtoms);
   settings->setValue("singleColorBonds",mol.singleColorBonds);   
   settings->setValue("LabelFont",cubeGL->myFont);
   settings->setValue("MolisoLegendFont",cubeGL->MLegendFont);
   settings->beginWriteArray("AtomColors",107);   
   for (int i = 0; i < 107; ++i) {
     settings->setArrayIndex(i);
     settings->setValue("red",   mol.Acol[i-1][0]);
     settings->setValue("green", mol.Acol[i-1][1]);
     settings->setValue("blue",  mol.Acol[i-1][2]);
     settings->setValue("alpha", mol.Acol[i-1][3]);
   }
   settings->endArray();
   settings->beginWriteArray("AtomStyles",107);   
   for (int i = 0; i < 107; ++i) {
     settings->setArrayIndex(i);
     settings->setValue("Style", mol.aStyle[i-1]);
   }
   settings->endArray();
   settings->beginWriteArray("CovaleceRadii",107);   
   for (int i = 0; i < 107; ++i) {
     settings->setArrayIndex(i);
     settings->setValue("Radius", mol.Kovalenz_Radien[i-1]);
   }
   settings->endArray();
   settings->setValue("InvertMouseZoom",(cubeGL->invertMouseZoom->checkState()==Qt::Checked));
   settings->setValue("BackgroundGradient",(back_Grad->isChecked()));//TOGGLE STATES
   settings->setValue("UnitCell",(togUnit->isChecked()));//TOGGLE STATES
   settings->endGroup();
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
  qDebug()<<"MyWindow::makeXDPartAux";
  if (QFile::exists("xd_part.aux")){
    readXDPartAux();
    return;
  }
  if ((!asymmUnit.isEmpty())&&(atmax)){
    const double TOLERANZ = 0.0005;
    QStringList befehle;
    befehle  <<"ACTA" << "AFIX" << "MPLA" << "ANIS" << "BASF" << "BIND" << "BLOC" << "BOND" << "BUMP" << "CELL" << "CGLS" << "CHIV" << "CONF" << "CONN" << "DAMP" << "DANG" <<
			 "DEFS" << "DELU" << "DFIX" << "DISP" << "EADP" << "EGEN" << "END" <<  "EQIV" << "ESEL" << "EXTI" << "EXYZ" << "FEND" << "FLAT" << "FMAP" << "FRAG" << "FREE" << 
			 "FVAR" << "GRID" << "HFIX" << "HKLF" << "HOPE" << "HTAB" << "INIT" << "ISOR" << "LAST" << "LATT" << "LAUE" << "LIST" << "L.S." << "MERG" << "MOLE" << "MORE" << 
			 "MOVE" << "NCSY" << "OMIT" << "PART" << "PATT" << "PHAN" << "PHAS" << "PLAN" << "PSEE" << "REM" <<  "RESI" << "RTAB" << "SADI" << "SAME" << "SFAC" << "SHEL" << 
			 "SIMU" << "SIZE" << "SPEC" << "SPIN" << "STIR" << "SUMP" << "SWAT" << "SYMM" << "TEMP" << "TEXP" << "TIME" << "TITL" << "TREF" << "TWIN" << "UNIT" << "VECT" << 
			 "WPDB" << "WGHT" << "ZERR";
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
    growSymm(5,cubeGL->expandatom);
    cubeGL->updateGL();
  }
}

inline int trindex(int i,int j){
  int I=(i>j)?i:j;
  int J=(i<j)?i:j;
  I++;J++;
  return (I*(I+1)/2-I+J-1);
}

void MyWindow::SDM(QStringList &brauchSymm,int packart){
  // George Sheldrick Seminar ideas
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

void MyWindow::growSymm(int packart,int packatom){
  QTime speedTest;
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
  if (!george) makeXDPartAux();
  xdinp=asymmUnit;
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
	    prime=mol.zelle.symmops.at(n) * asymmUnit.at(i).frac + mol.zelle.trans.at(n) + V3(0.005,0.005,0.005);
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
	  newAtom.molindex = asymmUnit[i].molindex;
	  if ((asymmUnit[i].u.m12==0.0)&&(asymmUnit[i].u.m23==0.0)&&(asymmUnit[i].u.m13==0.0)){
	    newAtom.u.m11=newAtom.u.m22=newAtom.u.m33=asymmUnit[i].uf.m11;
	    newAtom.u.m12=newAtom.u.m13=newAtom.u.m23=newAtom.u.m21=newAtom.u.m31=newAtom.u.m32=0.0;}
	  else {
	    Usym(asymmUnit[i].uf,mol.zelle.symmops[s],newAtom.uf);
	    Uf2Uo(newAtom.uf,newAtom.u);}
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
      for (int n=0; n<mol.zelle.symmops.size();n++){
	for (int i=0; i<asymmUnit.size();i++)
	  if ((asymmUnit.at(i).OrdZahl>-1)&&(asymmUnit.at(i).molindex>0)){
	    prime=mol.zelle.symmops.at(n) * asymmUnit.at(i).frac + mol.zelle.trans.at(n) + V3(0.005,0.005,0.005);
	    floorD=V3(floor(prime.x),floor(prime.y),floor(prime.z));
	    prime=prime -floorD;	  
	    dawars=1000.0;
	    for (int g=0; g<xdinp.size();g++){
	      if ((xdinp[i].OrdZahl*xdinp[g].OrdZahl<0)) continue;
	      dl=fl(prime.x-xdinp[g].frac.x, prime.y-xdinp[g].frac.y, prime.z-xdinp[g].frac.z);
	      dawars=(dl<dawars)?dl:dawars;
	    }
	    if (dawars<0.1) continue; // */
	    bs=QString("%1_%2%3%4:%5,").arg(n+1).arg(5-(int)floorD.x).arg(5-(int)floorD.y).arg(5-(int)floorD.z).arg(asymmUnit.at(i).molindex);
	    if  (!brauchSymm.contains(bs)) {
	      brauchSymm.append(bs);
	    }
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
	      Usym(asymmUnit[i].uf,mol.zelle.symmops[n],newAtom.uf);
	      Uf2Uo(newAtom.uf,newAtom.u);
	    }
	    xdinp.append(newAtom);
	    smx++;
	  }
      }
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
	    newAtom.OrdZahl=asymmUnit[i].OrdZahl;
	    newAtom.molindex=asymmUnit[i].molindex;
	    if ((asymmUnit[i].u.m12==0.0)&&(asymmUnit[i].u.m23==0.0)&&(asymmUnit[i].u.m13==0.0)){
	      newAtom.u.m11=newAtom.u.m22=newAtom.u.m33=asymmUnit[i].uf.m11;
	      newAtom.u.m12=newAtom.u.m13=newAtom.u.m23=newAtom.u.m21=newAtom.u.m31=newAtom.u.m32=0.0;}
	    else {
	      Usym(asymmUnit[i].uf,mol.zelle.symmops[s],newAtom.uf);
	      Uf2Uo(newAtom.uf,newAtom.u);}
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
	for (int i=0;i<asymmUnit.size();i++) 
	  if ((asymmUnit[i].molindex==symmgroup)&&(asymmUnit[i].OrdZahl>-1)){ 
	    newAtom.frac=mol.zelle.symmops.at(s)*asymmUnit[i].frac+mol.zelle.trans.at(s)+V3(h,k,l);
	    newAtom.part=asymmUnit[i].part;
	    sprintf(newAtom.atomname,"%s_%d",asymmUnit[i].atomname,j+1);
	  newAtom.sg=1+j;
	    newAtom.OrdZahl=asymmUnit[i].OrdZahl;
	    newAtom.molindex=asymmUnit[i].molindex;
	    if ((asymmUnit[i].u.m12==0.0 )&&(asymmUnit[i].u.m23==0.0)&&(asymmUnit[i].u.m13==0.0)){
	      newAtom.u.m11=newAtom.u.m22=newAtom.u.m33=asymmUnit[i].uf.m11;
	      newAtom.u.m12=newAtom.u.m13=newAtom.u.m23=newAtom.u.m21=newAtom.u.m31=newAtom.u.m32=0.0;}
	    else { 
	      Usym(asymmUnit[i].uf,mol.zelle.symmops[s],newAtom.uf);
	      Uf2Uo(newAtom.uf,newAtom.u);}
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
  if (!george){
    for (int i=0; i<asymmUnit.size() ;i++)
      if (xdinp[i].OrdZahl>-1){
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
    while (((kl2=fabs(l2-Norm(xdinp[idx2].kart-xdinp[idx3].kart)))>0.002)&&(idx3<atmax)) {
      kl2min=(kl2<kl2min)?kl2:kl2min;
      idx3++;
      idx3%=atmax;
    }
    if ((kl1>0.002)||(kl2>0.002)) {
      qDebug()<<kl1<<kl2<<"Can not find the first three atoms of the grid-file in file. Please check that these files have to be from the same refinement.";
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
  if ((Norm(atom1Pos)==0)&&(Norm(atom2Pos)==0)) cubeGL->L=100.0/dim;
  if (mol.nListe>2) {
    free(mol.vL);
    mol.vL=NULL;
    mol.nListe=0;
  }

  if ((!fck)&&(!fastrun)) mol.findChains(xdinp);
  if (mol.nListe>2) {
    mol.vL=mol.smoothPoints(mol.vL,mol.nListe);
    mol.vL=mol.smoothPoints(mol.vL,mol.nListe);
    mol.vL=mol.smoothPoints(mol.vL,mol.nListe);
  }

  cubeGL->resetENV();
  initLists(xdinp);
  dock->hide();
  dock2->hide();
  cubeGL->setVisible(true);
  dock2->show();
  dock->show();
  dock->setMaximumWidth(width()/5);
  printf("growSymm %d milliseconds with packart = %d\n",speedTest.restart(),packart);
}
