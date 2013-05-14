#ifndef SAVE_H
#define SAVE_H
#include "autostory.h"
int load_saved_state(const char *filename, state_t *state);
int save_state(const char *filename, state_t *state);
#endif
