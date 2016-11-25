#include "hirsh.h"

Hirshfeld::Hirshfeld(molekul *externMole,QList<INP> *_au,QList<INP> *sel,QString *_dirName){
  mol=externMole;
  au= _au;
  selectedAtoms=sel;
  dirName=_dirName;
  iamr=NULL;
  setWindowTitle("Hirshfeld partitioning of dynamic density");
  expansion=3.0;
  nx=12;
  ny=12;
  nz=12;
  nxs = new QSpinBox(this);
  nys = new QSpinBox(this);
  nzs = new QSpinBox(this);
  nxs->setMinimum(12);
  nys->setMinimum(12);
  nzs->setMinimum(12);
  nxs->setMaximum(4608);
  nys->setMaximum(4608);
  nzs->setMaximum(4608);
  xSpacing=new QLabel(this);
  ySpacing=new QLabel(this);
  zSpacing=new QLabel(this);

  iamp =  new QPushButton("Compute IAM prior");
  iamp->setDisabled(true);
  loadIamp =  new QPushButton("Load IAM prior");
  loadIamp->setDisabled(true);
  iampre = new QLineEdit(IAMpriorname);
  iampre ->setDisabled(true);
  QDoubleSpinBox *expa = new QDoubleSpinBox(this);
  expa->setSingleStep(0.1);
  expa->setMinimum(1.5);
  expa->setMaximum(4.0);
  expa->setPrefix("Minimal distance of molecule atoms to box border:");
  expa->setSuffix(QString::fromUtf8("Å"));
  expa->setValue(2.0);

  hirp = new QPushButton("Compute Hirshfeld sharing function");
  QPushButton *iampbrs = new QPushButton("Browse");
  iammol  = new QLineEdit(IAMmolname);
  QPushButton *iammolbrs = new QPushButton("Browse");
  densin  = new QLineEdit(DensityIn);
  QPushButton *densinbrs = new QPushButton("Browse");
  densout = new QLineEdit(DensityOut);
  QPushButton *densoutbrs = new QPushButton("Browse");
  hirshsh = new QLineEdit(HirshShare);
  QPushButton *hirshbrs = new QPushButton("Browse");

  forts = new QProgressBar();
  forts->setMinimum(0);
  forts->hide();
  aStatusBar = new QLabel(this);
  staticBx=new QCheckBox("static IAM density",this);

  connect(staticBx,SIGNAL(stateChanged(int)),this,SLOT(statdyn()));
  connect(iampre, SIGNAL(textChanged ( const QString &)),this, SLOT(priorThere(const QString &)));
  connect(hirshsh,SIGNAL(textChanged ( const QString &)),this, SLOT(hirshThere(const QString &)));
  connect(iammol  ,SIGNAL(editingFinished ()),this,SLOT(handleIAMmolname()));
  connect(densin  ,SIGNAL(editingFinished ()),this,SLOT(handleDensityIn()));
  connect(densout ,SIGNAL(editingFinished ()),this,SLOT(handleDensityOut()));
  connect(expa,SIGNAL(valueChanged( double)),this, SLOT(changeExpansion(double ))); 

  connect(iamp,SIGNAL(released()),this,SLOT(iamprior()));
  connect(loadIamp,SIGNAL(released()),this,SLOT(loadIAMPrior()));
  connect(hirp,SIGNAL(released()),this,SLOT(expandBox()));
  connect(iampbrs,SIGNAL(released()),this,SLOT(browseIAMpriorname()));
  connect(iammolbrs,SIGNAL(released()),this,SLOT(browseIAMmolname()));
  connect(densinbrs,SIGNAL(released()),this,SLOT(browseDensityIn()));
  connect(densoutbrs,SIGNAL(released()),this,SLOT(browseDensityOut()));
  connect(hirshbrs,SIGNAL(released()),this,SLOT(browseHirshShare()));
  connect(nxs,SIGNAL(valueChanged(int)),this,SLOT(stickyNumbersx(int)));
  connect(nys,SIGNAL(valueChanged(int)),this,SLOT(stickyNumbersy(int)));
  connect(nzs,SIGNAL(valueChanged(int)),this,SLOT(stickyNumbersz(int)));

  QGridLayout *la =new QGridLayout(this);
  la->addWidget (nxs,        1,0);
  la->addWidget (nys,        1,2);
  la->addWidget (nzs,        1,4);
  la->addWidget (new QLabel("Gridpoints along a",this),1,1);
  la->addWidget (new QLabel("Gridpoints along b",this),1,3);
  la->addWidget (new QLabel("Gridpoints along c",this),1,5);
  la->addWidget (xSpacing,   2,0);
  la->addWidget (ySpacing,   2,2);
  la->addWidget (zSpacing,   2,4);
  la->addWidget (staticBx,   3,0); 
  la->addWidget (new QLabel("IAM prior output:",this),5,1,1,4);
  la->addWidget (iamp,       6,0);
  la->addWidget (loadIamp,   7,0);
  la->addWidget (expa,       7,1,1,4);
  la->addWidget (iampre,     6,1,1,4);
  la->addWidget (iampbrs,    6,5);
  la->addWidget (new QLabel("IAM molecule output:",this),9,1,1,4);
  la->addWidget (hirp,       10,0);
  la->addWidget (iammol,     10,1,1,4);
  la->addWidget (iammolbrs,  10,5);
  la->addWidget (new QLabel("Dynamic density input:",this),11,1,1,4);
  la->addWidget (densin,     12,1,1,4);
  la->addWidget (densinbrs,  12,5);
  la->addWidget (new QLabel("Molecular density output:",this),13,1,1,4);
  la->addWidget (densout,    14,1,1,4);
  la->addWidget (densoutbrs, 14,5);
  la->addWidget (new QLabel("Hirshfeld sharing function output:",this),15,1,1,4);
  la->addWidget (hirshsh,    16,1,1,4);
  la->addWidget (hirshbrs,   16,5);
  la->addWidget (forts,      25,0,1,10);
  la->addWidget (aStatusBar, 26,0,1,10); 

  prepareMagicNumbers();
  printf("asymmetric unit size %d\n",au->size());
  fillUnitCell(*au);
  int j,n1=0,n2=0,n3=0;
  j=(int)(mol->zelle.a*25);
  for (int i=0; magic.at(i)< j; i++)n1=magic.at(i+1);
  j=(int)(mol->zelle.b*25);
  for (int i=0; magic.at(i)< j; i++)n2=magic.at(i+1);
  j=(int)(mol->zelle.c*25);
  for (int i=0; magic.at(i)< j; i++)n3=magic.at(i+1);
  //printf("j%d %d %d %d \n",j,n1,n2,n3);
  nxs->setValue(n1);
  nys->setValue(n2);
  nzs->setValue(n3);
  xSpacing->setText(QString("Voxel dimension x = %1 %2").arg(mol->zelle.a/n1, 0, 'f', 4).arg(QString::fromUtf8("Å")));
  ySpacing->setText(QString("Voxel dimension y = %1 %2").arg(mol->zelle.b/n2, 0, 'f', 4).arg(QString::fromUtf8("Å")));
  zSpacing->setText(QString("Voxel dimension z = %1 %2").arg(mol->zelle.c/n3, 0, 'f', 4).arg(QString::fromUtf8("Å")));
  IAMpriorname = *dirName;
  IAMpriorname = IAMpriorname.section("/",0,-2);
  IAMpriorname.append("/iam_prior.raw");
  iampre->setText(IAMpriorname);
/*  
  IAMmolname = *dirName;
  IAMmolname = IAMmolname.section("/",0,-2);
  IAMmolname.append("/iam_mol.cube");
  iammol->setText(IAMmolname);
*/
  
  HirshShare = *dirName;
  HirshShare = HirshShare.section("/",0,-2);
  HirshShare.append("/hirshfeld.raw");
  hirshsh->setText(HirshShare);
  hirshThere(HirshShare);  
  priorThere(IAMpriorname);
  aStatusBar->setText("");
}

void Hirshfeld::statdyn(){
  fillUnitCell(*au);
}
void Hirshfeld::fillUnitCell(QList<INP> xdinp){
  HiAtom atm;
  /*
  for (int i=0; i< xdinp.size();i++){
    if (xdinp.at(i).OrdZahl<0) continue;
    atm.frac=xdinp.at(i).frac;
    atm.frac.x=fmod(atm.frac.x,1.0);
    atm.frac.y=fmod(atm.frac.y,1.0);
    atm.frac.z=fmod(atm.frac.z,1.0);
    atm.occ=xdinp.at(i).amul;
    atm.Bij= u2b(xdinp.at(i).uf);
    atm.iscat=findSFAC(xdinp.at(i).OrdZahl);
    fillmi(atm);
    printf("#%d  %d mass=%f %f %f %f %f B11%f %f %f %f %f %f\n",i,atm.iscat, 
        formfactors[atm.iscat].mass,
        atm.occ,
        atm.frac.x, atm.frac.y, atm.frac.z, 
      atm.Bij.m11,  
      atm.Bij.m22,  
      atm.Bij.m33,  
      atm.Bij.m12,  
      atm.Bij.m13,  
      atm.Bij.m23
      );  
  }*/
 Matrix u;
 atomsInCell.clear();
 sfacstr.clear();
 double aiz=0.0;
// FILE *xyz=fopen("test.xyz","wt");
// for (int n=0; n<mol->zelle.symmops.size(); n++){
   for (int i=0; i< xdinp.size();i++){
     if (xdinp.at(i).OrdZahl<0) continue;
  //   printf("%s an==%d jtf=%d %f\n",xdinp.at(i).atomname,xdinp.at(i).OrdZahl,xdinp.at(i).jtf,xdinp.at(i).uf.m22);
     atm.frac=clamp3(xdinp.at(i).frac);//*mol->zelle.symmops.at(n)+mol->zelle.trans.at(n));
//     atm.frac=clamp3(mol->zelle.symmops.at(n)*xdinp.at(i).frac+mol->zelle.trans.at(n));
     atm.occ=xdinp.at(i).amul;
//     u=(mol->zelle.symmops.at(n)*xdinp.at(i).uf)*transponse(mol->zelle.symmops.at(n));
//     atm.Bij= u2b(u);
    /* printf("%9.6f%9.6f%9.6f\n%9.6f%9.6f%9.6f\n%9.6f%9.6f%9.6f\n"
         , xdinp.at(i).uf.m11, xdinp.at(i).uf.m12, xdinp.at(i).uf.m13
         , xdinp.at(i).uf.m21, xdinp.at(i).uf.m22, xdinp.at(i).uf.m23
         , xdinp.at(i).uf.m31, xdinp.at(i).uf.m32, xdinp.at(i).uf.m33
         ); //  */
     u=xdinp.at(i).uf;
     if ((xdinp.at(i).jtf<2)||(xdinp.at(i).uf.m22==0.0)) { 
       xdinp[i].jtf=1;

       u.m22=u.m33=u.m11 = xdinp.at(i).uf.m11;
       u.m21=u.m12=xdinp.at(i).uf.m11*mol->zelle.cosrg;
       u.m31=u.m13=xdinp.at(i).uf.m11*mol->zelle.cosrb;
       u.m32=u.m23=xdinp.at(i).uf.m11*mol->zelle.cosra;
     }
     atm.Bij= u2b(u);
  /*   printf("%9.4f%9.4f%9.4f%9.4f%9.4f%9.4f%9.4f%9.4f%9.4f\n"
         , atm.Bij.m11, atm.Bij.m12, atm.Bij.m13
         , atm.Bij.m21, atm.Bij.m22, atm.Bij.m23
         , atm.Bij.m31, atm.Bij.m32, atm.Bij.m33
         );*/
     atm.iscat=findSFAC(xdinp.at(i).OrdZahl);
     if (!sfacstr.contains(formfactors[atm.iscat].lab)) {sfacstr.append(formfactors[atm.iscat].lab); sfacstr.append(" ");isfac.append("0 ");} 
     if (atm.iscat<0){
       qDebug()<<"Scattering factor not found !\n";
       return;
     }
     fillmi(atm);
   /*  printf("%4d.%-4d %-12s %12.6f %12.6f %12.6f %12.6f B11:%-9.6f B22:%-9.6f B33:%-9.6f B12:%-9.6f B13:%-9.6f B23:%-9.6f\n",i,n, 
         formfactors[atm.iscat].lab,
         atm.occ,
         atm.frac.x, atm.frac.y, atm.frac.z, 
         atm.Bij.m11,  
         atm.Bij.m22,  
         atm.Bij.m33,  
         atm.Bij.m12,  
         atm.Bij.m13,  
         atm.Bij.m23
      );  */
     aiz+=atm.occ;
  //   V3 cart;
  //   mol->frac2kart(atm.frac,cart);
  //   fprintf(xyz,"%s %12.6f %12.6f %12.6f\n",formfactors[atm.iscat].lab, cart.x, cart.y, cart.z);
     atomsInCell.append(atm);
   }
// }
/* V3 frac;
 V3 cart;
 mol->frac2kart(V3(0,0,0),cart);
 fprintf(xyz,"Q %12.6f %12.6f %12.6f\n", cart.x, cart.y, cart.z);
 mol->frac2kart(V3(1,0,0),cart);
 fprintf(xyz,"Q %12.6f %12.6f %12.6f\n", cart.x, cart.y, cart.z);
 mol->frac2kart(V3(0,1,0),cart);
 fprintf(xyz,"Q %12.6f %12.6f %12.6f\n", cart.x, cart.y, cart.z);
 mol->frac2kart(V3(0,0,1),cart);
 fprintf(xyz,"Q %12.6f %12.6f %12.6f\n", cart.x, cart.y, cart.z);
 fclose(xyz);*/
 printf("%d atoms in asymmetric unit. Real:%g\n",atomsInCell.size(),aiz);
}

int Hirshfeld::findSFAC(int an){
  //  printf("%s p+:%d e-:%d an%d\n",hat.lab, hat.prot, hat.elec, an+1);
  for (int i=0; i<115; i++){
    //    printf("#%d %s p+:%d e-:%d an%d\n",i, formfactors[i].lab, formfactors[i].prot, formfactors[i].elec, an+1);
    if ((formfactors[i].prot==formfactors[i].elec)//neutral atoms
        &&(formfactors[i].prot==an+1)){ 
   /*   double as=0.0;
      for (int j=0; j<6; j++)
        as+=formfactors[i].ab6[j*2];
      printf("#%d %s p+:%d e-:%d an%d %f\n",i, formfactors[i].lab, formfactors[i].prot, formfactors[i].elec, an+1,as);*/
      return i;
    }
  }
  return -1;
}

void Hirshfeld::changeExpansion(double d){
expansion=d;
}

void Hirshfeld::fillmi(HiAtom &ha){
  for (int i=0; i<6; i++){
    if (staticBx->isChecked()) ha.M=(mol->zelle.Gi*(formfactors[ha.iscat].ab6[i*2+1]*0.25));
    else ha.M=(mol->zelle.Gi*(formfactors[ha.iscat].ab6[i*2+1]*0.25))+ha.Bij;
    ha.Mi[i]=inverse(ha.M);
    double d=determinant(ha.Mi[i]);
    ha.pref[i]=sqrt(d)*ha.occ*formfactors[ha.iscat].ab6[i*2]*pisqrt3/mol->zelle.V;
    ha.Mi[i]=ha.Mi[i]*(-pi2);
    //printf("%spref%g %g  occ %g\n",formfactors[ha.iscat].lab,ha.pref[i],sqrt(d),ha.occ);
    if (ha.pref[i]<acc) ha.oros[i]=V3(0.02,0.02,0.02);// simply a small number that includes only the small surrounding of the atomic center
    else 
      ha.oros[i]=V3(
          sqrt(-log(acc/ha.pref[i])*ha.M.m11/pi2),
          sqrt(-log(acc/ha.pref[i])*ha.M.m22/pi2),
          sqrt(-log(acc/ha.pref[i])*ha.M.m33/pi2));
//  printf("Radius of significance[%d]: %12.5g %12.5g %12.5g\n",i,ha.oros[i].x,ha.oros[i].y,ha.oros[i].z);
  }
}

double Hirshfeld::calcRho(HiAtom ha, int ix, int iy, int iz){
  double rho=0.0;
  V3 pos0=V3((double)ix/nx,(double)iy/ny,(double)iz/nz)-ha.frac;
  V3 minsh,maxsh;
  V3 po,pa,pu;
  for (int i=0; i<6; i++){
    pa=pos0-ha.oros[i];
    pu=pos0+ha.oros[i];
    minsh=ceil3(pa);
    maxsh=floor3(pu);
    //printf("shell %g %g %g  %g %g %g \n",minsh.x,minsh.y,minsh.z,maxsh.x,maxsh.y,maxsh.z);
    for (int ix=minsh.x;ix<=maxsh.x;ix++)
      for (int iy=minsh.y;iy<=maxsh.y;iy++)
        for (int iz=minsh.z;iz<=maxsh.z;iz++){
          po=pos0-V3(ix,iy,iz);
          rho+=ha.pref[i]*exp(po*(ha.Mi[i]*po));
        }
  }
return rho;
}

Matrix Hirshfeld::u2b(const Matrix u){
  Matrix erg;
  //Matrix erg2=(u*z.Gi)*twopi2;
  erg.m11=u.m11*twopi2*mol->zelle.as*mol->zelle.as;
  erg.m22=u.m22*twopi2*mol->zelle.bs*mol->zelle.bs;
  erg.m33=u.m33*twopi2*mol->zelle.cs*mol->zelle.cs;
  erg.m12=erg.m21=u.m12*twopi2*mol->zelle.bs*mol->zelle.as;
  erg.m13=erg.m31=u.m13*twopi2*mol->zelle.cs*mol->zelle.as;
  erg.m23=erg.m32=u.m23*twopi2*mol->zelle.cs*mol->zelle.bs;
//  printf("Uij: %9.6f%9.6f%9.6f%9.6f%9.6f%9.6f\n",u.m11,u.m22,u.m33,u.m12,u.m13,u.m23);
//  printf("Bij: %9.6f%9.6f%9.6f%9.6f%9.6f%9.6f\n",erg.m11,erg.m22,erg.m33,erg.m12,erg.m13,erg.m23);
  //printf("erg:\n%9.5f %9.5f %9.5f\n%9.5f %9.5f %9.5f\n%9.5f %9.5f %9.5f\n",erg.m11,erg.m12,erg.m13,erg.m12,erg.m22,erg.m23,erg.m13,erg.m23,erg.m33);
  //printf("erg2:\n%9.5f %9.5f %9.5f\n%9.5f %9.5f %9.5f\n%9.5f %9.5f %9.5f\n",erg2.m11,erg2.m12,erg2.m13,erg2.m12,erg2.m22,erg2.m23,erg2.m13,erg2.m23,erg2.m33);
  return erg;

}

void Hirshfeld::browseIAMpriorname(){
  selectedFilter="BayMEM binary file (*.raw);;";
  QString s= QFileDialog::getSaveFileName(this, tr("Write IAM prior file "), IAMpriorname,
        "Jana2006 m81-Files (*.m81);;"
        "BayMEM binary file (*.raw);;"
        "GAUSSIAN Cube-Files (*.cube);;" 
        
        ,&selectedFilter,QFileDialog::DontUseNativeDialog );

//  qDebug()<<"ok"<<s<<IAMpriorname;
  iampre->setText(s); 
      
  IAMpriorname  = iampre->text();
}

void Hirshfeld::browseIAMmolname(){
  selectedFilter="BayMEM binary file (*.raw);;";
  QString s= QFileDialog::getSaveFileName(this, tr("Write IAM molecule file "),IAMmolname ,
        "Jana2006 m81-Files (*.m81);;"
        "BayMEM binary file (*.raw);;"
        "GAUSSIAN Cube-Files (*.cube);;" 
        
        ,&selectedFilter,QFileDialog::DontUseNativeDialog );

  iammol->setText(s); 
      
  IAMmolname = iammol->text();
}

void Hirshfeld::browseDensityIn (){
  selectedFilter="BayMEM binary file (*.raw);;";
  QString s= QFileDialog::getOpenFileName(this, tr("Read dynamic density file "),DensityIn  ,
      "Jana2006 m81-Files (*.m81);;"
      "BayMEM binary file (*.raw);;"
      "GAUSSIAN Cube-Files (*.cube);;" 
      ,&selectedFilter,QFileDialog::DontUseNativeDialog );

  densin->setText(s); 
  DensityIn  = densin->text();
  if ((!DensityIn.isEmpty())&&(DensityIn.endsWith(".raw"))){
    FILE *raw=fopen(DensityIn.toStdString().c_str(),"rt");
    BMhead bm;
    //fread(void *ptr, size_t size, size_t nmemb, FILE *stream); 
    size_t siz=fread(&bm,sizeof(BMhead),1,raw);
    if ((siz!=sizeof(BMhead))&&(bm.h!=bm.f)&&(bm.h!=76)){
      //fprintf(stderr,"siz %ld==%ld %d==%d (should be 76)\n",siz,sizeof(BMhead),bm.h,bm.f);
      fclose(raw);
      s="";
      densin->setText(s); 
      DensityIn  = densin->text();
      return;
    }
    fclose(raw);
    nx=bm.nn1;
    ny=bm.nn2;
    nz=bm.nn3;
    nxs->setValue(bm.nn1);
    nys->setValue(bm.nn2);
    nzs->setValue(bm.nn3);
    ntot=nx*ny*nz;
  }else if((!DensityIn.isEmpty())&&(DensityIn.endsWith(".m81"))){
    QFile jh(DensityIn);
    //FILE *f;
    jh.open(QIODevice::ReadOnly);
    struct T{
      int nx[6],nxny,nmap;
      float xymin[12];
      float dx[6];
      int iorien[6],mapa,nsubs;
      bool sat;
    };
    T t;
    jh.read((char*)&t,sizeof(T));
    int idim=0,ups=0,aua=-1;
    for (int i=0; i<6; i++) {
      idim+=ups=t.iorien[i];
      //  printf("%d\n",ups);
      if (aua>ups) {
        qDebug()<<"m81 file axis order must be 1 2 3, x y z!";
        s="";
        densin->setText(s); 
        DensityIn  = densin->text();
      }
    }
    if (idim!=21) {
      s="";
      densin->setText(s); 
      DensityIn  = densin->text();
      qDebug()<<".m81 file suport only for 3 Dimensions, sorry"<<idim;
    }

    //qDebug()<<t.nx[0]<<t.nx[1]<<t.nx[2]<<t.nxny;
    jh.close();
    nx =(t.iorien[0]==1)?t.nx[0]:(t.iorien[1])==1?t.nx[1]:t.nx[2];
    ny =(t.iorien[0]==2)?t.nx[0]:(t.iorien[1])==2?t.nx[1]:t.nx[2];
    nz =(t.iorien[0]==3)?t.nx[0]:(t.iorien[1])==3?t.nx[1]:t.nx[2];
    nxs->setValue(nx);
    nys->setValue(ny);
    nzs->setValue(nz);
    ntot=nx*ny*nz;
  }
  priorThere(IAMpriorname);
}

void Hirshfeld::browseDensityOut(){
  selectedFilter="BayMEM binary file (*.raw);;";
  QString s= QFileDialog::getSaveFileName(this, tr("Write moleculr density file "),DensityOut ,
        "Jana2006 m81-Files (*.m81);;"
        "BayMEM binary file (*.raw);;"
        "GAUSSIAN Cube-Files (*.cube);;" 
        ,&selectedFilter,QFileDialog::DontUseNativeDialog );
  densout->setText(s); 
  DensityOut= densout->text();
}

void Hirshfeld::browseHirshShare(){
  selectedFilter="BayMEM binary file (*.raw);;";
  QString s = QFileDialog::getSaveFileName(this, tr("Write IAM prior file "),HirshShare ,
        "Jana2006 m81-Files (*.m81);;"
        "BayMEM binary file (*.raw);;"
        "GAUSSIAN Cube-Files (*.cube);;" 
        ,&selectedFilter,QFileDialog::DontUseNativeDialog );
  hirshsh->setText(s); 
  HirshShare= hirshsh->text();
}

void Hirshfeld::handleIAMpriorname(){
  QString s = iampre->text();
  if ((s.endsWith(".cube"))||(s.endsWith(".m81"))||(s.endsWith(".raw"))) 
    IAMpriorname=s;
}

void Hirshfeld::handleIAMmolname(){
  QString s = iammol->text();
  if ((s.endsWith(".cube"))||(s.endsWith(".m81"))||(s.endsWith(".raw"))) 
    IAMmolname=s;
}

void Hirshfeld::handleDensityIn (){
  QString s = densin->text();
  if ((s.endsWith(".cube"))||(s.endsWith(".m81"))||(s.endsWith(".raw"))) 
    if (QFile::exists(s)) DensityIn=s; 
}

void Hirshfeld::handleDensityOut(){
  QString s = densout->text();
  if ((s.endsWith(".cube"))||(s.endsWith(".m81"))||(s.endsWith(".raw"))) 
    DensityOut=s;
}

void Hirshfeld::handleHirshShare(){
  QString s = hirshsh->text();
  if ((s.endsWith(".cube"))||(s.endsWith(".m81"))||(s.endsWith(".raw"))) 
    HirshShare=s; 
}

void Hirshfeld::prepareMagicNumbers(){ 
  int z=1;
  int mustbe=2;
  V3 t,zaehler,nenner;
  for (int i=0; i<mol->zelle.trans.size(); i++){
    t=mol->zelle.trans.at(i);
    double egal;
    for (int g=1;g<5; g++){
        nenner.x=(t.x<0)?-g:g;;
        zaehler.x=(int)((t.x)*g);
        if (fabs(modf(t.x*g,&egal))<0.05) break;
    }
    for (int g=1;g<5; g++){
        nenner.y=(t.y<0)?-g:g;
        zaehler.y=(int)((t.y)*g);
        if (fabs(modf(t.y*g,&egal))<0.05) break;
    }
    for (int g=1;g<5; g++){
        nenner.z=(t.z<0)?-g:g;
        zaehler.z=(int)((t.z)*g);
        if (fabs(modf(t.z*g,&egal))<0.05) break;
    }
    mustbe=qMax((int) nenner.x,mustbe);  
    mustbe=qMax((int) nenner.y,mustbe);  
    mustbe=qMax((int) nenner.z,mustbe);  
  } 
  printf("pixel must be multiples of %d\n",mustbe);
  for (int i=0; i<15; i++){
    for (int j=0; j<7; j++){
      z=(int)(pow(2,i)*pow(3,j));
      if ((z>11)&&(z<5000)&&(0==(z%mustbe)))
        magic.append(z);
    }
  }
  qSort(magic.begin(),magic.end());
}

void Hirshfeld::stickyNumbersx(int iv){
  xSpacing->setText(QString("Voxel dimension x = %1 %2").arg(mol->zelle.a/nx, 0, 'f', 4).arg(QString::fromUtf8("Å")));
  if (nx==iv) return;
  int j;  
  for (j=0; magic.at(j)< iv; j++);

 // printf("what shall %d %d<>%d %d\n",magic.at(j],iv,nx,magic.at(j+1]);
  if (nx>iv) {
    nx=magic.at(j-1);
    nxs->setValue(magic.at(j-1));
  }
  else if (nx<iv) {
    nxs->setValue(magic.at(j));
    nx=magic.at(j);
  }
  if (mol->zelle.a==mol->zelle.b) {ny=nx;nys->setValue(nx);}
  if (mol->zelle.a==mol->zelle.c) {nz=nx;nzs->setValue(nx);}
  xSpacing->setText(QString("Voxel dimension x = %1 %2").arg(mol->zelle.a/nx, 0, 'f', 4).arg(QString::fromUtf8("Å")));
//  nxs->setValue(n1);
  priorThere(IAMpriorname);
}

void Hirshfeld::stickyNumbersy(int iv){
  ySpacing->setText(QString("Voxel dimension y = %1 %2").arg(mol->zelle.b/ny, 0, 'f', 4).arg(QString::fromUtf8("Å")));
  if (ny==iv) return;
  int j;  
  for (j=0; magic.at(j)< iv; j++);

 // printf("what shall %d %d<>%d %d\n",magic.at(j),iv,nx,magic.at(j+1));
  if (ny>iv) {
    ny=magic.at(j-1);
    nys->setValue(magic.at(j-1));
  }
  else if (ny<iv) {
    nys->setValue(magic.at(j));
    ny=magic.at(j);
  }
  if (mol->zelle.b==mol->zelle.a) {nx=ny;nxs->setValue(ny);}
  if (mol->zelle.b==mol->zelle.c) {nz=ny;nzs->setValue(ny);}
  ySpacing->setText(QString("Voxel dimension y = %1 %2").arg(mol->zelle.b/ny, 0, 'f', 4).arg(QString::fromUtf8("Å")));
  priorThere(IAMpriorname);
}

void Hirshfeld::stickyNumbersz(int iv){
  zSpacing->setText(QString("Voxel dimension z = %1 %2").arg(mol->zelle.c/nz, 0, 'f', 4).arg(QString::fromUtf8("Å")));
  if (nz==iv) return;
  int j;  
  for (j=0; magic.at(j)< iv; j++);

 // printf("what shall %d %d<>%d %d\n",magic.at(j),iv,nx,magic.at(j+1));
  if (nz>iv) {
    nz=magic.at(j-1);
    nzs->setValue(magic.at(j-1));
  }
  else if (nz<iv) {
    nzs->setValue(magic.at(j));
    nz=magic.at(j);
  }
  if (mol->zelle.c==mol->zelle.a) {nx=nz;nxs->setValue(nz);}
  if (mol->zelle.c==mol->zelle.b) {ny=nz;nys->setValue(nz);}
  zSpacing->setText(QString("Voxel dimension z = %1 %2").arg(mol->zelle.c/nz, 0, 'f', 4).arg(QString::fromUtf8("Å")));
  //printf("2%d 3%d 4%d 6%d\n",nz%2,nz%3,nz%4,nz%6);
  priorThere(IAMpriorname);
}

double Hirshfeld::checkSymmOfMap(){
  int itot,stot,sx,sy,sz,ck=0;
//  int tx,ty,tz;
  double err=0.0,er,summ=0.0;
  V3 p;//,o;
  for (int n=0; n<mol->zelle.symmops.size(); n++){
    char syyy[400];
    strcpy(syyy,mol->encodeSymm(n).toStdString().c_str());        
    for (int iz=0; iz<nz; iz++){
      for (int iy=0; iy<ny; iy++){
        for (int ix=0; ix<nx; ix++){
          itot=ix+(iy+iz*ny)*nx;
          if (n==0) summ+=iamr[itot];
     //     if (done[itot]==1) continue;
//          o=V3(1.0*ix/nx,1.0*iy/ny,1.0*iz/nz);
//          tx=((int)(o.x*nx+(9*nx)))%nx;
//          ty=((int)(o.y*ny+(9*ny)))%ny;
//          tz=((int)(o.z*nz+(9*nz)))%nz;
          p=clamp3(mol->zelle.symmops.at(n)*V3(1.0*ix/nx,1.0*iy/ny,1.0*iz/nz)  + mol->zelle.trans.at(n));
//          printf("%f %f %f -> %f %f %f\n",o.x,o.y,o.z,p.x,p.y,p.z);
          sx=((int)(p.x*nx+(9*nx)+0.5))%nx;
          sy=((int)(p.y*ny+(9*ny)+0.5))%ny;
          sz=((int)(p.z*nz+(9*nz)+0.5))%nz;

          stot=sx+(sy+sz*ny)*nx;
          er=fabs(iamr[itot]-iamr[stot]);
      /*    if (er>0.0001) {
            printf("%s \n", syyy);
            printf("n:%d %d %d %d -> %d %d %d %f==%f %f  \n",n+1, ix, iy, iz, sx, sy, sz,iamr[itot],iamr[stot],er);//,tx,ty,tz);
          }*/
          err+=er;
          ck++;

        }
      }
    }
  }
  double sosu=(int) (summ/ntot*mol->zelle.V+.5);
  double add= sosu-(summ/ntot*mol->zelle.V);
  printf("summ %f err %f sosu: %f %f %d %d \n",summ/ntot*mol->zelle.V,err,sosu,add, ntot, ck);
  add/=mol->zelle.V;
   for (int i=0; i<ntot; i++)iamr[i]+=add;
  return err;

}

void Hirshfeld::applySymmOnMap(){
  int syms=mol->zelle.symmops.size();
  printf("syms %d\n",syms);
  int *eqiv=(int*) malloc(sizeof(int)*ntot*syms);
  int *pixmult=(int*) malloc(sizeof(int)*ntot);
  double *R=(double*) malloc(sizeof(double)*ntot);
  int itot,stot,sx,sy,sz;
  V3 p;
  double rho;
  for (int iz=0; iz<nz; iz++){
    for (int iy=0; iy<ny; iy++){
      for (int ix=0; ix<nx; ix++){
        itot=ix+(iy+iz*ny)*nx;
        pixmult[itot]=0;
        for (int n=0; n<syms; n++){
          p=clamp3(mol->zelle.symmops.at(n) * V3(1.0*ix/nx,1.0*iy/ny,1.0*iz/nz) + mol->zelle.trans.at(n));
          sx=((int)(p.x*nx+(9*nx)+0.5))%nx;
          sy=((int)(p.y*ny+(9*ny)+0.5))%ny;
          sz=((int)(p.z*nz+(9*nz)+0.5))%nz;
          stot=sx+(sy+sz*ny)*nx;
          if (itot==stot) pixmult[stot]++;
      //    printf ("%d %d %d -> %d %d %d  %d->%d n%d %d \n",ix,iy,iz,sx,sy,sz,itot,stot,n,itot+n*ntot);
          //eqiv[itot+n*ntot]=((n>0)&&(itot==stot))?-1:stot;
          eqiv[itot+n*ntot]=stot;
        }
      }
    }
  }

  for (int i=0; i<ntot; i++) R[i]=0;
  for (int iz=0; iz<nz; iz++){
    for (int iy=0; iy<ny; iy++){
      for (int ix=0; ix<nx; ix++){
        itot=ix+(iy+iz*ny)*nx;
        rho=0.0;
        for (int n=0; n<syms; n++) {
          rho+=iamr[eqiv[itot+n*ntot]];
         // printf("%d %d mul%d %f %f\n", itot,eqiv[itot+n*ntot],pixmult[itot],iamr[eqiv[itot+n*ntot]],rho);
        }
        for (int n=0; n<syms; n++) R[eqiv[itot+n*ntot]]=rho;
      }
    }
  }
  for (int i=0; i<ntot; i++) iamr[i]=R[i];//pixmult[i];
  printf("syms %d\n",syms);
}

void Hirshfeld::iamprior(){
  ntot=nx*ny*nz;
  iamr  =(double*) malloc(sizeof(double)*ntot);
  if (iamr==NULL) {
  qDebug()<<"not enough memory!";
  return;
  }
  int itot=0;
  for (int i=0; i<ntot; i++){
    iamr[i]=0.0;
  }
  double max=0.0,min=1e99,cksum=0.0;
  forts->setMaximum(atomsInCell.size()+1);
  forts->show();
  update();
  for (int i=0; i<atomsInCell.size(); i++){
    forts->setValue(i);
    aStatusBar->setText(QString("Atom %1 of %2").arg(i+1).arg(atomsInCell.size())); 
//  printf("Atom %d of %d \n",i+1,atomsInCell.size());
    for (int iz=0; iz<nz; iz++){
      for (int iy=0; iy<ny; iy++){
        for (int ix=0; ix<nx; ix++){
          itot=ix+(iy+iz*ny)*nx;
          iamr[itot]+=calcRho(atomsInCell.at(i),ix,iy,iz);
        }
      }
    }
  }
  applySymmOnMap();
  for (int i=0; i<ntot; i++){
    max=fmax(iamr[i],max);
    min=fmin(iamr[i],min);
    cksum+=iamr[i];
  }
  cksum*=mol->zelle.V/itot;
  printf("%g %g %g\n",min,max,cksum);
  aStatusBar->setText(QString("...now writing file. max rho = %1 eA^-3 min rho = %2 eA^-3 sum %3 electrons" ).arg(max,0,'g',5).arg(min,0,'g',5).arg(cksum,0,'g',5)); 
  forts->setValue(atomsInCell.size());
  update();
//  printf("now writing cube max=%g\n",max);
  writeMap(iamr,IAMpriorname);
  printf("back from cube %g\n",checkSymmOfMap()); 
  forts->setValue(atomsInCell.size()+1);
  aStatusBar->setText(QString("IAM rho finished! max rho = %1 eA^-3 min rho = %2 eA^-3").arg(max,0,'g',5).arg(min,0,'g',5));
  forts->hide();
  hirshThere(HirshShare);


}

void Hirshfeld::loadIAMPrior(){
  ntot=nx*ny*nz;
  iamr  =(double*) malloc(sizeof(double)*ntot);
  readMap(iamr,IAMpriorname);
  hirshThere(hirshsh->text());

}

void Hirshfeld::expandBox(){
  aStatusBar->setText(QString("Expanding unit cell box for hirshfeld pratitioning..."));
  repaint();
  forts->show();
  forts->setValue(0);
  atomsInMolecule.clear();
  int xmax=-1e+9,
      xmin= 1e+9,
      ymax=-1e+9,
      ymin= 1e+9,
      zmax=-1e+9,
      zmin= 1e+9;
  int xx,yy,zz,anr=0;
  V3 mid=V3(0.0,0.0,0.0);
  if (selectedAtoms->isEmpty()){
  for (int i=0; i<au->size();i++){
    if (au->at(i).OrdZahl<0) continue;
    mid+=au->at(i).frac;
    anr++;
    xx=au->at(i).frac.x*nx;
    yy=au->at(i).frac.y*ny;
    zz=au->at(i).frac.z*nz;
    //printf("%d %d %d %f %f %f\n",xx,yy,zz,au->at(i).frac.x,au->at(i).frac.y,au->at(i).frac.z);
    xmax=(xmax<xx)?xx:xmax;
    ymax=(ymax<yy)?yy:ymax;
    zmax=(zmax<zz)?zz:zmax;
    xmin=(xmin>xx)?xx:xmin;
    ymin=(ymin>yy)?yy:ymin;
    zmin=(zmin>zz)?zz:zmin;

  }
  }else { 
  for (int i=0; i<selectedAtoms->size();i++){
    if (selectedAtoms->at(i).OrdZahl<0) continue;
    mid+=selectedAtoms->at(i).frac;
    anr++;
    xx=selectedAtoms->at(i).frac.x*nx;
    yy=selectedAtoms->at(i).frac.y*ny;
    zz=selectedAtoms->at(i).frac.z*nz;
    //printf("%d %d %d %f %f %f\n",xx,yy,zz,selectedAtoms->at(i).frac.x,selectedAtoms->at(i).frac.y,selectedAtoms->at(i).frac.z);
    xmax=(xmax<xx)?xx:xmax;
    ymax=(ymax<yy)?yy:ymax;
    zmax=(zmax<zz)?zz:zmax;
    xmin=(xmin>xx)?xx:xmin;
    ymin=(ymin>yy)?yy:ymin;
    zmin=(zmin>zz)?zz:zmin;

  }
  }
    xmin-=(int) round((expansion*nx)/mol->zelle.a);//+2Angstroems around each atom
    ymin-=(int) round((expansion*ny)/mol->zelle.b);
    zmin-=(int) round((expansion*nz)/mol->zelle.c);
    xmax+=(int) round((expansion*nx)/mol->zelle.a);
    ymax+=(int) round((expansion*ny)/mol->zelle.b);
    zmax+=(int) round((expansion*nz)/mol->zelle.c);
    printf("%d<%d %d\n%d<%d %d\n%d<%d %d\n",xmin,xmax,xmax-xmin ,ymin,ymax,ymax-ymin,zmin,zmax,zmax-zmin);
    mid*=1.0/anr;
    forts->setMaximum(anr);
    forts->show();
    update();
    printf("%g %g\n",(xmax+xmin)/(2.0*nx),mid.x);
    printf("%g %g\n",(ymax+ymin)/(2.0*ny),mid.y);
    printf("%g %g\n",(zmax+zmin)/(2.0*nz),mid.z);
    V3 midd=V3((xmax+xmin)/(2.0*nx),(ymax+ymin)/(2.0*ny),(zmax+zmin)/(2.0*nz));
    printf("%g %g %g\n",mid.x*nx,mid.y*ny,mid.z*nz);  
//    int ofx=(int) 0;//round(mid.x*.vox.x);
//    int ofy=(int) 0;//round(mid.y*z.vox.y);
//    int ofz=(int) 0;//round(mid.z*z.vox.z);
    entot=(xmax-xmin)*(ymax-ymin)*(zmax-zmin);
    enx=(xmax-xmin);
    eny=(ymax-ymin);
    enz=(zmax-zmin);
    aStatusBar->setText(QString("Box size: %1 x %2 x %3 = %4 voxel").arg(enx).arg(eny).arg(enz).arg(entot));
    update();
    int ecoo=0,ei;
    crys_rho=(double*) malloc(sizeof(double)*entot);
    mole_rho=(double*) malloc(sizeof(double)*entot);
    atom_rho=(double*) malloc(sizeof(double)*entot);
    hirsh_share=(double*) malloc(sizeof(double)*entot);
    C =NULL;
    _C=NULL;
    expandetrho2=NULL;
    density_out=NULL;
    if (!DensityOut.isEmpty()) {
//      qDebug()<<DensityOut<<"DensityOut";
      density_out=(double*) malloc(sizeof(double)*entot);
    }
    if (!DensityIn.isEmpty()) {
      _C=C=(double*)malloc(sizeof(double)*ntot);
      readMap(C,DensityIn);
      if (C==NULL) free(_C);
//      qDebug()<<DensityIn<<"DensityIn"<<(C==NULL);
      expandetrho2=(double*) malloc(sizeof(double)*entot);
    }
    int *inteX=(int*)malloc(sizeof(int)*entot);
    int *inteY=(int*)malloc(sizeof(int)*entot);
    int *inteZ=(int*)malloc(sizeof(int)*entot);
    for(int iii=zmin; iii<zmax; iii++) 
      for(int ii=ymin; ii<ymax;  ii++) 
        for(int i=xmin; i<xmax;   i++){
          int ix,iy,iz;
          ix=(9*nx+i)%nx;
          iy=(9*ny+ii)%ny;
          iz=(9*nz+iii)%nz;
          ei=ix+nx*(iy+iz*ny);
          /*
             int nx,ny,nz;
             nx=i-xmin;
             ny=ii-ymin;
             nz=iii-zmin;
             ecoo=nx+en1*(ny+nz*en2);
             */
          crys_rho[ecoo]=iamr[ei];      
          if (C!=NULL)expandetrho2[ecoo]=C[ei];      
          inteX[ecoo]=i   ;
          inteY[ecoo]=ii  ;
          inteZ[ecoo]=iii ;
          ecoo++;
        }
    expandedCell.a  = (double)enx/nx*mol->zelle.a;
    expandedCell.b  = (double)eny/ny*mol->zelle.b;
    expandedCell.c  = (double)enz/nz*mol->zelle.c;
    expandedCell.al = mol->zelle.al; 
    expandedCell.be = mol->zelle.be; 
    expandedCell.ga = mol->zelle.ga; 
    expandedCell.V  = mol->zelle.V*entot/ntot;
    setup_zelle();
    printf("expanded 'empty' cell: %f %f %f %f %f %f %f\n"
        ,expandedCell.a  
        ,expandedCell.b  
        ,expandedCell.c  
        ,expandedCell.al 
        ,expandedCell.be 
        ,expandedCell.ga 
        ,expandedCell.V  );
//    FILE *esx=fopen("expandedcell.res","wt");
    FILE *esx=fopen("expandedcell.res","wt");
    fprintf(esx,"TITL %s\nCELL 1.00 %f %f %f %f %f %f\nZERR 0 0 0 0 0 0 0\nLATT -1\nSFAC %s\nUNIT %s\nFVAR 1.0\n","not to refine!"
        ,expandedCell.a 
        ,expandedCell.b 
        ,expandedCell.c 
        ,expandedCell.al
        ,expandedCell.be
        ,expandedCell.ga
        ,sfacstr.toStdString().c_str()
        ,isfac.toStdString().c_str()
        );
    HiAtom atm;
    Matrix u;
    int molsize=anr;
    anr=0;

    if (selectedAtoms->isEmpty()){
      for (int i=0; i<au->size();i++){
        if (au->at(i).OrdZahl<0) continue;
        //      for (int icoo=0; icoo<entot; icoo++){atom_rho[icoo]=0.0;}
        forts->setValue(anr++);
        aStatusBar->setText(QString("Computing IAM rho for atom %1 of %2").arg(anr).arg(molsize));
        update();
        atm.frac=V3((au->at(i).frac.x-midd.x)*nx/enx+0.5,(au->at(i).frac.y-midd.y)*ny/eny+0.5,(au->at(i).frac.z-midd.z)*nz/enz+0.5);
        atm.occ=fmax(0.0,fmin(1.0,au->at(i).amul*au->at(i).multiplicity));
        if ((au->at(i).jtf<2)||(au->at(i).uf.m22==0.0)) { 
          fprintf(esx,"%-4s %4d %9.5f %9.5f %9.5f %9.5f %9.5f\n",
              au->at(i).atomname,
              au->at(i).atomtype,
              atm.frac.x,//
              atm.frac.y,//
              atm.frac.z,//
              au->at(i).amul,
              au->at(i).uf.m11);
          double bb=au->at(i).uf.m11;
          u=Matrix(
              bb,bb*mol->zelle.cosrg,bb*mol->zelle.cosrb,
              bb*mol->zelle.cosrg,bb,bb*mol->zelle.cosra,
              bb*mol->zelle.cosrb,bb*mol->zelle.cosra,bb);
        }
        else {
          fprintf(esx,"%-4s %4d %9.5f %9.5f %9.5f %9.5f %9.5f %9.5f = \n  %9.5f %9.5f %9.5f %9.5f\n",
              au->at(i).atomname,
              au->at(i).atomtype,
              atm.frac.x,//
              atm.frac.y,//
              atm.frac.z,//
              au->at(i).amul,au->at(i).uf.m11,au->at(i).uf.m22,au->at(i).uf.m33,au->at(i).uf.m23,au->at(i).uf.m13,au->at(i).uf.m12);
          u=au->at(i).uf;
        }
        atm.Bij= u2b(u);
        atm.iscat=findSFAC(au->at(i).OrdZahl);
        if (atm.iscat<0){
          qDebug()<<"Scattering factor not found !\n";
          return;
        }
        fillmi(atm);
        atomsInMolecule.append(atm);
        double argu,rho; 
        for (int icoo=0; icoo<entot; icoo++){
          V3 pos1=V3((double)inteX[icoo]/nx,(double)inteY[icoo]/ny,(double)inteZ[icoo]/nz);
          V3 pos0=pos1-au->at(i).frac;
          rho=0.0;
          for (int i = 0; i < 6; i++){
            argu=pos0*(atm.Mi[i]*pos0);
            rho+=atm.pref[i]*exp(argu);
          }
          atom_rho[icoo]=rho;
          mole_rho[icoo]+=rho;

        }
        if (expandetrho2!=NULL) hirsh41atom(au->at(i),atom_rho,crys_rho,expandetrho2);
      }
    }
    else{
      mol->multiplicity(*selectedAtoms);
    for (int i=0; i<selectedAtoms->size();i++){
      if (selectedAtoms->at(i).OrdZahl<0) continue;
//      for (int icoo=0; icoo<entot; icoo++){atom_rho[icoo]=0.0;}
      forts->setValue(anr++);
      aStatusBar->setText(QString("Computing IAM rho for atom %1 of %2").arg(anr).arg(molsize));
      update();
      atm.frac=V3((selectedAtoms->at(i).frac.x-midd.x)*nx/enx+0.5,(selectedAtoms->at(i).frac.y-midd.y)*ny/eny+0.5,(selectedAtoms->at(i).frac.z-midd.z)*nz/enz+0.5);
      atm.occ=fmax(0.0,fmin(1.0,selectedAtoms->at(i).amul*selectedAtoms->at(i).multiplicity));
      if ((selectedAtoms->at(i).jtf<2)||(selectedAtoms->at(i).uf.m22==0.0)) { 
        fprintf(esx,"%-4s %4d %9.5f %9.5f %9.5f %9.5f %9.5f\n",
            selectedAtoms->at(i).atomname,
            selectedAtoms->at(i).atomtype,
            atm.frac.x,//
            atm.frac.y,//
            atm.frac.z,//
            selectedAtoms->at(i).amul,
            selectedAtoms->at(i).uf.m11);
          double bb=selectedAtoms->at(i).uf.m11;
          u=Matrix(
              bb,bb*mol->zelle.cosrg,bb*mol->zelle.cosrb,
              bb*mol->zelle.cosrg,bb,bb*mol->zelle.cosra,
              bb*mol->zelle.cosrb,bb*mol->zelle.cosra,bb);
      }
        else {
          fprintf(esx,"%-4s %4d %9.5f %9.5f %9.5f %9.5f %9.5f %9.5f = \n  %9.5f %9.5f %9.5f %9.5f\n",
          selectedAtoms->at(i).atomname,
          selectedAtoms->at(i).atomtype,
          atm.frac.x,//
          atm.frac.y,//
          atm.frac.z,//
          selectedAtoms->at(i).amul,selectedAtoms->at(i).uf.m11,selectedAtoms->at(i).uf.m22,selectedAtoms->at(i).uf.m33,selectedAtoms->at(i).uf.m23,selectedAtoms->at(i).uf.m13,selectedAtoms->at(i).uf.m12);
          u=selectedAtoms->at(i).uf;
/*      printf("%-4s %4d %9.5f %9.5f %9.5f %9.5f %9.5f %9.5f = \n  %9.5f %9.5f %9.5f %9.5f\n",
          selectedAtoms->at(i).atomname,
          selectedAtoms->at(i).atomtype,
          selectedAtoms->at(i).frac.x,//
          selectedAtoms->at(i).frac.y,//
          selectedAtoms->at(i).frac.z,//
          selectedAtoms->at(i).amul,selectedAtoms->at(i).uf.m11,selectedAtoms->at(i).uf.m22,selectedAtoms->at(i).uf.m33,selectedAtoms->at(i).uf.m23,selectedAtoms->at(i).uf.m13,selectedAtoms->at(i).uf.m12);
          */
        }
      atm.Bij= u2b(u);
/*     printf("%s:\n%9.4f%9.4f%9.4f%9.4f%9.4f%9.4f%9.4f%9.4f%9.4f\n",selectedAtoms->at(i).atomname
         , atm.Bij.m11, atm.Bij.m12, atm.Bij.m13
         , atm.Bij.m21, atm.Bij.m22, atm.Bij.m23
         , atm.Bij.m31, atm.Bij.m32, atm.Bij.m33
         );*/
      atm.iscat=findSFAC(selectedAtoms->at(i).OrdZahl);
      if (atm.iscat<0){
        qDebug()<<"Scattering factor not found !\n";
        return;
      }
      fillmi(atm);
      atomsInMolecule.append(atm);
      double argu,rho; 
      for (int icoo=0; icoo<entot; icoo++){
        V3 pos1=V3((double)inteX[icoo]/nx,(double)inteY[icoo]/ny,(double)inteZ[icoo]/nz);
        V3 pos0=pos1-selectedAtoms->at(i).frac;
        rho=0.0;
        for (int i = 0; i < 6; i++){
          argu=pos0*(atm.Mi[i]*pos0);
          rho+=atm.pref[i]*exp(argu);
        }
        atom_rho[icoo]=rho;
        mole_rho[icoo]+=rho;

      }
      if (expandetrho2!=NULL) hirsh41atom(selectedAtoms->at(i),atom_rho,crys_rho,expandetrho2);
    }
    }
    update();
    fprintf(esx,"HKLF 0\n");
    fclose(esx);
    aStatusBar->setText(QString("Computing Hirshfeld sharing function"));
    update();
    for (int icoo=0; icoo<entot; icoo++){
      hirsh_share[icoo]=mole_rho[icoo]/crys_rho[icoo];
    }
    if ((density_out!=NULL)&&(C!=NULL)){
      for (int icoo=0; icoo<entot; icoo++){
        density_out[icoo]=expandetrho2[icoo] * hirsh_share[icoo];
      }
      writeMap2(density_out,DensityOut);
    }
    if (!IAMmolname.isEmpty())writeMap2(mole_rho,IAMmolname);
    writeMap2(hirsh_share,HirshShare);
    aStatusBar->setText(QString("finished!"));
    forts->hide();
    update();
    V3 dd=midd;
    mol->frac2kart(dd,dd);
    center=Vector3(dd.x,dd.y,dd.z);
    emit doface();

}

void Hirshfeld::forMolIso(MolIso *mi){
  QString hfac(".__hirsh.face");
  if (QFile::exists(hfac)) QFile::remove(hfac);
  mi->lineNr=0;
  V3 dx=V3(1.0/(nx),0,0);
  V3 dy=V3(0,1.0/(ny),0);
  V3 dz=V3(0,0,1.0/(nz));
  mol->frac2kart(dx,dx); 
  mol->frac2kart(dy,dy); 
  mol->frac2kart(dz,dz);    
  mi->x_dim=Vector3(dx.x,dx.y,dx.z); 
  mi->y_dim=Vector3(dy.x,dy.y,dy.z); 
  mi->z_dim=Vector3(dz.x,dz.y,dz.z); 
  mi->breite=enx;
  mi->hoehe=eny;
  mi->tiefe=enz;
  mi->bh=enx*eny;
  mi->iso_level=0.5;
  mi->orig=center;
  for (int i=0; i<entot; i++){
    mi->data.append(hirsh_share[i]);
    if (C!=NULL) mi->mdata.append(expandetrho2[i]);
  }
  mi->createSurface(hfac,((C!=NULL)));

  if (mi->mibas) glDeleteLists(mi->mibas,6);
  mi->mibas=glGenLists(6);
  mi->loadMI(hfac,false);

  close();
}

void Hirshfeld::writeCube2(double *dat, QString fileName){
//  printf("%p\n",dat);
  double max=0.0,min=1e99;
  for (int i=0; i<entot; i++){
    max=fmax(dat[i],max);
    min=fmin(dat[i],min);
  }
  printf("max%g min %g\n",max,min);
  FILE *fo;
  float factor=0.1481847095290449;//a0**3
  double a0=0.52917720859;  //bohr=0.5291775108
  int i=0;
  V3 dx1=V3(1.0/(nx),0,0);
  V3 dy1=V3(0,1.0/(ny),0);
  V3 dz1=V3(0,0,1.0/(nz));
  mol->frac2kart(dx1,dx1); 
  mol->frac2kart(dy1,dy1); 
  mol->frac2kart(dz1,dz1);     

  fo=fopen(fileName.toStdString().c_str(),"w");
  if ((fo==NULL)||(dat==NULL)) return ;
  fprintf(fo,"Hirshfeld sharing function by MoleCoolQt\nusing dynamic iam density\n");
  fprintf(fo,"%5d%12.6f%12.6f%12.6f\n",atomsInMolecule.size(),
      (dx1.x+dy1.x+dz1.x)*0.5/a0,
      (dx1.y+dy1.y+dz1.y)*0.5/a0,
      (dx1.z+dy1.z+dz1.z)*0.5/a0
      //0.0,0.0,0.0
      );
  fprintf(fo,"%5d%12.6f%12.6f%12.6f\n",enx,dx1.x/a0,dx1.y/a0,dx1.z/a0);
  fprintf(fo,"%5d%12.6f%12.6f%12.6f\n",eny,dy1.x/a0,dy1.y/a0,dy1.z/a0);
  fprintf(fo,"%5d%12.6f%12.6f%12.6f",  enz,dz1.x/a0,dz1.y/a0,dz1.z/a0);
  //no newline here because it is in atomlist
  QString al;
  for (int i=0; i<atomsInMolecule.size();i++){
    V3 cart;
    frac2kart(atomsInMolecule[i].frac,cart);
    al.append(QString("\n%1%2%3%4%5").arg(formfactors[atomsInMolecule[i].iscat].prot,5).arg((double)formfactors[atomsInMolecule[i].iscat].elec,12,'f',6).arg(cart.x/a0,12,'f',6).arg(cart.y/a0,12,'f',6).arg(cart.z/a0,12,'f',6));
  }
  fprintf(fo,"%s",al.toStdString().c_str());
  for (int xi=0;xi<enx;xi++)
    for (int yi=0;yi<eny;yi++)
      for (int zi=0;zi<enz;zi++)
      {
        //fprintf(fo,"%s%13.5E",(((i%6)==0)?"\n":""),datfo[dex(xi,yi,zi)]*factor);
        fprintf(fo,"%s",QString("%1%2").arg(((i%6)==0)?"\n":"").arg(dat[xi+(yi+zi*eny)*enx]*factor,13,'E',5).toStdString().c_str());
        i++;
      }
  fclose(fo);
  printf("writing of cube file is finished.\n");
}

void Hirshfeld::writeCube(double *dat, QString fileName){
//  printf("%p\n",dat);
  mol->setup_zelle(); 
  double max=0.0,min=1e99;
  for (int i=0; i<ntot; i++){
    max=fmax(dat[i],max);
    min=fmin(dat[i],min);
  }
  printf("max%g min %g\n",max,min);
  FILE *fo;
  float factor=0.1481847095290449;//a0**3
  double a0=0.52917720859;  //bohr=0.5291775108
  int i=0;
  //
  //FO MAP
  //
  V3 dx=V3(1.0/(nx),0,0);
  V3 dy=V3(0,1.0/(ny),0);
  V3 dz=V3(0,0,1.0/(nz));
  mol->frac2kart(dx,dx); 
  mol->frac2kart(dy,dy); 
  mol->frac2kart(dz,dz);     

  fo=fopen(fileName.toStdString().c_str(),"w");
  if ((fo==NULL)||(dat==NULL)) return ;
  fprintf(fo,"IAM rho prior written by MoleCoolQt\nusing a ported algorithm from Lucas Pallatinus\n");
  fprintf(fo,"%5d%12.6f%12.6f%12.6f\n",atomsInCell.size(),
      (dx.x+dy.x+dz.x)*0.5/a0,
      (dx.y+dy.y+dz.y)*0.5/a0,
      (dx.z+dy.z+dz.z)*0.5/a0
      //0.0,0.0,0.0
      );
  fprintf(fo,"%5d%12.6f%12.6f%12.6f\n",nx,dx.x/a0,dx.y/a0,dx.z/a0);
  fprintf(fo,"%5d%12.6f%12.6f%12.6f\n",ny,dy.x/a0,dy.y/a0,dy.z/a0);
  fprintf(fo,"%5d%12.6f%12.6f%12.6f"  ,nz,dz.x/a0,dz.y/a0,dz.z/a0);
  //no newline here because it is in atomlist
  QString al;
  for (int i=0; i<atomsInCell.size();i++){
    V3 cart;
    mol->frac2kart(atomsInCell[i].frac,cart);
    al.append(QString("\n%1%2%3%4%5").arg(formfactors[atomsInCell[i].iscat].prot,5).arg((double)formfactors[atomsInCell[i].iscat].elec,12,'f',6).arg(cart.x/a0,12,'f',6).arg(cart.y/a0,12,'f',6).arg(cart.z/a0,12,'f',6));
  }
  fprintf(fo,"%s",al.toStdString().c_str());
  for (int xi=0;xi<nx;xi++)
    for (int yi=0;yi<ny;yi++)
      for (int zi=0;zi<nz;zi++)
      {
        //fprintf(fo,"%s%13.5E",(((i%6)==0)?"\n":""),datfo[dex(xi,yi,zi)]*factor);
        fprintf(fo,"%s",QString("%1%2").arg(((i%6)==0)?"\n":"").arg(dat[xi+(yi+zi*ny)*nx]*factor,13,'E',5).toStdString().c_str());
        i++;
      }
  fclose(fo);
  printf("writing of cube file is finished.\n");
}

void Hirshfeld::frac2kart (V3 x, V3 & y){
//x ist fraktionell, y wird kartesisch
  
  y.x = x.x * expandedCell.f2c.m11 + x.y * expandedCell.f2c.m12 + x.z * expandedCell.f2c.m13;
  y.y = x.x * expandedCell.f2c.m21 + x.y * expandedCell.f2c.m22 + x.z * expandedCell.f2c.m23;
  y.z = x.x * expandedCell.f2c.m31 + x.y * expandedCell.f2c.m32 + x.z * expandedCell.f2c.m33;
  
}

void Hirshfeld::readCube(double *dat, QString fileName){
  QFile gh(fileName);
  gh.open(QIODevice::ReadOnly);
  QString all =gh.readAll();
  gh.close();
  QStringList lines = all.split(QRegExp("[\n\r]+"));
  all.clear();
  int anr=0,n1=0,n2=0,n3=0;
  //  double a,b,c;
  V3 x_dim, y_dim, z_dim;
  double bohr=0.52917720859 ;//0.5291775108;
  if (lines.size()>6) {
    QStringList tok;
    tok=lines.at(2).split(QRegExp("\\s+"),QString::SkipEmptyParts);
    if (tok.size()==4){	
      anr=tok.at(0).toInt();
      //      a=tok.at(1).toDouble();//origin a b c
      //      b=tok.at(2).toDouble();
      //      c=tok.at(3).toDouble();
      tok=lines.at(3).split(QRegExp("\\s+"),QString::SkipEmptyParts);	
      n1= tok.at(0).toInt();
      x_dim = V3(tok.at(1).toFloat(),tok.at(2).toFloat(),tok.at(3).toFloat());
      tok=lines.at(4).split(QRegExp("\\s+"),QString::SkipEmptyParts);	
      n2=  tok.at(0).toInt();

      y_dim = V3(tok.at(1).toFloat(),tok.at(2).toFloat(),tok.at(3).toFloat());
      tok=lines.at(5).split(QRegExp("\\s+"),QString::SkipEmptyParts);	
      n3=  tok.at(0).toInt();
      z_dim = V3(tok.at(1).toFloat(),tok.at(2).toFloat(),tok.at(3).toFloat());
      x_dim *= bohr;
      y_dim *= bohr;
      z_dim *= bohr;
    }
  }
  if ((n1!=nx)||(n2!=ny)||(n3!=nz)){
    qDebug()<<"Number of gridpoints differ from your choice!"<<n1<<n2<<n3<<nx<<ny<<nz;
    dat=NULL;
    return;
  }
  double *xdat=(double*)malloc(sizeof(double)*ntot);
  gh.open(QIODevice::ReadOnly);
  for (int i=0; i<(anr+6);i++) gh.readLine();
  int p=0;
  while (!gh.atEnd () && (p>=0)) {
    QStringList numbers = QString(gh.readLine()).split(" ",QString::SkipEmptyParts);
    if ((!numbers.isEmpty())&&(!numbers.at(0).contains('.'))) continue;
    for (int i=0; i<numbers.size();i++) {

      xdat[p++]=(numbers.at(i).toDouble());
    }
  }
  gh.close(); 
  p=0;
  for (int xi=0;xi<nx;xi++)
    for (int yi=0;yi<ny;yi++)
      for (int zi=0;zi<nz;zi++) dat[xi+(yi+zi*ny)*nx]=xdat[p++];
  free(xdat);

}

void Hirshfeld::readMap(double *dat, QString fileName){
  if (fileName.endsWith(".cube")) readCube(dat,fileName);
  else if (fileName.endsWith(".raw")) readRaw(dat,fileName);
  else if (fileName.endsWith(".m81")) readM81(dat,fileName);
}

void Hirshfeld::writeMap(double *dat, QString fileName){
  if (fileName.endsWith(".cube")) writeCube(dat,fileName);
  else if (fileName.endsWith(".raw")) writeRaw(dat,fileName);
  else if (fileName.endsWith(".m81")) writeM81(dat,fileName);
}

void Hirshfeld::writeMap2(double *dat, QString fileName){
  if (fileName.endsWith(".cube")) writeCube2(dat,fileName);
  else if (fileName.endsWith(".raw")) writeRaw2(dat,fileName);
  else if (fileName.endsWith(".m81")) writeM812(dat,fileName);
}

void Hirshfeld::writeRaw(double *data, QString fileName){
  int n6=ntot*sizeof(double);
  double *dat=(double*) malloc(n6);
  double capvx=1.0/ntot*mol->zelle.V;
  for(int i=0; i<ntot; i++){
    dat[i]=data[i]*capvx;   
  }
  FILE *raw=fopen(fileName.toStdString().c_str(),"wb");
  BMhead bm;
  bm.h=bm.f=76;
  bm.sd=bm.d=3;
  bm.nn1=nx;
  bm.nn2=ny;
  bm.nn3=nz;
  bm.cel[0]=mol->zelle.a;
  bm.cel[1]=mol->zelle.b;
  bm.cel[2]=mol->zelle.c;
  bm.cel[3]=mol->zelle.al;
  bm.cel[4]=mol->zelle.be;
  bm.cel[5]=mol->zelle.ga;
  bm.cel[6]=mol->zelle.V;
  fwrite((void*)&bm,sizeof(BMhead)-sizeof(int),1,raw);
  fwrite((void*)&n6,sizeof(int),1,raw);
  //fwrite((void*)expandetrho,n6,1,raw);
  fwrite((void*)dat,n6,1,raw);
  fwrite((void*)&n6,sizeof(int),1,raw);
  fwrite((void*)dat,n6,1,raw);
  fclose(raw);
}

void Hirshfeld::writeRaw2(double *data, QString fileName){
  int n6=entot*sizeof(double);
  double *dat=(double*) malloc(n6);
  double capvx=1.0/entot*expandedCell.V;
  for(int i=0; i<entot; i++){
    dat[i]=data[i]*capvx;   
  }
  FILE *raw=fopen(fileName.toStdString().c_str(),"wb");
  BMhead bm;
  bm.h=bm.f=76;
  bm.sd=bm.d=3;
  bm.nn1=enx;
  bm.nn2=eny;
  bm.nn3=enz;
  bm.cel[0]=expandedCell.a;
  bm.cel[1]=expandedCell.b;
  bm.cel[2]=expandedCell.c;
  bm.cel[3]=expandedCell.al;
  bm.cel[4]=expandedCell.be;
  bm.cel[5]=expandedCell.ga;
  bm.cel[6]=expandedCell.V;
  fwrite((void*)&bm,sizeof(BMhead)-sizeof(int),1,raw);
  fwrite((void*)&n6,sizeof(int),1,raw);
  //fwrite((void*)expandetrho,n6,1,raw);
  fwrite((void*)dat,n6,1,raw);
  fwrite((void*)&n6,sizeof(int),1,raw);
  free(dat);
  fclose(raw);
}

void Hirshfeld::writeM81(double *data, QString fileName){
  float *dat=(float*) malloc(ntot*sizeof(float));
  for(int i=0; i<ntot; i++){
    dat[i]=data[i];   
  }
  FILE *raw=fopen(fileName.toStdString().c_str(),"wb");
  struct T{
    int nx[6],nxny,nmap;
    float xymin[12];
    float dx[6];
    int iorien[6],mapa,nsubs;
    bool sat;
  };
  T t;
  t.nx[0] = nx;
  t.nx[1] = ny;
  t.nx[2] = nz;
  t.nx[3] = 1;
  t.nx[4] = 1;
  t.nx[5] = 1;
  t.nxny=nx*ny;
  t.nmap=nz;
  t.mapa=4;
  t.nsubs=1;
  for (int i=0; i<6; i++){
    t.iorien[i]=i+1;
    t.dx[i]=1.0/t.nx[i];  
    t.xymin[i*2]=0.0;
    t.xymin[i*2+1]=t.dx[i]*(t.nx[i]-1);
  }
  float *dummyHeader=(float*) malloc(sizeof(float)*t.nxny);
  for (int i=0; i<t.nxny;i++) dummyHeader[i]=0.0f;
  fwrite((void*)dummyHeader,sizeof(float)*t.nxny,1,raw);
  fwrite((void*)dat,ntot*sizeof(float),1,raw);
  rewind(raw);
  fwrite((void*)&t,sizeof(T),1,raw);
  free(dat);
  fclose(raw);
}

void Hirshfeld::writeM812(double *data, QString fileName){
 // printf("allocate %ld\n",ntot*sizeof(float));
  float *dat=(float*) malloc(entot*sizeof(float));
  for(int i=0; i<entot; i++){
    dat[i]=data[i];   
  }
  FILE *raw=fopen(fileName.toStdString().c_str(),"wb");
  struct T{
    int nx[6],nxny,nmap;
    float xymin[12];
    float dx[6];
    int iorien[6],mapa,nsubs;
    bool sat;
  };
  T t;
  t.nx[0] = enx;
  t.nx[1] = eny;
  t.nx[2] = enz;
  t.nx[3] = 1;
  t.nx[4] = 1;
  t.nx[5] = 1;
  t.nxny=enx*eny;
  t.nmap=enz;
  t.mapa=4;
  t.nsubs=1;
  for (int i=0; i<6; i++){
    t.iorien[i]=i+1;
    t.dx[i]=1.0f/t.nx[i];  
    t.xymin[i*2]=0.0f;
    t.xymin[i*2+1]=t.dx[i]*(t.nx[i]-1);
  }
  float *dummyHeader=(float*) malloc(sizeof(float)*t.nxny);
  for (int i=0; i<t.nxny;i++) dummyHeader[i]=0.0f;
//  printf("write dummy header of %ld %d %p\n",sizeof(float)*t.nxny,t.nxny,raw);
//  int wb=
    fwrite((void*)dummyHeader,sizeof(float)*t.nxny,1,raw);
//  printf("(%d) write data %ld\n",wb,entot*sizeof(float));
  fwrite((void*)dat,entot*sizeof(float),1,raw);
  rewind(raw);
//  printf("write header of %ld\n",sizeof(T));
  fwrite((void*)&t,sizeof(T),1,raw);
  printf("free dat\n");
  free(dat);
  fclose(raw);
  printf("m 8 1 w r i t t e n !\n");
}

void Hirshfeld::readRaw(double *dat, QString fileName){

  ntot=nx*ny*nz;
  FILE *raw=fopen(fileName.toStdString().c_str(),"rt");
  BMhead bm;
  //fread(void *ptr, size_t size, size_t nmemb, FILE *stream); 
  size_t siz=fread(&bm,sizeof(BMhead),1,raw);
  if ((siz!=sizeof(BMhead))&&(bm.h!=bm.f)&&(bm.h!=76)){
//    fprintf(stderr,"siz %ld==%ld %d==%d (should be 76)\n",siz,sizeof(BMhead),bm.h,bm.f);
    dat=NULL;
    return;
  }
  printf("rdim = %d sdim = %d\n n1 = %d; n2 = %d; n3 = %d\n %f %f %f %f %f %f %f\n",bm.d,bm.sd,bm.nn1,bm.nn2,bm.nn3,bm.cel[0],bm.cel[1],bm.cel[2],bm.cel[3],bm.cel[4],bm.cel[5],bm.cel[6]);
//  printf("sid %d %ld %d\n",bm.sid,bm.sid/sizeof(double),ntot);
  fread((double*)dat,bm.sid,1,raw);
  double capvx=1.0/ntot*mol->zelle.V;
  for(int i=0; i<ntot; i++){
    dat[i]/=capvx;   
  }
  fclose(raw);
}

void Hirshfeld::readM81(double *dat, QString fileName){
  QFile jh(fileName);
  //FILE *f;
  jh.open(QIODevice::ReadOnly);
  struct T{
    int nx[6],nxny,nmap;
    float xymin[12];
    float dx[6];
    int iorien[6],mapa,nsubs;
    bool sat;
  };
  T t;
  jh.read((char*)&t,sizeof(T));
  int idim=0,ups=0,aua=-1;
    for (int i=0; i<6; i++) {
    idim+=ups=t.iorien[i];
  //  printf("%d\n",ups);
    if (aua>ups) {
      qDebug()<<"m81 file axis order must be 1 2 3, x y z!";
    exit(0);  
    }
  }
  if (idim!=21) {
  qDebug()<<".m81 file suport only for 3 Dimensions, sorry"<<idim;
  exit(0);
  }
  
  //qDebug()<<t.nx[0]<<t.nx[1]<<t.nx[2]<<t.nxny;
  jh.close();
  int kk=0;
  nx =(t.iorien[0]==1)?t.nx[0]:(t.iorien[1])==1?t.nx[1]:t.nx[2];
  ny =(t.iorien[0]==2)?t.nx[0]:(t.iorien[1])==2?t.nx[1]:t.nx[2];
  nz =(t.iorien[0]==3)?t.nx[0]:(t.iorien[1])==3?t.nx[1]:t.nx[2];
  int bh =t.nxny;
  ntot=nx*ny*nz;
  jh.open(QIODevice::ReadOnly);
  float *floatdat=(float*)malloc(bh*sizeof(float));
  jh.read((char*) floatdat,sizeof(float)*bh);//skip first record 
  for (int i=0;i<nz;i++){
    jh.read((char*) floatdat,sizeof(float)*bh);
    for (int j=0;j<bh;j++)
      dat[kk++]=floatdat[j];// */
  }
  jh.close(); 
  free(floatdat);
}

void Hirshfeld::setup_zelle(){  
  const double g2r=180.0/M_PI;
  double        
    cs_al = (expandedCell.al==90)?0:cos(expandedCell.al/g2r),
          cs_be = (expandedCell.be==90)?0:cos(expandedCell.be/g2r),
          cs_ga = (expandedCell.ga==90)?0:cos(expandedCell.ga/g2r),

          sn_al = (expandedCell.al==90)?1:sin(expandedCell.al/g2r),
          sn_be = (expandedCell.be==90)?1:sin(expandedCell.be/g2r),
          sn_ga = (expandedCell.ga==90)?1:sin(expandedCell.ga/g2r);

  expandedCell.phi=  sqrt(1-(cs_al*cs_al)-(cs_be*cs_be)-(cs_ga*cs_ga) + 2*cs_al*cs_be*cs_ga);
  expandedCell.V = expandedCell.a*expandedCell.b*expandedCell.c*expandedCell.phi;
  expandedCell.as= expandedCell.c*expandedCell.b*sn_al/expandedCell.V;
  expandedCell.bs= expandedCell.c*expandedCell.a*sn_be/expandedCell.V;
  expandedCell.cs= expandedCell.a*expandedCell.b*sn_ga/expandedCell.V;
  double
    cosra=(cs_be*cs_ga-cs_al)/(sn_be*sn_ga),
    cosrb=(cs_al*cs_ga-cs_be)/(sn_al*sn_ga),
    cosrg=(cs_al*cs_be-cs_ga)/(sn_al*sn_be);

  expandedCell.als=acos(cosra)*g2r;
  expandedCell.bes=acos(cosrb)*g2r;
  expandedCell.gas=acos(cosrg)*g2r; 
  const double tau=expandedCell.c*((cs_al-cs_be*cs_ga)/sn_ga);
  expandedCell.f2c.m11 = expandedCell.a;
  expandedCell.f2c.m21 = 0.0;
  expandedCell.f2c.m31 = 0.0;
  expandedCell.f2c.m12 = expandedCell.b * cs_ga;
  expandedCell.f2c.m22 = expandedCell.b * sn_ga;
  expandedCell.f2c.m32 = 0.0;
  expandedCell.f2c.m13 = expandedCell.c * cs_be;
  expandedCell.f2c.m23 = tau;
  expandedCell.f2c.m33 = expandedCell.c * expandedCell.phi / sn_ga;
  expandedCell.G.m11=expandedCell.a*expandedCell.a;
  expandedCell.G.m22=expandedCell.b*expandedCell.b;
  expandedCell.G.m33=expandedCell.c*expandedCell.c;
  expandedCell.G.m12=expandedCell.G.m21=expandedCell.a*expandedCell.b*cs_ga;
  expandedCell.G.m13=expandedCell.G.m31=expandedCell.a*expandedCell.c*cs_be;
  expandedCell.G.m23=expandedCell.G.m32=expandedCell.b*expandedCell.c*cs_al;

  expandedCell.Gi.m11=expandedCell.as*expandedCell.as;
  expandedCell.Gi.m22=expandedCell.bs*expandedCell.bs;
  expandedCell.Gi.m33=expandedCell.cs*expandedCell.cs;
  expandedCell.Gi.m12=expandedCell.Gi.m21=expandedCell.as*expandedCell.bs*cosrg;
  expandedCell.Gi.m13=expandedCell.Gi.m31=expandedCell.as*expandedCell.cs*cosrb;
  expandedCell.Gi.m23=expandedCell.Gi.m32=expandedCell.bs*expandedCell.cs*cosra;
}

void Hirshfeld::priorThere(const QString & text){
  iamp->setEnabled((!text.isEmpty())&&(text.contains(QRegExp("(.cube)|(.raw)|(.m81)$"))));
  if ((!text.isEmpty())&&(text.endsWith(".raw"))&&(QFile::exists(text))){
//    printf ("Testing '%s'\n",text.toStdString().c_str());
    FILE *raw=fopen(text.toStdString().c_str(),"rt");
    BMhead bm;
    //fread(void *ptr, size_t size, size_t nmemb, FILE *stream); 
    size_t siz=fread(&bm,sizeof(BMhead),1,raw);
    if ((siz!=sizeof(BMhead))&&(bm.h!=bm.f)&&(bm.h!=76)){
 //     fprintf(stderr,"siz %ld==%ld %d==%d (should be 76)\n",siz,sizeof(BMhead),bm.h,bm.f);
      fclose(raw);
      return;
    }
    fclose(raw);
//    printf("%d %d %d ok %d\n",bm.nn1,bm.nn2,bm.nn3,(nx==bm.nn1)&&(ny==bm.nn2)&&(nz==bm.nn3));
    loadIamp->setEnabled((nx==bm.nn1)&&(ny==bm.nn2)&&(nz==bm.nn3)); 
  }

}

void Hirshfeld::hirshThere(const QString & text){
  hirp->setEnabled((!text.isEmpty())&&(iamr!=NULL)&&(text.contains(QRegExp("(.cube)|(.raw)|(.m81)$"))));

}

void Hirshfeld::hirsh41atom(const INP atm,double *ra,double *rc,double *dd){
  double electrons=0.0;
  double charge=0.0,rho;
  double min=1e99,max=-1e99;
  for (int icoo=0; icoo<entot; icoo++){
    charge+=ra[icoo];
    rho=(ra[icoo]/rc[icoo])*dd[icoo];
    electrons+=rho;
    min=fmin(rho,min);
    max=fmax(rho,max);
  }
  electrons*=expandedCell.V/entot;
  charge*=expandedCell.V/entot;
  printf("%-8s electrons %f charge %f min %f max %f\n",atm.atomname,electrons,charge-electrons,min,max);
}
