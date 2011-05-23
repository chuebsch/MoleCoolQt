#!/bin/sh
FILE=MyWindow.cpp
Version=`/usr/bin/svn info|grep Revision:|sed -e "s/Revision: //" ` 
let Version=$Version+1
#Datum=`/usr/bin/svn info|grep "Last Changed Date:"|sed -e "s/Last Changed Date://"` 
#ADatum=`grep "LastChangedDate: " $FILE|sed -e "s/[\$;\"=]//g"|sed -e "s/  QString dateLastChangedDate://"`
AlteVersion=`grep -P "rev=\d+" $FILE|sed -e "s/int rev=//"|sed -e "s/[\"\$= ;\r\n]//g"`
let AlteVersion=$AlteVersion+0
sed -i -r -e "s/rev=$AlteVersion/rev=$Version/" $FILE
#sed -i -e "s/$ADatum/$Datum/" $FILE
echo "Revision  is: $Version It was !$AlteVersion!"
#QString date="$LastChangedDate:
