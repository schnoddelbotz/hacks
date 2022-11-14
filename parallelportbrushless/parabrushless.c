#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#include <curses.h>
#define BOLDKEY(x,y) attron(A_UNDERLINE ^ A_BOLD); printw(x); attroff(A_UNDERLINE ^ A_BOLD); printw(y);
/**  gcc -lcurses -o parabrushless parabrushless.c && ./parabrushless [0-300] **/

main(int argc, char **argv) {
	WINDOW *w;
	int par_data;
	int PORT = 0x378;
	int i,ch;
	int hi_ms,lo_ms,frame_len;
	frame_len=1650; // 1650 seems fine
	lo_ms=200; 
	if (argc==2) {	
		lo_ms = atoi(argv[1]);
	}
  	hi_ms = frame_len - lo_ms;

	// lpt
	if (ioperm(PORT,1,1))  {
		fprintf(stderr, "ERROR: Can't gain access to port %x\n", PORT);
		exit(1);
	}
	// curses 
	w=initscr(); cbreak(); noecho(); nodelay(w,1); keypad(w,1); box(w, 0, 0);
	// init screen
	mvprintw(1,2,"Brushless motor controller parallel port tester (PWM generator)");
	mvprintw(2,2,"Keys : ");
	BOLDKEY("f", "ull speed (100), ");  BOLDKEY("z", "ero pos (200), "); BOLDKEY("r", "everse (300)");
	mvprintw(3,2,"       ");
	BOLDKEY("u", "p +1, KEY_UP +10, "); BOLDKEY("d", "own -1, KEY_DOWN -10. "); BOLDKEY("q",  "uit!");
	wattron(w, A_REVERSE);
	mvprintw(5,2,"Initial value: %3d", lo_ms);

	while (1) {
	  // check for keystroke (non-blocking)
	  if ((ch=getch())>0) {
		if (ch==113) { endwin(); exit(0); }
		if (ch==KEY_DOWN ) { /*Down*/ if(lo_ms>90)lo_ms=lo_ms-10; }
		if (ch==100) { /*down*/ if(lo_ms>90)lo_ms--; }
		if (ch==KEY_UP ) { /*Up*/   if(lo_ms<300)lo_ms=lo_ms+10; }
		if (ch==117) { /*up*/   if(lo_ms<300)lo_ms++; }
		if (ch==114) { /*reverse*/ lo_ms = 300; }
		if (ch==122) { /*zero*/ lo_ms = 200; }
		if (ch==102) { /*full*/ lo_ms = 100; }
		mvprintw(5,2,"Current value: %3d", lo_ms);
	  	hi_ms=frame_len-lo_ms;
	  }

	  // play the signal, 30 times, before next keycheck
	  for (i=1; i<30; i++) {
		par_data = 0;
		outb((unsigned char)par_data, PORT);
		usleep(lo_ms);

		par_data = 1;
		outb((unsigned char)par_data, PORT);
		usleep(hi_ms);
	  }
	}

	endwin();
	exit(0);
}
