rm molecoolqt.dmg
rm -r molecoolqt.app/ 
make distclean
/Users/chuebsch/QtSDK/Desktop/Qt/4.8.0/gcc/bin/qmake
make 
/Users/chuebsch/QtSDK/Desktop/Qt/4.8.0/gcc/bin/macdeployqt molecoolqt.app/ -dmg
rm -r molecoolqt.app/ 
make
