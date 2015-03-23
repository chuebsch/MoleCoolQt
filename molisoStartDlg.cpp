#include "molisoStartDlg.h"

MolisoStartDlg::MolisoStartDlg(QString isoname, QString mapname, QString lfcename, QString sfcename, int check, QString adpname){
  if (isoname.isEmpty()){
      loadSettings();
  }else{
      iso_grid_name=isoname;
      checker=check;
      map_grid_name=   mapname;
      load_face_name=  lfcename;
      save_face_name=  sfcename;
      adp_struct_name= adpname;
      this->setWindowTitle(QString("%1 MolIso Start Dialog").arg(check));
  }
  setMinimumSize(QSize(550,200));
  QLabel *isol= new QLabel("Iso-Grid-File");
  mcbx= new QCheckBox("Map-Grid-File");
  lfcbx = new QRadioButton("Load MolIso-Surface-File");
  sfcbx = new QRadioButton("Save MolIso-Surface-File");
  sfcbx->setStatusTip("If you check this new iso-surfaces will be created.");
  sfcbx->setWhatsThis("If you check this new iso-surfaces will be created.");
  acbx= new QCheckBox("ADP-structure-File");

  mcbx->setChecked(checker&1);
  lfcbx->setChecked(checker&2);
  sfcbx->setChecked(checker&4);
  acbx->setChecked(checker&8);
  iedit = new QLineEdit(iso_grid_name);
  aedit = new QLineEdit(adp_struct_name);
  medit = new QLineEdit(map_grid_name);
  lfedit = new QLineEdit(load_face_name);
  sfedit = new QLineEdit(save_face_name);
  sfedit->setStatusTip("You may leave this field empty. If you do so then a temporary file will be created.");
  sfedit->setWhatsThis("You may leave this field empty. If you do so then a temporary file will be created.");
  ibrws = new QPushButton("Browse");
  abrws = new QPushButton("Browse");
  mbrws = new QPushButton("Browse");
  lfbrws = new QPushButton("Browse");
  sfbrws = new QPushButton("Browse");
  doit = new QPushButton("Make surfaces!");
  doit->setDefault (true);
  connect(doit,SIGNAL(pressed()),this,SLOT(accept()));
  connect(ibrws,SIGNAL(pressed()),this,SLOT(brwsIso()));
  connect(mbrws,SIGNAL(pressed()),this,SLOT(brwsMap()));
  connect(lfbrws,SIGNAL(pressed()),this,SLOT(brwslFace()));
  connect(sfbrws,SIGNAL(pressed()),this,SLOT(brwssFace()));
  connect(abrws,SIGNAL(pressed()),this,SLOT(brwsADP()));
  connect(iedit,SIGNAL(textEdited ( const QString)),this,SLOT(ichange(const QString)));
  connect(aedit,SIGNAL(textEdited ( const QString)),this,SLOT(achange(const QString)));
  connect(medit,SIGNAL(textEdited ( const QString)),this,SLOT(mchange(const QString)));
  connect(lfedit,SIGNAL(textEdited ( const QString)),this,SLOT(lfchange(const QString)));
  connect(sfedit,SIGNAL(textEdited ( const QString)),this,SLOT(sfchange(const QString)));
  QGridLayout *gl = new QGridLayout();
  gl->addWidget(isol,0,0,1,2);
  gl->addWidget(iedit,0,2,1,1);
  gl->addWidget(ibrws,0,3,1,1);
  gl->addWidget(mcbx,1,0,1,1);
  gl->addWidget(medit,1,2,1,1);
  gl->addWidget(mbrws,1,3,1,1);
  gl->addWidget(lfcbx,2,0,1,1);
  gl->addWidget(lfedit,2,2,1,1);
  gl->addWidget(lfbrws,2,3,1,1);

  gl->addWidget(sfcbx,3,0,1,1);
  gl->addWidget(sfedit,3,2,1,1);
  gl->addWidget(sfbrws,3,3,1,1);
  gl->addWidget(acbx,4,0,1,1);
  gl->addWidget(aedit,4,2,1,1);
  gl->addWidget(abrws,4,3,1,1);
  gl->addWidget(doit,5,3,1,1);
  setLayout(gl);
}


void MolisoStartDlg::achange(const QString &s){
  adp_struct_name= s;
}
void MolisoStartDlg::mchange(const QString &s){
  map_grid_name  = s;
}
void MolisoStartDlg::ichange(const QString &s){
  iso_grid_name  = s;
}
void MolisoStartDlg::lfchange(const QString &s){
  load_face_name = s;
}

void MolisoStartDlg::sfchange(const QString &s){
  save_face_name = s;
}

void MolisoStartDlg::brwsIso(){
  iedit->setText(QFileDialog::getOpenFileName(this, tr("Open iso-grid file "), iso_grid_name,
					     "XD-3D-Grid-Files (*.grd);;"
                         "Jana2006 m81-Files (*.m81);;"
					     "GAUSSIAN Cube-Files (*.cube *.cub);;" ,&selectedFilter,QFileDialog::DontUseNativeDialog ));

  iso_grid_name  = iedit->text();
}
void MolisoStartDlg::brwsMap(){
  medit->setText(QFileDialog::getOpenFileName(this, tr("Open map-grid file "), map_grid_name,
					     "XD-3D-Grid-Files (*.grd);;"
					     "GAUSSIAN Cube-Files (*.cube *.cub);;",&selectedFilter,QFileDialog::DontUseNativeDialog  ));
  map_grid_name  = medit->text();
}

void MolisoStartDlg::brwslFace(){
  lfedit->setText(QFileDialog::getOpenFileName(this, tr("Open MolIso-surface-file "), load_face_name,
					     "MolIso-surface-file (*.face *);;" ,&selectedFilter,QFileDialog::DontUseNativeDialog ));
  load_face_name = lfedit->text();
}

void MolisoStartDlg::brwssFace(){
  sfedit->setText(QFileDialog::getSaveFileName(this, tr("Open MolIso-surface-file "), save_face_name,
                                             "MolIso-surface-file (*.face *);;" ,&selectedFilter,QFileDialog::DontUseNativeDialog ));
  save_face_name = sfedit->text();
}

void MolisoStartDlg::brwsADP(){
  aedit->setText(QFileDialog::getOpenFileName(this, tr("Open structure-file for ADP's"), adp_struct_name,
					      "XD-Files (*.res *.inp *.mas);;"
					      "SHELX-Files (*.res *.ins);;"
					      "CIF-Files (*.cif);;"
					      "Protein Data Base file (*.pdb *.ent);;" ,&selectedFilter,QFileDialog::DontUseNativeDialog ));
  adp_struct_name= aedit->text();
}

void MolisoStartDlg::loadSettings(){
    QSettings settings(QSettings::IniFormat,  QSettings::UserScope ,"Christian_B._Huebschle", "MoleCoolQt" );
    settings.beginGroup("Version 0.1");    
    iso_grid_name=settings.value("iso_grid_name").toString();
    map_grid_name=settings.value("map_grid_name").toString();
    load_face_name=settings.value("load_face_name").toString();
    save_face_name=settings.value("save_face_name").toString();
    adp_struct_name=settings.value("adp_struct_name").toString();
    map_grid_name.remove('!');
    load_face_name.remove('!');
    save_face_name.remove('!');
    adp_struct_name.remove('!');
    checker=settings.value("Moliso_Dialog_checks").toInt();
    settings.endGroup();
}
void MolisoStartDlg::accept(){  
  if (acbx->checkState()==Qt::Unchecked) adp_struct_name=QString("!%1").arg(adp_struct_name);
  else adp_struct_name.remove('!');
  if (!lfcbx->isChecked()) load_face_name=QString("!%1").arg(load_face_name);
  else load_face_name.remove('!');
  if (!sfcbx->isChecked()) save_face_name=QString("!%1").arg(save_face_name);
  else save_face_name.remove('!');
  if (!mcbx->isChecked()) map_grid_name=QString("!%1").arg(map_grid_name);
  else map_grid_name.remove('!');
  if (!iso_grid_name.isEmpty()){
    QSettings settings(QSettings::IniFormat,  QSettings::UserScope ,"Christian_B._Huebschle","MoleCoolQt" );
    settings.beginGroup("Version 0.1");    
    settings.setValue("adp_struct_name", adp_struct_name );
    settings.setValue("load_face_name", load_face_name);
    settings.setValue("map_grid_name", map_grid_name);
    settings.setValue("iso_grid_name", iso_grid_name);
    int checker=0;
    checker|=mcbx->isChecked();
    checker|=(lfcbx->isChecked())<<1;
    checker|=(sfcbx->isChecked())<<2;
    checker|=(acbx->isChecked())<<3;
    settings.setValue("Moliso_Dialog_checks",checker);
    settings.endGroup();
    done(QDialog::Accepted);
  }
}
