#include <QGridLayout>
#include <QMessageBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QToolButton>
#include <QFileDialog>
#include "doxd.h"

Doxd::Doxd(void){
  isFirstTime=true;
  runner = new QProcess();
  QStringList env=runner->systemEnvironment();//
  runner->setWorkingDirectory(QDir::currentPath());
  settings = new QSettings(QSettings::IniFormat,  QSettings::UserScope ,"Christian_B._Huebschle", "MoleCoolQt" );
  settings->beginGroup("XDSettings");  
  xd_data_dir = settings->value("XD_DATA_DIR","").toString();
  xdLsmPath = settings->value("XD_LSM_PATH","").toString();
  xdPropPath = settings->value("XD_PROP_PATH","").toString();
  settings->endGroup();
  int idx=env.indexOf(QRegExp("^XD\\_DATADIR\\=\\S+"));
  if (idx>-1){
    QString edatadir=env.at(idx);
    edatadir.remove(0,11);
    if (xd_data_dir.isEmpty()) xd_data_dir=edatadir;
  }
  if ((xd_data_dir=="")||(xdLsmPath=="")||(xdPropPath=="")) {
    sdlg = new settingsDlg(xd_data_dir,xdLsmPath,xdPropPath);
    if (QDialog::Accepted==sdlg->exec()){
      xd_data_dir=sdlg->path.at(0);
      xdLsmPath=sdlg->path.at(1);
      xdPropPath=sdlg->path.at(2);
      settings->beginGroup("XDSettings");  
      settings->setValue("XD_DATA_DIR", xd_data_dir);
      settings->setValue("XD_LSM_PATH", xdLsmPath);
      settings->setValue("XD_PROP_PATH", xdPropPath);
      settings->endGroup();
    }
  }

  if (idx>-1) env.removeAt(idx);
  env.append("XD_DATADIR="+xd_data_dir);
  runner->setEnvironment(env);
  connect(runner,SIGNAL(finished(int,QProcess::ExitStatus)),this, SLOT(jobdone(int,QProcess::ExitStatus)));
}
void Doxd::presentDialog(){
  QStringList env=runner->systemEnvironment();
  int idx=env.indexOf(QRegExp("^XD\\_DATADIR\\=\\S+"));
  sdlg = new settingsDlg(xd_data_dir,xdLsmPath,xdPropPath);
  if (QDialog::Accepted==sdlg->exec()){
    xd_data_dir=sdlg->path.at(0);
    xdLsmPath=sdlg->path.at(1);
    xdPropPath=sdlg->path.at(2);
    settings->beginGroup("XDSettings");  
    settings->setValue("XD_DATA_DIR", xd_data_dir);
    settings->setValue("XD_LSM_PATH", xdLsmPath);
    settings->setValue("XD_PROP_PATH", xdPropPath);
    settings->endGroup();
    if (idx>-1) env.removeAt(idx);
    env.append(xd_data_dir);
  }
}
void Doxd::jobstart(){
  QMessageBox::information(0,"xdprop",QString("Job started!"),QMessageBox::Ok);  
}
void Doxd::jobdone( int exitCode, QProcess::ExitStatus exitStatus ){
  if ((exitCode!=1)||(exitStatus!=0)){;}//empty
  QDir work=QDir(QDir::current());  
  QStringList filter;
  filter << "*.out";  
  QStringList out;
  out=work.entryList(filter,QDir::Files,QDir::Time);  
  if (out.size()){
    if (out.at(0).contains("lsm"))emit reloadXD();
    QFile outp(out.at(0));
    outp.open(QIODevice::ReadOnly);
    editor = new QTextEdit;
    search = new QLineEdit;
    search->setMinimumSize(QSize(150, 0));

    QHBoxLayout *slt=new QHBoxLayout();
    QLabel sela("Search:");
    QToolButton prev;
    prev.setIcon(QIcon(":/images/moveleft.png"));
    prev.setText("previous");
    prev.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    prev.setAutoRaise(true);
    QToolButton next;
    next.setIcon(QIcon(":/images/moveright.png"));
    next.setText("next");
    next.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    next.setAutoRaise(true);
    slt->addWidget(&sela);
    slt->addWidget(search);
    slt->addWidget(&prev);
    slt->addWidget(&next);
    slt->addStretch(10);
    connect(&next, SIGNAL(clicked() ), this, SLOT(findNext()));
    connect(&prev, SIGNAL(clicked() ), this, SLOT(findPrev()));    
    connect(search, SIGNAL(textChanged(const QString&)),this,SLOT(findText()));
    editor->setReadOnly(true);
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);  
    editor->setFont(font);
    editor->setPlainText(runner->readAllStandardError()+"\n"+runner->readAllStandardOutput()+"\n"+outp.readAll());
    editor->setUndoRedoEnabled(false);
    editor->setUndoRedoEnabled(true);
    outp.close();  
    QDialog *outpview =new QDialog();
    outpview->setMinimumSize(900,300);  
    QVBoxLayout sss;
    sss.addWidget(editor);
    sss.addLayout(slt);
    outpview->setLayout(&sss);
    outpview->setWindowTitle(out.at(0));
    outpview->exec();
  }
}
void Doxd::findNext(){
  searchchanged(false,false);
}
void Doxd::findText(){
  searchchanged(true,false);
}
void Doxd::findPrev(){
  searchchanged(false,true);
}
void Doxd::searchchanged(bool current,bool back){
  QString searchString = search->text();
  QPalette p = search->palette();
  p.setBrush(QPalette::Active,QPalette::Base,Qt::white);
  QTextDocument *document = editor->document();
  QTextCursor cursor = editor->textCursor();
  if (!searchString.isEmpty()){
    if (cursor.hasSelection())
      cursor.setPosition(current|back?cursor.anchor():cursor.position(), QTextCursor::MoveAnchor);
    
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
void Doxd::runXdLsm(QString cid, QString mid){
  QStringList args;
  args<<cid;
  if (!mid.isEmpty())args<<mid;
  runner->start(xdLsmPath,args);  
}
void Doxd::runXdProp(QString cid, QString mid){
  QStringList args;
  args<<cid;
  if (!mid.isEmpty())args<<mid;
  runner->start(xdPropPath,args);  
}
settingsDlg::settingsDlg(QString xd_data_dir,QString xdLsmPath,QString xdPropPath): QDialog(){
  QLabel *datl  = new QLabel("XD_DATADIR");
  QLabel *lsml  = new QLabel("path to xdlsm executable");
  QLabel *propl = new QLabel("path to xdprop executable");
  dataL = new QLineEdit(xd_data_dir);
  lsmL  = new QLineEdit(xdLsmPath);
  propL = new QLineEdit(xdPropPath);
  dataBrws = new QPushButton(QIcon(":/images/folder_page_white.png"),"Browse");
  lsmBrws  = new QPushButton(QIcon(":/images/folder_page_white.png"),"Browse");
  propBrws = new QPushButton(QIcon(":/images/folder_page_white.png"),"Browse");
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  QGridLayout *qgl = new QGridLayout;
  qgl->addWidget(datl ,0,0);
  qgl->addWidget(dataL,1,0);
  qgl->addWidget(dataBrws,1,2);
  qgl->addWidget(lsml ,2,0);
  qgl->addWidget(lsmL ,3,0);
  qgl->addWidget(lsmBrws ,3,2);
  qgl->addWidget(propl,4,0);
  qgl->addWidget(propL,5,0);
  qgl->addWidget(propBrws,5,2);
  qgl->addWidget(buttonBox,6,0);
  setLayout(qgl);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(dataBrws,  SIGNAL(released()), this, SLOT(brwsData()));
  connect(lsmBrws,   SIGNAL(released()), this, SLOT(brwsLsm()));
  connect(propBrws,  SIGNAL(released()), this, SLOT(brwsProp()));
}

void settingsDlg::brwsData(){
  QString fileName = QFileDialog::getExistingDirectory(this, "XD_DATADIR", dataL->text());
  if (!fileName.isEmpty())dataL->setText(fileName);
  update();
}
void settingsDlg::brwsLsm(){
  QString fileName = QFileDialog::getOpenFileName(this, "path to xdlsm executeable", lsmL->text(),
						  "xdlsm (*);;",&selectedFilter,QFileDialog::DontUseNativeDialog  );
  if (!fileName.isEmpty())lsmL->setText(fileName);
  update();
}
void settingsDlg::brwsProp(){
  QString fileName = QFileDialog::getOpenFileName(this, "path to xdprop executeable", propL->text(),
						  "xdprop (*);;" ,&selectedFilter,QFileDialog::DontUseNativeDialog );
  if (!fileName.isEmpty())propL->setText(fileName);
  update();
}
void settingsDlg::accept () {
  path.clear();
  path.append(dataL->text());
  path.append(lsmL->text());
  path.append(propL->text());
  done(QDialog::Accepted);
}  
