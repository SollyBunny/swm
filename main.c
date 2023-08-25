
#include "include.h"
#include "config.h"

#define LEN(l) (sizeof(l) / sizeof((l)[0]))

// Var Declerations

	#define FONTWIDTH (FONTSIZE * 3 / 4)
	#define BARLESSWINHEIGHT (H - BARHEIGHT - (GAP * 2))
	#define BARDESKPAD (FONTSIZE / 3)
	#define BARDESKWIDTH (FONTSIZE + BARDESKPAD)
	#define BARDESKTOTALWIDTH ((BARDESKWIDTH * DESKTOPNUM) + (BARDESKPAD / 2))
	#define BARWINWIDTH ((W - (GAP * 2)) - BARDESKTOTALWIDTH - ((ROOTTEXTSIZE - 1) * FONTWIDTH))
	#define BARROOTTEXTSTART ((W - (GAP * 2)) - BARDESKTOTALWIDTH)
	static unsigned int mods[] = { 0, LockMask };

	static Client *desks[DESKTOPNUM]; 
	static unsigned int desksnum[DESKTOPNUM];
	static unsigned int desk = 0;

	static int autostartpids[LEN(autostart)];

	static Cursor cursor[3];
	
	static Atom wmatom[WMLast];
	static Atom netatom[NetLast];

	static char roottext[ROOTTEXTSIZE];

	static unsigned char stopped = 0;

	static long int dummy;

// Function declerations

	#define CLEANMASK(mask) ((mask) & ~LockMask)

	#ifndef NOERR
		int xerrordummy(Display *_, XErrorEvent *e) {
			(void)_;
			(void)e;
			return 0;
		}
		int __attribute__((noreturn)) xerrorstart(Display *_, XErrorEvent *e) {
			(void)_;
			(void)e;
			die("Another wm is already running");
		}
		int xerror(Display *_, XErrorEvent *e) {
			(void)_;
			printf("swm: XError: %d\n", e->error_code);
			return 0;
		}
	#endif

	static Client *manage(Window win) {

		static XWindowAttributes wa;
		if (!XGetWindowAttributes(disp, win, &wa)) return NULL;

		Client *c = malloc(sizeof(Client));
		c->win = win;
		c->desk = desk;
		c->istop = 0;
		c->x = wa.x;
		c->y = wa.y;
		c->w = wa.width;
		c->h = wa.height;
		c->title[0] = '\0';
		c->titlelen = 1;

		if (wa.map_state == IsUnmapped)
			XMapWindow(disp, win);

		// Get property change events such as title changes or size changes
		XSelectInput(disp, win, PropertyChangeMask);
		updatetitle(c);

		return c;

	}

	static void scan() {
		unsigned int i, num;
		Window *wins = NULL;
		Client *c;
		if (XQueryTree(disp, root, (Window *)&dummy, (Window *)&dummy, &wins, &num)) {
			for (i = 0; i < num; ++i) {
				c = manage(wins[i]);
				if (c) deskadd(c);
			}
			if (wins)
				XFree(wins);
		}
	}

	static void updatetitle(Client *c) {
		char **list = NULL;
		int n;
		XTextProperty name;
		c->title[0] = '\0';
		if (XGetTextProperty(disp, c->win, &name, XA_WM_NAME) && name.nitems) {
			c->titlelen = name.nitems;
			if (name.encoding == XA_STRING)
				strncpy(c->title, (char *)name.value, c->titlelen);
			else {
				if (XmbTextPropertyToTextList(disp, &name, &list, &n) >= Success && n > 0 && *list) {
					strncpy(c->title, *list, c->titlelen);
					XFreeStringList(list);
				}
			}
		} else if (XGetTextProperty(disp, c->win, &name, netatom[NetWMName]) && name.nitems) {
			c->titlelen = name.nitems;
			if (name.encoding == XA_STRING)
				strncpy(c->title, (char *)name.value, c->titlelen - 1);
			else {
				if (XmbTextPropertyToTextList(disp, &name, &list, &n) >= Success && n > 0 && *list) {
					strncpy(c->title, *list, c->titlelen - 1);
					XFreeStringList(list);
				}
			}
		}
		c->title[c->titlelen] = '\0';
		XFree(name.value);
		if (c->title[0] == '\0') { // hack to mark broken clients
			strcpy(c->title, "broken");
			c->titlelen = LEN("broken") - 1;
		}
		
	}

	static void updatebardummy() {
		printf("swm: Dummy was called\n");
	}
	static void updatebarreal() {
		XClearWindow(disp, bar.win);
		bar.xfttext[0] = '1';
		bar.selrect[0].x = -BARDESKWIDTH / 32;
		for (unsigned int i = 0; i < DESKTOPNUM; ++i) {
			if (i == desk) {
				
			} else if (desks[i] == NULL) {
				XftDrawStringUtf8(
					bar.xftgc, 
					&bar.xftfg1, bar.xftfont, 
					BARDESKPAD + BARDESKWIDTH * i, FONTSIZE * 4 / 5,
					bar.xfttext, 1
				);
			} else {
				XSetForeground(disp, bar.gc, COLBG2);
				XFillPolygon(disp, bar.win, bar.gc, (XPoint*)bar.selrect, 4, Nonconvex, CoordModePrevious);
				XftDrawStringUtf8(
					bar.xftgc, 
					&bar.xftfg2, bar.xftfont, 
					BARDESKPAD + BARDESKWIDTH * i, FONTSIZE * 4 / 5,
					bar.xfttext, 1
				);
			}
			++bar.xfttext[0];
			bar.selrect[0].x += BARDESKWIDTH;
		}
		bar.xfttext[0] = DESKTOPSTART + desk;
		bar.selrect[0].x = -BARDESKWIDTH / 32 + BARDESKWIDTH * desk;
		XSetForeground(disp, bar.gc, COLBG3);
		XFillPolygon(disp, bar.win, bar.gc, (XPoint*)bar.selrect, 4, Nonconvex, CoordModePrevious);
		XftDrawStringUtf8(
			bar.xftgc, 
			&bar.xftfg3, bar.xftfont, 
			BARDESKPAD + BARDESKWIDTH * desk, FONTSIZE * 4 / 5,
			bar.xfttext, 1//sizeof(xfttext) / sizeof(xfttext[0])
		);
		Client *c = desks[desk];
		if (c != NULL) {
			bar.winrect[0].x = BARDESKTOTALWIDTH;
			bar.winrect[1].x = (BARWINWIDTH / desksnum[desk]) + BARDESKWIDTH / 8; //
			bar.winrect[3].x = -(BARWINWIDTH / desksnum[desk]) - BARDESKWIDTH / 8; //
			bar.winrect[1].x = (BARWINWIDTH / desksnum[desk]) + BARDESKWIDTH / 8; //
			bar.winrect[3].x = -(BARWINWIDTH / desksnum[desk]) - BARDESKWIDTH / 8; //
			unsigned int i = 0;
			do {
				if (i == 0) {
					XSetForeground(disp, bar.gc, COLBG3);
					XFillPolygon(disp, bar.win, bar.gc, (XPoint*)bar.winrect, 4, Nonconvex, CoordModePrevious);
					XftDrawStringUtf8(
						bar.xftgc, 
						&bar.xftfg3, bar.xftfont, 
						BARDESKTOTALWIDTH + (BARWINWIDTH * i / desksnum[desk]) + (BARWINWIDTH / desksnum[desk] / 2) - (c->titlelen * FONTWIDTH / 2), FONTSIZE * 4 / 5,
						(FcChar8*)c->title, c->titlelen//sizeof(xfttext) / sizeof(xfttext[0])
					);
				} else if (c->istop) {
					XSetForeground(disp, bar.gc, COLBG2);
					XFillPolygon(disp, bar.win, bar.gc, (XPoint*)bar.winrect, 4, Nonconvex, CoordModePrevious);
					XftDrawStringUtf8(
						bar.xftgc, 
						&bar.xftfg2, bar.xftfont, 
						BARDESKTOTALWIDTH + (BARWINWIDTH * i / desksnum[desk]) + (BARWINWIDTH / desksnum[desk] / 2) - (c->titlelen * FONTWIDTH / 2), FONTSIZE * 4 / 5,
						(FcChar8*)c->title, c->titlelen//sizeof(xfttext) / sizeof(xfttext[0])
					);
				} else {
					XftDrawStringUtf8(
						bar.xftgc, 
						&bar.xftfg1, bar.xftfont, 
						BARDESKTOTALWIDTH + (BARWINWIDTH * i / desksnum[desk]) + (BARWINWIDTH / desksnum[desk] / 2) - (c->titlelen * FONTWIDTH / 2), FONTSIZE * 4 / 5,
						(FcChar8*)c->title, c->titlelen//sizeof(xfttext) / sizeof(xfttext[0])
					);
				}
				++i;
				bar.winrect[0].x += (BARDESKPAD / 2) + BARWINWIDTH / desksnum[desk];
			} while ((c = c->next));
		}
		printf("wrtiiotiten %s\n", roottext);
		XftDrawStringUtf8(
			bar.xftgc, 
			&bar.xftfg1, bar.xftfont, 
			BARROOTTEXTSTART, FONTSIZE * 4 / 5,
			(FcChar8*)roottext, ROOTTEXTSIZE
		);
		// XSync(disp, False);
		XFlush(disp);
	}
	static void (*updatebar)() = updatebardummy;
	static inline void createbar() {
		XMatchVisualInfo(disp, XDefaultScreen(disp), 32, TrueColor, &bar.visual);
		bar.attr.border_pixel     = 0;
		bar.attr.background_pixel = COLBG1;
	    bar.attr.colormap         = XCreateColormap(disp, root, bar.visual.visual, None);
	    bar.attr.bit_gravity      = NorthWestGravity;
		bar.attr.event_mask       = ExposureMask;
		bar.win = XCreateWindow(
			disp, root,
			GAP, H - GAP - BARHEIGHT, W - GAP - GAP, BARHEIGHT,
			0,
			bar.visual.depth,
			InputOutput,
			bar.visual.visual,
			CWBackPixel | CWBorderPixel | CWBitGravity | CWEventMask | CWColormap,
			&bar.attr
		);
		XStoreName(disp, bar.win, "Bar");
		// Create drawing context
			bar.gc = XCreateGC(disp, bar.win, 0, 0);
		// Create XFT gc + font + text + col
			bar.xftfont = XftFontOpenName(disp, XDefaultScreen(disp), FONT);
			if (!bar.xftfont) die("Failed to open font \"" FONT "\"");
			bar.xftfontinfo = XftFontInfoCreate(disp, bar.xftfont->pattern);
			if (!bar.xftfontinfo) die("Failed to open font info");
			bar.xftgc = XftDrawCreate(disp, bar.win, bar.visual.visual, bar.attr.colormap);
			if (!bar.xftgc) die("Failed to allocate xft gc\n");
			bar.xfttext = malloc(256 * sizeof(FcChar8));
			bar.xftfg1.pixel = COLFG1;
			bar.xftfg1.color.red   = (COLFG1 & 255) << 8;
			bar.xftfg1.color.green = ((COLFG1 >>  8) & 255) << 8;
			bar.xftfg1.color.blue  = ((COLFG1 >> 16) & 255) << 8;
			bar.xftfg1.color.alpha = (COLFG1 >> 24) << 8;
			bar.xftfg2.pixel = COLFG2;
			bar.xftfg2.color.red   = (COLFG2 & 255) << 8;
			bar.xftfg2.color.green = ((COLFG2 >>  8) & 255) << 8;
			bar.xftfg2.color.blue  = ((COLFG2 >> 16) & 255) << 8;
			bar.xftfg2.color.alpha = (COLFG2 >> 24) << 8;
			bar.xftfg3.pixel = COLFG3;
			bar.xftfg3.color.red   = (COLFG3 & 255) << 8;
			bar.xftfg3.color.green = ((COLFG3 >>  8) & 255) << 8;
			bar.xftfg3.color.blue  = ((COLFG3 >> 16) & 255) << 8;
			bar.xftfg3.color.alpha = (COLFG3 >> 24) << 8;
			
			bar.selrect[0].y = 0;
			bar.selrect[1].x = BARDESKWIDTH + BARDESKWIDTH / 8; //
			bar.selrect[1].y = 0; 
			bar.selrect[2].x = -BARDESKWIDTH / 4; //
			bar.selrect[2].y = BARHEIGHT; 
			bar.selrect[3].x = -(BARDESKWIDTH + BARDESKWIDTH / 8); //
			bar.selrect[3].y = 0;
			
			bar.winrect[0].y = 0;
			bar.winrect[1].y = 0; 
			bar.winrect[2].x = -BARDESKWIDTH / 4; //
			bar.winrect[2].y = BARHEIGHT; 
			bar.winrect[3].x = -50;
			bar.winrect[3].y = 0;
			
		XMapWindow(disp, bar.win);
		XSync(disp, False);
		updatebar = &updatebarreal;
	}

	Client *wintoclient(Window w) {
		Client *c;
//		for (unsigned int d = 0; d < DESKTOPNUM; ++d) {
			c = desks[desk];
			if (c == NULL) return NULL; // this desktop has no clients
			do {
				if (c->win == w) return c;
			} while ((c = c->next) != NULL);
//		}
		return NULL;
	}

	static void deskadd(Client *c) {
		desksnum[c->desk] += 1;
		if (desks[c->desk]) grabbtns(desks[c->desk]->win, 0); // unfocus previous window
		// add client to stack;
		c->last = NULL;
		c->next = desks[c->desk];
		if (c->next) c->next->last = c;
		desks[c->desk] = c;
	}
	static void deskremove(Client *c) {
		desksnum[c->desk] -= 1;
		if (c->next == NULL) { // top of the stack
			if (c->last == NULL) { // only 1 window left (c)
				desks[c->desk] = NULL;
			} else {
				c->last->next = NULL;
			}
		} else if (c->last == NULL) { // bottom of the stack
			c->next->last = NULL; // create new bottom of stack
			desks[c->desk] = c->next;
		} else { // somewhere imbetween (c->next && c->last)
			c->last->next = c->next; // remove c from stack
			c->next->last = c->last;
		}
		if (c->desk == desk) { // if on this desk
			updatebar();
			focus(desks[desk]);
		}
	}

	void grabkeys(Window w) {
		XUngrabKey(disp, AnyKey, AnyModifier, w);
		for (unsigned int i = 0; i < LEN(keys); ++i) {
			for (unsigned int m = 0; m < LEN(mods); ++m) {
				XGrabKey(
					disp,
					XKeysymToKeycode(disp, keys[i].key),
					keys[i].mod | mods[m], 
					root,
					True, GrabModeAsync, GrabModeAsync
				);
			}
		}
	}

	void grabbtns(Window w, int focus) {
		XUngrabButton(disp, AnyButton, AnyModifier, w);
		if (focus) {
			for (unsigned int i = 0; i < LEN(btns); ++i)
				if (btns[i].clk == ClkClient) {
					XGrabButton(
						disp,
						btns[i].btn, btns[i].mod,
						w, False,
						ButtonPressMask | ButtonReleaseMask,
						GrabModeAsync, GrabModeSync,
						None, None
					);
				}
		} else {
			XGrabButton(
				disp,
				AnyButton, AnyModifier,
				w, False, 
				ButtonPressMask | ButtonReleaseMask,
				GrabModeAsync, GrabModeSync, 
				None, None
			);
		}
	}

	void focus(Client *c) {
		if (c == NULL) {
			if (desks[desk]) grabbtns(desks[desk]->win, 0);
			XSetInputFocus(disp, root, RevertToPointerRoot, CurrentTime);
			//XDeleteProperty(disp, root, netatom[NetActiveWindow]);
		} else {
			if (c->desk != desk) return;
			grabbtns(desks[desk]->win, 0); // unfocus previous client
			XSetInputFocus(disp, c->win, RevertToPointerRoot, CurrentTime);
			grabbtns(c->win, 1);
			/*XChangeProperty(disp, root, netatom[NetActiveWindow],
				XA_WINDOW, 32, PropModeReplace,
				(unsigned char *) &(c->win), 1
			);
			XEvent e;
			e.type = ClientMessage;
			e.xclient.window = c->win;
			e.xclient.message_type = wmatom[WMProtocols];
			e.xclient.format = 32;
			e.xclient.data.l[0] = wmatom[WMTakeFocus];
			e.xclient.data.l[1] = CurrentTime;
			XSendEvent(disp, c->win, False, NoEventMask, &e);*/
			
			XRaiseWindow(disp, c->win);
			
			if (desks[desk]->win != c->win) { // make sure isnt already at the top
				// remove c from stack
				if (c->next) c->next->last = c->last;
				if (c->last) c->last->next = c->next;
				// add c to front of stack
				c->last = NULL;
				desks[desk]->last = c;
				c->next = desks[desk];
				desks[desk] = c;
			}
			// move top windows up
			if (c != NULL && !c->istop) // make sure c isnt NULL and that the focused window isn't already ontop
				do {
					if (c->istop) XRaiseWindow(disp, c->win);
				} while ((c = c->next) != NULL);

			XSync(disp, False);
		}
	}

	void resize(Client *c, int x, int y, int w, int h) {
		XWindowChanges wc;
		c->x = wc.x      = x;
		c->y = wc.y      = y;
		c->h = wc.height = h;
		c->w = wc.width  = w;
		XConfigureWindow(disp, c->win, CWX|CWY|CWWidth|CWHeight, &wc);
	}

	// Actions
		static void a_spawn(const Arg *arg) {
			if (fork() != 0) return;
			printf("swm: exec %s\n", ((char**)arg)[0]);
			setsid();
			//char *cmd[] = {"/usr/local/bin/st", NULL}; // NULL terminated array of char* strings
			//execvp(cmd[0], cmd);
			execvp(((char**)arg)[0], (char**)arg);
			exit(1);
		}
		static void a_mousemove(const Arg *arg) {
			XEvent e;
			Time lasttime = 0;
			Client *c = desks[desk];
			int x, y, nx, ny, ocx, ocy;
			focus(c);
			XUngrabPointer(disp, CurrentTime);
			if (XGrabPointer(
				disp, root, 
				False, PointerMotionMask | ButtonReleaseMask,
				GrabModeAsync, GrabModeAsync,
				None, cursor[CurMove], CurrentTime
			) != GrabSuccess) return;
			if (!XQueryPointer(disp, root, (Window *)&dummy, (Window *)&dummy, &x, &y, (int *)&dummy, (int *)&dummy, (unsigned int *)&dummy)) return;
			ocx = c->x;
			ocy = c->y;
			do {
				XMaskEvent(disp, PointerMotionMask | ButtonReleaseMask, &e);
				if (e.type == MotionNotify) {
					if ((e.xmotion.time - lasttime) <= (1000 / 60)) continue; // restrict to 60fps
					lasttime = e.xmotion.time;
					nx = ocx + (e.xmotion.x - x);
					ny = ocy + (e.xmotion.y - y);
					resize(c, nx, ny, c->w, c->h);
				}
			} while (e.type != ButtonRelease);
			XUngrabPointer(disp, CurrentTime);
			XSync(disp, True); // destroy extra events
		}
		static void a_mouseresize(const Arg *arg) {
			(void)arg;
			XEvent e;
			Time lasttime = 0;
			Client *c = desks[desk];
			int w, h, x, y, nx, ny;
			nx = x = c->x;
			ny = y = c->y;
			focus(c); // TODO nearest corner
			if (XGrabPointer(
				disp, root, 
				False, PointerMotionMask | ButtonReleaseMask, 
				GrabModeAsync, GrabModeAsync,
				None, cursor[CurResize], CurrentTime
			) != GrabSuccess) return;
			XWarpPointer(disp, None, c->win, 0, 0, 0, 0, c->w, c->h);
			XSync(disp, False);
			do {
				XMaskEvent(disp, PointerMotionMask | ButtonReleaseMask, &e);
				if (e.type == MotionNotify) {
					if ((e.xmotion.time - lasttime) <= (1000 / 60)) continue; // restrict to 60fps
					lasttime = e.xmotion.time;
					w = e.xmotion.x - x;
					h = e.xmotion.y - y;
					nx = x;
					ny = y;
					if (w < 10) w = 10;
					if (h < 10) h = 10;
					resize(c, nx, ny, w, h);
				}
			} while (e.type != ButtonRelease);
			XUngrabPointer(disp, CurrentTime);
			XSync(disp, True); // destroy extra events
		}
		static void a_resize(const Arg *arg) {
			resize(
				desks[desk], 
				((int*)arg)[0], ((int*)arg)[1],
				((int*)arg)[2], ((int*)arg)[3]
			);
		}
		static void a_tile(const Arg *arg) {
			static int x, y, w, h;
			switch ((enum Tile)arg) {
				case TileNW:
					x = GAP;
					y = GAP;
					w = W/2 - GAP*1.5;
					h = BARLESSWINHEIGHT/2 - GAP;
					break;
				case TileW:
					x = GAP;
					y = GAP;
					w = W/2 - GAP*1.5;
					h = BARLESSWINHEIGHT - GAP;
					break;
				case TileSW:
					x = GAP;
					y = BARLESSWINHEIGHT/2 + GAP;
					w = W/2 - GAP*1.5;
					h = BARLESSWINHEIGHT/2 - GAP;
					break;
				case TileN:
					x = GAP;
					y = GAP;
					w = W - GAP*2;
					h = BARLESSWINHEIGHT/2 - GAP;
					break;
				case TileFill:
					x = GAP;
					y = GAP;
					w = W - GAP*2;
					h = BARLESSWINHEIGHT - GAP;
					break;
				case TileS:
					x = GAP;
					y = BARLESSWINHEIGHT/2 + GAP;
					w = W - GAP*2;
					h = BARLESSWINHEIGHT/2 - GAP;
					break;
				case TileNE:
					x = W/2 + GAP*0.5;
					y = GAP;
					w = W/2 - GAP*1.5;
					h = BARLESSWINHEIGHT/2 - GAP;
					break;
				case TileE:
					x = W/2 + GAP*0.5;
					y = GAP;
					w = BARLESSWINHEIGHT/2 - GAP*1.5;
					h = BARLESSWINHEIGHT - GAP;
					break;
				case TileSE:
					x = W/2 + GAP*0.5;
					y = BARLESSWINHEIGHT/2 + GAP;
					w = W/2 - GAP*1.5;
					h = BARLESSWINHEIGHT/2 - GAP;
					break;
				case TileCenter:
					x = W/4 + GAP*0.75;
					y = BARLESSWINHEIGHT/4 - GAP*0.5;
					w = W/2 - GAP*1.5;
					h = BARLESSWINHEIGHT/2 - GAP;
					break;
				case TileFullscreen:
					x = 0;
					y = 0;
					w = W;
					h = H;
					break;
				case TileDoubleFullscreen:
					x = -W;
					y = -H;
					w = W * 2;
					h = H * 2;
					break;
       		}
			resize(
				desks[desk], 
				x, y, w, h
			);
		}
		static void a_deskview(const Arg *arg) {
			#define d (unsigned int)(long int)arg
			if (d == desk) return;
			Client *c = desks[desk];
			if ((c = desks[desk]) != NULL) { // check there are windows to hide		
				do { // hide all windows from current desktop
					XUnmapWindow(disp, c->win);
				} while ((c = c->next) != NULL);
			}
			desk = d;
			printf("swm: desktop %d\n", desk + 1);
			if ((c = desks[desk]) != NULL) { // check there are windows to show
				do { // hide all windows from current desktop
					XMapWindow(disp, c->win);
				} while ((c = c->next) != NULL);
			}
			focus(desks[desk]);
			updatebar();
			#undef d
		}
		static void a_deskmove(const Arg *arg) {
			#define d (unsigned int)(long int)arg
			if (d == desk) return;
			Client *c = desks[desk];
			deskremove(c);
			XUnmapWindow(disp, c->win);
			c->desk = d;
			deskadd(c);
			XMapWindow(disp, c->win);
			if (desks[desk]) { // don't focus new top if no more clients
				XSetInputFocus(disp, desks[desk]->win, RevertToPointerRoot, CurrentTime);
				XRaiseWindow(disp, desks[desk]->win);
			}
			#undef d
		}

		static Client *a_switchclientnext  = NULL;
		static Client *a_switchclientstart = NULL;
		static Client *a_switchclientcur   = NULL;
		static void a_switchstart(const Arg *arg) {
			(void)arg;
			a_switchclientstart = desks[desk];
			if (a_switchclientstart != NULL) {
				a_switchclientnext  = a_switchclientstart->next;
				if (a_switchclientnext == NULL) a_switchclientstart = NULL;
			}
		}
		static void a_switch(const Arg *arg) {
			(void)arg;
			if (a_switchclientstart == NULL) return;
			a_switchclientcur = a_switchclientnext->next;
			focus(a_switchclientnext);
			updatebar();
			a_switchclientnext = a_switchclientcur;
			if (a_switchclientnext == NULL) a_switchclientnext = a_switchclientstart;
		}
		static void a_kill(const Arg *arg) {
			(void)arg;
			Client *c = desks[desk];
			if (!c) return;
			int n;
			Atom *protocols;
			int exists = 0;
			XEvent e;
			if (XGetWMProtocols(disp, c->win, &protocols, &n)) {
				while (!exists && n--)
					exists = protocols[n] == wmatom[WMDelete];
				XFree(protocols);
			}
			if (exists) {
				e.type = ClientMessage;
				e.xclient.window = c->win;
				e.xclient.message_type = wmatom[WMProtocols];
				e.xclient.format = 32;
				e.xclient.data.l[0] = wmatom[WMDelete];
				e.xclient.data.l[1] = CurrentTime;
				XSendEvent(disp, c->win, False, NoEventMask, &e);
			} else {
				XGrabServer(disp);
				#ifndef NOERR
					XSetErrorHandler(xerrordummy);
				#endif
				XSetCloseDownMode(disp, DestroyAll);
				XKillClient(disp, c->win);
				XSync(disp, False);
				#ifndef NOERR
					XSetErrorHandler(xerror);
				#endif
				XUngrabServer(disp);
			}
		}
		static void a_toggletop(const Arg *arg) {
			(void)arg;
			if (desks[desk]->istop == 0) {
				desks[desk]->istop = 1;
				XRaiseWindow(disp, desks[desk]->w);
			} else {
				desks[desk]->istop = 0;
			}
			updatebar();
		}
		static void a_quit(const Arg *arg) {
			stopped = 1;
		}

	// Events
		static inline void e_buttonpress(XEvent e) {
			static Client *c;
			#if 1
				c = desks[desk];
				if (c == NULL) { // empty
					printf("swm: desktop empty\n");
				} else {
					printf("swm: desktop %d):\n", desksnum[desk]);
					unsigned int i = 0;
					do {
						printf("\t%u: %lu %s", i, c->win, c->title);
						if (c->istop) printf(" (ontop)");
						//if (c->last) printf("\t\tL: %lu", c->last->win);
						//if (c->next) printf("\t\tN: %lu", c->next->win);
						putchar('\n');
						++i;
					} while ((c = c->next) != NULL);
				}
			#endif
			static unsigned int clk;
			if (e.xbutton.window == root) {
				clk = ClkRoot;
			} else if ((c = wintoclient(e.xbutton.window)) != NULL) {
				clk = ClkClient;
				focus(c);
				XAllowEvents(disp, ReplayPointer, CurrentTime);
			}
			for (unsigned int i = 0; i < LEN(btns); ++i) {
				if (
					clk == btns[i].clk && e.xbutton.button == btns[i].btn && 
					CLEANMASK(btns[i].mod) == CLEANMASK(e.xbutton.state)
				) {
					btns[i].fun(btns[i].arg);
				}
			}
		}
		static inline void e_keypress(XEvent e) {
			static KeySym key;
			key = XkbKeycodeToKeysym(disp, e.xkey.keycode, 0, 0);
			for (unsigned int i = 0; i < LEN(keys); ++i) {
				if (key == keys[i].key && CLEANMASK(e.xkey.state) == CLEANMASK(keys[i].mod)) {
					keys[i].fun(keys[i].arg);
				}
			}
			XSync(disp, True);
		}
		static inline void e_maprequest(XEvent e) {
			static Client *c;
			c = manage(e.xmaprequest.window);
			if (!c) return;
			deskadd(c);
			resize(c,
				(W - c->w) / 2, (H - c->h) / 2,
				c->w, c->h
			);
			focus(c);
		}
		static inline void e_destroynotify(XEvent e) {
			static Client *c;
			c = wintoclient(e.xdestroywindow.window);
			if (c == NULL) return; // just in case
			deskremove(c);
			free(c);
		}
		static inline void e_configurerequest(XEvent e) {
			static Client *c;
			if ((c = wintoclient(e.xconfigurerequest.window)) != NULL) {
				if (e.xconfigurerequest.value_mask & CWX     ) c->x = e.xconfigurerequest.x;
				if (e.xconfigurerequest.value_mask & CWY     ) c->y = e.xconfigurerequest.y;
				if (e.xconfigurerequest.value_mask & CWWidth ) c->w = e.xconfigurerequest.width;
				if (e.xconfigurerequest.value_mask & CWHeight) c->h = e.xconfigurerequest.height;
			} /*else {
				XPutBackEvent(disp, &e); // a window tried to configure before mapping
			}*/
		}
		void e_propertynotify(XEvent e) {
			static Client *c;
			if (e.xproperty.window == root) {
				
			} else if ((c = wintoclient(e.xproperty.window))) {
				/*switch(ev.xproperty.atom) {
					case XA_WM_NORMAL_HINTS:
						updatesizehints(c);
						break;
					case XA_WM_HINTS:
						drawbars();
						break;
					default: break; // stop warn
				}*/
				if (e.xproperty.atom == XA_WM_NAME || e.xproperty.atom == netatom[NetWMName]) {
					updatetitle(c);
					updatebar();
				}
			}
		}
		void e_clientmessage(XEvent e) {
			if (
				e.xproperty.window == root &&
				e.xclient.message_type == wmatom[WMProtocols] &&
				e.xclient.data.l[0] == wmatom[WMStatus]
			) {
				setroottext();
				printf("The thign we found was %s\n", roottext);
				updatebar();
			}
		}

void signalint(int _) {
	printf("swm: Stopped with ctrl-c\n");
	stopped = 1;
	signal(SIGINT, SIG_DFL);
}

int main() {

	// Install handelers
		struct sigaction signalintact;
		signalintact.sa_handler = signalint;
		sigaction(SIGINT, &signalintact, NULL);

	// Setup Xlib
		printf("Display: %s\n", getenv("DISPLAY"));
		if ((disp = XOpenDisplay(NULL)) == NULL) die("Failed to open Display");
		root = XDefaultRootWindow(disp);
		XGetWindowAttributes(disp, root, &rootattrs);

	// Check for other wms (by checking if root is taken)
		#ifndef NOERR
			XSetErrorHandler(xerrorstart);
		#endif
		XSelectInput(disp, root, SubstructureRedirectMask);
		XSync(disp, False);
		#ifndef NOERR
			XSetErrorHandler(xerror);
		#endif
		XSync(disp, False);

	// Init desks;
		for (unsigned int i = 0; i < LEN(desks); ++i) desks[i] = NULL;

	// Setup atoms
		wmatom[WMProtocols]            = XInternAtom(disp, "WM_PROTOCOLS",               False);
		wmatom[WMDelete]               = XInternAtom(disp, "WM_DELETE_WINDOW",           False);
		wmatom[WMState]                = XInternAtom(disp, "WM_STATE",                   False);
		wmatom[WMTakeFocus]            = XInternAtom(disp, "WM_TAKE_FOCUS",              False);
		wmatom[WMStatus]               = XInternAtom(disp, "WM_STATUS",                  False);
		netatom[NetActiveWindow]       = XInternAtom(disp, "_NET_ACTIVE_WINDOW",         False);
		netatom[NetSupported]          = XInternAtom(disp, "_NET_SUPPORTED",             False);
		netatom[NetWMName]             = XInternAtom(disp, "_NET_WM_NAME",               False);
		netatom[NetWMState]            = XInternAtom(disp, "_NET_WM_STATE",              False);
		netatom[NetWMCheck]            = XInternAtom(disp, "_NET_SUPPORTING_WM_CHECK",   False);
		netatom[NetWMFullscreen]       = XInternAtom(disp, "_NET_WM_STATE_FULLSCREEN",   False);
		netatom[NetWMWindowType]       = XInternAtom(disp, "_NET_WM_WINDOW_TYPE",        False);
		netatom[NetWMWindowTypeDialog] = XInternAtom(disp, "_NET_WM_WINDOW_TYPE_DIALOG", False);
		netatom[NetClientList]         = XInternAtom(disp, "_NET_CLIENT_LIST",           False);

	// Setup cursors
		cursor[CurNormal] = XCreateFontCursor(disp, XC_left_ptr);
		cursor[CurResize] = XCreateFontCursor(disp, XC_sizing);
		cursor[CurMove]   = XCreateFontCursor(disp, XC_fleur);

	// Setup window mgr
		XSetWindowAttributes wa;
		wa.cursor = cursor[CurNormal];
		wa.event_mask = ButtonPress | KeyPress | SubstructureRedirectMask | SubstructureNotifyMask | PropertyChangeMask;
		XChangeWindowAttributes(disp, root, CWEventMask | CWCursor, &wa);
		grabkeys(root);

	// Scan for windows
		scan();

	if (desks[desk]) focus(desks[desk]);
	else             focus(NULL);
	XSelectInput(disp, root, wa.event_mask);
	createbar();

	// Roottext
		// if (fork() != 0) {
		// 	Display *tempdisp = XOpenDisplay(NULL);
		// 	while (1) {
		// 		XEvent statusevent;
		// 		statusevent.type = ClientMessage;
		// 		statusevent.xclient.window = root;
		// 		statusevent.xclient.message_type = wmatom[WMProtocols];
		// 		statusevent.xclient.format = 32;
		// 		statusevent.xclient.data.l[0] = wmatom[WMStatus];
		// 		statusevent.xclient.data.l[1] = CurrentTime;
		// 		XSendEvent(tempdisp, root, False, SubstructureRedirectMask, &statusevent);
		// 		XSync(tempdisp, False);
		// 		sleep(1);
		// 	}
		// 	exit(0);
		// }
	
	// Autostart
		const char **p = autostart;
		unsigned int i = 0;
		while (*p) {
			printf("swm: exec '%s'\n", *p);
			dummy = fork();
			if (dummy == 0) {
				setsid();
				execvp(*p, (char *const *)p);
				exit(1);
			}
			// Add pid to pids
			autostartpids[i] = dummy;
			++i;
			// skip arguments
			while (*(++p));
			++p;
		}

	// Event Loop
		XEvent e;
		while (!XNextEvent(disp, &e) && stopped == 0) {
			switch (e.type) {
				case Expose:           updatebar();           break;
				case ButtonPress:      e_buttonpress(e);      break;
				case KeyPress:         e_keypress(e);         break;
				case MapRequest:       e_maprequest(e);       break;
				case DestroyNotify:    e_destroynotify(e);    break;
				case ConfigureRequest: e_configurerequest(e); break;
				case PropertyNotify:   e_propertynotify(e);   break;
				case ClientMessage:    e_clientmessage(e);    break;
			}
		}

	// Free stuff

		for (unsigned int i = 0; i < LEN(autostart); ++i)
			kill(autostartpids[i], SIGTERM);

		XDestroyWindow(disp, bar.win);
		XFreeGC(disp, bar.gc);
		if (bar.xftfont->pattern)
			FcPatternDestroy(bar.xftfont->pattern);
		XftFontClose(disp, bar.xftfont);

		XDeleteProperty(disp, root, netatom[NetActiveWindow]);
		XUngrabKey(disp, AnyKey, AnyModifier, root);
		XSetInputFocus(disp, PointerRoot, RevertToPointerRoot, CurrentTime);

		XSync(disp, False);
		XCloseDisplay(disp);

		for (unsigned int i = 0; i < LEN(autostart); ++i)
			waitpid(autostartpids[i], NULL, 0);

		printf("swm: Bye!");

	return 0;
}
