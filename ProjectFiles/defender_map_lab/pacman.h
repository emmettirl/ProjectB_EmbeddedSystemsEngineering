#include "Object.h"
struct sprite {
   int x,y;
   int w,h;
   char *p;
   int direction;
   int enabled;
   int oldstartRow;
   int oldstartCol;
};

struct Pacman {
  int x,y;
  int isalive;
};
extern struct Pacman pacmans[];
extern int pacman_size;
extern unsigned char pacman_tiles[];
extern Object * pacman;
extern unsigned char Tiles[];
