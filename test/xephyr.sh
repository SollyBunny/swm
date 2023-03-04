_OLDDISPLAY="$DISPLAY"
. ./conf.sh
_DISPLAY="$DISPLAY"
DISPLAY="$_OLDDISPLAY"
Xephyr -br -ac -noreset -screen 1920x1080 "$_DISPLAY"
