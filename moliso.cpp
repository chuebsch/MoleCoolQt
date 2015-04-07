#include "moliso.h"
#include "molekul.h"
#include <QtGui>
#include "cubeGL.h"

bool X_cmp(const Polygn &p1,const Polygn &p2){
  return (p1.vertex.x < p2.vertex.x);
}
bool x_cmp(const Polygn &p1,const Polygn &p2){
  return (p1.vertex.x > p2.vertex.x);
}
bool Y_cmp(const Polygn &p1,const Polygn &p2){
  return p1.vertex.y < p2.vertex.y;
}
bool y_cmp(const Polygn &p1,const Polygn &p2){
  return p1.vertex.y > p2.vertex.y;
}
bool Z_cmp(const Polygn &p1,const Polygn &p2){
  return p1.vertex.z < p2.vertex.z;
}
bool z_cmp(const Polygn &p1,const Polygn &p2){
  return p1.vertex.z > p2.vertex.z;
}

void MolIso::PXsort( void) {
  qSort(pgns.begin(),pgns.end(),X_cmp);
}
void MolIso::Pxsort( void) {
  qSort(pgns.begin(),pgns.end(),x_cmp);
}
void MolIso::PYsort( void) {
  qSort(pgns.begin(),pgns.end(),Y_cmp);
}
void MolIso::Pysort( void) {
  qSort(pgns.begin(),pgns.end(),y_cmp);
}
void MolIso::PZsort( void) {
  qSort(pgns.begin(),pgns.end(),Z_cmp);
}
void MolIso::Pzsort( void) {
  qSort(pgns.begin(),pgns.end(),z_cmp); 
}



MolIso::MolIso(){
  //Constructor
  cubeiso=false;
  cubemap=false;
  fixmin=666.666;
  fixmax=666.666;
  Farben=6;    
  farbe[0][0]=0.6;    
  farbe[0][1]=0;    
  farbe[0][2]=0;    
  farbe[0][3]=0.5;    

  farbe[1][0]=0.6;    
  farbe[1][1]=0.5;    
  farbe[1][2]=0;    
  farbe[1][3]=0.5;    

  farbe[2][0]=0;    
  farbe[2][1]=0.5;    
  farbe[2][2]=0;    
  farbe[2][3]=0.5;    

  farbe[3][0]=0;    
  farbe[3][1]=0.5;    
  farbe[3][2]=0.9;    
  farbe[3][3]=0.5;    

  farbe[4][0]=0;    
  farbe[4][1]=0;    
  farbe[4][2]=0.9;    
  farbe[4][3]=0.5;
    
  farbe[5][0]=0.6;    
  farbe[5][1]=0;    
  farbe[5][2]=0.9;    
  farbe[5][3]=0.5;
    
  farbe[6][0]=0.6;    
  farbe[6][1]=0;    
  farbe[6][2]=0;    
  farbe[6][3]=0.5;
  mibas=0;
}
void MolIso::legende(){
  glNewList(mibas+6, GL_COMPILE ); 
  glPushMatrix();
  double mat[16]; 
  glEnable(GL_COLOR_MATERIAL);
  glGetDoublev( GL_MODELVIEW_MATRIX, (double*)mat );
  glLoadIdentity();
  glDisable( GL_LIGHTING ); 
  glDisable( GL_DEPTH_TEST ); 
  glBegin( GL_TRIANGLE_STRIP );
  for (int j=0;j<256;j++){		
    Farbverlauf((0.0039*j*(max-min))+min);
    glTexCoord1f(0.0039*j);
    glVertex3f(-2.85,-0.3+0.006640625*j,-6.1);
    glVertex3f(-2.90,-0.3+0.006640625*j,-6.1);
  }
  glEnd();
  glDisable(GL_TEXTURE_1D);
  glEnable( GL_LIGHTING ); 
  glEnable( GL_DEPTH_TEST ); 
  glLoadMatrixd(mat);
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
  glEndList();
}

void MolIso::loadMI(QString fname, bool om, bool mima){
 printf("mima %s  OM = %s\n",(mima)?"true":"false",(om)?"true":"false");
  for (int i=0;i<6;i++){
    if ((mibas)&&(glIsList(mibas+i))) {
      printf("NOT deleting list #%d\n",mibas+i);
    }else{
    printf("deleting %d\n",mibas+i);
      if (mibas) glDeleteLists(mibas+i,1);
    }
  }
//  mibas=0;
//  mibas=glGenLists(6);*/
  extern QProgressBar *balken;
  if (orte.isEmpty()){
  QFile sf(fname);
  if (!sf.exists()) {qDebug()<<"The File "<<fname<<" does not exist!"; exit(0);}
  if (!sf.open(QIODevice::ReadOnly)) {qDebug()<<"Can not open "<<fname<<" for reading. Please check your rights."; exit(0);}

  Ort v;
  Polygn p;
  QString all =sf.readAll();
  if (all.isEmpty()) {qDebug()<<"Can not read  "<<fname<<". may be it is empty or corrupt!"; exit(0);}
  QStringList lines = all.split(QRegExp("[\n\r]{1,2}")); 
  if (mima){
  min=1e99;
  max=-1e99;
  }
  if (lines.size()){

    balken->setMinimum(0);
    balken->setMaximum(7);
    balken->show();
    double dm=0,ds=0,sigma=0,dmp=0,dmm=0,dsp=0,dsm=0,sigmap=0,sigmam=0,api;//,potsigplus=0,potsigminus=0;
    int np=0,nm=0;
    for (int i = 1; i <= lines[0].toInt();i++){
      QStringList tok = lines[i].split(' ',QString::SkipEmptyParts);
      if (tok.size()==8){

	balken->setValue(i);
	
	v.vertex.x = tok.at(1).toFloat();
	v.vertex.y = tok.at(2).toFloat();
	v.vertex.z = tok.at(3).toFloat();
	if (om) {
	  v.vertex.y += (float) orig.y;
	  v.vertex.x += (float) orig.x;
	  v.vertex.z += (float) orig.z;
	}
	//
	v.normal.x = tok.at(4).toFloat();
	v.normal.y = tok.at(5).toFloat();
	v.normal.z = tok.at(6).toFloat();
	dm += v.color = tok.at(7).toFloat();
	ds += v.color*v.color;
	if (v.color>0){
	  dmp += v.color;
	  dsp += (v.color* v.color);
	  np++;
	}else{
	  dmm += v.color;
	  dsm += v.color* v.color;
	  nm++;
	}

	v.direct=0;
        if(mima){
	min=(min>v.color)?v.color:min;
	max=(max<v.color)?v.color:max;
        }
	orte.append(v);
      }
    }
    ds/=fmax(np+nm,1.0);;
    dm/=fmax(np+nm,1.0);;
    dmp/=fmax(np,1.0);
    dsp/=fmax(np,1.0);
    dmm/=fmax(nm,1.0);
    dsm/=fmax(nm,1.0);

    sigma=(ds-(dm*dm));
    sigmap=(dsp-(dmp*dmp));
    sigmam=(dsm-(dmm*dmm));
    api=0;
    for (int i=0; i< orte.size(); i++){
//        if (orte.at(i).color>0) potsigplus+=pow(orte.at(i).color-dm,2);
//        else potsigminus+=pow(orte.at(i).color-dm,2);
       api+=fabs(orte.at(i).color-dm);
    }
    //potsigminus/=fmax(nm,1);
    //potsigplus/=fmax(np,1);
    api/=fmax(np+nm,1);
    emit bigmessage(QString(
                        "Average of positive surface values V<sub>S</sub><sup>+</sup><sub>av.</sub> = <b>%1</b><br>"
                        "Average of negative surface values V<sub>S</sub><sup>-</sup><sub>av.</sub> = <b>%2</b><br>"
                        "Average deviation from the average surface value &Pi; = <b>%10</b><br>"
                        "&sigma;<sup>2</sup><sub>+</sub> = &sum; (V<sub>S</sub><sup>+</sup> - V<sub>S</sub><sup>+</sup><sub>av.</sub>)<sup>2</sup> = <b>%3</b><br>"
                        "&sigma;<sup>2</sup><sub>-</sub> = &sum; (V<sub>S</sub><sup>-</sup> - V<sub>S</sub><sup>-</sup><sub>av.</sub>)<sup>2</sup> = <b>%4</b><br>"
                        "&sigma;<sup>2</sup><sub>tot</sub> = &sigma;<sup>2</sup><sub>+</sub> + &sigma;<sup>2</sup><sub>-</sub> = <b>%5</b><br>"
                        "&sigma;<sup>2</sup> = &sum; (V<sub>S</sub> - V<sub>Sav.</sub>)<sup>2</sup> = <b>%6</b><br>n+ = <b>%7</b><br>n- = <b>%9</b><br>"
                        "&nu; = (&sigma;<sup>2</sup><sub>+</sub>&sdot;&sigma;<sup>2</sup><sub>-</sub>)/(&sigma;<sup>2</sup><sub>tot</sub>)<sup>2</sup> = <b>%11</b><br>Please see: <br>"
                        "Politzer, P., Murray, J. S., Peralta-Inga, Z.,<br>"
                        "<em>Int. J. Quantum. Chem.</em> <b>85</b> (2001), 676-684.<br>for details about these numbers.")
                    .arg(dmp,0,'f',3)
            .arg(dmm,0,'f',3)
            .arg(sigmap,0,'f',4)
            .arg(sigmam,0,'f',4)
            .arg(sigmam+sigmap,0,'f',4)
            .arg(sigma,0,'f',4)
            .arg(np)
            .arg(nm)
            .arg(api,0,'f',3)
                    .arg(sigmap*sigmam/((sigmam+sigmap)*(sigmam+sigmap))));
//    printf("\nAverage of positive surface values VS+= %f\nAverage of negative surface values VS-= %f\nsigma square + = %f\nsigma square - = %f\n%f %f n+ %d n- %d\n%g  \n"
    /*potnu=potsigplus*potsigminus/(potsigtot*potsigtot);
      printf("\nAverage of positive surface values VS+= %f %s\n Average of negative surface values VS-= %f %s\n Average deviation from the average surface value PI= %f %s\n sigma square + = %f (%s)^2\n sigma square - = %f (%s)^2\n sigma square total = %f (%s)^2\n nu = %f \n\n",
      avpotplus,lul,avpotminus,lul,avdevPI,lul,potsigplus,lul,potsigminus,lul,potsigtot,lul,potnu);*/

    max+=max*0.00001+0.00001;
    min-=min*0.00001+0.00001;
    if (fixmin!=666.666) min=fixmin;
    if (fixmax!=666.666) max=fixmax;
    for (int i=lines[0].toInt()+1;i<lines.size();i++){
      QStringList tok = lines[i].split(' ',QString::SkipEmptyParts);
      if (tok.size()){
	p.vertex.x=0;
	p.vertex.y=0;
	p.vertex.z=0;
	for (p.n=0; p.n<tok.size();p.n++){
	  p.ii[p.n]=tok.at(p.n).toInt();
	  p.vertex+=orte.at(p.ii[p.n]).vertex;
	}	
	p.vertex*=1.0/p.n;
	pgns.append(p);
      }
    }
  }  
  }//orte is Empty
  printf("%d orte.size %d \n",orte.size(),pgns.size());
  balken->setValue(1);
  glNewList(mibas, GL_COMPILE );{                       //Isooberfl"ache ::Perspektive 1     
    glPushMatrix();{
      glScaled( L, L, L );
      PXsort();
      if (!mima) glDisable( GL_DEPTH_TEST ); 
      DrawPlys();
      if (!mima) glEnable( GL_DEPTH_TEST ); 
    }glPopMatrix();
  }glEndList();


  balken->setValue(2);
  glNewList(mibas+1, GL_COMPILE  );{                       //Isooberfl"ache ::Perspektive 1     
    glPushMatrix();{
      glScaled( L, L, L );
      Pxsort();
      if (!mima) glDisable( GL_DEPTH_TEST ); 
      DrawPlys();
      if (!mima) glEnable( GL_DEPTH_TEST ); 
    }glPopMatrix();
  }glEndList();

  //balken->setValue(3*lines[0].toInt());
  glNewList(mibas+2, GL_COMPILE  );{                       //Isooberfl"ache ::Perspektive 1     
    glPushMatrix();{
      glScaled( L, L, L );
      PYsort();
if (!mima) glDisable( GL_DEPTH_TEST ); 
      DrawPlys();
if (!mima) glEnable( GL_DEPTH_TEST ); 
    }glPopMatrix();
  }glEndList();

  balken->setValue(4);
  glNewList(mibas+3, GL_COMPILE  );{                       //Isooberfl"ache ::Perspektive 1     
    glPushMatrix();{
      glScaled( L, L, L );
      Pysort();
if (!mima) glDisable( GL_DEPTH_TEST ); 
      DrawPlys();
if (!mima) glEnable( GL_DEPTH_TEST ); 
    }glPopMatrix();
  }glEndList();

  balken->setValue(5);
  glNewList(mibas+4, GL_COMPILE  );{                       //Isooberfl"ache ::Perspektive 1     
    glPushMatrix();{
      glScaled( L, L, L );
      PZsort();
if (!mima) glDisable( GL_DEPTH_TEST ); 
      DrawPlys();
if (!mima) glEnable( GL_DEPTH_TEST ); 
    }glPopMatrix();
  }glEndList();

  balken->setValue(6);  
  glNewList(mibas+5, GL_COMPILE  );{                       //Isooberfl"ache ::Perspektive 1     
    glPushMatrix();{
      glScaled( L, L, L );
      Pzsort();
if (!mima) glDisable( GL_DEPTH_TEST ); 
      DrawPlys();
if (!mima) glEnable( GL_DEPTH_TEST ); 
    }glPopMatrix();
  }glEndList();


  balken->setValue(7);  

  balken->hide();
  faceFile=fname;
}


void MolIso::DrawPlys(){
  glLoadName((GLuint)-1);
  for (int i=0;i<pgns.size();i++){
    switch (pgns.at(i).n){
    case  3:glBegin(GL_TRIANGLES); 
      for (int k=0;k<3;k++) {
	Farbverlauf(orte.at(pgns.at(i).ii[k]).color);
	glTexCoord1f((orte.at(pgns.at(i).ii[k]).color-min)/(max-min));
	glNormal3fv( (GLfloat*)&orte.at(pgns.at(i).ii[k]).normal);
	glVertex3fv( (GLfloat*)&orte.at(pgns.at(i).ii[k]).vertex);
      }          
      glEnd();
      break;
    case  4:  //if (Quad)
	{
	glBegin(GL_QUADS);
	for (int k=0;k<4;k++) {
	  Farbverlauf(orte.at(pgns.at(i).ii[k]).color);
	  glTexCoord1f((orte.at(pgns.at(i).ii[k]).color-min)/(max-min));
	  glNormal3fv( (GLfloat*)&orte.at(pgns.at(i).ii[k]).normal);
	  glVertex3f(orte.at(pgns.at(i).ii[k]).vertex.x,orte.at(pgns.at(i).ii[k]).vertex.y,orte.at(pgns.at(i).ii[k]).vertex.z);
	}
	glEnd();
	break;
      }// */
    case  5:
    case  6:
    case  7:
    case  8:
    case  9:
    case  10:
    case  11:
    case   12:{
      Vector3 mO=orte.at(pgns.at(i).ii[0]).vertex,mN=orte.at(pgns.at(i).ii[0]).normal;
      GLfloat mC=orte.at(pgns.at(i).ii[0]).color;
      for (int k=1;k <pgns.at(i).n; k++) {
	mO+=orte.at(pgns.at(i).ii[k]).vertex;
	mN+=orte.at(pgns.at(i).ii[k]).normal;
	mC+=orte.at(pgns.at(i).ii[k]).color;}
      mC/=(GLfloat)pgns.at(i).n;
      mO*=(GLfloat)1.0/pgns.at(i).n;
      mN*=(GLfloat)1.0/pgns.at(i).n;         
      glBegin(GL_TRIANGLE_FAN);
      Farbverlauf(mC);
      glTexCoord1f((mC-min)/(max-min));
      glNormal3fv( (GLfloat*)&mN);
      glVertex3fv( (GLfloat*)&mO);
    }
      for (int k=0;k<=pgns.at(i).n;k++) {Farbverlauf(orte.at(pgns.at(i).ii[k%pgns.at(i).n]).color);
	glTexCoord1f((orte.at(pgns.at(i).ii[k%pgns.at(i).n]).color-min)/(max-min));
	glNormal3fv( (GLfloat*)&orte.at(pgns.at(i).ii[k%pgns.at(i).n]).normal);
	glVertex3fv( (GLfloat*)&orte.at(pgns.at(i).ii[k%pgns.at(i).n]).vertex);
      }
      glEnd();
      break;
    default : printf("wrong polygonshape! %d\n",pgns.at(i).n);glEnd();break;
    }
  }		
}	

void MolIso::readJanaHeader(QString fname){
  QFile jh(fname);
  QStringList atypen;
  bool habenatome=false;
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
  //qDebug()<<t.nx[0]<<t.nx[1]<<t.nx[2]<<t.nxny;
  jh.close();
  breite=t.nx[0];
  hoehe=t.nx[1];
  tiefe=t.nx[2];
  bh =t.nxny;
  QString M50Name=fname;
  M50Name=M50Name.replace(QRegExp(".m81$"),".m50");
  QFile m50(M50Name);
  //molekul molj;
  extern molekul mol;
  if (!m50.open(QIODevice::ReadOnly|QIODevice::Text)){
    bool ok,ok2;
    QString celltext;
    do {
      ok2=false;
      celltext = QInputDialog::getText (this,
          "Please enter cell parameters (a b c alpha beta gamma) (Angstroem degrees)",
          "Please enter cell parameters (a b c alpha beta gamma) (Angstroem degrees)", 
          QLineEdit::Normal,"",&ok);
      if (!celltext.isEmpty()){
        QStringList tok=celltext.split(" ",QString::SkipEmptyParts);
        if (tok.size()<6 )continue;
        mol.zelle.a=  tok.at(0).toDouble(&ok2);if (!ok2) continue;  
        mol.zelle.b=  tok.at(1).toDouble(&ok2);if (!ok2) continue;  
        mol.zelle.c=  tok.at(2).toDouble(&ok2);if (!ok2) continue;  
        mol.zelle.al= tok.at(3).toDouble(&ok2);if (!ok2) continue;  
        mol.zelle.be= tok.at(4).toDouble(&ok2);if (!ok2) continue;  
        mol.zelle.ga= tok.at(5).toDouble(&ok2);if (!ok2) continue; 
        mol.setup_zelle(); 
      } 
    }while (!ok||celltext.isEmpty());
  }else {
    QString all50 = m50.readAll();
    QStringList lines50 = all50.split(QRegExp("[\n\r]+"));
    bool hascell=false;
    bool ok=false,ok2=false;
    for (int i=0; i<lines50.size(); i++){
      if ((!hascell)&&(lines50.at(i).startsWith("cell"))) {
        QStringList tok=lines50.at(i).split(" ",QString::SkipEmptyParts);
        mol.zelle.a=  tok.at(1).toDouble(&ok2);ok=ok2;
        mol.zelle.b=  tok.at(2).toDouble(&ok2);ok&=ok2;
        mol.zelle.c=  tok.at(3).toDouble(&ok2);ok&=ok2;
        mol.zelle.al= tok.at(4).toDouble(&ok2);ok&=ok2;
        mol.zelle.be= tok.at(5).toDouble(&ok2);ok&=ok2;
        mol.zelle.ga= tok.at(6).toDouble(&ok2);ok&=ok2;
        mol.setup_zelle(); 
        hascell=true;
      }else if (lines50.at(i).startsWith("atom")) {
        QStringList tok=lines50.at(i).split(" ",QString::SkipEmptyParts);
        atypen.append(tok.at(1));
        habenatome=true;
      }

    }
    if (!ok) {
      QString celltext;
      do {
        ok2=false;
        celltext = QInputDialog::getText (this,
            "Please enter cell parameters (a b c alpha beta gamma) (Angstroem degrees)",
            "Please enter cell parameters (a b c alpha beta gamma) (Angstroem degrees)", 
            QLineEdit::Normal,"",&ok);
        if (!celltext.isEmpty()){
          QStringList tok=celltext.split(" ",QString::SkipEmptyParts);
          if (tok.size()<6 )continue;
          mol.zelle.a=  tok.at(0).toDouble(&ok2);if (!ok2) continue;  
          mol.zelle.b=  tok.at(1).toDouble(&ok2);if (!ok2) continue;  
          mol.zelle.c=  tok.at(2).toDouble(&ok2);if (!ok2) continue;  
          mol.zelle.al= tok.at(3).toDouble(&ok2);if (!ok2) continue;  
          mol.zelle.be= tok.at(4).toDouble(&ok2);if (!ok2) continue;  
          mol.zelle.ga= tok.at(5).toDouble(&ok2);if (!ok2) continue; 
          mol.setup_zelle(); 
        } 
      }while (!ok||celltext.isEmpty());
    }
  }
  V3 xd,yd,zd,xdk,ydk,zdk;
  xd=V3(t.dx[0],0,0);
  yd=V3(0,t.dx[1],0);
  zd=V3(0,0,t.dx[2]);
  mol.frac2kart(xd,xdk);
  mol.frac2kart(yd,ydk);
  mol.frac2kart(zd,zdk);
  x_dim=Vector3(xdk.x,xdk.y,xdk.z);
  y_dim=Vector3(ydk.x,ydk.y,ydk.z);
  z_dim=Vector3(zdk.x,zdk.y,zdk.z);
  printf("%9.6f %9.6f %9.6f\n",x_dim.x,x_dim.y,x_dim.z);
  printf("%9.6f %9.6f %9.6f\n",y_dim.x,y_dim.y,y_dim.z);
  printf("%9.6f %9.6f %9.6f\n\n",z_dim.x,z_dim.y,z_dim.z);
  printf("%9.6f %9.6f %9.6f\n",x_dim.x*breite,x_dim.y*breite,x_dim.z*breite);
  printf("%9.6f %9.6f %9.6f\n",y_dim.x*hoehe,y_dim.y*hoehe,y_dim.z*hoehe);
  printf("%9.6f %9.6f %9.6f %9.6f %9.6f %9.6f %9.6f\n", mol.zelle.a ,mol.zelle.b , mol.zelle.c, mol.zelle.al, mol.zelle.be, mol.zelle.ga, mol.zelle.V);
  printf("%9.6f %9.6f %9.6f\n",z_dim.x*tiefe,z_dim.y*tiefe,z_dim.z*tiefe);
  QString M40Name=fname;
  if (habenatome){
    M40Name=M40Name.replace(QRegExp(".m81$"),".m40");
    QFile m40(M40Name);
    if (m40.open(QIODevice::ReadOnly|QIODevice::Text)){
      QString all40 = m40.readAll();
      QStringList lines40 = all40.split(QRegExp("[\n\r]+"));
      QStringList tok;
      extern int atmax;
      extern QList<INP> asymmUnit;
      INP newAtom;
      newAtom.part=0;
      extern int smx;
      smx=0;
      int iread=0;
      for (int li=0; li<lines40.size();li++){
        tok.clear();
        tok=lines40.at(li).split(" ",QString::SkipEmptyParts);
        if (tok.size()){
          if (li==0) smx=atmax = tok.at(0).toInt();
          if ((li)&&(!smx)) return;
          if ((asymmUnit.size()<smx)&&(lines40.at(li).contains(QRegExp("^[A-z]+")))){
            //C1        1  1     1.000000 0.199051 0.122184 0.071881    
            //123456789012345678901234567890123456789012345678901234567890
            //000000000111111111122222222223333333333444444444455555555556
            //
            newAtom.lmax=-1;
            if ((lines40.at(li).length()>16)&&(lines40.at(li)[16]!=' ')) 
              iread=sscanf(lines40.at(li).toStdString().c_str(),"%8s%3d%3d%3d %9lf%9lf%9lf%9lf",newAtom.atomname,&newAtom.atomtype,&newAtom.jtf, &newAtom.lmax, &newAtom.amul, &newAtom.frac.x, &newAtom.frac.y, &newAtom.frac.z);
            else iread=sscanf(lines40.at(li).toStdString().c_str(),"%8s%3d%3d    %9lf%9lf%9lf%9lf",newAtom.atomname,&newAtom.atomtype,&newAtom.jtf,  &newAtom.amul, &newAtom.frac.x, &newAtom.frac.y, &newAtom.frac.z);
            //        qDebug()<<all.at(li)<<all.at(li)[16];
            //        printf("'11111111222333444 555555555666666666777777777888888888'\n");
            printf("%3d%3d%3d %-10s %4d %12.6f%12.6f%12.6f%12.6f\n",iread,newAtom.jtf,newAtom.lmax,newAtom.atomname,newAtom.atomtype,newAtom.frac.x,newAtom.frac.y,newAtom.frac.z,newAtom.amul);
            if (newAtom.jtf>0) {
              li++;
              sscanf(lines40.at(li).toStdString().c_str(),"%9lf%9lf%9lf%9lf%9lf%9lf",
                  &newAtom.uf.m11,
                  &newAtom.uf.m22,
                  &newAtom.uf.m33,
                  &newAtom.uf.m12,
                  &newAtom.uf.m13,
                  &newAtom.uf.m23);

            }

            newAtom.OrdZahl = mol.Get_OZ(atypen.at(newAtom.atomtype-1));
            if (newAtom.amul==0) newAtom.OrdZahl = -1;
            asymmUnit.append(newAtom);
          }
        }
      }
      for (int i=0;i<asymmUnit.size();i++) { 
        mol.frac2kart(asymmUnit[i].frac,asymmUnit[i].kart);
        //    printf("%-10s %4d %12.6f%12.6f%12.6f\n",asymmUnit[i].atomname,asymmUnit[i].OrdZahl,asymmUnit[i].kart.x,asymmUnit[i].kart.y,asymmUnit[i].kart.z);
        if ((asymmUnit[i].uf.m22==0.0)&&(asymmUnit[i].uf.m33==0.0)){
          asymmUnit[i].u.m11=asymmUnit[i].u.m22=asymmUnit[i].u.m33=asymmUnit[i].uf.m11;
          asymmUnit[i].u.m12=asymmUnit[i].u.m13=asymmUnit[i].u.m23=asymmUnit[i].u.m21=asymmUnit[i].u.m31=asymmUnit[i].u.m32=0.0;}
        else mol.Uf2Uo(asymmUnit[i].uf,asymmUnit[i].u);
        //    printf("%12.6f%12.6f%12.6f%12.6f%12.6f%12.6f\n",asymmUnit[i].u.m11,asymmUnit[i].u.m22,asymmUnit[i].u.m33, asymmUnit[i].u.m12, asymmUnit[i].u.m13,asymmUnit[i].u.m23);
      }
    }
  }
  // exit(0);
}

void MolIso::readXDGridHeader(QString fname,int &fileType){
  if (fname.endsWith(".m81",Qt::CaseInsensitive)) {
    readJanaHeader(fname);
    GHName=fname;
    fileType=81;
    return;
  }
  QFile gh(fname);

  printf("%s\n",fname.toStdString().c_str());
  gh.open(QIODevice::ReadOnly);
  QString all =gh.readAll();
  QStringList lines = all.split(QRegExp("[\n\r]+"));
  if ((lines.size())&&(lines.at(0).contains("3DGRDFIL "))) {
    fileType=0;
    extern molekul mol;
    extern int atmax;
    extern QList<INP> asymmUnit;
    INP newAtom;
    newAtom.part=0;
    extern int smx;
    int i=0;
    while (!lines.at(i).contains("Gridpoints")) i++;
    QStringList tok = lines[i+1].split(' ',QString::SkipEmptyParts);
    breite= tok.at(0).toInt();
    hoehe=  tok.at(1).toInt();
    tiefe=  tok.at(2).toInt();
    bh = hoehe*breite;
    tok = lines[i+3].split(' ',QString::SkipEmptyParts);
    x_dim = Vector3(tok.at(0).toFloat()/breite,0,0);
    y_dim = Vector3(0,tok.at(1).toFloat()/hoehe,0);
    z_dim = Vector3(0,0,tok.at(2).toFloat()/tiefe);
    while (!lines.at(i).contains("Objects")) i++;
    {
      i++;
      atomanzahl=smx=atmax=lines[i].toInt();
      i++;
      for (int j=0 ; j<atmax; j++){
        tok = lines[i+j].split(' ',QString::SkipEmptyParts);
        strncpy(newAtom.atomname,tok.at(0).toLatin1(),38);
        newAtom.kart.x=tok.at(1).toDouble();
        newAtom.kart.y=tok.at(2).toDouble();
        newAtom.kart.z=tok.at(3).toDouble();
        if (tok.contains("ATOM")){
          char *dv=NULL,dm[80];
          dv=strcpy(dm,newAtom.atomname);
          if (dv[0]=='X') dv+=3;
          strtok(dv,"(1234567890+- ");
          newAtom.OrdZahl=mol.Get_OZ(dv);
        } else	
          newAtom.OrdZahl=-1;
        asymmUnit.append(newAtom);
      }
    }

    if (!smx) {
      smx=8;    
      for (int iz=0; iz <2;iz++)
        for (int iy=0; iy <2;iy++)
          for (int ix=0; ix <2;ix++) {
            Vector3 ppp =  ((ix*breite)*x_dim);
            ppp += ((iy*hoehe)*y_dim);
            ppp += ((iz*tiefe)*z_dim);
            newAtom.kart=V3(ppp.x,ppp.y,ppp.z);
            newAtom.OrdZahl=-1;
            strcpy(newAtom.atomname,"x");
            asymmUnit.append(newAtom);
          }
    }
  }else{//may be this is a cube file?
    printf("cubefile?\n");
    fileType=125;
    double a,b,c;
    double bohr=0.5291775108;
    if (lines.size()>6) {
      QStringList tok;
      tok=lines.at(2).split(QRegExp("\\s+"),QString::SkipEmptyParts);
      if (tok.size()==4){	
        extern int atmax;
        extern QList<INP> asymmUnit;
        INP newAtom;
        extern int smx;
        char PSE_Symbol[109][3] = {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar",
          "K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr",
          "Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe",
          "Cs","Ba", "La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu",
          "Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra",
          "Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No","Lr","Ku","Ha","Rf","Ns","Hs","Mt"};
        atomanzahl=smx=atmax=tok.at(0).toInt();
        a=tok.at(1).toDouble();
        b=tok.at(2).toDouble();
        c=tok.at(3).toDouble();
        tok=lines.at(3).split(QRegExp("\\s+"),QString::SkipEmptyParts);	
        breite= tok.at(0).toInt();
        x_dim = Vector3(tok.at(1).toFloat(),tok.at(2).toFloat(),tok.at(3).toFloat());
        tok=lines.at(4).split(QRegExp("\\s+"),QString::SkipEmptyParts);	
        hoehe=  tok.at(0).toInt();

        y_dim = Vector3(tok.at(1).toFloat(),tok.at(2).toFloat(),tok.at(3).toFloat());
        tok=lines.at(5).split(QRegExp("\\s+"),QString::SkipEmptyParts);	
        tiefe=  tok.at(0).toInt();
        z_dim = Vector3(tok.at(1).toFloat(),tok.at(2).toFloat(),tok.at(3).toFloat());
        x_dim *= bohr;
        y_dim *= bohr;
        z_dim *= bohr;
        bh = hoehe*breite;
        for (int i=0;i<atmax;i++){
          tok=lines.at(6+i).split(QRegExp("\\s+"),QString::SkipEmptyParts);
          newAtom.OrdZahl=tok.at(0).toInt()-1;
          strcpy(newAtom.atomname,QString("%1%2").arg(PSE_Symbol[newAtom.OrdZahl]).arg(i+1).toLatin1());
          newAtom.kart.x=(tok.at(2).toDouble()-a)*bohr;
          newAtom.kart.y=(tok.at(3).toDouble()-b)*bohr;
          newAtom.kart.z=(tok.at(4).toDouble()-c)*bohr;
          asymmUnit.append(newAtom);
        }
      }
    }
  }
  GHName=fname;
  gh.close();
}
void MolIso::makeFaces(int nn, Node poly[] ){
  //   /* 
  int n=0;
  int ly[13];
  for (int j=0;j<nn;j++)//zu nahe beieinander liegende verts ignorieren
    if (Distance(orte.at(poly[j].index).vertex,orte.at(poly[(j+1)%nn].index).vertex)>0.00)
      ly[n++]=poly[j].index;
    else{
      orte[poly[j].index].vertex=
        orte[poly[(j+1)%nn].index].vertex=
        (orte.at(poly[j].index).vertex+orte.at(poly[(j+1)%nn].index).vertex)*0.5f;
      orte[poly[j].index].color=
        orte[poly[(j+1)%nn].index].color=
        (orte.at(poly[j].index).color+orte.at(poly[(j+1)%nn].index).color)*0.5f;
      ly[n++]=poly[(j+1)%nn].index;
      j++;
    }
  GLfloat mid_col = 0.0;
  Vector3 mid_ver = Vector3(0,0,0);
  Vector3 mid_nor = Vector3(0,0,0);
  for(int i=0; i<n; i++ ){
    mid_ver += orte.at(ly[i]).vertex;
    mid_nor += orte.at(ly[i]).normal;
    mid_col += orte.at(ly[i]).color;
  }
  mid_ver *= (1.0/n);
  Normalize(mid_nor);
  mid_col *= (1.0/n);
  if (n<3) return;  //weniger als 3 verts -> nichts zu tun
  Polygn neupoly;
  neupoly.vertex=mid_ver;
  neupoly.n=n;
  if (orte.at(poly[0].index).direct*iso_level<0.0){//drehsinn der polygone abfragen* /
    for(int i=0; i<n; i++ ){
      neupoly.ii[i]=ly[i];
    }
  }else{
    for(int i=n-1,k=0; i>=0; i-- ){
      neupoly.ii[i]=ly[k++];
    }
  }//else* /
  pgns.append(neupoly);
  //  */
}
void MolIso::simpelGrad(void){
  GLfloat hier;
  for (int i=1; i<breite-1;i++){
    for (int j=1; j<hoehe-1;j++){
      for (int k=1; k<tiefe-1;k++){
        hier=data[(i  )+(j  )*breite+(k  )*bh];
        grad[i+j*breite+k*bh].x=(
            -1*(data[(i-1)+(j-1)*breite+(k-1)*bh]-hier)+//0  
            (data[(i+1)+(j-1)*breite+(k-1)*bh]-hier)+//2
            -1*(data[(i-1)+(j  )*breite+(k-1)*bh]-hier)+//3
            (data[(i+1)+(j  )*breite+(k-1)*bh]-hier)+//5  
            -1*(data[(i-1)+(j+1)*breite+(k-1)*bh]-hier)+//6  
            (data[(i+1)+(j+1)*breite+(k-1)*bh]-hier)+//8  
            -1*(data[(i-1)+(j-1)*breite+(k-1)*bh]-hier)+//9 
            (data[(i+1)+(j-1)*breite+(k  )*bh]-hier)+//11 
            -1*(data[(i-1)+(j  )*breite+(k  )*bh]-hier)+//12 	    					
            (data[(i+1)+(j  )*breite+(k  )*bh]-hier)+//14 
            -1*(data[(i-1)+(j+1)*breite+(k  )*bh]-hier)+//15
            (data[(i+1)+(j+1)*breite+(k  )*bh]-hier)+//17
            -1*(data[(i-1)+(j-1)*breite+(k+1)*bh]-hier)+//18 
            (data[(i+1)+(j-1)*breite+(k+1)*bh]-hier)+//20 
            -1*(data[(i-1)+(j  )*breite+(k+1)*bh]-hier)+//21
            (data[(i+1)+(j  )*breite+(k+1)*bh]-hier)+//23 
            -1*(data[(i-1)+(j+1)*breite+(k+1)*bh]-hier)+//24 
            (data[(i+1)+(j+1)*breite+(k+1)*bh]-hier))/26.0;//26 

        grad[i+j*breite+k*bh].y=(
            -1*(data[(i-1)+(j-1)*breite+(k-1)*bh]-hier)+//0
            -1*(data[(i  )+(j-1)*breite+(k-1)*bh]-hier)+//1
            -1*(data[(i+1)+(j-1)*breite+(k-1)*bh]-hier)+//2  
            (data[(i-1)+(j+1)*breite+(k-1)*bh]-hier)+//6  
            (data[(i  )+(j+1)*breite+(k-1)*bh]-hier)+//7  
            (data[(i+1)+(j+1)*breite+(k-1)*bh]-hier)+//8  
            -1*(data[(i-1)+(j-1)*breite+(k-1)*bh]-hier)+//9  
            -1*(data[(i  )+(j-1)*breite+(k  )*bh]-hier)+//10 
            -1*(data[(i+1)+(j-1)*breite+(k  )*bh]-hier)+//11
            (data[(i-1)+(j+1)*breite+(k  )*bh]-hier)+//15 
            (data[(i  )+(j+1)*breite+(k  )*bh]-hier)+//16 
            (data[(i+1)+(j+1)*breite+(k  )*bh]-hier)+//17
            -1*(data[(i-1)+(j-1)*breite+(k+1)*bh]-hier)+//18 
            -1*(data[(i  )+(j-1)*breite+(k+1)*bh]-hier)+//19 
            -1*(data[(i+1)+(j-1)*breite+(k+1)*bh]-hier)+//20 
            (data[(i-1)+(j+1)*breite+(k+1)*bh]-hier)+//24 
            (data[(i  )+(j+1)*breite+(k+1)*bh]-hier)+//25 
            (data[(i+1)+(j+1)*breite+(k+1)*bh]-hier))/26.0;//26 

        grad[i+j*breite+k*bh].z=(
            -1*(data[(i-1)+(j-1)*breite+(k-1)*bh]-hier)+//0
            -1*(data[(i  )+(j-1)*breite+(k-1)*bh]-hier)+//1
            -1*(data[(i+1)+(j-1)*breite+(k-1)*bh]-hier)+//2
            -1*(data[(i-1)+(j  )*breite+(k-1)*bh]-hier)+//3  
            -1*(data[(i  )+(j  )*breite+(k-1)*bh]-hier)+//4
            -1*(data[(i+1)+(j  )*breite+(k-1)*bh]-hier)+//5  
            -1*(data[(i-1)+(j+1)*breite+(k-1)*bh]-hier)+//6  
            -1*(data[(i  )+(j+1)*breite+(k-1)*bh]-hier)+//7  
            -1*(data[(i+1)+(j+1)*breite+(k-1)*bh]-hier)+//8
            (data[(i-1)+(j-1)*breite+(k+1)*bh]-hier)+//18 
            (data[(i  )+(j-1)*breite+(k+1)*bh]-hier)+//19 
            (data[(i+1)+(j-1)*breite+(k+1)*bh]-hier)+//20 
            (data[(i-1)+(j  )*breite+(k+1)*bh]-hier)+//21 
            (data[(i  )+(j  )*breite+(k+1)*bh]-hier)+//22
            (data[(i+1)+(j  )*breite+(k+1)*bh]-hier)+//23 
            (data[(i-1)+(j+1)*breite+(k+1)*bh]-hier)+//24 
            (data[(i  )+(j+1)*breite+(k+1)*bh]-hier)+//25 
            (data[(i+1)+(j+1)*breite+(k+1)*bh]-hier))/26.0;//26 
        if (i==1) {
          grad[0+j*breite+k*bh].x= grad[i+j*breite+k*bh].x;
          grad[0+j*breite+k*bh].y= grad[i+j*breite+k*bh].y;
          grad[0+j*breite+k*bh].z= grad[i+j*breite+k*bh].z;
        }if (j==1) {
          grad[i+0*breite+k*bh].x= grad[i+j*breite+k*bh].x;
          grad[i+0*breite+k*bh].y= grad[i+j*breite+k*bh].y;
          grad[i+0*breite+k*bh].z= grad[i+j*breite+k*bh].z;
        }if (k==1) {
          grad[i+j*breite+0*bh].x= grad[i+j*breite+k*bh].x;
          grad[i+j*breite+0*bh].y= grad[i+j*breite+k*bh].y;
          grad[i+j*breite+0*bh].z= grad[i+j*breite+k*bh].z;
        }
        if (i==breite-2) {
          grad[breite-1+j*breite+k*bh].x= grad[i+j*breite+k*bh].x;
          grad[breite-1+j*breite+k*bh].y= grad[i+j*breite+k*bh].y;
          grad[breite-1+j*breite+k*bh].z= grad[i+j*breite+k*bh].z;
        }if (j==hoehe-2) {
          grad[i+(hoehe-1)*breite+k*bh].x= grad[i+j*breite+k*bh].x;
          grad[i+(hoehe-1)*breite+k*bh].y= grad[i+j*breite+k*bh].y;
          grad[i+(hoehe-1)*breite+k*bh].z= grad[i+j*breite+k*bh].z;
        }if (k==tiefe-2) {
          grad[i+j*breite+(tiefe-1)*bh].x= grad[i+j*breite+k*bh].x;
          grad[i+j*breite+(tiefe-1)*bh].y= grad[i+j*breite+k*bh].y;
          grad[i+j*breite+(tiefe-1)*bh].z= grad[i+j*breite+k*bh].z;
        }
      }
    }
  }
}
void MolIso::CalcVertex( int ix, int iy, int iz ) {
  GLfloat vo, vx=0, vy=0, vz=0,sig;
  //Vector3 movo(0.5,0.5,0.5);
  Vector3 test3= ((breite-1)/-2.0) *  x_dim + ((hoehe-1)/-2.0) * y_dim + ((tiefe-1)/-2.0) * z_dim + orig;
  if (cubeiso) test3 =orig;
  Ort o;
  sig = (iso_level>0)?-1:1;
  vo = data.at(ix+iy*breite+iz*bh)  - iso_level;
  if( ix != breite-1 ) vx = data.at((1+ix)+iy*breite+iz*bh) - iso_level;
  if( iy != hoehe-1 ) vy = data.at(ix+(iy+1)*breite+iz*bh) - iso_level;
  if( iz != tiefe-1 ) vz = data.at(ix+iy*breite+(iz+1)*bh) - iso_level;
  if( ix != breite-1 && Intersect(vo,vx) ){
    o.vertex= x_dim*((vo/(vo-vx))+ix) + y_dim*iy + z_dim*iz;
    o.vertex+=test3;
    o.normal= Normalize(sig*( grad[ix+iy*breite+iz*bh]*(1.0-vo/(vo-vx)) +grad[(1+ix)+iy*breite+iz*bh]*(1.0-vo/(vo-vx))));
    o.color= mdata[ix+iy*breite+iz*bh]*(1.0-vo/(vo-vx))+vo/(vo-vx)*mdata[(1+ix)+iy*breite+iz*bh];
    orte.append(o);
    nodex[ix+iy*breite+iz*bh].index=orte.size()-1;
    nodex[ix+iy*breite+iz*bh].flag=1; 
  }else{
    nodex[ix+iy*breite+iz*bh].flag   = 0;
  }
  if( iy != hoehe-1 && Intersect(vo,vy) ){
    o.vertex = x_dim*ix + y_dim*((vo/(vo-vy))+iy) + z_dim*iz;
    o.vertex+=test3;
    o.normal=Normalize(sig*( grad[ix+iy*breite+iz*bh]*(1.0-vo/(vo-vy)) +grad[ix+(iy+1)*breite+iz*bh]*(1.0-vo/(vo-vy))));
    o.color=mdata[ix+iy*breite+iz*bh]*(1.0-vo/(vo-vy))+ vo/(vo-vy)*mdata[ix+(iy+1)*breite+iz*bh];
    orte.append(o);
    nodey[ix+iy*breite+iz*bh].index=orte.size()-1;
    nodey[ix+iy*breite+iz*bh].flag=1;
  }else{
    nodey[ix+iy*breite+iz*bh].flag=0;
  }
  if( iz != tiefe-1 && Intersect(vo,vz) ){
    o.vertex = x_dim*ix + y_dim*iy + ((vo/(vo-vz))+iz)*z_dim;
    o.vertex+=test3;
    o.normal = Normalize(sig*( grad[ix+iy*breite+iz*bh]*(1.0-vo/(vo-vz)) +grad[ix+iy*breite+(iz+1)*bh]*(1.0-vo/(vo-vz))));
    o.color = mdata[ix+iy*breite+iz*bh]*(1.0-vo/(vo-vz))+vo/(vo-vz)*mdata[ix+iy*breite+(iz+1)*bh];
    orte.append(o);
    nodez[ix+iy*breite+iz*bh].index = orte.size()-1;
    nodez[ix+iy*breite+iz*bh].flag=1;
  }else{
    nodez[ix+iy*breite+iz*bh].flag  = 0;
  }
}

void MolIso::CalcVertexes( void ) {
  //  printf("ooo %g %g %g\n",orig.x,orig.y,orig.z);
  for( int ix=0; ix<breite; ix++ ){
    for( int iy=0; iy<hoehe; iy++ ){
      for( int iz=0; iz<tiefe; iz++ ){
        CalcVertex(ix,iy,iz);
      }
    }
  }
}
int MolIso::IndexSelected( Node& node0, Node& node1, Node& node2, Node& node3 ) {
  if( node1 && node2 && node3 ){
    GLfloat d1 = Distance( orte.at(node0.index).vertex, orte.at(node1.index).vertex ) + 
      Distance( orte.at(node3.index).vertex, orte.at(node2.index).vertex );
    GLfloat d2 = Distance( orte.at(node0.index).vertex, orte.at(node2.index).vertex ) + 
      Distance( orte.at(node3.index).vertex, orte.at(node1.index).vertex );
    if( d1 > d2 ) return 2; else return 1;
  }else{
    if(      node1 )   return 1;
    else if( node2 )   return 2;
    else if( node3 )   return 3;
  }
  return 0;
}

Vector3& MolIso::VectorSelected( Node& node0, Node& node1, Node& node2, Node& node3 ) {
  if( node1 && node2 && node3 ){
    GLfloat d1 = Distance( orte.at(node0.index).vertex, orte.at(node1.index).vertex ) + 
      Distance( orte.at(node3.index).vertex, orte.at(node2.index).vertex );
    GLfloat d2 = Distance( orte.at(node0.index).vertex, orte.at(node2.index).vertex ) + 
      Distance( orte.at(node3.index).vertex, orte.at(node1.index).vertex );
    if( d1 > d2 ) return orte[node2.index].vertex; else return orte[node1.index].vertex;
  }else{
    if(      node1 )   return orte[node1.index].vertex;
    else if( node2 )   return orte[node2.index].vertex;
    else if( node3 )   return orte[node3.index].vertex;
  }
  return orte[node0.index].vertex;
}

GLfloat MolIso::CalcNormalX( int ix, int iy, int iz ) {
  Vector3 tang[4];

  if(( iy == hoehe-1 )||(ix == breite-1)){
    tang[0] = orte[nodex[ix+iy*breite+iz*bh].index].vertex;
  }else{
    tang[0] = VectorSelected(nodex[ix+iy*breite+iz*bh],nodey[ix+iy*breite+iz*bh],
        nodey[1+ix+iy*breite+iz*bh],nodex[ix+(iy+1)*breite+iz*bh]);
  }
  if(( iy == 0 )||(ix == breite-1)){
    tang[1] = orte[nodex[ix+iy*breite+iz*bh].index].vertex;
  }else{
    tang[1] = VectorSelected(nodex[ix+iy*breite+iz*bh],nodey[ix+(iy-1)*breite+iz*bh],
        nodey[(ix+1)+(iy-1)*breite+(iz)*bh],nodex[ix+(iy-1)*breite+(iz)*bh]) ;
  }
  if(( iz == tiefe-1 )||(ix == breite-1)){
    tang[2] = orte[nodex[ix+iy*breite+iz*bh].index].vertex;
  }else{
    tang[2] = VectorSelected(nodex[ix+iy*breite+iz*bh],nodez[ix+iy*breite+iz*bh],
        nodez[(1+ix)+(iy)*breite+iz*bh],nodex[ix+iy*breite+(iz+1)*bh]) ;
  }

  if(( iz == 0 )||(ix == breite-1)){
    tang[3] = orte[nodex[ix+iy*breite+iz*bh].index].vertex;
  }else{
    tang[3] = VectorSelected(nodex[ix+iy*breite+iz*bh],nodez[ix+(iy)*breite+(iz-1)*bh],
        nodez[(ix+1)+(iy)*breite+(iz-1)*bh],nodex[ix+(iy)*breite+(iz-1)*bh]) ;
  }
  return (((tang[0] - tang[1])%(tang[2] - tang[3])) * grad[ix+iy*breite+iz*bh]);
}

GLfloat MolIso::CalcNormalY( int ix, int iy, int iz ) {
  Vector3 tang[4];

  if(( ix == breite-1 )||(iy == hoehe-1)){
    tang[0] = orte[nodey[ix+iy*breite+iz*bh].index].vertex;
  }else{
    tang[0] =  VectorSelected(nodey[ix+iy*breite+iz*bh],nodex[ix+iy*breite+iz*bh],
        nodex[ix+(iy+1)*breite+iz*bh],nodey[1+ix+iy*breite+iz*bh]);
  }

  if(( ix == 0 )||(iy == hoehe-1)){
    tang[1] = orte[nodey[ix+iy*breite+iz*bh].index].vertex;
  }else{
    tang[1] =  VectorSelected(nodey[ix+iy*breite+iz*bh],nodex[(ix-1)+(iy)*breite+(iz)*bh],
        nodex[(ix-1)+(iy+1)*breite+(iz)*bh],nodey[(ix-1)+(iy)*breite+(iz)*bh]) ;
  }

  if(( iz == tiefe-1 )||(iy == hoehe-1)){
    tang[2] = orte[nodey[ix+iy*breite+iz*bh].index].vertex;
  }else{
    tang[2] =  VectorSelected(nodey[ix+iy*breite+iz*bh],nodez[ix+iy*breite+iz*bh],
        nodez[ix+(iy+1)*breite+iz*bh],nodey[ix+iy*breite+(iz+1)*bh]) ;
  }

  if(( iz == 0 )||(iy == hoehe-1)){
    tang[3] = orte[nodey[ix+iy*breite+iz*bh].index].vertex;
  }else{
    tang[3] =  VectorSelected(nodey[ix+iy*breite+iz*bh],nodez[(ix)+(iy)*breite+(iz-1)*bh],
        nodez[(ix)+(iy+1)*breite+(iz-1)*bh],nodey[(ix)+(iy)*breite+(iz-1)*bh]) ;
  }

  return (((tang[2] - tang[3])%(tang[0] - tang[1]))*grad[ix+iy*breite+iz*bh]);
}

GLfloat MolIso::CalcNormalZ( int ix, int iy, int iz ) {
  Vector3 tang[4];

  if(( ix == breite-1 )||(iz == tiefe-1)){
    tang[0] = orte[nodez[ix+iy*breite+iz*bh].index].vertex;
  }else{
    tang[0] =  VectorSelected(nodez[ix+iy*breite+iz*bh],nodex[ix+iy*breite+iz*bh],
        nodex[ix+iy*breite+(iz+1)*bh],nodez[1+ix+iy*breite+iz*bh]) ;
  }

  if(( ix == 0 )||(iz == tiefe-1)){
    tang[1] = orte[nodez[ix+iy*breite+iz*bh].index].vertex;
  }else{
    tang[1] =  VectorSelected(nodez[ix+iy*breite+iz*bh],nodex[(ix-1)+(iy)*breite+(iz)*bh],
        nodex[(ix-1)+(iy)*breite+(iz+1)*bh],nodez[(ix-1)+(iy)*breite+(iz)*bh]) ;
  }

  if(( iy == hoehe-1 )||(iz == tiefe-1)){
    tang[2] = orte[nodez[ix+iy*breite+iz*bh].index].vertex;
  }else{
    tang[2] =  VectorSelected(nodez[ix+iy*breite+iz*bh],nodey[ix+iy*breite+iz*bh],
        nodey[ix+iy*breite+(iz+1)*bh],nodez[ix+(iy+1)*breite+iz*bh]) ;
  }

  if(( iy == 0 )||(iz == tiefe-1)){
    tang[3] = orte[nodez[ix+iy*breite+iz*bh].index].vertex;
  }else{
    tang[3] =  VectorSelected(nodez[ix+iy*breite+iz*bh],nodey[(ix)+(iy-1)*breite+(iz)*bh],
        nodey[(ix)+(iy-1)*breite+(iz+1)*bh],nodez[(ix)+(iy-1)*breite+(iz)*bh]) ;
  }

  return (((tang[0] - tang[1])%(tang[2] - tang[3]))*grad[ix+iy*breite+iz*bh]);
}
void MolIso::CalcNormals( void ) {
  for( int ix=0; ix<breite; ix++ ){
    for( int iy=0; iy<hoehe; iy++ ){
      for( int iz=0; iz<tiefe; iz++ ){
        if( nodex[ix+iy*breite+iz*bh] ){
          orte[nodex[ix+iy*breite+iz*bh].index].direct = (CalcNormalX(ix,iy,iz)>0)?1:-1;
        }
        if( nodey[ix+iy*breite+iz*bh] ){
          orte[nodey[ix+iy*breite+iz*bh].index].direct = (CalcNormalY(ix,iy,iz)>0)?1:-1;
        }
        if( nodez[ix+iy*breite+iz*bh] ){
          orte[nodez[ix+iy*breite+iz*bh].index].direct = (CalcNormalZ(ix,iy,iz)>0)?1:-1;
        }
      }
    }
  }
}
void MolIso::MakeElement( int ix, int iy, int iz ,int s1, int s2) {//das ist der Teil des japanischen Programms den ich nicht verstehe.
  //Hauptsache fuktioniert.
  //    /*
  static int conn[12][2][4] = {           //char->int wegen warning g++ >3.0
    {{ 0, 1, 7, 6}, { 0, 2, 8, 3}},  //  0
    {{ 1, 2, 5, 4}, { 1, 0, 6, 7}},  //  1
    {{ 2, 0, 3, 8}, { 2, 1, 4, 5}},  //  2
    {{ 3, 8, 2, 0}, { 3, 4,10, 9}},  //  3
    {{ 4, 3, 9,10}, { 4, 5, 2, 1}},  //  4
    {{ 5, 4, 1, 2}, { 5, 6, 9,11}},  //  5
    {{ 6, 5,11, 9}, { 6, 7, 1, 0}},  //  6
    {{ 7, 6, 0, 1}, { 7, 8,11,10}},  //  7
    {{ 8, 7,10,11}, { 8, 3, 0, 2}},  //  8
    {{ 9,10, 4, 3}, { 9,11, 5, 6}},  //  9
    {{10,11, 8, 7}, {10, 9, 3, 4}},  // 10
    {{11, 9, 6, 5}, {11,10, 7, 8}}   // 11
  };
  static Node node[12];
  static Node polygon[12];

  node[ 0] = nodex[ix+iy*s1+iz*s2];        // 000x
  node[ 1] = nodey[ix+iy*s1+iz*s2];        // 000y
  node[ 2] = nodez[ix+iy*s1+iz*s2];        // 000z
  node[ 3] = nodex[ix+iy*s1+(iz+1)*s2];    // 001y
  node[ 4] = nodey[ix+iy*s1+(iz+1)*s2];    // 001z
  node[ 5] = nodez[ix+(iy+1)*s1+iz*s2];    // 010x
  node[ 6] = nodex[ix+(iy+1)*s1+iz*s2];    // 010y
  node[ 7] = nodey[1+ix+iy*s1+iz*s2];      // 100y
  node[ 8] = nodez[1+ix+iy*s1+iz*s2];      // 100z
  node[ 9] = nodex[ix+(iy+1)*s1+(iz+1)*s2];// 011x
  node[10] = nodey[(ix+1)+iy*s1+(iz+1)*s2];// 101y
  node[11] = nodez[(ix+1)+(iy+1)*s1+iz*s2];// 110z
  if (((char)node[0]+node[1]+node[2]+node[3]+node[4]+node[5]
        +node[6]+node[7]+node[8]+node[9]+node[10]+node[11])==0) return;
  for( int is=0; is<12; is++ ) {
    if( !node[is] ) continue;

    int n=0, i=is, m=0;//,ai=i;
    do {
      polygon[n++]= node[i];
      int sol = IndexSelected(node[conn[i][m][0]],node[conn[i][m][1]],
          node[conn[i][m][2]],node[conn[i][m][3]]);
   //   ai=i;
      i = conn[i][m][sol];

      if( sol == 2 ) m ^= 1;
      node[i].flag = 0;
    }while( (i!=is)&&(n<11) );
    {
      makeFaces( n, polygon );
    }
  }
  //  */
}

void MolIso::createSurface(QString isoFileName, QString mapFileName, QString &storeFaceName,int fileType){
  if (storeFaceName.isEmpty()){
    QTemporaryFile *tf = new  QTemporaryFile();
    tf->open();
    storeFaceName=tf->fileName();
    tf->close();
    delete tf;
  }
  if ((mapFileName.contains('!'))||(mapFileName.isEmpty()))mapFileName=isoFileName;
  QFile *tf = new QFile(storeFaceName);
  data.clear();
  mdata.clear();
  extern QProgressBar *balken;
  balken->setMinimum(0);
  balken->setMaximum(100);
  balken->show();
  balken->setValue(1);
  QFile isoF(isoFileName);
  QFile mapF(mapFileName);
  if (fileType==0){//if (isoFileName.contains("grd")) {
    isoF.open(QIODevice::ReadOnly);
    while (!QString(isoF.readLine()).contains("Values")) {;}
    int p=0,pmax=breite*hoehe*tiefe,altb=0,b=0;
    while (!isoF.atEnd () && (p<pmax)) {
      QStringList numbers = QString(isoF.readLine()).split(" ",QString::SkipEmptyParts);
      for (int i=0; i<numbers.size();i++) data.append(numbers.at(i).toDouble());
      p+=numbers.size();
      b=37*p/pmax;
      if (b!=altb){
        balken->setValue(b);
        altb=b;
      }
    }
    isoF.close(); 
  }else if (fileType==125){// ein cube file hoffentlich
    isoF.open(QIODevice::ReadOnly);
    for (int i=0; i<(atomanzahl+6);i++) isoF.readLine();
    int pmax=breite*hoehe*tiefe,altb=0,b=0;
    int p=pmax-1;
    QVector<double> rewerte(pmax);
    while (!isoF.atEnd () && (p>=0)) {
      QStringList numbers = QString(isoF.readLine()).split(" ",QString::SkipEmptyParts);
      for (int i=0; i<numbers.size();i++) {rewerte[p--]=(numbers.at(i).toDouble());}
      b=38-37*p/pmax;
      if (b!=altb){
        balken->setValue(b);
        altb=b;
      }
    }
    for (int i=(tiefe)-1;i>=0;i--)
      for (int j=(hoehe) -1;j>=0;j--)
        for (int k=(breite) -1;k>=0;k--)
          data.append(rewerte[i+j*(tiefe)+k*(tiefe)*(hoehe)]);// */
    cubeiso=true;
  isoF.close(); 
  }else if(fileType==81){
  
    isoF.open(QIODevice::ReadOnly);
    float *floatdat=(float*)malloc(bh*sizeof(float));
    isoF.read((char*) floatdat,sizeof(float)*bh);//skip first record 
    for (int i=0;i<tiefe;i++){
    isoF.read((char*) floatdat,sizeof(float)*bh);
  //  printf("%f %f %f\n",floatdat[0],floatdat[1],floatdat[2]);
      for (int j=0;j<bh;j++)
          data.append(floatdat[j]);// */
    }
    cubeiso=true;
    isoF.close(); 
  }
  if (mapFileName==isoFileName) {
    mdata=data;
  }else {
    if (fileType==0){//if (mapFileName.contains("grd")) {
    mapF.open(QIODevice::ReadOnly);
      while (!QString(mapF.readLine()).contains("Values")) {;}
      int p=0,pmax=breite*hoehe*tiefe,altb=0,b=0;
      while (!mapF.atEnd () && (p<pmax)) {
        QStringList numbers = QString(mapF.readLine()).split(" ",QString::SkipEmptyParts);
        for (int i=0; i<numbers.size();i++) mdata.append(numbers.at(i).toDouble());
        p+=numbers.size();
        b=38+37*p/pmax;
        if (b!=altb){
          balken->setValue(b);
          altb=b;
        }
      }
    mapF.close();                                                                              
    }else if (fileType==125){//{ // ein cube file hoffentlich
    mapF.open(QIODevice::ReadOnly);
      for (int i=0; i<(atomanzahl+6);i++) mapF.readLine();
      int pmax=breite*hoehe*tiefe,altb=0,b=0;
      int p=pmax-1;
      QVector<double> rewerte(pmax);
      while (!mapF.atEnd () && (p>=0)) {
        QStringList numbers = QString(mapF.readLine()).split(" ",QString::SkipEmptyParts);
        for (int i=0; i<numbers.size();i++) {rewerte[p--]=(numbers.at(i).toDouble());}
        b=76-37*p/pmax;
        if (b!=altb){
          balken->setValue(b);
          altb=b;
        }
      }
      for (int i=(tiefe)-1;i>=0;i--)
        for (int j=(hoehe) -1;j>=0;j--)
          for (int k=(breite) -1;k>=0;k--)
            mdata.append(rewerte[i+j*(tiefe)+k*(tiefe)*(hoehe)]);// */
      cubemap=true;
      mapF.close();                                                                              
    }else if (fileType==81){
//      qDebug()<< "Des han I no`it g`schrieba!";
    mapF.open(QIODevice::ReadOnly);
    float *floatdat=(float*)malloc(bh*sizeof(float));
    mapF.read((char*) floatdat,sizeof(float)*bh);//skip first record 
    for (int i=0;i<tiefe;i++){
    mapF.read((char*) floatdat,sizeof(float)*bh);
  //  printf("%f %f %f\n",floatdat[0],floatdat[1],floatdat[2]);
      for (int j=0;j<bh;j++)
          mdata.append(floatdat[j]);// */
    }
    cubeiso=true;
    mapF.close(); 
    }
  }
  if (data.size()!=mdata.size()) {
    qDebug()<<"Map-Grid and Iso-Grid must have the same size!";
    qApp->quit();
  }
  if (( grad =(Vector3*)malloc(sizeof(Vector3)*bh*tiefe))==NULL) {
    fprintf(stderr ,"Less Memory(grad)%d !!\n",bh*tiefe);
    exit(1);
  }

  if (( nodex =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) {
    fprintf(stderr ,"Less Memory(X) %d!!\n",bh*tiefe*2);
    exit(1);  
  } 
  if (( nodey =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) { 
    fprintf(stderr ,"Less Memory(Y)!!\n");
    exit(1); 
  }
  if (( nodez =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) {
    fprintf(stderr ,"Less Memory(Z)!!\n");
    exit(1); 
  }
  simpelGrad();	
  balken->setValue(89);
  for (int k=0; k<isoValues.size(); k++){
    iso_level=isoValues.at(k);
    CalcVertexes();
    CalcNormals();
    std::cout<<iso_level;
    tf->open(QIODevice::WriteOnly);

    tf->write(QString("%1\n").arg(orte.size()).toLatin1());
    for (int i=0;i<orte.size();i++)
      tf->write(QString("%1  %2 %3 %4   %5 %6 %7  %8\n")
          .arg(i,-6)
          .arg(orte.at(i).vertex.x,9,'f',6)
          .arg(orte.at(i).vertex.y,9,'f',6)
          .arg(orte.at(i).vertex.z,9,'f',6)
          .arg(orte.at(i).normal.x,9,'f',6)
          .arg(orte.at(i).normal.y,9,'f',6)
          .arg(orte.at(i).normal.z,9,'f',6)
          .arg(orte.at(i).color,12,'f',7).toLatin1());
    for( int ix=0; ix<breite-1; ix++ )
      for( int iy=0; iy<hoehe-1; iy++ )
        for( int iz=0; iz<tiefe-1; iz++ )
          MakeElement(ix,iy,iz,breite,bh);

    PXsort();
    balken->setValue(90+(10/isoValues.size())*(k+1));
    QString Line="";
    for (int i=0; i<pgns.size();i++) {
      for (int j=0; j<pgns.at(i).n;j++){
        Line.append(QString("%1 ").arg(pgns.at(i).ii[j],6));
      }
      if (pgns.at(i).n>0) {
        Line.append("\n");
        tf->write(Line.toLatin1());
        Line.clear();
      }
    }
    tf->close();
  }
  balken->setValue(100);
  free(grad);
  free(nodex);
  free(nodey);
  free(nodez);
  mdata.clear();
  data.clear();
  pgns.clear();
  orte.clear();
}


void MolIso::createSurface(QString &storeFaceName, double proba){
  Farben=5;    
  farbe[0][0]=1.0;    
  farbe[0][1]=0;    
  farbe[0][2]=0;    
  farbe[0][3]=0.6;

  farbe[1][0]=1.0;    
  farbe[1][1]=0.0;    
  farbe[1][2]=0.0;    
  farbe[1][3]=0.5;

  farbe[2][0]=0.3;    
  farbe[2][1]=0.7;    
  farbe[2][2]=0.3;    
  farbe[2][3]=0.6;

  farbe[3][0]=0.0;    
  farbe[3][1]=0.0;    
  farbe[3][2]=1.0;    
  farbe[3][3]=0.5;

  farbe[4][0]=0;    
  farbe[4][1]=0;    
  farbe[4][2]=1.0;    
  farbe[4][3]=0.6;    
  QFile *tf = new QFile(storeFaceName);
  mdata=data;
  bh=breite*hoehe;
  extern QProgressBar *balken;
  balken->setMinimum(0);
  balken->setMaximum(100);
  balken->show();
  balken->setValue(0);
  if (( grad =(Vector3*)malloc(sizeof(Vector3)*bh*tiefe))==NULL) {
    fprintf(stderr ,"Less Memory(grad)%d !!\n",bh*tiefe);
    exit(1);
  }

  if (( nodex =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) {
    fprintf(stderr ,"Less Memory(X) %d!!\n",bh*tiefe*2);
    exit(1);  
  } 
  if (( nodey =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) { 
    fprintf(stderr ,"Less Memory(Y)!!\n");
    exit(1); 
  }
  if (( nodez =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) {
    fprintf(stderr ,"Less Memory(Z)!!\n");
    exit(1); 
  }
  simpelGrad();
  if (proba<1) {  
    CalcVertexes();
    CalcNormals();
    for( int ix=0; ix<breite-1; ix++ )
      for( int iy=0; iy<hoehe-1; iy++ )
        for( int iz=0; iz<tiefe-1; iz++ )
          MakeElement(ix,iy,iz,breite,bh);
    iso_level*=-1.0;
    CalcVertexes();
    CalcNormals();
    tf->open(QIODevice::WriteOnly|QIODevice::Text);

    tf->write(QString("%1\n").arg(orte.size()).toLatin1());
    for (int i=0;i<orte.size();i++){
      tf->write(QString("%1  %2 %3 %4   %5 %6 %7  %8\n")
          .arg(lineNr,-6)
          .arg(orte.at(i).vertex.x,9,'f',6)
          .arg(orte.at(i).vertex.y,9,'f',6)
          .arg(orte.at(i).vertex.z,9,'f',6)
          .arg(orte.at(i).normal.x,9,'f',6)
          .arg(orte.at(i).normal.y,9,'f',6)
          .arg(orte.at(i).normal.z,9,'f',6)
          .arg(orte.at(i).color,12,'f',7).toLatin1());
      lineNr++;
    }
    for( int ix=0; ix<breite-1; ix++ )
      for( int iy=0; iy<hoehe-1; iy++ )
        for( int iz=0; iz<tiefe-1; iz++ )
          MakeElement(ix,iy,iz,breite,bh);
    PXsort();
    QString Line="";
    for (int i=0; i<pgns.size();i++) {
      for (int j=0; j<pgns.at(i).n;j++){
        Line.append(QString("%1 ").arg(pgns.at(i).ii[j],6));
      }
      if (pgns.at(i).n>0) {
        Line.append("\n");
        tf->write(Line.toLatin1());
        Line.clear();
      }
    }
    tf->close();
  }else{
    CalcVertexes();
    CalcNormals();
    for( int ix=0; ix<breite-1; ix++ )
      for( int iy=0; iy<hoehe-1; iy++ )
        for( int iz=0; iz<tiefe-1; iz++ )
          MakeElement(ix,iy,iz,breite,bh);
    iso_level=-0.01;
    CalcVertexes();
    CalcNormals();
    tf->open(QIODevice::WriteOnly|QIODevice::Text);

    tf->write(QString("%1\n").arg(orte.size()).toLatin1());
    for (int i=0;i<orte.size();i++){
      tf->write(QString("%1  %2 %3 %4   %5 %6 %7  %8\n")
          .arg(lineNr,-6)
          .arg(orte.at(i).vertex.x,9,'f',6)
          .arg(orte.at(i).vertex.y,9,'f',6)
          .arg(orte.at(i).vertex.z,9,'f',6)
          .arg(orte.at(i).normal.x,9,'f',6)
          .arg(orte.at(i).normal.y,9,'f',6)
          .arg(orte.at(i).normal.z,9,'f',6)
      //    .arg(orte.at(i).color,12,'f',7).toLatin1());
      .arg((orte.at(i).color>0)?proba:-0.01,12,'f',7).toLatin1());
      lineNr++;
    }
    for( int ix=0; ix<breite-1; ix++ )
      for( int iy=0; iy<hoehe-1; iy++ )
        for( int iz=0; iz<tiefe-1; iz++ )
          MakeElement(ix,iy,iz,breite,bh);

    PXsort();
    QString Line="";
    for (int i=0; i<pgns.size();i++) {
      for (int j=0; j<pgns.at(i).n;j++){
        Line.append(QString("%1 ").arg(pgns.at(i).ii[j],6));
      }
      if (pgns.at(i).n>0) {
        Line.append("\n");
        tf->write(Line.toLatin1());
        Line.clear();
      }
    }
    tf->close();
  }
  free(grad);
  free(nodex);
  free(nodey);
  free(nodez);
  mdata.clear();
  data.clear();
  pgns.clear();
  orte.clear();
}

void MolIso::Farbverlauf (GLfloat wrt){
  static GLclampd ff[4];
  static GLdouble anf,lang;
  static int lauf=0;
  wrt=(wrt-min)/(max-min);
  wrt=(wrt>=1.0)?0.999999:wrt;
  wrt=(wrt<=0.0)?0.000001:wrt;
  lang=1.0/(Farben-1.0);
  lauf=(int (wrt/lang));
  anf=lang*lauf;
  ff[0]=((1.0-(wrt-anf)/lang)*farbe[lauf][0]+((wrt-anf)/lang)*farbe[lauf+1][0]);
  ff[1]=((1.0-(wrt-anf)/lang)*farbe[lauf][1]+((wrt-anf)/lang)*farbe[lauf+1][1]);
  ff[2]=((1.0-(wrt-anf)/lang)*farbe[lauf][2]+((wrt-anf)/lang)*farbe[lauf+1][2]);
  ff[3]=(1.0-(wrt-anf)/lang)*farbe[lauf][3]+((wrt-anf)/lang)*farbe[lauf+1][3];  
  glColor4dv(ff);
  return;
}
QColor MolIso::qtFarbe(int index){
  if ((index <0)||(index>6)) return QColor("gray");
  static QColor c;
  c.setRgbF(farbe[index][0],farbe[index][1],farbe[index][2],farbe[index][3]);
  return c;
}
MolIso::~MolIso(){
  if (mdata.size()) mdata.clear();
  if (data.size())  data.clear();
  if (orte.size()) orte.clear();
  if (pgns.size()) pgns.clear();
}
