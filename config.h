
#include <time.h>
#include "include.h"

#define NOERR

#define W 1920
#define H 1080
#define GAP 10
#define BARHEIGHT 18

#define SCREENEDP "eDP-1"
#define SCREENHDM "HDMI-1-1" // Noevau
//#define SCREENHDM "HDMI-1-0"

#define DESKTOPNUM 9

#define ALPHA 0.7
#define COLSEL (0x00FFAAFF | ((unsigned char)(0xff * ALPHA) << 24))
#define COLWIN (0x00AA88AA | ((unsigned char)(0xff * ALPHA) << 24))

#define MODKEY Mod4Mask
#define ALTKEY Mod1Mask

#define FONT "SourceCodePro-ExtraLight:pixelsize=18:antialias=true:autohint=true"
#define FONTSIZE 18
/*static const char *const autostart[] = {
	"setxkbmap", "-model", "USB Keyboard", "-layout", "gb", NULL,
	"xrandr", "--output", SCREENHDM, "--primary", NULL,
	"xrandr", "--output", SCREENHDM, "--preferred", NULL,
	//"xrandr", "--output", SCREENEDP, "--off", NULL,
	"/home/solly/Backgrounds/fehbg", NULL,
	"xcompmgr", NULL,
	NULL
};*/

// this function is executed at start
#define ROOTTEXTSIZE 19
const char  *roottextdays[]  = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char  *roottextmonths[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static char *roottext;
static void __attribute__((noreturn)) setroottext() {
	time_t     timeraw;
	struct tm *timeinf;
	static Display *t_disp;
	static Window   t_root;
	if ((t_disp = XOpenDisplay(NULL)) == NULL) die("Failed to open Display");
	t_root = XDefaultRootWindow(disp);
	while (1) {

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

		XStoreName(t_disp, t_root, NULL);
		XSync(t_disp, False);
		sleep(1);
	}
}

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
static const char *c_dc[]        = { "discord",                                   NULL };
static const char *c_mc[]        = { "multimc",                                   NULL };

static const char *c_screen[]    = { "screenshot",                                NULL };
static const char *c_screenall[] = { "screenshot", "1",                           NULL };
static const char *c_xkill[]     = { "xkill",                                     NULL };
static const char *c_panic[]     = { "st", "-e", "doas", "/usr/local/bin/panic",  NULL };
static const char *c_goodstart[] = { "/home/install/util/scripts/goodstart.sh",   NULL };

static Key keys[] = {
	/* modifier             key        function             argument             */
	{ MODKEY,               XK_t,	   a_spawn,             (const Arg*)c_tr        },
	{ MODKEY,               XK_e,      a_spawn,             (const Arg*)c_fm        },
	{ MODKEY,               XK_v,	   a_spawn,             (const Arg*)c_pv        },

	{ MODKEY,               XK_s,      a_spawn,             (const Arg*)c_st        },
	{ MODKEY,               XK_f,      a_spawn,             (const Arg*)c_br        },
	{ MODKEY,               XK_d,      a_spawn,             (const Arg*)c_dc        },
	{ MODKEY,               XK_m,      a_spawn,             (const Arg*)c_mc        },

	{ ALTKEY,               XK_Print,  a_spawn,             (const Arg*)c_screenall },
	{ 0,                    XK_Print,  a_spawn,             (const Arg*)c_screen    },
	{ MODKEY,               XK_F4,     a_spawn,             (const Arg*)c_xkill     },

	{ ALTKEY,             	XK_F4,     a_kill,              NULL                    },
    { ALTKEY,               XK_space,  a_toggletop,         NULL                    },
	{ ALTKEY|ControlMask,   XK_Delete, a_quit,              NULL                    },

	{ 0,                    XK_Alt_L,  a_startalttab,       NULL                    },
    { ALTKEY,               XK_Tab,    a_alttab,            NULL                    },

   	{ ALTKEY,               XK_1,      a_deskview,          (const Arg*)0           },
	{ ALTKEY,               XK_2,      a_deskview,          (const Arg*)1           },
	{ ALTKEY,               XK_3,      a_deskview,          (const Arg*)2           },
	{ ALTKEY,               XK_4,      a_deskview,          (const Arg*)3           },
	{ ALTKEY,               XK_5,      a_deskview,          (const Arg*)4           },
	{ ALTKEY,               XK_6,      a_deskview,          (const Arg*)5           },
	{ ALTKEY,               XK_7,      a_deskview,          (const Arg*)6           },
	{ ALTKEY,               XK_8,      a_deskview,          (const Arg*)7           },
	{ ALTKEY,               XK_9,      a_deskview,          (const Arg*)8           },

	{ ALTKEY|ShiftMask,     XK_1,      a_deskmove,          (const Arg*)0           },
	{ ALTKEY|ShiftMask,     XK_2,      a_deskmove,          (const Arg*)1           },
	{ ALTKEY|ShiftMask,     XK_3,      a_deskmove,          (const Arg*)2           },
	{ ALTKEY|ShiftMask,     XK_4,      a_deskmove,          (const Arg*)3           },
	{ ALTKEY|ShiftMask,     XK_5,      a_deskmove,          (const Arg*)4           },
	{ ALTKEY|ShiftMask,     XK_6,      a_deskmove,          (const Arg*)5           },
	{ ALTKEY|ShiftMask,     XK_7,      a_deskmove,          (const Arg*)6           },
	{ ALTKEY|ShiftMask,     XK_8,      a_deskmove,          (const Arg*)7           },
	{ ALTKEY|ShiftMask,     XK_9,      a_deskmove,          (const Arg*)8           },

	{ ALTKEY,				XK_q,	   a_resize,            (const Arg*)(int []) { GAP                  , GAP                                            , W/2 - (GAP*1.5), (H-(GAP*2)-BARHEIGHT)/2 - GAP } },
	{ ALTKEY,				XK_w,	   a_resize,            (const Arg*)(int []) { GAP                  , GAP                                            , W   - (GAP*2  ), (H-(GAP*2)-BARHEIGHT)/2 - GAP } },
	{ ALTKEY,				XK_e,	   a_resize,            (const Arg*)(int []) { (W/2) + (GAP*0.5)    , GAP                                            , W/2 - (GAP*1.5), (H-(GAP*2)-BARHEIGHT)/2 - GAP } },

	{ ALTKEY,				XK_a,	   a_resize,            (const Arg*)(int []) { GAP                  , GAP                                            , W/2 - (GAP*1.5), (H-(GAP*2)-BARHEIGHT)   - GAP } },
	{ ALTKEY,				XK_s,	   a_resize,            (const Arg*)(int []) { GAP                  , GAP                                            , W   - (GAP*2  ), (H-(GAP*2)-BARHEIGHT)   - GAP } },
	{ ALTKEY,				XK_d,	   a_resize,            (const Arg*)(int []) { (W/2) + (GAP*0.5)    , GAP                                            , W/2 - (GAP*1.5), (H-(GAP*2)-BARHEIGHT)   - GAP } },

	{ ALTKEY,				XK_z,	   a_resize,            (const Arg*)(int []) { GAP                  , (H-(GAP*2)-BARHEIGHT)/2 + GAP                  , W/2 - (GAP*1.5), (H-(GAP*2)-BARHEIGHT)/2 - GAP } },
	{ ALTKEY,				XK_x,	   a_resize,            (const Arg*)(int []) { GAP                  , (H-(GAP*2)-BARHEIGHT)/2 + GAP                  , W   - (GAP*2  ), (H-(GAP*2)-BARHEIGHT)/2 - GAP } },
	{ ALTKEY,				XK_c,	   a_resize,            (const Arg*)(int []) { (W/2) + (GAP*0.5)    , (H-(GAP*2)-BARHEIGHT)/2 + GAP                  , W/2 - (GAP*1.5), (H-(GAP*2)-BARHEIGHT)/2 - GAP } },

	{ ALTKEY,				XK_t,	   a_resize,            (const Arg*)(int []) { W/2 - W/4 + (GAP*.75), (H-(GAP*2)-BARHEIGHT)/2 - (H-GAP)/4 - (GAP*0.5), W/2 - (GAP*1.5), (H-(GAP*2)-BARHEIGHT)/2 - GAP } },
	{ ALTKEY,				XK_f,	   a_resize,            (const Arg*)(int []) { 0                    , 0                                              , 1920           , 1080                          } },


};

static Btn btns[] = {
	/* click                modifier        btn             function        argument */
	{ ClkClient,            ALTKEY,         Button1,        a_mousemove,      NULL },
	{ ClkClient,            ALTKEY,         Button3,        a_mouseresize,    NULL },
	//{ ClkTagBar,            0,              Button1,        view,           { 0 } },

	/*{ ClkStatusText,        0,              Button1,        spawn,          {.v = termcmd } },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkStatusText,        0,              Button3,        spawn,          {.v = termcmd } },*/
	
};
