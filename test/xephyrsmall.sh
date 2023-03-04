_OLDDISPLAY="$DISPLAY"
. ./conf.sh
_DISPLAY="$DISPLAY"
DISPLAY="$_OLDDISPLAY"
Xephyr -br -ac -noreset -screen 900x1080 "$_DISPLAY"
