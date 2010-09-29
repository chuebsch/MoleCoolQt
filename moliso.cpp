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

void MolIso::loadMI(QString fname){
  extern QProgressBar *balken;

  extern double L;
  //qDebug()<<x_dim<<L;
  QFile sf(fname);
  if (!sf.exists()) {qDebug()<<"The File "<<fname<<" does not exist!"; exit(0);}
  if (!sf.open(QIODevice::ReadOnly)) {qDebug()<<"Can not open "<<fname<<" for reading. Please check your rights."; exit(0);}

  Ort v;
  Polygn p;
  QString all =sf.readAll();
  if (all.isEmpty()) {qDebug()<<"Can not read  "<<fname<<". may be it is empty or corrupt!"; exit(0);}
  QStringList lines = all.split(QRegExp("[\n\r]{1,2}")); 
  min=1e99;
  max=-1e99;
  //  qDebug()<<lines.at(0);
  if (lines.size()){

  balken->setMinimum(0);
  balken->setMaximum(lines[0].toInt()*7);
  balken->show();
    for (int i = 1; i <= lines[0].toInt();i++){
      QStringList tok = lines[i].split(' ',QString::SkipEmptyParts);
      if (tok.size()==8){

	balken->setValue(i);
	v.vertex.x = x_dim*(tok.at(1).toFloat()-0.5);
	v.vertex.y = x_dim*(tok.at(2).toFloat()-0.5);
	v.vertex.z = x_dim*(tok.at(3).toFloat()-0.5);
//
//	v.vertex.x = tok.at(1).toFloat();
//	v.vertex.y = tok.at(2).toFloat();
//	v.vertex.z = tok.at(3).toFloat();

	v.normal.x = tok.at(4).toFloat();
	v.normal.y = tok.at(5).toFloat();
	v.normal.z = tok.at(6).toFloat();
	v.color = tok.at(7).toFloat();
	v.direct=0;
	min=(min>v.color)?v.color:min;
	max=(max<v.color)?v.color:max;
//	printf("%d %f %f %f %f %f %f\n",orte.size(),v.vertex.x,v.vertex.y,v.vertex.z,min,max,fixmin);
	orte.append(v);
      }
    }
    max+=0.00001;
    if (fixmin!=666.666) min=fixmin;
    if (fixmax!=666.666) max=fixmax;
    //    printf("%f %f  %f %f \n",min,fixmin, max,fixmax);
    //     printf("Vektoren sind wie Regen\n");
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
//      printf("Regen ist nass %d %d \n",pgns.size(),orte.size());
  //  printf("%f\n",L);

  //  QFile f("molisobug");
  //f.open(QIODevice::Append);
  //f.write(QString("before %1 %2 %3 %4 %5 MIBAS%6\n").arg(min).arg(max).arg((char*)gluErrorString(glGetError())).arg(pgns.size()).arg(orte.size()).arg(mibas).toLatin1());
  balken->setValue(lines[0].toInt());
  glNewList(mibas, GL_COMPILE );{                       //Isooberfl"ache ::Perspektive 1     
    glPushMatrix();{
      //      glTranslated(-L/2 , -L*y_dim/x_dim/2, -L*z_dim/x_dim/2 );
      glScaled( L, L*y_dim/x_dim, L*z_dim/x_dim );           
      //      printf("Regen ist bloed\n");
      PXsort();
      //    printf("Regen ist doof\n");
      DrawPlys();
      //    printf("Regen ist nass\n");
    }glPopMatrix();
  }glEndList();

  //f.write(QString("X%1 %2 %3 mibas%4\n").arg(min).arg(max).arg((char*)gluErrorString(glGetError())).arg(mibas).toLatin1());

  balken->setValue(2*lines[0].toInt());
  //  printf("Regen ist nass\n");
  glNewList(mibas+1, GL_COMPILE  );{                       //Isooberfl"ache ::Perspektive 1     
    glPushMatrix();{
      //      glTranslated(-L/2 , -L*y_dim/x_dim/2, -L*z_dim/x_dim/2 );
      glScaled( L, L*y_dim/x_dim, L*z_dim/x_dim );           
      Pxsort();
      DrawPlys();
    }glPopMatrix();
  }glEndList();

  //f.write(QString("X%1 %2 %3 mibas%4\n").arg(min).arg(max).arg((char*)gluErrorString(glGetError())).arg(mibas+1).toLatin1());
  balken->setValue(3*lines[0].toInt());
  glNewList(mibas+2, GL_COMPILE  );{                       //Isooberfl"ache ::Perspektive 1     
    glPushMatrix();{
      //      glTranslated(-L/2 , -L*y_dim/x_dim/2, -L*z_dim/x_dim/2 );
      glScaled( L, L*y_dim/x_dim, L*z_dim/x_dim );           
      PYsort();
      DrawPlys();
    }glPopMatrix();
  }glEndList();

  //f.write(QString("Y%1 %2 %3\n").arg(min).arg(max).arg((char*)gluErrorString(glGetError())).toLatin1());
  balken->setValue(4*lines[0].toInt());
  glNewList(mibas+3, GL_COMPILE  );{                       //Isooberfl"ache ::Perspektive 1     
    glPushMatrix();{
      //      glTranslated(-L/2 , -L*y_dim/x_dim/2, -L*z_dim/x_dim/2 );
      glScaled( L, L*y_dim/x_dim, L*z_dim/x_dim );           
      Pysort();
      DrawPlys();
    }glPopMatrix();
  }glEndList();

  //f.write(QString("y%1 %2 %3\n").arg(min).arg(max).arg((char*)gluErrorString(glGetError())).toLatin1());
    balken->setValue(5*lines[0].toInt());
  glNewList(mibas+4, GL_COMPILE  );{                       //Isooberfl"ache ::Perspektive 1     
    glPushMatrix();{
      //      glTranslated(-L/2 , -L*y_dim/x_dim/2, -L*z_dim/x_dim/2 );
      glScaled( L, L*y_dim/x_dim, L*z_dim/x_dim );           
      PZsort();
      DrawPlys();
    }glPopMatrix();
  }glEndList();

  //f.write(QString("Z%1 %2 %3\n").arg(min).arg(max).arg((char*)gluErrorString(glGetError())).toLatin1());
  balken->setValue(6*lines[0].toInt());  
  glNewList(mibas+5, GL_COMPILE  );{                       //Isooberfl"ache ::Perspektive 1     
    glPushMatrix();{
      //      glTranslated(-L/2 , -L*y_dim/x_dim/2, -L*z_dim/x_dim/2 );
      glScaled( L, L*y_dim/x_dim, L*z_dim/x_dim );           
      Pzsort();
      DrawPlys();
    }glPopMatrix();
  }glEndList();

  //f.write(QString("z%1 %2 %3\n").arg(min).arg(max).arg((char*)gluErrorString(glGetError())).toLatin1());

  //f.close();  
  balken->setValue(7*lines[0].toInt());  
  //legende();
  //  printf("Regen ist nass\n");

  balken->hide();
  faceFile=fname;
}


void MolIso::DrawPlys(){
  glLoadName((GLuint)-1);
  //if (!lastcall) return;
  for (int i=0;i<pgns.size();i++){
    switch (pgns.at(i).n){
    case  3:glBegin(GL_TRIANGLES); 
      for (int k=0;k<3;k++) {
	//	    printf("#####%d %d %f\n",k,ly[k],C[ly[k]]);fflush(stdout);
	Farbverlauf(orte.at(pgns.at(i).ii[k]).color);
		      //printf("#####\n");fflush(stdout);
	glTexCoord1f((orte.at(pgns.at(i).ii[k]).color-min)/(max-min));
	glNormal3fv( (GLfloat*)&orte.at(pgns.at(i).ii[k]).normal);
	glVertex3fv( (GLfloat*)&orte.at(pgns.at(i).ii[k]).vertex);
      }          
      glEnd();
      break;
    case  4://if (Quad)
	{
	glBegin(GL_QUADS);
	for (int k=0;k<4;k++) {
	  //	  printf("[%f %f %f  %f]\n",orte.at(pgns.at(i).ii[k]).vertex.x,orte.at(pgns.at(i).ii[k]).vertex.y,orte.at(pgns.at(i).ii[k]).vertex.z,orte.at(pgns.at(i).ii[k]).color);
	  //printf("i %d k %d pgnsat %d \n",i, k , pgns.at(i).ii[k]);
	  Farbverlauf(orte.at(pgns.at(i).ii[k]).color);
	  //printf("[%f %f %f  %f]\n",orte.at(pgns.at(i).ii[k]).vertex.x,orte.at(pgns.at(i).ii[k]).vertex.y,orte.at(pgns.at(i).ii[k]).vertex.z,orte.at(pgns.at(i).ii[k]).color);
	  glTexCoord1f((orte.at(pgns.at(i).ii[k]).color-min)/(max-min));
	  glNormal3fv( (GLfloat*)&orte.at(pgns.at(i).ii[k]).normal);
	  glVertex3f(orte.at(pgns.at(i).ii[k]).vertex.x,orte.at(pgns.at(i).ii[k]).vertex.y,orte.at(pgns.at(i).ii[k]).vertex.z);
	}
	glEnd();
	break;
      }
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

void MolIso::readXDGridHeader(QString fname){
//  printf(" Ich lese nur Übeschriften, weißt Du?\n");
  QFile gh(fname);
  gh.open(QIODevice::ReadOnly);
  QString all =gh.readAll();
  QStringList lines = all.split(QRegExp("[\n\r]+"));
  if ((lines.size())&&(lines.at(0).contains("3DGRDFIL "))) {
    extern molekul mol;
    extern int atmax;
    extern QList<INP> asymmUnit;
    INP newAtom;
    newAtom.part=0;
    extern int smx;
    //extern double L;
    int i=0;
    while (!lines.at(i).contains("Gridpoints")) i++;
    QStringList tok = lines[i+1].split(' ',QString::SkipEmptyParts);
    breite= tok.at(0).toInt();
    hoehe=  tok.at(1).toInt();
    tiefe=  tok.at(2).toInt();
    bh = hoehe*breite;
    tok = lines[i+3].split(' ',QString::SkipEmptyParts);
    x_dim = tok.at(0).toDouble();
    //    L=10.0*x_dim;
    y_dim = tok.at(1).toDouble();
    z_dim = tok.at(2).toDouble();
//    printf("%d %d %d %f %f %f\n",breite,hoehe,tiefe,x_dim,y_dim,z_dim); 
    while (!lines.at(i).contains("Objects")) i++;
    {
      i++;
      atomanzahl=smx=atmax=lines[i].toInt();
      i++;
      for (int j=0 ; j<atmax; j++){
	tok = lines[i+j].split(' ',QString::SkipEmptyParts);
	strncpy(newAtom.atomname,tok.at(0).toLatin1(),38);
	//	newAtom.labPos.x=
	newAtom.kart.x=tok.at(1).toDouble();
//	std::cout<<tok.at(1).toStdString<<"  "<<newAtom.kart.x<<"   ";
//	newAtom.labPos.y=
	newAtom.kart.y=tok.at(2).toDouble();
//	std::cout<<tok.at(2).toStdString<<"  "<<newAtom.kart.y<<"   ";
	//	newAtom.labPos.z=
	newAtom.kart.z=tok.at(3).toDouble();
//	std::cout<<tok.at(3).toStdString<<"  "<<newAtom.kart.z<<"   ";
	//       	printf("%f %f %f \n",newAtom.kart.x,newAtom.kart.y,newAtom.kart.z);
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
	newAtom.kart.x=ix*x_dim-x_dim/2.0;
	newAtom.kart.y=iy*y_dim-y_dim/2.0;
	newAtom.kart.z=iz*z_dim-z_dim/2.0;
	newAtom.OrdZahl=-1;
	strcpy(newAtom.atomname,"x");
	asymmUnit.append(newAtom);
	}
  }
  }else{//may be this is a cube file?
    printf("cubefile?\n");
    double a,b,c,xst,yst,zst;
    double bohr=0.5291775108;// 0.5291772108;
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
				   "Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","J","Xe",
				   "Cs","Ba", "La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu",
				   "Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra",
				   "Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No","Lr","Ku","Ha","Rf","Ns","Hs","Mt"};
	atomanzahl=smx=atmax=tok.at(0).toInt();
	a=tok.at(1).toDouble();
	b=tok.at(2).toDouble();
	c=tok.at(3).toDouble();
	tok=lines.at(3).split(QRegExp("\\s+"),QString::SkipEmptyParts);	
	breite= tok.at(0).toInt();
	xst=tok.at(1).toDouble();
	tok=lines.at(4).split(QRegExp("\\s+"),QString::SkipEmptyParts);	
	hoehe=  tok.at(0).toInt();
	yst=tok.at(2).toDouble();
	tok=lines.at(5).split(QRegExp("\\s+"),QString::SkipEmptyParts);	
	tiefe=  tok.at(0).toInt();
	zst=tok.at(3).toDouble();
	x_dim = (breite-1)*xst*bohr;
	y_dim = (hoehe-1) *yst*bohr;
	z_dim = (tiefe-1) *zst*bohr;
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
//  printf("sg %d %d %d %d %d\n",breite,hoehe,tiefe,breite,bh);
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
  Vector3 movo(0.5,0.5,0.5);
  Ort o;
  sig = (iso_level>0)?-1:1;
  vo = data.at(ix+iy*breite+iz*bh)  - iso_level;
  if( ix != breite-1 ) vx = data.at((1+ix)+iy*breite+iz*bh) - iso_level;
  if( iy != hoehe-1 ) vy = data.at(ix+(iy+1)*breite+iz*bh) - iso_level;
  if( iz != tiefe-1 ) vz = data.at(ix+iy*breite+(iz+1)*bh) - iso_level;
  if( ix != breite-1 && Intersect(vo,vx) ){
    o.vertex= Vector3( ((ix+vo/(vo-vx))+0.5)/breite, (iy+0.5)/hoehe, (iz+0.5)/tiefe );
    if (cubeiso) o.vertex+=movo;
    o.normal= Normalize(sig*( grad[ix+iy*breite+iz*bh]*(1.0-vo/(vo-vx)) +grad[(1+ix)+iy*breite+iz*bh]*(1.0-vo/(vo-vx))));
    o.color= mdata[ix+iy*breite+iz*bh]*(1.0-vo/(vo-vx))+vo/(vo-vx)*mdata[(1+ix)+iy*breite+iz*bh];
    orte.append(o);
    nodex[ix+iy*breite+iz*bh].index=orte.size()-1;
    nodex[ix+iy*breite+iz*bh].flag=1; 
  }else{
    nodex[ix+iy*breite+iz*bh].flag   = 0;
  }
  if( iy != hoehe-1 && Intersect(vo,vy) ){
    o.vertex=Vector3( (ix+0.5)/breite, (iy+vo/(vo-vy)+0.5)/hoehe, (iz+0.5)/tiefe );
    if (cubeiso) o.vertex+=movo;
    o.normal=Normalize(sig*( grad[ix+iy*breite+iz*bh]*(1.0-vo/(vo-vy)) +grad[ix+(iy+1)*breite+iz*bh]*(1.0-vo/(vo-vy))));
    o.color=mdata[ix+iy*breite+iz*bh]*(1.0-vo/(vo-vy))+ vo/(vo-vy)*mdata[ix+(iy+1)*breite+iz*bh];
    orte.append(o);
    nodey[ix+iy*breite+iz*bh].index=orte.size()-1;
    nodey[ix+iy*breite+iz*bh].flag=1;
  }else{
    nodey[ix+iy*breite+iz*bh].flag=0;
  }
  if( iz != tiefe-1 && Intersect(vo,vz) ){
    o.vertex = Vector3( (ix+0.5)/breite, (iy+0.5)/hoehe, (iz+vo/(vo-vz)+0.5)/tiefe );
    if (cubeiso) o.vertex+=movo;
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
  //FILE *test= fopen("Directions.txt","a");
  //fprintf(test,"neu\n");
  for( int ix=0; ix<breite; ix++ ){
    for( int iy=0; iy<hoehe; iy++ ){
      for( int iz=0; iz<tiefe; iz++ ){
	if( nodex[ix+iy*breite+iz*bh] ){
	  orte[nodex[ix+iy*breite+iz*bh].index].direct = (CalcNormalX(ix,iy,iz)>0)?1:-1;
	  //fprintf(test,"%9d [%4d,%4d,%4d] D:%2d X\n",nodex[ix+iy*breite+iz*bh].index,ix,iy,iz,orte[nodex[ix+iy*breite+iz*bh].index].direct);
	}
	if( nodey[ix+iy*breite+iz*bh] ){
	  orte[nodey[ix+iy*breite+iz*bh].index].direct = (CalcNormalY(ix,iy,iz)>0)?1:-1;
	  //fprintf(test,"%9d [%4d,%4d,%4d] D:%2d Y\n",nodey[ix+iy*breite+iz*bh].index,ix,iy,iz,orte[nodey[ix+iy*breite+iz*bh].index].direct);
	}
	if( nodez[ix+iy*breite+iz*bh] ){
	  orte[nodez[ix+iy*breite+iz*bh].index].direct = (CalcNormalZ(ix,iy,iz)>0)?1:-1;
	  //fprintf(test,"%9d [%4d,%4d,%4d] D:%2d Z\n",nodez[ix+iy*breite+iz*bh].index,ix,iy,iz,orte[nodez[ix+iy*breite+iz*bh].index].direct);
	}
      }
    }
  }
  //fclose(test);
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

    int n=0, i=is, m=0,ai=i;
    do {
      polygon[n++]= node[i];
      int sol = IndexSelected(node[conn[i][m][0]],node[conn[i][m][1]],
                      node[conn[i][m][2]],node[conn[i][m][3]]);
      ai=i;
      i = conn[i][m][sol];

      if( sol == 2 ) m ^= 1;
      node[i].flag = 0;
    }while( (i!=is)&&(n<11) );
    {
      //DrawPolygon( n, polygon );
      makeFaces( n, polygon );
    }
  }
//  */
}

void MolIso::createSurface(QString isoFileName, QString mapFileName, QString &storeFaceName){
//  QTime uhr;
//  uhr.start();
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
  isoF.open(QIODevice::ReadOnly);
  if (isoFileName.contains("grd")) {
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
  //qDebug()<<data.size()<<data.last();
  }else {// ein cube file hoffentlich
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
  }
  isoF.close(); 
  if (mapFileName==isoFileName) {
    mdata=data;
  }else {
  mapF.open(QIODevice::ReadOnly);
  if (mapFileName.contains("grd")) {
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
  }else {// ein cube file hoffentlich
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
  }

  mapF.close();                                                                              
  }
  if (data.size()!=mdata.size()) {
    qDebug()<<"Map-Grid and Iso-Grid must have the same size!";
    qApp->quit();
  }
  //  printf("%d %d breite %d hoehe %d tiefe %d bh %d \n",data.size(),mdata.size(),breite,hoehe,tiefe,bh);
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
//    std::cout<<iso_level<<std::endl;
//    std::cout<<cubemap<<cubeiso<<std::endl;
    CalcVertexes();
    CalcNormals();
//    std::cout<<
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
    //  printf("PCN %d \n",pgns.size());
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
  //  std::cout<<uhr.restart()<<std::endl;
  //qDebug()<<"Ich brauch Speicher!\n";
  free(grad);
  free(nodex);
  free(nodey);
  free(nodez);
  mdata.clear();
  data.clear();
  pgns.clear();
  orte.clear();
  //qDebug()<<"Ich brauch weniger Speicher!\n";
  //  qApp->quit();
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
