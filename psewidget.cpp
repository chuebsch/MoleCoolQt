
/****************************************************************************
**
** Copyright (C) 2011 Christian B. Huebschle & George M. Sheldrick
** All rights reserved.
** Contact: chuebsch@moliso.de
**
** This file is part of the ShelXle
**
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file COPYING included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**
****************************************************************************/
#include "psewidget.h"
#include <QtGui>

PSEWidget::PSEWidget(QWidget *parent) :
    QWidget(parent){
    selection=0;
    static QStringList PSE;
    mole.einstellung  = new QSettings( QSettings::IniFormat, QSettings::UserScope ,"Christian_B._Huebschle","MoleCoolQt" );

    mole.loadSettings();
    sz=mole.pseSize;
    PSE.clear();
    PSE<<"H"<<"He"<<"Li"<<"Be"<<"B"<<"C"<<"N"<<"O"<<"F"<<"Ne"<<"Na"<<"Mg"<<"Al"<<"Si"<<"P"<<"S"<<"Cl"<<"Ar"<<
            "K"<<"Ca"<<"Sc"<<"Ti"<<"V"<<"Cr"<<"Mn"<<"Fe"<<"Co"<<"Ni"<<"Cu"<<"Zn"<<"Ga"<<"Ge"<<"As"<<"Se"<<"Br"<<"Kr"<<
            "Rb"<<"Sr"<<"Y"<<"Zr"<<"Nb"<<"Mo"<<"Tc"<<"Ru"<<"Rh"<<"Pd"<<"Ag"<<"Cd"<<"In"<<"Sn"<<"Sb"<<"Te"<<"I"<<"Xe"<<
            "Cs"<<"Ba"<< "La"<<"Ce"<<"Pr"<<"Nd"<<"Pm"<<"Sm"<<"Eu"<<"Gd"<<"Tb"<<"Dy"<<"Ho"<<"Er"<<"Tm"<<"Yb"<<"Lu"<<
            "Hf"<<"Ta"<<"W"<<"Re"<<"Os"<<"Ir"<<"Pt"<<"Au"<<"Hg"<<"Tl"<<"Pb"<<"Bi"<<"Po"<<"At"<<"Rn"<<"Fr"<<"Ra"<<
            "Ac"<<"Th"<<"Pa"<<"U"<<"Np"<<"Pu"<<"Am"<<"Cm"<<"Bk"<<"Cf"<<"Es"<<"Fm"<<"Md"<<"No"<<"Lr";//<<"Hs"<<"Mt";
    QToolButton *TB;
    lplus = new QToolButton();
    lplus->setIcon(QIcon(":/images/efplus.png"));
    connect(lplus,SIGNAL(pressed()),this,SLOT(incFontSize()));
    lminus = new QToolButton();
    lminus->setIcon(QIcon(":/images/efminus.png"));
    lplus->setStyleSheet(
                QString(
                "QToolButton { /* all types of tool button */\n"
                "border: 1px solid #8f8f91;"
                "border-radius: 2px;"
                "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                "stop: 0 %1, stop: 1 #dadbde);"
                "}"
                "QToolButton:hover {"
                "border: 5px; solid #8f8f91;"
                "border-radius: 2px;"
               // "width: 40px;"
               // "height: 40px;"
                "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,"
                "stop: 0 #ffffff, stop: 1 %1);"

               // "border: 0px;"
                "}"
                "QToolButton:pressed {"
                "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,"
                "stop: 0 %1, stop: 1 #ffffff);"
                "border: 0px;"
                "}").arg("#dddddd"));
    lplus->setFixedWidth(QFontMetrics(QFont("Arial",sz+2,QFont::Black)).width("Mmk"));
    lminus->setStyleSheet(
                QString(
                "QToolButton { /* all types of tool button */\n"
                "border: 1px solid #8f8f91;"
                "border-radius: 2px;"
                "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                "stop: 0 %1, stop: 1 #dadbde);"
                "}"
                "QToolButton:hover {"
                "border: 5px; solid #8f8f91;"
                "border-radius: 2px;"
               // "width: 40px;"
               // "height: 40px;"
                "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,"
                "stop: 0 #ffffff, stop: 1 %1);"

               // "border: 0px;"
                "}"
                "QToolButton:pressed {"
                "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,"
                "stop: 0 %1, stop: 1 #ffffff);"
                "border: 0px;"
                "}").arg("#dddddd"));
    lminus->setFixedWidth(QFontMetrics(QFont("Arial",sz+2,QFont::Black)).width("Mmk"));
    connect(lminus,SIGNAL(pressed()),this,SLOT(decFontSize()));


/*    a=viewMenu->addAction(QIcon(":/icons/laminus.png"),
		    tr("decrease label fontsize"),chgl,SLOT(decFontSize()),
		    QKeySequence("Alt+-"));*/
    esm = new QSignalMapper(this);
    for (int i=0; i<PSE.size(); i++){
        TB = new QToolButton();
        TB->setText(PSE.at(i));
        //TB->setBackgroundRole(QPalette::AlternateBase);
        TB->setFont(QFont("Arial",sz,QFont::Black));

        //printf("ii %d %d\n",i,QFontMetrics(QFont("Arial",-1,QFont::Black)).width("Mmk"));
        //fflush(stdout);
        TB->setAutoRaise(true);
        TB->setFixedWidth(QFontMetrics(QFont("Arial",sz+2,QFont::Black)).width("Mmk"));
        QColor ac=QColor((int)(255*mole.Acol[i][0]),
                         (int)(255*mole.Acol[i][1]),
                         (int)(255*mole.Acol[i][2]),
                         (int)(255*mole.Acol[i][3]));
        if (i<107)TB->setStyleSheet(
                QString(
                "QToolButton { /* all types of tool button */\n"
                "border: 1px solid #8f8f91;"
                "border-radius: 2px;"
                "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                "stop: 0 %1, stop: 1 #dadbde);"
                "}"
                "QToolButton:hover {"
                "border: 5px; solid #8f8f91;"
                "border-radius: 2px;"
               // "width: 40px;"
               // "height: 40px;"
                "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,"
                "stop: 0 #ffffff, stop: 1 %1);"

               // "border: 0px;"
                "}"
                "QToolButton:pressed {"
                "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,"
                "stop: 0 %1, stop: 1 #ffffff);"
                "border: 0px;"
                        "}").arg(ac.name()));
{
	}
        elebutt[i]=TB;

        esm->setMapping(elebutt[i],i);
        connect(elebutt[i], SIGNAL(clicked()), esm, SLOT(map()));
    }

    //connect(esm, SIGNAL(mapped(int)),this,SIGNAL(pseSelected(int)));
    connect(esm, SIGNAL(mapped(int)),this,SLOT(selectPSE(int)));
    QGridLayout *gl = new QGridLayout();
    gl->addWidget(elebutt[0],0,0); //H
    gl->addWidget(lplus,0,5);
    gl->addWidget(lminus,0,6);

    gl->addWidget(elebutt[1],0,17);//He
    gl->addWidget(elebutt[2],1,0); //Li
    gl->addWidget(elebutt[3],1,1); //Be
    gl->addWidget(elebutt[4],1,12);//B
    gl->addWidget(elebutt[5],1,13);//C
    gl->addWidget(elebutt[6],1,14);//N
    gl->addWidget(elebutt[7],1,15);//O
    gl->addWidget(elebutt[8],1,16);//F
    gl->addWidget(elebutt[9],1,17);//Ne
    gl->addWidget(elebutt[10],2,0); //Na
    gl->addWidget(elebutt[11],2,1); //Mg
    gl->addWidget(elebutt[12],2,12);//Al
    gl->addWidget(elebutt[13],2,13);//Si
    gl->addWidget(elebutt[14],2,14);//P
    gl->addWidget(elebutt[15],2,15);//S
    gl->addWidget(elebutt[16],2,16);//Cl
    gl->addWidget(elebutt[17],2,17);//Ar
    gl->addWidget(elebutt[18],3,0);//K
    gl->addWidget(elebutt[19],3,1);//Ca
    gl->addWidget(elebutt[20],3,2);//Sc
    gl->addWidget(elebutt[21],3,3);//Ti
    gl->addWidget(elebutt[22],3,4);//V
    gl->addWidget(elebutt[23],3,5);//Cr
    gl->addWidget(elebutt[24],3,6);//Mn
    gl->addWidget(elebutt[25],3,7);//Fe
    gl->addWidget(elebutt[26],3,8);//Co
    gl->addWidget(elebutt[27],3,9);//Ni
    gl->addWidget(elebutt[28],3,10);//Cu
    gl->addWidget(elebutt[29],3,11);//Zn
    gl->addWidget(elebutt[30],3,12);//Ga
    gl->addWidget(elebutt[31],3,13);//Ge
    gl->addWidget(elebutt[32],3,14);//As
    gl->addWidget(elebutt[33],3,15);//Se
    gl->addWidget(elebutt[34],3,16);//Br
    gl->addWidget(elebutt[35],3,17);//Kr
    gl->addWidget(elebutt[36],4,0);//Rb
    gl->addWidget(elebutt[37],4,1);//Sr
    gl->addWidget(elebutt[38],4,2);//Y
    gl->addWidget(elebutt[39],4,3);//Zr
    gl->addWidget(elebutt[40],4,4);//Nb
    gl->addWidget(elebutt[41],4,5);//Mo
    gl->addWidget(elebutt[42],4,6);//Tc
    gl->addWidget(elebutt[43],4,7);//Ru
    gl->addWidget(elebutt[44],4,8);//Rh
    gl->addWidget(elebutt[45],4,9);//Pd
    gl->addWidget(elebutt[46],4,10);//Ag
    gl->addWidget(elebutt[47],4,11);//Cd
    gl->addWidget(elebutt[48],4,12);//In
    gl->addWidget(elebutt[49],4,13);//Sn
    gl->addWidget(elebutt[50],4,14);//Sb
    gl->addWidget(elebutt[51],4,15);//Te
    gl->addWidget(elebutt[52],4,16);//I
    gl->addWidget(elebutt[53],4,17);//Xe
    gl->addWidget(elebutt[54],5,0);//Cs
    gl->addWidget(elebutt[55],5,1);//Ba
    gl->addWidget(elebutt[56],7,0);//La
    gl->addWidget(elebutt[57],7,1);//Ce
    gl->addWidget(elebutt[58],7,2);//Pr
    gl->addWidget(elebutt[59],7,3);//Nd
    gl->addWidget(elebutt[60],7,4);//Pm
    gl->addWidget(elebutt[61],7,5);//Sm
    gl->addWidget(elebutt[62],7,6);//Eu
    gl->addWidget(elebutt[63],7,7);//Gd
    gl->addWidget(elebutt[64],7,8);//Tb
    gl->addWidget(elebutt[65],7,9);//Dy
    gl->addWidget(elebutt[66],7,10);//Ho
    gl->addWidget(elebutt[67],7,11);//Er
    gl->addWidget(elebutt[68],7,12);//Tm
    gl->addWidget(elebutt[69],7,13);//Yb
    gl->addWidget(elebutt[70],7,14);//Lu
    gl->addWidget(elebutt[71],5,3);//Hf
    gl->addWidget(elebutt[72],5,4);//Ta
    gl->addWidget(elebutt[73],5,5);//W
    gl->addWidget(elebutt[74],5,6);//Re
    gl->addWidget(elebutt[75],5,7);//Os
    gl->addWidget(elebutt[76],5,8);//Ir
    gl->addWidget(elebutt[77],5,9);//Pt
    gl->addWidget(elebutt[78],5,10);//Au
    gl->addWidget(elebutt[79],5,11);//Hg
    gl->addWidget(elebutt[80],5,12);//Tl
    gl->addWidget(elebutt[81],5,13);//Pb
    gl->addWidget(elebutt[82],5,14);//Bi
    gl->addWidget(elebutt[83],5,15);//Po
    gl->addWidget(elebutt[84],5,16);//At
    gl->addWidget(elebutt[85],5,17);//Rn
    gl->addWidget(elebutt[86],6,0);//Fr
    gl->addWidget(elebutt[87],6,1);//Ra
    gl->addWidget(elebutt[88],8,0);//Ac
    gl->addWidget(elebutt[89],8,1);//Th
    gl->addWidget(elebutt[90],8,2);//Pa
    gl->addWidget(elebutt[91],8,3);//U
    gl->addWidget(elebutt[92],8,4);//Np
    gl->addWidget(elebutt[93],8,5);//Pu
    gl->addWidget(elebutt[94],8,6);//Am
    gl->addWidget(elebutt[95],8,7);//Cm
    gl->addWidget(elebutt[96],8,8);//Bk
    gl->addWidget(elebutt[97],8,9);//Cf
    gl->addWidget(elebutt[98],8,10);//Es
    gl->addWidget(elebutt[99],8,11);//Fm
    gl->addWidget(elebutt[100],8,12);//Md
    gl->addWidget(elebutt[101],8,13);//No
    gl->addWidget(elebutt[102],8,14);//Lr
    //gl->addWidget(elebutt[103],8,17);//Ku
    //gl->addWidget(elebutt[104],6,4);//Ha
    //gl->addWidget(elebutt[105],6,5);//Rf
    //gl->addWidget(elebutt[106],6,6);//Ns
   // gl->addWidget(elebutt[107],6,7);//Hs
   // gl->addWidget(elebutt[108],6,8);//Mt
    gl->setRowStretch(9,12);
    gl->setSpacing(1);
    setLayout(gl);
}

void PSEWidget::incFontSize(){
  QToolButton *action = qobject_cast<QToolButton *>(sender());

  QFont ft;
  ft= elebutt[0]->font();
  mole.pseSize=sz=qMin(ft.pointSize()+1,24);
  ft.setPointSize(sz);
  action->setFixedWidth(QFontMetrics(QFont("Arial",sz+2,QFont::Black)).width("Mmk"));
  lminus->setFixedWidth(QFontMetrics(QFont("Arial",sz+2,QFont::Black)).width("Mmk"));
  for (int i=0; i<103; i++){
        elebutt[i]->setFont(ft);
        elebutt[i]->setFixedWidth(QFontMetrics(QFont("Arial",sz+2,QFont::Black)).width("Mmk"));
  }
  mole.einstellung->beginGroup("PSE");
  mole.einstellung->setValue("FontSize",mole.pseSize);
  mole.einstellung->endGroup();
  update();
}

void PSEWidget::decFontSize(){
  QToolButton *action = qobject_cast<QToolButton *>(sender());
  QFont ft;
  ft= elebutt[0]->font();
  mole.pseSize=sz=qMax(ft.pointSize()-1,6);
  ft.setPointSize(sz);
  action->setFixedWidth(QFontMetrics(QFont("Arial",sz+2,QFont::Black)).width("Mmk"));
  lplus->setFixedWidth(QFontMetrics(QFont("Arial",sz+2,QFont::Black)).width("Mmk"));
  for (int i=0; i<103; i++){
        elebutt[i]->setFont(ft);
        elebutt[i]->setFixedWidth(QFontMetrics(QFont("Arial",sz+2,QFont::Black)).width("Mmk"));
  }
  mole.einstellung->beginGroup("PSE");
  mole.einstellung->setValue("FontSize",mole.pseSize);
  mole.einstellung->endGroup();
  update();

}

void PSEWidget::colorchange(QColor bgcolor){
    elebutt[selection]->setStyleSheet(
            QString(
            "QToolButton { /* all types of tool button */\n"
            "border: 1px solid #8f8f91;"
            "border-radius: 2px;"
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
            "stop: 0 %1, stop: 1 #dadbde);"
            "}"
            "QToolButton:hover {"
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,"
            "stop: 0 #ffffff, stop: 1 %1);"
            "}"
            "QToolButton:pressed {"
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,"
            "stop: 0 %1, stop: 1 #ffffff);"
            "border: 0px;"
            "}").arg(bgcolor.name()));
    update();
}
