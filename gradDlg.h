#include "moliso.h"
#include <QtGui>

class gradientWidget:public QWidget{
Q_OBJECT
    public:
gradientWidget(QLinearGradient &gra,QWidget *parent = 0);
gradientWidget(QColor color,QWidget *parent=0);
void setColor(QColor color);
protected:
     void paintEvent(QPaintEvent *event);
private:
    void generateShade();
     QLinearGradient gradient;
     QImage _shade;
};


class GradDlg:public QDialog{
Q_OBJECT
  private:
  MolIso *mi;
  QPushButton *f1, *f2, *f3, *f4, *f5, *f6,*f7,*f8;
  QSlider *s1,*s2,*s3,*s4,*s5,*s6,*s7;
  gradientWidget *g1,*g2,*g3,*g4, *g5, *g6, *g7;
  QLineEdit *isoValuesEdit;
  QDoubleSpinBox *mxedit,*miedit; 
  QCheckBox *mmcb;
  QHBoxLayout *hla;
  QLinearGradient *gg;
  gradientWidget *gw;
  QComboBox *cmbx;
  QString valuesString;
  QString selectedFilter;
 public:
  void loadSettings();
  GradDlg(MolIso *mi,QString mapFileName);
  QCheckBox *cmmcp;
  public slots:
  void accept () ;
  void  changeF1();
  void  changeF2();
  void  changeF3();
  void  changeF4();
  void  changeF5();
  void  changeF6();
  void  changeF7();
  void mmstate(int);

  void  regen();
  void  protein();
  void gradientpunktdat();
  void savegradientpunktdat();
  void changeCSteps(int steps);
  void  changeS1( int value );
  void  changeS2( int value );
  void  changeS3( int value );
  void  changeS4( int value );
  void  changeS5( int value );
  void  changeS6( int value );
  void  changeS7( int value );
  void updateColors();
};

