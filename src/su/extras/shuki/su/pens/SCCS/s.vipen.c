h23729
s 00000/00000/00171
d D 1.3 88/11/15 14:07:34 shuki 3 2
c 
e
s 00001/00000/00170
d D 1.2 88/05/16 06:42:14 shuki 2 1
c 
e
s 00170/00000/00000
d D 1.1 88/04/14 13:59:31 shuki 1 0
c date and time created 88/04/14 13:59:31 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * vipen - vplot command interpreter for a Visual (?)
 */

#include <stdio.h>
#include <math.h>
/* Device independent constants */
#include "pen.h"

#define NL	012
#define FF	014
#define CAN	030
#define ESC	033
#define FS	034
#define GS	035
#define US	037

int xargc; char **xargv;

/* Device dependent constants */
#define MRPERIN	100 /*78*/
/* #define MYSIZE	400 */
/* #define MYSIZE	200 */
#define MYSIZE	800
#define MXSIZE	1056

#define ALPHA	1
#define VECTOR	2
#define POINT	3

static int lost,tekmode;
static FILE *tty;
FILE *fopen();

pen_vpause(i)
int i;
{
	sleep(i);
}

puttek(x,y)
int x,y;
{
	int hix,hiy,lox,loy;
	static int ohix,ohiy,olox,oloy;
	hiy = 0040|((y>>5)&037);
	loy = 0140|(y&037);
	hix = 0040|((x>>5)&037);
	lox = 0100|(x&037);
	if(lost||hiy!=ohiy)
		putc(hiy,stdout);
	if(lost||loy!=oloy||hix!=ohix)
		putc(loy,stdout);
	if(lost||hix!=ohix)
		putc(hix,stdout);
	if(lost||hiy!=ohiy||hix!=ohix||loy!=oloy||lox!=olox)
		putc(lox,stdout);
	lost = 0;
}

pen_pldone()
{
	unsigned short xy[2];
	xy[0] = 0;
	xy[1] = 0;
	pen_move(xy);
	fprintf(stdout,"%c%c0Hit return to continue (the plot will disappear)",
			US,ESC);
I 2
	fflush(stdout);
E 2
	getc(tty);
 	setmode(ALPHA);
	fprintf(stdout,"\n");
}
setmode(mode)
int mode;
{
	switch(mode) {
		case VECTOR:
			if (tekmode!=VECTOR) {
				putc(GS,stdout);
				tekmode = VECTOR;
			}
			break;
		case ALPHA:
			if (tekmode!=ALPHA) {
				putc(CAN,stdout);
				tekmode = ALPHA;
			}
			lost = 1;
			break;
		case POINT:
			if (tekmode!=POINT) {
				putc(FS,stdout);
				tekmode = POINT;
			}
			lost = 1;
			break;
		default:
			err(__FILE__,__LINE__,"setmode: unknown mode %d\n",mode);
	}
}
pen_init()
{
	tty = fopen("/dev/tty","r");
	tekmode = -1;
	pen_erase();
	setmode(VECTOR);
}
pen_erase()
{
	setmode(ALPHA);
	putc(ESC,stdout);
	putc(FF,stdout);
	lost = 1;
}
pen_move(xy)
unsigned short xy[2];
{
	int x,y;
	localxy(xy,&x,&y);
/* 	setmode(ALPHA); */
	putc(GS,stdout);
	puttek(x,y);
}
pen_draw(xy)
unsigned short xy[2];
{
	int x,y;
	localxy(xy,&x,&y);
/* 	setmode(VECTOR); */
	puttek(x,y);
}
localxy(xy,x,y)
unsigned short xy[2];
int *x,*y;
{
	*x = xy[0]*MRPERIN/RPERIN;
	if(*x>MXSIZE) *x = MXSIZE;
	else if(*x<0) *x = 0;
	*y = xy[1]*MRPERIN/RPERIN;
	if(*y>MYSIZE) *y = MYSIZE;
	else if(*y<0) *y = 0;
}
pen_col(col)
short int col;
{
}
pen_fat(fat)
short int fat;
{
	/* NOT READY */
}
pen_Text(size,orient,string)
short int size,orient;
char *string;
{
/* 	fprintf(stdout,"%c%c0%s",US,ESC,string); */
}
pen_text(size,orient,string)
short int size,orient;
char *string;
{
/* 	fprintf(stdout,"%c%c0%s",US,ESC,string); */
}

pen_area(lp,ua,va)
short lp;
float *ua,*va;
{
/* 	fprintf(stdout,"p[%d,%d];\n",ua[0],va[0]); */
}
E 1
