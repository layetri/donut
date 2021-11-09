//
// Created by Daniël Kamp on 09/11/2021.
//

#if false
#include "Header/CommandPool.h"

bool Command::can_handle(string command) {
  return command == this->command;
}

function<void()> Command::handle() {
  return handler;
}
#endif