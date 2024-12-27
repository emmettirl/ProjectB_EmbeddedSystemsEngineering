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
#define printf kprintf

char *tab = "0123456789ABCDEF";
int BASE;
int color;

#include "uart.c"
#include "kbd.c"
#include "timer.c"
#include "vid.c"
#include "exceptions.c"
#include "sdc.c"
#include "sdc.h"
#include "filesystem.h"

typedef struct {
    char name[32];
    u32 size;
    u32 start_sector;
} DirectoryEntry;

void ls() {
    struct sdc_File files[10]; // Assuming a maximum of 10 files
    int i, num_files;
    char buf[512];

    printf("File Name\tSize\n");
    printf("-------------------------\n");

    getSector(0, buf);

    // Read the number of files
    memcpy(&num_files, buf, 4);

    // Parse the buffer to extract file entries
    for (i = 0; i < num_files; i++) {
        int offset = 4 + i * sizeof(struct sdc_File); // Each entry is 24 bytes
        if (offset + sizeof(struct sdc_File) > 512) break; // Ensure we don't read beyond the buffer
        memcpy(&files[i], buf + offset, sizeof(struct sdc_File));
    }

    // Print the file names and sizes
    for (i = 0; i < num_files; i++) {
        if (files[i].name[0] != '\0') { // Check if the entry is valid
            printf("%s\t%d bytes\n", files[i].name, files[i].size);
        }
    }
}

void cat(char *filename) {
    struct sdc_File files[10]; // Assuming a maximum of 10 files
    int i, num_files;
    char buf[512];

    getSector(0, buf);

    // Read the number of files
    memcpy(&num_files, buf, 4);

    // Parse the buffer to extract file entries
    for (i = 0; i < num_files; i++) {
        int offset = 4 + i * sizeof(struct sdc_File); // Each entry is 24 bytes
        if (offset + sizeof(struct sdc_File) > 512) break; // Ensure we don't read beyond the buffer
        memcpy(&files[i], buf + offset, sizeof(struct sdc_File));
    }

    // Find the file by name
    for (i = 0; i < num_files; i++) {
        if (strcmp(files[i].name, filename) == 0) {
            // File found, read and print its contents
            int j;
            for (j = 0; j < MAX_SECTORS && files[i].sectors[j] != 0; j++) {
                getSector(files[i].sectors[j], buf);
                printf("%s", buf);
            }
            printf("\n");
            return;
        }
    }

    // File not found
    printf("File not found: %s\n", filename);
}


void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);
void timer_handler();
extern int sdc_handler();

// IRQ interrupts handler entry point
//void __attribute__((interrupt)) kc_handler()
void kc_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC SIV status registers to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  
    //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);
    // VIC status BITs: timer0=4, uart0=13, uart1=14, SIC=31: KBD at 3

    if (vicstatus & (1<<4)){
         timer_handler(0);
	 // kprintf("TIMER "); // verify timer handler return to here
    }
    if (vicstatus & (1<<12)){ // Bit 12 
         uart_handler(&uart[0]);
	 // kprintf("U0 "); 
    }
    if (vicstatus & (1<<13)){ // bit 13
         uart_handler(&uart[1]);
    }
    if (vicstatus & 0x80000000){ // SIC interrupts=bit_31=>KBD at bit 3 
       if (sicstatus & (1<<3)){
          kbd_handler();
       }
       if (sicstatus & (1<<22)){
          sdc_handler();
       }
    }
}


char rbuf[512], wbuf[512];
char *line[2] = {"THIS IS A TEST LINE", "this is a test line"};
int main()
{
  int i, sector, N; 
   UART *up;
   KBD  *kp;
 
   color = RED;
   row = col = 0; 
   fbuf_init();
   kbd_init();
   uart_init();
   up = &uart[0];
   kp = &kbd;

   /* enable timer0,1, uart0,1 SIC interrupts */
   VIC_INTENABLE |= (1<<4);  // timer0,1 at bit4 
   VIC_INTENABLE |= (1<<12); // UART0 at bit12
   VIC_INTENABLE |= (1<<13); // UART1 at bit13
   VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31
  
   /* enable KBD and SDC IRQ */
   SIC_INTENABLE |= (1<<3);  // KBD int=bit3 on SIC
   SIC_INTENABLE |= (1<<22); // SDC int=bit22 on SIC

   SIC_ENSET |= 1<<3;  // KBD int=3 on SIC
   SIC_ENSET |= 1<<22;  // SDC int=22 on SIC
   *(kp->base+KCNTL) = 0x12;

   kprintf("C3.4 start: test TIMER KBD UART SDC interrupt-driven drivers\n");
   timer_init();
   timer_start(0);

   /***********
   kprintf("test uart0 I/O: enter text from UART 0\n");
   while(1){
     ugets(up, line);
     //color=GREEN; kprintf("line=%s\n", line); color=RED;
     uprintf("  line=%s\n", line);
     if (strcmp(line, "end")==0)
	break;
   }

   //uprintf("test KBD inputs\n"); // print to uart0
   kprintf("test KBD inputs\n"); // print to LCD
   
   while(1){
      kgets(line);
      color = GREEN;
      //uprintf("line=%s\n", line);
      kprintf("line=%s\n", line);
      if (strcmp(line, "end")==0)
         break;
   }
   **************/
   color = CYAN;
   printf("test SDC DRIVER\n");
   sdc_init();
   N = 3;
   
   

   for (sector=0; sector < N; sector++){
     uprintf("READ  sector %d\n", sector);
     for (i=0; i<512; i++)
        rbuf[i] = ' ';

     getSector(sector, rbuf);
     for (i=0; i<512; i++){
       uprintf("%d %c", rbuf[i],rbuf[i]);
     }
     uprintf("\n");
   }
   
   printf("in while(1) loop: enter ls\n");
   char line1[100];
   while(1){
     if (upeek(up)){
        ugets(up, line1);
        if (line1[0]=='l' && line1[1] =='s'){
			printf("got ls\n");
			printf("-------------------------\n");
        	ls();
        	printf("-------------------------\n");
           }
		else if (line1[0] == 'c' && line1[1] == 'a' && line1[2] == 't' && line1[3] == ' ') {
            printf("got cat\n");
            printf("cat command implementation for file: %s\n", line1 + 4);
            cat(line1 + 4);
            printf("-------------------------\n");

        } else if (line1[0] == 'm' && line1[1] == 'v' && line1[2] == ' ') {
            printf("got mv\n");
            printf("mv command implementation for file: %s\n", line1 + 3);
        	printf("-------------------------\n");

        } else if (line1[0] == 'c' && line1[1] == 'o' && line1[2] == 'p' && line1[3] == 'y' && line1[4] == ' ') {
            printf("got copy\n");
            printf("copy command implementation for file: %s\n", line1 + 5);
            printf("-------------------------\n");

        } else {
			printf("not recognised\n");
	        printf("-------------------------\n");

    	}
    }
   }
   
}
