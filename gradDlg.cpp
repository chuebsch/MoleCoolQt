#include "gradDlg.h"

GradDlg::GradDlg(MolIso *_mi){
  mi=_mi;
  loadSettings();
  f1=new QPushButton("Color");
  f2=new QPushButton("Color");
  f3=new QPushButton("Color");
  f4=new QPushButton("Color");
  f5=new QPushButton("Color");
  f6=new QPushButton("Color");  
  f7=new QPushButton("Color");
  g1=new gradientWidget(mi->qtFarbe(0));
  g2=new gradientWidget(mi->qtFarbe(1));
  g3=new gradientWidget(mi->qtFarbe(2));
  g4=new gradientWidget(mi->qtFarbe(3));
  g5=new gradientWidget(mi->qtFarbe(4));
  g6=new gradientWidget(mi->qtFarbe(5));
  g7=new gradientWidget(mi->qtFarbe(6));
  isoValuesEdit = new QLineEdit(QString("%1 %2").arg(-0.1,5,'f',2).arg(0.1,5,'f',2));
  if (!valuesString.isEmpty())isoValuesEdit->setText(valuesString);
  QLabel *ivL = new QLabel("Iso values:");
  QHBoxLayout *isola = new QHBoxLayout();
  isola->addWidget(ivL);
  isola->addWidget(isoValuesEdit);
  mmcb = new QCheckBox("Fix Min/Max");
  mmcb->setCheckState((mi->fixmax==mi->fixmin)?Qt::Unchecked:Qt::Checked);
  
  mxedit = new QDoubleSpinBox();
  mxedit->setSingleStep(0.1);
  mxedit->setDecimals(5);
  mxedit->setMaximum(1e99);
  mxedit->setMinimum(-1e99);
  mxedit->setValue((mi->fixmax==mi->fixmin)?0.0001:mi->fixmax);

  miedit = new QDoubleSpinBox();
  miedit->setSingleStep(0.1);
  miedit->setDecimals(5);
  miedit->setRange(-1e99, 1e99);
  miedit->setValue((mi->fixmax==mi->fixmin)?-0.0001:mi->fixmin);
  if (mi->fixmax==mi->fixmin){
    mxedit->setEnabled(false);
    miedit->setEnabled(false);
  }
  QHBoxLayout *mimala = new QHBoxLayout();
  mimala->addWidget(mmcb);
  mimala->addWidget(miedit);
  mimala->addWidget(mxedit);

  connect(mmcb,SIGNAL(stateChanged ( int )),this,SLOT(mmstate(int)));
  connect(f1,SIGNAL(pressed()),this,SLOT(changeF1()));
  connect(f2,SIGNAL(pressed()),this,SLOT(changeF2()));
  connect(f3,SIGNAL(pressed()),this,SLOT(changeF3()));
  connect(f4,SIGNAL(pressed()),this,SLOT(changeF4()));
  connect(f5,SIGNAL(pressed()),this,SLOT(changeF5()));
  connect(f6,SIGNAL(pressed()),this,SLOT(changeF6()));
  connect(f7,SIGNAL(pressed()),this,SLOT(changeF7()));

  s1 = new  QSlider(Qt::Horizontal);
  s2 = new  QSlider(Qt::Horizontal);
  s3 = new  QSlider(Qt::Horizontal);
  s4 = new  QSlider(Qt::Horizontal);
  s5 = new  QSlider(Qt::Horizontal);
  s6 = new  QSlider(Qt::Horizontal);
  s7 = new  QSlider(Qt::Horizontal);

  s1->setTickPosition(QSlider::TicksBelow);
  s2->setTickPosition(QSlider::TicksBelow);
  s3->setTickPosition(QSlider::TicksBelow);
  s4->setTickPosition(QSlider::TicksBelow);
  s5->setTickPosition(QSlider::TicksBelow);
  s6->setTickPosition(QSlider::TicksBelow);
  s7->setTickPosition(QSlider::TicksBelow);


  s1->setTickInterval(5);  
  s2->setTickInterval(5);
  s3->setTickInterval(5);
  s4->setTickInterval(5);
  s5->setTickInterval(5);
  s6->setTickInterval(5);
  s7->setTickInterval(5);

  s1->setMaximum(100);
  s2->setMaximum(100);
  s3->setMaximum(100);
  s4->setMaximum(100);
  s5->setMaximum(100);
  s6->setMaximum(100);
  s7->setMaximum(100);

  s1->setMinimum(0);
  s2->setMinimum(0);
  s3->setMinimum(0);
  s4->setMinimum(0);
  s5->setMinimum(0);
  s6->setMinimum(0);
  s7->setMinimum(0);

  s1->setValue((int)(mi->farbe[0][3]*100.0));
  s2->setValue((int)(mi->farbe[1][3]*100.0));
  s3->setValue((int)(mi->farbe[2][3]*100.0));
  s4->setValue((int)(mi->farbe[3][3]*100.0));
  s5->setValue((int)(mi->farbe[4][3]*100.0));
  s6->setValue((int)(mi->farbe[5][3]*100.0));
  s7->setValue((int)(mi->farbe[6][3]*100.0));


  connect(s1,SIGNAL(sliderMoved(int)),this,SLOT(changeS1(int)));
  connect(s2,SIGNAL(sliderMoved(int)),this,SLOT(changeS2(int)));
  connect(s3,SIGNAL(sliderMoved(int)),this,SLOT(changeS3(int)));
  connect(s4,SIGNAL(sliderMoved(int)),this,SLOT(changeS4(int)));
  connect(s5,SIGNAL(sliderMoved(int)),this,SLOT(changeS5(int)));
  connect(s6,SIGNAL(sliderMoved(int)),this,SLOT(changeS6(int)));
  connect(s7,SIGNAL(sliderMoved(int)),this,SLOT(changeS7(int)));
  QVBoxLayout *gla=new QVBoxLayout();
  QVBoxLayout *la=new QVBoxLayout();
  gg=new QLinearGradient(0,0,0,1);
  gg->setColorAt(1.0,QColor::fromRgbF(mi->farbe[0][0],mi->farbe[0][1],mi->farbe[0][2],mi->farbe[0][3]));
  gg->setColorAt(0.8,QColor::fromRgbF(mi->farbe[1][0],mi->farbe[1][1],mi->farbe[1][2],mi->farbe[1][3]));
  gg->setColorAt(0.6,QColor::fromRgbF(mi->farbe[2][0],mi->farbe[2][1],mi->farbe[2][2],mi->farbe[2][3]));
  gg->setColorAt(0.4,QColor::fromRgbF(mi->farbe[3][0],mi->farbe[3][1],mi->farbe[3][2],mi->farbe[3][3]));
  gg->setColorAt(0.2,QColor::fromRgbF(mi->farbe[4][0],mi->farbe[4][1],mi->farbe[4][2],mi->farbe[4][3]));
  gg->setColorAt(0.0,QColor::fromRgbF(mi->farbe[5][0],mi->farbe[5][1],mi->farbe[5][2],mi->farbe[5][3]));
  gw = new gradientWidget(*gg);
  gw->setMinimumHeight(67*mi->Farben);
  gw->setMinimumWidth(60);
  f8=new QPushButton("OK");
  connect(f8,SIGNAL(pressed()),this,SLOT(accept()));
  cmbx= new QComboBox();
  QStringList xx;
  xx<<"2 colors steps"<<"3 colors steps"<<"4 colors steps"<<"5 colors steps"<<"6 colors steps"<<"7 colors steps";
  cmbx->addItems(xx);
  cmbx->setCurrentIndex(mi->Farben-2);
  hla=new QHBoxLayout(); 
  la->addWidget(f7);
  la->addWidget(g7);
  la->addWidget(s7);
  la->addWidget(f6);
  la->addWidget(g6);
  la->addWidget(s6);
  la->addWidget(f5);
  la->addWidget(g5);
  la->addWidget(s5);
  la->addWidget(f4);
  la->addWidget(g4);
  la->addWidget(s4);
  la->addWidget(f3);
  la->addWidget(g3);
  la->addWidget(s3);
  la->addWidget(f2);
  la->addWidget(g2);
  la->addWidget(s2);
  la->addWidget(f1);
  la->addWidget(g1);
  la->addWidget(s1);
  hla->addLayout(la);
  hla->addWidget(gw);

  connect(cmbx,SIGNAL( currentIndexChanged (int)),this,SLOT(changeCSteps(int)));
  QHBoxLayout *defla= new QHBoxLayout();
  QPushButton *rain=new QPushButton("default rainbow");
  QPushButton *prot=new QPushButton("default red/white/blue");
  QPushButton *dat=new QPushButton("load 'Gradient.dat'");
  QPushButton *sdat=new QPushButton("export 'Gradient.dat'");
  defla->addWidget(rain);
  defla->addWidget(prot);
  defla->addWidget(dat);
  defla->addWidget(sdat);
  connect(rain,SIGNAL(pressed()),this,SLOT(regen()));
  connect(prot,SIGNAL(pressed()),this,SLOT(protein()));
  connect(dat,SIGNAL(pressed()),this,SLOT(gradientpunktdat()));
  connect(sdat,SIGNAL(pressed()),this,SLOT(savegradientpunktdat()));
  gla->addLayout(isola);
  gla->addLayout(mimala);
  gla->addLayout(defla);
  gla->addWidget(cmbx);
  gla->addLayout(hla);
  gla->addWidget(f8);
  f8->setDefault (true);
  setLayout(gla);
  changeCSteps(mi->Farben-2);
}
void GradDlg::changeCSteps(int steps){
  switch (steps){
  case 0:
    f1->show();
    s1->show();
    f2->show();
    s2->show();
    f3->hide();
    s3->hide();
    f4->hide();
    s4->hide();
    f5->hide();
    s5->hide();
    f6->hide();
    s6->hide();
    f7->hide();
    s7->hide();
    g1->show();
    g2->show();
    g3->hide();
    g4->hide();
    g5->hide();
    g6->hide();
    g7->hide();
    mi->Farben=2;
    break;
  case 1:
    f1->show();
    s1->show();
    f2->show();
    s2->show();
    f3->show();
    s3->show();
    f4->hide();
    s4->hide();
    f5->hide();
    s5->hide();
    f6->hide();
    s6->hide();
    f7->hide();
    s7->hide();
    g1->show();
    g2->show();
    g3->show();
    g4->hide();
    g5->hide();
    g6->hide();
    g7->hide();
    mi->Farben=3;
    break;
  case 2:
    f1->show();
    s1->show();
    f2->show();
    s2->show();
    f3->show();
    s3->show();
    f4->show();
    s4->show();
    f5->hide();
    s5->hide();
    f6->hide();
    s6->hide();
    f7->hide();
    s7->hide();
    g1->show();
    g2->show();
    g3->show();
    g4->show();
    g5->hide();
    g6->hide();
    g7->hide();
    mi->Farben=4;
    break;
  case 3:
    f1->show();
    s1->show();
    f2->show();
    s2->show();
    f3->show();
    s3->show();
    f4->show();
    s4->show();
    f5->show();
    s5->show();
    f6->hide();
    s6->hide();
    f7->hide();
    s7->hide();
    g1->show();
    g2->show();
    g3->show();
    g4->show();
    g5->show();
    g6->hide();
    g7->hide();
    mi->Farben=5;
    break;
  case 4:
    f1->show();
    s1->show();
    f2->show();
    s2->show();
    f3->show();
    s3->show();
    f4->show();
    s4->show();
    f5->show();
    s5->show();
    f6->show();
    s6->show();
    f7->hide();
    s7->hide();
    g1->show();
    g2->show();
    g3->show();
    g4->show();
    g5->show();
    g6->show();
    g7->hide();
    mi->Farben=6;
    break;
  case 5:
    f1->show();
    s1->show();
    f2->show();
    s2->show();
    f3->show();
    s3->show();
    f4->show();
    s4->show();
    f5->show();
    s5->show();
    f6->show();
    s6->show();
    f7->show();
    s7->show();
    g1->show();
    g2->show();
    g3->show();
    g4->show();
    g5->show();
    g6->show();
    g7->show();
    mi->Farben=7;
    break;
  }
  updateColors();
}

void GradDlg::changeF1(){  
  QColor bgcolor;    
  bgcolor=QColorDialog::getColor(QColor((int)(mi->farbe[0][0]*255),(int)(mi->farbe[0][1]*255),(int)(mi->farbe[0][2]*255),(int)(mi->farbe[0][3]*255)), this);  
  if (bgcolor.isValid()) {
    qreal bgRed,bgGreen,bgBlue,bgAlpha;
    bgcolor.getRgbF(&bgRed,&bgGreen,&bgBlue,&bgAlpha);
    mi->farbe[0][0]=(GLclampf)bgRed;
    mi->farbe[0][1]=(GLclampf)bgGreen;
    mi->farbe[0][2]=(GLclampf)bgBlue;
  }
  updateColors();
}
void GradDlg::changeF2(){
  QColor bgcolor;    
  bgcolor=QColorDialog::getColor(QColor((int)(mi->farbe[1][0]*255),(int)(mi->farbe[1][1]*255),(int)(mi->farbe[1][2]*255),(int)(mi->farbe[1][3]*255)), this);  
  if (bgcolor.isValid()) {
    qreal bgRed,bgGreen,bgBlue,bgAlpha;
    bgcolor.getRgbF(&bgRed,&bgGreen,&bgBlue,&bgAlpha);
    mi->farbe[1][0]=(GLclampf)bgRed;
    mi->farbe[1][1]=(GLclampf)bgGreen;
    mi->farbe[1][2]=(GLclampf)bgBlue;
  }
  updateColors();
}
void GradDlg::changeF3(){
  QColor bgcolor;    
  bgcolor=QColorDialog::getColor(QColor((int)(mi->farbe[2][0]*255),(int)(mi->farbe[2][1]*255),(int)(mi->farbe[2][2]*255),(int)(mi->farbe[2][3]*255)), this);  
  if (bgcolor.isValid()) {
    qreal bgRed,bgGreen,bgBlue,bgAlpha;
    bgcolor.getRgbF(&bgRed,&bgGreen,&bgBlue,&bgAlpha);
    mi->farbe[2][0]=(GLclampf)bgRed;
    mi->farbe[2][1]=(GLclampf)bgGreen;
    mi->farbe[2][2]=(GLclampf)bgBlue;
  }
  updateColors();
}
void GradDlg::changeF4(){
  QColor bgcolor;    
  bgcolor=QColorDialog::getColor(QColor((int)(mi->farbe[3][0]*255),(int)(mi->farbe[3][1]*255),(int)(mi->farbe[3][2]*255),(int)(mi->farbe[3][3]*255)), this);  
  if (bgcolor.isValid()) {
    qreal bgRed,bgGreen,bgBlue,bgAlpha;
    bgcolor.getRgbF(&bgRed,&bgGreen,&bgBlue,&bgAlpha);
    mi->farbe[3][0]=(GLclampf)bgRed;
    mi->farbe[3][1]=(GLclampf)bgGreen;
    mi->farbe[3][2]=(GLclampf)bgBlue;
  }
  updateColors();
}
void GradDlg::changeF5(){
  QColor bgcolor;    
  bgcolor=QColorDialog::getColor(QColor((int)(mi->farbe[4][0]*255),(int)(mi->farbe[4][1]*255),(int)(mi->farbe[4][2]*255),(int)(mi->farbe[4][3]*255)), this);  
  if (bgcolor.isValid()) {
    qreal bgRed,bgGreen,bgBlue,bgAlpha;
    bgcolor.getRgbF(&bgRed,&bgGreen,&bgBlue,&bgAlpha);
    mi->farbe[4][0]=(GLclampf)bgRed;
    mi->farbe[4][1]=(GLclampf)bgGreen;
    mi->farbe[4][2]=(GLclampf)bgBlue;
  }
  updateColors();
}
void GradDlg::changeF6(){
  QColor bgcolor;    
  bgcolor=QColorDialog::getColor(QColor((int)(mi->farbe[5][0]*255),(int)(mi->farbe[5][1]*255),(int)(mi->farbe[5][2]*255),(int)(mi->farbe[5][3]*255)), this);  
  if (bgcolor.isValid()) {
    qreal bgRed,bgGreen,bgBlue,bgAlpha;
    bgcolor.getRgbF(&bgRed,&bgGreen,&bgBlue,&bgAlpha);
    mi->farbe[5][0]=(GLclampf)bgRed;
    mi->farbe[5][1]=(GLclampf)bgGreen;
    mi->farbe[5][2]=(GLclampf)bgBlue;
  }
  updateColors();
}
void GradDlg::changeF7(){
  QColor bgcolor;    
  bgcolor=QColorDialog::getColor(QColor((int)(mi->farbe[6][0]*255),(int)(mi->farbe[6][1]*255),(int)(mi->farbe[6][2]*255),(int)(mi->farbe[6][3]*255)), this);  
  if (bgcolor.isValid()) {
    qreal bgRed,bgGreen,bgBlue,bgAlpha;
    bgcolor.getRgbF(&bgRed,&bgGreen,&bgBlue,&bgAlpha);
    mi->farbe[6][0]=(GLclampf)bgRed;
    mi->farbe[6][1]=(GLclampf)bgGreen;
    mi->farbe[6][2]=(GLclampf)bgBlue;
  }
  updateColors();
}

void  GradDlg::changeS1(int value){
  mi->farbe[0][3]=value/100.0;
  updateColors();
}	       
void  GradDlg::changeS2(int value){
  mi->farbe[1][3]=value/100.0;
  updateColors();
}
void  GradDlg::changeS3(int value){
  mi->farbe[2][3]=value/100.0;
  updateColors();
}
void  GradDlg::changeS4(int value){
  mi->farbe[3][3]=value/100.0;
  updateColors();
}
void  GradDlg::changeS5(int value){
  mi->farbe[4][3]=value/100.0;
  updateColors();
}
void  GradDlg::changeS6(int value){
  mi->farbe[5][3]=value/100.0;
  updateColors();
}
void  GradDlg::changeS7(int value){
  mi->farbe[6][3]=value/100.0;
  updateColors();
}
void GradDlg::updateColors(){
 delete gg;
  gg=new QLinearGradient(0,0,0,1);
  for (int i=0; i< mi->Farben; i++)
    gg->setColorAt(1.0-(i/(mi->Farben-1.0)),QColor::fromRgbF(mi->farbe[i][0],mi->farbe[i][1],mi->farbe[i][2],mi->farbe[i][3]));
  delete gw;
  gw = new gradientWidget(*gg);
  gw->setMinimumHeight(67*mi->Farben);
  gw->setMinimumWidth(60);
  hla->addWidget(gw);
  setMinimumHeight(205+mi->Farben*68);
g1->setColor(mi->qtFarbe(0));
g2->setColor(mi->qtFarbe(1));
g3->setColor(mi->qtFarbe(2));
g4->setColor(mi->qtFarbe(3));
g5->setColor(mi->qtFarbe(4));
g6->setColor(mi->qtFarbe(5));
g7->setColor(mi->qtFarbe(6));
  s1->setValue((int)(mi->farbe[0][3]*100.));
  s2->setValue((int)(mi->farbe[1][3]*100.));
  s3->setValue((int)(mi->farbe[2][3]*100.));
  s4->setValue((int)(mi->farbe[3][3]*100.));
  s5->setValue((int)(mi->farbe[4][3]*100.));
  s6->setValue((int)(mi->farbe[5][3]*100.));
  s7->setValue((int)(mi->farbe[6][3]*100.));
}


void GradDlg::regen(){
  //Constructor
  mi->Farben=6;    
  mi->farbe[0][0]=0.6;    
  mi->farbe[0][1]=0;    
  mi->farbe[0][2]=0;    
  mi->farbe[0][3]=0.5;
  mi->farbe[1][0]=0.6;    
  mi->farbe[1][1]=0.5;    
  mi->farbe[1][2]=0;    
  mi->farbe[1][3]=0.5;
  mi->farbe[2][0]=0;    
  mi->farbe[2][1]=0.5;    
  mi->farbe[2][2]=0;    
  mi->farbe[2][3]=0.5;
  mi->farbe[3][0]=0;    
  mi->farbe[3][1]=0.5;    
  mi->farbe[3][2]=0.9;    
  mi->farbe[3][3]=0.5;
  mi->farbe[4][0]=0;    
  mi->farbe[4][1]=0;    
  mi->farbe[4][2]=0.9;    
  mi->farbe[4][3]=0.5;    
  mi->farbe[5][0]=0.6;    
  mi->farbe[5][1]=0;    
  mi->farbe[5][2]=0.9;    
  mi->farbe[5][3]=0.5;    
  mi->farbe[6][0]=0.6;    
  mi->farbe[6][1]=0;    
  mi->farbe[6][2]=0;    
  mi->farbe[6][3]=0.5;  

  cmbx->setCurrentIndex(4);
  updateColors();
}


void GradDlg::protein(){
  //Constructor
  mi->Farben=5;    
  mi->farbe[0][0]=1.0;    
  mi->farbe[0][1]=0;    
  mi->farbe[0][2]=0;    
  mi->farbe[0][3]=0.6;

  mi->farbe[1][0]=1.0;    
  mi->farbe[1][1]=0.0;    
  mi->farbe[1][2]=0.0;    
  mi->farbe[1][3]=0.5;

  mi->farbe[2][0]=1.0;    
  mi->farbe[2][1]=1.0;    
  mi->farbe[2][2]=1.0;    
  mi->farbe[2][3]=0.3;

  mi->farbe[3][0]=0.0;    
  mi->farbe[3][1]=0.0;    
  mi->farbe[3][2]=1.0;    
  mi->farbe[3][3]=0.5;

  mi->farbe[4][0]=0;    
  mi->farbe[4][1]=0;    
  mi->farbe[4][2]=1.0;    
  mi->farbe[4][3]=0.6;    

  cmbx->setCurrentIndex(3);
  updateColors();
}

void GradDlg::gradientpunktdat(){  
  QString fileName;;
  fileName = QFileDialog::getOpenFileName(this, tr("Open 'Gradient.dat' "), "",//dirName,
					    "MolIso Gradient.dat(*.dat);;"
					    "All files (*);;",&selectedFilter,QFileDialog::DontUseNativeDialog );
  if (!fileName.isEmpty()) {
    QFile gdat(fileName);
    gdat.open(QIODevice::ReadOnly);
    QString all=gdat.readAll();
    QStringList tok=all.split(QRegExp("\\s+"),QString::SkipEmptyParts);
    if ((tok.size() % 4)==0){
      mi->Farben=tok.size()/4;
      for (int i=0; i<tok.size();i++)
	mi->farbe[i/4][i%4]=tok.at(i).toDouble();
      cmbx->setCurrentIndex(mi->Farben-2);
      updateColors();
    }
  gdat.close();
  }
}


void GradDlg::savegradientpunktdat(){  
  QString fileName;;
  fileName = QFileDialog::getSaveFileName(this, tr("Save 'Gradient.dat' "), "Gradient.dat",//dirName,
					    "MolIso Gradient.dat(*.dat);;",&selectedFilter,QFileDialog::DontUseNativeDialog );
  if (!fileName.isEmpty()) {
    QFile gdat(fileName);
    gdat.open(QIODevice::WriteOnly);
    for (int i=0; i<mi->Farben; i++){
      gdat.write(QString("%1 %2 %3 %4\n").arg(mi->farbe[i][0]).arg(mi->farbe[i][1]).arg(mi->farbe[i][2]).arg(mi->farbe[i][3]).toLatin1());

    
    }
  gdat.close();
  }
}
void GradDlg::loadSettings(){  
  QSettings settings(QSettings::IniFormat,  QSettings::UserScope ,"Christian_B._Huebschle", "MoleCoolQt");
  settings.beginGroup("Version 0.1");
  valuesString=settings.value("Iso-Values").toString();
  int gsize = settings.beginReadArray("GradientColors");   
  gsize=(gsize>7)?6:gsize;
  gsize=(gsize<2)?6:gsize;
  for (int i = 0; i < gsize; i++) {
     settings.setArrayIndex(i+1);
     mi->farbe[i][0] = (GLfloat)settings.value("red"   ).toDouble();
     mi->farbe[i][1] = (GLfloat)settings.value("green" ).toDouble();
     mi->farbe[i][2] = (GLfloat)settings.value("blue"  ).toDouble();
     mi->farbe[i][3] = (GLfloat)settings.value("alpha" ).toDouble();
  }
  mi->Farben=gsize;
  settings.endArray();
  settings.endGroup();
}

void GradDlg::accept(){  
  QSettings settings(QSettings::IniFormat,  QSettings::UserScope ,"Christian_B._Huebschle", "MoleCoolQt" );
  settings.beginGroup("Version 0.1");
  settings.setValue("Iso-Values",isoValuesEdit->text());
  settings.beginWriteArray("GradientColors",mi->Farben);   
  for (int i = 0; i < mi->Farben; i++) {
    settings.setArrayIndex(i+1);
    settings.setValue("red",   mi->farbe[i][0]);
    settings.setValue("green", mi->farbe[i][1]);
    settings.setValue("blue",  mi->farbe[i][2]);
    settings.setValue("alpha", mi->farbe[i][3]);
  }
  settings.endArray();
  if (mmcb->checkState()==Qt::Checked){
    mi->fixmax=mxedit->value();
    mi->fixmin=miedit->value();
  }else{
    mi->fixmax=666.666;
    mi->fixmin=666.666;
  }
  settings.endGroup();
  QStringList sl=isoValuesEdit->text().split(QRegExp("\\s+"),QString::SkipEmptyParts);
  for (int i=0;i<sl.size();i++) {
      mi->isoValues.append(sl.at(i).toDouble());
  }
  done(QDialog::Accepted);
}
void GradDlg::mmstate(int state){
  if (state==Qt::Checked) {
    miedit->setEnabled(true);
    mxedit->setEnabled(true);
  }else{
    miedit->setEnabled(false);
    mxedit->setEnabled(false);
  }
}
gradientWidget::gradientWidget(QLinearGradient &gra,QWidget *parent) : QWidget(parent){  
  gradient=gra;
  gradient.setCoordinateMode(QGradient::StretchToDeviceMode);
  QPixmap pm(10, 10);
  QPainter pmp(&pm);
         pmp.fillRect(0, 0, 5, 5, Qt::lightGray);
         pmp.fillRect(5, 5, 5, 5, Qt::lightGray);
         pmp.fillRect(0, 5, 5, 5, Qt::darkGray);
         pmp.fillRect(5, 0, 5, 5, Qt::darkGray);
         pmp.end();
         QPalette pal = palette();
         pal.setBrush(backgroundRole(), QBrush(pm));
         setAutoFillBackground(true);
         setPalette(pal);
}
gradientWidget::gradientWidget(QColor color,QWidget *parent) : QWidget(parent){
  QLinearGradient gra(0,0,1,0);
  gra.setCoordinateMode(QGradient::StretchToDeviceMode);
  QColor c=color;
  c.setAlpha(0);
  gra.setColorAt(0,c);
  c.setAlpha(255);
  gra.setColorAt(1,c);
  gradient=gra;
  QPixmap pm(10, 10);
  QPainter pmp(&pm);
         pmp.fillRect(0, 0, 5, 5, Qt::lightGray);
         pmp.fillRect(5, 5, 5, 5, Qt::lightGray);
         pmp.fillRect(0, 5, 5, 5, Qt::darkGray);
         pmp.fillRect(5, 0, 5, 5, Qt::darkGray);
         pmp.end();
         QPalette pal = palette();
         pal.setBrush(backgroundRole(), QBrush(pm));
         setAutoFillBackground(true);
         setPalette(pal);
         setMinimumHeight(10);
}
void gradientWidget::setColor(QColor color){
    QLinearGradient gra(0,0,1,0);
  gra.setCoordinateMode(QGradient::StretchToDeviceMode);
  QColor c=color;
  c.setAlpha(0);
  gra.setColorAt(0,c);
  c.setAlpha(255);
  gra.setColorAt(1,c);
  gradient=gra;
}
void gradientWidget::generateShade(){
_shade = QImage(size(), QImage::Format_ARGB32_Premultiplied);
_shade.fill(0);
QPainter p(&_shade);
p.fillRect(rect(), gradient);
/*
             p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
             QLinearGradient fade(0, 0, 0, height());
             fade.setColorAt(0, QColor(0, 0, 0, 255));
             fade.setColorAt(1, QColor(0, 0, 0, 0));
             p.fillRect(rect(), fade);
// */
}
void gradientWidget::paintEvent(QPaintEvent *){ 
  generateShade();
  QPainter p(this);
  p.drawImage(0, 0, _shade);

}
