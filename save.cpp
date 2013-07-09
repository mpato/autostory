#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "save.h"
#include "autostory.h"

int load_character(FILE *file, state_t *state, int is_player)
{
  char line[100];
  character_t *character;
  npc_t * npc;
  int id, type;
  relationship_t *relationship;
  if (is_player)
    character = &state->player;
  else {
    npc = new npc_t();
    character = npc;
  }
  if (fscanf(file, "ID = %d\n", &character->id) != 1) {
    fprintf(stderr, "Error: missing character id\n");
    return -1;
  }
  
  state->characters.add(character);
  
  while (!feof(file)) {
    fscanf(file, "%[^\n]\n", line);
    if (strcmp(line, "__END__") == 0)
      return 0;
    if (sscanf(line, "LIFE = %d", &character->life) == 1)
      continue;
    if (sscanf(line, "RANK = %d", &character->rank) == 1)
      continue;
    if (sscanf(line, "PROGRESS = %d", &character->progress) == 1)
      continue;
    if (sscanf(line, "STRENGTH = %d", &character->strength) == 1)
      continue;
    if (sscanf(line, "DEFENSE = %d", &character->defense) == 1)
      continue;
    if (sscanf(line, "AGE = %d", &character->age) == 1)
      continue;
    if (sscanf(line, "GENDER = %d", &character->gender) == 1)
      continue;
    if (sscanf(line, "ALIGNMENT = %d", &character->alignment) == 1)
      continue;
    if (sscanf(line, "MONEY = %d", &character->money) == 1)
      continue;
    if (sscanf(line, "NATION = %d", &id) == 1) {
      character->nation = (nation_t *) id;
      continue;
    }
    if (sscanf(line, "LOCATION = %d", &id) == 1) {
      character->location = (location_t *) id;
      continue;
    }
    if (sscanf(line, "RELATIONSHIP = %d : %d", &id, &type) == 2) {
      relationship = new relationship_t();
      relationship->a = character;
      relationship->b = (character_t *) id;
      relationship->type = type;
      character->relationships.add(relationship);
      continue;
    }
    if (sscanf(line, "FIRST_NAME = %s", character->first_name) == 1)
      continue;
    if (sscanf(line, "MIDDLE_NAME = %s", character->middle_name) == 1)
      continue;
    if (sscanf(line, "LAST_NAME = %[^\n]", character->last_name) == 1)
      continue;
    if (sscanf(line, "TARGET = %d", &id) == 1) {
      character->target = (character_t *) id;
      continue;
    }
    if (sscanf(line, "LAST_ATTACKER = %d", &id) == 1) {
      character->last_attacker = (character_t *) id;
      continue;
    }
    if (is_player) {
      fprintf(stderr, "Error: Unknown player field %s\n", line);
      return -1;
    }
    if (sscanf(line, "STATE = %d", &npc->state) == 1)
      continue;
    fprintf(stderr, "Error: Unknown npc field %s\n", line);
    return  -1;
  }
  fprintf(stderr, "Error: unexpected end of file when loading character\n");
  return -1;
}

int load_nation(FILE *file, state_t *state)
{
  char line[100];
  nation_t *nation;
  int tmp;
  nation = new nation_t();
 
  if (fscanf(file, "ID = %d\n", &nation->id) != 1) {
    fprintf(stderr, "Error: missing nation id\n");
    return -1;
  }
  
  state->nations.add(nation);
  
  while (!feof(file)) {
    fscanf(file, "%[^\n]\n", line);
    if (strcmp(line, "__END__") == 0)
      return 0;
    if (sscanf(line, "NAME = %s", nation->name) == 1)
      continue;
    if (sscanf(line, "RULER = %d", &tmp) == 1) {
      nation->ruler = (character_t *) tmp;
      continue;
    }    
    fprintf(stderr, "Error: Unknown nation field %s\n", line);
    return  -1;
  }
  fprintf(stderr, "Error: unexpected end of file when loading nation\n");
  return -1;
}

int load_location(FILE *file, state_t *state)
{
  char line[100];
  location_t *location;
  int tmp, where;
  location = new location_t();
 
  if (fscanf(file, "ID = %d\n", &location->id) != 1) {
    fprintf(stderr, "Error: missing location id\n");
    return -1;
  }

  state->locations.add(location);

  while (!feof(file)) {
    fscanf(file, "%[^\n]\n", line);
    if (strcmp(line, "__END__") == 0)
      return 0;
    if (sscanf(line, "DESCRIPTION = %s", location->description) == 1)
      continue;
    if (sscanf(line, "NAME = %[^\n]", location->name) == 1)
      continue;
    if (sscanf(line, "OWNER = %d", &tmp) == 1) {
      location->owner = (character_t *) tmp;
      continue;
    }    
    if (sscanf(line, "NATION = %d", &tmp) == 1) {
      location->nation = (nation_t *) tmp;
      continue;
    }
    if (sscanf(line, "WIDTH = %d", &location->width) == 1)
      continue;
    if (sscanf(line, "HEIGHT = %d", &location->height) == 1)
      continue;
    if (sscanf(line, "NEIGHBOOR = %d @ %d", &tmp, &where) == 2) {
      location->neighboors[where] = (location_t *) tmp;
      continue;
    }
    fprintf(stderr, "Error: Unknown location field %s\n", line);
    return  -1;
  }
  fprintf(stderr, "Error: unexpected end of file when loading location\n");
  return -1;
}

int link_characters(state_t * state)
{
  character_t *character, *tmp;
  location_t *location;
  for (int i = 0; i < state->characters.size(); i++) {
    character = (character_t *) state->characters.get(i);
    if (character->life) {
      location = (location_t *) state->locations.search_by_id((int) character->location);
      if (location == NULL) {
	fprintf(stderr, "Error: character %d is missing location with id %d\n", character->id, (int) character->location);
	return -1;
      }
      character->location = location;
      location->characters_in_here.add(character);
    }
    if (character->target) {
      tmp = (character_t *) state->characters.search_by_id((int) character->target);
      if (tmp == NULL) {
	fprintf(stderr, "Error: character %d is missing target with id %d\n", character->id, (int) character->target);
	return -1;
      }
      character->target = tmp;
    }
    if (character->last_attacker) {
      tmp = (character_t *) state->characters.search_by_id((int) character->last_attacker);
      if (tmp == NULL) {
	fprintf(stderr, "Error: character %d is missing last attacker with id %d\n", character->id, (int) character->last_attacker);
	return -1;
      }
      character->last_attacker = tmp;
    }
  }
  return 0;
}

int link_locations(state_t * state)
{
  character_t *character;
  location_t *location, *neighboor;
  for (int i = 0; i < state->locations.size(); i++) {
    location = (location_t *) state->locations.get(i);
    character = (character_t *) state->characters.search_by_id((int) location->owner);
    if (character != NULL) {
      location->owner = character;
      character->lands.add(location);
    }
    if (location->neighboors[0]) {
      neighboor = (location_t *) state->locations.search_by_id((int) location->neighboors[0]);
      if (neighboor != NULL) {
	location->neighboors[0] = neighboor;
	neighboor->neighboors[3] = location;
      }
    }
    if (location->neighboors[1]) {
      neighboor = (location_t *) state->locations.search_by_id((int) location->neighboors[1]);
      if (neighboor != NULL) {
	location->neighboors[1] = neighboor;
	neighboor->neighboors[2] = location;
      }
    }
  }
  return 0;
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
  if (fscanf(file, "ROUND = %d\n", &state->round) != 1)
    state->round = 0;
  if (state->round == 0) {
    if (!feof(file))
      fprintf(stderr, "WARNING: starting at round zero; the rest of the file will be ignored\n");
    return 0;
  }
  while (!feof(file)) {
    fscanf(file, "%[^\n]\n", line);
    if (strcmp(line, "__CHARACTER__") == 0) {
      if (load_character(file, state, 0) != 0)
	return -1;
    } else if (strcmp(line, "__PLAYER__") == 0) {
      if (load_character(file, state, 1) != 0)
	return -1;
    } else if (strcmp(line, "__LOCATION__") == 0) {
      if (load_location(file, state) != 0)
	return -1;
    }/* else if (strcmp(line, "__NATION__") == 0) {
      if (load_nation(file, state) != 0)
	return -1;
	} */
    else {
      fprintf(stderr, "Error: Unknown structure %s\n", line);
    }
  }
  if (link_characters(state) != 0)
    return -1;
  if (link_locations(state) != 0)
    return -1;
  fprintf(stderr, "Loading done\n");
  return 0;
}

int save_character(FILE *file, character_t *character, int is_player)
{
  char line[100];
  int id, type;
  relationship_t relationship;
  if (is_player)
    fprintf(file, "__PLAYER__\n");
  else
    fprintf(file, "__CHARACTER__\n");
  
  fprintf(file, "ID = %d\n", character->id);
  fprintf(file, "LIFE = %d\n", character->life);
  fprintf(file, "RANK = %d\n", character->rank);
  fprintf(file, "PROGRESS = %d\n", character->progress);
  fprintf(file, "STRENGTH = %d\n", character->strength);
  fprintf(file, "DEFENSE = %d\n", character->defense);
  fprintf(file, "AGE = %d\n", character->age);
  fprintf(file, "GENDER = %d\n", character->gender);
  fprintf(file, "ALIGNMENT = %d\n", character->alignment);
  fprintf(file, "MONEY = %d\n", character->money);
  if (character->nation)
    fprintf(file, "NATION = %d\n", character->nation->id);
  if (character->location)
    fprintf(file, "LOCATION = %d\n", character->location->id);
  /*   if (sscanf(line, "RELATIONSHIP = %d : %d", &id, &type) == 2) {
       relationship = new relationship_t();
       relationship->a = character;
       relationship->b = id;
       relationship->type = type;
       character->relationships.add(relationship);
       continue;
       }*/
  fprintf(file, "FIRST_NAME = %s\n", character->first_name);
  fprintf(file, "MIDDLE_NAME = %s\n", character->middle_name);
  fprintf(file, "LAST_NAME = %s\n", character->last_name);
  if (character->target)
    fprintf(file, "TARGET = %d\n", character->target->id);
  if (character->last_attacker)
    fprintf(file, "LAST_ATTACKER = %d\n", character->last_attacker->id);
  fprintf(file, "__END__\n");
}

int save_location(FILE *file, location_t *location)
{
  fprintf(file, "__LOCATION__\n");
  fprintf(file, "ID = %d\n", location->id);

  if (location->description[0])
    fprintf(file, "DESCRIPTION = %s\n", location->description);
  if (location->name[0])
    fprintf(file, "NAME = %s\n", location->name);   
  if (location->owner != NULL)
    fprintf(file, "OWNER = %d\n", location->owner->id);
  if (location->nation != NULL)
    fprintf(file, "NATION = %d\n", location->nation->id);
  fprintf(file, "WIDTH = %d\n", location->width);
  fprintf(file, "HEIGHT = %d\n", location->height);
  if (location->neighboors[0] != NULL)
    fprintf(file, "NEIGHBOOR = %d @ 0\n", location->neighboors[0]->id);
  if (location->neighboors[1] != NULL)
    fprintf(file, "NEIGHBOOR = %d @ 1\n", location->neighboors[1]->id);
  fprintf(file, "__END__\n");
  return -1;
}

int save_state(const char *filename, state_t *state)
{
  FILE *file;
  character_t *character;
  location_t *location;
  if (state == NULL) {
    fprintf(stderr, "No state to save\n");
    return 0;
  }
  fprintf(stderr, "Saving state to file %s\n", filename);
  file = fopen(filename, "w");
  if (file == NULL) {
    fprintf(stderr, "Error openning file %s\n", filename);
    return -1;
  }
  fprintf(file, "ROUND = %d\n", state->round);
  if (state->round == 0) {
    fprintf(stderr, "WARNING: saving round zero; the rest of the state will be ignored\n");
    return 0;
  }

  for (int i = 0; i < state->characters.size(); i++) {
    character = (character_t *) state->characters.get(i);
    save_character(file, character, character == &state->player);
  }

  for (int i = 0; i < state->locations.size(); i++) {
    location = (location_t *) state->locations.get(i);
    save_location(file, location);
  }
  
  fprintf(stderr, "saving done\n");
  return 0;
}
