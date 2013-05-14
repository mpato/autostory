#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "save.h"
#include "autostory.h"

int load_character(FILE *file, state_t *state)
{
  char line[100];
  character_t *character;
  character = &state->characters[state->nr_characters++];
  while (!feof(file)) {
    fscanf(file, "%[^\n]\n", line);
    if (strcmp(line, "__END__") == 0)
      return 0;
  }
  fprintf(stderr, "Error: unexpected end of file when loading character\n");
  return -1;
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
  if (fscanf(file, "MAX_CHARACTERS = %d\n", state->max_characters) != 1) {
    fprintf(stderr, "Error: missing max characters setting\n");
    return -1;
  }
  if (fscanf(file, "MAX_NATIONS=%d\n", state->max_nations) != 1) {
    fprintf(stderr, "Error: missing max nations setting\n");
    return -1;
  }
  if (fscanf(file, "MAX_LOCATIONS=%d\n", state->max_locations) != 1) {
    fprintf(stderr, "Error: missing max locations setting\n");
    return -1;
  }
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
  fprintf(stderr, "Loading done\n");
  return 0;
}
