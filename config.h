
#include <time.h>
#include "include.h"

#define NOERR

#define GAP 10
#define BARHEIGHT 18

// Colors for bar
#define ALPHA 0.7
#define OPAQUE 1.0
#define ALPHAMASK ((unsigned int)(0xff * 0.7) << 24)
#define OPAQUEMASK ((unsigned int)(0xff * 1.0) << 24)

// Default colors
#define COLBG1 (0x000000 | ALPHAMASK)
#define COLFG1 (0xffffff | OPAQUEMASK)
// Used for highlighted
#define COLBG2 (0x000000 | ALPHAMASK)
#define COLFG2 (0xffddff | OPAQUEMASK)
// Used for selected
#define COLBG3 (0xffddff | OPAQUEMASK)
#define COLFG3 (0x000000 | ALPHAMASK)

// How many desktops there are
#define DESKTOPNUM 9
// What character to start from (common ones are 0, 1, a and A)
#define DESKTOPSTART '1'

// #define MODKEY Mod4Mask
#define MODKEY Mod4Mask

#define FONT "SourceCodePro-ExtraLight:pixelsize=17:antialias=false:autohint=true"
#define FONTSIZE 17

// this function is executed at start
#define ROOTTEXTSIZE 19
const char  *roottextdays[]   = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char  *roottextmonths[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static char  roottext[ROOTTEXTSIZE];
static void setroottext() {
	static time_t timeraw;
	static struct tm *timeinf;
	time(&timeraw);
	timeinf = localtime(&timeraw);
	sprintf(roottext, "%02d:%02d.%02d %s %02d/%s",  
		timeinf->tm_hour,               // hours
		timeinf->tm_min,                // minutes
		timeinf->tm_sec,                // seconds
		roottextdays[timeinf->tm_wday], // weekday
		timeinf->tm_mday,               // day
		roottextmonths[timeinf->tm_mon] // month
	);
}

#define SCREENEDP "eDP-1"
#define SCREENHDM "HDMI-1-1" // Noevau
//#define SCREENHDM "HDMI-1-0"
static const char *autostart[] = {
	"xrandr", "--output", SCREENHDM, "--primary", NULL,
	"xrandr", "--output", SCREENHDM, "--preferred", NULL,
	"xrandr", "--output", SCREENEDP, "--off", NULL,
	"/home/install/util/scripts/goodstart.sh", NULL,
	#ifndef XEPHYR
		/*
picom --backend glx --glx-no-stencil --glx-no-rebind-pixmap --blur-method dual_kawase --blur-strength 5 --vsync --no-fading-openclose -r 10 -c --corner-radius 5 --shadow-offset-x -10 --shadow-offset-y -10 --shadow-red 1 --shadow-green 0.8 --shadow-blue 1
		*/
		"picom",
			"--backend", "glx", 
			"--glx-no-stencil", "--glx-no-rebind-pixmap", 
			"--blur-method", "dual_kawase", "--blur-strength", "5", 
			"--vsync", "--no-fading-openclose", 
			"--corner-radius", "5", 
			"-c",
			"-r", "10",
			"--shadow-offset-x", "-10", "--shadow-offset-y", "-10",
			"--shadow-red", "1", "--shadow-green", "0.8", "--shadow-blue", "1",
		NULL,
	#endif
	NULL
};

static const char *c_tr[]        = { "st",                                        NULL };
static const char *c_fm[]        = { "pcmanfm",                                   NULL };
static const char *c_pv[]        = { "/home/install/util/scripts/pavucontrol.sh", NULL };

static const char *c_st[]        = { "steam",                                     NULL };
static const char *c_br[]        = { "browser",                                   NULL };
static const char *c_mc[]        = { "prism",                                     NULL };

static const char *c_screen[]    = { "screenshot",                                NULL };
static const char *c_screenall[] = { "screenshot", "1",                           NULL };
static const char *c_xkill[]     = { "xkill",                                     NULL };
static const char *c_panic[]     = { "st", "-e", "doas", "/usr/local/bin/panic",  NULL };
static const char *c_goodstart[] = { "/home/install/util/scripts/goodstart.sh",   NULL };

static Key keys[] = {
	/* modifier             key        function             argument             */
	{ MODKEY,               XK_t,      a_spawn,             (const Arg*)c_tr        },
	{ MODKEY,               XK_f,      a_spawn,             (const Arg*)c_fm        },
	{ MODKEY,               XK_v,      a_spawn,             (const Arg*)c_pv        },

	{ MODKEY,               XK_s,      a_spawn,             (const Arg*)c_st        },
	{ MODKEY,               XK_b,      a_spawn,             (const Arg*)c_br        },
	{ MODKEY,               XK_m,      a_spawn,             (const Arg*)c_mc        },

	{ MODKEY,               XK_Print,  a_spawn,             (const Arg*)c_screenall },
	{ 0,                    XK_Print,  a_spawn,             (const Arg*)c_screen    },
	{ MODKEY,               XK_F4,     a_spawn,             (const Arg*)c_xkill     },

	{ MODKEY,             	XK_F4,     a_kill,              NULL                    },
    { MODKEY,               XK_space,  a_toggletop,         NULL                    },
	{ MODKEY|ControlMask,   XK_Delete, a_quit,              NULL                    },

	{ 0,                    XK_Alt_L,  a_switchstart,       NULL                    },
    { MODKEY,               XK_Tab,    a_switch,            NULL                    },

   	{ MODKEY,               XK_1,      a_deskview,          (const Arg*)0           },
	{ MODKEY,               XK_2,      a_deskview,          (const Arg*)1           },
	{ MODKEY,               XK_3,      a_deskview,          (const Arg*)2           },
	{ MODKEY,               XK_4,      a_deskview,          (const Arg*)3           },
	{ MODKEY,               XK_5,      a_deskview,          (const Arg*)4           },
	{ MODKEY,               XK_6,      a_deskview,          (const Arg*)5           },
	{ MODKEY,               XK_7,      a_deskview,          (const Arg*)6           },
	{ MODKEY,               XK_8,      a_deskview,          (const Arg*)7           },
	{ MODKEY,               XK_9,      a_deskview,          (const Arg*)8           },

	{ MODKEY|ShiftMask,     XK_1,      a_deskmove,          (const Arg*)0           },
	{ MODKEY|ShiftMask,     XK_2,      a_deskmove,          (const Arg*)1           },
	{ MODKEY|ShiftMask,     XK_3,      a_deskmove,          (const Arg*)2           },
	{ MODKEY|ShiftMask,     XK_4,      a_deskmove,          (const Arg*)3           },
	{ MODKEY|ShiftMask,     XK_5,      a_deskmove,          (const Arg*)4           },
	{ MODKEY|ShiftMask,     XK_6,      a_deskmove,          (const Arg*)5           },
	{ MODKEY|ShiftMask,     XK_7,      a_deskmove,          (const Arg*)6           },
	{ MODKEY|ShiftMask,     XK_8,      a_deskmove,          (const Arg*)7           },
	{ MODKEY|ShiftMask,     XK_9,      a_deskmove,          (const Arg*)8           },

	{ MODKEY,               XK_q,      a_tile,              (const Arg*)TileNW      },
	{ MODKEY,               XK_a,      a_tile,              (const Arg*)TileW       },
	{ MODKEY,               XK_z,      a_tile,              (const Arg*)TileSW      },

	{ MODKEY,               XK_w,      a_tile,              (const Arg*)TileN       },
	{ MODKEY,               XK_s,      a_tile,              (const Arg*)TileFill    },
	{ MODKEY,               XK_x,      a_tile,              (const Arg*)TileS       },

	{ MODKEY,               XK_e,      a_tile,              (const Arg*)TileNE      },
	{ MODKEY,               XK_d,      a_tile,              (const Arg*)TileE       },
	{ MODKEY,               XK_c,      a_tile,              (const Arg*)TileSE      },

	{ MODKEY|ShiftMask,     XK_s,      a_tile,              (const Arg*)TileFullscreen },
	{ MODKEY|ControlMask,   XK_s,      a_tile,              (const Arg*)TileCenter  },
	{ MODKEY|ControlMask|ShiftMask, XK_s, a_tile,           (const Arg*)TileDoubleFullscreen },

	// { MODKEY|ShiftMask,		XK_q,      a_resize,            (const Arg*)(int []) { GAP                  , GAP                                            , W/2 - (GAP*1.5), (H-(GAP*2)-BARHEIGHT)/2 - GAP } }, // top left
	// { MODKEY,				XK_a,      a_resize,            (const Arg*)(int []) { GAP                  , GAP                                            , W   - (GAP*2  ), (H-(GAP*2)-BARHEIGHT)/2 - GAP } }, // top
	// { MODKEY|ControlMask,   XK_z,      a_resize,            (const Arg*)(int []) { (W/2) + (GAP*0.5)    , GAP                                            , W/2 - (GAP*1.5), (H-(GAP*2)-BARHEIGHT)/2 - GAP } }, // top right

	// { MODKEY|ShiftMask,		XK_q,      a_resize,            (const Arg*)(int []) { GAP                  , GAP                                            , W/2 - (GAP*1.5), (H-(GAP*2)-BARHEIGHT)   - GAP } }, // left
	// { MODKEY,				XK_a,      a_resize,            (const Arg*)(int []) { GAP                  , GAP                                            , W   - (GAP*2  ), (H-(GAP*2)-BARHEIGHT)   - GAP } }, // full
	// { MODKEY|ControlMask,   XK_z,      a_resize,            (const Arg*)(int []) { (W/2) + (GAP*0.5)    , GAP                                            , W/2 - (GAP*1.5), (H-(GAP*2)-BARHEIGHT)   - GAP } }, // right

	// { MODKEY|ShiftMask,		XK_q,      a_resize,            (const Arg*)(int []) { GAP                  , (H-(GAP*2)-BARHEIGHT)/2 + GAP                  , W/2 - (GAP*1.5), (H-(GAP*2)-BARHEIGHT)/2 - GAP } }, // bottom left
	// { MODKEY,				XK_a,      a_resize,            (const Arg*)(int []) { GAP                  , (H-(GAP*2)-BARHEIGHT)/2 + GAP                  , W   - (GAP*2  ), (H-(GAP*2)-BARHEIGHT)/2 - GAP } }, // bottom
	// { MODKEY|ControlMask,   XK_z,      a_resize,            (const Arg*)(int []) { (W/2) + (GAP*0.5)    , (H-(GAP*2)-BARHEIGHT)/2 + GAP                  , W/2 - (GAP*1.5), (H-(GAP*2)-BARHEIGHT)/2 - GAP } }, // bottom right

	// { MODKEY|ShiftMask,		XK_q,      a_resize,            (const Arg*)(int []) { W/2 - W/4 + (GAP*.75), (H-(GAP*2)-BARHEIGHT)/2 - (H-GAP)/4 - (GAP*0.5), W/2 - (GAP*1.5), (H-(GAP*2)-BARHEIGHT)/2 - GAP } }, // mid mid
	// { MODKEY,				XK_a,      a_resize,            (const Arg*)(int []) { 0                    , 0                                              , W            , H                          } }, // fullscren
	// { MODKEY|ControlMask,   XK_z,      a_resize,            (const Arg*)(int []) { 0                    , 0                                              , W            , H                          } }, // fullscren+


};

static Btn btns[] = {
	/* click                modifier        btn             function        argument */
	{ ClkClient,            MODKEY,         Button1,        a_mousemove,      NULL },
	{ ClkClient,            MODKEY,         Button3,        a_mouseresize,    NULL },
	//{ ClkTagBar,            0,              Button1,        view,           { 0 } },

	/*{ ClkStatusText,        0,              Button1,        spawn,          {.v = termcmd } },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkStatusText,        0,              Button3,        spawn,          {.v = termcmd } },*/
	
};
