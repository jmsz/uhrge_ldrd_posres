/*==> y2dplot library  <======================================//

Display arrays of different data types

//--- Version ------------------------------------------------//

Version: 2.0
Date:    2012

//--- Copyright ----------------------------------------------//

Copyright (C) by MIZZI Computer Software GmbH, Germany.

This software is furnished under a license and may be used
and copied only in accordance with the terms of such
license and with the inclusion of the above copyright
notice.

This software or any other copies thereof may not be
provided or otherwise made available to any other person.
No title to and ownership of the software is hereby
transferred.

The information in this software is subject to change
without notice and should not be construed as a commitment
by the holder of the copyright.

//------------------------------------------------------------*/

/*--- Include ------------------------------------------------*/

//#include "y2dplot.h"

/*------------------------------------------------------------*/

/*============================================================*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// utilities //////////////////////////////////////////////////////
// used inside only
// getkeypress(int mode);

/* Description ------------------------------------------------//

read in and return one char with different modes
if eof is detected 0 is returned.

mode 0: default mode
        set the character mode to default mode (normally
        same as mode 1) and reject the eof condition.
        no character is read from standart input but at
        interactiv devices the terminal mode is resetted
        to the mode at program startup.
        returned value is 0.

mode 1: line io mode
        at interactive devices like terminals, the job
        will wait for input and the input is formatted in
        lines. characters are echoed.

mode 2: character io mode
        read character by character from interactiv devices
        characters are not broken in lines and c characters
        like \b are transfered to the user buffer, but they
        are translated correctly. e.g. on vax a delete
        (ascii 127) will result in a \b (ascii 8). at
        terminals characters are not echoed. not interactiv
        devices (disk files etc.) return 0.

mode 3: direct io mode
        same as mode 2 but if no character is available
        at interactiv devices the job does not wait for
        input. on not interactiv devices like disk
        mode = 3 returns 0. this mode is primarily used
        to check input at the controlling keybord.

mode -1: unget character
        push back the last read character in the read buffer.
        you can not push back more than one character and
        you can not push back the eof char (0).

NOTE: if you mix stdio and IO calls you must switch at
SOME operating systems to mode 0 or your characters are
read with no echo etc.

INFO: the getkeypress function is very fast if the mode is not
switched between the calls. previously buffered characters
read by mode 1 are returned also by mode 2 and 3.

//-------------------------------------------------------------*/

#include <unistd.h>

static int unixchan=2; /* unix interactive channel */

#ifdef bsd4_2

#include <sgtty.h>
#include <fcntl.h>
#include <sys/file.h>

static struct sgttyb ttybuf;
static short ttyoldflags=0;

#else /* system five term definition */

#include <termio.h>
static struct termio ttybuf;
static unsigned short ttyoldflags=0;
static char eofchr=0;

#endif

static int  unixttyio=0;
static int  iomode=0;                           /* current io mode */
// static char outbuffer[256];                  /* output buffers */
// static int  outcnt=0;                           /* output count */
static char inbuffer[256];                      /* input buffers */
static int  incnt=0;                            /* input count */


/*******************************************************/
static int unixiomode(int mode)
/*******************************************************/
{
/* printf("setiomode %d\n",mode); */

#ifdef bsd4_2
 if(unixttyio==0)
 {
   if (isatty(unixchan))
   {
     ioctl(unixchan,TIOCGETP,&ttybuf);
     ttyoldflags=ttybuf.sg_flags;
     unixttyio=1;
   }
   else unixttyio=(-1);
 }
#else
 if(unixttyio==0)
 {
   if (isatty(unixchan))
   {
     ioctl(unixchan,TCGETA,&ttybuf);
     ttyoldflags=ttybuf.c_lflag;
     eofchr=ttybuf.c_cc[VEOF];
     unixttyio=1;
   }
   else unixttyio=(-1);
 }
#endif

if(unixttyio <= 0) return 0;
if(mode < 0 )      return 0;
iomode=mode;

#ifdef bsd4_2
 if( mode>1) ttybuf.sg_flags = (ttyoldflags | CBREAK) & (~ECHO);
 else        ttybuf.sg_flags = ttyoldflags;
 if (ioctl(unixchan,TIOCSETN,&ttybuf) == -1) write(2,"IO.ERROR: SET ioctl\n",20);
 // { int n=0; ioctl(unixchan,TIOCFLUSH,&n); } /* flush in out, not nessesary */
 write(1,"\0",1);  /* send 0 byte ??? bug in pseudo terminal driver or atty */
#else
 if( mode>1)
 {
   ttybuf.c_lflag=ttyoldflags & (~(ICANON|ECHO|ISIG));
   ttybuf.c_cc[VTIME]=0;
   if(mode>2) ttybuf.c_cc[VMIN]=0;
   else       ttybuf.c_cc[VMIN]=1;
 }
 else
 {
   ttybuf.c_cc[VTIME]=0;
   ttybuf.c_cc[VMIN]=1;
   ttybuf.c_cc[VEOF]=eofchr;
   ttybuf.c_lflag=ttyoldflags;
 }
 ioctl(unixchan,TCSETA,&ttybuf);
#endif

return 0;
}


/*******************************************************/
int getkeypress(int mode)
/*******************************************************/
{
char chr;
static int ieof=0;
if(mode > 0)     { chr=inbuffer[incnt++]; if(chr!=0) return chr; }
if(mode==(-1))   { incnt--; if(incnt<0) incnt=0; return 0; }
if(mode!=iomode) unixiomode(mode);
switch(iomode)
{
  case  0: ieof=0; return 0;
  case  3: if(unixttyio<=0) return inbuffer[incnt=0]=0;
#ifdef bsd4_2
           { int nc; ioctl(unixchan,FIONREAD,&nc);
             if (nc < 1 ) return inbuffer[incnt=0]=0; }
#endif
  case  2: if(unixttyio==1)
           { chr=0;
             read(unixchan,&chr,1);
             if(chr==127)       chr='\b';
             else if(chr=='\r') chr='\n';
             inbuffer[0]=chr; inbuffer[1]=0; incnt=1;
             return chr; }
           else return inbuffer[incnt=0]=0;

  default:
  inbuffer[0]=0; incnt=0; if(ieof==1) return 0;
  fgets(inbuffer,256,stderr);
  chr=inbuffer[incnt]; if(chr!=0) { incnt++; return chr; }
  ieof=1; incnt=0; return 0;
}
}
// graphic terminal interface ////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct  GTfile_struct                 /* user interface structure */
{

/*
//      driver specific items
//      ---------------------
*/
        char    *name;          /* driver name */
        int     dsi[10];        /* driver specific int */
        void    *dsp[10];       /* driver specific pointers */

/*
//      terminal properties
//      -------------------
*/
        int     pixelsize;      /* no of pixels/meter */
        int     xpixels;        /* x pixels */
        int     ypixels;        /* y pixels */
        int     colors;         /* number of colors indexes */
        int     reds;           /* red colors */
        int     greens;         /* green colors */
        int     blues;          /* blue colors */
        int     fontysize;      /* character y pixels */
        int     fontxsize;      /* character x pixels */
/*
//      terminal reports
//      ----------------
*/
        const char *error;      /* last error text 0=noerror */

        int     eventtype;      /* event type  */
        int     keycode;        /* key pressed by user */
        int     xpointer;       /* x position of pointer */
        int     ypointer;       /* y position of pointer */

} ;


typedef struct GTfile_struct *GTfile;


/*  Macros  --------------------------------------------- */

/*
//  macros to access data structures used by programmer
//  ---------------------------------------------------
*/

#define GTpixelsize(gid)   (gid->pixelsize)     /* pixelsize in m */
#define GTxpixels(gid)     (gid->xpixels)       /* x pixels */
#define GTypixels(gid)     (gid->ypixels)       /* y pixels */
#define GTcolors(gid)      (gid->colors)        /* number of mapped colors */
#define GTreds(gid)        (gid->reds)          /* number of reds */
#define GTgreens(gid)      (gid->greens)        /* number of greens */
#define GTblues(gid)       (gid->blues)         /* number of blues */
#define GTfontysize(gid)   (gid->fontysize)     /* character y pixels */
#define GTfontxsize(gid)   (gid->fontxsize)     /* character x pixels */

#define GTerror(gid)       (gid->error) /* current error text */

#define GTeventtype(gid)   (gid->eventtype)     /* event type of last event */
#define GTkeycode(gid)     (gid->keycode)     /* key pressed by user */
#define GTxpointer(gid)    (gid->xpointer)      /* x pos of pointer */
#define GTypointer(gid)    (gid->ypointer)      /* y pos of pointer */

#define GTmaxcolor(gid)    (gid->colors-1)      /* number of mapped colors */
#define GTmaxred(gid)      (gid->reds-1)        /* number of reds */
#define GTmaxgreen(gid)    (gid->greens-1)      /* number of greens */
#define GTmaxblue(gid)     (gid->blues-1)       /* number of blues */

/*-------------------------------------------------------------*/

/* Constants --------------------------------------------------*/

/* graphics modes */

#define GTalpha         0
#define GTnographic     0
#define GTgraphic       1
#define GTinput         2
#define GToutput        3
#define GTnoinput       4
#define GTnooutput      5

/* pixelmodes */

#define GTbackground    0
#define GTreplace       1
#define GTcomplement    2
#define GTerase         3

/* event types */

#define GTnowait        0
#define GTwait          1

#define GTnoevent       0
#define GTkeyinput      1
#define GTxypointer     2
#define GTxymotion      3
#define GTexpose        4


/* key symbols */

#define GTkeyspecial(c) ((c)>10000)

#define GTkeyfunction 11000

#define GTkeyunknown  10001
#define GTkeyreturn   10002
#define GTkeydelete   10003
#define GTkeytab      10004

#define GTkeyup       10011
#define GTkeydown     10012
#define GTkeyleft     10013
#define GTkeyright    10014

#define GTkeyhelp     10021
#define GTkeyexecute  10022
#define GTkeyfind     10023
#define GTkeytop      10024
#define GTkeybottom   10025
#define GTkeynext     10026
#define GTkeyprevious 10027
#define GTkeyselect   10028
#define GTkeyremove   10029
#define GTkeyinsert   10030


/* Function ======================================================*/

int GTdebug(GTfile gid)

/* Description ---------------------------------------------------//

shows on standart output all characteristics of gt device id
saved in gid.

//----------------------------------------------------------------*/
{
printf("*** GTdebug ***\n");

printf("xpixels       %d\n",GTxpixels(gid));
printf("ypixels       %d\n",GTypixels(gid));
printf("colors        %d\n",GTcolors(gid));
printf("reds          %d\n",GTreds(gid));
printf("greens        %d\n",GTgreens(gid));
printf("blues         %d\n",GTblues(gid));
printf("fontxsize     %d\n",GTfontxsize(gid));
printf("fontysize     %d\n",GTfontysize(gid));

if(GTerror(gid))
printf("error         %s\n",GTerror(gid));
printf("eventtype     %d\n",GTeventtype(gid));
printf("keycode       %d\n",GTkeycode(gid));
printf("xpointer      %d\n",GTxpointer(gid));
printf("ypointer      %d\n",GTypointer(gid));


return 0;

}



// drivers /////////////////////////////////////////////////

// x11 driver
// xwindow  GT device driver function for x-window
// only one window is currently supported, black and white.

//-------------------------------------------------------------*/


#define XWindow 1
#ifdef XWindow  /* compile only for above system */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

/* search list for font names */

static  struct { int cxsize; int cysize; const char *name; } fontlist[]=
{
  { 9,15,"*-fixed-bold-r-normal--18-120-*"},
  { 10,17,"*Adobe-Courier-Bold-R-Normal*-17-*"},
  { 9,14,"*Adobe-Courier-Medium-R-Normal*-14-*"},
  { 9,14,"*Adobe-Courier-Medium-R-Normal*-14-*"},
  { 9,14,"*DEC-Terminal-Medium-R-Normal*-14-*" },
  { 8,13,"8x13" },
  { 6,12,"fixed" },
  { 0, 0,"" }
};

static  XSetWindowAttributes xwwattributes; /* window attribute struct */

static  Display *xwdid;                 /* X display id */
static  Window  xwwid;                  /* X window id */
static  GC      xwgc;                   /* X grafic contex */
static  int     xwsid;                  /* X screen identifier */
static  Font    xwfid;                  /* font ident. */
static  XEvent  xwevtid;                /* X event structure */
static  Window  xwlastfocus;            /* last input focus */
static  int     xwlastrevertto;         /* last revert to */

static int      init=0;

/*******************************************************************\
 open graphics device
\*******************************************************************/

static int xwopen(GTfile gid, const char *file, int xp, int yp, int xs, int ys)
{

   static char  name[101]={0}; /* window name */
   static int   xpos=0,ypos=0;       /* x and y position on screen */
   static int   xsize=700,ysize=500;     /* total size in x and y */
   static int   xwfontxsize=0,xwfontysize=0; /* fontsizes */
   int          mapnew;
   int i;
   // printf("pos,size  %d %d %d %d\n",xp,yp,xs,ys);
   mapnew=0;
   strncpy(name,file,100);
   if(xs>0 && xs!=xsize) mapnew=1, xsize=xs;
   if(ys>0 && ys!=ysize) mapnew=1, ysize=ys;
   if(xp>=0 && xp!=xpos) mapnew=1, xpos=xp;
   if(yp>=0 && yp!=ypos) mapnew=1, ypos=yp;

   if(init && mapnew) XDestroyWindow(xwdid,xwwid);
   if(!init)
   {
     xwdid=XOpenDisplay("");
     if(xwdid==0) { GTerror(gid)="ERROR GTDx11.xwopen: cant open display"; return -1; }
     xwsid=DefaultScreen(xwdid);
     mapnew=1;
     init=1;
   }
   if(mapnew)
   {
     XGetInputFocus(xwdid,&xwlastfocus,&xwlastrevertto);
     xwwid=XCreateSimpleWindow
          (xwdid,DefaultRootWindow(xwdid),xpos,ypos,xsize,ysize,
           5,BlackPixel(xwdid,xwsid),WhitePixel(xwdid,xwsid));
     xwgc=XCreateGC(xwdid,xwwid,0,0);
     XSetBackground(xwdid,xwgc,WhitePixel(xwdid,xwsid));
     XSetForeground(xwdid,xwgc,BlackPixel(xwdid,xwsid));

     /* set window manager properties */
     {
     XSizeHints xwwmhints;
     xwwmhints.width=xwwmhints.min_width=xwwmhints.max_width=xsize;
     xwwmhints.height=xwwmhints.min_height=xwwmhints.max_height=ysize;
     xwwmhints.x=xpos; xwwmhints.y=ypos;
     xwwmhints.flags=PPosition|PSize|PMinSize|PMaxSize;
     if(xpos!=0 && ypos!=0) xwwmhints.flags=xwwmhints.flags|USPosition|USSize;
     XSetStandardProperties(xwdid,xwwid,name,name,None,(char**)0,0,&xwwmhints);
     }
     /* load first font found in fontlist */

     xwfontysize=13; xwfontxsize=8;  /* default sizes */
     for(i=0;fontlist[i].cxsize!=0;i++)
     {
       int fonts;
       char **list;
       // printf("trying to load: %s\n",fontlist[i].name);
       list=XListFonts(xwdid,fontlist[i].name,30,&fonts);
       if(fonts>0)
       {
         xwfid=XLoadFont(xwdid,list[0]);
         XSetFont(xwdid,xwgc,xwfid);
         xwfontysize=fontlist[i].cysize;
         xwfontxsize=fontlist[i].cxsize;
         break;
       }
       XFreeFontNames(list);
     }
     XSelectInput(xwdid,xwwid,ExposureMask);
     XMapWindow(xwdid,xwwid);
     XFlush(xwdid);
     while(!XPending(xwdid));
     XSync(xwdid,True);
     XSelectInput(xwdid,xwwid,
       ButtonPressMask|PointerMotionMask|KeyPressMask|ExposureMask);
   }

   {
     XSizeHints xwwmhints;
     xwwmhints.width=xwwmhints.min_width=xwwmhints.max_width=xsize;
     xwwmhints.height=xwwmhints.min_height=xwwmhints.max_height=ysize;
     xwwmhints.x=xpos; xwwmhints.y=ypos;
     xwwmhints.flags=PPosition|PSize|PMinSize|PMaxSize;
     if(xpos!=0 && ypos!=0) xwwmhints.flags=xwwmhints.flags|USPosition|USSize;
     XSetStandardProperties(xwdid,xwwid,name,name,None,(char**)0,0,&xwwmhints);
   }
   if(XDoesBackingStore(XScreenOfDisplay(xwdid,xwsid))==Always)
   {
     xwwattributes.backing_store=Always;
     XChangeWindowAttributes(xwdid,xwwid,CWBackingStore,&xwwattributes);
   }
   XFlush(xwdid);
   GTfontysize(gid)=xwfontysize;         /* font sizes */
   GTfontxsize(gid)=xwfontxsize;
   GTxpixels(gid)=xsize;                /* max x pixels */
   GTypixels(gid)=ysize;                /* max y pixels */

   return 0;

}

/*******************************************************************\
 close graphic
\*******************************************************************/

static int xwclose(GTfile gid)
{
  XCloseDisplay(xwdid);
  init=0;
  return 0;
}

/*******************************************************************\
 select graphics device
\*******************************************************************/

static int xwselect(GTfile gid, int mode)
{
  if(mode==GTnoinput)
    XSetInputFocus(xwdid,xwlastfocus,xwlastrevertto,CurrentTime);
  else if(mode==GTinput)
    XSetInputFocus(xwdid,xwwid,RevertToParent,CurrentTime);
  else if(mode==GTnooutput)
    XLowerWindow(xwdid,xwwid);
  else if(mode==GToutput)
    XRaiseWindow(xwdid,xwwid);
  XSync(xwdid,True);   /* XSync(xwdid,False); */

  return 0;
}

/*******************************************************************\
 set color
\*******************************************************************/

static int xwcolor(GTfile gid, int funct,int index,int red, int green,int blue)
{
   if(funct==GTreplace)    XSetFunction(xwdid,xwgc,GXcopy);
   if(funct==GTcomplement) XSetFunction(xwdid,xwgc,GXinvert);
   if(funct==GTerase)      XSetFunction(xwdid,xwgc,GXclear);
   return 0;
}



/*******************************************************************\
 clear
\*******************************************************************/

static int xwclear(GTfile gid)
{
  XClearWindow(xwdid,xwwid);
  return 0;
}


/*******************************************************************\
 plot
\*******************************************************************/

static int xwplot(GTfile gid, int n, int len, int *x,int *y)
{
   int i,ib;
   int maxy=GTypixels(gid)-1;

   if(len==1) /* points */
   {
     XPoint vecpoints[200];
     for (ib=0,i=0;i<n;i++)
     { vecpoints[ib].x=(*x++),vecpoints[ib].y=maxy-(*y++); ib++;
       if(ib>199)
        XDrawPoints(xwdid,xwwid,xwgc,vecpoints,ib,CoordModeOrigin),ib=0;
     }
     if(ib>0) XDrawPoints(xwdid,xwwid,xwgc,vecpoints,ib,CoordModeOrigin);
     return 0;
   }

   else if(len==2) /* lines */
   {
     XSegment veclines[100];
     for (ib=0,i=0;i<n;i++)
     { veclines[ib].x1=(*x++),veclines[ib].y1=maxy-(*y++);
       veclines[ib].x2=(*x++),veclines[ib].y2=maxy-(*y++); ib++;
       if(ib>99)  XDrawSegments(xwdid,xwwid,xwgc,veclines,ib),ib=0;
     }
     if(ib>0) XDrawSegments(xwdid,xwwid,xwgc,veclines,ib);
     return 0;
   }
   return 0;
}

/*******************************************************************\
 draw text
\*******************************************************************/

static int xwtext(GTfile gid, int xpos, int ypos, int n, const char *text)
{
  int maxy=GTypixels(gid)-1;
  ypos=maxy-(ypos+GTfontysize(gid)*0.1);
  xpos=xpos+1;
  XDrawString(xwdid,xwwid,xwgc,xpos,ypos,text,n);
  return 0;
}

/*******************************************************************\
 get event
\*******************************************************************/

static int xwevent(GTfile gid, int mode)
{

  char string[1];
  KeySym keysymbol;
  int key;
  if(mode<0) return 0;

  readevent:

  XNextEvent(xwdid,&xwevtid);

  switch(xwevtid.type)
  {

    default:

    if(mode==GTwait) goto readevent;
    break;

    case KeyPress:  /* keyboard input */

    GTeventtype(gid)=GTkeyinput;
    string[0]=0;key=0;
    XLookupString((XKeyEvent*)&xwevtid,string,1,&keysymbol,0);
    key=string[0];

    switch (keysymbol & 0xffff)
    {
       case XK_Return:  key=1000+'R';break;
       case XK_BackSpace:
       case XK_Delete:  key=1000+'D';break;
       case XK_Tab:     key=1000+'T';break;

       case XK_Up:      key=1000+'u';break;
       case XK_Down:    key=1000+'d';break;
       case XK_Left:    key=1000+'l';break;
       case XK_Right:   key=1000+'r';break;

       case XK_Help:    key=1000+'h';break;
       case XK_Menu:
       case XK_Execute: key=1000+'x';break;
       case XK_Find:    key=1000+'f';break;
       case XK_KP_F1:
       case XK_Begin:   key=1000+'b';break;
       case XK_KP_F2:
       case XK_End:     key=1000+'e';break;
       case XK_Next:    key=1000+'n';break;
       case XK_Prior:   key=1000+'p';break;
       case XK_Select:  key=1000+'s';break;
       case XK_Insert:  key=1000+'i';break;

       default:
         if(keysymbol>=XK_F1 && keysymbol<=XK_F20 )
                key=2000+keysymbol-XK_F1+1;
    }

    if (key==0)
      if(mode==GTwait) goto readevent;
      else GTeventtype(gid)=GTnoevent;
    else
      GTkeycode(gid)=key;

    return 0;

    case ButtonPress:   /* mouse input */

        GTeventtype(gid)=GTxypointer;
        GTxpointer(gid)=xwevtid.xbutton.x;
        GTypointer(gid)=GTypixels(gid)-xwevtid.xbutton.y-1;
        return 0;

    case MotionNotify:   /* mouse input */

        GTeventtype(gid)=GTxymotion;
        GTxpointer(gid)=xwevtid.xmotion.x;
        GTypointer(gid)=GTypixels(gid)-xwevtid.xmotion.y-1;
        return 0;

    case Expose:

        GTeventtype(gid)=GTexpose;
        return 0;

    case MappingNotify:

        XRefreshKeyboardMapping((XMappingEvent*)&xwevtid);
        goto readevent;
   }

   GTeventtype(gid)=GTnoevent;

   return 0;
}

#define GTopen xwopen
#define GTcolor xwcolor
#define GTclose xwclose
#define GTclear xwclear
#define GTselect xwselect
#define GTtext xwtext
#define GTplot xwplot
#define GTevent xwevent

#endif
#include <math.h>

#define pd(x) printf("x = %d \n",x) /* for debugging */
#define pg(x) printf("x = %g \n",x) /* for debugging */

/* ************************************************************************

y2dplinticks(...)
-----------------
generates linear ticks and correnponding strings for plot frames.

* ************************************************************************/

/*  + syntax */

int y2dplinticks
(
        double  f1,
        double  f2,             /* scale min max (f1 must NOT f2) */
        int     maxticks,       /* max. number of ticks between f1,f2 */
        int     maxtextticks,   /* max number of labeled ticks  */
        int     maxchar,        /* max number of character at the ticks */
        int     expfactor,      /* exponentiation factor */
        int     *exponent,      /* exponent returned */
        int     *nticks,        /* actual number of ticks at scale */
        double  tickvalue[],    /* the actual tickvalue 1..nticks */
        int     *maxlen,        /* max. length used in tickstr */
        char    tickstr[][20]   /* field of character strings */
)

/*  - end syntax */

/* *****************

parameters:
exponent will return the order of magnitude at the scale.
expfactor is the factor the exponent is increased if nessesary.
3 = engeneering notation 10^0 10^3 10^6 10^9 ...
1 = scientific notation 10^0 10^1 10^2 10^3 10^4 10^5

return:
0 if ok;
>0 ( nticks=0,maxlen=0 ) if error;

precision:
is max 8 digits after decimal point (but returned are double) !
number of character returned may be greater then maxchar. this happens whenever
the precision of the maxchar character is not enough to make different ticks
at all. then you can get maxchar character (truncating the rest of the string)
or get <= maxlen character to have the full precision.

restrictions:
maxticks < 100, > 2
maxchar < 10, > 0
expfactor >= 1
expfactor <  maxchar

note:
the number of ticks returned is in the range maxticks/3 ... maxticks.
make sure that your data field tickstr,tickvalue has enough elements
to store the data.

you get good result for normal grafics with maxticks=50 maxtextticks=15.
for representation with fewer ticks you should have a minimum of
maxtexttick=8..10 and maxticks=2*maxtextticks to force a minimum of 1 subtick.

subticks are stored with a NULL text string. you can check that comparing
the first character with zero.

* *****************/

{
double  min,max,absmax,diff;
int     mainticks;
double  ticklog,ticklogmin,ticklogmax;
int     pospow,negpow;
int     iexp;
double  tickwidth,firsttick,lasttick;
int     tickoffset,ticklen;
char    text[30];
int     iticks;
double  tickmult;

*nticks=0;
*maxlen=0;
*exponent=0;

if ( maxticks < 1 )     return 1;
if ( maxchar < 1 )      return 1;

if ( expfactor < 1 )      expfactor = 1;

diff = fabs(f1-f2); if (diff <= 0) return 1;
min = ( (f1<f2)? f1:f2 );
max = ( (f1>f2)? f1:f2 );

/* calculate tick width and number of ticks at scale */

mainticks = maxtextticks;
if ( mainticks < 1 ) mainticks = 1;
if ( mainticks > maxticks ) mainticks = maxticks;

tickwidth = pow( 10.0, floor( log10( diff/mainticks ) + 1e-8 ) );
if      ( ( int )(diff/(tickwidth * 1.0)+1.000001) <= mainticks ) tickmult  =  1.0;
else if ( ( int )(diff/(tickwidth * 2.0)+1.000001) <= mainticks ) tickmult  =  2.0;
else if ( mainticks < 5 &&
          ( int )(diff/(tickwidth * 2.5)+1.000001) <= mainticks ) tickmult  =  2.5;
else if ( ( int )(diff/(tickwidth * 5.0)+1.000001) <= mainticks ) tickmult  =  5.0;
else                                                              tickmult  = 10.0;
tickwidth *= tickmult;
iticks =  diff / tickwidth  + 1.00000001;
if ( iticks > mainticks) iticks = mainticks;
if ( iticks == 0 ) return 1;


/* get the max min of scale, calc. the first tick (smallest value)
   last tick and the abs tick maximum of scale */

firsttick =  floor( min / tickwidth + 0.99999999) * tickwidth;
lasttick = firsttick + ( iticks-1 ) * tickwidth;
if ( lasttick > max + tickwidth * 0.00000001 )
{ iticks--; lasttick -= tickwidth; }
if ( iticks < 2 ) lasttick += tickwidth/10;
absmax = ( fabs(lasttick) > fabs(firsttick) )? fabs(lasttick) : fabs(firsttick);

/* get order of magnitude ticklog???, used to get the exponent */

ticklogmax = floor( log10( absmax    * 1.00000001 ) );
ticklogmin = floor( log10( tickwidth * 1.00000001 ) );
ticklog = 0;
if ( expfactor > 0 && ( ticklogmax > maxchar-2 || ticklogmax < 4-maxchar ) )
   ticklog = floor( ticklogmax / expfactor ) * expfactor;

/* calc the pos power ( number of digits before decimal point)
            neg power (                  after               ) */

pospow = floor( ticklogmax - ticklog + 0.5 ) + 1.5;
if ( pospow < 1 ) pospow = 1;
negpow = floor( ticklog - ticklogmin + 0.5 );
if ( tickmult == 2.5 ) negpow += 1;
if ( negpow < 0 ) negpow = 0;
iexp = floor( ticklog + 0.5 );

/* and get the offset and lenght of an %19.8f conversion */

tickoffset = 9 - pospow;
ticklen = pospow + 1;
if ( negpow > 0 ) ticklen = ticklen + negpow + 1;
if ( (firsttick>=0) && (lasttick>=0) ) { tickoffset++; ticklen--; }


{ /* make the string in tickstr and values in tickvalue */

int tick,subtick;
int section;
int maxsubticks;
double subwidth;
int subdiv;
int sublog;

maxsubticks = ( maxticks - iticks ) / ( iticks + 1 ) + 1;

if ( maxsubticks > 1 )
  {
  sublog = pow( 10.0, floor( log10( maxsubticks + 0.99999999 ) ) ) + 0.5;
  subdiv = maxsubticks / sublog;
  if ( tickmult == 1.0 || tickmult == 5.0 || tickmult == 10.0 )
  {      if ( subdiv >= 10 ) subdiv = 10;
    else if ( subdiv >=  5 ) subdiv =  5;
    else if ( subdiv >=  2 ) subdiv =  2; }
  if ( tickmult == 2.0 )
  {      if ( subdiv >= 10 ) subdiv = 10;
    else if ( subdiv >=  4 ) subdiv =  4;
    else if ( subdiv >=  2 ) subdiv =  2; }
  if ( tickmult == 2.5 )
  {      if ( subdiv >= 10 ) subdiv = 10;
    else if ( subdiv >=  5 ) subdiv =  5;
    else if ( subdiv >=  1 ) subdiv =  1; }

  maxsubticks = sublog * subdiv ;
  }

maxsubticks -= 1;
if ( maxsubticks < 0 ) maxsubticks = 0;
subwidth = tickwidth / ( maxsubticks + 1 );

for ( section = -1, tick = 0; section < iticks; section++ )
{
  double r = ( firsttick + section * tickwidth );
  if ( section > -1 && section < iticks )
  { int i1,i2;
    double textval;
    tickvalue[tick] = r;
    textval = r / pow( 10.0, ticklog );
    textval = floor( textval * 1e8 + 0.5 ) / 1e8;
    sprintf(text,"%19.8f",textval);
    for ( i1 = 0, i2 = tickoffset; i2 < ticklen + tickoffset; i2++ )
        if( text[i2]!=' ' )  { tickstr[tick][i1] = text[i2]; i1++; }
    tickstr[tick][i1]=0;
    tick++; }
  for ( subtick = 1; subtick <= maxsubticks; subtick++)
  { double subvalue = r + subtick * subwidth;
    if ( subvalue >= min && subvalue <= max )
    { tickvalue[tick] = subvalue; tickstr[tick][0] = 0; tick++; } }
}

*nticks = tick;
*exponent = iexp;
*maxlen = ticklen;

} /* end make strings and values */


return 0;

}

/* ************************************************************************

y2dplogticks(...)
-----------------
generates logaritmic ticks and correnponding strings for plot frames.

* ************************************************************************/

/*  + syntax */

int y2dplogticks
(
        double  f1,
        double  f2,             /* scale min max (f1 != f2 and > 0) */
        int     maxticks,       /* max. number of ticks between f1,f2 */
        int     maxtextticks,   /* max number of text labeled ticks  */
        int     maxchar,        /* max number of character at the ticks */
        int     expfactor,      /* exponentiation factor */
        int     *exponent,      /* exponent returned */
        int     *nticks,        /* actual number of ticks at scale */
        double  tickvalue[],    /* the actual tickvalue 1..iticks */
        int     *maxlen,        /* max. length used in tickstr */
        char    tickstr[][20]   /* field of character strings */
)
/*  - end syntax */

/* *****************

parameters:
expfactor is the factor the exponent is increased if nessesary.
3 = engeneering notation 10^0 10^3 10^6 10^9 ...
1 = scientific notation 10^0 10^1 10^2 10^3 10^4 10^5
if the number of decade ticks at the scale is less than expfactor 1 is used.
if the number of decade ticks at the scale is greater than maxtextticks.
expfactor is multplied with an integer so that maxtextticks is at the scale.
a possible results is then 10^6 10^12 10^18 ... .

if less than one decade is given with f1 and f2 a linear scale will be used
without decade ticks (see y2dplinticks). exponent will then return the order of
magnitude at the scale. in the case of decade ticks exponent is zero.


return:
0 if ok;
>0 ( nticks=0 ) if error;

precision:
is max 8 digits after decimal point (but returned are double) !
number of character returned may be greater then maxchar. this happens whenever
the precision of the maxchar character is not enough to make different ticks
at all. then you can get maxchar character (truncating the rest of the string)
or get <= maxlen character to have the full precision.

restrictions:
maxticks < 100, > 2
nchar < 10, > 0
expfactor >= 1
expfactor <  nchar

note:
the number of ticks returned is in the range maxticks/3 ... maxticks
make sure that your data field tickstr,tickvalue has enough elements
to store the data.

you get good result for normal grafics with maxticks=50 maxtextticks=15.
for representation with fewer ticks you should have a minimum of
maxtexttick=8..10 and maxticks=3*maxtextticks to force a minimum of 1 subtick.

the ticks are stored in tickstr left justified
and truncated by a zero byte at the end.
the decade ticks are stored in the form "EPddd\0" where ddd is the exponent.
the strings are stored left justified and ddd are the digits of the exponent
( inclusive minus sign ).
subticks are stored with a NULL text string. you can check that comparing
the first character with zero.

* *****************/

{
double  min,max;//,diff;
int     itotalticks,itextticks,isubticks;
int     mainticks;
double  decades;
double  maxlog,minlog;
int     powerlog,firstlog,lastlog;
int     subpowerlog,subfirstlog;//,sublastlog;
int     missticks,misstextticks,decticks,dectextticks;

*nticks=0;
*maxlen=0;
*exponent=0;

if ( maxticks < 1 )       return 1;
if ( maxchar < 1 )        return 1;
if ( f1 <= 0 || f2 <= 0 ) return 1;

if ( expfactor < 1 )      expfactor = 1;

/* calculate log coordinates and stuff used  */

min = ( (f1<f2)? f1:f2 );
max = ( (f1>f2)? f1:f2 );
//diff = fabs(f1-f2);
maxlog = log10( max ) + 1e-10;
minlog = log10( min ) - 1e-10;
decades = maxlog - minlog;

/* get number of powers per tick */

mainticks = maxtextticks;
if ( mainticks < 1 ) mainticks = 1;
if ( mainticks > maxticks ) mainticks = maxticks;

powerlog = floor( decades / mainticks + 1.00000001 );
if ( powerlog < 1 ) powerlog = 1;
if ( powerlog > 1 )
   powerlog = floor( (double)powerlog / expfactor + 0.99999999 ) * expfactor;

subpowerlog = floor( decades / maxticks + 1.00000001 );
if ( subpowerlog < 1 ) subpowerlog = 1;
if ( subpowerlog > 1 )
   subpowerlog = floor( (double)subpowerlog / expfactor + 0.99999999 )
               * expfactor;

if ( powerlog % subpowerlog != 0 )
  powerlog = ( ( powerlog / subpowerlog ) + 1 ) * subpowerlog;

/* calc the first and last decade tick logarithmic scale
   and number of dec ticks */

firstlog =  floor( minlog / powerlog ) * powerlog;
if ( firstlog  < minlog ) firstlog += powerlog;
itextticks = floor( maxlog - firstlog ) / powerlog  + 1.00000001;
if ( itextticks < 0 ) itextticks = 0;
lastlog = firstlog + ( itextticks - 1 ) * powerlog;

subfirstlog =  floor( minlog / subpowerlog ) * subpowerlog;
if ( subfirstlog  < minlog ) subfirstlog += subpowerlog;
itotalticks = floor( maxlog - subfirstlog ) / subpowerlog  + 1.00000001;
if ( itotalticks < 1 ) itotalticks = 1;
//sublastlog = subfirstlog + ( itextticks - 1 ) * subpowerlog;

isubticks = powerlog/subpowerlog - 1;

/* if not two decade ticks used make lin scala with maxticks */

if ( itextticks < 1 || decades < 1.001 )
{ y2dplinticks( f1,f2,maxticks,maxtextticks,maxchar,expfactor,
     exponent,nticks,tickvalue,maxlen,tickstr );
  return 0; }

misstextticks = mainticks - itextticks;
dectextticks = misstextticks/decades-1; /* floor(decades+0.9999999); */
missticks = maxticks - itotalticks;
if ( missticks < 0 ) missticks = 0;
decticks = missticks/decades+1;         /* floor(decades+0.9999999); */

/* pd(maxticks);pd(missticks);
   pg(decades); pd(decticks); */

/* linticks get back 2 or 1 tick with out .25 ticks */
if ( decticks == 4 ) decticks = 2;
if ( decticks == 3 ) decticks = 2;
if ( decticks == 2 ) decticks = 2;
if ( decticks == 1 ) decticks = 2;
if ( decticks <  1 ) decticks = 0;
if ( dectextticks == 4 ) dectextticks = 2;
if ( dectextticks == 3 ) dectextticks = 2;
if ( dectextticks == 2 ) dectextticks = 2;
if ( dectextticks == 1 ) dectextticks = 2;
if ( dectextticks <  1 ) dectextticks = 0;
if ( dectextticks > decticks ) decticks = dectextticks;
/* trick out linticks to force 2 4 6 8 instead of 2.5 subticks */
if ( dectextticks == 2 && decticks < 9 && decticks > 2 )
{ if ( itextticks > 2 ) dectextticks = 1; else decticks = dectextticks; }
if ( isubticks > 0 ) decticks = 0;

/* pd(decticks);pd(dectextticks); */

y2dplinticks( 1.0001,9.9999,decticks,dectextticks,maxchar,1,
                exponent,nticks,tickvalue,maxlen,tickstr );

if ( *nticks > 0 )      /* reorder the decade ticks */
{
  double        tempvalue;
  char          tempstr[20];
  int           tempticks = *nticks;
  double        decmin = pow( 10.0, floor(minlog+1e-9));
  int           i,i1,i2;
  double        r;

  /* find reordering offset  and rotate ticks if first tick is > min */

  for ( i1=0; i1 < tempticks; i1++)
  { r = tickvalue[0]*decmin; if ( r >= min ) break;
    tempvalue = tickvalue[0];
    for ( i=0; i < 20; i++) tempstr[i]=tickstr[0][i];
    for ( i2=1; i2 < tempticks; i2++)
    { tickvalue[i2-1]=tickvalue[i2];
      for ( i=0; i < 20; i++) tickstr[i2-1][i]=tickstr[i2][i]; }
    tickvalue[tempticks-1]=tempvalue;
    for ( i=0; i < 20; i++) tickstr[tempticks-1][i]=tempstr[i]; }

  /* multiply the decade ticks to correct value */

  for ( i2 = 0;  i2 < tempticks-i1; i2++)        tickvalue[i2] *= decmin;
  for ( i2 = tempticks-i1; i2 < tempticks; i2++) tickvalue[i2] *= 10*decmin;

  /* and extend to missticks or max the tickvalues */

  for ( i2 = 0; tempticks < missticks; i2++,tempticks++)
  { r = tickvalue[i2] * 10.0; if ( r > max * 1.00000001 ) break;
    tickvalue[tempticks] = r;
    for ( i=0; i < 20; i++ ) tickstr[tempticks][i] = tickstr[i2][i]; }

  *nticks = tempticks;

} /* end reorder ticks */

/* get number of decimal digits used */
{
  int i,ilog;

  int decdigfirst = log10(fabs(10.0*firstlog)+1)+1e-9;
  int decdiglast  = log10(fabs(10.0*lastlog)+1)+1e-9;
  int decdigits = ( decdigfirst > decdiglast ) ? decdigfirst : decdiglast;

  if ( firstlog < 0 || lastlog < 0 ) decdigits += 1;
  if ( decdigits + 2 > *maxlen ) *maxlen = decdigits + 2;
  if ( decdigits <= 0 ) decdigits = 1, *maxlen +=1;

  for ( ilog = firstlog - powerlog, i = - 1;
        ( i < itextticks ) && ( *nticks < maxticks );
        ilog += powerlog, i++)
  {
    double r = pow( 10.0, (double)ilog );
    int subtick;

    if ( ilog >= firstlog )
    { int i1;
      tickvalue[*nticks] = r;
      for ( i1 = 0; i1 < 20; i1++ ) tickstr[*nticks][i1] = 0;
      sprintf( tickstr[*nticks], "EP%d", ilog );
      for ( i1=0; i1 < decdigits + 2 ; i1++ )
      if ( tickstr[*nticks][i1] == 0 ) tickstr[*nticks][i1] = ' ' ;
      (*nticks)++; }

   for ( subtick = 1; subtick <= isubticks; subtick++)
   { double submult = pow( 10.0, (double)( subpowerlog * subtick ) );
     double subvalue = r * submult;
     if ( subvalue >= min && subvalue <= max )
     { tickvalue[*nticks] = subvalue;
       tickstr[*nticks][0] = 0;
       (*nticks)++; }
   }

  }

} /* end of get digits */

return 0;


}

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/****************** y2dplot function *******************************/
/****************** common variables *******************************/

/* debug macros */
#define dbgi(i) printf("i=%d\n",i)
#define dbgf(d) printf("d=%f\n",d)

/* coordinates entered by user */
static  int  ixmini,ixmaxi,iymini,iymaxi,icomfi;
static  double  rxmini,rxmaxi,rymini,rymaxi;
static  double   rzmini,rzmaxi,xscalei,yscalei;

/* coordinates of plot */
static  int  ixmin,ixmax,iymin,iymax,icomf,ililog,ihpvf;
static  double   rzmin,rzmax,xscale,yscale;
static  double   ixlow,ixhigh;
static  double   rzlow,rzhigh;
static  int  ixinc,iyinc;

/* coordinates buffered */
static  int     buffered=0;
static  int  ibxmin,ibxmax,ibymin,ibymax,ibcomf,iblilog,ibhpvf;
static  double   rbzmin,rbzmax,xbscale,ybscale;

/* marker positions  and increments for marker move commands */
static  int  ixml,ixmr;
static  double  rzmu;
static  int  imxincf,imxincs;
static  double  rmzinc;

/* addr pixels of plot device */
static  double  scxmin,scymin,scxmax,scymax,cxpixel,cypixel;
static  char  plotname[100];

/* renormalisation factors of last plot group */
static  double   xmult,ymult,xadd,yadd;
static  double   xscmin,xscmax,yscmin,yscmax;

/* screen size and pos */
static int      sxsize=700,sysize=500,sxpos=1,sypos=1;

/* parameters passed to y2dp changed to common var. for all subroutines */
static  int          pmode;
static  const char   *comment;
static  int       inx,iny;
static  double       lx,hx,ly,hy;
static  void       *rdata;

static  int dtype=0;

static double rdat(int x, int y)
{
   if(!rdata) return 0;
   switch(dtype)
   {
     case 'i': return ((int *)(rdata))[(((y)-1)*(inx))+((x)-1)]; break;
     case 'I': return ((unsigned int *)(rdata))[(((y)-1)*(inx))+((x)-1)]; break;
     case 's': return ((short *)(rdata))[(((y)-1)*(inx))+((x)-1)]; break;
     case 'S': return ((unsigned short *)(rdata))[(((y)-1)*(inx))+((x)-1)]; break;
     case 'd': return ((double *)(rdata))[(((y)-1)*(inx))+((x)-1)]; break;
     case 'f': return ((float *)(rdata))[(((y)-1)*(inx))+((x)-1)]; break;

     case 'i'+'p': if(((int**)(rdata))[y-1]==0) return 0;
                   else return ((int **)(rdata))[y-1][x-1];
       break;
     case 'I'+'p': if(((unsigned int**)(rdata))[y-1]==0) return 0;
                   else return ((unsigned int **)(rdata))[y-1][x-1];
       break;
     case 's'+'p': if(((short**)(rdata))[y-1]==0) return 0;
                   else return ((short **)(rdata))[y-1][x-1];
       break;
     case 'S'+'p': if(((unsigned short**)(rdata))[y-1]==0) return 0;
                   else return ((unsigned short **)(rdata))[y-1][x-1];
       break;
     case 'd'+'p': if(((double**)(rdata))[y-1]==0) return 0;
                   else return ((double **)(rdata))[y-1][x-1];
       break;
     case 'f'+'p': if(((float**)(rdata))[y-1]==0) return 0;
                   else return ((float **)(rdata))[y-1][x-1];
       break;
     default: return ((float *)(rdata))[(((y)-1)*(inx))+((x)-1)];
       break;
   }
}


static int checkcount=(-50);
static int graficfile;


/******** Plot application subroutines ****************/

static int y2dpgetkey(int mode);

static struct GTfile_struct gidstruct;
static GTfile gid=0;
static int iopen=0;

static  int  agmode;
static  int  mgbuffer,ngbuffer,xgbuffer[100],ygbuffer[100];

static int y2dpopen()
{
if(gid==0) gid=&gidstruct;
GTopen(gid,comment,sxpos,sypos,sxsize,sysize);
if(GTerror(gid))
{
    printf("\n%s\n",GTerror(gid));
    iopen=0;
    return 1;
}

GTevent(gid,-1);
iopen=1;
GTcolor(gid,GTbackground,0,0,0,0);
GTselect(gid,GTnoinput);
scxmax=GTxpixels(gid);
scymax=GTypixels(gid);
cxpixel=GTfontxsize(gid);
cypixel=GTfontysize(gid);
// printf("screen sizes %g %g %g %g\n",scxmax,scymax,cxpixel,cypixel);
scxmin=8*cxpixel; scxmax-=8*cxpixel;
scymin=3*cypixel; scymax-=2*cypixel;

return 0;
}

static int y2dpclose() /////////////////////////////////////////
{
  if(iopen==0) return 1;
  GTclose(gid);
  iopen=0;
  return 0;
}

static int y2dpclear()
{
  if(iopen==0) return 1;
  return GTclear(gid);
}
static int y2dpsetdots()
{
  if(iopen==0) return 1;
  return GTcolor(gid,GTreplace,1,0,GTmaxgreen(gid),GTmaxblue(gid));
}
static int y2dptoggledots()
{
  if(iopen==0) return 1;
  return GTcolor(gid,GTcomplement,2,GTmaxred(gid),0,GTmaxblue(gid));
}

static int y2dpalpha(int mode)
{
  // GTdebug(gid);
  if(iopen==0) return 1;
  GTselect(gid,GTalpha);
  y2dpgetkey(0);
  if(mode!=agmode||mode==0)
  {
    printf("%80s\n%80s\n","","");
    GTselect(gid,GTalpha);
    agmode=mode;
  }
  return 0;
}

static int y2dpgraphic()
{
  if(iopen==0) return 1;
  return GTselect(gid,GTgraphic);
}

static int y2dptextout(double x, double y, int n, const char *text)
{
  if(iopen==0) return 1;
  return GTtext(gid,( int )(x),( int )(y),n,text);
}

static int y2dpline(double x1, double y1, double x2, double y2)
{
  if(iopen==0) return 1;
  if(mgbuffer!=1)
  {
  int xgt[2],ygt[2];
  xgt[0]=x1; xgt[1]=x2;
  ygt[0]=y1; ygt[1]=y2;
  GTplot(gid,1,2,xgt,ygt);
  }
  else
  {
  if(ngbuffer>0) ngbuffer--;
  xgbuffer[ngbuffer]=x1; ygbuffer[ngbuffer]=y1; ngbuffer++;
  xgbuffer[ngbuffer]=x2; ygbuffer[ngbuffer]=y2; ngbuffer++;
  if (ngbuffer > 40)
  {
    GTplot(gid,ngbuffer/2,2,xgbuffer,ygbuffer);
    ngbuffer=0;
  }
  if (ngbuffer!=1)
  {
    xgbuffer[ngbuffer]=x2; ygbuffer[ngbuffer]=y2; ngbuffer++;
  }
  }
  return 0;
}

static int y2dpvectorplot() { mgbuffer=1; ngbuffer=0; return 0; }
static int y2dppointplot()  { mgbuffer=2; ngbuffer=0; return 0; }

static int y2dpendplot()
{
if(iopen==0) return 1;
if (ngbuffer > 0)
{
  switch(mgbuffer)
  {
    case 1: GTplot(gid,ngbuffer/2,2,xgbuffer,ygbuffer); break;
    case 2: GTplot(gid,ngbuffer,1,xgbuffer,ygbuffer); break;
  }
  ngbuffer=0;
}
mgbuffer=0;
return 0;
}

static int y2dpxy(double x,double y)
{
if(iopen==0) return 1;
xgbuffer[ngbuffer]=x; ygbuffer[ngbuffer]=y; ngbuffer++;
if (ngbuffer > 40)
{
  switch(mgbuffer)
  { case 1: GTplot(gid,ngbuffer/2,2,xgbuffer,ygbuffer); break;
    case 2: GTplot(gid,ngbuffer,1,xgbuffer,ygbuffer); break; }
  ngbuffer=0;
}
if (mgbuffer==1 && ngbuffer!=1)
{ xgbuffer[ngbuffer]=x; ygbuffer[ngbuffer]=y; ngbuffer++; }
return 0;
}



/**************************** inquire routines *****************************/

static int y2dpgetkey(int mode)
/*
   0   = enabled stdio
   n<0 = check each -n'th call for one char and store it
   1   = wait for one char
   2   = return last checked character if any
   3   = zero buffer
*/
{
static int waitcount=0;
static int lastmode=0;
static char chr;

/* { printf("y2dpgetkey: mode %d\n",mode); } */

if(mode<0)
{ if(lastmode!=mode) { lastmode=mode; chr=0; waitcount=0; }
  if((waitcount%(-mode))==0) { chr=getkeypress(3); }
  waitcount++; return chr;
}
else if(mode==1) { lastmode=mode;  y2dpgraphic();
                   chr=getkeypress(2); return chr; }
else if(mode==2) { return chr; }
else if(mode==3) { lastmode=mode;
                   while(getkeypress(3)!=0)
                   ;
                   return chr=0;}
else             { lastmode=0; getkeypress(0); return chr=0; }
}


static FILE *ttyio;
static int y2dppromptstring(const char *prompt, char *line)
{
if(ttyio==0) ttyio=fopen("/dev/tty","w+");
if(!ttyio) return line[0]=0;
y2dpalpha(0);
fputs(prompt,ttyio);
fflush(ttyio);
fgets(line,100,ttyio);
return 1;
}

static int y2dpgetint(const char *prompt, int *val)
{
char line[100],p[100];
sprintf(p,"%s [ %d ] ? ",prompt,*val);
y2dppromptstring(p,line);
sscanf(line,"%d",val); return 0;
}

static int y2dpget2double(const char *prompt, double *val1, double *val2)
{
char line[100],item[30],p[100];
sprintf(p,"%s [ %g %g ] ? ",prompt,*val1,*val2);
y2dppromptstring(p,line);
sscanf(line,"%s ",item);
switch(item[0])
{ case 's': ; case 'S': break;
  case 'l': ; case 'L': *val1 = -1.1e30; break;
  case 'h': ; case 'H': *val1 =  1.1e30; break;
  default: sscanf(line,"%lf",val1); break; }
sscanf(line,"%*s %s ",item);
switch(item[0])
{ case 's': ; case 'S': break;
  case 'l': ; case 'L': *val2 = -1e30; break;
  case 'h': ; case 'H': *val2 =  1e30; break;
  default: sscanf(line,"%*s %lf",val2); break; }
return 0;
}

static int y2dpgetstring(const char *prompt, int n, char *val)
{
// int i;
char line[100],p[100];
sprintf(p,"%s [%s] ? ",prompt,val);
y2dppromptstring(p,line);
sscanf(line,"%s",val);
return 0;
}

/************************** help routine ********************************/

static int y2dphelp()
{
// y2dpclear();
y2dpalpha(0);
printf("----------------- coordinates commands ----------------------------\n");
printf("x=enter xmin,xmax, y=enter ymin,ymax, z=enter zmin,zmax\n");
printf("c=enter x-compression, 2=find zmin,zmax\n");
printf("J=show next group, k=show prev.group\n");
printf("1=toggle x-compression (1 or auto) \n");
printf("g=enter isovector, i=toggle isovectors\n");
printf("\n");
printf("----------------- plot control commands ---------------------------\n");
printf("h=histogramm plot, f=fill plot, v=continous vector, p=point plot\n");
printf("n=linear plot, l=logarithmic, s=square root\n");
printf("\n");
printf("----------------- calibration commands ----------------------------\n");
printf("#=set x,y calibration of x and y axis\n");
printf(";=set x calibration with marker\n");
printf("$=reset calibration to default\n");
printf("\n");
printf("----------------- control commands --------------------------------\n");
printf("e,q=exit,quit, u,d=update or do command\n");
printf("b=buffer coordinates, r=recall coordinates\n");
printf("t=totalize(window)\n");
printf("o=overlay group \n");
printf("= = create plot file\n");
printf("~ = setup graphics window \n");
printf("e,q=clear,save(e only),return  u,d=noclear,save(d only),return,\n");
printf("b=buffer coordinates   r=recall coordinates  t=totalize(window)\n");
printf("\n");
printf("----------------- marker commands (use at keypad) -----------------\n");
printf("W = window to the middle          M = marker to the middle\n");
printf("8 = x-marker to the left (slow)   9 = x-marker to the right (slow)\n");
printf("7 = x-marker to the left (fast)   - = x-marker to the right (fast)\n");
printf("5 = x-window smaller (slow)       6 = x-window larger (slow)\n");
printf("4 = x-window smaller (fast)      +, = x-window larger (fast)\n");
printf("3 = z-marker up                   . = z-marker down\n");
printf("0 = plot with z-marker=zmax       / = plot with zmarker=zmin\n");
printf("<enter> or <return> = plot with x = x-markers\n");
printf("\n");
// printf("*** hit blank for last plot or any command listed above ***\n");
return 0;
}


/************ coordinate functions **********************/

static double y2dpxnx(double nx)
{
  if(inx<2) return 0.;
  return lx + ((hx-lx)/(inx-1))*(nx-1);
}

static double y2dpyny(double ny)
{
  if(iny<2) return 0.;
  return ly + ((hy-ly)/(iny-1))*(ny-1);
}

static int y2dpnx(double x)
{
  double tmp;
  if(inx<2) return 0;
  if(lx<hx) { if(x<lx) x=lx; else if(x>hx) x=hx; }
  else      { if(x>lx) x=lx; else if(x<hx) x=hx; }
  tmp=(x-lx)/(hx-lx)*(inx-1) + 1.5;
  return tmp;
}

static int y2dpny(double y)
{
  double tmp;
  if(iny<2) return 0;
  if(ly<hy) { if(y<ly) y=ly; else if(y>hy) y=hy; }
  else      { if(y>ly) y=ly; else if(y<hy) y=hy; }
  tmp=(y-ly)/(hy-ly)*(iny-1) + 1.5;
  return tmp;
}




/*********************** save and restore routine **************************/

static FILE *y2dpmkfname(const char *mode, const char *f,int i1,int i2)
{
char fname[1001];
int l;
const char *sdir=getenv("HOME");
if (sdir==0) sdir=".";
sprintf(fname,"%s/.y2dplot",sdir);
if(mkdir(fname,0755)<0)
; // perror(fname);
sprintf(fname,"%s/.y2dplot/%d-%d-",sdir,i1,i2);
l=strlen(fname);
for (;l<1000 && *f;f++,l++)
{
  if(*f >= 'A' && *f <= 'Z') fname[l]=*f;
  else if(*f >= 'a'&& *f <= 'z') fname[l]=*f;
  else if(*f >= '0'&& *f <= '9') fname[l]=*f;
  else l--;
}
fname[l]=0;
// printf("y2dpmkfname: %s -> %s \n",f,fname);
FILE *fp=fopen(fname,mode);
if(fp==0 && mode[0]=='w') fprintf(stderr,"y2dplot/ERROR: can not write %s\n",fname);
return fp;
}

static int y2dpsave(int mode) /////////////////////////////////////////
{
  FILE *out=y2dpmkfname("w","terminal",0,0);
  if(!out) return 0;
  fprintf(out,"%d %d %d %d\n",sxpos,sypos,sxsize,sysize);
  fprintf(out,"%d %d %d %d %d %d %d %d\n",buffered,ibxmin,ibxmax,ibymin,ibymax,ibcomf,iblilog,ibhpvf);
  fprintf(out,"%g %g %g %g\n",rbzmin,rbzmax,xbscale,ybscale);
  fclose(out);
  out=y2dpmkfname("w",comment,iny,inx);
  if(!out) return 0;
  fprintf(out,"%g %g %g %g %d\n",rxmini,rxmaxi,rymini,rymaxi,icomfi);
  fprintf(out,"%g %g %d %d \n",rzmini,rzmaxi,ihpvf,ililog);
  fprintf(out,"%d %d %g\n",ixml,ixmr,rzmu);
  fprintf(out,"%g %g \n",xscalei,yscalei);
  fprintf(out,"%g %g %g %g\n",lx,hx,ly,hy);
  fclose(out);
  return 1;
}

static int y2dprestore(int nx, int ny) //////////////////////////////////
{
  FILE *in=y2dpmkfname("r","terminal",0,0);
  if(!in) return 0;
  fscanf(in,"%d %d %d %d\n",&sxpos,&sypos,&sxsize,&sysize);
  fscanf(in,"%d %d %d %d %d %d %d %d\n",&buffered,&ibxmin,&ibxmax,&ibymin,&ibymax,&ibcomf,&iblilog,&ibhpvf);
  fscanf(in,"%lf %lf %lf %lf\n",&rbzmin,&rbzmax,&xbscale,&ybscale);
  fclose(in);
  in=y2dpmkfname("r",comment,iny,inx);
  if(!in) return 0;
  fscanf(in,"%lf %lf %lf %lf %d\n",&rxmini,&rxmaxi,&rymini,&rymaxi,&icomfi);
  fscanf(in,"%lf %lf %d %d \n",&rzmini,&rzmaxi,&ihpvf,&ililog);
  fscanf(in,"%d %d %lf\n",&ixml,&ixmr,&rzmu);
  fscanf(in,"%lf %lf \n",&xscalei,&yscalei);
  fscanf(in,"%lf %lf %lf %lf\n",&lx,&hx,&ly,&hy);
  fclose(in);
  // printf(" screen %d %d %d %d\n",sxpos,sypos,sxsize,sysize);
  // printf(" coord %g %g %g %g\n",rxmini,rxmaxi,rymini,rymaxi);
  return 1;
}


/******************* marker routine **********************************/

static int y2dpmarker(int mode)
{
static  double  rxlmold,rxrmold,rzmold;

if(mode=='c')   { rxlmold=rxrmold=rzmold= -1.; return 0; }

y2dpgraphic();
y2dptoggledots();

if(mode=='x'||mode=='b')
{
if(ixml < 1) ixml=1;
if(ixmr < 1) ixmr=1;
if(ixml > inx) ixml=inx;
if(ixmr > inx) ixmr=inx;
if(ixmin <= ixmax && ixmr < ixml) ixml=(ixml+ixmr)/2.0,ixmr=ixml;
if(ixmin >  ixmax && ixmr > ixml) ixml=(ixml+ixmr)/2.0,ixmr=ixml;
/* delete old markers */
if(rxlmold >= 0.) { y2dpline(rxlmold,yscmin,rxlmold,yscmax); rxlmold= -1.;}
if(rxrmold >= 0.) { y2dpline(rxrmold,yscmin,rxrmold,yscmax); rxrmold= -1.;}
/* plot new markers */
if((ixml >= ixlow) && (ixml <= ixhigh))
{ rxlmold=xmult*ixml+xadd; y2dpline(rxlmold,yscmin,rxlmold,yscmax); }
if((ixmr >= ixlow) && (ixmr <= ixhigh))
{ rxrmold=xmult*ixmr+xadd;
  if(rxrmold > rxlmold+1.5) y2dpline(rxrmold,yscmin,rxrmold,yscmax);
  else                      rxrmold = -1.; }
}

if(mode=='y'||mode=='b')
{
if(rzmu > 1.0e30) rzmu= 1.0e30;
if(rzmu <= -1.0e30) rzmu = -1.0e30;
/* delete old marker if set */
if(rzmold >= 0) { y2dpline(xscmin,rzmold,xscmax,rzmold); rzmold = -1; }
/* plot new z marker */
if((rzmu < rzhigh) && (rzmu > rzlow))
{ switch(ililog)
  { case 0: rzmold=rzmu; break;
    case 1: rzmold=log10(rzmu); break;
    case 2: rzmold=sqrt(rzmu); break; }
  rzmold=rzmold*ymult+yadd;
  y2dpline(xscmin,rzmold,xscmax,rzmold); }
}
y2dpalpha('m');
if ((ixml == ixmr) && (mode=='x'))
    printf("M: X: %12.5g (%5d)   Value = %g                     \n",
        y2dpxnx((double)ixml),ixml,rdat(ixml,iymax));
else  printf("W: X: %12.5g (%5d) %12.5g (%5d) Z: %g      \n",
        y2dpxnx((double)ixml),ixml,y2dpxnx((double)ixmr),ixmr,rzmu);

return 0;
}





/*************** Tick Plot subroutine ****************************/

static int y2dppticks ////////////////////////////////////////////
(
int maxticks,
double f1, double f2,
double x1, double y1, double x2, double y2,
int dir,
int funct
)
{
// printf("%d  %g %g %g %g %g %g %d %d \n",maxticks,f1,f2,x1,y1,x2,y2,dir,funct);
int exponent;
int nticks;
char tickstr[40][20];
int  tickmaxlen;
double tickvalue[40];
int mainticks=10;
int maxchar=6;
int expfactor=3;

if (dir=='l' || dir=='r')
{ double height=fabs(y1-y2);
  if (mainticks>height/GTfontysize(gid)/1.5)
      mainticks=height/GTfontysize(gid)/1.5;
  if (maxticks>3*mainticks) maxticks=3*mainticks; }

if (dir=='u' || dir=='d')
{ double width=fabs(x1-x2);
  if (mainticks>width/GTfontxsize(gid)/maxchar*1.5)
  mainticks=width/GTfontxsize(gid)/maxchar*1.5;
  if (maxticks>3*mainticks) maxticks=3*mainticks; }

if(maxticks<2) return 0;

switch ( funct )
{
case 1: y2dplogticks(f1,f2,maxticks,mainticks,maxchar,expfactor,
          &exponent,&nticks,tickvalue,&tickmaxlen,tickstr);
        break;
default: y2dplinticks(f1,f2,maxticks,mainticks,maxchar,expfactor,
           &exponent,&nticks,tickvalue,&tickmaxlen,tickstr);
         break;
}

/* tick plot section */
{
int x[2],y[2];
int i;
int xpos,ypos;
int xtpos,ytpos;
int lastmaintick=0;

double xmult,ymult,xoffset,yoffset;

if ( f1 == f2 ) return 0;

switch ( funct )
{
case 1:
  xmult = ( x2 - x1 ) / ( log10( f2 ) - log10( f1 ) );
  ymult = ( y2 - y1 ) / ( log10( f2 ) - log10( f1 ) );
  xoffset = x1 - log10(f1) * xmult ;
  yoffset = y1 - log10(f1) * ymult ;
  break;
case 2:
  xmult = ( x2 - x1 ) / ( sqrt( f2 ) - sqrt( f1 ) );
  ymult = ( y2 - y1 ) / ( sqrt( f2 ) - sqrt( f1 ) );
  xoffset = x1 - sqrt(f1) * xmult ;
  yoffset = y1 - sqrt(f1) * ymult ;
  break;
default:
  xmult = ( x2 - x1 ) / ( ( f2 ) - ( f1 ) );
  ymult = ( y2 - y1 ) / ( ( f2 ) - ( f1 ) );
  xoffset = x1 - (f1) * xmult ;
  yoffset = y1 - (f1) * ymult ;
  break;
}

if (exponent != 0)
{ for (i = 0; i < nticks; i++ ) if (tickstr[i][0]!=0) lastmaintick=i;
  sprintf(tickstr[lastmaintick],"xEP%d",exponent); }

for (i = 0; i < nticks; i++ )
{
double r;
char str[20];
int slen;
int i1,i2;
double xd=0,yd=0,xt=0,yt=0,xw=0,yw=0;
int  ticklen;

switch ( funct )
{ case 1:  r = log10(tickvalue[i]); break;
  case 2:  r =  sqrt(tickvalue[i]); break;
  default: r =      (tickvalue[i]); break; }


  for(i1=0,slen=0; i1<maxchar && (str[slen]=tickstr[i][i1])!=0;i1++)
  { if ( str[slen] != ' ') slen++; }

  xpos=xoffset+xmult*r;
  ypos=yoffset+ymult*r;
  ticklen=GTfontxsize(gid);

  switch ( dir )
  {  case 'r' : xw = 0; yw=1;
                xd = 1; yd =  0;
                xt = 0; yt = -0.5;
                break;
     case 'l' : xw = 0; yw=1;
                xd = -1; yd =  0;
                xt = - slen; yt = -0.5;
                break;
     case 'd' : xw = 1; yw=0;
                xd = 0; yd = -1; xt = - slen/2.0; yt = -1.00;
          if ( str[0]=='-') xt -= 0.5;
          if ( str[0]=='E') xt  = -1;
                break;
     case 'u' : xw = 1; yw=0;
                xd =  0; yd =  1; xt = - slen/2.0; yt =  -0.25;
          if ( str[0]=='-') xt -= 0.5;
          if ( str[0]=='E') xt  = -1;
                break;             }

  if(str[0]==0) xw=yw=0;
  for ( i1 = -xw; i1 <= xw; i1++)
   for ( i2 = -yw; i2 <= yw; i2++)
    { x[0]=xpos+i1; x[1]=xpos+xd*ticklen+i1;
      y[0]=(ypos+i2); y[1]=(ypos+yd*ticklen+i2);
      if ( str[0]==0 )
      { x[1]=xpos+xd*ticklen*0.5; y[1]=(ypos+yd*ticklen*0.5); }
      GTplot(gid,1,2,x,y); }

  xtpos=xpos+xt*GTfontxsize(gid)+xd*ticklen*1.5;
  ytpos=ypos+yt*GTfontysize(gid)+yd*ticklen*1.5;
  if ( str[0] == 'x' )
  { y2dptextout(xtpos,ytpos,3,"x10");
    xtpos += (3*GTfontxsize(gid)); ytpos += (GTfontysize(gid)/3);
    y2dptextout(xtpos,ytpos,slen-3,&str[3]); }
  else if ( str[0] == 'E' )
  { y2dptextout(xtpos,ytpos,2,"10");
    xtpos += (2*GTfontxsize(gid)); ytpos += (GTfontysize(gid)/3);
    y2dptextout(xtpos,ytpos,slen-2,&str[2]); }
  else
  { y2dptextout(xtpos,ytpos,slen,str);}

} /* end for nticks */

} /* end tick plot section */

return 0;
} /* end of tick program */




/**************** plot routine *************************************/


static int y2dpdraw()
{

// double max,min,mingt0;

/* check y-min-max */
if ( iymini <=  0 ) iymini=1;
if ( iymini > iny ) iymini=iny;
if ( iymaxi <=  0 ) iymaxi=1;
if ( iymaxi > iny ) iymaxi=iny;
/* if ( iymaxi == iymini ) iymaxi=iny,iymini=1; */
iymin=iymini; iymax=iymaxi;
if(iymax<iymin) iyinc = -1; else iyinc=1;
rymini=y2dpyny((double)iymin); rymaxi=y2dpyny((double)iymax);

/* check x-min-max */
if ( ixmini <=  0 ) ixmini=1;
if ( ixmini > inx ) ixmini=inx;
if ( ixmaxi <=  0 ) ixmaxi=1;
if ( ixmaxi > inx ) ixmaxi=inx;
/* if ( ixmaxi == ixmini ) ixmaxi=inx,ixmini=1; */
ixmin=ixmini; ixmax=ixmaxi;
if(ixmax<ixmin) ixinc = -1; else ixinc=1;
if(ixmax>ixmin) ixlow=ixmin,ixhigh=ixmax;
else    ixlow=ixmax,ixhigh=ixmin;
rxmini=y2dpxnx((double)ixmin); rxmaxi=y2dpxnx((double)ixmax);

/* calculate compressionfactor */
if((icomfi > inx)|| (icomfi < 0)) icomfi=0;
icomf=icomfi;
if(icomfi == 0)
{

  if(ihpvf == 1) icomf=1;
  else
  {
    int i=1;
    if(ihpvf >= 3) i=2;
    icomf=1+((abs(ixmax-ixmin)*i)/1024);
  }
}

/* calc z-min-max */
/* find z-min-max  */
if(rzmaxi==rzmini) { rzmaxi = 1e30; rzmini = -1e30;}

if(rzmaxi >= 1e30)
{
  int ix,iy;
  double r1 = -1.0e30, r2=0.0;
  for (iy=iymin;iy!=iymax+iyinc;iy+=iyinc)
  {
    for (ix=ixmin; ix!=ixmax+ixinc; ix+=ixinc)
    { r2=rdat(ix,iy); if(r2 > r1) r1=r2; }
  }
  rzmaxi=r1;
}

if(rzmaxi <= -1e30)
{
  int ix,iy;
  double r1 = 1.0e30, r2=0.0;
  for (iy=iymin;iy!=iymax+iyinc;iy+=iyinc)
  {
     for (ix=ixmin; ix!=ixmax+ixinc; ix+=ixinc)
     { r2=rdat(ix,iy); if(r2 < r1) r1=r2; }
  }
  rzmaxi=r1;
}

if(rzmini >= 1e30)
{
  int ix,iy;
  double r1 = -1.0e30, r2=0.0;
  for (iy=iymin;iy!=iymax+iyinc;iy+=iyinc)
  {
     for (ix=ixmin; ix!=ixmax+ixinc; ix+=ixinc)
     { r2=rdat(ix,iy); if(r2 > r1) r1=r2; }
  }
  rzmini=r1;
}

if(rzmini <= -1e30)
{
  int ix,iy;
  double r1 = 1.0e30, r2=0.0;
  for (iy=iymin;iy!=iymax+iyinc;iy+=iyinc)
  {
    for (ix=ixmin; ix!=ixmax+ixinc; ix+=ixinc)
     { r2=rdat(ix,iy); if(r2 < r1) r1=r2; }
  }
  rzmini=r1;
}


/* check z-min-max */
if((rzmini < -1e30) || (rzmini >  1e30)) rzmini = -1e30;
if((rzmaxi < -1e30) || (rzmaxi >  1e30)) rzmaxi =  1e30;

rzmin=rzmini;
rzmax=rzmaxi;

if(ililog==1 && (rzmax<=0 || rzmin<=0) ) // find value > 0
{
  int ix,iy;
  double r1=3e30,r2=1;
  for (iy=iymin;iy!=iymax+iyinc;iy+=iyinc)
   for (ix=ixmin; ix!=ixmax+ixinc; ix+=ixinc)
   {
     r2=rdat(ix,iy);
     if(r2 > 0 && r2 < r1) r1=r2;
   }
  r1/=2;
  if(r1>1e30) r1=0.1;
  r1/=icomf;

  if(rzmin<1e-30) rzmin=r1;
  if(rzmax<1e-30) rzmax=2*r1;
  // printf("min > 0 %g %g %g\n",r1,rzmin,rzmax);
}

switch (ililog) /* korr. minmum , korretur falls log-plot oder sqrt-plot */

{ case 1: if(rzmax <= 0) rzmax=1e-30;
          if(rzmin <= 0) rzmin=1e-30;
    break;    /* log */
  case 2: if(rzmax <= 0) rzmax=0.;
          if(rzmin <= 0) rzmin=0.;
    break;    /* sqrt */
  default:
  break;
}

if(fabs(rzmax-rzmin) < 1.0e-30) rzmax=rzmin+1.;
if(rzmax>rzmin) rzlow=rzmin,rzhigh=rzmax;
else    rzlow=rzmax,rzhigh=rzmin;

/* check xscale yscale */
if((xscalei < 0.) || (xscalei > 1.)) xscalei=1.;
if((yscalei < 0.) || (yscalei > 1.)) yscalei=1.;
if((xscalei==0) && (yscalei==0)) xscalei=0.75,yscalei=0.5;
xscale=xscalei; yscale=yscalei;

/* additioanal parameter */
imxincf=(ixmax-ixmin)/20+ixinc*20;      /* marker increment fast  */
imxincs=imxincf/20;      /* marker increment slow */
rmzinc=(rzmax-rzmin)/20.0;      /* marker z increment */
if(ililog < 0 || ililog > 2) ililog=0;  /* check linlog */
if(ihpvf < 0 || ihpvf > 4) ihpvf=0;  /* check histo ... */


/*  for debugging
dbgi(inx);dbgi(iny);dbgi(iymin);dbgi(iymax);dbgi(ixmin);dbgi(ixmax);
dbgi(icomfi);dbgi(icomf);dbgf(rzmin);dbgf(rzmax);dbgf(xscale);dbgf(yscale);
dbgi(ixml);dbgi(ixmr);dbgf(rzmu);
dbgi(imxincf);dbgi(imxincs);dbgf(rmzinc);
{int i; for(i=1;i<10;i++) { dbgi(i); dbgf(rdat(i,1)); dbgf(rdat(i,2));} }
*/

/* clear the screen and set dots on in graficmode */
y2dpmarker('c');
y2dpgraphic(); if(pmode<2) y2dpclear(); y2dpsetdots();


/* annotate the coordinates */
if(pmode<2)
{
  static const char *strhpvf[]  = { "Hist","Point","Vector","Fill","Box" };
  static const char *strlilog[] = { "Lin","Log","Sqrt" };
  if(graficfile==0)
  { y2dpalpha(0);
    printf("X: %g (%d) %g (%d) *** Y: %g (%d) %g (%d) \
          \nZ: %g %g *** C: %d *** %s *** %s\n",
     rxmini,ixmin,rxmaxi,ixmax,rymini,iymin,rymaxi,iymax,
     rzmin,rzmax,icomf,strlilog[ililog],strhpvf[ihpvf]); }
  y2dpgraphic();

  // char line[100];
  // sprintf(line,"%s ",comment);
  // y2dptextout(scxmin+cxpixel,scymax+1*cypixel,strlen(line),line);
}


{ /* plot section */
int ig,ign;
int ix;
double rx,rxold=0,rxnext=0;

int nofgroups=1+abs(iymax-iymin);
double groupf=1./nofgroups;

double cxscale = (xscale>groupf)? xscale : groupf;
double cyscale = (yscale>groupf)? yscale : groupf;

double xwidth=(scxmax-scxmin)*cxscale;
double xminfirst=scxmin;
double xmaxfirst=scxmin+xwidth;
double xminlast=scxmax-xwidth;
double xmaxlast=scxmax;
double xstep=(xminlast-xminfirst)/(nofgroups-0.9999);

double ywidth=(scymax-scymin)*cyscale;
double yminfirst=scymin;
double ymaxfirst=scymin+ywidth;
double yminlast=scymax-ywidth;
double ymaxlast=scymax;
double ystep=(yminlast-yminfirst)/(nofgroups-0.9999);


for (ign=0,ig=iymin;ig!=iymax+iyinc;ig+=iyinc,ign++)
{ /* loop over groups */
double rxinc;
xscmin=xminfirst+xstep*ign; xscmax=xscmin+xwidth;
yscmin=yminfirst+ystep*ign; yscmax=yscmin+ywidth;

rxinc=(xscmax-xscmin)/(abs(ixmax-ixmin)+1)*icomf;
xmult=(xscmax-xscmin)/(ixmax-ixmin+ixinc);
xadd=xscmin-(ixmin-(ixinc*0.5))*xmult;

switch(ililog)
{ case 0: ymult=(yscmax-yscmin)/(rzmax-rzmin);
          yadd=yscmin-rzmin*ymult; break;
  case 1: ymult=(yscmax-yscmin)/(log10(rzmax)-log10(rzmin));
          yadd=yscmin-log10(rzmin)*ymult; break;
  case 2: ymult=(yscmax-yscmin)/(sqrt(rzmax)-sqrt(rzmin));
          yadd=yscmin-sqrt(rzmin)*ymult; break;  }

/* all koord. computed,  now window aktiv  */

if(y2dpgetkey(-1)!=0) return 0;

/* calculate x-y coordinates and plot on screen */


switch(ihpvf)
{
  case 0: y2dpvectorplot();
          rxold=xscmin; rxnext=rxold+rxinc; break;
  case 1: y2dppointplot(); break;
  default: y2dpvectorplot(); break;
}

{
int n;
int vectoron=1;
for (ix=ixmin,n=0; n<=(abs(ixmax-ixmin)+1)/icomf && ix!=ixmax+ixinc;
     n++,ix+=(icomf*ixinc))
{ /* loop over x channels */
double ry=0.; int i1; int nchan;
for (nchan=0,i1=ix; nchan<icomf && i1>0 && i1<=inx; i1+=ixinc,nchan++)
{ ry += rdat(i1,ig); }
ry /= nchan; rx = (ix+(ix+ixinc*(nchan-1)))/2.0;
switch(ililog)
{ case 0: break;
  case 1: if(ry<rzlow) ry=rzlow/2; ry=log10(ry); break;
  case 2: if(ry<rzlow) ry=rzlow/2; ry=sqrt(ry); break; }
ry=ry*ymult+yadd;
rx=rx*xmult+xadd;
if(y2dpgetkey(checkcount)!=0) { y2dpendplot(); return 0; }

switch(ihpvf)
{
  case 0:
    if(ry<yscmin) ry=yscmin;
    if(ry>yscmax) ry=yscmax;
    y2dpxy(rxold,ry);
    if(rxnext>xscmax) rxnext=xscmax;
    y2dpxy(rxnext,ry);
    rxold=rxnext; rxnext+=rxinc;
    break;
  case 1:
    if(ry<=(yscmin+0.00001) || ry>=(yscmax-0.00001)) break;
    if(rx>xscmax) break;
    y2dpxy(rx,ry);
    break;
  case 2:
    if( (ry<=(yscmin-0.00001)) || (ry>=(yscmax+0.00001)))
    {
      if (vectoron==1){ y2dpendplot(); vectoron=0; break; }
      else{ break; }
     }
     if( vectoron==0 ){ y2dpvectorplot(); vectoron=1; }
     if(rx>xscmax) break;
     y2dpxy(rx,ry);
     break;
  case 3:
    if(ry<=(yscmin+0.00001)) break;
    if(ry>yscmax) ry=yscmax;
    if(rx>xscmax) break;
    y2dpline(rx,yscmin,rx,ry);
    break;
  case 4:
    if(ry<=(yscmin-cxpixel/2.) || ry>=(yscmax+cxpixel/2.)) break;
    {
      double ry1=ry-cxpixel/2;
      double ry2=ry+cxpixel/2;
      double rx1=rx-cxpixel/2;
      double rx2=rx+cxpixel/2;
      if(ry1>yscmax) ry1=yscmax;
      if(ry2>yscmax) ry2=yscmax;
      if(ry1<yscmin) ry1=yscmin;
      if(ry2<yscmin) ry2=yscmin;
      if(rx1>xscmax) rx1=xscmax;
      if(rx2>xscmax) rx2=xscmax;
      if(rx1<xscmin) rx1=xscmin;
      if(rx2<xscmin) rx2=xscmin;
      y2dpline(rx1,ry1,rx1,ry2);
      y2dpline(rx1,ry2,rx2,ry2);
      y2dpline(rx2,ry2,rx2,ry1);
      y2dpline(rx2,ry1,rx1,ry1);
    }
          /* y2dptextout(rx-cxpixel/2,ry-cypixel/3,1,"o"); */
    break;
  default:break; }

} /* end loop over x-channels */
}

y2dpendplot();

/* end loop over groups */ }

/* plot the frame */
{
int i1,i2;
for (i1 = -1; i1<=1; i1++)
for (i2 = -1; i2<=1; i2++)
{
  y2dpvectorplot();
  y2dpxy(i1+xminfirst,i2+yminfirst); y2dpxy(i1+xminfirst,i2+ymaxfirst);
  y2dpxy(i1+xminlast, i2+ymaxlast);  y2dpxy(i1+xmaxlast, i2+ymaxlast);
  y2dpxy(i1+xmaxlast, i2+yminlast);  y2dpxy(i1+xmaxfirst,i2+yminfirst);
  y2dpxy(i1+xminfirst,i2+yminfirst);
  y2dpendplot();
} /* end for */


i1=0;i2=0;
if(xscale!=1 && yscale!=1)
{ y2dpline(i1+xminlast,i2+yminlast,i1+xminfirst,i2+yminfirst);
  y2dpline(i1+xminlast,i2+yminlast,i1+xminlast, i2+ymaxlast);
  y2dpline(i1+xminlast,i2+yminlast,i1+xmaxlast, i2+yminlast); }

/* plot ticks */
{
int zmaxticks=20,xmaxticks=20,ymaxticks=20;

if (ililog==1) zmaxticks=30;
if(xscale==1 && yscale==0 && iymin != iymax)
{ int i,groups=abs(iymax-iymin)+1;
  // double ywidht=ymaxfirst-ymaxfirst;
  for (i=0;i<groups;i++)
  { double ylow=yminfirst+i*ywidth,yhigh=ymaxfirst+(i-0.2)*ywidth;
    y2dppticks(zmaxticks,rzmin,rzmin+(rzmax-rzmin)*0.8,
               xminfirst,ylow,xminfirst,yhigh,'l',ililog); }   }
else { y2dppticks(zmaxticks,rzmin,rzmax,
             xminfirst,yminfirst,xminfirst,ymaxfirst,'l',ililog); }

if( lx==1.0 && hx==inx && (xmaxticks > ixmax-ixmin+2) )
  xmaxticks=abs(ixmax-ixmin)+2;
y2dppticks(xmaxticks,y2dpxnx(ixmin-ixinc*0.5),y2dpxnx(ixmax+ixinc*0.5),
             xminfirst,yminfirst,xmaxfirst,yminfirst,'d',0);

if( ly==1.0 && hy==iny && (ymaxticks > iymax-iymin+1) )
ymaxticks=abs(iymax-iymin)+1;
y2dppticks(ymaxticks,y2dpyny(1.0*iymin),y2dpyny(1.0*iymax),
             xmaxfirst,yminfirst,xmaxlast,yminlast,'r',0);
}
} /* end plot frame */

} /* end of plot section */

return 0;
}



/*=== Function ===================================================*/

int y2dplotnorm
(
  const char *text, int dmode,
  int ny, double yle, double yhe,
  int nx, double xle, double xhe,
  void *data
)

/*--- Description ------------------------------------------------//

Displays a field of one and two dim. array elements.

programmer:     Thomas Kihm
date:           Oct-1989 ... oct-2012
version:        3.0

this replaces the old style y2dplot without the renormalization
of x and y axis.

//--- Parameters -------------------------------------------------//

text:   field of text comment

dmode:  display mode

        data pointer must be a pointer to a data type
        corresponding to the dmode specified:

        'f'=float 'd'=double
        'i'=int 's'=short
        'I'=unsigned int 'S'=unsigned short

        if you add 'p' data must be a field pointers to the
        specified type. e.g. if dmode is 'S'+'p' data must be
        unsigned short **data

ny:     number of y channels
yle:    low edge of the y index
yhe:    high edge of the y index
        if yle==yhe 0 and ny is used

nx:     number of x channels
xle:    low edge of the x index
xhe:    high edge of the x index
        if xle==xhe 0 and nx is used

data:   data field

in interactive mode the function returns
a character 'e','q','u','d' according to the exit command
entered by the interactive user (see help command).
if a uppercase key was pressed the returned value is the
pressed key.

The text string is used to store the last coordinates
of the plot. the files are store in $HOME/.y2dplot.
Make sure that text is different for all plots. If text and
nx and ny are equal the data share the same config file.

Online help is available by typing '?' in command mode!

examples:

y2dplotnorm("test ---  4 spectra a 1000 channels",
         'd',4,0,0,1000,0,0,float_data_array);

y2dplotnorm("test ---  4 spectra a 1000 channels",
         'S',4,0,0,1000,0,0,unsiged_short_data_array);

//----------------------------------------------------------------*/


/*==> Interactive Commands <======================================//

y2dplot has a powerful interactive part which
lets the user select various plot forms, different
coordinates to plot and many other features like markers.

The terms within the y2dplot are as follows:

Given a data array

        double z[iy][ix]

y2dplot plots the contents (z) on the screen in
direction up (y-screen-direction), (ix) array elements are
are plotted in horizontal direction (x-screen-direction) and
the (iy) coordinate of the array is plotted from down left to
upper right corner of the screen. A channel is one value
z[y][x]. A group is any one dimesional array (for fixed iy)
of a two dimesional array with z[y][x].

Each command is executed by pressing the corresponding
key on the keyboard. If the graphics driver opens a new window
the standard input is use for input. If y2dplot is currently
plotting it will immediatly terminate the plot and execute
the given command. With this feature you can terminate long plot
sequences on very slow terminals. After executing the
command most often a new graph is plotted on screen.

y2dplot has on top of the screen or by window system
in a separate window two status lines containing:

X: low-x (low-x in channels) high-x (high-x in channels)
Y: low-y (low-y in channels) high-y (high-y in channels)
Z: low-z high-z
C: X-compression in channels
lin log field: Lin, Log, Sqrt
plot form field: Hist, Vector, Point

Example:
X: 1 (1) 1024 (1024) *** Y: 0 (1) 0 (1)
Z: -0.8 100.2 *** C: 2 *** Lin *** Hist

NOTE:
If you are using a window system (vws or x-window) input
must be given in the window where the status lines appears.



Command-List
-------------------------------------------------------------

Some commands will prompt you for one or two values.
Defaults are given in brackets [].

Help command:
-------------

?:      Will give a short help about available commands.

Coordinate commands:
--------------------

x:      Enter x-min and x-max.
        You can enter for each value any number or s,l or h
        which have the meaning: same, lowest, highest value
        possible.
        example: X-min, X-max [ 1 1000 ] ? 100 h
        or     : X-min, X-max [ 100 1024 ] ? 1000 100

y:      Enter y-min and y-max.
        You can enter for each value any number or s,l or h
        which have the meaning same, lowest, highest value
        possible.

z:      Enter z-min and z-max.
        You can enter for each value any number or s,l or h
        which have the meaning same, minimum, maximum.

c:      Enter x-compression in channels.
        The x coordinate can be compressed. This means
        y2dplot computes the average of
        <x-commpression> channels to plot one channel
        on the screen. This feature lets you plot on slow
        terminals large amount of data.
        By the special case c=0 Fplorerealarray will compute
        for each x-section the compression in the way that
        only 512 channels are shown on the screen (auto
        compression). This feature is most often turned on.

1:      Toggle between c=0 and c=1.

2:      Find z-min z-max.

a:      Same as
        set x to : l h
        set y to : l h
        set z to : l h
        set c to : 0

#:      Select x and y calibration constants.
        with this feature you can renormalize the x and y
        axis to new values.

j:      Same as
        y-min = y-min + one group
        y-max = y-min
        which selects just one one dim. array out of
        a two dim array.

k:      Same as
        y-min = y-min - one group.
        y-max = y-min

o:      Overlay a y group.

g:      Enter isovector control. when plotting two dim.
        arrays you can select the fractional part of
        the screen in x and y to show one y-group.
        Default is x-scale=0.7 y-scale=0.5 .
        (100% corresponds 1.0).
        There are two special cases:
        x-scale=1.0 y-scale=0.0:
        Plot all y group one upon an other.
        x-scale=1.0 y-scale=1.0:
        Overlay all y group in plot.

i:      Toggle between
        x-scale=0.7 y-scale=0.5 and
        x-scale=1.0 y-scale=0.0


Plot control commands:
----------------------

h:      Histogram plot. Channels are connected with
        vertical and horizontal lines.

f:      Fill plot. Channels are shown with vertical bars.

v:      Vector plot. Channels are connected by lines.

p:      Point plot. Channels are shown as small points.

n:      Linear plot.

l:      Logarithmic plot.

s:      Sqare root plot.


Control commands:
-----------------

blank (space bar):
        Redraw the the last plot.

e:      Save coordinates and exit with return code 'e'.

q:      Dont save and return with 'q'.

u:      Dont save and return with 'u'.

d:      Save and return with 'd'.

b:      Store all coordinates and plot forms in an
        internal buffer.

r:      Recall stored values buffered by b command.

=:      Create plot file.
        y2dplot prompts for file name.

~:      Setup graphic window

Marker and window commands:
---------------------------

There are three markers: Two x-markers and one z-marker.
The commands available for the markers are:

w:      Window in the middle of the current window.
        x-markers and z-marker is set visible in the
        center of the window.

m:      x-markers (left and right) are set in the middle
        of the window. This mode is called the marker mode.
        The status line contains:
        M: x-position (x in channels) value= z-value
        with this mode you can scan and display the
        numerical values of your data array.
        NOTE: If more than one y-group is displayed the
        marker displays the value of the last y-group
        plotted on the screen.


Marker movement:
----------------

Use the following commands on the keypad if possible.

8:      Move window or marker to the left (slow)

9:      Move window or marker to the right (slow).

7:      Move window or marker to the left (fast).

-:      Move window or marker to the right (fast).

5:      Make window smaller (slow).

6:      Make window bigger (slow).

4:      Make window smaller (fast).

+:      Make window bigger (fast).

3:      z-marker up.

.:      z-marker down.
,:      z-marker down.


Marker control:
---------------

0:      Plot with z-marker = z-max

/:      Plot with z-marker = z-min

;:      Set normalisation with x-markers.
        The calibration of the x-axis can be set by moving
        the markers and then give the new x-values with
        the ';' command.

t:      Totalize the range between the markers.
        Only possible by one dimesional arrays.

return/enter:
        Plot with x-min x-max set to the x-markers.


//--------------------------------------------------------------------*/

{
char  chr;
int ix,iy;

if(dmode<=2 && dmode >=0)  pmode=dmode,dtype='f';
else dtype=dmode,pmode=0;

rdata=data;
comment=text;
inx=nx;iny=ny;
if(inx<2 || iny<1)
{ printf("ERROR y2dplot: can not plot, nx < 2 or ny < 1 \n"); return 0; }


rxmini=rymini=(-1e30);
rxmaxi=rymaxi=1e30;
ililog=ihpvf=0;
icomfi=0;
rzmini=rzmaxi=0;
xscalei=yscalei=0;

y2dprestore(inx,iny);
if(xhe!=xle)
{
  double ss=(xhe-xle)/nx;
  lx=xle+ss/2, hx=xhe-ss/2;
}
if(lx==hx) lx=1,hx=nx;

if(yhe!=yle)
{
  double ss=(yhe-yle)/ny;
  ly=yle+ss/2, hy=yhe-ss/2;
}
if(ly==hy) ly=1,hy=ny;

ixmini=y2dpnx(rxmini); ixmaxi=y2dpnx(rxmaxi);
iymini=y2dpny(rymini); iymaxi=y2dpny(rymaxi);

y2dpopen();
graficfile=0;           /* interactive */

y2dpdraw();
chr=y2dpgetkey(2);            /* try to read an abort commnad */
if((chr==0)&&(pmode>0)) { y2dpalpha(0); y2dpclose(); return 0; }

getcmd:    /* get command */
pmode=0;
chr=y2dpgetkey(2);            /* try to read a abort command */
if(chr==0) chr=y2dpgetkey(1);  /* if no chr wait for it */
y2dpgetkey(3);                /* and clear the buffer */

if(chr>='A' && chr<='Z')
{
   y2dpsave('p'); y2dpalpha(0); return chr;
}
switch(chr)
{
  case ' ': /* plot */
      y2dpdraw(); break;

  case '?': /* help */
      y2dphelp(); break;

  case  0 : ;
  case 'e': /* exit with clear and save */
      y2dpsave('p'); y2dpclear(); y2dpalpha(0); y2dpclose(); return chr;

  case 'q': /* quit with clear and nosave */
      y2dpsave('p'); y2dpclear(); y2dpalpha(0); y2dpclose(); return chr;

  case 'd': /* exit noclear save (new display mode) */
      y2dpsave('p'); y2dpalpha(0);
      return chr;

  case 'u': /* exit noclear nosave (update mode) */
        y2dpsave('p'); y2dpalpha(0);
      return chr;

  case 'o': /* overlay */
            if(iymax!=iymin)
      { y2dpalpha(0); printf("Y-min not Y-max!\n"); break; }
            y2dpgetint("Y- overlay group",&iymini);
      iymaxi=iymini; pmode=2;  y2dpdraw(); pmode=0; break;

  case '~': y2dpalpha('=');
            y2dpclear();
            y2dpclose();
            {
              double d1,d2;
              d1=sxsize; d2=sysize; y2dpget2double("window size x y",&d1,&d2);
              sxsize=d1; sysize=d2;
              d1=sxpos; d2=sypos; y2dpget2double("window pos x y ",&d1,&d2);
              sxpos=d1; sypos=d2;
            }
            y2dpopen();
            y2dpalpha(0);
      graficfile=0;
      y2dpdraw();
            break;

  case '=': /* create plot file */

            y2dpalpha(0);
            sscanf(comment,"%80s",plotname); strncat(plotname,".plt",90);
            y2dpgetstring("Enter plot filename ",80,plotname);
            y2dpalpha('=');
            printf("Creating plot file ( %s ), please wait ...\n",plotname);
            // printf("%s x %d %d y %d %d rx %g %g\n",
            //   plotname,ixmin,ixmax,iymin,iymax,rxmini,rxmaxi,rymini,rymaxi);
            {
            double xm,ym;
            FILE *fp=fopen(plotname,"w");
            if(fp==0) { printf("ERROR creating plotfile %s\n",plotname); break; }
            if(ixmax==ixmin) xm=0; else xm=(rxmaxi-rxmini)/(ixmax-ixmin);
            if(iymax==iymin) ym=0; else ym=(rymaxi-rymini)/(iymax-iymin);

            for (iy=iymin;iy!=iymax+iyinc;iy+=1)
            {
              for (ix=ixmin; ix!=ixmax+ixinc; ix+=1)
              {
                double d=rdat(ix,iy);
                // double x,y;
                fprintf(fp,"%d %d %g %g %g\n",
                   ix,iy,d,rxmini+(ix-ixmin)*xm,rymini+(iy-iymin)*ym);
              }
              fprintf(fp,"\n");
            }
            fclose(fp);
            printf("plot file ( %s ) created\n",plotname);
            }
            break;

  case 'x': /* get x min max */
            y2dpget2double("X-min,X-max",&rxmini,&rxmaxi);
      ixmini=y2dpnx(rxmini); ixmaxi=y2dpnx(rxmaxi);
            y2dpdraw(); break;

  case 'y': /* get y min max */
            y2dpget2double("Y-min,Y-max",&rymini,&rymaxi);
            iymini=y2dpny(rymini); iymaxi=y2dpny(rymaxi);
            y2dpdraw(); break;

  case 'z': /* get z min max */
            y2dpget2double("Z-min,Z-max",&rzmini,&rzmaxi); y2dpdraw(); break;

  case 'g': /* get iso vector control x y */
            y2dpget2double("X-scale,Y-scale",&xscalei,&yscalei);
            y2dpdraw(); break;

  case 'c': /* get x compression */
            y2dpgetint("X-compression",&icomfi); y2dpdraw(); break;

  case 'h': /* histogramm */
      ihpvf=0; y2dpdraw(); break;

  case 'p': /* point */
            ihpvf=1; y2dpdraw(); break;

  case 'v': /* vector, continous */
            ihpvf=2; y2dpdraw(); break;

  case 'f': /* fill */
            ihpvf=3; y2dpdraw(); break;

  case '*':
  case '[': /* sqares */
            ihpvf=4; y2dpdraw(); break;

  case 'n': /* linear */
            ililog=0; y2dpdraw(); break;

  case 'l': /* log */
            ililog=1; y2dpdraw(); break;

  case 's': /* square root */
            ililog=2; y2dpdraw(); break;

  case 'a': /* show all */
            iymini=ixmini=1; iymaxi=iny; ixmaxi=inx; icomfi=0;
            rzmini=rzmaxi=0.; y2dpdraw(); break;

  case '2': /* find min max */
            rzmaxi=rzmini=0; y2dpdraw(); break;

  case '@': /* find min max */
            rzmaxi=rzmax+(rzmax-rzmin)/10.;
      rzmini=rzmin-(rzmax-rzmin)/10.;
      y2dpdraw(); break;

  case '1': /* toggle compression factor */
            if(icomfi!=0) icomfi=0; else icomfi=1;
            y2dpdraw(); break;

  case 'j': /* next group */
            iymini=iymini+1;
            if(iymini>iny) iymini=1;
            iymaxi=iymini; y2dpdraw(); break;

  case 'k': /* previous group */
            iymini=iymini-1;
            if(iymini < 1) iymini=iny;
            iymaxi=iymini; y2dpdraw(); break;

  case 'i': /* iso vector control */
            if((yscalei==0.5)&&(xscalei==0.75)) xscalei=1.,yscalei=0.;
            else if((yscalei==0)&&(xscalei==1)) xscalei=1.,yscalei=1.;
      else yscalei=0.5,xscalei=0.75;
      y2dpdraw(); break;

  case 't': /* total */
      y2dpalpha(0);
            for(iy=iymin; iy<=iymax; iy++)
      {
            if(ixml < 1) ixml=1;
            if(ixmr < 1) ixmr=1;
            if(ixml > inx) ixml=inx;
            if(ixmr > inx) ixmr=inx;
      {
        int i; double rsum,rbckg,r1,r2;
        if(ixmr >= ixml)
        for(rsum=0.,i=ixml;i<=ixmr;i++) rsum += rdat(i,iy);
        else
        for(rsum=0.,i=ixmr;i<=ixml;i++) rsum += rdat(i,iy);
              r1=rdat(ixml,iy); r2=rdat(ixmr,iy);
              rbckg=r1/2+r2/2; if ((rbckg > 1e30) || (rbckg < -1e30)) rbckg=0;
              rbckg *= (abs(ixmr-ixml)+1);
              printf("W: Y: %d X: %g (%d) %g (%d) Total=%g Bckg=%g Total-Bckg=%g\n",
              iy,y2dpxnx((double)ixml),ixml,y2dpxnx((double)ixmr),ixmr,
              rsum,rbckg,rsum-rbckg);
      }
            }
            break;

  case 'b': /* store current parameter */
      ibxmin=ixmini; ibxmax=ixmaxi;
            ibymin=iymini; ibymax=iymaxi;
            rbzmin=rzmini; rbzmax=rzmaxi;
            ibcomf=icomfi; ibhpvf=ihpvf; iblilog=ililog;
            xbscale=xscalei; ybscale=yscalei;
            buffered=1;
      y2dpalpha(0); printf("coordinates buffered recall with r!\n");
      break;

  case 'r': /* recall buffered parameter */
            if(buffered==0) { printf("nothing buffered - sorry\n"); break; }
            ixmini=ibxmin;ixmaxi=ibxmax;
            iymini=ibymin;iymaxi=ibymax;
            rzmini=rbzmin;rzmaxi=rbzmax;
            icomfi=ibcomf;ihpvf=ibhpvf;ililog=iblilog;
            xscalei=xbscale;yscalei=ybscale;
      y2dpdraw(); break;

  case '$': /* reset x,y calibration */
      { lx=1;hx=inx; }
      { ly=1;hy=iny; }
            if(xhe!=xle)
            {
              double ss=(xhe-xle)/nx;
              lx=xle+ss/2, hx=xhe-ss/2;
            }
            else lx=1,hx=nx;

            if(yhe!=yle)
            {
              double ss=(yhe-yle)/ny;
              ly=yle+ss/2, hy=yhe-ss/2;
            }
            else ly=1,hy=ny;
            y2dpdraw(); break;

  case '#': /* get x,y calibration */
            y2dpget2double("Calibration X-low,X-high",&lx,&hx);
      if( fabs(hx-lx) <= 1e-30 ) { lx=1;hx=inx; }
            y2dpget2double("Calibration Y-low,Y-high",&ly,&hy);
      if( fabs(hy-ly) <= 1e-30 ) { ly=1;hy=iny; }
            y2dpdraw(); break;

  case ';': /* set marker x,y calibration */
            if(ixml==ixmr)
            { y2dpalpha(0); printf("Window not defined! \n"); break; }
      { double a,b,r1=y2dpxnx(1.0*ixml),r2=y2dpxnx(1.0*ixmr);
            y2dpget2double("Window calibration X-left,X-right",&r1,&r2);
      a = (r2-r1)/(ixmr-ixml); b = r1 - a*ixml;
      lx = a + b; hx = a*inx + b;
      if( fabs(hx-lx) <= 1e-30 ) { lx=1;hx=inx; }
            y2dpdraw(); break; }


  case '\n':/* make plot with x-markers */
            ixmini=ixml; ixmaxi=ixmr; y2dpdraw(); break;

  case '0': /* make plot with z-marker=zmax */
            rzmaxi=rzmu; y2dpdraw(); break;

  case '/': /* make plot with z- marker=zmin */
            rzmini=rzmu; y2dpdraw(); break;

  case 'w': /* window in the middle */
            rzmu=(rzmax+rzmin)/2.;
            ixml=(ixmax-ixmin)*0.333333+ixmin+0.5;
      ixmr=(ixmax-ixmin)*0.666666+ixmin+0.5;
      y2dpmarker('b'); break;

  case 'm': /* m=marker-mode */
            ixml=(ixmin+ixmax)/2.0,ixmr=ixml;
      y2dpmarker('x'); break;

  case '9': /* 9=x-window slow to the right */
            if(ixml == ixmr) ixml=ixml+ixinc, ixmr=ixml;
            else ixml=ixml+imxincs, ixmr=ixmr+imxincs;
            y2dpmarker('x'); break;

  case '8': /* 8=x-window slow to the left */
            if(ixml == ixmr) ixml=ixml-ixinc,ixmr=ixml;
      else ixml=ixml-imxincs,ixmr=ixmr-imxincs;
            y2dpmarker('x'); break;

  case '-': /* -=x-window fast right */
            ixml=ixml+imxincf,ixmr=ixmr+imxincf; y2dpmarker('x'); break;

  case '7': /* 7=x-window fast left */
            ixml=ixml-imxincf,ixmr=ixmr-imxincf; y2dpmarker('x'); break;

  case '4': /* 4=x-window fast smaller */
            ixml=ixml+imxincf,ixmr=ixmr-imxincf; y2dpmarker('x'); break;

  case '5': /* 5=x-window slow smaller */
            ixml=ixml+imxincs,ixmr=ixmr-imxincs; y2dpmarker('x'); break;

  case '6': /* 6=x-window slow greater */
            ixml=ixml-imxincs,ixmr=ixmr+imxincs; y2dpmarker('x'); break;

  case '+': case ',': /* ,+=x-window fast greater */
            ixml=ixml-imxincf,ixmr=ixmr+imxincf; y2dpmarker('x'); break;

  case '.': /* .=z-marker down */
            if(ililog)
            {
              if(rzmu<0) rzmu=1;
              rzmu/=sqrt(2.);
            }
            else rzmu=rzmu-rmzinc;
            y2dpmarker('y'); break;

  case '3': /* 3= z-marker up */
            if(ililog)
            {
              if(rzmu<0) rzmu=1;
              rzmu*=sqrt(2.);
            }
            else rzmu=rzmu+rmzinc;
            y2dpmarker('y'); break;

  default: /* no valid command */
     y2dpalpha(0); printf("Invalid command, type ? for help (q or e to quit or exit)\n");
           break;
}

goto getcmd;

}

/*=== Function ===================================================*/

int y2dplot(const char *text, int dmode, int ny, int nx, void *data)

/*--- Description ------------------------------------------------//

This is the old style interface with out the low edge and high
edge paramters.

//----------------------------------------------------------------*/
{
  return y2dplotnorm(text,dmode,ny,0.,0.,nx,0.,0.,data);
}

/*=== Function ===================================================*/

int y2dpgetmarker(int *xml, int *xmr, double *zm)

/*--- Description ------------------------------------------------//

returns the value of the last x and z markers

xml =   left x marker

xmr =   right x marker

zm  =   z marker

WARNING: this function has been changed in version >3

//----------------------------------------------------------------*/
{
// comment=text;
// inx=1;iny=1;
// y2dprestore(inx,iny);

*xml = ixml-1; *xmr = ixmr-1; *zm = rzmu;
return 1;
}
