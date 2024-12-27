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

#include "defines.h"

#include "string.c"

#include "pacman.h"

int spriteMove = 0;
char * tab = "0123456789ABCDEF";
int color = RED;

struct Pacman pacmans[40];

#include "timer.c"

#include "interrupts.c"

#include "kbd.c"

#include "uart.c"

#include "vid.c"

#include "ObjectLayer.c"

extern char _binary_pokey_bmp_start;
extern char _binary_shadow_bmp_start;
extern char _binary_ship_left_bmp_start;
extern char _binary_ship_right_bmp_start;
extern char _binary_shoot_bmp_start;
extern char _binary_tilemap_bmp_start;

void copy_vectors(void) {
  extern u32 vectors_start;
  extern u32 vectors_end;
  u32 * vectors_src = & vectors_start;
  u32 * vectors_dst = (u32 * ) 0;

  while (vectors_src < & vectors_end)
    *vectors_dst++ = * vectors_src++;
}
int kprintf(char *fmt,...);
void timer_handler();
void vid_handler();

void uart0_handler() {
  uart_handler( & uart[0]);
}
void uart1_handler() {
  uart_handler( & uart[1]);
}

// IRQ interrupts handler entry point
//void __attribute__((interrupt)) IRQ_handler()
// timer0 base=0x101E2000; timer1 base=0x101E2020
// timer3 base=0x101E3000; timer1 base=0x101E3020
// currentValueReg=0x04
TIMER * tp[4];

void IRQ_handler() {
  int vicstatus, sicstatus;
  int ustatus, kstatus;

  // read VIC SIV status registers to find out which interrupt
  vicstatus = VIC_STATUS;
  sicstatus = SIC_STATUS;
  //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);
  // VIC status BITs: timer0,1=4, uart0=13, uart1=14, SIC=31: KBD at 3
  /**************
  if (vicstatus & 0x0010){   // timer0,1=bit4
    if (*(tp[0]->base+TVALUE)==0) // timer 0
       timer_handler(0);
    if (*(tp[1]->base+TVALUE)==0)
       timer_handler(1);
  }
  if (vicstatus & 0x0020){   // timer2,3=bit5
     if(*(tp[2]->base+TVALUE)==0)
       timer_handler(2);
     if (*(tp[3]->base+TVALUE)==0)
       timer_handler(3);
  }
  if (vicstatus & 0x80000000){
     if (sicstatus & 0x08){
        kbd_handler();
     }
  }
  *********************/
  /******************
  if (vicstatus & (1<<4)){   // timer0,1=bit4
    if (*(tp[0]->base+TVALUE)==0) // timer 0
       timer_handler(0);
    if (*(tp[1]->base+TVALUE)==0)
       timer_handler(1);
  }
  if (vicstatus & (1<<5)){   // timer2,3=bit5
     if(*(tp[2]->base+TVALUE)==0)
       timer_handler(2);
     if (*(tp[3]->base+TVALUE)==0)
       timer_handler(3);
  }
  *********************/
  if (vicstatus & (1 << 4)) { // timer0,1=bit4
    timer_handler(0);
  }
  if (vicstatus & (1 << 12)) { // bit 12: uart0
    uart0_handler();
  }
  if (vicstatus & (1 << 13)) { // bit 13: uart1
    uart1_handler();
  }
  if (vicstatus & (1 << 16)) { // bit 16: video
    vid_handler();
    VIC_CLEAR &= ~(1 << 16);
  }
  if (vicstatus & (1 << 31)) {
    if (sicstatus & (1 << 3)) {
      //   kbd_handler();
    }
  }
  VIC_CLEAR = 0;
}
extern int oldstartR;
extern int oldstartC;
extern int replacePix;
extern int buff[16][16];
struct sprite sprites[4];
#define Width 80
#define Height 60
int fire;
struct Triangle {
  int x1;
  int y1;
  int x2;
  int y2;
  int x3;
  int y3;
};
struct Triangle Test = {
  200,
  200,
  250,
  250,
  280,
  200
};
#define sgn(x)((x < 0) ? -1 : ((x > 0) ? 1 : 0))
int abs(int a) {
  return a < 0 ? -a : a;
}
void line_fast(int x1, int y1, int x2, int y2) {
  int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py;

  dx = x2 - x1; /* the horizontal distance of the line */
  dy = y2 - y1; /* the vertical distance of the line */
  dxabs = abs(dx);
  dyabs = abs(dy);
  sdx = sgn(dx);
  sdy = sgn(dy);
  x = dyabs >> 1;
  y = dxabs >> 1;
  px = x1;
  py = y1;

  //VGA[(py<<8)+(py<<6)+px]=color;

  if (dxabs >= dyabs) /* the line is more horizontal than vertical */ {
    for (i = 0; i < dxabs; i++) {
      y += dyabs;
      if (y >= dxabs) {
        y -= dxabs;
        py += sdy;
      }
      px += sdx;
      setpix(px, py);
    }
  } else /* the line is more vertical than horizontal */ {
    for (i = 0; i < dyabs; i++) {
      x += dxabs;
      if (x >= dyabs) {
        x -= dyabs;
        px += sdx;
      }
      py += sdy;
      setpix(px, py);
    }
  }
}
checkObstacle(Object * o, int x, int y, int h, int w) {
  //uprintf("obj=%d ox %d oy =%d ow=%d oh=%d\n",Collisions-o,o->x,o->y,o->width,o->height);
  if (y + h >= o -> y && y <= o -> y + o -> height && x + w >= o -> x && x <= o -> x + o -> width)
    return 1;
  return 0;
}

checkPacman(Object * o, int x, int y, int h, int w) {
  //uprintf("x =%d y=%d h=%d w = %d obj=%d ox %d oy =%d ow=%d oh=%d\n",x,y,h,w,pacman-o,o->x,o->y,o->width,o->height);
  if (y + h >= o -> y && y <= o -> y + o -> height && x + w >= o -> x && x <= o -> x + o -> width)
    o -> isalive = 2;
  return 1;
  return 0;
}
int lastx = 0;

int xpos = 80;
int mm = 0;
Draw_all() {
  char * p;
  int i = 0, j = 0;
  for (int x = 0; x < 640 * 480; x++)
    fb[x] = 0x00000000; // clean screen; all pixels are BLACK
  TIMER * t = & timer[0];

  /* TODO */
  /* take out the timer value and put in the score e.g. how many pacmans have been saved. */

  for (i = 0; i < 8; i++) {
    kpchar(t -> clock[i], 1, 70 + i);
  }
  int lx = 0;

  /* TODO
  loop 1*/
  for (j = 0; j < 18; j++) {
    for (i = xpos; i < xpos + 40 && i < 120; i++) {

      int tile = Tiles[j * 128 + i];

      if (tile && (i - xpos) * 16 < 624)

        put_tile(j * 16, (i - xpos) * 16, tile, & _binary_tilemap_bmp_start);
    }
  }
  /* loop 2 */
  // draw all pacman entities
  for (j = 0; j < 18; j++) {
    for (i = xpos; i < xpos + 40 && i < 120; i++) {

      int tile = pacman_tiles[j * 128 + i];

      if (tile && (i - xpos) * 16 < 624)

        put_tile(j * 16, (i - xpos) * 16, tile, & _binary_tilemap_bmp_start);
    }
  }

  for (i = 1; i < 3; i++) {
    if (i == 2) {
      int x = sprites[i].x - xpos * 16;
      // uprintf("draw x =%d y = %d\n",x,sprites[i].y);
      if (x > 0 && x < 640)
        show_bmp(sprites[i].p, sprites[i].y, x);
    } else
      show_bmp(sprites[i].p, sprites[i].y, sprites[i].x);

    if (i == 3)
      sprites[i].x += 10;

  }

  fbmodified = 1;
  *(volatile unsigned int * )(0x1012001C) |= 0xc; // enable video interrupts
}

int counter;
int main() {
  int i, j;
  char line[128], string[32];
  UART * up;

  int x = 80;
  int y = 0;

  sprites[0].x = 80;
  sprites[0].y = 0;
  sprites[0].p = & _binary_shoot_bmp_start;
  sprites[1].x = 80;
  sprites[1].y = 0;
  sprites[1].p = & _binary_ship_right_bmp_start;
  sprites[1].direction = 1;
  sprites[2].x = 1280;
  sprites[2].y = 0;
  sprites[2].p = & _binary_pokey_bmp_start;
  sprites[3].x = 200;
  sprites[3].y = 100;
  sprites[3].p = & _binary_shadow_bmp_start;
  get_image_size( & sprites[1].w, & sprites[1].h, sprites[1].p);
  char * p;
  color = YELLOW;
  row = col = 0;

  /* enable timer0,1, uart0,1 SIC interrupts */
  VIC_INTENABLE |= (1 << 4); // timer0,1 at bit4
  // VIC_INTENABLE |= (1<<5);  // timer2,3 at bit5

  VIC_INTENABLE |= (1 << 12); // UART0 at 12
  // VIC_INTENABLE |= (1<<13); // UART1 at 13

  VIC_INTENABLE |= (1 << 16); // LCD at 16

  UART0_IMSC = 1 << 4; // enable UART0 RXIM interrupt
  UART1_IMSC = 1 << 4; // enable UART1 RXIM interrupt

  //VIC_INTENABLE |= 1<<31;   // SIC to VIC's IRQ31

  /* enable KBD IRQ */
  SIC_ENSET = 1 << 3; // KBD int=3 on SIC
  SIC_PICENSET = 1 << 3; // KBD int=3 on SIC
  fbuf_init();
  kprintf("C3.3 start: Interrupt-driven drivers for Timer KBD UART\n");
  timer_init();

  /***********
  for (i=0; i<4; i++){
     tp[i] = &timer[i];
     timer_start(i);
  }
  ************/
  timer_start(0);
  kbd_init();

  uart_init();
  up = & uart[0];

  unlock();
  kprintf("\nenter KBD here\n");
  uuprints("from UART0 here\n\r");
  int move = 0;
  int key;
  int cnt = 0;
  // fix bug? in tiled 0 size
  for (int k = 0; k < pacman_size; k++) {
    pacman[k].height = 17;
    pacman[k].width = 17;
  }

  /* TODO
  sort the pacman array based on the x coordinate so that the pacman postions are in order.
  You need to do the sorting before the following loop.
  The pacmans and the pacman arrays need to be aligned i.e a given index refers to the same pacman entity in both arrays.
  */
  for (j = 0; j < 18; j++) {
    for (i = 0; i < 120; i++) {
      int tile = pacman_tiles[j * 128 + i];

      if (tile) {
        for (int k = 0; k < pacman_size; k++) {
          //put_tile(j*17,(i-xpos)*17,tile,&_binary_tilemap_bmp_start);
          int check = checkPacman(pacman + k, i * 16, j * 16, 17, 17);
          if (check) {
            pacmans[k].x = i;
            pacmans[k].y = j;
            pacmans[k].isalive = 1;
            //  uprintf("found %d %d\n",j,i);
          }

        }
      }
    }
  }

  while (1) {
    //uprintf("enable %x\n",*(volatile unsigned int *)(0x1012001C));
    if (vidhandler) {
      //     uprintf("got vidhandler %x counter %d\n",*(volatile unsigned int *)(0x1012001C),counter);
      vidhandler = 0;
      counter++;
    }

    move = 0;
    if (upeek(up)) {
      key = ugetc(up);
      switch (key) {
      case 's':
        if (sprites[1].x) {

          if (xpos > 10)
            xpos--;
          else
            xpos = 100;
        }
        move = 1;
        break;
      case 'd':
        if (sprites[1].x < 624) {
          sprites[1].p = & _binary_ship_right_bmp_start;
          sprites[1].direction = 1;
          //sprites[1].x+=16;
          if (xpos == 100)
            xpos = 10;
          else
            xpos++;
        }
        move = 1;
        break;
      case 'w':
        if (sprites[1].y > 16)
          sprites[1].y -= 16;
        move = 1;
        break;
      case 'e':
        if (sprites[1].y < 600)
          sprites[1].y += 16;
        move = 1;
        break;
      case 'f':

        break;
      default:
        move = 0;
      }
    }
    if (move) {
      //uprintf("x= %d y= %d w =%d h= %d\n",sprites[1].x+xpos*16,sprites[1].y,sprites[1].h, sprites[1].w);
      for (int i = 0; i < Collisions_size; i++) {
        int check = checkObstacle(Collisions + i, sprites[1].x + xpos * 16, sprites[1].y, sprites[1].h, sprites[1].w);

        if (check) {
          /* TODO */
          /* a collision do not allow the defender to enter into the obstacle */
          uprintf("cnt = %d collision obj %d\n", cnt++, i);
          }
        }
      }


      for (int i = 0; i < pacman_size; i++) {
        int check = checkPacman(pacman + i, sprites[1].x + xpos * 16, sprites[1].y, sprites[1].h, sprites[1].w);

        if (check) {
          // Remove the Pacman entity from the pacman_tiles array
          int tile_x = pacmans[i].x;
          int tile_y = pacmans[i].y;
          pacman_tiles[tile_y * 128 + tile_x] = 0; // Set the tile to 0 to remove the Pacman
          pacmans[i].isalive = 2; // Mark the Pacman as saved
          uprintf("cnt = %d pacman obj %d\n", cnt++, i);

        }
      }


      if (move || spriteMove) {
        // switch_buffer();
        if (spriteMove) {
          /* TODO
          may need changes here if you combine layer arrays.
          return -1 when no more paths to pacman entites can be found.
           otherwise the function returns the pacman number in the pacmans and pacman arrays.
           */
          int pacno = catch_pacman(sprites + 2);
        }
        Draw_all();
        spriteMove = 0;
      }

  }
}