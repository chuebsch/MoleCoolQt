rm molecoolQt.dmg
rm -r molecoolQt.app/ 
make 
macdeployqt molecoolQt.app/ -dmg
rm -r molecoolQt.app/ 
make
