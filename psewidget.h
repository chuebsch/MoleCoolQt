
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
#ifndef PSEWIDGET_H
#define PSEWIDGET_H

#include <QWidget>
#include <QSignalMapper>
#include <QDebug>
#include <QToolButton>
#include "molekul.h"
/*! \brief PSEWidget is a widget that features a periodic table of the elements. 
 *
 *  The color scheme is given by the molecule class and the settings given there.
 *  
 *  The elements in the table can be clicked to select one of them.
 */ 
class PSEWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PSEWidget(QWidget *parent = 0);
    int selection;//!< The curent selected element starting from zero which refers to Hydrogen 
    QSignalMapper* esm;
signals:
void pseSelected(int index);//!< This signal is emitted when an element is clicked.
public slots:
void selectPSE(int index){selection=index; emit pseSelected(index);}//!< Change the current element by the program.
void incFontSize();//!< Increase the font size of the Elements by one point.
void decFontSize();//!< Decrease the font size of the Elements by one point.
void colorchange(QColor bgcolor); //! Assign a new color to the current element.
private:
int sz; 
molekul mole;
QToolButton *elebutt[109];
QToolButton *lplus,*lminus;
};

#endif // PSEWIDGET_H
