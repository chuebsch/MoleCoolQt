#include "fourmcq.h"

FourMCQ::FourMCQ(const char filename[], int kind, double resol, double wght){
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
  nr=0;
  nc=0;
  jm= kind;
  rr=resol;
  rw=wght;
  int winformat=0,ok;
  FILE *f;
  f=fopen(filename,"rb");
  fseek (f , 0 , SEEK_END);
  size_t lSize = ftell (f);
  rewind (f);
  if (!(lSize%sizeof(reco))&&(lSize%sizeof(rec64)))winformat=1;
  ok= readMas(filename);
  if (!ok) {
    char masname[4096];
    int len=strlen(filename);
    strncpy(masname,filename,len-3);
    strcat(masname,"mas");
    fprintf(stderr,"Problems while reading XD master file! [%s]\n",masname);
    exit(1);
  }
  while (!feof(f)){
    if (winformat) {
      fread(&wr[nr],sizeof(reco),1,f);
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
    else fread(&lr[nr],sizeof(rec64),1,f);
    if ((lr[nr].ih|lr[nr].ik|lr[nr].il)!=0) nr++;
    if (nr>=LM) {fprintf(stderr,"to many reflections in xd.fou file\n");exit(0); }
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
    j=(int)(rr*mh+.5);
    for (int i=0; it[i]< j; i++)n1=it[i+1];
    j=(int)(rr*mk+.5);
    for (int i=0; it[i]< j; i++)n2=it[i+1];
    j=(int)(rr*ml+.5);
    for (int i=0; (it[i]< j)||((nc)&&(it[i]%2)); i++) n3=it[i+1];
    n4=n2*n1;
    n5=n3*n4;
    B=(fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*n5);
    dx=1.0/n1;
    dy=1.0/n2;
    dz=1.0/n3;
  }
  for (int i=0; i<nr;i++){
    float fmod1 = sqrt(lr[i].d4*lr[i].d4+lr[i].d5*lr[i].d5);
    float  u=lr[i].ih,v=lr[i].ik,w=lr[i].il;
    float  s=0,t=0,q,p;
    for (int n=0; n<ns;n++){
      int j,k,l;
      j=(int) (u*sy[0][n]+ v*sy[3][n] + w*sy[6][n]);
      k=(int) (u*sy[1][n]+ v*sy[4][n] + w*sy[7][n]);
      l=(int) (u*sy[2][n]+ v*sy[5][n] + w*sy[8][n]);
      if((abs(j-lr[i].ih)+abs(k-lr[i].ik)+abs(l-lr[i].il))==0)s+=1.0;
      if(abs(j+lr[i].ih)+abs(k+lr[i].ik)+abs(l+lr[i].il)==0)t+=1.0;
    }
    if(jm==0) s=(lr[i].d1-fmod1)/(C[14]*(s+t));
    else s=(lr[i].d1*jm-fmod1*(jm-1))/(C[14]*(s+t));

    if(fmod1>1.E-6) s=s/(1.+rw*pow(lr[i].d2/fmod1,4));
    for (int n=0; n<ns;n++){ 
      int j,k,l,m;
      j=(int) (u*sy[0][n]+ v*sy[3][n] + w*sy[6][n]);
      k=(int) (u*sy[1][n]+ v*sy[4][n] + w*sy[7][n]);
      l=(int) (u*sy[2][n]+ v*sy[5][n] + w*sy[8][n]);
      q=(lr[i].d3-2*M_PI*(u*sy[9][n]+v*sy[10][n]+w*sy[11][n]))-M_PI*(j*dx+k*dy+l*dz);
      j=(999*n1+j)%n1;
      k=(999*n2+k)%n2;
      l=(999*n3+l)%n3;
      m=j+n1*(k+n2*l);
      p=s*cosf(q);
      B[m][0]=p;
      q=s*sinf(q);
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
  fprintf(stderr,"Starting Fourier!\n");
  fwd_plan = fftwf_plan_dft_3d(n3,n2,n1,B,B,FFTW_FORWARD,FFTW_ESTIMATE);
  fftwf_execute(fwd_plan);
  fftwf_destroy_plan(fwd_plan);
  float t=0; 
  double DM=0.,  DS=0., DD  ; 
  for (int i=0; i<n5;i++){
    DD=B[i][0];
    DM+=DD;
    DS+=DD*DD;
  }
  t=sqrt((DS/n5)-((DM/n5)*(DM/n5)));
  fprintf(stderr,"Finished!\n");  

}

FourMCQ::~FourMCQ(){
  fftwf_free(B);
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
  char line[122];
  size_t zlen=120;
  int ok=0;
  int i;double T,V;
  strncpy(masname,filename,len-3);
  strcat(masname,"mas");
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
    fgets(line,120,f);
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
  return 0;
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

}
