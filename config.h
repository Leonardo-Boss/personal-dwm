/* See LICENSE file for copyright and license details. */
#include <X11/X.h>
#include <X11/XF86keysym.h>
#define SESSION_FILE "/tmp/dwm-session"
#define STATUSBAR "dwmblocks"

/* appearance */
static const unsigned int borderpx  = 5;        /* border pixel of windows */
static const unsigned int gappx     = 6;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 1;        /* 1 means swallow floating windows by default */
static const int showbar            = 0;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const double activeopacity   = 1;     /* Window opacity when it's focused (0 <= opacity <= 1) */
static const double inactiveopacity = 0.9f;     /* Window opacity when it's inactive (0 <= opacity <= 1) */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

static const char *const autostart[] = {
	"dunst", NULL,
	"picom", NULL,
	"dwmblocks", NULL,
	"random_wallpaper", NULL,
	"alacritty", NULL,
	"flatpak", "run", "io.gitlab.librewolf-community", NULL,
    "ibus", "start", NULL,
	NULL /* terminate */
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

#include "shiftview.c"

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  opacity       	unfocusopacity  	isterminal  noswallow  monitor  scratch key iscentered noborders*/
	{ "LibreWolf", NULL,     NULL,         2,         0,          activeopacity,	inactiveopacity,	0,         -1,        -1,       0,	        0,         0 },
	{ "discord",   NULL,     NULL,         3,         0,          activeopacity,	inactiveopacity,	0,         -1,        -1,       0,	        0,         0 },
	{ "Alacritty", NULL,     NULL,         0,         0,          activeopacity,	inactiveopacity,	1,          0,        -1,       0,	        0,         0 },
	{ "Alacritty", NULL,   "scratchterm",  0,         1,          activeopacity,	inactiveopacity,	1,          0,        -1,      's',         1,         0 },
	{ NULL,        "pavuscratch", NULL,    0,         1,          activeopacity,	inactiveopacity,	0,          0,        -1,      'v',         1,         0 },
	{ NULL,        "spotify",     NULL,    0,         1,          activeopacity,	inactiveopacity,	0,          0,        -1,      'm',         1,         0 },
	{ "org.gnome.Nautilus", NULL, NULL,    0,         0,          activeopacity,	inactiveopacity,	0,          0,        -1,      'n',         1,         0 },
	{ NULL,        NULL,   "Event Tester", 0,         0,          activeopacity,	inactiveopacity,	0,          1,        -1,       0,	        0,         0 }, /* xev */
	{ "mpv",       NULL,     NULL,         0,         0,          1,            	inactiveopacity,    0,          0,        -1,       0,	        0,         1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 0; /* 1 will force focus on the fullscreen window */

/*First arg only serves to match against key in rules*/
static const char *scratchpadcmd[] = {"s", "alacritty", "-T", "scratchterm", NULL}; 
static const char *scratchpadpavucontrol[] = {"v", "pavuscratch", NULL}; 
static const char *scratchpadspotify[] = {"m", "flatpak", "run", "com.spotify.Client", NULL}; 
static const char *scratchpadnautilus[] = {"n", "nautilus", NULL}; 

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "TTT",      bstack },
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu-run-extended", NULL };
static const char *termcmd[]  = { "alacritty", NULL };
static const char *random_wallpaper[]  = { "random_wallpaper", NULL };
static const char *dark_toggle[] = { "darkman", "toggle", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_u,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY|ControlMask,	        XK_comma,  cyclelayout,    {.i = -1 } },
	{ MODKEY|ControlMask,           XK_period, cyclelayout,    {.i = +1 } },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_Page_Up, shiftview,     {.i = +1 } },
	{ MODKEY,                       XK_Page_Down, shiftview,   {.i = -1 } },
    { 0,        	                XK_Print,  spawn,	       SHCMD("/home/eppi/.local/bin/screenshot") },
    { ShiftMask,	                XK_Print,  spawn,	       SHCMD("/home/eppi/.local/bin/screenshotsel") },
    { MODKEY|ShiftMask,             XK_a,      changefocusopacity,   {.f = +0.1}},
    { MODKEY|ShiftMask,             XK_s,      changefocusopacity,   {.f = -0.1}},
	{ MODKEY|ShiftMask,             XK_z,      changeunfocusopacity, {.f = +0.1}},
    { MODKEY|ShiftMask,             XK_x,      changeunfocusopacity, {.f = -0.1}},
    { MODKEY|ShiftMask,             XK_t,      spawn,          {.v = dark_toggle }},
	{ MODKEY,                       XK_s,      togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY|ShiftMask,             XK_v,      togglescratch,  {.v = scratchpadpavucontrol } },
	{ MODKEY|ShiftMask,             XK_m,      togglescratch,  {.v = scratchpadspotify } },
	{ MODKEY|ShiftMask,             XK_n,      togglescratch,  {.v = scratchpadnautilus } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_q,      quit,           {1} }, 
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkStatusText,        0,              Button1,        sigstatusbar,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigstatusbar,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigstatusbar,   {.i = 3} },
	{ ClkStatusText,        ShiftMask,      Button1,        sigstatusbar,   {.i = 6} },
	{ ClkStatusText,        ShiftMask,      Button3,        spawn,          SHCMD("alacritty -e nvim ~/Downloads/git/dwmblocks-async/config.c")},
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

