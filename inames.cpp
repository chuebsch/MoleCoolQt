#include "inames.h"
#include <QMessageBox>
#include <QFile>
 
int getOZ(string s){
  const char PSE_Symbol[109][3] = {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar",
                              "K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr",
                              "Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","J","Xe",
                              "Cs","Ba", "La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu",
                              "Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra",
                                  "Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No","Lr","Ku","Ha","Rf","Ns","Hs","Mt"};
  size_t p=s.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWabcdefghijklmnopqrstuvwxyz",1);
  s[0]=toupper(s[0]);
  for (int i=0;i<109;i++)
    if ((s.substr(0,p)==(PSE_Symbol[i]))) {
      return i+1;
    }
  return -1;
}

bool anso( const MyAtom &a, const MyAtom &b){
  return (a.an > b.an);
}

int quersumme(int zahl){
  int summe=zahl%10;//einer
  int kopie=zahl;
  while (kopie){
    kopie/=10;
    summe+=kopie%10;
  }
  return summe;
}

bool icmp (const QString &i,const QString &j) { 
  double d1,d2;
  char hash1,hash2;
  int ring1=0,ring2=0;
  stringstream str1,str2;
  string a1,a2;
  str1<<i.toStdString();
  if (i[0]=='#') {
    str1>>hash1>>ring1>>a1;
    ring1=quersumme(ring1);
    d1=1.5;
  }
  else if (i[0]=='@') {
    str1>>hash1>>ring1>>a1;
    ring1=quersumme(ring1);
    d1=1.0;
  }
  else	str1>>d1>>a1;
  str2<<j.toStdString();
  if (j[0]=='#') {
    str2>>hash2>>ring2>>a2;
    ring2=quersumme(ring2);
    d2=1.5;
  }
  else if (j[0]=='@') {
    str2>>hash2>>ring2>>a2;
    ring2=quersumme(ring2);
    d2=1.0;
  }
  else	str2>>d2>>a2;
  if (d1!=d2) return (d1>d2);//wenn bindungsgrade verschieden dann ist hier schon entschieden
  int o1,o2;
  o1=getOZ(a1);
  o2=getOZ(a2);
  if ((o1==o2)&&(ring1!=ring2)) return(ring1>ring2); 
  if ((o1==o2)&&(i!=j)) {
    int t1,t2;
    t1=i.indexOf(QRegExp("[A-Za-z]"));
    t1=i.indexOf(QRegExp("[123#@]"),t1);
    t2=j.indexOf(QRegExp("[A-Za-z]"));
    t2=j.indexOf(QRegExp("[123#@]"),t2);
    QString k,l;
    for (int w=t1;w<i.length();w++)k+=i[w];
    for (int w=t2;w<j.length();w++)l+=j[w];
    return icmp(k,l);//rekursion
  }
  return (o1>o2);
}

QString inames::botostr(int bo){
  QString str;
  switch(bo){
  case  1: str="1";break;
  case  2: str="1.5";break;
  case  3: str="2";break;
  case  4: str="3";break;
  default: str="Error!"; break;
  }
  return str;
}
   
int ringCodeSort(int rg){// aus ringlort.cc (Invariomtool)
  int stellen=(int)(log(rg)/log(10))+1;
  int *stelle=(int*)malloc(sizeof(int)*stellen);
  int c=rg;
  stelle[0]=c%10;
  for (int i=1; i<stellen; i++){
    c/=10;
    stelle[i]=c%10;
  }
 //bubble sort
 for (int i=0; i<stellen-1;i++)
   for (int j=0; j<stellen-1;j++)
   if (stelle[j]>stelle[j+1]){
     int q=stelle[j+1];
     stelle[j+1]=stelle[j];
     stelle[j]=q;
   }
 c=0;
 for (int i=0,j=1; i<stellen;i++,j*=10)
   c+=stelle[i]*j;
 return c;
}

QMap<MyAtom,long long int> ringCode;
QMap<MyAtom,long long int> ringID;
QMap<MyAtom,double> planarity;

double isPlanar(MyAtom core, Connection cl){
  double plan=1.0;
  V3 v0=core.pos;
  QList<V3> bv;
  QList<V3> xv;
  for (int i=0;i<cl.size();i++){
    if (*cl.at(i).ato2==core)
      bv.append(cl.at(i).ato1->pos-v0);
    else if(*cl.at(i).ato1==core)
      bv.append(cl.at(i).ato2->pos-v0);
  }
  for (int i=0; i<bv.size();i++){
  xv.append(Normalize( bv.at(i) % bv.at((i+1) % bv.size())));  
  }
  for (int i=0; i<xv.size();i++){
   plan*=fabs(xv.at(i) * xv.at((i+1) % xv.size()));
  }
  return (bv.size()>1)?plan:0;
}

  typedef struct Tripel{
    MyAtom t[3];
    bool taken;
  }Tripel;

V3 possibleRingCenter(const Tripel t){
  double w=molekul::winkel(t.t[1].pos-t.t[0].pos,t.t[2].pos-t.t[0].pos);
  double scal=sin(w/360.0*M_PI)*sin(w/360.0*M_PI)/(sin(w/180.0*M_PI)*sin(w/180.0*M_PI));
  V3 mx=t.t[0].pos+scal*(t.t[1].pos+t.t[2].pos-(t.t[0].pos*2.0));
  return mx;
}

void inames::ringlord(Connection cl){//
  /* DREIECK fuer DREIRING ALGO:
   *      A\
   *      | \b
   *      a   C
   *      |  c 
   *      B/
   */
  ringID.clear();
  planarity.clear();
  ringCode.clear();

  MyAtom const *A=NULL,*B=NULL, *C=NULL;
  long long int ID=1;
  Tripel tripel;
  QList<Tripel> tripels;
  tripels.clear();
  for (int a=0; a<cl.size();a++){
    if (!planarity.contains(*cl.at(a).ato1)) planarity[*cl.at(a).ato1] = isPlanar(*cl.at(a).ato1,cl);
    if (!planarity.contains(*cl.at(a).ato2)) planarity[*cl.at(a).ato2] = isPlanar(*cl.at(a).ato2,cl);
    for (int b=a+1; b<cl.size();b++){
      int da=0;
      if (cl.at(a).ato1==cl.at(b).ato1){
	A=cl.at(a).ato1;
	B=cl.at(a).ato2;
	C=cl.at(b).ato2;
	da++;
      }
      if (cl.at(a).ato2==cl.at(b).ato1){
	A=cl.at(a).ato2;
	B=cl.at(a).ato1;
	C=cl.at(b).ato2;
	da++;
      }
      if (cl.at(a).ato1==cl.at(b).ato2){
	A=cl.at(a).ato1;
	B=cl.at(a).ato2;
	C=cl.at(b).ato1;
	da++;
      }
      if (cl.at(a).ato2==cl.at(b).ato2){
	A=cl.at(a).ato2;
	B=cl.at(a).ato1;
	C=cl.at(b).ato1;
	da++;
      }
      if (da) {
	for (int c=b+1;c<cl.size();c++)
	  if (((cl.at(c).ato1==C)&&(cl.at(c).ato2==B))||((cl.at(c).ato2==C)&&(cl.at(c).ato1==B))){
	    ringCode[*A]=(ringCode[*A]*10) + 3;   
	    ringCode[*B]=(ringCode[*B]*10) + 3;   
	    ringCode[*C]=(ringCode[*C]*10) + 3;  
	    ringID[*A]|=ID; 
	    ringID[*B]|=ID; 
	    ringID[*C]|=ID; 
	    ID<<=1;
	    ID=(ID)?ID:1;
	  }
	tripel.t[0]=*A;
	tripel.t[1]=*B;
	tripel.t[2]=*C;
	tripel.taken=false;
	tripels.append(tripel);
      }
    }

  }
  
  //CastingAlgo::
  CEnvironment all=planarity.keys(),
	  firstCast;
  QList<Tripel> secondCast;
  for (int i=0; i<all.size(); i++) if (planarity[all.at(i)]>0.85) {
    firstCast.append(all.at(i));
  }
  for (int i=0; i<tripels.size(); i++) 
  if ((firstCast.contains(tripels.at(i).t[0]))&&(firstCast.contains(tripels.at(i).t[1]))&&(firstCast.contains(tripels.at(i).t[2]))) {
    secondCast.append(tripels.at(i));
  }
  QList<V3> mitten;
  QList<double> sollen;
  QList<int> dabei;
  for (int i=0; i <secondCast.size(); i++){
    mitten.append(possibleRingCenter(secondCast.at(i)));
  }
  for (int  i=0; i<mitten.size();i++){
    dabei.clear();
    dabei.append(i);
    for (int  j=i+1; j<mitten.size();j++){
      if ((!secondCast.at(j).taken)&&(0.3>Distance(mitten.at(i),mitten.at(j)))){
	dabei.append(j);

      }
    }
    if (dabei.size()>3){
      for (int k=0; k<dabei.size();k++){
	ringCode[secondCast.at(dabei.at(k)).t[0]]=(ringCode[secondCast.at(dabei.at(k)).t[0]]*10)+ dabei.size();
	ringID[secondCast.at(dabei.at(k)).t[0]]|=ID;

	secondCast[dabei.at(k)].taken=true;
      }
      ID<<=1; 
      ID=(ID)?ID:1;
    }
  }
 CEnvironment alleInRingen=ringCode.keys();
  for (int i=0; i<alleInRingen.size();i++){

    ringCode[alleInRingen.at(i)]=ringCodeSort(ringCode[alleInRingen.at(i)]);
  }
  alleInRingen.clear();
  /*
  for (int k=0; k<na;k++){
    xdinp[k].ringcode=0;
    xdinp[k].ringID=0; 
  }
  int ID=1;
  for (int i=0; i<na; i++)
    for (int j=0; j<knopf[i].Liganzahl; j++)
      for (int k=0; k<knopf[knopf[i].Lig[j]].Liganzahl; k++)
	for (int l=0; l<knopf[knopf[knopf[i].Lig[j]].Lig[k]].Liganzahl; l++)
	  if ((i<knopf[i].Lig[j])&&(knopf[i].Lig[j]<knopf[knopf[i].Lig[j]].Lig[k])&&(knopf[knopf[knopf[i].Lig[j]].Lig[k]].Lig[l]==i)){
	    xdinp[i].ringcode=                            10*xdinp[i].ringcode+3;
	    xdinp[knopf[i].Lig[j]].ringcode=              10*xdinp[knopf[i].Lig[j]].ringcode+3;
	    xdinp[knopf[knopf[i].Lig[j]].Lig[k]].ringcode=10*xdinp[knopf[knopf[i].Lig[j]].Lig[k]].ringcode+3;
	    xdinp[i].ringID|=ID;                            
	    xdinp[knopf[i].Lig[j]].ringID|=ID;              
	    xdinp[knopf[knopf[i].Lig[j]].Lig[k]].ringID|=ID;
	    ID<<=1;
	    printf("%s%s%s %d\n",xdinp[i].atomname, 
			    xdinp[knopf[i].Lig[j]].atomname,
			    xdinp[knopf[knopf[i].Lig[j]].Lig[k]].atomname,ID);
	  } 
*/

}

QString inames::invName(MyAtom core,Connection &cl, CEnvironment &sel ,int rung){
  if (rung==0) ringlord(cl); 
  CEnvironment erstesphaere;
  int tmp1;
  if (core.an<0) return QString("Not a real atom!"); 
  sel.clear();
  QString nom=core.Symbol;
  int OSollDoppel=-1;
  QVector<QString> arms;//Bildliche vorstellung: atom mit mehreren armen (bindungen mit nachbarn und ggf deren nachbarn dran)
  QString hand,digit; //wollte den schlechten gag von oben nur noch weiter treiben. (hand ist spaeter teil von arms )
  QVector<QString> fingers; //schenkelklopfer... uebernachste nachbarn... an der hand dran ...
  sel.append(core);
  erstesphaere.append(core);
  for (int i=0; i<cl.size();i++){//1.for
    if (core.pos==cl.at(i).ato2->pos){
      if (ringCode.contains(*cl.at(i).ato1)){

	hand=QString("%1%2%3").arg((ringID[*cl.at(i).ato1]&ringID[core])?"#":"@").arg(ringCode[*cl.at(i).ato1]).arg(cl.at(i).ato1->Symbol);
      }else{
	hand=botostr(cl.at(i).order);
	hand+=cl.at(i).ato1->Symbol;
      }
      if (!sel.contains(*cl.at(i).ato1)) {
	sel.append(*cl.at(i).ato1);
	erstesphaere.append(*cl.at(i).ato1);
      }

      tmp1=1;

    }
    else if (core.pos==cl.at(i).ato1->pos){
      if (ringCode.contains(*cl.at(i).ato2)){
	hand=QString("%1%2%3").arg((ringID[*cl.at(i).ato2]&ringID[core])?"#":"@").arg(ringCode[*cl.at(i).ato2]).arg(cl.at(i).ato2->Symbol);
      }else{
	hand=botostr(cl.at(i).order);
	hand+=cl.at(i).ato2->Symbol;
      }
      if (!sel.contains(*cl.at(i).ato2)) {
	sel.append(*cl.at(i).ato2);
	erstesphaere.append(*cl.at(i).ato2);
      }
      tmp1=2;
    } 
    else {
      continue;
    }
	if ((ringCode.contains(*cl.at(i).ato1))&&(core.pos==cl.at(i).ato2->pos)&&(!(ringID[*cl.at(i).ato1]&ringID[core]))&&(core.Symbol=="O")){ 
	  OSollDoppel=i;//O@ immer 2fach 
	}
	if ((ringCode.contains(*cl.at(i).ato2))&&(core.pos==cl.at(i).ato1->pos)&&(!(ringID[*cl.at(i).ato2]&ringID[core]))&&(core.Symbol=="O")) {
	  OSollDoppel=i;
	}
    if (
		    (hand.contains("#"))||
		    (!hand.contains("@")&&(2==cl.at(i).order))||
		    ((!hand.contains("@"))&&(core.Symbol=="H"))||
		    ((core.an<12)&&((cl.at(i).ato1->an>11)||(cl.at(i).ato2->an>11)))||
		    (((cl.at(i).ato1->an>11)&&(cl.at(i).ato2->an>11))))
    { //meso
      hand+="[";    
      fingers.clear();
      for (int j=0; j<cl.size();j++){//2.for
	if ((tmp1==1)&&(cl.at(i).ato1->pos==cl.at(j).ato2->pos)&&(!(cl.at(j).ato1->pos==core.pos))){	  
	  if (!sel.contains((*cl.at(j).ato1))) sel.append(*cl.at(j).ato1);
	  digit=(ringCode.contains(*cl.at(j).ato1))
		  ? QString("%1%2").arg((ringID[*cl.at(j).ato1]&ringID[*cl.at(i).ato1])?"#":"@").arg(ringCode[*cl.at(j).ato1])
		  : botostr(cl.at(j).order);
	  digit+=cl.at(j).ato1->Symbol;

	}
	else if ((tmp1==1)&&(cl.at(i).ato1->pos==cl.at(j).ato1->pos)&&(!(cl.at(j).ato2->pos==core.pos))){	  
	  if (!sel.contains((*cl.at(j).ato2))) sel.append(*cl.at(j).ato2);
	  digit=(ringCode.contains(*cl.at(j).ato2))
		  ? QString("%1%2").arg((ringID[*cl.at(j).ato2]&ringID[*cl.at(i).ato1])?"#":"@").arg(ringCode[*cl.at(j).ato2])
		  : botostr(cl.at(j).order);
	  digit+=cl.at(j).ato2->Symbol;

	}
	else if ((tmp1==2)&&(cl.at(i).ato2->pos==cl.at(j).ato2->pos)&&(!(cl.at(j).ato1->pos==core.pos))){	  
	  if (!sel.contains((*cl.at(j).ato1))) sel.append(*cl.at(j).ato1);
	  digit=(ringCode.contains(*cl.at(j).ato1))
		  ? QString("%1%2").arg((ringID[*cl.at(j).ato1]&ringID[*cl.at(i).ato2])?"#":"@").arg(ringCode[*cl.at(j).ato1])
		  : botostr(cl.at(j).order);
	  digit+=cl.at(j).ato1->Symbol;

	}
	else if ((tmp1==2)&&(cl.at(i).ato2->pos==cl.at(j).ato1->pos)&&(!(cl.at(j).ato2->pos==core.pos))){	  
	  if (!sel.contains((*cl.at(j).ato2))) sel.append(*cl.at(j).ato2);
	  digit=(ringCode.contains(*cl.at(j).ato2))
		  ? QString("%1%2").arg((ringID[*cl.at(j).ato2]&ringID[*cl.at(i).ato2])?"#":"@").arg(ringCode[*cl.at(j).ato2])
		  : botostr(cl.at(j).order);
	  digit+=cl.at(j).ato2->Symbol;

	} 
	else continue;

	fingers.push_back(digit);
      }

      qSort(fingers.begin(), fingers.end(), icmp);

      for (int j=0; j<fingers.size();j++){
	hand+=fingers.at(j);
      }
      hand+="]";
    }//meso
    arms.push_back(hand);
  }//1.for

  if ((erstesphaere.size()<3)&&(OSollDoppel>=0)) {
    cl[OSollDoppel].order=3;//O@ immer 2fach 
  }
  qSort(arms.begin(), arms.end(), icmp);
  for (int i=0;i<arms.size();i++)
    nom+=arms.at(i);
  int leerpos;
  while ((leerpos = nom.indexOf("[]"))!=-1) nom.remove(leerpos,2);
  nom=nom.toLower();  
  nom[0]=nom[0].toUpper();
  if ((sel.size()==5)&&(arms.size()==4)){
//    printf("arme %d finger %d  sel %d\n",arms.size(),fingers.size(),sel.size());
    CEnvironment chiral;
    if ((sel.at(1).an!=sel.at(2).an)&&
	(sel.at(1).an!=sel.at(3).an)&&
	(sel.at(1).an!=sel.at(4).an)&&
	(sel.at(2).an!=sel.at(3).an)&&
	(sel.at(2).an!=sel.at(4).an)&&
	(sel.at(3).an!=sel.at(4).an)){
      for (int w=1;w<5;w++)
	chiral.append(sel.at(w));
      qSort(chiral.begin(),chiral.end(),anso);
      double chiralitaet = ((chiral.at(0).pos-sel.at(0).pos)%(chiral.at(1).pos-sel.at(0).pos))*(chiral.at(2).pos-sel.at(0).pos);
      if (chiralitaet <0.0)//spatprodukt
	nom.prepend("R-");
      else
	nom.prepend("S-");
/*      printf("[%s]: %s %s %s (%s) %f\n",sel.at(0).Label.toStdString().c_str(),
		      chiral.at(0).Label.toStdString().c_str(),
		      chiral.at(1).Label.toStdString().c_str(),
		      chiral.at(2).Label.toStdString().c_str(),
		      chiral.at(3).Label.toStdString().c_str(),chiralitaet);*/
    }
  }
  if (ringCode.contains(core)) nom.prepend(QString("%1-").arg(ringCode[core]));
  sel.clear();
  sel=erstesphaere;
  if ((nom.startsWith("N@"))&&(planarity.value(core)>0.9)&&(arms.size()>=3)) nom.prepend("=-");
  return nom;
}
