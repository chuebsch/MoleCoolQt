#ifndef _INAMES_H
#define _INAMES_H
#include <string>
#include <sstream>
#include <QVector>
#include <QString>
#include "molekul.h"
using namespace std;
class inames{
  //  int getOZ(string s);
 static  QString botostr(int bo);
  //bool icmp(string i, string j);
 public:
 static  QString invName(MyAtom core,Connection &cl,CEnvironment &sel,int rung=1);
 static void ringlord(Connection cl);
};
#endif
