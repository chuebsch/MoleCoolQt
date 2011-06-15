#include "fourmcq.h"
#include <QtCore>
#include <QtGui>
FourMCQ::FourMCQ(molekul *mole_, CubeGL *chgl_,QToolBar *toolView, double resol, double wght){
  mole =mole_;
  chgl=chgl_;
  map_radius=5.0;
  maptrunc = 1;
  chgl->foubas[0]=0;
  chgl->foubas[1]=0;
  chgl->foubas[2]=0;
  chgl->foubas[3]=0;
  chgl->foubas[4]=0;
  datfo_fc=datf1_f2=datfo=NULL;
  nodex=nodey=nodez=NULL;
  urs=V3(0,0,0);
  nr=0;
  nc=0;
  lintrans=0.6;
  linwidth=1.0;
  rr=resol;
  rw=wght; 
  chgl->foact=toolView->addAction(QIcon(":/images/fomap.png"),"toggle Fo map",chgl,SLOT(updateGL()));
  chgl->foact->setCheckable(true);
  chgl->foact->setChecked(false);
  chgl->fofcact=toolView->addAction(QIcon(":/images/diffmap.png"),"toggle Fo-Fc map",chgl,SLOT(updateGL()));
  chgl->fofcact->setCheckable(true);
  chgl->fofcact->setChecked(true);
  chgl->f1f2act=toolView->addAction(QIcon(":/images/f12map.png"),"toggle F1-F2 map",chgl,SLOT(updateGL()));
  chgl->f1f2act->setCheckable(true);
  chgl->f1f2act->setChecked(false);
  chgl->fofcact->setVisible(false);
  chgl->foact->setVisible(false);
  chgl->f1f2act->setVisible(false);
}

FourMCQ::~FourMCQ(){
}

void FourMCQ::killmaps(){
  chgl->fofcact->setVisible(false);
  chgl->foact->setVisible(false);
  chgl->f1f2act->setVisible(false);
  if (datfo!=NULL) free(datfo);
  if (datfo_fc!=NULL) free(datfo_fc);
  if (datf1_f2!=NULL) free(datf1_f2);
  datfo=datfo_fc=datf1_f2=NULL;
  deleteLists();
  if (nodex!=NULL) free(nodex);
  if (nodey!=NULL) free(nodey);
  if (nodez!=NULL) free(nodez);
  nodex=nodey=nodez=NULL;
  disconnect(chgl,SIGNAL(diffscroll(int ,int )),0,0);
  disconnect(chgl,SIGNAL(neuemitte(V3)),0,0);
  disconnect(chgl,SIGNAL(inimibas()),0,0);

}
bool FourMCQ::loadFouAndPerform(const char filename[],bool neu){

  const int it[480]= {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 20, 21, 22, 24, 25, 26, 27,
    28, 30, 32, 33, 35, 36, 39, 40, 42, 44, 45, 48, 49, 50, 52, 54, 55, 56, 60, 63, 64, 65, 66, 70, 72, 75, 77,
    78, 80, 81, 84, 88, 90, 91, 96, 98, 99, 100, 104, 105, 108, 110, 112, 117, 120, 125, 126, 128, 130, 132, 135,
    140, 143, 144, 147, 150, 154, 156, 160, 162, 165, 168, 175, 176, 180, 182, 189, 192, 195, 196, 198, 200, 208,
    210, 216, 220, 224, 225, 231, 234, 240, 243, 245, 250, 252, 256, 260, 264, 270, 273, 275, 280, 286, 288, 294,
    297, 300, 308, 312, 315, 320, 324, 325, 330, 336, 343, 350, 351, 352, 360, 364, 375, 378, 384, 385, 390, 392,
    396, 400, 405, 416, 420, 429, 432, 440, 441, 448, 450, 455, 462, 468, 480, 486, 490, 495, 500, 504, 512, 520,
    525, 528, 539, 540, 546, 550, 560, 567, 572, 576, 585, 588, 594, 600, 616, 624, 625, 630, 637, 640, 648, 650,
    660, 672, 675, 686, 693, 700, 702, 704, 715, 720, 728, 729, 735, 750, 756, 768, 770, 780, 784, 792, 800, 810,
    819, 825, 832, 840, 858, 864, 875, 880, 882, 891, 896, 900, 910, 924, 936, 945, 960, 972, 975, 980, 990, 1000,
    1001, 1008, 1024, 1029, 1040, 1050, 1053, 1056, 1078, 1080, 1092, 1100, 1120, 1125, 1134, 1144, 1152, 1155,
    1170, 1176, 1188, 1200, 1215, 1225, 1232, 1248, 1250, 1260, 1274, 1280, 1287, 1296, 1300, 1320, 1323, 1344,
    1350, 1365, 1372, 1375, 1386, 1400, 1404, 1408, 1430, 1440, 1456, 1458, 1470, 1485, 1500, 1512, 1536, 1540,
    1560, 1568, 1575, 1584, 1600, 1617, 1620, 1625, 1638, 1650, 1664, 1680, 1701, 1715, 1716, 1728, 1750, 1755,
    1760, 1764, 1782, 1792, 1800, 1820, 1848, 1872, 1875, 1890, 1911, 1920, 1925, 1944, 1950, 1960, 1980, 2000,
    2002, 2016, 2025, 2048, 2058, 2079, 2080, 2100, 2106, 2112, 2145, 2156, 2160, 2184, 2187, 2200, 2205, 2240,
    2250, 2268, 2275, 2288, 2304, 2310, 2340, 2352, 2376, 2400, 2401, 2430, 2450, 2457, 2464, 2475, 2496, 2500,
    2520, 2548, 2560, 2574, 2592, 2600, 2625, 2640, 2646, 2673, 2688, 2695, 2700, 2730, 2744, 2750, 2772, 2800,
    2808, 2816, 2835, 2860, 2880, 2912, 2916, 2925, 2940, 2970, 3000, 3003, 3024, 3072, 3080, 3087, 3120, 3125,
    3136, 3150, 3159, 3168, 3185, 3200, 3234, 3240, 3250, 3276, 3300, 3328, 3360, 3375, 3402, 3430, 3432, 3456,
    3465, 3500, 3510, 3520, 3528, 3564, 3575, 3584, 3600, 3640, 3645, 3675, 3696, 3744, 3750, 3773, 3780, 3822,
    3840, 3850, 3861, 3888, 3900, 3920, 3960, 3969, 4000, 4004, 4032, 4050, 4095, 4096, 4116, 4125, 4158, 4160,
    4200, 4212, 4224, 4290, 4312, 4320, 4368, 4374, 4375, 4400, 4410, 4455, 4459, 4480, 4500, 4536, 4550, 4576,
    4608, 4620, 4680, 4704, 4725, 4752, 4800, 4802, 4851, 4860, 4875, 4900, 4914, 4928, 4950, 4992, 5000};
  killmaps();
  int winformat=0,ok;
  if (!doMaps->isChecked()) return false;
  if (strstr(filename,"xd.fou")==NULL) return false;
  FILE *f;
  f=fopen(filename,"rb");
  if (f==NULL) return false;
  fseek (f , 0 , SEEK_END);
  size_t lSize = ftell (f), readsize;
  rewind (f);
  if (!(lSize%sizeof(reco))&&(lSize%sizeof(rec64)))winformat=1;
  //printf("%lu %lu\n",lSize%sizeof(reco),lSize%sizeof(rec64));
  ok= readMas(filename);
  nr=0;
  if (!ok) {
    char masname[4096];
    int len=strlen(filename);
    strncpy(masname,filename,len-3);
    strcat(masname,"mas");
    fprintf(stderr,"Problems while reading XD master file! [%s]\n",masname);
    return false;
  }
  emit bigmessage(filename);
  while (!feof(f)){
    if (winformat) {
      readsize = fread(&wr[nr],sizeof(reco),1,f);
      lr[nr].ih=wr[nr].ih;
      lr[nr].ik=wr[nr].ik;
      lr[nr].il=wr[nr].il;
      lr[nr].d1=wr[nr].d1;
      lr[nr].d2=wr[nr].d2;
      lr[nr].d3=wr[nr].d3;
      lr[nr].d4=wr[nr].d4;
      lr[nr].d5=wr[nr].d5;
      lr[nr].d6=wr[nr].d6;
      lr[nr].d7=wr[nr].d7;
    }
    else readsize = fread(&lr[nr],sizeof(rec64),1,f);
    if (((lr[nr].ih|lr[nr].ik|lr[nr].il)!=0)&&(abs(lr[nr].ih)<130)&&(abs(lr[nr].ik)<130)&&(abs(lr[nr].il)<130)) nr++;
    if (nr>=LM) {
      fprintf(stderr,"to many reflections in xd.fou file\n");
      return false;
    }
  }
  fclose(f);
  for (int i=0;i<nr;i++){
    double u=lr[i].ih,v=lr[i].ik,w=lr[i].il;
    int mh=lr[i].ih,mk=lr[i].ik,ml=lr[i].il;
    double p,q=lr[i].d3;
    lr[i].d3=fmod(4*M_PI+q,2*M_PI);
    for (int k=0; k<ns; k++){
      int nh,nk,nl;
      double t=1.0;
      nh=(int) (u*sy[0][k]+ v*sy[3][k] + w*sy[6][k]);
      nk=(int) (u*sy[1][k]+ v*sy[4][k] + w*sy[7][k]);
      nl=(int) (u*sy[2][k]+ v*sy[5][k] + w*sy[8][k]);
      if((nl<0)||((nl==0)&&(nk<0))||((nl==0)&&(nk==0)&&(nh<0)))
      {nh*=-1;nk*=-1;nl*=-1;t=-1.0;}
      if ((nl<ml)||((nl==ml)&&(nk<mk))||((nl==ml)&&(nk==mk)&&(nh<=mh))) continue;
      mh=nh;mk=nk;ml=nl;
      p=u*sy[9][k]+v*sy[10][k]+w*sy[11][k];
      lr[i].d3=fmod(4*M_PI+t*fmod(q-2*M_PI*p,2*M_PI)-0.01,2*M_PI)+0.01;

    }
    lr[i].ih=mh;
    lr[i].ik=mk;
    lr[i].il=ml;
  }
  sorthkl(nr,lr);
  int n=-1;
  {int i=0;
    while(i<nr){
      double t=0.;
      double u=0.;
      double v=0.;
      double z=0.;
      double y=0.;
      double p=0.;
      int m;
      int k=i;
      while ((i<nr)&&(lr[i].ih==lr[k].ih)&&(lr[i].ik==lr[k].ik)&&(lr[i].il==lr[k].il)) {
	t=t+1.;
	u+=lr[i].d1;
	v+=1./(lr[i].d2*lr[i].d2);
	y=lr[i].d5;
	z+=lr[i].d4;
	p=lr[i].d3;
	i++;
      }
      m=n+1;
      lr[m].d1=fmax(0.,u/t);
      lr[m].d2=sqrt(1./v);
      lr[m].d5=y;
      lr[m].d4=z/t;
      lr[m].d3=p;
      n=m;
      lr[n].ih=lr[k].ih;
      lr[n].ik=lr[k].ik;
      lr[n].il=lr[k].il;
    }
  }
  n++;
  nr=n;
  {
    float DX;
    float DY;
    float DZ;	


    {
      int mh=0, mk=0, ml=0,j;
      for (int n=0; n<nr; n++){
	double u=lr[n].ih,v=lr[n].ik,w=lr[n].il;
	double a,b,c;
	for (int k=0; k<ns;k++){
	  a=abs((int)(u*sy[0][k]+v*sy[3][k]+w*sy[6][k]));
	  b=abs((int)(u*sy[1][k]+v*sy[4][k]+w*sy[7][k]));
	  c=abs((int)(u*sy[2][k]+v*sy[5][k]+w*sy[8][k]));
	  mh=(mh<a)?a:mh;
	  mk=(mk<b)?b:mk;
	  ml=(ml<c)?c:ml;
	}
      }
      printf("max hkl = %d %d %d\n",mh,mk,ml);
      j=(int)(rr*mh+.5);
      for (int i=0; it[i]< j; i++)n1=it[i+1];
      j=(int)(rr*mk+.5);
      for (int i=0; it[i]< j; i++)n2=it[i+1];
      j=(int)(rr*ml+.5);
      for (int i=0; (it[i]< j)||((nc)&&(it[i]%2)); i++) n3=it[i+1];
      n4=n2*n1;
      n5=n3*n4;
      datfo=(float*) malloc(sizeof(float)*n5);
      datfo_fc=(float*) malloc(sizeof(float)*n5);
      datf1_f2=(float*) malloc(sizeof(float)*n5);
      DX=1.0/n1;
      DY=1.0/n2;
      DZ=1.0/n3;
    } 
    for (int typ=0; typ<3;typ++){
      B=(fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*n5);
      for (int i=0; i<n5; i++){B[i][0]=0;B[i][1]=0;}
      for (int i=0; i<nr;i++){
	float fmod1 = sqrt(lr[i].d4*lr[i].d4+lr[i].d5*lr[i].d5);
	float a12=lr[i].d4-lr[i].d6;
	float b12=lr[i].d5-lr[i].d7;
	float f12=sqrt(a12*a12+b12*b12);
	b12=(b12<0)?-1:1;
	float phi12=(f12!=0)?b12*acosf(a12/f12):0;
	float  u,v,w;
	u=lr[i].ih;
	v=lr[i].ik;
	w=lr[i].il;
	float  ss,s=0,t=0,q,p;
	for (int n=0; n<ns;n++){
	  int j,k,l;
	  j=(int) (u*sy[0][n]+ v*sy[3][n] + w*sy[6][n]);
	  k=(int) (u*sy[1][n]+ v*sy[4][n] + w*sy[7][n]);
	  l=(int) (u*sy[2][n]+ v*sy[5][n] + w*sy[8][n]);
	  if((abs(j-lr[i].ih)+abs(k-lr[i].ik)+abs(l-lr[i].il))==0)s+=1.0;
	  if(abs(j+lr[i].ih)+abs(k+lr[i].ik)+abs(l+lr[i].il)==0)t+=1.0;
	}
	if(typ==0) ss=(lr[i].d1-fmod1)/(C[14]*(s+t));
	else if (typ==1) ss=(lr[i].d1)/(C[14]*(s+t));
	else ss=f12/(C[14]*(s+t));
	if(fmod1>1.E-6) ss=ss/(1.+rw*pow(lr[i].d2/fmod1,4));
	for (int n=0; n<ns;n++){
	  int j,k,l,m;
	  j=(int) (u*sy[0][n]+ v*sy[3][n] + w*sy[6][n]);
	  k=(int) (u*sy[1][n]+ v*sy[4][n] + w*sy[7][n]);
	  l=(int) (u*sy[2][n]+ v*sy[5][n] + w*sy[8][n]);
	  if (typ==2) q=(phi12-2*M_PI*(u*sy[9][n]+v*sy[10][n]+w*sy[11][n]))-M_PI*(j*DX+k*DY+l*DZ);
	  else q=(lr[i].d3-2*M_PI*(u*sy[9][n]+v*sy[10][n]+w*sy[11][n]))-M_PI*(j*DX+k*DY+l*DZ);
	  j=(999*n1+j)%n1;
	  k=(999*n2+k)%n2;
	  l=(999*n3+l)%n3;
	  m=j+n1*(k+n2*l);
	  p=ss*cosf(q);
	  B[m][0]=p;
	  q=ss*sinf(q);
	  B[m][1]=q;
	  j*=-1;
	  if(j<0)j=n1+j;
	  k*=-1;
	  if(k<0)k=n2+k;
	  l*=-1;
	  if(l<0)l=n3+l;
	  m=j+n1*(k+n2*l);
	  B[m][0]=p;
	  B[m][1]=-q;
	}
      }
      fprintf(stderr,"Starting Fourier %d %d %d!\n",n1,n2,n3);
      fwd_plan = fftwf_plan_dft_3d(n3,n2,n1,B,B,FFTW_FORWARD,FFTW_ESTIMATE);
      fftwf_execute(fwd_plan);
      fftwf_destroy_plan(fwd_plan);
      float t=0;
      double DM=0.,  DS=0., DD  ;
      for (int i=0; i<n5;i++){
	DD=B[i][0];
	DM+=DD;
	DS+=DD*DD;
	if (typ==0) datfo_fc[i]=B[i][0];
	else if (typ==1) datfo[i]=B[i][0];
	else datf1_f2[i]=B[i][0];
      }
      sigma[typ]=t=sqrt((DS/n5)-((DM/n5)*(DM/n5)));
      fftwf_free(B);
      fprintf(stderr,"Finished! sigma %g %g %g\n",t,DS,DM);
    }//1
  }//2
  extern QList<INP> xdinp;      
  urs=V3(0,0,0);int gt=0;
  for (int i=0; i<xdinp.size();i++) {
    urs+=xdinp.at(i).frac;
    gt++;
  }
  urs*=1.0/gt;
  urs=V3(1,1,1)-1.0*urs;
  mole->frac2kart(urs,urs);
  //    printf("ursprung %g %g %g \n",urs.x,urs.y,urs.z);
  nodex= (FNode*)malloc(sizeof(FNode)*n5);
  nodey= (FNode*)malloc(sizeof(FNode)*n5);
  nodez= (FNode*)malloc(sizeof(FNode)*n5);
  for (int o=0; o<n5;o++){
    nodex[o].flag=0;
    nodey[o].flag=0;
    nodez[o].flag=0;
  }
  dx=V3(1.0/(n1),0,0);
  dy=V3(0,1.0/(n2),0);
  dz=V3(0,0,1.0/(n3));
  mole->frac2kart(dx,dx); 
  mole->frac2kart(dy,dy); 
  mole->frac2kart(dz,dz);     
  delDA[ 0]=  -n1*dx -n2*dy -n3*dz;//nx ny,nz??
  delDA[ 1]=         -n2*dy -n3*dz;
  delDA[ 2]=   n1*dx -n2*dy -n3*dz;
  delDA[ 3]=  -n1*dx        -n3*dz;
  delDA[ 4]=                -n3*dz;
  delDA[ 5]=   n1*dx        -n3*dz;
  delDA[ 6]=  -n1*dx +n2*dy -n3*dz;
  delDA[ 7]=          n2*dy -n3*dz;
  delDA[ 8]=   n1*dx +n2*dy -n3*dz;
  delDA[ 9]=  -n1*dx -n2*dy       ;
  delDA[10]=         -n2*dy       ;
  delDA[11]=   n1*dx -n2*dy       ;
  delDA[12]=  -n1*dx              ;
  delDA[13]=  V3(0,0,0)           ;
  delDA[14]=   n1*dx              ;
  delDA[15]=  -n1*dx +n2*dy       ;
  delDA[16]=          n2*dy       ;
  delDA[17]=   n1*dx +n2*dy       ;
  delDA[18]=  -n1*dx -n2*dy +n3*dz;
  delDA[19]=         -n2*dy +n3*dz;
  delDA[20]=   n1*dx -n2*dy +n3*dz;
  delDA[21]=  -n1*dx        +n3*dz;
  delDA[22]=                +n3*dz;
  delDA[23]=   n1*dx        +n3*dz;
  delDA[24]=  -n1*dx +n2*dy +n3*dz;
  delDA[25]=          n2*dy +n3*dz;
  delDA[26]=   n1*dx +n2*dy +n3*dz;
  gen_surface(neu);
  return true;
}

void FourMCQ::deleteLists(){
 for (int fac=0; fac<5; fac++){
    if ((chgl->foubas[fac])&&(glIsList(chgl->foubas[fac]))) {
      //printf("deleting list #%d\n",chgl->foubas[fac]);
      glDeleteLists(chgl->foubas[fac],1);
      chgl->foubas[fac]=0;
    }//else printf("NOT deleting list #%d %d\n",chgl->foubas[fac],glIsList(chgl->foubas[fac]));
 }
}

void FourMCQ::trimm(char s[]){
  char sc[409];
  int j=0;
  int len=strlen(s);
  strncpy(sc,s,400);
  for (int i=0; i<len; i++) if (!isspace(sc[i])) s[j++]=toupper(sc[i]);
  s[j]='\0';
}

void FourMCQ::deletes(char *s, int count){
  if ((s==NULL)||(count <1)||((size_t)count>strlen(s))) return;
  for (int i=0; i<count; i++) s[i]=' ';
  trimm(s);
}

int FourMCQ::readMas(const char *filename){
  FILE *f;
  char masname[4096];
  int len=strlen(filename);
  char line[122],*dum;
  //size_t zlen=120;
  int ok=0;
  int i;double T,V;
  strncpy(masname,filename,len-4);
  masname[len-4]='\0';
  strcat(masname,".mas");
  f=fopen(masname,"r");
  if (f==NULL) return 0;
  ns=0;
  sy[0][ns]=1.0;
  sy[1][ns]=0.0;
  sy[2][ns]=0.0;

  sy[3][ns]=0.0;
  sy[4][ns]=1.0;
  sy[5][ns]=0.0;

  sy[6][ns]=0.0;
  sy[7][ns]=0.0;
  sy[8][ns]=1.0;

  sy[9][ns]=0.0;
  sy[10][ns]=0.0;
  sy[11][ns]=0.0;
  ns=1;
  do{
    dum=fgets(line,120,f);
    if (!strncmp(line,"TITLE",5)) {
      sscanf(line,"TITLE %[^!\r\n]",titl);
      trimm(titl);
    }
    if (!strncmp(line,"CELL ",5)) {
      sscanf(line,"CELL %lf %lf %lf %lf %lf %lf",&C[0],&C[1],&C[2],&C[3],&C[4],&C[5]);

      for (i=0;i<3;i++){
	if (C[i]<0.1) return 0;
	T=.0174533*C[i+3];
	if (T<0.001) return 0;
	D[i]=sin(T);
	D[i+3]=cos(T);
	C[i+6]=(D[i]/(C[i]*C[i]));
      } 
      V=1.-D[3]*D[3]-D[4]*D[4]-D[5]*D[4]+2.*D[3]*D[4]*D[5];
      C[6]/=V;
      C[7]/=V;
      C[8]/=V;
      C[9]= 2.*sqrt(C[7]*C[8])*(D[4]*D[5]-D[3])/(D[2]*D[2]);
      C[10]=2.*sqrt(C[6]*C[8])*(D[3]*D[5]-D[4])/(D[0]*D[2]);
      C[11]=2.*sqrt(C[6]*C[7])*(D[3]*D[4]-D[5])/(D[0]*D[1]);
      C[14]=C[1]*C[2]*C[0]*sqrt(V);
      D[6]=C[1]*C[2]*D[0]/C[14];
      D[7]=C[0]*C[2]*D[1]/C[14];
      D[8]=C[0]*C[1]*D[2]/C[14];
    }
    if (!strncmp(line,"WAVE",4)){
      sscanf(line,"WAVE %lf",&wave);
     /* printf("wavelength = %g\n",wave);*/
    }
    if (!strncmp(line,"LATT",4)){
      sscanf(line,"LATT %1c %1c",&cen,&git);
      nc=(cen=='C')?1:0;
//      printf("Structure is %scentrosymmetric.\n",(nc)?"":"non-");
    }
    if (!strncmp(line,"SYMM",4)){
      char s1[50],s2[50],s3[50];
      char *kill,*nom,*div ;
      sscanf(line,"SYMM %[^,],%[^,],%s",s1,s2,s3);
      trimm(s1);
      trimm(s2);
      trimm(s3);
      sy[0][ns]=(NULL!=(kill=strstr(s1,"-X")))?-1:(NULL!=(kill=strstr(s1,"X")))?+1:0;
      if (kill!=NULL) ((kill[0]=='-')||(kill[-1]=='+'))?deletes(kill,2):deletes(kill,1);
      sy[1][ns]=(NULL!=(kill=strstr(s1,"-Y")))?-1:(NULL!=(kill=strstr(s1,"Y")))?+1:0;
      if (kill!=NULL) ((kill[0]=='-')||(kill[-1]=='+'))?deletes(kill,2):deletes(kill,1);
      sy[2][ns]=(NULL!=(kill=strstr(s1,"-Z")))?-1:(NULL!=(kill=strstr(s1,"Z")))?+1:0;
      if (kill!=NULL) ((kill[0]=='-')||(kill[-1]=='+'))?deletes(kill,2):deletes(kill,1);

      sy[3][ns]=(NULL!=(kill=strstr(s2,"-X")))?-1:(NULL!=(kill=strstr(s2,"X")))?+1:0;
      if (kill!=NULL) ((kill[0]=='-')||(kill[-1]=='+'))?deletes(kill,2):deletes(kill,1);
      sy[4][ns]=(NULL!=(kill=strstr(s2,"-Y")))?-1:(NULL!=(kill=strstr(s2,"Y")))?+1:0;
      if (kill!=NULL) ((kill[0]=='-')||(kill[-1]=='+'))?deletes(kill,2):deletes(kill,1);
      sy[5][ns]=(NULL!=(kill=strstr(s2,"-Z")))?-1:(NULL!=(kill=strstr(s2,"Z")))?+1:0;
      if (kill!=NULL) ((kill[0]=='-')||(kill[-1]=='+'))?deletes(kill,2):deletes(kill,1);

      sy[6][ns]=(NULL!=(kill=strstr(s3,"-X")))?-1:(NULL!=(kill=strstr(s3,"X")))?+1:0;
      if (kill!=NULL) ((kill[0]=='-')||(kill[-1]=='+'))?deletes(kill,2):deletes(kill,1);
      sy[7][ns]=(NULL!=(kill=strstr(s3,"-Y")))?-1:(NULL!=(kill=strstr(s3,"Y")))?+1:0;
      if (kill!=NULL) ((kill[0]=='-')||(kill[-1]=='+'))?deletes(kill,2):deletes(kill,1);
      sy[8][ns]=(NULL!=(kill=strstr(s3,"-Z")))?-1:(NULL!=(kill=strstr(s3,"Z")))?+1:0;
      if (kill!=NULL) ((kill[0]=='-')||(kill[-1]=='+'))?deletes(kill,2):deletes(kill,1);
      if (NULL!=(kill=strstr(s1,"/"))){
	kill[0]='\0';
	div=kill+1;
	nom=s1;
	sy[9][ns]=atof(nom)/atof(div);
      }else sy[9][ns]=atof(s1);
      if (NULL!=(kill=strstr(s2,"/"))){
	kill[0]='\0';
	div=kill+1;
	nom=s2;
	sy[10][ns]=atof(nom)/atof(div);
      }else sy[10][ns]=atof(s2);

      if (NULL!=(kill=strstr(s3,"/"))){
	kill[0]='\0';
	div=kill+1;
	nom=s3;
	sy[11][ns]=atof(nom)/atof(div);
      }else sy[11][ns]=atof(s3);
/*
      printf("%d\n%3g %3g %3g  %4g\n%3g %3g %3g  %4g\n%3g %3g %3g  %4g\n\n",ns+1
		      ,sy[0][ns]
		      ,sy[1][ns]
		      ,sy[2][ns]
		      ,sy[9][ns] 
		      ,sy[3][ns]
		      ,sy[4][ns]
		      ,sy[5][ns] 
		      ,sy[10][ns] 
		      ,sy[6][ns]
		      ,sy[7][ns]
		      ,sy[8][ns]
		      ,sy[11][ns] 
	    );*/
      ns++;

    }
    if (!strncmp(line,"BANK",4)) ok=1;
  }while(!ok);
  return 1;  
}

void FourMCQ::sorthkl(int nr, rec64 r[]){
  rec64 *hilf= (rec64*) malloc(sizeof(rec64)*nr);
  int i,j,k,nj,ni,spalte;int index[4096];
  for (spalte=0; spalte<3; spalte++){
    j=-999999;
    k=999999;
    switch (spalte) {
	    case 0: for (i=0; i<nr; i++){ j=(j<r[i].ih)?r[i].ih:j; k=(k>r[i].ih)?r[i].ih:k; } break;
	    case 1: for (i=0; i<nr; i++){ j=(j<r[i].ik)?r[i].ik:j; k=(k>r[i].ik)?r[i].ik:k; } break;
	    case 2: for (i=0; i<nr; i++){ j=(j<r[i].il)?r[i].il:j; k=(k>r[i].il)?r[i].il:k; } break;
    }
    nj=-k;
    ni=(nj+j+1);
    for (i=0; i<=ni; i++) index[i]=0;
    for (i=0; i<nr; i++){
      switch (spalte){
	      case 0: j=r[i].ih+nj; break;
	      case 1: j=r[i].ik+nj; break;
	      case 2: j=r[i].il+nj; break;
      }
      index[j]++;/*brauch ich das? -->JA!*/
      hilf[i].ih=r[i].ih;
      hilf[i].ik=r[i].ik;
      hilf[i].il=r[i].il;
      hilf[i].d1=r[i].d1;
      hilf[i].d2=r[i].d2;
      hilf[i].d3=r[i].d3;
      hilf[i].d4=r[i].d4;
      hilf[i].d5=r[i].d5;
      hilf[i].d6=r[i].d6;
      hilf[i].d7=r[i].d7;
    }/*/4*/
   j=0;
      for (i=0; i<ni; i++){
	k=j;
	j+=index[i];
	index[i]=k;     
      }/*/5*/
   for (i=0; i<nr;i++){
     switch (spalte) {
	     case 0: j=hilf[i].ih +nj;break;
	     case 1: j=hilf[i].ik +nj;break;
	     case 2: j=hilf[i].il +nj;break;
     }     
     index[j]++;   
     j=index[j]-1;
     r[j].ih=hilf[i].ih;
     r[j].ik=hilf[i].ik;
     r[j].il=hilf[i].il;
     r[j].d1=hilf[i].d1;
     r[j].d2=hilf[i].d2;
     r[j].d3=hilf[i].d3;
     r[j].d4=hilf[i].d4;
     r[j].d5=hilf[i].d5;
     r[j].d6=hilf[i].d6;
     r[j].d7=hilf[i].d7;
   }/*/6*/
  }/*/spalten*/
free(hilf);
}

void FourMCQ::bewegt(V3 nm){
  V3 v , alturs=urs;
  mole->kart2frac(nm,v);
  urs=V3(1,1,1)-1.0*v;
  mole->frac2kart(urs,urs);
  printf("moved? =>%s\n",(urs==alturs)?"no":"yes");

//  printf("ursprung %g %g %g \n",urs.x,urs.y,urs.z);
  if (urs==alturs) return;

  //balken->setMinimum(0);
  //balken->setMaximum(n3*6);
  //balken->show();
  //balkenstep=0;
  //chgl->pause=true;
  //scroller=true;
  //mode=0;
  //iso=difsig;
  gen_surface(false);
  //mode=1;
  //iso=fosig;
  //gen_surface();
  //balken->hide();
  //chgl->pause=false;
  //chgl->updateGL();

}

void FourMCQ::inimap(){//for screenies
  //int lsts=glIsList(chgl->mibas)+glIsList(chgl->mibas+1);
  //glDeleteLists(chgl->mibas,lsts);
  //balken->setMinimum(0);
  //balken->setMaximum(n3*6);
  //balken->show();
  //balkenstep=0;
  //chgl->pause=true;
  //scroller=false;
  //mode=0;
  //iso=difsig;
 // printf("inimap\n");
  deleteLists();
  gen_surface(false);
  //mode=1;
  //iso=fosig;
  //gen_surface();
  //balken->hide();
  //chgl->pause=false;
}
#include <omp.h>

void FourMCQ::gen_surface(bool neu,int imin,int imax){
  /*if (!((chgl->isFO())||(chgl->isDF()))) {
    chgl->warFaul();
    return ;
  }
  */
  disconnect(chgl,SIGNAL(diffscroll(int ,int )),0,0);
  disconnect(chgl,SIGNAL(neuemitte(V3)),0,0);
  disconnect(chgl,SIGNAL(inimibas()),0,0);
  /*
  if ((mode==0)&&(!scroller)) chgl->mibas=glGenLists(2);
  scroller=false;*/
 for (int fac=imin; fac<imax; fac++){
    if ((chgl->foubas[fac])&&(glIsList(chgl->foubas[fac]))) glDeleteLists(chgl->foubas[fac],1);
    if (!chgl->foubas[fac]) chgl->foubas[fac]=glGenLists(1);
    glNewList(chgl->foubas[fac], GL_COMPILE );
    glLineWidth(linwidth);
    switch (fac){
	    case 0:
                    glColor4d(0.0, 0.7, 0.0, lintrans);
                    if (neu) iso[1]=sigma[0]*2.7;
                    else iso[1]=fabs(iso[1]);
		    mtyp=1;
//		    printf("fac %d case %d %g %g\n",fac,mtyp,iso[1],sigma[0]);
		    break;
	    case 1:
                    glColor4d(0.8, 0.0, 0.0, lintrans);
                    if (neu) iso[1]=-sigma[0]*2.7;
                    else iso[1]=-fabs(iso[1]);
                    mtyp=1;
//		    printf("fac %d case %d %g %g\n",fac,mtyp,iso[1],sigma[0]);
		    break;
	    case 2:
                    glColor4d(0.0, 0.0, 1.0, lintrans);
                   if (neu) iso[0]=sigma[1]*1.2;
		    mtyp=0;
//		    printf("fac %d case %d %g %g\n",fac,mtyp,iso[0],sigma[1]);

		    break;
	    case 3:
                    glColor4d(0.0, 0.7, 0.9, lintrans);
                    if (neu) iso[2]=sigma[2]*1.7;
                    else iso[2]=fabs(iso[2]);
                    mtyp=2;
//		    printf("fac %d case %d %g %g\n",fac,mtyp,iso[2],sigma[2]);
		    break;
	    case 4:
                    glColor4d(0.9, 0.6, 0.0, lintrans);
                    if (neu) iso[2]=-sigma[2]*1.7;
                    else iso[2]=-fabs(iso[2]);
                    mtyp=2;
//		    printf("fac %d case %d %g %g\n",fac,mtyp,iso[2],sigma[2]);
		    break;
    }
    int chunk=3;
//    QTime t;
 //   t.start();
    //balken->setValue(balkenstep++);
    //printf("iso=%g e/A^3 %s %10.5f %10.5f %10.5f\n",iso,(!mode)?"difference map":"Fo-map",urs.x,urs.y,urs.z);
#pragma omp parallel shared(chunk)
    {
      int ix,iy,iz;
#pragma omp for schedule(dynamic,chunk) private(ix,iy) nowait
      for (iz=0; iz<n3;iz++){
	for (iy=0; iy<n2;iy++){
	  for (ix=0; ix<n1;ix++){
	    CalcVertex(ix,iy,iz);
	  }
	}
      }
    }//omp
   // balken->setValue(balkenstep+=n3);
 //   printf("...%d ms later. drawing to memory\n",t.elapsed());
    //t.start();
    //#pragma omp parallel
    {
      glPushMatrix();
      glScaled(chgl->L,chgl->L,chgl->L);
      int h,k,l;
      //#pragma omp for private(h,k,l)
      for ( l=0; l<n3;l++){
	for ( k=0; k<n2;k++){
	  for ( h=0; h<n1;h++){
	    MakeElement(h,k,l,n1,n4);
	  }
	}
//	balken->setValue(balkenstep++);
      }
      glPopMatrix();
   //   printf("...%d ms later. \n",t.elapsed());
    }//O M P
  glEndList();
  }
  QString info=QString("<b>Fo-Fc1 Map:</b><font color=green>%1 e&Aring;<sup>-3</sup></font>"
		  "<font color=red> %2 e&Aring;<sup>-3</sup> </font><br><font color=grey> Hint:  [%3 Scroll (up or down)] to change. </font><br>"
		  "<b>Fo-Map:</b><font color=blue>%4  e&Aring;<sup>-3</sup></font><br><font color=grey> Hint:  [%5 Scroll (up or down)] to change. </font><br>"
		  "<b>F1-Fc2 Map:</b><font color=orange>%6 e&Aring;<sup>-3</sup></font>"
		  "<font color=cyan> %7 e&Aring;<sup>-3</sup> </font><br><font color=grey> Hint:  [%8 Scroll (up or down)] to change. </font><br>")
	  .arg(-iso[1],6,'g',2)
	  .arg(iso[1],6,'g',2)
	  .arg(QKeySequence(Qt::ControlModifier).toString(QKeySequence::NativeText))
	  .arg(iso[0],6,'g',2)
	  .arg(QKeySequence(Qt::ShiftModifier).toString(QKeySequence::NativeText))
	  .arg(-iso[2],6,'g',2)
	  .arg(iso[2],6,'g',2)
	  .arg(QKeySequence(Qt::AltModifier).toString(QKeySequence::NativeText))
	  ;
  emit bigmessage(info);
  connect(chgl,SIGNAL(inimibas()),this,SLOT(inimap()));
  connect(chgl,SIGNAL(neuemitte(V3)),this, SLOT(bewegt(V3)));
  connect(chgl,SIGNAL(diffscroll(int ,int )),this,SLOT(change_iso(int ,int )));
  chgl->fofcact->setVisible(((chgl->foubas[0])&&(glIsList(chgl->foubas[0]))));
  chgl->foact->setVisible(((chgl->foubas[2])&&(glIsList(chgl->foubas[2]))));
  chgl->f1f2act->setVisible(((chgl->foubas[3])&&(glIsList(chgl->foubas[3]))));
}

void FourMCQ::change_iso(int numsteps,int diff){
  disconnect(chgl,SIGNAL(diffscroll(int ,int )),0,0);
  iso[diff]+=iso[diff]*numsteps/10.0;
  int mi=0,ma=5;
  switch (diff) {
	  case 0: 
		  mi=2;ma=3;
		  break;
	  case 1:
		  mi=0; ma=2;
		  break;
	  case 2:
		  mi=3;ma=5;
		  break;
  
  }
  gen_surface(false,mi,ma);
  chgl->updateGL();
}

void FourMCQ::CalcVertex( int ix, int iy, int iz) {
  V3 mdz=(0.5*dx)+(0.5*dy)+(0.5*dz);
  V3 o,fl;
  double vo, vx=0,vy=0,vz=0;

  //nodex[(ix+n1*(iy+n2*iz))%n5].index=0;
  //nodey[(ix+n1*(iy+n2*iz))%n5].index=0;
  //nodez[(ix+n1*(iy+n2*iz))%n5].index=0;
  nodex[(ix+n1*(iy+n2*iz))%n5].flag=0;
  nodey[(ix+n1*(iy+n2*iz))%n5].flag=0;
  nodez[(ix+n1*(iy+n2*iz))%n5].flag=0;
  if (mtyp==0){//*datfo,*datfo_fc,*datf1_f2
    vo = datfo[ (ix+n1*iy+n2*n1*iz                  )%n5]   -iso[mtyp];
    vx = datfo[ (((ix+1)%n1) + n1* iy    + n2*n1 *  iz   )%n5]   -iso[mtyp];
    vy = datfo[ ( ix +    n1*((iy+1)%n2)+ n2*n1 *  iz   )%n5]   -iso[mtyp];
    vz = datfo[ ( ix +    n1* iy    + n2*n1 * ((iz+1)%n3))%n5]   -iso[mtyp];
  }else if (mtyp==1){
    vo = datfo_fc[(ix+n1*iy+n2*n1*iz                  )%n5]   -iso[mtyp];
    vx = datfo_fc[(((ix+1)%n1) + n1* iy    + n2*n1 *  iz   )%n5]   -iso[mtyp];
    vy = datfo_fc[( ix +    n1*((iy+1)%n2) + n2*n1 *  iz   )%n5]   -iso[mtyp];
    vz = datfo_fc[( ix +    n1* iy    + n2*n1 * ((iz+1)%n3))%n5]   -iso[mtyp];
  }else{
    vo = datf1_f2[(ix+n1*iy+n2*n1*iz                  )%n5]   -iso[mtyp];
    vx = datf1_f2[(((ix+1)%n1) + n1* iy    + n2*n1 *  iz   )%n5]   -iso[mtyp];
    vy = datf1_f2[( ix +    n1*((iy+1)%n2) + n2*n1 *  iz   )%n5]   -iso[mtyp];
    vz = datf1_f2[( ix +    n1* iy    + n2*n1 * ((iz+1)%n3))%n5]   -iso[mtyp];
  }

  if (Intersect(vo,vx)) {
    o=dx*((vo/(vo-vx))+ix) + dy*iy + dz*iz+urs;
    mole->kart2frac(o,o);
    o+=V3(-0.5,-0.5,-0.5);
    fl=V3(floor(o.x),floor(o.y),floor(o.z));
    o+=-1.0*fl;
    o+=V3(0.5,0.5,0.5);
    mole->frac2kart(o,o);
    o+=-1.0*urs;
    o+=mdz;
//    orte.append(o);
    nodex[ix+n1*iy+n2*n1*iz].vertex=o;
    nodex[ix+n1*iy+n2*n1*iz].flag=1;
  }
  if (Intersect(vo,vy)) {
    o=dx*ix + dy*((vo/(vo-vy))+iy) + dz*iz+urs;
    mole->kart2frac(o,o);
    o+=V3(-0.5,-0.5,-0.5);
    fl=V3(floor(o.x),floor(o.y),floor(o.z));
    o+=-1.0*fl;
    o+=V3(0.5,0.5,0.5);
    mole->frac2kart(o,o);
    o+=-1.0*urs;
    o+=mdz;
//    orte.append(o);
    nodey[ix+n1*iy+n2*n1*iz].vertex=o;
    nodey[ix+n1*iy+n2*n1*iz].flag=1;
  }
  if (Intersect(vo,vz)) {
    o=dx*ix + dy*iy + dz*((vo/(vo-vz))+iz)+urs;
    mole->kart2frac(o,o);
    o+=V3(-0.5,-0.5,-0.5);
    fl=V3(floor(o.x),floor(o.y),floor(o.z));
    o+=-1.0*fl;
    o+=V3(0.5,0.5,0.5);
    mole->frac2kart(o,o);
    o+=-1.0*urs;
    o+=mdz;
//    orte.append(o);
    nodez[ix+n1*iy+n2*n1*iz].vertex=o;
    nodez[ix+n1*iy+n2*n1*iz].flag=1;
  }
}

V3& FourMCQ::VectorSelected( FNode& node0, FNode& node1, FNode& node2, FNode& node3 ) {
  if( node1 && node2 && node3 ){
    GLfloat d1 = Distance( node0.vertex, node1.vertex ) +
            Distance( node3.vertex, node2.vertex );
    GLfloat d2 = Distance( node0.vertex, node2.vertex ) +
            Distance( node3.vertex, node1.vertex );
    if( d1 > d2 ) return node2.vertex; else return node1.vertex;
  }else{
    if(      node1 )   return node1.vertex;
    else if( node2 )   return node2.vertex;
    else if( node3 )   return node3.vertex;
  }
  return node0.vertex;
}

void FourMCQ::makeFaces(int n, FNode poly[] ){

  if (n<3) return;  //weniger als 3 verts -> nichts zu tun
  V3 mid_ver = V3(0,0,0);
  for(int i=0; i<n; i++ ){
    mid_ver += poly[i].vertex;
  }
  mid_ver *= (1.0/n);

  V3 mit=V3(1,1,1);
  mole->frac2kart(mit,mit);
  for (int w=0; w<27; w++){
    if (maptrunc==0){w=13;}
    else if ((maptrunc==1)&&(Distance(mit-urs,mid_ver+delDA[w])>(map_radius*map_radius))) continue;
    else if (maptrunc==2) {
      double maexle=2;
      extern QList<INP> xdinp;      
      for (int g=0; g<xdinp.size();g++){
        maexle=qMin(maexle,Distance(mid_ver+delDA[w],xdinp.at(g).kart));
      }
      if (maexle==2) continue;
    }
    if (n==3){
      glBegin(GL_TRIANGLES);
      for (int k=0; k<n;k++){
        glVertex3f( poly[k].vertex.x+delDA[w].x,
                        poly[k].vertex.y+delDA[w].y,
                        poly[k].vertex.z+delDA[w].z);
      }
      glEnd();
    }else{
      glBegin(GL_TRIANGLE_FAN);
      glVertex3f( mid_ver.x+delDA[w].x,mid_ver.y+delDA[w].y,mid_ver.z+delDA[w].z);
      for (int k=0; k<n;k++){
        glVertex3f(     poly[k%n].vertex.x+delDA[w].x,
                        poly[k%n].vertex.y+delDA[w].y,
                        poly[k%n].vertex.z+delDA[w].z);
      }
      glVertex3f(     poly[0].vertex.x+delDA[w].x,
                      poly[0].vertex.y+delDA[w].y,
                      poly[0].vertex.z+delDA[w].z);
      glEnd();
    }
    if (maptrunc==0) w=27;
  }
}//omp

int FourMCQ::IndexSelected( FNode& node0, FNode& node1, FNode& node2, FNode& node3 ) {
  if( node1 && node2 && node3 ){
    GLfloat d1 = Distance( node0.vertex, node1.vertex) +
            Distance( node3.vertex, node2.vertex) ;
    GLfloat d2 = Distance( node0.vertex, node2.vertex ) +
            Distance( node3.vertex, node1.vertex );
    if( d1 > d2 ) return 2; else return 1;
  }else{
    if(      node1 )   return 1;
    else if( node2 )   return 2;
    else if( node3 )   return 3;
  }
  return 0;

}

void FourMCQ::MakeElement( int ix, int iy, int iz ,int s1, int s2) {//das ist der Teil des japanischen Programms den ich nicht verstehe.
  //Hauptsache fuktioniert.
 // printf("ich bin fred #%d\n",omp_get_thread_num());
  int conn[12][2][4] = {           //char->int wegen warning g++ >3.0
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
  FNode node[12];
  FNode polygon[12];
  node[ 0] = nodex[(ix+iy*s1+iz*s2)        %n5];        // 000x
  node[ 1] = nodey[(ix+iy*s1+iz*s2)        %n5];        // 000y
  node[ 2] = nodez[(ix+iy*s1+iz*s2)        %n5];        // 000z
  node[ 3] = nodex[(ix+iy*s1+((iz+1)%n3)*s2)    %n5];    // 001y
  node[ 4] = nodey[(ix+iy*s1+((iz+1)%n3)*s2)    %n5];    // 001z
  node[ 5] = nodez[(ix+((iy+1)%n2)*s1+iz*s2)    %n5];    // 010x
  node[ 6] = nodex[(ix+((iy+1)%n2)*s1+iz*s2)    %n5];    // 010y
  node[ 7] = nodey[(((1+ix)%n1)+iy*s1+iz*s2)      %n5];      // 100y
  node[ 8] = nodez[(((1+ix)%n1)+iy*s1+iz*s2)      %n5];      // 100z
  node[ 9] = nodex[(ix+((iy+1)%n2)*s1+((iz+1)%n3)*s2)%n5];// 011x
  node[10] = nodey[(((ix+1)%n1)+iy*s1+((iz+1)%n3)*s2)%n5];// 101y
  node[11] = nodez[(((ix+1)%n1)+((iy+1)%n2)*s1+iz*s2)%n5];// 110z
  if (((char)node[0]+node[1]+node[2]+node[3]+node[4]+node[5]
                          +node[6]+node[7]+node[8]+node[9]+node[10]+node[11])==0) return;
  for( int is=0; is<12; is++ ) {
    if( !node[is] ) continue;

    int n=0, i=is, m=0,ai=i;
    GLfloat dis;
    dis=0;
    do {
      polygon[n++]= node[i];
      int sol = IndexSelected(
                      node[conn[i][m][0]],
                      node[conn[i][m][1]],
                      node[conn[i][m][2]],
                      node[conn[i][m][3]]);
      ai=i;
      i = conn[i][m][sol];
      if( sol == 2 ) m ^= 1;
      dis+=Distance(polygon[0].vertex,node[i].vertex);
        node[i].flag= 0;
    }while( (i!=is)&&(n<11) );
    if (n>=3) {
      if (dis<5)
      makeFaces( n, polygon );
      else {
        int axe=0;
        double delx=0,dely=0,delz=0;
        double mind=100000000;
        V3 minp=V3(10000,10000,10000),lihiun=V3(-10000,-10000,-10000);
        int minii=0;
        for (int polni=1; polni<=n; polni++){
            delx+=fabs(polygon[polni-1].vertex.x - polygon[polni%n].vertex.x);
            dely+=fabs(polygon[polni-1].vertex.y - polygon[polni%n].vertex.y);
            delz+=fabs(polygon[polni-1].vertex.z - polygon[polni%n].vertex.z);
            if (Distance(polygon[polni%n].vertex,lihiun)<mind) {
              mind=Distance(polygon[polni%n].vertex,minp);
              minii=polni%n;
            }
        }
        minp=polygon[minii].vertex;
        axe|=(delx>1)?1:0;
        axe|=(dely>1)?2:0;
        axe|=(delz>1)?4:0;
        for (int polni=0; polni<n; polni++){
          V3 neo=polygon[polni].vertex;
          double lang =Distance(minp,neo);
          if ((lang>Distance(minp,neo+dx*n1))&&(axe&1)) neo+=dx*n1;
          else if ((lang>Distance(minp,neo-dx*n1))&&(axe&1)) neo+=-n1*dx;
          lang =Distance(minp,neo);
          if ((lang>Distance(minp,neo+dy*n2))&&(axe&2)) neo+=n2*dy;
          else if ((lang>Distance(minp,neo-dy*n2))&&(axe&2)) neo+=-n2*dy;
          lang =Distance(minp,neo);
          if ((lang>Distance(minp,neo+n3*dz))&&(axe&4)) neo+=n3*dz;
          else if ((lang>Distance(minp,neo-n3*dz))&&(axe&4)) neo+=-n3*dz;
          polygon[polni].vertex=neo;
        }
        dis=0;
        for (int polni=1; polni<=n; polni++){
            dis+=Distance(polygon[polni-1].vertex , polygon[polni%n].vertex);
        }
      if (dis<5)
        makeFaces( n, polygon );
      }
    }
  }

}
