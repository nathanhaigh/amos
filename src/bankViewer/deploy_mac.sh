#!/bin/sh

## From http://doc.trolltech.com/qq/qq09-mac-deployment.html

mkdir -p hawkeye.app/Contents/Frameworks
cp $QTDIR/lib/libqt.3.dylib hawkeye.app/Contents/Frameworks

/usr/bin/install_name_tool -id @executable_path/../Frameworks/libqt.3.dylib hawkeye.app/Contents/Frameworks/libqt.3.dylib
/usr/bin/install_name_tool -change libqt.3.dylib @executable_path/../Frameworks/libqt.3.dylib hawkeye.app/Contents/MacOS/hawkeye
