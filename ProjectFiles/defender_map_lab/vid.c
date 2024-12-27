/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

// vid.c file: implement fbuf for the ARM PL110 LCD display
/**************** Reference: ARM PL110 and DUI02241 ********************
Color LCD base address: 0x10120000 - 0x1012FFFF
00    timing0
04    timing1
08    timing2
0C    timing3
10    upperPanelframeBaseAddressRegister // use only ONE panel
14    lowerPanelFrameBaseAddressRegister // some display allows 2 panels
18    interruptMaskClrregister
1C    controlRegister
20    interruptStatusReg
etc
************************************************************************/
#include "font0"

u8 cursor;
int fb1[1*1024*1024];
int fb2[1*1024*1024];
//int fb1[640*480];
//int fb2[640*480];
int volatile *fb;
unsigned char *font;
int row, col;
int frame_buffer;
#define PL110_CR_EN   0x001
#define PL110_CR_BGR  0x100
#define PL110_CR_BEBO 0x200
#define PL110_CR_BEPO 0x400
#define PL110_CR_PWR  0x800
#define PL110_IE_NB   0x004
#define PL110_IE_VC   0x008
int fbuf_init()
{
  int x; int i;
  /**** for SVGA 800X600 these values are in ARM DUI02241 *********
  *(volatile unsigned int *)(0x1000001c) = 0x2CAC; // 800x600
  *(volatile unsigned int *)(0x10120000) = 0x1313A4C4;
  *(volatile unsigned int *)(0x10120004) = 0x0505F6F7;
  *(volatile unsigned int *)(0x10120008) = 0x071F1800;
  *(volatile unsigned int *)(0x10120010) = (1*1024*1024);
  *(volatile unsigned int *)(0x10120018) = 0x82B;
  ***************************************************************/
fb = fb1;
  /********* for VGA 640x480 ************************/
  *(volatile unsigned int *)(0x1000001c) = 0x2C77;        // LCDCLK SYS_OSCCLK
  *(volatile unsigned int *)(0x10120000) = 0x3F1F3F9C;    // time0
  *(volatile unsigned int *)(0x10120004) = 0x090B61DF;    // time1
  *(volatile unsigned int *)(0x10120008) = 0x067F1800;    // time2
  *(volatile unsigned int *)(0x10120010) = fb;//(1*1024*1024); // panelBaseAddress
  *(volatile unsigned int *)(0x10120018) = 0x82B|PL110_CR_EN|PL110_CR_PWR;         // control register
  
  /****** at 0x200-0x3FC are LCDpalletes of 128 words ***************
  unsigned int *inten = (unsigned int *)(0x10120200);
  for (i=0; i<128; i++){
       inten[i]=0x8F008F00;
  }
  ******** yet to figure out HOW TO use these palletes *************/
//fb = (int *)(1*1024*1024);

  font = fonts0;              // use fonts0 for char bit patterns 

  // for (x = 0; x < (800 * 600); ++x) // for one BAND
  /******** these will show 3 bands of BLUE, GREEN, RED ********* 
  for (x = 0; x < (212*480); ++x)
  fb[x] = 0x00FF0000;  //00BBGGRR
  for (x = 212*480+1; x < (424*480); ++x)
  fb[x] = 0x0000FF00;  //00BBGGRR
  for (x = 424*480+1; x < (640*480); ++x)
  fb[x] = 0x000000FF;  //00BBGGRR
  ************* used only during intial testing ****************/

  // for 640x480 VGA mode display
  for (x=0; x<640*480; x++)
    fb[x] = 0x00000000;    // clean screen; all pixels are BLACK
  cursor = 127; // cursor bit map in font0 at 127
*(volatile unsigned int *)(0x1012001C) |= 0xc; // enable interrupts
//*(volatile unsigned int *)(0x10120024) = 0xc; // enable interrupts
}

int clrpix(int x, int y)
{
  int pix = y*640 + x;
  fb[pix] = 0x00000000;
}

int setpix(int x, int y)
{
  int pix = y*640 + x;
  if (color==RED)
     fb[pix] = 0x000000FF;
  if (color==BLUE)
     fb[pix] = 0x00FF0000;
  if (color==GREEN)
     fb[pix] = 0x0000FF00;
  if (color==WHITE)
     fb[pix] = 0x00FFFFFF;
  if (color==YELLOW)
     fb[pix] = 0x0000FFFF;
  if (color==CYAN)
    fb[pix] = 0x00FFFF00;
}

int dchar(unsigned char c, int x, int y)
{
  int r, bit;
  unsigned char *caddress, byte;

  caddress = font + c*16;
  //  printf("c=%x %c caddr=%x\n", c, c, caddress);

  for (r=0; r<16; r++){
    byte = *(caddress + r);

    for (bit=0; bit<8; bit++){
      if (byte & (1<<bit))
	  setpix(x+bit, y+r);
    }
  }
}

int undchar(unsigned char c, int x, int y)
{
  int row, bit;
  unsigned char *caddress, byte;

  caddress = font + c*16;
  //  printf("c=%x %c caddr=%x\n", c, c, caddress);

  for (row=0; row<16; row++){
    byte = *(caddress + row);

    for (bit=0; bit<8; bit++){
      if (byte & (1<<bit))
	  clrpix(x+bit, y+row);
    }
  }
}

int dstring(char *s, int x, int y)
{
  while(*s){
    dchar(*s, x, y);
    x+=8;
    s++;
  }
}

int scroll()
{
  int i;
  for (i=0; i<640*480-640*16; i++){
    fb[i] = fb[i+ 640*16];
  } 
}
  
int kpchar(char c, int ro, int co)
{
   int x, y;
   x = co*8;
   y = ro*16;
   //printf("c=%x [%d%d] (%d%d)\n", c, ro,co,x,y);
   dchar(c, x, y);
   
}

int unkpchar(char c, int ro, int co)
{
   int x, y;
   x = co*8;
   y = ro*16;
   //printf("c=%x [%d%d] (%d%d)\n", c, ro,co,x,y);
   undchar(c, x, y);
}

int erasechar()
{ 
  // erase char at (row,col)
  int r, bit, x, y;
  unsigned char *caddress, byte;

  x = col*8;
  y = row*16;
 
  //printf("ERASE: row=%d col=%d x=%d y=%d\n",row,col,x,y);

  for (r=0; r<16; r++){
     for (bit=0; bit<8; bit++){
        clrpix(x+bit, y+r);
    }
  }
} 

int clrcursor()
{
  unkpchar(cursor, row, col);
}

int putcursor(unsigned char c)
{
  kpchar(c, row, col);
}

int kputc(char c)
{
  clrcursor();
  if (c=='\r'){
    col=0;
    //printf("row=%d col=%d\n", row, col);
    putcursor(cursor);
    return;
  }
  if (c=='\n'){
    row++;
    if (row>=25){
      row = 24;
      scroll();
    }
    //printf("row=%d col=%d\n", row, col);
    putcursor(cursor);
    return;
  }
  if (c=='\b'){
    if (col>0){
      col--;
      erasechar();
      putcursor(cursor);
    }
    return;
  }
  // c is ordinary char
  kpchar(c, row, col);
  col++;
  if (col>=80){
    col = 0;
    row++;
    if (row >= 25){
      row = 24;
      scroll();
    }
  }
  putcursor(cursor); 
  //printf("row=%d col=%d\n", row, col);
}

int kprints(char *s)
{
  while(*s){
    kputc(*s);
    s++;
  }
}

int krpx(int x)
{
  char c;
  if (x){
     c = tab[x % 16];
     krpx(x / 16);
  }
  kputc(c);
}

int kprintx(int x)
{
  kputc('0'); kputc('x');
  if (x==0)
    kputc('0');
  else
    krpx(x);
  kputc(' ');
}

int krpu(int x)
{
  char c;
  if (x){
     c = tab[x % 10];
     krpu(x / 10);
  }
  kputc(c);
}

int kprintu(int x)
{
  if (x==0){
    kputc(' ');kputc('0');
  }
  else
    krpu(x);
  kputc(' ');
}

int kprinti(int x)
{
  if (x<0){
    kputc('-');
    x = -x;
  }
  kprintu(x);
}

int kprintf(char *fmt,...)
{
  int *ip;
  char *cp;
  cp = fmt;
  ip = (int *)&fmt + 1;

  while(*cp){
    if (*cp != '%'){
      kputc(*cp);
      if (*cp=='\n')
	kputc('\r');
      cp++;
      continue;
    }
    cp++;
    switch(*cp){
    case 'c': kputc((char)*ip);      break;
    case 's': kprints((char *)*ip);  break;
    case 'd': kprinti(*ip);          break;
    case 'u': kprintu(*ip);          break;
    case 'x': kprintx(*ip);          break;
    }
    cp++; ip++;
  }
}


int get_height_width(char *p,int *h,int*w)
{ 
  
   int *q = (int *)(p+14); // skip over 14 bytes file header 
   q++;                    // skip 4 bytes in image header
   *w = *q;                 // width in pixels 
   *h = *(q + 1);           // height in pixels
}

int show_bmp(char *p, int startRow, int startCol)
{ 
  int h, w, pixel, r1, r2, i, j; 
   unsigned char r, g, b;
   char *pp;
 
   int *q = (int *)(p+14); // skip over 14 bytes file header 
   q++;                    // skip 4 bytes in image header
   w = *q;                 // width in pixels  was w
   h = *(q + 1);           // height in pixels

   p += 54;                // p point at pixels now 

   // but the picture is up-side DOWN

   r1 = 3*w;
   r2 = 4*((3*w+3)/4);     // row size is a multiple of 4 bytes  

   //p = (char *)(pp + (h-1)*r2);     // 3 bytes per pixel
   // p += (h-1)*r2;
   // p += h*r2;
   p += (h-1)*r2;
   for (i=startRow; i<h+startRow; i++){
     pp = p;
     // for (j=startCol; j<w+startCol; j++){
     for (j=startCol; j<startCol+w; j++){
         b = *pp; g = *(pp+1); r = *(pp+2);
         pixel = (b<<16) + (g<<8) + r;
         if (b==0xff && g==0xff && r == 0xff)
            pixel =0;
         fb[i*640 + j] = pixel;
         pp += 3;    // back pp by 3 bytes
     }
     p -= r2;
   }
  // uprintf("h=%d w=%d r1=%d r2=%d\n", h, w, r1, r2);
   /*********
   row = 20;
   kprintf("h=%d w=%d r1=%d r2=%d\n", h, w, r1, r2);
   ******/
  // uprintf("\nBMP image height=%d width=%d\n", h, w);

}
int fbmodified;
switch_buffer(){
   if (fb == fb1)
      fb = fb2;
   else
      fb = fb1;
}
int vidhandler=1;
void vid_handler() 
{
vidhandler=1;
 //uprintf("\nvid handler=%x \n",*(volatile unsigned int *)(0x10120020) );
 *(volatile unsigned int *)(0x10120028) = 0; // clear interrupts
*(volatile unsigned int *)(0x1012001C) =0;
 *(volatile unsigned int *)(0x10120010) = fb;
if (fbmodified)
	switch_buffer();
fbmodified =0;
  //  mask = *(up->base + MASK);  // read MASK register
 // if (frame_buffer){
   //  *(volatile unsigned int *)(0x10120010) = (1*1024*1024);
  //fb = (int *)(2*1024*1024);  // at 1MB area; enough for 800x600 SVGA
    //  frame_buffer = 0;
    // }
  //else{
//*(volatile unsigned int *)(0x10120010) = (2*1024*1024);
 // fb = (int *)(1*1024*1024);  // at 2MB area; enough for 800x600 SVGA
  //   frame_buffer = 1;
//}
 
}
#pragma pack(push, 1)  // Ensure no padding is added in structs
typedef struct {
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    unsigned int biSize;
    int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
} BITMAPINFOHEADER;
#pragma pack(pop)

int
get_image_size(int * img_width,int *img_height,char * p)
{ 
int y,x,i,j;
BITMAPFILEHEADER *fileHeader;
    BITMAPINFOHEADER *infoHeader;

    // Read the BMP file header and info header
    
   fileHeader = p;
   infoHeader = p+sizeof(BITMAPFILEHEADER);

    *img_width = infoHeader->biWidth;
    *img_height = infoHeader->biHeight;
}

int
put_tile(int startRow,int startCol,int tileNo,char * p)
{ 
int y,x,i,j;
BITMAPFILEHEADER *fileHeader;
    BITMAPINFOHEADER *infoHeader;

    // Read the BMP file header and info header
    
   fileHeader = p;
   infoHeader = p+sizeof(BITMAPFILEHEADER);

    int img_width = infoHeader->biWidth;
    int img_height = infoHeader->biHeight;
    int bytes_per_pixel = infoHeader->biBitCount / 8;
    unsigned char *image_data = p+fileHeader->bfOffBits;
    int m = tileNo%13;
    int ro = tileNo/13;
    int tileNo1 = (6-ro)*13+m;
    tileNo=tileNo1;
    int tile_height = 16;
    int tile_width = 16;
    int gap =1;
 
 int tile_num = 0;
 int x_start,pixel;
  int y_start;
  int fin=0;
   unsigned char r, g, b;
   m = tileNo%13;
   ro = tileNo/13;
   y_start = ro*(tile_height + gap);
   x_start = m*(tile_width + gap); 
  
     int copied=0;
    // Write pixel data for the 16x16 tile
    for (j=startRow+15,y = 0; y < tile_height; y++,j--) {
        // Calculate the source position in the original image
        int src_y = y_start + y;
        if (src_y >= img_height) continue;  // Out of bounds

        for (i=startCol,x = 0; x < tile_width; x++,i++) {
            int src_x = x_start + x;
            if (src_x >= img_width) continue;  // Out of bounds

            // Calculate the pixel index in the original image
            unsigned char *pp = image_data + (src_y * img_width + src_x) * bytes_per_pixel;

            // Write the pixel to the output file
            
            b = *pp; g = *(pp+1); r = *(pp+2);
            copied=1;
            pixel = (b<<16) + (g<<8) + r;
            if (b==0xff && g==0xff && r == 0xff)
               pixel =0;
            fb[j*640+i]=pixel;
        }
    }
   // uprintf("ro=%d m=%d offset = %d tile_num %d tile %d y_start %d y_start %d copied =%d\n",
     //               ro,m,bytes_per_pixel,tile_num,tileNo1,y_start,x_start,copied);   
 }
 
 
 
