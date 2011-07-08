#ifndef MyWindow_H
#define MyWindow_H 1

#include <QtGui>
#include "cubeGL.h"
#include "moliso.h"
#include "fourmcq.h"
#include "psewidget.h"
 class MySortFilterProxyModel : public QSortFilterProxyModel {
     Q_OBJECT

 public:
     MySortFilterProxyModel(QObject *parent = 0);

     double filterMinimum() const { return minVal; }
     void setFilterMinimum(const double &val);
     void setMyIndex(const int &idx);

     double filterMaximum() const { return maxVal; }
     void setFilterMaximum(const double &val);

 protected:
     bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
     bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
 private:
     bool valInRange(const double &val) const;

     double minVal;
     double maxVal;
     int myIndex;
 };

class MyWindow : public QMainWindow {
     Q_OBJECT
public:
     MyWindow( QMainWindow *parent = 0,
               Qt::WindowFlags flags = 0);
     QDockWidget *dock,*dock2,*dock3, *renamDock;
     QTextBrowser *infoKanal,*hbKanal ;
     CubeGL *cubeGL;
     QSlider *speedSldr;
     QAction *act1, *act2, *act3, *act4, *ldipAct, *nodipAct, *enterRenameMode;
     QAction *xdSetupAct;
     QAction *xdRestoreAct;
     QAction *Start;
     QAction *filterAct , *packAct;
     QAction *ignorant,*hFilter,*dFilter;
     QAction *atomClickAct  ;
     QAction *unfilterAct;
     QAction *togElli;
     QAction *togBond, *noBondAct,*singleColorAct; 
     QAction *togAtom, *noAtomAct, *depthCueingAct; 
     QAction *noElli;
     QAction *createmoliso,*noMoliso;
     QAction *showface,*showLeg,*movLeg, *mildir;
     QLabel *sLabel;
     QPushButton *savset, *lodset ;
     QAction *back_Grad, *togLabel,*togHBond;
     QCheckBox *mclmox;
     QLCDNumber *time;
     QToolBar *toolSettings;
     QToolBar *toolFile;
     QToolBar *toolMove;
     QToolBar *toolView;
     QSettings *settings;
     QTimer *idl;
     QAction *ydlStartAct;
     QAction *ydlStopAct;
     QAction *seReAct;
     void growSymm(int packart,int packatom = -1);
     QComboBox *filterColumnComboBox;

public slots:
     void addMoreQPeaks();
     void syncMconf();
     void changeGD();
     void expandAround();
     void showPackDlg();
     void genMoliso();
     void destroyMoliso();
     void toggleTubes(bool b);
     void changeBondStrength();
     void syncBondActs();
     void syncAtomActs();
     void about();
     void reloadXDfiles();
     void reloadFiles();
     void editXDmas();
     void restoreXDfiles();
     void resinp();
     void xdprop();
     void xdlsm();
     void editAtomColor();
     void openFile();
     void openDipoleFile();
     void removeDipoleMoments();
     void openRecentFile();
     void saveScene();
     void printScene();
     void pdfDlg();
     void idleMoveStart();
     void idleMoveStop();
     void changeScalePic();
     void changeADP();
     void zoomIn();
     void zoomOut();
     void rotLeft();
     void rotRight();
     void moveR();
     void moveL();
     void moveU();
     void moveD();
     void rotUp();
     void rotDown();
     void openMapControl();
     void doMapsNow(bool b);
     void controlMap();
     void wasistdas();
     void togglSt();
     void toggleElli(bool b);
     void toggleNoElli(bool b);
     void toggleSingleColorBond(bool b);
     void xdenv();
     void findNext();
     void findText();
     void findPrev();
     void tidyCPS();
     void updateStatusBar();
     void updateStatusBar(const QString& s) ;
     void infoKanalNews(const QString& s); 
     void updateTime();
     void closeEvent(QCloseEvent *event);
     void setPackArt(int);
     void setPackArt2(int);
     void unFilter();
     void sortFilterDlg();
     void filterXD();
     void filterHydrogens(bool b);
     void filterNonAtoms(bool b);
     void filterThisAtom();
     void filterThisFragment();
     void filterOtherFragments();
     void textFilterChanged();
     void valFilterChanged();
     void setGD(double gd);
     void SDM(QStringList &brauchSymm,int packart=0);
     void renamUpdate(bool vis);
     void changeElemetofLabel(const QString &pse);
     void changeIndexofLabel(const int i);
     void changeSuffixofLabel(const QString &fix);
     void addNewScatteringFactor(int oz);
     void updateLabel();

protected:
     void paintEvent(QPaintEvent *event);
     void moveEvent(QMoveEvent *event);
 private:
     bool isLabelInUse();
     double pdf2(INP atom, V3 pos);
     double ueq(const Matrix m);
     void makePDFGrid(INP atom,double proba=50.0,bool c2=true ,bool c3=true,bool c4=true);
    QSlider *legendSize,*strikesSldr,*swidthSldr;
    QCheckBox *zebraBox,*mt;
    QComboBox *mapSchnitt;
    QDoubleSpinBox *maprad,*fomaps,*difmaps,*f12maps,*weak,*mapprec,*lineTrans,*lineWidth;
    QDialog *md;
    QRadioButton *cullNone,*cullBack,*cullFront;
    QSpinBox *fos;
    QFontComboBox *mlf;
//
     QSignalMapper *signalMapper;
     QMenu *xdMenu;
     FourMCQ *fmcq;
     void makeXDPartAux();
     void readXDPartAux();
     bool george,fck;
     void searchchanged(bool current,bool back);
     void createRenameWgd();
     QTextEdit *editor ;
     QLineEdit *search ;
     QDoubleSpinBox *gdedit ;
     QStandardItemModel *model;
     MySortFilterProxyModel *proxyModel;
     QTreeView * proxyView;
     QLineEdit *fromEdit,*toEdit,*regExpEdit;
     QDoubleValidator *qv;
     QDialog *sfdlg;
     QDialog *packdlg;
     int maxResi,filtered;
     char CID[80];
     double scalePic;
     QString dirName;
     QString saveName;
     void initLists(QList<INP> xd);
     void updateRecentFileActions();
     QString strippedName(const QString &fullFileName);
     double dimension(QList<INP> xdinp);
     void loadDipoleMoments(QString fileName);
     void someThingToRestore();
     void load_pdb(QString fileName);
     void load_sheldrick(QString fileName);
     void load_xdres(QString fileName);
     void load_MoPro(QString fileName);
     void cifcard(const QString v);
     void load_cif(QString fileName);
     void load_gaus(QString fileName);
     void load_fchk(QString fileName);
     void setup_zelle();
     QAction *togAxen,*togUnit,*togLuft;
     void loadFile(QString fileName,double GD=-1);
     QMenu *recentFilesMenu;
     enum { MaxRecentFiles = 10 };
     QAction *recentFileActs[MaxRecentFiles];
     QAction *grdpckAct;
     QLabel *nextNameL;
     QString labelPSE,labelSuffix,nextLabel;
     int labelIndex;
     QGroupBox *sfacBox;
     QSignalMapper *dsm;
     QRadioButton *pserbt[110];
     QSpinBox *indexSpin;
     QComboBox *sufixBox;
};


#endif

