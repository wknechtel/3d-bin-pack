/*
// ***************************************************************************
// INCLUDED HEADER FILES
// ***************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <graphics.h>

/*
// ***************************************************************************
// Function prototypes
// ***************************************************************************
*/
void Initialize(void);
void Pack(void);
void PutBox(void);
void SayGoodbye(void);
void Pause(void);
void MainWindow(char *header);
void StatusLine(char *msg);
void DrawBorder(void);
void changetextstyle(int font, int direction, int charsize);
int gprintf(int *xloc, int *yloc, char *fint,...);

/*
// ***************************************************************************
// VARIABLE, CONSTANT AND STRUCTURE DECLARATIONS
// ***************************************************************************
*/
int GraphDriver;                            /* The Graphics device driver */
int GraphMode;                              /* The Graphics mode value */
double AspectRatio;                         /* Aspect ratio of a pixel on the screen*/
int MaxX, MaxY;                             /* The maximum resolution of the screen */
int MaxColors;                              /* The maximum # of colors available */
int ErrorCode;                              /* Reports any graphics errors */
struct palettetype palette;                 /* Used to read palette info */

struct dataarray {
  int cx, cy, cz, pax, pay, paz; 
} data[2000];

int px, py, pz, cox, coy, coz, packx, packy, packz, a, b, index, currenty, q;

double max, sc;

char strpx[8], strpy[8], strpz[8], oldstrcoy[8], strcox[8], strcoy[8], strcoz[8], strpackx[8], strpacky[8], strpackz[8];

FILE *igf;

/*
// ***************************************************************************
// BEGIN MAIN FUNCTION
// ***************************************************************************
*/
int main()
{
  Initialize();                             /* Set system into Graphics mode */
  Pack();
  SayGoodbye();                             /* Give user the closing screen */
  closegraph();                             /* Return the system to text mode */
  return(0);
}

/*
// ***************************************************************************
// INITIALIZE: INITIALIZES THE GRAPHICS SYSTEM AND 
// REPORTS ANY ERRORS WHICH OCCURED. 
// ***************************************************************************
*/

void Initialize(void)
{
  int xasp, yasp;                           /* Used to read the aspect ratio*/
  GraphDriver = DETECT;                     /* Request auto-detection */
  initgraph( &GraphDriver, &GraphMode,"");
  ErrorCode = graphresult();                /* Read result of initialization*/
  if (ErrorCode != grOk)                    /* Error occured during init */
  {
    printf(" Graphics System Error: %s\n", grapherrormsg( ErrorCode));
    exit(1);
  } 
  
  getpalette( &palette);                    /* Read the palette from board */
  MaxColors = getmaxcolor() + 1;            /* Read maximum number of colors*/
  MaxX = getmaxx();                         /* Read size of screen */
  MaxY = getmaxy();
  getaspectratio( &xasp, &yasp);            /* read the hardware aspect */
  AspectRatio = (double)xasp / (double)yasp;/* Get correction factor */
}

/*
// ***************************************************************************
// PACK: READS THE DATA FROM "VISUDAT" FILE AND ARRANGES 
// BOXES TO PACK FROM FAR END TO THE CLOSE 
// ***************************************************************************
*/
void Pack(void)
{
  struct viewporttype vp;
  char buffer[10];
  
  MainWindow( "PACKING OF THE BEST SOLUTION FOUND" );
  if ((igf=fopen("visudat","r")) == NULL) 
  {
    outtextxy(0,0,"CANNOT OPEN FILE visudat");
    exit(1);
  }
  
  fscanf(igf,"%s %s %s",strpx, strpy, strpz);
  px = atoi(strpx); 
  py = atoi(strpy); 
  pz = atoi(strpz);
  max = px;
  if (py > max) max = py; 
  if (pz > max) max = pz;
  sc=120/max;
  getviewsettings( &vp);
  settextjustify( CENTER_TEXT, TOP_TEXT );
  changetextstyle( TRIPLEX_FONT, HORIZ_DIR, 4);
  changetextstyle( DEFAULT_FONT, HORIZ_DIR, 1);
  setviewport( vp.left+50, vp.top+40, vp.right-50, vp.bottom-10,1);
  getviewsettings( &vp );
  settextjustify( CENTER_TEXT, CENTER_TEXT);
  outtextxy( 20, 0, "PRESS 'Q' TO QUIT" );
  setcolor(3);
  outtextxy( 220, 0, "PALLET ORIENTATION (X Y Z) : " );
  outtextxy( 350, 0, strpx);
  outtextxy( 390, 0, strpy);
  outtextxy( 430, 0, strpz);
  setfillstyle( EMPTY_FILL, 1);
  bar3d( 10, 350-sc*2*py, 10+sc*2*px, 350, sc*pz, 1);
  outtextxy(460, 30, "COORDINATES:");
  outtextxy(460,40, "CX: CY: CZ:");
  outtextxy(460, 70, "DIMENSIONS:");
  outtextxy(460,80, "DX: DY: DZ:");
  index=1;
  fscanf (igf,"%s %s %s %s %s %s", strcox, strcoy, strcoz, strpackx, strpacky, strpackz);
  cox = atoi(strcox); 
  coy = atoi(strcoy); 
  coz = atoi(strcoz);
  packx = atoi(strpackx); 
  packy = atoi(strpacky); 
  packz = atoi(strpackz);
  data[index].cx = cox; 
  data[index].cy = coy; 
  data[index].cz = coz;
  data[index].pax = packx; 
  data[index].pay = packy; 
  data[index].paz = packz;
  index++; 
  currenty = data[index].cy;
  
  while (fscanf (igf,"%s %s %s %s %s %s", strcox, strcoy, strcoz, strpackx, strpacky, strpackz) != EOF)
  {
    cox = atoi(strcox); 
    coy = atoi(strcoy); 
    coz = atoi(strcoz);
    packx = atoi(strpackx);
    packy = atoi(strpacky); 
    packz = atoi(strpackz);
    data[index].cx = cox; 
    data[index].cy = coy; 
    data[index].cz = coz;
    data[index].pax = packx; 
    data[index].pay = packy; 
    data[index].paz = packz;
    if (data[index].cy != currenty)
    {
      b = index; 
      index--;
      PutBox();
    }
    index++;
    if (q == 1) return;
  }
  index--;
  PutBox();
  fclose(igf);
  Pause();                                  /* Pause for user's response */
}

/*
// ***************************************************************************
// PUTBOX: DRAW BOXES IN THEIR LOCATIONS 
// ***************************************************************************
*/
void PutBox(void)
{
  for (a = index; a > 0; a--)
  {
    setcolor(0);
    outtextxy(528, 50, "REDACTED LINE 1");
    outtextxy(528, 90, "REDACTED LINE 2");
    setcolor(random(15) + 1);
    itoa(data[a].pax, strpackx, 10); outtextxy(410, 90, strpackx);
    itoa(data[a].pay, strpacky, 10); outtextxy(460, 90, strpacky);
    itoa(data[a].paz, strpackz, 10); outtextxy(508, 90, strpackz);
    itoa(data[a].cx, strcox, 10); outtextxy(410,50, strcox);
    itoa(data[a].cy, strcoy, 10); outtextxy(460, 50, strcoy);
    itoa(data[a].cz, strcoz, 10); outtextxy(508, 50, strcoz);
    bar3d(10 + sc * 2 * data[a].cx + sc * data[a].cz, 
          350 - sc * 2 * data[a].cy - sc * .74 * data[a].cz - sc * 2 * data[a].pay, 
          10 + sc * 2 * data[a].cx + sc * data[a].cz + sc * 2 * data[a].pax, 
          350 - sc * 2 * data[a].cy - sc * .74 * data[a].cz, 
          sc * data[a].paz, 1);
    if (toupper(getch()) == 'Q')
    { 
      q = 1; 
      break;
    }
  }
  data[1].cx = data[b].cx; 
  data[1].cy = data[b].cy;
  data[1].cz = data[b].cz; 
  data[1].pax = data[b].pax;
  data[1].pay = data[b].pay; 
  data[1].paz = data[b].paz;
  index = 1;
  currenty = data[index].cy;
}

/*
// ***************************************************************************
// SAYGOODBYE: GIVE A CLOSING SCREEN 
// TO THE USER BEFORE LEAVING. 
// ***************************************************************************
*/

void SayGoodbye(void)
{
  struct viewporttype viewinfo;             /* Structure to read viewport */
  int h, w;
  
  MainWindow( "== The End ==");
  getviewsettings( &viewinfo);              /* Read viewport settings */
  changetextstyle( TRIPLEX_FONT, HORIZ_DIR, 4);
  settextjustify( CENTER_TEXT, CENTER_TEXT );
  h = viewinfo.bottom - viewinfo.top;
  w = viewinfo.right - viewinfo.left;
  outtextxy( w/2, h/2, "That's all, folks!");
  StatusLine( "Press any key to EXIT");
  getch();
  cleardevice(); /* Clear the graphics screen */
}

/*
// ***************************************************************************
// PAUSE: PAUSE UNTIL THE USER ENTERS A KEYSTROKE. 
// ***************************************************************************
*/
void Pause(void)
{
  static char msg[] = "Esc aborts or press a key...";
  int c;
  
  StatusLine(msg);                          /* Put msg at bottom of screen */
  c = getch();                              /* Read a character from kbd */
  if(0 == c)                                /* Did use hit a non-ASCII key? */
  {
    c = getch();                            /* Read scan code for keyboard */
  }
  cleardevice();                            /* Clear the screen */
}

/*
// ***************************************************************************
// MAINWINDOW: ESTABLISH THE MAIN WINDOW 
// ***************************************************************************
*/

void MainWindow( char *header)
{
  int height;
  cleardevice();                            /* Clear graphics screen */
  setcolor(MaxColors -1);                  /* Set current color to white */
  setviewport(0, 0, MaxX, MaxY, 1);        /* Open port to foil screen */
  height = textheight("H");                /* Get basic text height */
  changetextstyle( DEFAULT_FONT, HORIZ_DIR, 1);
  settextjustify( CENTER_TEXT, TOP_TEXT );
  outtextxy( MaxX/2,2, header);
  setviewport(0, height + 4, MaxX, MaxY - (height+4), 1);
  DrawBorder();
  setviewport( 1, height + 5, MaxX - 1, MaxY - (height+5), 1);
}

/*
// ***************************************************************************
// STATUSLINE: DISPLAY A STATUS LINE 
// AT THE BOTTOM OF THE SCREEN. 
// ***************************************************************************
*/
void StatusLine( char *msg)
{
  int height;
  setviewport(0, 0, MaxX, MaxY, 1);         /* Open port to foil screen */
  setcolor(MaxColors -1);                   /* Set current color to white */
  changetextstyle( DEFAULT_FONT, HORIZ_DIR, 1);
  settextjustify( CENTER_TEXT, TOP_TEXT );
  setlinestyle( SOLID_LINE, 0, NORM_WIDTH );
  setfillstyle( EMPTY_FILL, 0 );
  height = textheight("H");                /* Detemine current height */
  bar( 0, MaxY - (height+4), MaxX, MaxY);
  rectangle( 0, MaxY - (height+4), MaxX, MaxY);
  outtextxy( MaxX / 2, MaxY - (height+2), msg);
  setviewport( 1, height + 5, MaxX - 1, MaxY - (height + 5), 1);
}

/*
// ***************************************************************************
// DRAWBORDER: DRAW A SOLID SINGLE LINE 
// AROUND THE CURRENT VIEWPORT. 
// ***************************************************************************
*/
void DrawBorder(void)
{
  struct viewporttype vp;
  setcolor(MaxColors -1);                  /* Set current color to white */
  setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
  getviewsettings(&vp);
  rectangle( 0, 0, vp.right-vp.left, vp.bottom-vp.top);
}

/*
// ***************************************************************************
// CHANGETEXTSTYLE: SIMILAR TO SETTEXTSTYLE, BUT CHECKS FOR 
// ERRORS THAT MIGHT OCCUR WHIL LOADING THE FONT FILE. 
// ***************************************************************************
*/
void changetextstyle(int font, int direction, int charsize)
{
  int ErrorCode;
  graphresult();                            /* clear error code */
  settextstyle(font, direction, charsize);
  ErrorCode = graphresult();                /* check result */
  if( ErrorCode != grOk)                    /* if error occured */
  {
    closegraph();
    printf(" Graphics System Error: %s\n", grapherrormsg( ErrorCode));
    exit( 1 );
  }
}