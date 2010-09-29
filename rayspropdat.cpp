#include "rpd.h"
bool rpd::comp(VertBond a, VertBond b){
  return (a.l<b.l);
}

QList<VertBond> rpd::findBonds(QList<VertBond> bonds,int a){
  QList<VertBond> res;
  for (int i=0;i<bonds.size();i++){
    if ((bonds.at(i).a==a)||(bonds.at(i).e==a))
      res.append(bond[i]);
  }
  return res;
}

VertBond rpd::findBond(QList<VertBond> bonds,int a,int e){
  VertBond res;
  for (int i=0;i<bonds.size();i++){
    if (((bonds.at(i).a==a)||(bonds.at(i).e==a))&&((bonds.at(i).a==e)||(bonds.at(i).e==e)))
      res=bond[i];
  }
  return res;
}

void rpd::load(QString filename){
  QFile dat(filename);
  dat.open(QIODevice::ReadOnly);
  QString alles=dat.readAll();
  dat.close();
  QStringList lines=alles.split("\n");
  QStringList items;
  int i=0;
  while (!item.contains("ntheta=")){
    items=lines.at(i).split(" ",QString::SkipEmptyParts);
    i++;
  }
  int wo=items.indexOf("ntheta=",0)+1;
  int ende=items.at(wo).toInt();
  QList<V3> punkte;
  V3 punkt;
  for (int j=i; j<ende+i; j++){
    items=lines.at(i).split(" ",QString::SkipEmptyParts);
    if ((items.size())&&(items.at(0).toInt()>j-i)){
      punkt.x=items.at(1).toDouble();
      punkt.y=items.at(2).toDouble();
      punkt.z=items.at(3).toDouble();
      punkte.append(punkt);
    }
  }
  VertBond vb;
  QList<VertBond> vbs;
  for (int j=1;j<punkte.size();j++){
    vb.a=0;
    vb.e=j;
    vb.l=sqrt(Distance(punkte.at(0),punkte.at(j)));
    vbs.append(vb);
  }
  qStableSort(vbs.begin(), vbs.end(),comp);
  double maxL=(vbs.size()>12)?vbs.at(12).l:vbs.at(vbs.size()-1);
  vbs.clear();
  for (int j=0; j<punkte.size();j++)
    for (int k=0; k<punkte.size();k++){      
      vb.a=k;
      vb.e=j;
      vb.l=sqrt(Distance(punkte.at(k),punkte.at(j)));
      if (vb.l<maxL) vbs.append(vb);
    }
  //die vierte stirbt
  for (i=0; i<punkte.size();i++){
    QList<VertBond> nb= findBopnds(vbs,i);
    qStableSort(nb.begin(), nb.end(),comp);
    if (nb.at(0).a==i) int j=nb.at(0).e;
    if (nb.at(0).e==i) int j=nb.at(0).a;
    QList<VertBond> nnb= findBopnds(vbs,j);
    qStableSort(nnb.begin(), nnb.end(),comp);
  }
}
