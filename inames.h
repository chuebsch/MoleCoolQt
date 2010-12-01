#ifndef _INAMES_H
#define _INAMES_H
#include <string>
#include <sstream>
#include <QVector>
#include <QString>
#include "molekul.h"
using namespace std;
class inames{
 static  QString botostr(int bo);
 public:
 static  QString invName(MyAtom core,Connection &cl,CEnvironment &sel,int rung=1);
 static void ringlord(Connection cl);
};
#endif
