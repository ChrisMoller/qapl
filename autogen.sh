#nin/sh

#APL_SRC="sed -es#@INCLUDEPATH@#/home/moller/Downloads/apl-1486#  < qapl.pro-in"
APL_SRC="sed -es#@INCLUDEPATH@#/home/moller/Downloads/apl-1486#"

$APL_SRC <qapl.pro-in >qapl.pro
