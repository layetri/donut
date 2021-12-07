//
// Created by Daniël Kamp on 09/11/2021.
//

#if false
#ifndef DONUT_COMMANDPOOL_H
#define DONUT_COMMANDPOOL_H

#include <string>
#include <functional>
using namespace std;

struct Command {
    string command;
    function handler;

    bool can_handle(string command);
    function<void()> handle();
};

class CommandPool {

};


#endif //DONUT_COMMANDPOOL_H
#endif
