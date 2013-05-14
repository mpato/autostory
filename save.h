#ifndef SAVE_H
#define SAVE_H
#include "autostory.h"
void load_saved_stated(const char *filename, state_t *state);
void save_state(const char *filename, state_t *state);
#endif
