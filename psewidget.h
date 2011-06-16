#ifndef PSEWIDGET_H
#define PSEWIDGET_H

#include <QWidget>
#include <QSignalMapper>
#include <QDebug>
#include <QToolButton>
class PSEWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PSEWidget(QWidget *parent = 0);
    int selection;
    QSignalMapper* esm;
signals:
void pseSelected(int index);
public slots:
void selectPSE(int index){selection=index; emit pseSelected(index);}
void colorchange(QColor bgcolor);
private:
int col2int(float Acol[]){
  int erg=0;
  erg+=(int)(Acol[0]*255)*65536;
  erg+=(int)(Acol[1]*255)*256;
  erg+=(int)(Acol[2]*255);
  return erg;
}
QToolButton *elebutt[109];
};

#endif // PSEWIDGET_H
