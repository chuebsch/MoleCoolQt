#include <QtGui>

class MolisoStartDlg:public QDialog{
Q_OBJECT
  private:
  QPushButton *ibrws,*abrws,*mbrws,*lfbrws,*sfbrws,*doit;
  QLineEdit *iedit,*aedit,*medit,*lfedit,*sfedit;
  QRadioButton *lfcbx,*sfcbx;
  QCheckBox *mcbx,*acbx;
  QString selectedFilter;
  void loadSettings();
  int checker;
  public:
  QString iso_grid_name,map_grid_name,load_face_name,save_face_name,adp_struct_name;
  MolisoStartDlg(QString isoname="",QString mapname="",QString lfcename="",QString sfcename="",int check=0,QString adpname="");
  public slots:
  void accept () ;
  void brwsIso();
  void brwsMap();
  void brwsADP();
  void brwslFace();
  void brwssFace();
  void achange(const QString &s);
  void mchange(const QString &s);
  void ichange(const QString &s);
  void lfchange(const QString &s);
  void sfchange(const QString &s);
  
};
