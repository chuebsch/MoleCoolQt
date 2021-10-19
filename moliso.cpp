#include "moliso.h"
#include "molekul.h"
#include <QtGui>
#include "cubeGL.h"
#include <QFileDialog>

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
  calcextrema=false;
  cubeiso=false;
  cubemap=false;
  untersumme=obersumme=0;
  deV=0.0;
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
  thisIsPDF=false;
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
  extern FILE *polifile;
  printf("mima %s  OM = %s\n",(mima)?"true":"false",(om)?"true":"false");
  for (int i=0;i<6;i++){
    if ((mibas)&&(glIsList(mibas+i))) {
      //      printf("NOT deleting list #%d\n",mibas+i);
      ;
    }else{
      printf("deleting %d\n",mibas+i);
      if (mibas) glDeleteLists(mibas+i,1);
    }
  }
  //  mibas=0;
  //  mibas=glGenLists(6);*/
  extern QProgressBar *balken;
  Vector3 minV,maxV;
  if (orte.isEmpty()){
    QFile sf(fname);
    if (!sf.exists()) {qDebug()<<"The File "<<fname<<" does not exist!"; exit(0);}
    if (!sf.open(QIODevice::ReadOnly)) {qDebug()<<"Can not open "<<fname<<" for reading. Please check your rights."; exit(0);}

    Ort v;
    Polygn p;
    QString all =sf.readAll();
    if (all.isEmpty()) {qDebug()<<"Can not read  "<<fname<<". may be it is empty or corrupt!"; exit(0);}
    QStringList lines = all.split(QRegExp("[\n\r]{1,2}")); 
    all.clear();
    if (mima){
      min=1e99;
      max=-1e99;
    }
    mima=true;
    if (lines.size()){

      balken->setMinimum(0);
      balken->setMaximum(7);
      balken->show();
      double dm=0,ds=0,sigma=0,dmp=0,dmm=0,dsp=0,dsm=0,sigmap=0,sigmam=0,api;//,potsigplus=0,potsigminus=0;
      int np=0,nm=0;
      minV=Vector3(0,0,0);
      maxV=minV;
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
            minV=(v.color<min)?v.vertex:minV;
            maxV=(v.color>max)?v.vertex:maxV;
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
      api=0;/*
               if  ((minV.x!=maxV.x)&&(minV.y!=maxV.y)&&(minV.z!=maxV.z)){
               INP newAtom;
               extern QList<INP> xdinp;
               newAtom.molindex=0;
               newAtom.part=0;
               newAtom.sg=0;
               sprintf(newAtom.atomname,"%s","CP_Smin");
               newAtom.OrdZahl=-2;
               newAtom.cptype=-3;
               newAtom.kart.x=minV.x;
               newAtom.kart.y=minV.y;
               newAtom.kart.z=minV.z;
               newAtom.peakHeight=min;
               xdinp.append(newAtom);
               sprintf(newAtom.atomname,"%s","CP_Smax");
               newAtom.cptype=3;
               newAtom.kart.x=maxV.x;
               newAtom.kart.y=maxV.y;
               newAtom.kart.z=maxV.z;
               newAtom.peakHeight=max;
               xdinp.append(newAtom);
               printf("%f %f %f (%f)  %f %f %f (%f)\n",minV.x,minV.y,minV.z,min,maxV.x,maxV.y,maxV.z,max);
               }*/
      for (int i=0; i< orte.size(); i++){
        //        if (orte.at(i).color>0) potsigplus+=pow(orte.at(i).color-dm,2);
        //        else potsigminus+=pow(orte.at(i).color-dm,2);
        api+=fabs(orte.at(i).color-dm);
      }
      //potsigminus/=fmax(nm,1);
      //potsigplus/=fmax(np,1);
      api/=fmax(np+nm,1);
      if (polifile!=NULL) {
        int nt=np+nm;
        fprintf(polifile,"#   alpha      beta    VS+    VS-     PI     sig+     sig-   sigtot         nu    min    max  Volume enclosed     Area\n");
        fprintf(polifile,"#     %%         %%      e/A    e/A    e/A  e^2/A^2  e^2/A^2  e^2/A^2               e/A    e/A              A^3      A^2\n");
        fprintf(polifile,"%9.1f %9.1f %6.3f %6.3f %6.3f   %6.4f   %6.4f   %6.4f   %8.6f %6.3f %6.3f %16.1f"
            ,(double)np/(nt)*100.0,(double) nm/(nt)*100.0,dmp,dmm,api,sigmap,sigmam,sigmam+sigmap,sigmap*sigmam/((sigmam+sigmap)*(sigmam+sigmap)),min,max,Volumen2);
      }
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
      if ((max-min)<0.001){
        max+=0.00001;
        min-=0.00001;
      }
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
  if  ((calcextrema)&&(minV.x!=maxV.x)&&(minV.y!=maxV.y)&&(minV.z!=maxV.z)){
    extern molekul mol;
    int *mimawa=(int*)malloc(sizeof(int)*orte.size());
    for (int i=0; i<orte.size();i++)mimawa[i]=0;
    for (int i=0; i<pgns.size();i++){
      double pgmin=1e99;
      double pgmax=-1e99;
      int imi=0,ima=0;
      for (int j=0; j<pgns.at(i).n; j++){
        int idx=pgns.at(i).ii[j];
        imi=(orte.at(idx).color<pgmin)?idx:imi;
        ima=(orte.at(idx).color>pgmax)?idx:ima;

        pgmin=(orte.at(idx).color<pgmin)?orte.at(idx).color:pgmin;
        pgmax=(orte.at(idx).color>pgmax)?orte.at(idx).color:pgmax;
      }
      for (int j=0; j<pgns.at(i).n; j++){
        int idx=pgns.at(i).ii[j];
        if (imi==ima) mimawa[idx]|=1;
        else {
          if (idx==imi) mimawa[idx]|=2;
          if (idx==ima) mimawa[idx]|=4;
          if ((idx!=ima)&&(idx!=imi)) mimawa[idx]|=1;
        }
      }
    }
    int minza=1;
    FILE *sfmm=fopen("sufaceminmax.log","wt");
    for (int i=0; i<orte.size();i++){

      INP newAtom;
      extern QList<INP> xdinp;
      newAtom.molindex=0;
      newAtom.part=0;
      newAtom.sg=0;
      newAtom.OrdZahl=-2;
      if (mimawa[i]==4){
        sprintf(newAtom.atomname,"%s%d","CP",minza);
        newAtom.cptype=6;
        newAtom.kart.x=orte.at(i).vertex.x;
        newAtom.kart.y=orte.at(i).vertex.y;
        newAtom.kart.z=orte.at(i).vertex.z;
        mol.kart2frac(newAtom.kart,newAtom.frac);
        fprintf(sfmm,"CP%-4d local max at %12.7f %12.7f %12.7f %g   %12.7f %12.7f %12.7f\n",minza,orte.at(i).vertex.x, orte.at(i).vertex.y, orte.at(i).vertex.z, orte.at(i).color,newAtom.frac.x,newAtom.frac.y,newAtom.frac.z);
        newAtom.peakHeight=orte.at(i).color;
        xdinp.append(newAtom);
        minza++;
      }
      else if (mimawa[i]==2){
        //   printf("local min at %f %f %f (%f)\n",orte.at(i).vertex.x, orte.at(i).vertex.y, orte.at(i).vertex.z, orte.at(i).color);
        sprintf(newAtom.atomname,"%s%d","CP",minza);
        newAtom.cptype=0;
        newAtom.kart.x=orte.at(i).vertex.x;
        newAtom.kart.y=orte.at(i).vertex.y;
        newAtom.kart.z=orte.at(i).vertex.z;
        mol.kart2frac(newAtom.kart,newAtom.frac);
        fprintf(sfmm,"CP%-4d local min at %12.7f %12.7f %12.7f %g   %12.7f %12.7f %12.7f\n",minza,orte.at(i).vertex.x, orte.at(i).vertex.y, orte.at(i).vertex.z, orte.at(i).color,newAtom.frac.x,newAtom.frac.y,newAtom.frac.z);
        newAtom.peakHeight=orte.at(i).color;
        xdinp.append(newAtom);
        minza++;
      }
      /*  orte[i].color=mimawa[i];
          min=0;
          max=8;*/
    }
    fclose(sfmm);
  }

  printf("MinMax %f %f %f %f\n",min,max,fixmax,fixmin);
  printf("%d orte.size %d  L%g\n",orte.size(),pgns.size(),L);
  Area=pgnArea();
  printf("Area %g Angstrom^2\n",Area); 

  if (polifile!=NULL) {
    fprintf(polifile," %8.0f\n",Area);
  }
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

double winkel(Vector3 a,Vector3 b){

  static double erg;
  erg=(a.x*b.x+a.y*b.y+a.z*b.z)/(sqrt(a.x*a.x+a.y*a.y+a.z*a.z)*sqrt(b.x*b.x+b.y*b.y+b.z*b.z));
  erg=acos(erg)/M_PI*180.0;
  return(erg);
}

bool mimacmp(const Imum &p1,const Imum &p2){
  return p1.v > p2.v;
}

void MolIso::exportSTL(){
  QString stln = QFileDialog::getSaveFileName ( this, 
      "Export Isosurfaces to stereolithography ASCII .stl file",
      QDir::homePath(), "ASCII STL (*.stl)");
  if (stln.isEmpty()) return;
  if (!stln.contains (QRegExp(".stl$",Qt::CaseInsensitive))) stln.append(".stl");
  double scale=10.0;

  Vector3 zentrum=Vector3(0.,0.,0.);
  for (int i=0; i<orte.size();i++) zentrum+=orte.at(i).vertex;
  zentrum*=1.0/orte.size()*scale;

  QFile of(stln);
  of.open(QIODevice::WriteOnly|QIODevice::Text);
  of.write(QString("solid MoleCoolQt surface\n").toLatin1());
  for (int i=0; i<pgns.size();i++){
    int n=pgns.at(i).n;
    Vector3 normale=Vector3(0.0,0.0,0.0);
    for (int j=0; j<n;j++) normale+=orte.at(pgns.at(i).ii[j]).normal;
    normale=Normalize(normale);
    double xxx=sqrt(Norm(normale));
    normale*=1.0/xxx;
    if (n==3){
      of.write(
          QString("  facet normal %1 %2 %3\n     outer loop\n      vertex %4 %5 %6\n      vertex %7 %8 %9\n      vertex %10 %11 %12\n    endloop\n  endfacet\n")
          .arg(normale.x).arg(normale.y).arg(normale.z)
          .arg(orte.at(pgns.at(i).ii[0]).vertex.x*scale).arg(orte.at(pgns.at(i).ii[0]).vertex.y*scale).arg(orte.at(pgns.at(i).ii[0]).vertex.z*scale)
          .arg(orte.at(pgns.at(i).ii[1]).vertex.x*scale).arg(orte.at(pgns.at(i).ii[1]).vertex.y*scale).arg(orte.at(pgns.at(i).ii[1]).vertex.z*scale)
          .arg(orte.at(pgns.at(i).ii[2]).vertex.x*scale).arg(orte.at(pgns.at(i).ii[2]).vertex.y*scale).arg(orte.at(pgns.at(i).ii[2]).vertex.z*scale)
          .toLatin1());

    }else{
      Vector3 mid=Vector3(0.0,0.0,0.0);
      for (int j=0; j<n;j++) mid+=orte.at(pgns.at(i).ii[j]).vertex;
      mid*=1.0/n;
      for (int k=0;k<n;k++){
        int kp=(k+1)%n;
        of.write(
            QString("  facet normal %1 %2 %3\n     outer loop\n      vertex %4 %5 %6\n      vertex %7 %8 %9\n      vertex %10 %11 %12\n    endloop\n  endfacet\n")
            .arg(normale.x).arg(normale.y).arg(normale.z)
            .arg(mid.x*scale).arg(mid.y*scale).arg(mid.z*scale)
            .arg(orte.at(pgns.at(i).ii[k]).vertex.x*scale).arg(orte.at(pgns.at(i).ii[k]).vertex.y*scale).arg(orte.at(pgns.at(i).ii[k]).vertex.z*scale)
            .arg(orte.at(pgns.at(i).ii[kp]).vertex.x*scale).arg(orte.at(pgns.at(i).ii[kp]).vertex.y*scale).arg(orte.at(pgns.at(i).ii[kp]).vertex.z*scale)
            .toLatin1());
      }
    }

  }
  of.write(QString("endsolid name\n").toLatin1());
  of.close();
  if  (1){//((calcextrema)){
    extern molekul mol;
    int *mimawa=(int*)malloc(sizeof(int)*orte.size());
    for (int i=0; i<orte.size();i++)mimawa[i]=0;
    for (int i=0; i<pgns.size();i++){
      double pgmin=1e99;
      double pgmax=-1e99;
      int imi=0,ima=0;
      for (int j=0; j<pgns.at(i).n; j++){
        int idx=pgns.at(i).ii[j];
        imi=(orte.at(idx).color<pgmin)?idx:imi;
        ima=(orte.at(idx).color>pgmax)?idx:ima;

        pgmin=(orte.at(idx).color<pgmin)?orte.at(idx).color:pgmin;
        pgmax=(orte.at(idx).color>pgmax)?orte.at(idx).color:pgmax;
      }
      for (int j=0; j<pgns.at(i).n; j++){
        int idx=pgns.at(i).ii[j];
        if (imi==ima) mimawa[idx]|=1;
        else {
          if (idx==imi) mimawa[idx]|=2;
          if (idx==ima) mimawa[idx]|=4;
          if ((idx!=ima)&&(idx!=imi)) mimawa[idx]|=1;
        }
      }
    }
    QString ncad=stln;
    ncad=ncad.replace(".stl",".scad");
    QFile scad(ncad);
    scad.open(QIODevice::WriteOnly|QIODevice::Text);
    scad.write(QString("/* Change SCALE if you wish to change 1cm=1Angstrom\nCylinders below are for 5mm diameter 2mm height magnets.\nCylinders are sorted by value.\nValue is given behind //.\n You might comment out the lower ones and those overlapping.\n */\nSCALE=%5;\nmag_diameter=5/cos(180/100);//corrected to get outer radius\nmag_height=2;\ntranslate([%2*SCALE,%3*SCALE,%4*SCALE])\ndifference(){\nscale([SCALE,SCALE,SCALE])\nimport(\"%1\");\n\n")
        .arg(stln)
        .arg(-zentrum.x)
        .arg(-zentrum.y)
        .arg(-zentrum.z)
        .arg(scale/10.0)

        .toLatin1());
    QList<Imum> extrem;
    for (int i=0; i<orte.size();i++){
      if ((mimawa[i]==4)||(mimawa[i]==2)){
        Imum I;
        I.i=i;
        I.v=orte.at(i).color;
        extrem.append(I);
      }
    }
    qSort(extrem.begin(),extrem.end(),mimacmp);

    for (int i=0; i<extrem.size();i++){
      int j=extrem.at(i).i;
      Vector3 ax = Normalize(Vector3(0,0,1)%orte.at(j).normal);
      //      double NAX=Norm(ax);
      //    ax*=1.0/NAX;
      double angle = winkel(Vector3(0,0,1),orte.at(j).normal);
      //      printf("norm %f %f %f %f ang %f\n",orte.at(j).normal.x,orte.at(j).normal.y,orte.at(j).normal.z,Norm(orte.at(j).normal),angle);
      QColor qc=farbverlauf((orte.at(j).color-min)/(max-min));
      scad.write(QString("color([%1,%2,%3,%4])\n")
          .arg(qc.redF())
          .arg(qc.greenF())
          .arg(qc.blueF())
          .arg(qc.alphaF())
          .toLatin1());

      scad.write(QString("translate([%1*SCALE, %2*SCALE, %3*SCALE])\nrotate(%4, [%5, %6, %7])\ncylinder($fn=100, d=mag_diameter, h=2.5*mag_height, center=true);\n// %8\n")
          .arg(orte.at(j).vertex.x*scale)
          .arg(orte.at(j).vertex.y*scale)
          .arg(orte.at(j).vertex.z*scale)
          .arg(angle)
          .arg(ax.x)
          .arg(ax.y)
          .arg(ax.z)
          .arg(orte.at(j).color)
          .toLatin1());
    }
    scad.write(QString("\\ *\\n}\n").toLatin1());
    scad.close();
  } 
}

void MolIso::exportObj() {
  QString wfobj = QFileDialog::getSaveFileName ( this, 
      "Export Isosurfaces to WaveFront .obj file",
      QDir::homePath(), "Wavefront (*.obj)");
  if (wfobj.isEmpty()) return;
  if (!wfobj.contains (QRegExp(".obj$"))) wfobj.append(".obj");
  QString wfmtl=wfobj;

  wfmtl=wfmtl.replace(QRegExp("obj$"),"mtl"); 
  QString wfmtlbase=wfmtl.section('/', -1);
  QString base= wfmtlbase;
  base=base.remove(QRegExp("mtl$"));
  QString path=wfmtl.section('/',0, -2);
  //  qDebug()<<wfmtlbase<<path;
  Vector3 centr=Vector3(0,0,0);
  for (int i=0; i<orte.size(); i++){
    centr+=orte.at(i).vertex;
  }
  centr*=1.0/orte.size();
  QFile of(wfobj);
  of.open(QIODevice::WriteOnly|QIODevice::Text);
  of.write(QString("#Created by MoleCoolQt\nmtllib %1\n#%2 vertices %3 faces\nusemtl grad\no Mesh\n").arg(wfmtlbase).arg(orte.size()).arg(pgns.size()).toLatin1());
  for (int i=0; i<orte.size(); i++){
    of.write(QString("v  %1 %2 %3\n")
        .arg(orte.at(i).vertex.x-centr.x,12,'f',6) 
        .arg(orte.at(i).vertex.y-centr.y,12,'f',6) 
        .arg(orte.at(i).vertex.z-centr.z,12,'f',6) 
        .toLatin1()
        ); 
  }
  for (int i=0; i<orte.size(); i++){
    of.write(QString("vn %1 %2 %3\n")
        .arg(orte.at(i).normal.x,12,'f',6) 
        .arg(orte.at(i).normal.y,12,'f',6) 
        .arg(orte.at(i).normal.z,12,'f',6) 
        .toLatin1()
        ); 
  }
  for (int i=0; i<orte.size(); i++){
    of.write(QString("vt %1 0.2\n")
        .arg((orte.at(i).color-min)/(max-min),12,'f',6)
        .toLatin1()
        ); 
  
  }
  for (int i=0; i<pgns.size();i++){
    for (int j=1; j<pgns.at(i).n-1;j++){
      int n= pgns.at(i).ii[0]+1;
      int m= pgns.at(i).ii[j]+1;
      int o= pgns.at(i).ii[j+1]+1;
      of.write(QString("f %1/%1/%1  %2/%2/%2  %3/%3/%3\n").arg(n).arg(m).arg(o).toLatin1());
    
    }
  }
  of.close();
  QFile mf(wfmtl);
  mf.open(QIODevice::WriteOnly|QIODevice::Text);
  mf.write(QString("newmtl grad\nKa 1 1 1\nKs 0.6 0.6 0.6\nmap_Kd %1png\n").arg(base).toLatin1());
  mf.close();
  QPixmap fmap = QPixmap(512,8);
  fmap.fill(QColor(128,128,128,0));
  QPainter *paint = new QPainter(&fmap);
  QPen pen;
  for (int i=0; i<512; i++){
    QColor c=farbverlauf(i/512.0);
    c.setAlpha(255);
    pen.setBrush(c);
    paint->setPen(pen);
    paint->drawLine(i,0,i,7);
  }
  paint->end();
  QString fn2=QString("%1/%2png").arg(path).arg(base);
  fmap.save(fn2);
}

int cutTriangle(GLfloat va, GLfloat vb, GLfloat vc){
  if ((va<=0.0f)&&(vb<=0.0f)&&(vc<=0.0f)) return 0;
  if ((va>0.0f)&&(vb>0.0f)&&(vc>0.0f)) return 0;

  if ((va<=0.0f)&&(vb>0.0f)&&(vc>0.0f)) return 1; //A
  if ((va>0.0f)&&(vb<=0.0f)&&(vc<=0.0f)) return 1;

  if ((va>0.0f)&&(vb<=0.0f)&&(vc>0.0f)) return 2;
  if ((va<=0.0f)&&(vb>0.0f)&&(vc<=0.0f)) return 2;

  if ((va>0.0f)&&(vb>0.0f)&&(vc<=0.0f)) return 3;
  if ((va<=0.0f)&&(vb<=0.0f)&&(vc>0.0f)) return 3;

  return 4;
}

void MolIso::findContour(QList<Vector3> &lines, GLfloat value){
  GLfloat Va,Vb,Vc,mix1,mix2;
  Vector3 sta,end;
  for (int i=0; i<Planpgns.size();i++){
    int n=Planpgns.at(i).n;
    if (n==3){
      Va=Planorte.at(Planpgns.at(i).ii[0]).color-value;
      Vb=Planorte.at(Planpgns.at(i).ii[1]).color-value;
      Vc=Planorte.at(Planpgns.at(i).ii[2]).color-value;
      switch (cutTriangle(Va, Vb, Vc)){
        case 1: 
          mix1=(Va/(Va-Vb));
          mix2=(Va/(Va-Vc));
          sta=(1.0f-mix1)*Planorte.at(Planpgns.at(i).ii[0]).vertex + (mix1)*Planorte.at(Planpgns.at(i).ii[1]).vertex;
          end=(1.0f-mix2)*Planorte.at(Planpgns.at(i).ii[0]).vertex + (mix2)*Planorte.at(Planpgns.at(i).ii[2]).vertex;
          lines.append(sta);
          lines.append(end);
          break;
        case 2: 
          mix1=(Va/(Va-Vb));
          mix2=(Vb/(Vb-Vc));
          sta=(1.0f-mix1)*Planorte.at(Planpgns.at(i).ii[0]).vertex + (mix1)*Planorte.at(Planpgns.at(i).ii[1]).vertex;
          end=(1.0f-mix2)*Planorte.at(Planpgns.at(i).ii[1]).vertex + (mix2)*Planorte.at(Planpgns.at(i).ii[2]).vertex;
          lines.append(sta);
          lines.append(end);
          break;
        case 3: 
          mix1=(Vc/(Vc-Vb));
          mix2=(Va/(Va-Vc));
          sta=(1.0f-mix1)*Planorte.at(Planpgns.at(i).ii[2]).vertex + (mix1)*Planorte.at(Planpgns.at(i).ii[1]).vertex;
          end=(1.0f-mix2)*Planorte.at(Planpgns.at(i).ii[0]).vertex + (mix2)*Planorte.at(Planpgns.at(i).ii[2]).vertex;
          lines.append(sta);
          lines.append(end);
          break;
      }
    }else{
      Vector3 mid=Vector3(0.0,0.0,0.0);
      for (int j=0; j<n;j++) mid+=Planorte.at(Planpgns.at(i).ii[j]).vertex;
      mid*=1.0/n;
      GLfloat mixColor=0.0f;
      for (int j=0; j<n;j++) mixColor+=Planorte.at(Planpgns.at(i).ii[j]).color;
      mixColor*=1.0f/n;
      Va=mixColor-value;
      for (int k=0;k<n;k++){
        int kp=(k+1)%n;
        Vb=Planorte.at(Planpgns.at(i).ii[k]).color-value;
        Vc=Planorte.at(Planpgns.at(i).ii[kp]).color-value;
        switch (cutTriangle(Va, Vb, Vc)){
          case 1: 
            mix1=(Va/(Va-Vb));
            mix2=(Va/(Va-Vc));
            sta=(1.0f-mix1)*mid + (mix1)*Planorte.at(Planpgns.at(i).ii[k]).vertex;
            end=(1.0f-mix2)*mid + (mix2)*Planorte.at(Planpgns.at(i).ii[kp]).vertex;
            lines.append(sta);
            lines.append(end);
            break;
          case 2: 
            mix1=(Va/(Va-Vb));
            mix1=(Va/(Va-Vb));
            mix2=(Vb/(Vb-Vc));
            sta=(1.0f-mix1)*mid + (mix1)*Planorte.at(Planpgns.at(i).ii[k]).vertex;
            end=(1.0f-mix2)*Planorte.at(Planpgns.at(i).ii[k]).vertex + (mix2)*Planorte.at(Planpgns.at(i).ii[kp]).vertex;
            lines.append(sta);
            lines.append(end);
            break;
          case 3: 
            mix1=(Vc/(Vc-Vb));
            mix2=(Va/(Va-Vc));
            sta=(1.0f-mix1)*Planorte.at(Planpgns.at(i).ii[kp]).vertex + (mix1)*Planorte.at(Planpgns.at(i).ii[k]).vertex;
            end=(1.0f-mix2)*mid + (mix2)*Planorte.at(Planpgns.at(i).ii[kp]).vertex;
            lines.append(sta);
            lines.append(end);
            break;
          case 4:
            printf("@@~~~~ %g %g %g\n",Va,Vb,Vc);
        }
      }
    }
  }
}

double MolIso::pgnArea(){
  double area=0.0;
  for (int i=0;i<pgns.size();i++){
    switch (pgns.at(i).n){
      case 3:
        area+= 0.5*sqrt(Norm((
                orte.at(pgns.at(i).ii[1]).vertex-
                orte.at(pgns.at(i).ii[0]).vertex)%(
                orte.at(pgns.at(i).ii[2]).vertex-
                orte.at(pgns.at(i).ii[0]).vertex)));
        break;
      case 4:
        area+=sqrt(Norm((orte.at(pgns.at(i).ii[1]).vertex-orte.at(pgns.at(i).ii[0]).vertex)%(orte.at(pgns.at(i).ii[3]).vertex-orte.at(pgns.at(i).ii[0]).vertex)));
        break;
      case 5:
      case 6:
      case 7:
      case  8:
      case  9:
      case  10:
      case  11:
      case   12:{
                  Vector3 mO=orte.at(pgns.at(i).ii[0]).vertex;
                  double a=0;
                  for (int k=1;k <pgns.at(i).n; k++) {
                    mO+=orte.at(pgns.at(i).ii[k]).vertex;
                  }
                  mO*=(GLfloat)1.0/pgns.at(i).n;
                  for (int k=0;k<=pgns.at(i).n;k++)
                    a+=sqrt(Norm((orte.at(pgns.at(i).ii[k%pgns.at(i).n]).vertex-mO)%(orte.at(pgns.at(i).ii[(k+1)%pgns.at(i).n]).vertex-mO)));
                  area+=0.5*a;

                  break;
                }
      default : printf("wrong polygonshape! %d\n",pgns.at(i).n);glEnd();break;


    }
  }
  return area;
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

void MolIso::readBMBinaryHeader(QString fname){
  QFile bmb(fname);
  bmb.open(QIODevice::ReadOnly);
  // OPEN (81, ERR=900, FILE=rhofile, FORM='UNFORMATTED', STATUS='OLD')
  //READ (81, ERR=17) SDim, RDim, (nNVox(Cnt1), Cnt1=1,ndim), (Cell(Cnt1), Cnt1=1,CellCon)!everything must be read in one read
  //!                                                                   command so we have to try unil we get the right answer
  //write (*,*) SDim, RDim, nNVox(1:SDim),Cell
  //int maxdim=8;
  int ndim=7;
  int sdim;
  //int cellcon=7;
  //int nvox[8];
  int xxx;
  double cell[7];
  bmb.read((char*)&xxx,sizeof(int));
  if (xxx!=76) {qDebug()<<"Unsuported Format "<<xxx; exit(0);}
  //qDebug()<<xxx<<5*sizeof(int)+7*sizeof(double);
  bmb.read((char*)&sdim,sizeof(int));

  if (sdim!=3) {qDebug()<<"Unsuported Format (only 3D!)"<<sdim; exit(0);}
  //qDebug()<<sdim;
  bmb.read((char*)&ndim,sizeof(int));
  bmb.read((char*)&breite,sizeof(int));
  bmb.read((char*)&hoehe,sizeof(int));
  bmb.read((char*)&tiefe,sizeof(int));

  for (int i=0; i<7; i++)
    bmb.read((char*)&cell[i],sizeof(double));

  extern molekul mol;
  mol.zelle.a=cell[0];
  mol.zelle.b=cell[1];
  mol.zelle.c=cell[2];
  mol.zelle.al=cell[3];
  mol.zelle.be=cell[4];
  mol.zelle.ga=cell[5];
  mol.setup_zelle();
  if( fabs(cell[6]-mol.zelle.V)>0.001) {

    qDebug()<< "File reports Volume "<<cell[6]<<"A^3 which is differnt from expected value "<< mol.zelle.V;
  }
  bh=breite*hoehe;
  capVx=mol.zelle.V/(bh*tiefe);
  deV=capVx;
  V3 xd,yd,zd,xdk,ydk,zdk;
  /* qDebug()<<ndim<<breite<<hoehe<<tiefe
     <<cell[0] 
     <<cell[1] 
     <<cell[2] 
     <<cell[3] 
     <<cell[4] 
     <<cell[5] 
     <<cell[6] 

     <<capVx;*/
  double
    jdx=1.0/(breite),
    jdy=1.0/(hoehe),
    jdz=1.0/(tiefe);

  xd=V3(jdx ,0,0);
  yd=V3(0, jdy,0);
  zd=V3(0,0, jdz);
  mol.frac2kart(xd,xdk);
  mol.frac2kart(yd,ydk);
  mol.frac2kart(zd,zdk);
  x_dim=Vector3(xdk.x,xdk.y,xdk.z);
  y_dim=Vector3(ydk.x,ydk.y,ydk.z);
  z_dim=Vector3(zdk.x,zdk.y,zdk.z);

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
  int idim=0,ups=0,aua=-1;
  for (int i=0; i<6; i++) {
    idim+=ups=t.iorien[i];
    printf("%d\n",ups);
    if (aua>ups) {
      qDebug()<<"m81 file axis order must be 1 2 3, x y z!";
      exit(0);  
    }
  }
  if ((idim!=21)&&(idim!=6)) {
    qDebug()<<".m81 file suport only for 3 Dimensions, sorry"<<idim;
    exit(0);
  }

  //qDebug()<<t.nx[0]<<t.nx[1]<<t.nx[2]<<t.nxny;
  jh.close();
  breite=(t.iorien[0]==1)?t.nx[0]:(t.iorien[1])==1?t.nx[1]:t.nx[2];
  hoehe =(t.iorien[0]==2)?t.nx[0]:(t.iorien[1])==2?t.nx[1]:t.nx[2];
  tiefe =(t.iorien[0]==3)?t.nx[0]:(t.iorien[1])==3?t.nx[1]:t.nx[2];
  bh =t.nxny;
  printf("%7d %7d %7d %7d %7d %7d %7d %7d \n",    t.nx[0],t.nx[1],t.nx[2],t.nx[3],t.nx[4],t.nx[5],t.nxny,t.nmap);
  printf("%6.4f %6.4f %6.4f %6.4f %6.4f %6.4f \n",t.dx[0],t.dx[1],t.dx[2],t.dx[3],t.dx[4],t.dx[5]);
  printf("%7d %7d %7d %7d %7d %7d mapa%7d nsubs%7d \n",    t.iorien[0],t.iorien[1],t.iorien[2],t.iorien[3],t.iorien[4],t.iorien[5],t.mapa,t.nsubs);
  printf("%6.4f %6.4f %6.4f %6.4f %6.4f %6.4f \n",t.xymin[0],t.xymin[1],t.xymin[2],t.xymin[3],t.xymin[4],t.xymin[5]);
  printf("%6.4f %6.4f %6.4f %6.4f %6.4f %6.4f \n",t.xymin[6],t.xymin[7],t.xymin[8],t.xymin[9],t.xymin[10],t.xymin[11]);
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
  double 
    jdx=t.dx[0],
    jdy=t.dx[1], 
    jdz=t.dx[2];
    xd=V3(jdx ,0,0);
    yd=V3(0, jdy,0);
    zd=V3(0,0, jdz);
    int ic1=t.iorien[0],ic2=t.iorien[1];
    printf("%d %d %d\n",ic1,ic2,6-ic1-ic2);
    mol.frac2kart((ic1==1)?xd:(ic2==1)?yd:zd,xdk);
    mol.frac2kart((ic1==2)?xd:(ic2==2)?yd:zd,ydk);
    mol.frac2kart((ic1==3)?xd:(ic2==3)?yd:zd,zdk);
    x_dim=Vector3(xdk.x,xdk.y,xdk.z);
    y_dim=Vector3(ydk.x,ydk.y,ydk.z);
    z_dim=Vector3(zdk.x,zdk.y,zdk.z);
    printf("first voxel vector  %9.6f %9.6f %9.6f\n",x_dim.x,x_dim.y,x_dim.z);
    printf("second voxel vector %9.6f %9.6f %9.6f\n",y_dim.x,y_dim.y,y_dim.z);
    printf("third voxel vector  %9.6f %9.6f %9.6f\n\n",z_dim.x,z_dim.y,z_dim.z);
    printf("%9.6f %9.6f %9.6f\n",x_dim.x*breite,x_dim.y*breite,x_dim.z*breite);
    printf("%9.6f %9.6f %9.6f\n",y_dim.x*hoehe,y_dim.y*hoehe,y_dim.z*hoehe);
    printf("%9.6f %9.6f %9.6f\n",z_dim.x*tiefe,z_dim.y*tiefe,z_dim.z*tiefe);
    printf("%9.6f %9.6f %9.6f %9.6f %9.6f %9.6f %9.6f\n", mol.zelle.a ,mol.zelle.b , mol.zelle.c, mol.zelle.al, mol.zelle.be, mol.zelle.ga, mol.zelle.V);
    QString M40Name=fname;
    deV=mol.zelle.V/(bh*tiefe);
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

void MolIso::readGVDHeader(QString fname){
  extern molekul mol;
  QFile gh(fname);
  gh.open(QIODevice::ReadOnly);
  QString all =gh.readAll();
  QStringList lines = all.split(QRegExp("[\n\r]+"));
  all.clear();

  QStringList tok = lines.at(1).split(" ",QString::SkipEmptyParts);
  mol.zelle.a  = tok.at(0).toDouble();
  mol.zelle.b  = tok.at(1).toDouble();
  mol.zelle.c  = tok.at(2).toDouble();
  mol.zelle.al = tok.at(3).toDouble();
  mol.zelle.be = tok.at(4).toDouble();
  mol.zelle.ga = tok.at(5).toDouble();
  mol.setup_zelle();

  tok = lines.at(2).split(" ",QString::SkipEmptyParts);
  breite=tok.at(0).toInt();
  hoehe=tok.at(1).toInt();
  tiefe=tok.at(2).toInt();
  bh = hoehe*breite;

  bh=breite*hoehe;
  double
    jdx=1.0/(breite-1),
    jdy=1.0/(hoehe-1),
    jdz=1.0/(tiefe-1);
  V3 xd,yd,zd,xdk,ydk,zdk;
  xd=V3(jdx ,0,0);
  yd=V3(0, jdy,0);
  zd=V3(0,0, jdz);
  mol.frac2kart(xd,xdk);
  mol.frac2kart(yd,ydk);
  mol.frac2kart(zd,zdk);
  x_dim=Vector3(xdk.x,xdk.y,xdk.z);
  y_dim=Vector3(ydk.x,ydk.y,ydk.z);
  z_dim=Vector3(zdk.x,zdk.y,zdk.z);
  GHName=fname;
  //qDebug()<<breite<<hoehe<<tiefe<<xdk.x<<xdk.y<<xdk.z<<ydk.x<<ydk.y<<ydk.z<<zdk.x<<zdk.y<<zdk.z;
  gh.close();
  deV=mol.zelle.V/(bh*tiefe);
}

void MolIso::readXDGridHeader(QString fname,int &fileType){
  if (fname.endsWith(".m81",Qt::CaseInsensitive)) {
    readJanaHeader(fname);
    GHName=fname;
    fileType=81;
    return;
  }
  if (fname.contains(QRegExp("\\..ed$"))){
    //    qDebug()<<fname<<__LINE__;
    readGVDHeader(fname);
    GHName=fname;
    fileType=321;
    return;
  }
  if (fname.endsWith(".raw",Qt::CaseInsensitive)) {
    readBMBinaryHeader(fname);
    GHName=fname;
    fileType=7202;//my room number in BT
    return;
  }
  QFile gh(fname);

  printf("filetype of:%s\n",fname.toStdString().c_str());
  gh.open(QIODevice::ReadOnly);
  QString all =gh.readAll();
  QStringList lines = all.split(QRegExp("[\n\r]+"));
  all.clear();
  if ((lines.size())&&(lines.at(0).contains("2DGRDFIL"))) {
    qDebug()<<"Currently only 3DGRDFILE are suported, sorry!";
    fileType=-666;
    return;
  }
  if ((lines.size())&&(lines.at(0).contains("3DGRDFIL"))) {
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
    double vol=tok.at(0).toDouble()*tok.at(1).toDouble()*tok.at(2).toDouble();
    deV=vol/(bh*tiefe);
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
        } else	{
          newAtom.OrdZahl=-1;
        }
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
    double bohr=0.52917720859 ;//0.5291775108;
    if (lines.size()>6) {
      QStringList tok;
      tok=lines.at(2).split(QRegExp("\\s+"),QString::SkipEmptyParts);
      if (tok.size()>=4){	
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
        deV=(x_dim%y_dim)*z_dim;
        bh = hoehe*breite;
        deV/=bh*hoehe;
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
            -1*(data[(i-1)+(j-1)*breite+(k  )*bh]-hier)+//9 
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
            -1*(data[(i-1)+(j-1)*breite+(k  )*bh]-hier)+//9  
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
  GLfloat vo, vx=0, vy=0, vz=0,sig,mix;
  //Vector3 movo(0.5,0.5,0.5);
  Ort o;
  sig = (iso_level>0)?-1:1;
  vo = data.at(ix+iy*breite+iz*bh)  - iso_level;

  if( ix != breite-1 ) vx = data.at((1+ix)+iy*breite+iz*bh) - iso_level;
  if( iy != hoehe-1 ) vy = data.at(ix+(iy+1)*breite+iz*bh) - iso_level;
  if( iz != tiefe-1 ) vz = data.at(ix+iy*breite+(iz+1)*bh) - iso_level;
  if( ix != breite-1 && Intersect(vo,vx) ){
    mix=(vo/(vo-vx));
    o.vertex= x_dim*(mix+ix) + y_dim*iy + z_dim*iz;
    o.vertex+=test3;
    o.normal= Normalize(sig*( grad[ix+iy*breite+iz*bh]*(1.0-mix) +grad[(1+ix)+iy*breite+iz*bh]*(1.0-mix)));
    o.color= mdata[ix+iy*breite+iz*bh]*(1.0-mix)+mix*mdata[(1+ix)+iy*breite+iz*bh];
    orte.append(o);
    nodex[ix+iy*breite+iz*bh].index=orte.size()-1;
    nodex[ix+iy*breite+iz*bh].flag=1; 
  }else{
    nodex[ix+iy*breite+iz*bh].flag   = 0;
  }
  if( iy != hoehe-1 && Intersect(vo,vy) ){
    mix=(vo/(vo-vy));
    o.vertex = x_dim*ix + y_dim*(mix+iy) + z_dim*iz;
    o.vertex+=test3;
    o.normal=Normalize(sig*( grad[ix+iy*breite+iz*bh]*(1.0-mix) +grad[ix+(iy+1)*breite+iz*bh]*(1.0-mix)));
    o.color=mdata[ix+iy*breite+iz*bh]*(1.0-mix)+ mix*mdata[ix+(iy+1)*breite+iz*bh];
    orte.append(o);
    nodey[ix+iy*breite+iz*bh].index=orte.size()-1;
    nodey[ix+iy*breite+iz*bh].flag=1;
  }else{
    nodey[ix+iy*breite+iz*bh].flag=0;
  }
  if( iz != tiefe-1 && Intersect(vo,vz) ){
    mix=(vo/(vo-vz));
    o.vertex = x_dim*ix + y_dim*iy + (mix+iz)*z_dim;
    o.vertex+=test3;
    o.normal = Normalize(sig*( grad[ix+iy*breite+iz*bh]*(1.0-mix) +grad[ix+iy*breite+(iz+1)*bh]*(1.0-mix)));
    o.color = mdata[ix+iy*breite+iz*bh]*(1.0-mix)+mix*mdata[ix+iy*breite+(iz+1)*bh];
    orte.append(o);
    nodez[ix+iy*breite+iz*bh].index = orte.size()-1;
    nodez[ix+iy*breite+iz*bh].flag=1;
  }else{
    nodez[ix+iy*breite+iz*bh].flag  = 0;
  }
}


void MolIso::CalcVertexes( void ) {
  /*  printf("ooo %g %g %g\n",orig.x,orig.y,orig.z);
      printf("x_dim %g %g %g\n", x_dim.x,  x_dim.y,  x_dim.z);
      printf("y_dim %g %g %g\n", y_dim.x,  y_dim.y,  y_dim.z);
      printf("z_dim %g %g %g\n", z_dim.x,  z_dim.y,  z_dim.z);*/
  test3= ((breite-1)/-2.0) *  x_dim + ((hoehe-1)/-2.0) * y_dim + ((tiefe-1)/-2.0) * z_dim + orig;
  Volumen2=0.0;
  //  Vector3 test4 = ((breite-1)) *  x_dim + ((hoehe-1)) * y_dim + ((tiefe-1)) * z_dim + orig;

  if (cubeiso) test3 =orig;
  //  printf("boq %g %g %g\n",test3.x,test3.y,test3.z);
  //  printf("||| %g %g %g\n",test4.x,test4.y,test4.z);
  for( int ix=0; ix<breite; ix++ ){
    for( int iy=0; iy<hoehe; iy++ ){
      for( int iz=0; iz<tiefe; iz++ ){
        CalcVertex(ix,iy,iz);
        if ((ix<breite-1)&&(iy<hoehe-1)&&(iz<tiefe-1)){
          double mul;
          if ((data[(ix)   + (iy*breite)   + (iz*bh) ]   > iso_level)&&
              (data[(ix)   + (iy*breite)   + (iz*bh+1) ] > iso_level)&&
              (data[(ix)   + (iy*breite+1) + (iz*bh) ]   > iso_level)&&
              (data[(ix)   + (iy*breite+1) + (iz*bh+1) ] > iso_level)&&
              (data[(ix+1) + (iy*breite)   + (iz*bh) ]   > iso_level)&&
              (data[(ix+1) + (iy*breite)   + (iz*bh+1) ] > iso_level)&&
              (data[(ix+1) + (iy*breite+1) + (iz*bh) ]   > iso_level)&&
              (data[(ix+1) + (iy*breite+1) + (iz*bh+1) ] > iso_level)){
            untersumme+=deV;	    
          }

          if ((data[(ix)   + (iy*breite)   + (iz*bh) ]   > iso_level)||
              (data[(ix)   + (iy*breite)   + (iz*bh+1) ] > iso_level)||
              (data[(ix)   + (iy*breite+1) + (iz*bh) ]   > iso_level)||
              (data[(ix)   + (iy*breite+1) + (iz*bh+1) ] > iso_level)||
              (data[(ix+1) + (iy*breite)   + (iz*bh) ]   > iso_level)||
              (data[(ix+1) + (iy*breite)   + (iz*bh+1) ] > iso_level)||
              (data[(ix+1) + (iy*breite+1) + (iz*bh) ]   > iso_level)||
              (data[(ix+1) + (iy*breite+1) + (iz*bh+1) ] > iso_level)){
            obersumme+=deV;
            mul=((data[(ix)   + (iy*breite)   + (iz*bh) ]   > iso_level)?0.125:0)+
              ((data[(ix)   + (iy*breite)   + (iz*bh+1) ] > iso_level)?0.125:0)+
              ((data[(ix)   + (iy*breite+1) + (iz*bh) ]   > iso_level)?0.125:0)+
              ((data[(ix)   + (iy*breite+1) + (iz*bh+1) ] > iso_level)?0.125:0)+
              ((data[(ix+1) + (iy*breite)   + (iz*bh) ]   > iso_level)?0.125:0)+
              ((data[(ix+1) + (iy*breite)   + (iz*bh+1) ] > iso_level)?0.125:0)+
              ((data[(ix+1) + (iy*breite+1) + (iz*bh) ]   > iso_level)?0.125:0)+
              ((data[(ix+1) + (iy*breite+1) + (iz*bh+1) ] > iso_level)?0.125:0);
            Volumen2+=deV*mul;
          }
        }
      }
    }
  }
  double Volumen=(obersumme+untersumme)/2.0;
  double fehler=(obersumme-untersumme)/4.0;   
  printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nIntegration of the surface volume[A^3]:\n %f < %5.0f(%.0f) < %f\n%12.2f\n",untersumme,Volumen,fehler,obersumme,Volumen2);
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
  thisIsPDF=false;

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
      numbers.clear();
      b=37*p/pmax;
      if (b!=altb){
        balken->setValue(b);
        altb=b;
      }
    }
    isoF.close(); 
    {
      double minv=1e37,maxv=-1e38,summ=0.0;
      for(int ii=0; ii<data.size();ii++) {
        summ+=data.at(ii);
        minv=qMin(data.at(ii),minv);
        maxv=qMax(data.at(ii),maxv);
      }
      printf("minmax grd: %g %g %g\n",minv,maxv,summ);
    }
  }else if ((fileType==321)||(fileType==125)){// ein cube file hoffentlich oder ein ?ed file 
    isoF.open(QIODevice::ReadOnly);
    if (fileType==321) for (int i=0; i<3 ;i++) isoF.readLine();
    else for (int i=0; i<(atomanzahl+6);i++) isoF.readLine();
    int pmax=breite*hoehe*tiefe,altb=0,b=0;
    int p=pmax-1;
    QVector<double> rewerte(pmax);
    while (!isoF.atEnd () && (p>=0)) {
      QStringList numbers = QString(isoF.readLine()).split(QRegExp("\\s+"),QString::SkipEmptyParts);
      if ((!numbers.isEmpty())&&(!numbers.at(0).contains('.'))) continue;
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
    {
      extern molekul mol;
      double minv=1e37,maxv=-1e38,meanv=0.0;
      for(int ii=0; ii<data.size();ii++) {
        meanv+=data.at(ii);
        minv=qMin(data.at(ii),minv);
        maxv=qMax(data.at(ii),maxv);
      }
      printf("minmax m81: %g %g mean: %g %g\n",minv,maxv,meanv/data.size(),meanv/data.size()*mol.zelle.V);
    }
    free(floatdat);
    cubeiso=true;
    isoF.close(); 
  }else if(fileType==7202){
    isoF.open(QIODevice::ReadOnly);
    int head;

    cubeiso=true;
    char *headdummy;
    isoF.read((char*) &head,sizeof(int));
    headdummy=(char*) malloc(head);
    isoF.read((char*) headdummy, head);
    isoF.read((char*) &head,sizeof(int));
    isoF.read((char*) &head,sizeof(int));
    double *doubdat=(double*)malloc(head);
    isoF.read((char*) doubdat,head);
    isoF.close();
    head/=sizeof(double);
    extern molekul mol;
    double rmin=9.0e99,rmax=-9.0e99,meanv=0.0;
    //qDebug()<<head<<bh*tiefe;
    for (int i = 0; i<head; i++){
      data.append(doubdat[i]/capVx);
      meanv+=data.at(i);
      rmin=fmin(data.at(i),rmin);
      rmax=fmax(data.at(i),rmax);

    }
    printf("minmax raw: %g %g mean: %g %g\n",rmin,rmax,meanv/data.size(),meanv/data.size()*mol.zelle.V);
    free(headdummy);
    free(doubdat);
  }
  if (mapFileName==isoFileName) {
    mdata=data;
  }else {
    printf("mapFileName\n");
    if (fileType==0){//if (mapFileName.contains("grd")) {
      mapF.open(QIODevice::ReadOnly);
      while (!QString(mapF.readLine()).contains("Values")) {;}
      int p=0,pmax=breite*hoehe*tiefe,altb=0,b=0;
      while (!mapF.atEnd () && (p<pmax)) {
        QStringList numbers = QString(mapF.readLine()).split(" ",QString::SkipEmptyParts);
        for (int i=0; i<numbers.size();i++) mdata.append(numbers.at(i).toDouble());
        p+=numbers.size();
        numbers.clear();
        b=38+37*p/pmax;
        if (b!=altb){
          balken->setValue(b);
          altb=b;
        }
      }
      mapF.close();                                                                              
    }else if ((fileType==321)||(fileType==125)){//{ // ein cube file hoffentlich
      mapF.open(QIODevice::ReadOnly);
      for (int i=0; i<(atomanzahl+6);i++) mapF.readLine();
      int pmax=breite*hoehe*tiefe,altb=0,b=0;
      int p=pmax-1;
      QVector<double> rewerte(pmax);
      while (!mapF.atEnd () && (p>=0)) {
        QStringList numbers = QString(mapF.readLine()).split(QRegExp("\\s+"),QString::SkipEmptyParts);
        if ((!numbers.isEmpty())&&(!numbers.at(0).contains('.'))) continue;
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
      {
        extern molekul mol;
        double minv=1e37,maxv=-1e38,meanv=0.0;
        for(int ii=0; ii<mdata.size();ii++) {
          meanv+=mdata.at(ii);
          minv=qMin(mdata.at(ii),minv);
          maxv=qMax(mdata.at(ii),maxv);
        }
        printf("minmax m81: %g %g mean: %g %g %g \n",minv,maxv,meanv/data.size(),meanv/data.size()*mol.zelle.V,meanv/data.size()*mol.zelle.V*M_PI);
      }
      cubeiso=true;
      mapF.close(); 
      free(floatdat);
    }if(fileType==7202){
      mapF.open(QIODevice::ReadOnly);
      int head;

      //cubeiso=true;
      char *headdummy;
      mapF.read((char*) &head,sizeof(int));
      headdummy=(char*) malloc(head);
      mapF.read((char*) headdummy, head);
      mapF.read((char*) &head,sizeof(int));
      mapF.read((char*) &head,sizeof(int));
      double *doubdat=(double*)malloc(head);
      mapF.read((char*) doubdat,head);
      mapF.close();
      head/=sizeof(double);

      //qDebug()<<head<<bh*tiefe;
      extern molekul mol;
      double rmin=9.0e99,rmax=-9.0e99,meanv=0.0;
      for (int i = 0; i<head; i++){
        mdata.append(doubdat[i]/capVx);
        meanv+=mdata.at(i);
        rmin=fmin(mdata.at(i),rmin);
        rmax=fmax(mdata.at(i),rmax);
      }
      printf("minmax raw: %g %g mean: %g %g\n",rmin,rmax,meanv/mdata.size(),meanv/mdata.size()*mol.zelle.V);
      free(headdummy);
      free(doubdat);
    }
    }
    if (data.size()!=mdata.size()) {
      qDebug()<<"Map-Grid and Iso-Grid must have the same size!";
      qApp->quit();
    }
    if (( grad =(Vector3*)malloc(sizeof(Vector3)*bh*tiefe))==NULL) {
      qDebug()<<"Less Memory(grad)";
      exit(1);
    }
    if (( nodex =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) {
      qDebug()<<"Less Memory(X) ";
      exit(1);  
    } 
    if (( nodey =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) { 
      qDebug()<<"Less Memory(Y)!!";
      exit(1); 
    }
    if (( nodez =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) {
      qDebug()<<"Less Memory(Z)!!";
      exit(1); 
    }
  simpelGrad();	
  balken->setValue(89);
  for (int k=0; k<isoValues.size(); k++){
    iso_level=isoValues.at(k);
    CalcVertexes();
    CalcNormals();
    printf("iso_level: %g\n",iso_level);
    //      std::cout<<iso_level;
    /*      
            tf->open(QIODevice::WriteOnly|QIODevice::Text);
            tf->write(QString("%1\n").arg(orte.size()).toLatin1());
            for (int i=0;i<orte.size();i++){
            orte[i].normal=Normalize(orte.at(i).normal);
            tf->write(QString("%1  %2 %3 %4   %5 %6 %7  %8\n")
            .arg(i,-6)
            .arg(orte.at(i).vertex.x,9,'f',6)
            .arg(orte.at(i).vertex.y,9,'f',6)
            .arg(orte.at(i).vertex.z,9,'f',6)
            .arg(orte.at(i).normal.x,9,'f',6)
            .arg(orte.at(i).normal.y,9,'f',6)
            .arg(orte.at(i).normal.z,9,'f',6)
            .arg(orte.at(i).color,12,'f',7).toLatin1());
            }*/
    for( int ix=0; ix<breite-1; ix++ )
      for( int iy=0; iy<hoehe-1; iy++ )
        for( int iz=0; iz<tiefe-1; iz++ )
          MakeElement(ix,iy,iz,breite,bh);

    PXsort();
    balken->setValue(90+(10/isoValues.size())*(k+1));
    /*      QString Line="";
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
            tf->close(); //  */
  }
  /////////////////////////////////////
  tf->open(QIODevice::WriteOnly|QIODevice::Text);
  tf->write(QString("%1\n").arg(orte.size()).toLatin1());
  for (int i=0;i<orte.size();i++){
    Vector3 v= orte[i].normal;
    double nn=1.0/sqrt((double)v.x*v.x + v.y*v.y + v.z*v.z);
    orte[i].normal=nn*orte.at(i).normal;
    tf->write(QString("%1  %2 %3 %4   %5 %6 %7  %8 \n")
        .arg(i,-6)
        .arg(orte.at(i).vertex.x,9,'f',6)
        .arg(orte.at(i).vertex.y,9,'f',6)
        .arg(orte.at(i).vertex.z,9,'f',6)
        .arg(orte.at(i).normal.x,9,'f',6)
        .arg(orte.at(i).normal.y,9,'f',6)
        .arg(orte.at(i).normal.z,9,'f',6)
        //    .arg(Norm(orte.at(i).normal),12,'f',7)
        .arg(orte.at(i).color,12,'f',7)
        .toLatin1());
  }
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
  ///////////////////////////////
  balken->setValue(100);
  free(grad);grad=NULL;
  free(nodex);nodex=NULL;
  free(nodey);nodey=NULL;
  free(nodez);nodez=NULL;
  mdata.clear();
  data.clear();
  pgns.clear();
  orte.clear();
}


double MolIso::aborp(double max,double v){
  /*
   * Final set of parameters            Asymptotic Standard Error
   * =======================            ==========================
   *
   * a0              = 52.8343          +/- 0.5022       (0.9505%)
   * a1              = -4.18442         +/- 0.0556       (1.329%)
   * a2              = 46.6335          +/- 0.5221       (1.12%)
   * a3              = 2.39034          +/- 0.01631      (0.6821%)
   * a4              = -6.14029         +/- 0.1194       (1.945%)
   *
   *
   * correlation matrix of the fit parameters:
   *
   *                a0     a1     a2     a3     a4     
   *                a0              1.000 
   *                a1              0.991  1.000 
   *                a2             -0.999 -0.996  1.000 
   *                a3             -0.889 -0.858  0.880  1.000 
   *                a4             -0.805 -0.784  0.798  0.971  1.000 
   *                gnuplot> plot '50',f(x)
   *                gnuplot>          warning: Warning - difficulty fitting plot titles into key
   *
   *                gnuplot> f(x)=a0*exp(a1*x)+a2*exp(-a3*x**2)+a4*x
   *
   * */
  double 
    a0=52.8343,
    a1=-4.18442,
    a2=46.6335,
    a3= 2.39034,
    a4=-6.14029,
    f,x;
  int sig=(v<0.0)?-1:1;
  x=fabs(v/max);
  f=a0*exp(a1*x)+a2*exp(-a3*x*x)+a4*x;
  return f*sig;
}
void MolIso::createSurface(QString &storeFaceName, double proba,double iso99,bool mapping,bool minus99,double maxharm){
  thisIsPDF=true;
  if (mapping){
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
  }else{
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
    mdata=data;
  }
  QFile *tf = new QFile(storeFaceName);
  bh=breite*hoehe;
  extern QProgressBar *balken;
  balken->setMinimum(0);
  balken->setMaximum(100);
  balken->show();
  balken->setValue(0);
  if (( grad =(Vector3*)malloc(sizeof(Vector3)*bh*tiefe))==NULL) {
    qDebug()<<"Less Memory(grad)";
    exit(1);
  }

  if (( nodex =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) {
    qDebug()<<"Less Memory(X) ";
    exit(1);  
  } 
  if (( nodey =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) { 
    qDebug()<<"Less Memory(Y)!!";
    exit(1); 
  }
  if (( nodez =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) {
    qDebug()<<"Less Memory(Z)!!";
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
    if (minus99){
      iso_level=-iso99;
      printf("99%% %g %g\n",iso99,iso_level);
      CalcVertexes();
      CalcNormals();
    }
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
          .arg((mapping)?aborp(maxharm,orte.at(i).color):
            ((orte.at(i).color>0)?proba:-99),12,'f',7).toLatin1());
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
  free(grad);grad=NULL;
  free(nodex);nodex=NULL;
  free(nodey);nodey=NULL;
  free(nodez);nodez=NULL;
  mdata.clear();
  data.clear();
  pgns.clear();
  orte.clear();
}

void MolIso::createSurface(QString &storeFaceName, bool mapping){//4 hirshfeld surfaces
  thisIsPDF=false;
  if (mapping){
    Farben=6;    
    farbe[0][0]=0.6;    
    farbe[0][1]=0.6;    
    farbe[0][2]=0.6;    
    farbe[0][3]=0.25;
    farbe[1][0]=0.75;   
    farbe[1][1]=0.2;   
    farbe[1][2]=0.0;  
    farbe[1][3]=0.25;
    farbe[2][0]=0.8;  
    farbe[2][1]=0.7;    
    farbe[2][2]=0;    
    farbe[2][3]=0.5;
    farbe[3][0]=0;    
    farbe[3][1]=0.5;    
    farbe[3][2]=0;      
    farbe[3][3]=0.5;
    farbe[4][0]=0;    
    farbe[4][1]=0;    
    farbe[4][2]=0.9;    
    farbe[4][3]=0.5;    
    farbe[5][0]=0.6;    
    farbe[5][1]=0;    
    farbe[5][2]=0.9;    
    farbe[5][3]=0.75;    
    farbe[6][0]=0.6;    
    farbe[6][1]=0;    
    farbe[6][2]=0;    
    farbe[6][3]=0.75;  
  }else{
    Farben=2;    

    farbe[0][0]=0.3;    
    farbe[0][1]=0.7;    
    farbe[0][2]=0.3;    
    farbe[0][3]=0.6;

    farbe[1][0]=0.3;    
    farbe[1][1]=0.7;    
    farbe[1][2]=0.3;    
    farbe[1][3]=0.6;

    mdata=data;
  }
  QFile *tf = new QFile(storeFaceName);
  bh=breite*hoehe;
  extern QProgressBar *balken;
  balken->setMinimum(0);
  balken->setMaximum(100);
  balken->show();
  balken->setValue(0);
  if (( grad =(Vector3*)malloc(sizeof(Vector3)*bh*tiefe))==NULL) {
    qDebug()<<"Less Memory(grad)";
    exit(1);
  }

  if (( nodex =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) {
    qDebug()<<"Less Memory(X) ";
    exit(1);  
  } 
  if (( nodey =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) { 
    qDebug()<<"Less Memory(Y)!!";
    exit(1); 
  }
  if (( nodez =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) {
    qDebug()<<"Less Memory(Z)!!";
    exit(1); 
  }
  simpelGrad();
  CalcVertexes();
  CalcNormals();
  for( int ix=0; ix<breite-1; ix++ )
    for( int iy=0; iy<hoehe-1; iy++ )
      for( int iz=0; iz<tiefe-1; iz++ )
        MakeElement(ix,iy,iz,breite,bh);
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
        .arg(orte.at(i).color).toLatin1());
    lineNr++;
  }
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

  free(grad);grad=NULL;
  free(nodex);nodex=NULL;
  free(nodey);nodey=NULL;
  free(nodez);nodez=NULL;
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

QColor MolIso::farbverlauf (double wrt){
  static double anf,lang;
  static int lauf=0;
  wrt=(wrt>=1.0)?0.999999:wrt;
  wrt=(wrt<=0.0)?0.000001:wrt;
  lang=1.0/(Farben-1.0);
  lauf=(int (wrt/lang));
  anf=lang*lauf;
  double r = ((1.0-(wrt-anf)/lang)*farbe[lauf][0]+((wrt-anf)/lang)*farbe[lauf+1][0]);
  double g = ((1.0-(wrt-anf)/lang)*farbe[lauf][1]+((wrt-anf)/lang)*farbe[lauf+1][1]);
  double b = ((1.0-(wrt-anf)/lang)*farbe[lauf][2]+((wrt-anf)/lang)*farbe[lauf+1][2]);
  double a = ((1.0-(wrt-anf)/lang)*farbe[lauf][3]+((wrt-anf)/lang)*farbe[lauf+1][3]);  
  //qDebug()<<anf<<lang<<lauf<<wrt<<ff[0]<<ff[1]<<ff[2]<<ff[3];
  QColor c;
  c.setRgbF(r,g,b,a);
  return c ;
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

/*=========================P=L=A=N=E===========================================/
 *
 *                         Contours lines on a plane 
 *
 *=============================================================================*/

int MolIso::IndexSelectedP( Node& node0, Node& node1, Node& node2, Node& node3 ) {
  if( node1 && node2 && node3 ){
    GLfloat d1 = Distance( Planorte.at(node0.index).vertex, Planorte.at(node1.index).vertex ) + 
      Distance( Planorte.at(node3.index).vertex, Planorte.at(node2.index).vertex );
    GLfloat d2 = Distance( Planorte.at(node0.index).vertex, Planorte.at(node2.index).vertex ) + 
      Distance( Planorte.at(node3.index).vertex, Planorte.at(node1.index).vertex );
    if( d1 > d2 ) return 2; else return 1;
  }else{
    if(      node1 )   return 1;
    else if( node2 )   return 2;
    else if( node3 )   return 3;
  }
  return 0;
}

void MolIso::CalcPlaneVertex( int ix, int iy, int iz , Vector3 n, Vector3 ap){
  GLfloat vo, vx=0, vy=0, vz=0, mix;
  Ort o;
  vo = n*(x_dim*ix + y_dim*iy + z_dim*iz-ap);
  vx = n*(x_dim*(ix+1) + y_dim*iy + z_dim*iz - ap);
  vy = n*(x_dim*ix + y_dim*(iy+1) + z_dim*iz - ap);
  vz = n*(x_dim*ix + y_dim*iy + z_dim*(iz+1) - ap);
  if( ix != breite-1 && Intersect(vo,vx) ){
    mix=(vo/(vo-vx));
    o.vertex = x_dim*(mix+ix) + y_dim*iy + z_dim*iz;
    o.vertex+=test3;
    o.normal = n;
    o.color = data[ix+iy*breite+iz*bh]*(1.0-mix)+mix*data[(1+ix)+iy*breite+iz*bh];
    Planorte.append(o);
    nodex[ix+iy*breite+iz*bh].index=Planorte.size()-1;
    nodex[ix+iy*breite+iz*bh].flag=1; 
  }else{
    nodex[ix+iy*breite+iz*bh].flag   = 0;
  }
  if( iy != hoehe-1 && Intersect(vo,vy) ){
    mix=(vo/(vo-vy));
    o.vertex = x_dim*ix + y_dim*(mix+iy) + z_dim*iz;
    o.vertex+=test3;
    o.normal = n;
    o.color = data[ix+iy*breite+iz*bh]*(1.0-mix)+ mix*data[ix+(iy+1)*breite+iz*bh];
    Planorte.append(o);
    nodey[ix+iy*breite+iz*bh].index=Planorte.size()-1;
    nodey[ix+iy*breite+iz*bh].flag=1;
  }else{
    nodey[ix+iy*breite+iz*bh].flag=0;
  }
  if( iz != tiefe-1 && Intersect(vo,vz) ){
    mix=(vo/(vo-vz));
    o.vertex = x_dim*ix + y_dim*iy + (mix+iz)*z_dim;
    o.vertex+=test3;
    o.normal = n;
    o.color = data[ix+iy*breite+iz*bh]*(1.0-mix)+mix*data[ix+iy*breite+(iz+1)*bh];
    Planorte.append(o);
    nodez[ix+iy*breite+iz*bh].index = Planorte.size()-1;
    nodez[ix+iy*breite+iz*bh].flag=1;
  }else{
    nodez[ix+iy*breite+iz*bh].flag  = 0;
  }
}

void MolIso::makeFacesp(int nn, Node poly[] ){
  //   /* 
  int n=0;
  int ly[13];
  for (int j=0;j<nn;j++)//zu nahe beieinander liegende verts ignorieren
    if (Distance(Planorte.at(poly[j].index).vertex,Planorte.at(poly[(j+1)%nn].index).vertex)>0.00)
      ly[n++]=poly[j].index;
    else{
      Planorte[poly[j].index].vertex=
        Planorte[poly[(j+1)%nn].index].vertex=
        (Planorte.at(poly[j].index).vertex+Planorte.at(poly[(j+1)%nn].index).vertex)*0.5f;
      Planorte[poly[j].index].color=
        Planorte[poly[(j+1)%nn].index].color=
        (Planorte.at(poly[j].index).color+Planorte.at(poly[(j+1)%nn].index).color)*0.5f;
      ly[n++]=poly[(j+1)%nn].index;
      j++;
    }
  GLfloat mid_col = 0.0;
  Vector3 mid_ver = Vector3(0,0,0);
  Vector3 mid_nor = Vector3(0,0,0);
  for(int i=0; i<n; i++ ){
    mid_ver += Planorte.at(ly[i]).vertex;
    mid_nor += Planorte.at(ly[i]).normal;
    mid_col += Planorte.at(ly[i]).color;
  }
  mid_ver *= (1.0/n);
  Normalize(mid_nor);
  mid_col *= (1.0/n);
  if (n<3) return;  //weniger als 3 verts -> nichts zu tun
  Polygn neupoly;
  neupoly.vertex=mid_ver;
  neupoly.n=n;
  if (Planorte.at(poly[0].index).direct*iso_level<0.0){//drehsinn der polygone abfragen* /
    for(int i=0; i<n; i++ ){
      neupoly.ii[i]=ly[i];
    }
  }else{
    for(int i=n-1,k=0; i>=0; i-- ){
      neupoly.ii[i]=ly[k++];
    }
  }//else* /
  Planpgns.append(neupoly);
  //  */
}

void MolIso::MakeElementp( int ix, int iy, int iz ,int s1, int s2) {//das ist der Teil des japanischen Programms den ich nicht verstehe.
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
      int sol = IndexSelectedP(node[conn[i][m][0]],node[conn[i][m][1]],
          node[conn[i][m][2]],node[conn[i][m][3]]);
      //   ai=i;
      i = conn[i][m][sol];

      if( sol == 2 ) m ^= 1;
      node[i].flag = 0;
    }while( (i!=is)&&(n<11) );
      {
        makeFacesp( n, polygon );
      }
  }
  //  */
}

void MolIso::makePlane(QList<Vector3> &lines,int a1, int a2, int a3) {
  /*
  farbe[0][0]=0.6;    
  farbe[0][1]=0.0;    
  farbe[0][2]=0.0;    
  farbe[0][3]=1.0;    

  farbe[1][0]=0.6;    
  farbe[1][1]=0.0;    
  farbe[1][2]=0.0;    
  farbe[1][3]=1.0;    

  farbe[2][0]=0.6;    
  farbe[2][1]=0.0;    
  farbe[2][2]=0.0;    
  farbe[2][3]=1.0;    

  farbe[3][0]=0.0;    
  farbe[3][1]=0.0;    
  farbe[3][2]=0.0;    
  farbe[3][3]=1.0;    

  farbe[4][0]=0.0;    
  farbe[4][1]=0.0;    
  farbe[4][2]=1.0;    
  farbe[4][3]=1.0;

  farbe[5][0]=0.0;    
  farbe[5][1]=0.0;    
  farbe[5][2]=1.0;    
  farbe[5][3]=1.0;

  farbe[6][0]=0.0;    
  farbe[6][1]=0.0;    
  farbe[6][2]=1.0;    
  farbe[6][3]=1.0;

  Farben=7;
  */

  //fprintf(stderr, "makePlane\n");
  extern molekul mol;
  extern QList<INP> xdinp;
  orig=Vector3(0,0,0);
  QString isoFileName=contMapFile->text();
  QFile isoF(isoFileName);
  int fileType=0;
  readXDGridHeader(isoFileName, fileType);
  //printf("file type %d\n",fileType);
  if (fileType==-666) {
    return;
  }
  bh=breite*hoehe;
  data.clear();

  if (( nodex =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) {
    qDebug()<<"Less Memory(X) ";
    exit(1);  
  } 
  if (( nodey =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) { 
    qDebug()<<"Less Memory(Y)!!";
    exit(1); 
  }
  if (( nodez =(Node*)malloc(sizeof(Node)*bh*tiefe*2))==NULL) {
    qDebug()<<"Less Memory(Z)!!";
    exit(1); 
  }
  for (int ix=0; ix<bh*tiefe*2; ix++){
    nodex->index=0;
    nodex->flag=0;
    nodey->index=0;
    nodey->flag=0;
    nodez->index=0;
    nodez->flag=0;
  }

  if (fileType==0){//if (isoFileName.contains("grd")) {
    isoF.open(QIODevice::ReadOnly);
    while (!QString(isoF.readLine()).contains("Values")) {;}
    int p=0,pmax=breite*hoehe*tiefe;
    while (!isoF.atEnd () && (p<pmax)) {
      QStringList numbers = QString(isoF.readLine()).split(" ",QString::SkipEmptyParts);
      for (int i=0; i<numbers.size();i++) data.append(numbers.at(i).toDouble());
      p+=numbers.size();
      numbers.clear();
    }
    isoF.close(); 
    {
      double minv=1e37,maxv=-1e38,summ=0.0;
      for(int ii=0; ii<data.size();ii++) {
        summ+=data.at(ii);
        minv=qMin(data.at(ii),minv);
        maxv=qMax(data.at(ii),maxv);
      }
      //printf("minmax grd: %g %g %g\n",minv,maxv,summ);
    }
  }else if ((fileType==321)||(fileType==125)){// ein cube file hoffentlich oder ein ?ed file 
    isoF.open(QIODevice::ReadOnly);
    if (fileType==321) for (int i=0; i<3 ;i++) isoF.readLine();
    else for (int i=0; i<(atomanzahl+6);i++) isoF.readLine();
    int pmax=breite*hoehe*tiefe;
    int p=pmax-1;
    QVector<double> rewerte(pmax);
    while (!isoF.atEnd () && (p>=0)) {
      QStringList numbers = QString(isoF.readLine()).split(QRegExp("\\s+"),QString::SkipEmptyParts);
      if ((!numbers.isEmpty())&&(!numbers.at(0).contains('.'))) continue;
      for (int i=0; i<numbers.size();i++) {rewerte[p--]=(numbers.at(i).toDouble());}
    }
    for (int i=(tiefe)-1;i>=0;i--)
      for (int j=(hoehe) -1;j>=0;j--)
        for (int k=(breite) -1;k>=0;k--)
          data.append(rewerte[i+j*(tiefe)+k*(tiefe)*(hoehe)]);// */
    cubeiso=true;
    isoF.close(); 
  }else 
    if(fileType==81){
      isoF.open(QIODevice::ReadOnly);
      float *floatdat=(float*)malloc(bh*sizeof(float));
      isoF.read((char*) floatdat,sizeof(float)*bh);//skip first record 
      for (int i=0;i<tiefe;i++){
        isoF.read((char*) floatdat,sizeof(float)*bh);
        //  printf("%f %f %f\n",floatdat[0],floatdat[1],floatdat[2]);
        for (int j=0;j<bh;j++)
          data.append(floatdat[j]);// */
      }
      {
        double minv=1e37,maxv=-1e38,meanv=0.0;
        for(int ii=0; ii<data.size();ii++) {
          meanv+=data.at(ii);
          minv=qMin(data.at(ii),minv);
          maxv=qMax(data.at(ii),maxv);
        }
        //printf("minmax m81: %g %g mean: %g %g\n",minv,maxv,meanv/data.size(),meanv/data.size()*mol.zelle.V);
      }
      free(floatdat);
      cubeiso=true;
      isoF.close(); 
    }else 
      if(fileType==7202){
        isoF.open(QIODevice::ReadOnly);
        int head;

        cubeiso=true;
        char *headdummy;
        isoF.read((char*) &head,sizeof(int));
        headdummy=(char*) malloc(head);
        isoF.read((char*) headdummy, head);
        isoF.read((char*) &head,sizeof(int));
        isoF.read((char*) &head,sizeof(int));
        double *doubdat=(double*)malloc(head);
        isoF.read((char*) doubdat,head);
        isoF.close();
        head/=sizeof(double);
        double rmin=9.0e99,rmax=-9.0e99,meanv=0.0;
        //qDebug()<<head<<bh*tiefe;
        for (int i = 0; i<head; i++){
          data.append(doubdat[i]/capVx);
          meanv+=data.at(i);
          rmin=fmin(data.at(i),rmin);
          rmax=fmax(data.at(i),rmax);

        }
        //printf("minmax raw: %g %g mean: %g %g\n",rmin,rmax,meanv/data.size(),meanv/data.size()*mol.zelle.V);
        free(headdummy);
        free(doubdat);
      }
  Planorte.clear();
  Planpgns.clear();
  lines.clear();
  contval.clear();
  Vector3 pnormal,aufpunkt;

  test3= ((breite-1)/-2.0) *  x_dim + ((hoehe-1)/-2.0) * y_dim + ((tiefe-1)/-2.0) * z_dim + orig;
  if (cubeiso) test3 =orig;
  //printf("cubeiso %d %g %g %g\n",cubeiso,test3.x,test3.y,test3.z);
  printf("Atomindices: %d %d %d < %d\n",a1,a2,a3,xdinp.size());
  //fprintf(stderr, "makePlane %d %p\n",asymmUnit.size(),nodex);
  if ((a1>=xdinp.size())||(a2>=xdinp.size())||(a3>=xdinp.size())) return;
  if ((a1<0)||(a2<0)||(a3<0)) return;
  if  (xdinp[a1].kart==xdinp[a2].kart){
    mol.frac2kart(xdinp[a1].frac,xdinp[a1].kart);
    mol.frac2kart(xdinp[a2].frac,xdinp[a2].kart);
    mol.frac2kart(xdinp[a3].frac,xdinp[a3].kart);
  }
  Vector3 a1v=Vector3(xdinp.at(a1).kart.x,xdinp.at(a1).kart.y,xdinp.at(a1).kart.z);
  Vector3 a2v=Vector3(xdinp.at(a2).kart.x,xdinp.at(a2).kart.y,xdinp.at(a2).kart.z);
  Vector3 a3v=Vector3(xdinp.at(a3).kart.x,xdinp.at(a3).kart.y,xdinp.at(a3).kart.z);
  pnormal = Normalize((a2v - a1v) % (a3v - a1v));
  switch (centerIsOn->currentIndex()){
    case 0: aufpunkt = Vector3(xdinp.at(a1).kart.x,xdinp.at(a1).kart.y,xdinp.at(a1).kart.z);break;
    case 1: aufpunkt = Vector3(xdinp.at(a2).kart.x,xdinp.at(a2).kart.y,xdinp.at(a2).kart.z);break;
    case 2: aufpunkt = Vector3(xdinp.at(a3).kart.x,xdinp.at(a3).kart.y,xdinp.at(a3).kart.z);break;
    case 3: {
              aufpunkt = (1.0/3.0)*
                (Vector3(xdinp.at(a1).kart.x,xdinp.at(a1).kart.y,xdinp.at(a1).kart.z)+
                Vector3(xdinp.at(a2).kart.x,xdinp.at(a2).kart.y,xdinp.at(a2).kart.z)+
                Vector3(xdinp.at(a3).kart.x,xdinp.at(a3).kart.y,xdinp.at(a3).kart.z));
              break;
            }
    
  }
//  aufpunkt = Vector3(xdinp.at(a1).kart.x,xdinp.at(a1).kart.y,xdinp.at(a1).kart.z);
  //printf("%g  %g %g %g   %g %g %g\n",Norm(pnormal),pnormal.x,pnormal.y,pnormal.z,aufpunkt.x,aufpunkt.y,aufpunkt.z);
  double angle = winkel(pnormal, Vector3(0,0,1))/180.0*M_PI;
  Vector3 ax= Normalize(pnormal % Vector3(0,0,1));
  double cp = cos(angle), cp1 = 1.0 - cos(angle), sp = sin(angle);
  double X=ax.x,Y=ax.y,Z=ax.z;
  Matrix ROT = Matrix(
         cp+cp1*X*X, -Z*sp+cp1*X*Y,  Y*sp+cp1*X*Z, 
       Z*sp+cp1*X*Y,    cp+cp1*Y*Y, -X*sp+cp1*Y*Z, 
      -Y*sp+cp1*X*Z,  X*sp+cp1*Y*Z,    cp+cp1*Z*Z);
  V3 bv=ROT*Normalize(V3(a2v.x-a1v.x,a2v.y-a1v.y,a2v.z-a1v.z));
  //printf("BV %g %g %g\n",bv.x,bv.y,bv.z);
  double bww=mol.winkel(bv,V3( 1,0,0))/180.0*M_PI;
  cp=cos(bww);
  sp=sin(bww);
  //printf("%g %g %g\n",bww*180.0/M_PI,cp,sp);
  Matrix R2 = Matrix(
      cp, sp, 0,
     -sp, cp, 0,
       0,  0, 1);//*/
  bv=R2*bv;
  bww=mol.winkel(bv,V3( 1,0,0));
  if (fabs(bww)>1) {
  R2 = Matrix(
      cp,-sp, 0,
      sp, cp, 0,
       0,  0, 1);//*/
  
  } 
  //printf("BV %g %g %g %g\n",bv.x,bv.y,bv.z,bww);
  V3 vt = ROT * V3(pnormal.x, pnormal.y, pnormal.z);
  vt = R2*vt;
  //printf("ang %g %g %g \n%12.6f %12.6f %12.6f \n%g %g\n%g\n",angle/M_PI*180.0, pnormal*ax,ax*Vector3(0,0,1), vt.x, vt.y, vt.z,determinant(ROT),Norm(ax),winkel(a1v,Vector3(0,1,0)));
  //fprintf(stderr, "makePlane\n");
  for( int ix=0; ix<breite; ix++ ){
    for( int iy=0; iy<hoehe; iy++ ){
      for( int iz=0; iz<tiefe; iz++ ){
        CalcPlaneVertex(ix,iy,iz,pnormal,aufpunkt);
      }
    }
  }
  //fprintf(stderr, "makePlane\n");
  for( int ix=0; ix<breite-1; ix++ )
    for( int iy=0; iy<hoehe-1; iy++ )
      for( int iz=0; iz<tiefe-1; iz++ )
        MakeElementp(ix,iy,iz,breite,bh);
  //fprintf(stderr, "makePlane\n");
  double minP=1e37,maxP=-1e38;
  for (int i=0; i<Planorte.size(); i++) {
    minP=fmin(Planorte.at(i).color,minP);
    maxP=fmax(Planorte.at(i).color,maxP);
  }

  //fprintf(stderr, "makePlane Planpgns.size = %d Planorte.size = %d minP = %g maxP = %g\n",Planpgns.size(),Planorte.size(),minP,maxP);
  min= 1e37;
  max=-1e37;
  QStringList numbers = QString(contourValueEdit->text()).split(" ",QString::SkipEmptyParts);
  int z=0;
  //printf("ok\n");
  if (!numbers.isEmpty()) contval[0]=numbers.at(0).toFloat();
  for (int i = 0; i < numbers.size(); i++){
    float contour = numbers.at(i).toFloat();
    findContour(lines,contour);
    contval[lines.size()]=contour;
    if ((lines.size()-z)>0){
      min=fmin(contour,min);
      max=fmax(contour,max);
    }
    z=lines.size();
  }
  //printf("ok %f %f\n",min,max);
  //legende();
  free(nodex);
  nodex=NULL;
  free(nodey);
  nodey=NULL;
  free(nodez);
  nodez=NULL;
  data.clear();
  Planpgns.clear();
  Planorte.clear();
  //printf("ok\n");
  QList<V3> lin2d;
  int aspect1=3,aspect2=4;//todo make aspect ratio changable
  switch (aspectRatios->currentIndex()) {
    case 0: aspect1=3,aspect2=4; break;
    case 1: aspect1=9,aspect2=16; break;
    case 2: aspect1=1,aspect2=1; break;
  }
  int width=1024*aspect1/aspect2;
  float miX=9.e30,maX=-9.e30;
  float miY=9.e30,maY=-9.e30;
  V3 v1;//,v2=V3(99,99,99);
  for (int i=0; i<lines.size(); i++){
    v1=R2*(ROT * V3(lines.at(i).x-aufpunkt.x,lines.at(i).y-aufpunkt.y,lines.at(i).z-aufpunkt.z));
    //if (Distance(v1,v2)>0.0) 
    lin2d.append(v1);
    miX=fmin(v1.x,miX);
    maX=fmax(v1.x,maX);
    miY=fmin(v1.y,miY);
    maY=fmax(v1.y,maY);


    //v2=v1;
  }
  printf("!%d == %d! %f < X < %f, %f < Y < %f \n",lin2d.size(),lines.size(),miX,maX,miY,maY);
  if (cScopeBx->value() > 0.0){
    double XX=cScopeBx->value();
    double YY=(XX*aspect1)/aspect2;//todo make aspect ratio changable
    miX=-XX;
    miY=-YY;
    maX= XX;
    maY= YY;
  }
  printf("!%d == %d! %f < X < %f, %f < Y < %f \n",lin2d.size(),lines.size(),miX,maX,miY,maY);
  if (contEPSFile->text().isEmpty()){ 
    qDebug()<<"contEPSFile->text() is Empty()";
    return;}

//      ||(!QFile::exists (contEPSFile->text()))) 
//qDebug()<<QDate::currentDate().toString(Qt::ISODate);
  FILE *f=fopen(contEPSFile->text().toLocal8Bit(),"wt");
  fprintf(f,"%s%d\n","%!PS-Adobe-3.0 EPSF-3.0\n%%BoundingBox: 0 0 1024 ",width);
  fprintf(f,"%s",
"%%Title: MoleCoolQt Contour Plot\n"
"%%Creator: MoleCoolQt by Christian B. Hubschle\n");
  fprintf(f,"%%%%CreationDate: %s\n",QDate::currentDate().toString(Qt::ISODate).toStdString().c_str());

  fprintf(f,"%s",
//"%%Pages: 1\n"
"%%EndComments\n"
"%%BeginProlog\n"
//"/l { newpath moveto lineto stroke } bind def\n"
//"/nm { newpath moveto } bind def\n"
"/cp {closepath} bind def\n"
"/ef {eofill} bind def\n"
"/gr {grestore} bind def\n"
"/gs {gsave} bind def\n"
"/sa {save} bind def\n"
"/rs {restore} bind def\n"
"/l {lineto} bind def\n"
"/m {moveto} bind def\n"
"/rm {rmoveto} bind def\n"
"/n {newpath} bind def\n"
"/s {stroke} bind def\n"
"/sh {show} bind def\n"
"/slc {setlinecap} bind def\n"
"/slj {setlinejoin} bind def\n"
"/slw {setlinewidth} bind def\n"
"/srgb {setrgbcolor} bind def\n"
"/rot {rotate} bind def\n"
"/sc {scale} bind def\n"
"/sd {setdash} bind def\n"
"/ff {findfont} bind def\n"
"/sf {setfont} bind def\n"
"/scf {scalefont} bind def\n"
"/sw {stringwidth} bind def\n"
//"%%EndSetup\n"
"%%EndProlog\n"
//"%%Page: 1 1\n"
//"gsave\n"
//"20 20 scale\n"
//"10 10 translate\n"
"save\n0 slc 0 slj\n/Helvetica ff 18 scf sf\n");
fprintf(f,"n 0 0 m 1024 0 l 1024 %d l 0 %d l cp clip s\n",width,width);
fprintf(f,"n 2 2 m 1023 2 l 1023 %d l 2 %d l cp s \n",width-2,width-2);
//"0 setgray\n"


  //Atoms
  for (int i=0; i<xdinp.size(); i++){
    V3 ato=R2*(ROT*(xdinp.at(i).kart-V3(aufpunkt.x,aufpunkt.y,aufpunkt.z)));
    if ((ato.x>maX)||(ato.x<miX)||(ato.y>maY)||(ato.y<miY)) continue;
    //Acol[xdinp[j].OrdZahl]
    if (fabs(ato.z)<0.2){
      double rad=(xdinp[i].OrdZahl>-1)?mol.arad[xdinp[i].OrdZahl]:0.15;
      fprintf(f,"n %G %G %G 0 360 arc cp gs %g %g %g srgb fill s gr%% %g %s\n",
          (ato.x-miX)/(maX-miX)*1024, (ato.y-miY)/(maY-miY)*width, rad*50,
          mol.Acol[xdinp[i].OrdZahl][0], 
          mol.Acol[xdinp[i].OrdZahl][1], 
          mol.Acol[xdinp[i].OrdZahl][2],ato.z,
          xdinp.at(i).atomname);
    }
    //    4 5 3 0 360 arc closepath
  }
  //Bonds
  for (int k=0;k<mol.bcnt;k++){
    V3 anf=R2*(ROT*(xdinp.at(mol.bd[k].a).kart-V3(aufpunkt.x,aufpunkt.y,aufpunkt.z)));
    V3 end=R2*(ROT*(xdinp.at(mol.bd[k].e).kart-V3(aufpunkt.x,aufpunkt.y,aufpunkt.z)));
    if ((anf.x>maX)||(anf.x<miX)||(anf.y>maY)||(anf.y<miY)) continue;
    if ((end.x>maX)||(end.x<miX)||(end.y>maY)||(end.y<miY)) continue;
    if ((fabs(anf.z)<0.2)&&(fabs(end.z)<0.2))
    fprintf(f,"n %G %G m %G %G l cp s\n",
        (anf.x-miX)/(maX-miX)*1024, (anf.y-miY)/(maY-miY)*width,
        (end.x-miX)/(maX-miX)*1024, (end.y-miY)/(maY-miY)*width);

  }

 // V3 v1,v2;
 //qDebug()<<contval;
  float red=0.0f,green=0.0f,blue=0.0f;
  QString contourDescription;
  for (int i=0; i<lin2d.size()/2; i++){
    //(wrt-min)/(max-min)
    if (contval.contains(i*2)) {
      if (contval.value(i*2)>0.00001f) {
        red=0.0f;green=0.0f;blue=1.0f;
        contourDescription.append(QString("%blue@%1, %2 lines\n").arg(contval.value(i*2)).arg(i));
//        printf("blau %f %d\n",contval.value(i*2),i);
      }
      else if (contval.value(i*2)<-0.00001f){
        red=1.0f;green=0.0f;blue=0.0f;
        contourDescription.append(QString("%red@%1, %2 lines\n").arg(contval.value(i*2)).arg(i));
//        printf("rot %f %d\n",contval.value(i),i);
      }
      else {
        red=0.0f;green=0.0f;blue=0.0f;
        contourDescription.append(QString("%black@%1, %2 lines\n").arg(contval.value(i*2)).arg(i));
//        printf("schwarz %f %d\n",contval.value(i*2),i);
      }
    }
    if ((lin2d.at(2*i  ).x>maX)||(lin2d.at(2*i  ).x<miX)||(lin2d.at(2*i  ).y>maY)||(lin2d.at(2*i  ).y<miY)) continue;
    if ((lin2d.at(2*i+1).x>maX)||(lin2d.at(2*i+1).x<miX)||(lin2d.at(2*i+1).y>maY)||(lin2d.at(2*i+1).y<miY)) continue;
    fprintf(f,"n %G %G m %G %G l cp gs %g %g %g srgb 0.5 slw s gr\n",
        (lin2d.at(2*i  ).x-miX)/(maX-miX)*1024, (lin2d.at(i*2  ).y-miY)/(maY-miY)*width, 
        (lin2d.at(2*i+1).x-miX)/(maX-miX)*1024, (lin2d.at(i*2+1).y-miY)/(maY-miY)*width,red,green,blue);
  }
  for (int i=0; i<xdinp.size(); i++){
    V3 ato=R2*(ROT*(xdinp.at(i).kart-V3(aufpunkt.x,aufpunkt.y,aufpunkt.z)));
    if ((ato.x>maX)||(ato.x<miX)||(ato.y>maY)||(ato.y<miY)) continue;
    if (fabs(ato.z)<0.2){
      double rad=(xdinp[i].OrdZahl>-1)?mol.arad[xdinp[i].OrdZahl]:0.15;
      fprintf(f,"gs n %G %G m (%s) true charpath 1 slw 1 setgray s gr\n",(ato.x-miX)/(maX-miX)*1024+rad*50, (ato.y-miY)/(maY-miY)*width,xdinp.at(i).atomname);
      fprintf(f,"gs %G %G m (%s) sh gr\n",(ato.x-miX)/(maX-miX)*1024+rad*50, (ato.y-miY)/(maY-miY)*width,xdinp.at(i).atomname);
    }
    //    4 5 3 0 360 arc closepath
  }
  fprintf(f,"%s",
      "restore\n"
      "showpage\n"      
      "%%Trailer\n"
      "%%EOF\n"
      );
  fprintf(f,"%% map: %s\n%% plane: %s %s %s\n%% contours: %s\n",
      contMapFile->text().toStdString().c_str(),
      xdinp.at(a1).atomname,
      xdinp.at(a2).atomname,
      xdinp.at(a3).atomname,
      contourDescription.toStdString().c_str());
  fclose(f);
 // printf("ok\n");


  }
