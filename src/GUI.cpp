//
// Created by Daniël Kamp on 12/04/2022.
//

#include "System/GUI.h"

mutex mtx2;

GUI::GUI () {
	// Construct a GUI with ncurses, LVGL, or std::cout
	#if defined(BUILD_GUI_NCURSES)
		initscr();
		
		curs_set(0);
		use_default_colors();
		start_color();
		init_pair(0, COLOR_CYAN, -1);
		init_pair(1, COLOR_MAGENTA, -1);
		init_pair(2, COLOR_RED, -1);
		init_pair(3, COLOR_GREEN, -1);
		init_pair(4, COLOR_WHITE, -1);
		init_pair(5, COLOR_CYAN, -1);
	#endif
}

GUI::~GUI () {
	// Destroy the GUI.
	#if defined(BUILD_GUI_NCURSES)
		cout << "\x1B[2J\x1B[H";
		curs_set(1);
	#endif
}

void GUI::initgui () {
	#if defined(BUILD_GUI_NCURSES)
		cout << "\x1B[2J\x1B[H";
		getmaxyx(stdscr, row, col);
		
		move(0, col-18);
		attron(COLOR_PAIR(3));
		printw("Donut");
		attroff(COLOR_PAIR(3));
		printw(" version 0.2");
		refresh();
		
		move(4,0);
		clrtoeol();
		attron(COLOR_PAIR(2));
		printw("Incoming MIDI:");
		attroff(COLOR_PAIR(2));
		refresh();
		
		move(8, 0);
		clrtoeol();
		printw("[");
		attron(COLOR_PAIR(3));
		printw(">");
		attroff(COLOR_PAIR(3));
		printw("] ");
		refresh();
	#endif
}

string GUI::input() {
	// This function takes user input
	#if defined(BUILD_GUI_NCURSES)
		mtx2.lock();
		move(8, 4);
		clrtoeol();
		curs_set(1);
		mtx2.unlock();
		
		getstr(cmd_stream);
		cmd = cmd_stream;
	#elif defined(BUILD_GUI_COUT)
//		cin >> getline(cmd_stream);
	#endif
	
	return cmd;
}

void GUI::output(const string line, bool lb, int x, int y, int c) {
	// This function displays a command's output on the screen
	#if defined(BUILD_GUI_NCURSES)
		curs_set(0);
		if(lb) {
			clrtobot();
			refresh();
		}
		
		if(x > -1 && y > -1) {
			getyx(stdscr,winy,winx);
			move(y, x);
			clrtoeol();
			attron(COLOR_PAIR(c));
			printw(line.c_str());
			attroff(COLOR_PAIR(c));
			move(winy, winx);
		} else {
			attron(COLOR_PAIR(c));
			printw(line.c_str());
			attroff(COLOR_PAIR(c));
			if(lb) {
				printw("\n");
			}
		}
		
		curs_set(1);
		refresh();
	#elif defined(BUILD_GUI_COUT)
		if(lb) {
			cout << line << endl;
		} else {
			cout << line;
		}
	#endif
}

void GUI::process() {
	// This is where general GUI elements go (like voice count, midi io, etc)
}