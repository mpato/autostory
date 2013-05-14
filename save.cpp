#include <stdlib.h>
#include <stdio.h>

#include "save.h"
#include "autostory.h"

int load_character(FILE *file, state_t *state)
{
}

int load_saved_state(const char *filename, state_t *state)
{
  FILE *file;
  char line[100];
  fprintf(stderr, "Loading save file %s\n", filename);
  file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "Error openning file %s\n", filename);
    return -1;
  }
  fscanf(file, "MAX_CHARACTERS=%d\n", state->max_characters);
  fscanf(file, "MAX_NATIONS=%d\n", state->max_nations);
  fscanf(file, "MAX_LOCATIONS=%d\n", state->max_locations);
  while (!feof(file)) {
    fscanf(file, "%[^\n]\n", line);
    if (strcmp(line, "__CHARACTER__") == 0) {
      if (load_character(file, state) != 0)
	return -1;
    } else if (strcmp(line, "__LOCATION__") == 0) {
      if (load_location(file, state) != 0)
	return -1;
    } else if (strcmp(line, "__NATION__") == 0) {
      if (load_nation(file, state) != 0)
	return -1;
    } else {
      fprintf(stderr, "Unknown structure %s\n", line);
    }
  }
  fprintf(stderr, "\n");
  return 0;
}
