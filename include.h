#ifndef INCLUDE_H
#define INCLUDE_H

// Includes

	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <X11/keysym.h>
	#include <X11/XKBlib.h>
	#include <X11/cursorfont.h>
	#include <X11/Xatom.h>
	#include <X11/Xft/Xft.h>
	
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>

// Enums

	enum { NetSupported, NetWMName, NetWMState, NetWMCheck, NetWMFullscreen, NetActiveWindow, NetWMWindowType, NetWMWindowTypeDialog, NetClientList, NetLast };
	enum { WMProtocols, WMDelete, WMState, WMTakeFocus, WMLast };
	enum { CurNormal, CurResize, CurMove, CurLast };
	enum { ClkRoot, ClkClient };

// Structs

	typedef struct {
		const void* v;
	} Arg;

	typedef struct {
		unsigned int mod;
		KeySym       key;
		void       (*fun)(const Arg*);
		const Arg   *arg;
	} Key;

	typedef struct {
		unsigned int clk;
		unsigned int mod;
		unsigned int btn;
		void       (*fun)(const Arg*);
		const Arg   *arg;
	} Btn;

	struct Client {
		struct Client *last; // 64
		struct Client *next; // 64
		Window         win; // 64
		unsigned int   desk; // 32
		int            x, y, w, h; // 32
		char           title[256]; // 256 * 8
		unsigned char  titlelen; // 8
		int            istop : 1; // 1
	};
	typedef struct Client Client;

// Functions

	// Actions
		static void a_spawn(const Arg *arg);
		static void a_mousemove(const Arg *arg);
		static void a_mouseresize(const Arg *arg);
		static void a_resize(const Arg *arg);
		static void a_deskview(const Arg *arg);
		static void a_deskmove(const Arg *arg);
		static void a_startalttab(const Arg *arg);
		static void a_alttab(const Arg *arg);
		static void a_kill(const Arg *arg);
		static void a_toggletop(const Arg *arg);

	// Events
		static inline void e_buttonpress(XEvent e);
		static inline void e_keypress(XEvent e);
		static inline void e_maprequest(XEvent e);
		static inline void e_destroynotify(XEvent e);
		static inline void e_configurerequest(XEvent e);
		static inline void e_propertynotify(XEvent e);


#endif
