//
// Created by Daniël Kamp on 12/04/2022.
//

#ifndef DONUT_GUI_H
#define DONUT_GUI_H

#include <Global.h>
#include <string>
#include <mutex>

#if defined(BUILD_GUI_NCURSES)
#include <curses.h>
#endif

#include <iostream>

using namespace std;

class GUI {
	public:
		GUI ();
		~GUI ();
		
		void initgui();
		string input();
		void output(string, bool lb=true, int x=-1, int y=-1, int c=0);
		void process();
	
	private:
		int row, col, winy, winx;
		string cmd;
		char cmd_stream[80];
};


#endif //DONUT_GUI_H
