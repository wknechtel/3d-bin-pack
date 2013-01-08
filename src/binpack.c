//**********************
// INCLUDED HEADER FILES
//**********************

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <conio.h>

//**********************
// FUNCTION PROTOTYPES
//**********************

void initialize(void);
void inputboxlist(void);
void execiterations(void);
void listcanditlayers(void);
int complayerlist(const void *i, const void *j);
int packlayer(void);
int findlayer( short int thickness);
void findbox(short int hmx, short int hy, short int hmy, short int hz, short int hmz);
void analyzebox (short int hmx, short int hy, short int hmy, short int hz, short int hmz, short int dim1, short int dim2, short int dim3);
void findsmallestz(void);
void checkfound(void);
void volumecheck (void);
void graphunpackedout(void);
void outputboxlist(void);
void report(void);

//********************************************************
// VARIABLE, CONSTANT AND STRUCTURE DECLARATIONS
//********************************************************

char strpx[5], strpy[5], strpz[5];
char strcox[5], strcoy[5], strcoz[5];
char strpackx[5], strpacky[5], strpackz[5];

char filename[12];
char strtemp[]="";
char packing;
char layerdone;
char evened;
char variant;
char bestvariant;
char packingbest;
char hundredpercent;
char graphout[]="visudat";
char unpacked;
char quit;

short int boxx, boxy, boxz, boxi;
short int bboxx, bboxy, bboxz, bboxi;
short int cboxx, cboxy, cboxz, cboxi;
short int bfx, bfy, bfz;
short int bbfx, bbfy, bbfz;
short int xx, yy, zz;
short int px, py, pz;

short int tbn;
short int x;
short int n;
short int layerlistlen;
short int layerinlayer;
short int prelayer;
short int lilz;
short int itenum;
short int hour;
short int min;
short int sec;
short int layersindex;
short int remainpx, remainpy, remainpz;
short int packedy;
short int prepackedy;
short int layerthickness;
short int itelayer;
short int preremainpy;
short int bestite;
short int packednumbox;
short int bestpackednum;

double packedvolume;
double bestvolume;
double totalvolume;
double totalboxvol;
double temp;
double percentageused;
double percentagepackedbox;
double elapsedtime;

struct boxinfo { 
  char packst;
  short int dim1, dim2, dim3, n, cox, coy, coz, packx, packy, packz;
  long int vol; 
} boxlist[5000];

struct layerlist{
  long int layereval;
  short int layerdim; 
} layers[1000];

struct scrappad{
  struct scrappad *pre, *pos;
  short int cumx, cumz;
};

struct scrappad *scrapfirst, *scrapmemb, *smallestz, *trash; 

time_t start, finish;

FILE *ifp, *ofp, *gfp;

//********************************************************
// MAIN PROGRAM
//********************************************************

int main(int argc, char *argv[]) 
{ 
  if (argc == 1)
  {
    printf("(ASSUMED TO HAVE '.TXT' EXTENSION; UP TO 8 CHARACTERS LONG)\n");
    printf ("PLEASE ENTER THE NAME OF THE INPUT FILE :"); 
    scanf ("%s",filename);
  }
  else 
  {
    printf("%s", argv[1]);
    strcpy(filename, argv[1]);
  } 
  
  initialize();
  time(&start);
  printf("\nPRESS Q TO QUIT AT ANYTIME AND WAIT\n\n"); 
  execiterations();
  time(&finish);
  report();
  getch();
  return 0;
}

//********************************************************
// PERFORMS INITIALIZATIONS
//********************************************************

void initialize(void) 
{ 
  if (filename == "")
  {
    printf("\nINVALID FILE NAME\n"); 
    exit(1);
  } 
  inputboxlist();
  temp = 1.0; 
  totalvolume = temp * xx * yy * zz;
  totalboxvol = 0.0;
  for (x=1; x <= tbn; x++) {
    totalboxvol = totalboxvol + boxlist[x].vol; 
  }
  
  scrapfirst = malloc(sizeof(struct scrappad));
  
  if ((*scrapfirst).pos == NULL) 
  {
    printf("Insufficient memory available\n"); 
    exit(1);
  } 
  (*scrapfirst).pre = NULL;
  (*scrapfirst).pos = NULL;
  bestvolume = 0.0; 
  packingbest = 0;
  hundredpercent = 0;
  itenum = 0;
  quit=0;
}
         
         
//**********************************************************************
// READS THE PALLET AND BOX SET DATA ENTERED BY THE USER FROM 
// THE INPUT FILE
//**********************************************************************

void inputboxlist(void)
{
  short int n;
  char lbl[5], dim1[5], dim2[5], dim3[5], boxn[5], strxx[5], stryy[5], strzz[5];
  
  strcpy(strtemp, filename);
  strcat(strtemp, ".txt");
  
  if ( (ifp=fopen(strtemp,"r")) == NULL ) 
  {
    printf("Cannot open file %s", strtemp); 
    exit(1);
  }
  tbn = 1;
  
  if ( fscanf(ifp,"%s %s %s",strxx, stryy, strzz) == EOF )
  {
    exit(1);
  } 
  
  xx = atoi(strxx); 
  yy = atoi(stryy);
  zz = atoi(strzz);
  
  while ( fscanf(ifp,"%s %s %s %s %s",lbl,dim1,dim2,dim3,boxn) != EOF )
  {
    boxlist[tbn].dim1 = atoi(dim1);
    boxlist[tbn].dim2 = atoi(dim2);
    boxlist[tbn].dim3 = atoi(dim3);
    
    boxlist[tbn].vol = boxlist[tbn].dim1 * boxlist[tbn].dim2 * boxlist[tbn].dim3; 
    n = atoi(boxn); 
    boxlist[tbn].n = n;
    
    while (--n)
    {
      boxlist[tbn+n] = boxlist[tbn];
    }
    tbn = tbn+atoi(boxn);
  } 
  --tbn;
  fclose(ifp); 
  return;
}

//**********************************************************************
// ITERATIONS ARE DONE AND PARAMETERS OF THE BEST SOLUTION ARE 
// FOUND
//**********************************************************************

void execiterations(void) 
{
  for (variant = 1; (variant <= 6) && !quit; variant++)
  {
    switch(variant)
    { 
      case 1:
        px=xx; py=yy; pz=zz;
        break;
      case 2:
        px=zz; py=yy; pz=xx;
        break; 
      case 3:
        px=zz; py=xx; pz=yy;
        break; 
      case 4:
        px=yy; py=xx; pz=zz;
        break;
      case 5:
        px=xx; py=zz; pz=yy;
        break; 
      case 6:
        px=yy; py=zz; pz=xx; 
        break;
    }
    
    listcanditlayers();
    layers[0].layereval = -1;
    qsort(layers, layerlistlen+1, sizeof(struct layerlist), complayerlist);
    
    for (layersindex = 1; (layersindex <= layerlistlen) && !quit; layersindex++)
    {
      ++itenum;
      time(&finish);
      elapsedtime = difftime(finish, start);
      printf("VARIANT: %5d; ITERATION (TOTAL): %5d; BEST SO FAR: %.3f %%; TIME: %.0f", variant, itenum, percentageused, elapsedtime); 
      packedvolume = 0.0;
      packedy = 0;
      packing = 1;
      layerthickness = layers[layersindex].layerdim; 
      itelayer = layersindex;
      remainpy = py; 
      remainpz = pz;
      packednumbox = 0;
      for (x = 1; x <= tbn; x++)
      {
        boxlist[x].packst=0;
      }
      
      //BEGIN DO-WHILE
      do
      {
        layerinlayer = 0;
        layerdone = 0;
        if (packlayer())
        {
          exit(1); 
        } 
        packedy = packedy + layerthickness;
        remainpy = py - packedy;
        if (layerinlayer && !quit)
        {
          prepackedy = packedy; 
          preremainpy = remainpy; 
          remainpy = layerthickness - prelayer; 
          packedy = packedy - layerthickness + prelayer;
          remainpz = lilz;
          layerthickness = layerinlayer;
          layerdone = 0;
          if (packlayer())
          {
            exit( 1);
          } 
          packedy = prepackedy; 
          remainpy = preremainpy;
          remainpz = pz;
        } 
        findlayer(remainpy);
      }
      while (packing && !quit);
      // END DO-WHILE
      
      if ((packedvolume > bestvolume) && !quit) 
      { 
        bestvolume = packedvolume;
        bestvariant = variant; 
        bestite = itelayer; 
        bestpackednum = packednumbox;
      }
      
      if (hundredpercent) break;
      percentageused = bestvolume * 100 / totalvolume; 
      printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
      printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    }
    if (hundredpercent) break;
    if ((xx == yy) && (yy == zz)) variant = 6;
  }
}

//**********************************************************************
// LISTS ALL POSSIBLE LAYER HEIGHTS BY GIVING A WEIGHT VALUE TO 
// EACH OF THEM.
//**********************************************************************
void listcanditlayers(void)
{
  char same;
  short int exdim, dimdif, dimen2, dimen3, y, z, k; 
  long int layereval;
  
  layerlistlen = 0;
  
  for (x = 1; x <= tbn; x++)
  {
    for(y = 1; y <= 3; y++)
    { 
      switch(y) 
      {
        case 1:
          exdim = boxlist[x].dim1;
          dimen2 = boxlist[x].dim2;
          dimen3 = boxlist[x].dim3; 
          break;
        case 2:
          exdim = boxlist[x].dim2;
          dimen2 = boxlist[x].dim1;
          dimen3 = boxlist[x].dim3;
          break;
        case 3:
          exdim = boxlist[x].dim3;
          dimen2 = boxlist[x].dim1;
          dimen3 = boxlist[x].dim2;
          break;
      }
      if ((exdim > py) || (((dimen2 > px) || (dimen3 > pz)) && ((dimen3 > px) || (dimen2 > pz)))) continue;
      same=0;
      
      for (k = 1; k <= layerlistlen; k++)
      {
        if (exdim == layers[k].layerdim)
        {
          same = 1;
          continue;
        }
      }
      if (same) continue;
      layereval=0;
      for (z = 1; z <= tbn; z++)
      {
        if(!(x == z))
        {
          dimdif = abs(exdim - boxlist[z].dim1);
          if ( abs(exdim - boxlist[z].dim2) < dimdif )
          {
            dimdif = abs(exdim - boxlist[z].dim2);
          }
          if ( abs(exdim - boxlist[z].dim3) < dimdif )
          {
            dimdif = abs(exdim - boxlist[z].dim3);
          }
          layereval = layereval + dimdif;
        }
      }
      layers[++layerlistlen].layereval = layereval;
      layers[layerlistlen].layerdim = exdim;
    }
  }
  return;
}

//**********************************************************************
// REQUIRED FUNCTION FOR QSORT FUNCTION TO WORK
//**********************************************************************

int complayerlist(const void *i, const void *j)
{
  return *(long int*)i - *(long int*)j;
}

//**********************************************************************
// PACKS THE BOXES FOUND AND ARRANGES ALL VARIABLES AND 
// RECORDS PROPERLY
//**********************************************************************

int packlayer(void){
  short int lenx, lenz, lpz;
  
  if (!layerthickness) 
  { 
    packing=0;
    return 0;
  } 
  
  (*scrapfirst).cumx = px;
  (*scrapfirst).cumz = 0;
  
  for(;!quit;)
  {
    if (kbhit()) {
      if ( toupper(getch()) == 'Q' ) 
      {
        quit = 1;
        printf("\n\nWait please...\n");
      }
    } 
    findsmallestz();
    
    if (!(*smallestz).pre && !(*smallestz).pos)
    {
      //*** SITUATION-1: NO BOXES ON THE RIGHT AND LEFT SIDES ***
      
      lenx = (*smallestz).cumx;
      lpz = remainpz - (*smallestz).cumz;
      findbox(lenx, layerthickness, remainpy, lpz, lpz); 
      checkfound();
      
      if (layerdone) break;
      if (evened) continue; 
      
      boxlist[cboxi].cox = 0;
      boxlist[cboxi].coy = packedy; 
      boxlist[cboxi].coz = (*smallestz).cumz;
      if (cboxx == (*smallestz).cumx)
      {
        (*smallestz).cumz = (*smallestz).cumz + cboxz;
      }
      else 
      {
        (*smallestz).pos = malloc(sizeof(struct scrappad)); 
        if ((*smallestz).pos == NULL) 
        {
          printf("Insufficient memory available\n"); 
          return 1;
        } 
        (*((*smallestz).pos)).pos = NULL;
        (*((*smallestz).pos)).pre = smallestz; 
        (*((*smallestz).pos)).cumx = (*smallestz).cumx; 
        (*((*smallestz).pos)).cumz = (*smallestz).cumz; 
        (*smallestz).cumx = cboxx; 
        (*smallestz).cumz = (*smallestz).cumz + cboxz;
      } 
      volumecheck();
    }
    else if (!(*smallestz).pre) 
    {
      //*** SITUATION-2: NO BOXES ON THE LEFT SIDE ***
      
      lenx = (*smallestz).cumx;
      lenz = (*((*smallestz).pos)).cumz - (*smallestz).cumz; 
      lpz = remainpz - (*smallestz).cumz;
      findbox(lenx, layerthickness, remainpy, lenz, lpz); 
      checkfound();
      
      if (layerdone) break;
      if (evened) continue;
      
      boxlist[cboxi].coy = packedy; 
      boxlist[cboxi].coz = (*smallestz).cumz; 
      if (cboxx == (*smallestz).cumx) 
      {
        boxlist[cboxi].cox = 0;
        if ( (*smallestz).cumz + cboxz == (*((*smallestz).pos)).cumz )
        { 
          (*smallestz).cumz = (*((*smallestz).pos)).cumz; 
          (*smallestz).cumx = (*((*smallestz).pos)).cumx; 
          trash = (*smallestz).pos; 
          (*smallestz).pos = (*((*smallestz).pos)).pos;
          if ((*smallestz).pos) 
          {
            (*((*smallestz).pos)).pre = smallestz;
          }
          free(trash);
        }
        else 
        {
          (*smallestz).cumz = (*smallestz).cumz + cboxz;
        }
      }
      else 
      {
        boxlist[cboxi].cox = (*smallestz).cumx - cboxx; 
        if ( (*smallestz).cumz + cboxz == (*((*smallestz).pos)).cumz )
        {
          (*smallestz).cumx = (*smallestz).cumx - cboxx;
        }
        else 
        {
          (*((*smallestz).pos)).pre = malloc(sizeof(struct scrappad));
          if ((*((*smallestz).pos)).pre == NULL) 
          { 
            printf("Insufficient memory available\n"); 
            return 1;
          } 
          (*((*((*smallestz).pos)).pre)).pos = (*smallestz).pos;
          (*((*((*smallestz).pos)).pre)).pre = smallestz;
          (*smallestz).pos = (*((*smallestz).pos)).pre; 
          (*((*smallestz).pos)).cumx = (*smallestz).cumx;
          (*smallestz).cumx = (*smallestz).cumx - cboxx; 
          (*((*smallestz).pos)).cumz = (*smallestz).cumz + cboxz;
        }
      } 
      volumecheck();
    }
    else if (!(*smallestz).pos)
    {
      //*** SITUATION-3: NO BOXES ON THE RIGHT SIDE ***
      
      lenx = (*smallestz).cumx - (*((*smallestz).pre)).cumx; 
      lenz = (*((*smallestz).pre)).cumz - (*smallestz).cumz; 
      lpz = remainpz - (* smallestz).cumz;
      findbox(lenx, layerthickness, remainpy, lenz, lpz); 
      checkfound();
      
      if (layerdone) break;
      if (evened) continue;
      
      boxlist[cboxi].coy = packedy; 
      boxlist[cboxi].coz = (*smallestz).cumz;
      boxlist[cboxi].cox = (*((*smallestz).pre)).cumx;
      
      if (cboxx == (*smallestz).cumx - (*((*smallestz).pre)).cumx) 
      {
        if ( (*smallestz).cumz + cboxz == (*((*smallestz).pre)).cumz )
        { 
          (*((*smallestz).pre)).cumx = (*smallestz).cumx; 
          (*((*smallestz).pre)).pos = NULL;
          free(smallestz);
        } 
        else 
        {
          (*smallestz).cumz = (*smallestz).cumz + cboxz;
        }
      }
      else 
      {
        if ( (*smallestz).cumz + cboxz == (*((*smallestz).pre)).cumz )
        {
          (*((*smallestz).pre)).cumx = (*((*smallestz).pre)).cumx + cboxx;
        }
        else 
        {
          (*((*smallestz).pre)).pos = malloc(sizeof(struct scrappad));
          if ( (*((*smallestz).pre)).pos == NULL ) 
          {
            printf("Insufficient memory available\n"); 
            return 1;
          }
          (*((*((*smallestz).pre)).pos)).pre = (*smallestz).pre;
          (*((*((*smallestz).pre)).pos)).pos = smallestz; 
          (*smallestz).pre = (*((*smallestz).pre)).pos; 
          (*((*smallestz).pre)).cumx = (*((*((*smallestz).pre)).pre)).cumx + cboxx; 
          (*((*smallestz).pre)).cumz = (*smallestz).cumz + cboxz;
        }
      } 
      volumecheck();
    }
    else if ( (*((*smallestz).pre)).cumz == (*((*smallestz).pos)).cumz ) 
    {
      //*** SITUATION-4: THERE ARE BOXES ON BOTH OF THE SIDES *** 
      
      //*** SUBSITUATION-4A: SIDES ARE EQUAL TO EACH OTHER ***
      
      lenx = (*smallestz).cumx - (*((*smallestz).pre)).cumx; 
      lenz = (*((*smallestz).pre)).cumz - (*smallestz).cumz;
      lpz = remainpz - (*smallestz).cumz;
      
      findbox(lenx, layerthickness, remainpy, lenz, lpz); 
      checkfound();
      
      if (layerdone) break;
      if (evened) continue;
      
      boxlist[cboxi].coy = packedy;
      boxlist[cboxi].coz = (*smallestz).cumz; 
      if ( cboxx == (*smallestz).cumx - (*((*smallestz).pre)).cumx ) 
      {
        boxlist[cboxi].cox = (*((*smallestz).pre)).cumx; 
        if ( (*smallestz).cumz + cboxz == (*((*smallestz).pos)).cumz )
        {
          (*((*smallestz).pre)).cumx = (*((*smallestz).pos)).cumx;
          if ( (*((*smallestz).pos)).pos ) 
          { 
            (*((*smallestz).pre)).pos = (*((*smallestz).pos)).pos; 
            (*((*((*smallestz).pos)).pos)).pre = (*smallestz).pre;
            free(smallestz);
          }
          else 
          {
            (*((*smallestz).pre)).pos = NULL;
            free(smallestz);
          }
        } 
        else
        {
          (*smallestz).cumz = (*smallestz).cumz + cboxz;
        }
      }
      else if ( (*((*smallestz).pre)).cumx < px - (*smallestz).cumx ) 
      {
        if ( (*smallestz).cumz + cboxz == (*((*smallestz).pre)).cumz)
        { 
          (*smallestz).cumx = (*smallestz).cumx - cboxx; 
          boxlist[cboxi].cox = (*smallestz).cumx - cboxx;
        }
        else 
        {
          boxlist[cboxi].cox = (*((*smallestz).pre)).cumx; 
          (*((*smallestz).pre)).pos = malloc(sizeof(struct scrappad)); 
          if ( (*((*smallestz).pre)).pos == NULL ) 
          {
            printf("Insufficient memory available\n"); 
            return 1;
          } 
          (*((*((*smallestz).pre)).pos)).pre = (*smallestz).pre;
          (*((*((*smallestz).pre)).pos)).pos = smallestz;
          (*smallestz).pre = (*((*smallestz).pre)).pos;
          (*((*smallestz).pre)).cumx = (*((*((*smallestz).pre)).pre)).cumx + cboxx;
          (*((*smallestz).pre)).cumz = (*smallestz).cumz + cboxz;
        }
      }
      else 
      {
        if ( (*smallestz).cumz + cboxz == (*((*smallestz).pre)).cumz )
        { 
          (*((*smallestz).pre)).cumx = (*((*smallestz).pre)).cumx + cboxx; 
          boxlist[cboxi].cox = (*((*smallestz).pre)).cumx;
        }
        else 
        {
          boxlist[cboxi].cox = (*smallestz).cumx - cboxx;
          (*((*smallestz).pos)).pre = malloc(sizeof(struct scrappad));
          if ((*((*smallestz).pos)).pre == NULL) 
          {
            printf("Insufficient memory available\n"); 
            return 1;
          }
          (*((*((*smallestz).pos)).pre)).pos = (*smallestz).pos;
          (*((*((*smallestz).pos)).pre)).pre = smallestz;
          (*smallestz).pos = (*((*smallestz).pos)).pre;
          (*((*smallestz).pos)).cumx = (*smallestz).cumx;
          (*((*smallestz).pos)).cumz = (*smallestz).cumz + cboxz; 
          (*smallestz).cumx = (*smallestz).cumx - cboxx;
        }
      } 
      volumecheck();
    }
    else 
    {
      //*** SUBSITUATION-4B: SIDES ARE NOT EQUAL TO EACH OTHER ***
      
      lenx = (*smallestz).cumx - (*((*smallestz).pre)).cumx;
      lenz = (*((*smallestz).pre)).cumz - (*smallestz).cumz;
      lpz = remainpz - (*smallestz).cumz;
      findbox(lenx, layerthickness, remainpy, lenz, lpz);
      checkfound();
      
      if (layerdone) break;
      if (evened) continue;
      
      boxlist[cboxi].coy = packedy;
      boxlist[cboxi].coz = (*smallestz).cumz;
      boxlist[cboxi].cox = (*((*smallestz).pre)).cumx;
      if ( cboxx == (*smallestz).cumx - (*((*smallestz).pre)).cumx )
      {
        if ((*smallestz).cumz + cboxz == (*((*smallestz).pre)).cumz)
        { 
          (*((*smallestz).pre)).cumx = (*smallestz).cumx; 
          (*((*smallestz).pre)).pos = (*smallestz).pos;
          (*((*smallestz).pos)).pre = (*smallestz).pre;
          free(smallestz);
        } 
        else
        {
          (*smallestz).cumz = (*smallestz).cumz + cboxz;
        }
      }
      else 
      {
        if ( (*smallestz).cumz + cboxz == (*((*smallestz).pre)).cumz )
        {
          (*((*smallestz).pre)).cumx = (*((*smallestz).pre)).cumx + cboxx;
        } 
        else if ( (*smallestz).cumz + cboxz == (*((*smallestz).pos)).cumz )
        { 
          boxlist[cboxi].cox = (*smallestz).cumx - cboxx;
          (*smallestz).cumx = (*smallestz).cumx - cboxx;
        }
        else
        {
          (*((*smallestz).pre)).pos = malloc(sizeof(struct scrappad));
          if ( (*((*smallestz).pre)).pos == NULL ) 
          { 
            printf("Insufficient memory available\n");
            return 1;
          }
          (*((*((*smallestz).pre)).pos)).pre = (*smallestz).pre;
          (*((*((*smallestz).pre)).pos)).pos = smallestz;
          (*smallestz).pre = (*((*smallestz).pre)).pos;
          (*((*smallestz).pre)).cumx = (*((*((*smallestz).pre)).pre)).cumx + cboxx;
          (*((*smallestz).pre)).cumz = (*smallestz).cumz + cboxz;
        }
      }
      volumecheck();
    }
  } 
  return 0;
}

//**********************************************************************
// FINDS THE MOST PROPER LAYER HIGHT BY LOOKING AT THE UNPACKED 
// BOXES AND THE REMAINING EMPTY SPACE AVAILABLE
//**********************************************************************

int findlayer(short int thickness) 
{
  short int exdim, dimdif, dimen2, dimen3, y, z; 
  long int layereval, eval;
  layerthickness = 0; 
  eval = 1000000; 
  for (x=1; x <= tbn; x++)
  {
    if (boxlist[x].packst) continue; 
    for( y = 1; y <= 3; y++)
    {
      switch(y) 
      {
        case 1:
          exdim = boxlist[x].dim1;
          dimen2 = boxlist[x].dim2;
          dimen3 = boxlist[x].dim3;
          break;
        case 2:
          exdim = boxlist[x].dim2;
          dimen2 = boxlist[x].dim1;
          dimen3 = boxlist[x].dim3; 
          break;
       case 3:
          exdim = boxlist[x].dim3;
          dimen2 = boxlist[x].dim1;
          dimen3 = boxlist[x].dim2;
          break;
      }
      layereval = 0;
      if ( (exdim <= thickness) && (((dimen2 <= px) && (dimen3 <= pz)) || ((dimen3 <= px) && (dimen2 <= pz))) )
      { 
        for (z = 1; z <= tbn; z++)
        {
          if ( !(x == z) && !(boxlist[z].packst))
          { 
            dimdif = abs(exdim - boxlist[z].dim1);
            if ( abs(exdim - boxlist[z].dim2) < dimdif )
            {
              dimdif = abs(exdim - boxlist[z].dim2);
            }
            if ( abs(exdim - boxlist[z].dim3) < dimdif )
            {
              dimdif = abs(exdim - boxlist[z].dim3);
            }
            layereval = layereval + dimdif;
          }
        }
        if (layereval < eval)
        {
          eval = layereval;
          layerthickness = exdim;
        }
      }
    }
  }
  if (layerthickness == 0 || layerthickness > remainpy) packing = 0; 
  return 0;
}

//**********************************************************************
// FINDS THE MOST PROPER BOXES BY LOOKING AT ALL SIX POSSIBLE 
// ORIENTATIONS, EMPTY SPACE GIVEN, ADJACENT BOXES, AND PALLET LIMITS
//**********************************************************************

void findbox(short int hmx, short int hy, short int hmy, short int hz, short int hmz)
{ 
  short int y;
  bfx = 32767; bfy = 32767; bfz = 32767; 
  bbfx = 32767; bbfy = 32767; bbfz = 32767; 
  boxi = 0; bboxi = 0;
  for (y = 1; y <= tbn; y = y + boxlist[y].n)
  {
    for (x = y; x < x + boxlist[y].n - 1; x++) 
    {
      if (!boxlist[x].packst) break;
    }
    if (boxlist[x].packst) continue;
    if (x > tbn) return;
    analyzebox(hmx, hy, hmy, hz, hmz, boxlist[x].dim1, boxlist[x].dim2, boxlist[x].dim3);
    if ( (boxlist[x].dim1 == boxlist[x].dim3) && (boxlist[x].dim3 == boxlist[x].dim2) ) continue;
    analyzebox(hmx, hy, hmy, hz, hmz, boxlist[x].dim1, boxlist[x].dim3, boxlist[x].dim2);
    analyzebox(hmx, hy, hmy, hz, hmz, boxlist[x].dim2, boxlist[x].dim1, boxlist[x].dim3);
    analyzebox(hmx, hy, hmy, hz, hmz, boxlist[x].dim2, boxlist[x].dim3, boxlist[x].dim1);
    analyzebox(hmx, hy, hmy, hz, hmz, boxlist[x].dim3, boxlist[x].dim1, boxlist[x].dim2);
    analyzebox(hmx, hy, hmy, hz, hmz, boxlist[x].dim3, boxlist[x].dim2, boxlist[x].dim1);
  }
}

//**********************************************************************
// ANALYZES EACH UNPACKED BOX TO FIND THE BEST FITTING ONE TO 
// THE EMPTY SPACE GIVEN
//**********************************************************************

void analyzebox(short int hmx, short int hy, short int hmy, short int hz, short int hmz, short int dim1, short int dim2, short int dim3)
{
  if (dim1 <= hmx && dim2 <= hmy && dim3 <= hmz) 
  {
    if (dim2 <= hy) 
    {
      if (hy - dim2 < bfy) 
      {
        boxx = dim1; 
        boxy = dim2; 
        boxz = dim3;
        bfx = hmx - dim1;
        bfy = hy - dim2;
        bfz = abs(hz - dim3);
        boxi = x;
      }
      else if (hy - dim2 == bfy && hmx - dim1 < bfx) 
      {
        boxx = dim1; 
        boxy = dim2;
        boxz = dim3;
        bfx = hmx - dim1;
        bfy = hy - dim2;
        bfz = abs(hz - dim3);
        boxi = x;
      }
      else if (hy - dim2 == bfy && hmx - dim1 == bfx && abs(hz - dim3) < bfz)
      {
        boxx = dim1;
        boxy = dim2;
        boxz = dim3;
        bfx = hmx - dim1; 
        bfy = hy - dim2; 
        bfz = abs(hz - dim3);
        boxi = x;
      }
    }
    else 
    {
      if (dim2 - hy < bbfy) 
      {
        bboxx = dim1;
        bboxy = dim2;
        bboxz = dim3;
        bbfx = hmx - dim1;
        bbfy = dim2-hy;
        bbfz = abs(hz - dim3);
        bboxi = x;
      }
      else if (dim2 - hy == bbfy && hmx - dim1 < bbfx) 
      {
        bboxx = dim1;
        bboxy = dim2;
        bboxz = dim3;
        bbfx = hmx - dim1;
        bbfy = dim2 - hy;
        bbfz = abs(hz - dim3);
        bboxi = x;
      }
      else if (dim2 - hy == bbfy && hmx-dim1 == bbfx && abs(hz - dim3) < bbfz) 
      { 
        bboxx = dim1;
        bboxy = dim2;
        bboxz = dim3;
        bbfx = hmx - dim1;
        bbfy = dim2 - hy;
        bbfz = abs(hz - dim3);
        bboxi = x;
      }
    }
  }
}
            
//********************************************************
// FINDS THE FIRST TO BE PACKED GAP IN THE LAYER EDGE
//********************************************************
void findsmallestz(void) 
{ 
  scrapmemb = scrapfirst;
  smallestz = scrapmemb;
  while ( !((*scrapmemb).pos == NULL))
  { 
    if ( (*((*scrapmemb).pos)).cumz < (*smallestz).cumz )
    {
      smallestz = (*scrapmemb).pos;
    }
    scrapmemb = (*scrapmemb).pos;
  } 
  return;
} 

//************************************************************
// AFTER FINDING EACH BOX, THE CANDIDATE BOXES AND THE 
// CONDITION OF THE LAYER ARE EXAMINED
//************************************************************

void checkfound(void)
{ 
  evened = 0;
  if (boxi) 
  { 
    cboxi = boxi;
    cboxx = boxx;
    cboxy = boxy;
    cboxz = boxz;
  }
  else 
  {
    if ( (bboxi > 0) && (layerinlayer || (!(*smallestz).pre && !(*smallestz).pos)) )
    { 
      if (!layerinlayer) 
      {
        prelayer = layerthickness;
        lilz = (*smallestz).cumz;
      }
      cboxi = bboxi;
      cboxx = bboxx;
      cboxy = bboxy;
      cboxz =bboxz;
      layerinlayer = layerinlayer + bboxy - layerthickness;
      layerthickness = bboxy;
    }
    else
    {
      if ( !(*smallestz).pre && !(*smallestz).pos )
      {
        layerdone = 1; 
      }
      else 
      {
        evened = 1;
        if (!(*smallestz).pre) 
        {
          trash = (*smallestz).pos; 
          (*smallestz).cumx = (*((*smallestz).pos)).cumx;
          (*smallestz).cumz = (*((*smallestz).pos)).cumz; 
          (*smallestz).pos = (*((*smallestz).pos)).pos;
          if ((*smallestz).pos)
          {
            (*((*smallestz).pos)).pre = smallestz;
          }
          free(trash);
        }
        else if (!(*smallestz).pos)
        {
          (*((*smallestz).pre)).pos = NULL; 
          (*((*smallestz).pre)).cumx = (*smallestz).cumx;
          free(smallestz);
        }
        else 
        {
          if ( (*((*smallestz).pre)).cumz == (*((*smallestz).pos)).cumz ) 
          {
            (*((*smallestz).pre)).pos = (*((*smallestz).pos)).pos;
            if ((*((*smallestz).pos)).pos)
            {
              (*((*((*smallestz).pos)).pos)).pre = (*smallestz).pre;
            } 
            (*((*smallestz).pre)).cumx = (*((*smallestz).pos)).cumx;
            free((*smallestz).pos);
            free(smallestz);
          }
          else
          {
            (*((*smallestz).pre)).pos = (*smallestz).pos; 
            (*((*smallestz).pos)).pre = (*smallestz).pre;
            if ((*((*smallestz).pre)).cumz < (*((*smallestz).pos)).cumz) 
            {
              (*((*smallestz).pre)).cumx = (*smallestz).cumx;
            }
            free(smallestz);
          }
        } 
      }
    }
  }
  return;
}

//********************************************************************
// AFTER PACKING OF EACH BOX, 100% PACKING CONDITION IS CHECKED
//********************************************************************

void volumecheck (void) 
{ 
  boxlist[cboxi].packst = 1;
  boxlist[cboxi].packx = cboxx;
  boxlist[cboxi].packy = cboxy;
  boxlist[cboxi].packz = cboxz;
  packedvolume = packedvolume + boxlist[cboxi].vol;
  packednumbox++;
  if (packingbest)
  { 
    graphunpackedout();
    outputboxlist();
  }
  else if (packedvolume == totalvolume || packedvolume == totalboxvol) 
  {
    packing = 0; 
    hundredpercent = 1;
  } 
  return;
}

//*********************************************************************
// DATA FOR THE VISUALIZATION PROGRAM IS WRITTEN TO THE 
// "VISUDAT" FILE AND THE LIST OF UNPACKED BOXES IS
// MERGED TO THE END OF THE REPORT FILE
//*********************************************************************

void graphunpackedout(void) 
{ 
  char n[5];
  if (!unpacked)
  {
    itoa(boxlist[cboxi].cox, strcox, 10); 
    itoa(boxlist[cboxi].coy, strcoy, 10); 
    itoa(boxlist[cboxi].coz, strcoz, 10);
    itoa(boxlist[cboxi].packx, strpackx, 10);
    itoa(boxlist[cboxi].packy, strpacky, 10);
    itoa(boxlist[cboxi].packz, strpackz, 10);
  }
  else 
  {
    itoa(cboxi, n, 10); 
    itoa(boxlist[cboxi].dim1, strpackx, 10); 
    itoa(boxlist[cboxi].dim2, strpacky, 10);
    itoa(boxlist[cboxi].dim3, strpackz, 10);
  }
  if (!unpacked) 
  {
    fprintf(gfp, "%5s%5s%5s%5s%5s%5s\n", strcox, strcoy, strcoz, strpackx, strpacky, strpackz);
  }
  else
  {
    fprintf(ofp,"%5s%5s%5s%5s\n", n, strpackx, strpacky, strpackz);
  }
}

//*********************************************************************
// TRANSFORMS THE FOUND COORDINATE SYSTEM TO THE ONE ENTERED 
// BY THE USER AND WRITES THEM TO THE REPORT FILE
//*********************************************************************

void outputboxlist(void)
{
  char strx[5];
  char strpackst[5];
  char strdim1[5], strdim2[5], strdim3[5];
  char strcox[5], strcoy[5], strcoz[5];
  char strpackx[5], strpacky[5], strpackz[5];
  
  short int x, y, z, bx, by, bz;
  
  switch(bestvariant) 
  { 
    case 1:
      x = boxlist[cboxi].cox; 
      y = boxlist[cboxi].coy; 
      z = boxlist[cboxi].coz; 
      bx = boxlist[cboxi].packx; 
      by = boxlist[cboxi].packy; 
      bz = boxlist[cboxi].packz;
      break;
    case 2:
      x = boxlist[cboxi].coz;
      y = boxlist[cboxi].coy;
      z = boxlist[cboxi].cox;
      bx = boxlist[cboxi].packz;
      by = boxlist[cboxi].packy;
      bz = boxlist[cboxi].packx;
      break;
    case 3:
      x = boxlist[cboxi].coy;
      y = boxlist[cboxi].coz;
      z = boxlist[cboxi].cox;
      bx = boxlist[cboxi].packy;
      by = boxlist[cboxi].packz;
      bz = boxlist[cboxi].packx;
      break;
    case 4:
      x = boxlist[cboxi].coy;
      y = boxlist[cboxi].cox;
      z = boxlist[cboxi].coz;
      bx = boxlist[cboxi].packy;
      by = boxlist[cboxi].packx;
      bz = boxlist[cboxi].packz;
      break;
    case 5:
      x = boxlist[cboxi].cox;
      y = boxlist[cboxi].coz;
      z = boxlist[cboxi].coy;
      bx = boxlist[cboxi].packx;
      by = boxlist[cboxi].packz;
      bz = boxlist[cboxi].packy;
      break;
    case 6:
      x = boxlist[cboxi].coz;
      y = boxlist[cboxi].cox;
      z = boxlist[cboxi].coy;
      bx = boxlist[cboxi].packz;
      by = boxlist[cboxi].packx;
      bz = boxlist[cboxi].packy;
      break;
  }
  
  itoa(cboxi, strx,10);
  itoa(boxlist[cboxi].packst, strpackst, 10);
  itoa(boxlist[cboxi].dim1, strdim1, 10);
  itoa(boxlist[cboxi].dim2, strdim2, 10);
  itoa(boxlist[cboxi].dim3, strdim3,10);
  itoa(x, strcox, 10);
  itoa(y, strcoy, 10);
  itoa(z, strcoz, 10);
  itoa(bx, strpackx, 10);
  itoa(by, strpacky, 10);
  itoa(bz, strpackz, 10);
  
  boxlist[cboxi].cox = x;
  boxlist[cboxi].coy = y;
  boxlist[cboxi].coz = z;
  boxlist[cboxi].packx = bx;
  boxlist[cboxi].packy = by;
  boxlist[cboxi].packz = bz;
  fprintf(ofp, "%5s%5s%9s%9s%9s%9s%9s%9s%9s%9s%9s\n", strx, strpackst, strdim1, strdim2, strdim3, strcox, strcoy, strcoz, strpackx, strpacky, strpackz);
  return;
}
  
  
//*******************************************************************
// USING THE PARAMETERS FOUND, PACKS THE BEST SOLUTION FOUND 
// AND REPORS TO THE CONSOLE AND TO A TEXT FILE
//*******************************************************************

void report(void)
{ 
  quit = 0;
  switch(bestvariant) 
  { 
    case 1:
      px = xx; py = yy; pz = zz;
      break; 
    case 2:
      px = zz; py = yy; pz = xx;
      break; 
    case 3:
      px = zz; py = xx; pz = yy;
      break; 
    case 4:
      px=yy; py=xx; pz = zz;
      break; 
    case 5:
      px = xx; py = zz; pz = yy;
      break; 
    case 6:
      px = yy; py = zz; pz = xx; 
      break;
  }
  packingbest = 1;
  if ( (gfp = fopen(graphout,"w")) == NULL ) 
  {
    printf("Cannot open file %s", filename);
    exit(1);
  }
  
  itoa(px, strpx, 10);
  itoa(py, strpy, 10);
  itoa(pz, strpz, 10);
  
  fprintf(gfp,"%5s%5s%5s\n", strpx, strpy, strpz); 
  strcat(filename, ".out");
  
  if ( (ofp = fopen(filename,"w")) == NULL ) 
  { 
    printf("Cannot open file %s", filename);
    exit(1);
  }
  
  percentagepackedbox = bestvolume * 100 / totalboxvol;
  percentageused = bestvolume * 100 / totalvolume;
  elapsedtime = difftime( finish, start);
  
  fprintf(ofp,"---------------------------------------------------------------------------------------------\n");
  fprintf(ofp,"                                       *** REPORT ***\n");
  fprintf(ofp,"---------------------------------------------------------------------------------------------\n");
  fprintf(ofp,"ELAPSED TIME                                          : Almost %.0f sec\n", elapsedtime);
  fprintf(ofp,"TOTAL NUMBER OF ITERATIONS DONE                       : %d\n", itenum);
  fprintf(ofp,"BEST SOLUTION FOUND AT ITERATION                      : %d OF VARIANT: %d\n", bestite, bestvariant);
  fprintf(ofp,"TOTAL NUMBER OF BOXES                                 : %d\n", tbn);
  fprintf(ofp,"PACKED NUMBER OF BOXES                                : %d\n", bestpackednum);
  fprintf(ofp,"TOTAL VOLUME OF ALL BOXES                             : %.0f\n", totalboxvol);
  fprintf(ofp,"PALLET VOLUME                                         : %.0f\n", totalvolume);
  fprintf(ofp,"BEST SOLUTION'S VOLUME UTILIZATION                    : %.0f OUT OF %.0f\n", bestvolume, totalvolume);
  fprintf(ofp,"PERCENTAGE OF PALLET VOLUME USED                      : %.6f %%\n", percentageused);
  fprintf(ofp,"PERCENTAGE OF PACKED BOXES (VOLUME)                   : %.6f %%\n", percentagepackedbox);
  fprintf(ofp,"WHILE PALLET ORIENTATION                              : X=%d; Y=%d; Z= %d\n", px, py, pz);
  fprintf(ofp,"---------------------------------------------------------------------------------------------\n");
  fprintf(ofp,"  NO: PACKSTA DIMEN-1  DMEN-2  DIMEN-3   COOR-X   COOR-Y   COOR-Z   PACKEDX  PACKEDY  PACKEDZ\n");
  fprintf(ofp,"---------------------------------------------------------------------------------------------\n");
                                                                                                                                               
  listcanditlayers();
  layers[0].layereval= -1;
  qsort(layers, layerlistlen + 1, sizeof(struct layerlist), complayerlist);
  packedvolume = 0.0;
  packedy = 0;
  packing = 1;
  layerthickness = layers[bestite].layerdim;
  remainpy = py;
  remainpz = pz;
  
  for (x = 1; x <= tbn; x++)
  {
    boxlist[x].packst = 0;
  }
  
  do
  {
    layerinlayer = 0;
    layerdone = 0;
    packlayer();
    packedy = packedy + layerthickness;
    remainpy = py - packedy;
    if (layerinlayer)
    {
      prepackedy = packedy;
      preremainpy = remainpy;
      remainpy = layerthickness - prelayer;
      packedy = packedy - layerthickness + prelayer;
      remainpz = lilz;
      layerthickness = layerinlayer;
      layerdone = 0;
      packlayer();
      packedy = prepackedy;
      remainpy = preremainpy;
      remainpz = pz;
    }
    if (!quit)
    {
      findlayer(remainpy);
    }
  }
  while (packing && !quit);
  
  fprintf(ofp,"\n\n *** LIST OF UNPACKED BOXES ***\n");
  unpacked = 1;
  for (cboxi = 1; cboxi <= tbn; cboxi++)
  {
    if (!boxlist[cboxi].packst)
    {
      graphunpackedout();
    }
  }
  unpacked = 0;
  fclose(ofp);
  fclose(gfp);
  printf("\n");
  for (n = 1; n <= tbn; n++)
  {
    if (boxlist[n].packst)
    {
      printf("%d %d %d %d %d %d %d %d %d %d\n", n, boxlist[n].dim1, boxlist[n].dim2, boxlist[n].dim3, boxlist[n].cox, boxlist[n].coy, boxlist[n].coz, boxlist[n].packx, boxlist[n].packy, boxlist[n].packz);
    }
  }
  printf("ELAPSED TIME                       : Almost %.0f sec\n", elapsedtime);
  printf("TOTAL NUMBER OF ITERATIONS DONE    : %d\n", itenum);
  printf("BEST SOLUTION FOUND AT             : ITERATION: %d OF VARIANT: %d\n", bestite, bestvariant);
  printf("TOTAL NUMBER OF BOXES              : %d\n", tbn); 
  printf("PACKED NUMBER OF BOXES             : %d\n", bestpackednum);
  printf("TOTAL VOLUME OF ALL BOXES          : %.0f\n", totalboxvol);
  printf("PALLET VOLUME                      :%.0f\n",totalvolume); 
  printf("BEST SOLUTION'S VOLUME UTILIZATION :%.0f OUT OF %.0f\n", bestvolume, totalvolume);
  printf("PERCENTAGE OF PALLET VOLUME USED   : %.6f %%\n", percentageused);
  printf("PERCENTAGE OF PACKEDBOXES (VOLUME) :%.6f%%\n", percentagepackedbox);
  printf("WHILE PALLET ORIENTATION           : X=%d; Y=%d; Z= %d\n\n\n", px, py, pz); 
  printf("TO VISUALIZE THIS SOLUTION, PLEASE RUN 'VISUAL.EXE'\n");
}