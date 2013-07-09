#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "save.h"
#include "autostory.h"

typedef int (*command_f)(List *);

int starts_with(char *str, char *start)
{
  int i;
  for (i = 0; start[i]; i++) {
    if (str[i] != start[i])
      return 0;
  }
  return 1;
}

void tokenizer(char *str, GList *ret)
{
  int i, token;
  if (ret == NULL)
    return;
  token = 0;
  for (i = 0; str[i]; i++) {
    if (str[i] == '"' || str[i] == '\'') {
      str[i] = '\0';
      if (token != 2) {
	if (str[i + 1] != '\0') {
	  ret->add(str + i + 1);
	  token = 2;
	}
      } else
	  token = 0;
    } else if (str[i] != ' ' && str[i] != '\t') {
      if (!token) {
	ret->add(str + i);
	token = 1;
      }
    } else if (token != 2){
      str[i] = '\0';
      token = 0;
    }
  }
}

int is_empty(char *str)
{
  for (int i = 0; str[i]; i++) {
    if (str[i] == '"' || str[i] == '\'') {
      if (str[i + 1])
	return 0;
    } else if (str[i] != ' ' && str[i] != '\t')
      return 0;
  }
  return 1;
}

int my_atoi(char *str, int &result)
{
  char *end;
  int tmp;
  if (*str == '\0')
    return 0;
  tmp = strtol(str, &end, 10);
  if (*end != '\0')
    return 0;
  result = tmp;
  return 1;
}

state_t *state;
char *save_filename;

character_t *search_for_character(char *id_str)
{
  int id;
  character_t *character;
  if (!my_atoi(id_str, id)) {
    printf("%s is not a valid id\n", id_str);
    return NULL;
  }
  character = (character_t *) state->characters.search_by_id(id);
  if (character == NULL) {
    printf("No character with id %d\n", id);
    return NULL;
  }
  return character;
}

class event_listener_t : public event_listener_i {
public:
  virtual void on_attack(character_t *attacker, character_t *target, int damage) {
    char buf[75], buf2[75];
    if (attacker != &state->player && target != &state->player)
      return;
    attacker->get_full_name(buf, 75);
    target->get_full_name(buf2, 75);
    printf("'%s' attacks '%s' with %d damage\n", buf, buf2, damage);
  }

  virtual void on_kill(character_t *attacker, character_t *target) {
    char buf[75], buf2[75];
    if (attacker != &state->player && target != &state->player)
      return;
    attacker->get_full_name(buf, 75);
    target->get_full_name(buf2, 75);
    printf("'%s' kills '%s'\n", buf, buf2);
  }
  virtual void on_cannot_go(int direction) {
    printf("You cannot go there\n");
  }
};

event_listener_t event_listener;

int cmd_exit(List *args)
{
  char *name;
  name = (char *) args->get(0);
  if (strcmp(name, "exit"))
    return 0;
  exit(0);
}

int cmd_start(List *args)
{
  char *name, *input;
  GList tmp;
  int st;
  name = (char *) args->get(0);
  if (strcmp(name, "start"))
    return 0;
  if (state)
    delete state;
  state = new state_t(&event_listener);
  st = 0;
  while (st < 3) {
    if (st == 0)
      input = readline("Your first name:");
    else if (st == 1)
      input = readline("Your middle name:");
    else if (st == 2)
      input = readline("Your last name:");
    if (input == NULL)
      continue;
    tmp.clear();
    tokenizer(input, &tmp);
    if (tmp.size() > 0) {
      if (st == 0)
	strcpy(state->player.first_name, (char *) tmp.get(0));
      else if (st == 1)
	strcpy(state->player.middle_name, (char *) tmp.get(0));
      else if (st == 2)
	strcpy(state->player.last_name, (char *) tmp.get(0));
      st++;
    }
    free(input);
  }
  state->start(state->player);
  return 1;
}

int cmd_save(List *args)
{
  char *name;
  name = (char *) args->get(0);
  if (strcmp(name, "save"))
    return 0;
  save_state("test.sv", state);
  return 1;
}

int cmd_load(List *args)
{
  char *name;
  name = (char *) args->get(0);
  if (strcmp(name, "load"))
    return 0;
  if (state)
    delete state;
  state = new state_t(&event_listener);
  if (load_saved_state("test.sv", state)) {
    delete state;
    state = NULL;
  }
  return 1;
}

int cmd_stats(List *args)
{
  char *name, buf[25];
  character_t *character;
  name = (char *) args->get(0);
  if (strcmp(name, "stats"))
    return 0;
  if (!state) {
    printf("No game started\n");
    return 1;
  }
  if (args->size() > 1) {
    character = search_for_character((char *) args->get(1));
    if (character == NULL)
      return 1;
  } else
    character = &state->player;
  printf("ID          =  %d\n", character->id);
  printf("LIFE        =  %d\n", character->life);
  printf("RANK        =  %d\n", character->rank);
  if (character == &state->player)
    printf("PROGRESS    =  %d/%d\n", character->progress, get_progress_limit(character->rank));
  printf("STRENGTH    =  %d\n", character->strength);
  printf("DEFENSE     =  %d\n", character->defense);
  printf("AGE         =  %d\n", character->age);
  printf("GENDER      =  %s\n", character->gender == GENDER_MALE ? "MALE" : "FEMALE");
  get_alignment_name(buf, 25, character->alignment);
  printf("ALIGNMENT   =  %s\n", buf);
  printf("MONEY       =  %d\n", character->money);
  if (character->nation)
    printf("NATION      =  %s\n", character->nation->name);
  if (character->location)
    printf("LOCATION    =  %s\n", character->location->name);
  if (character->target)
    printf("TARGET      =  %s %s %s\n", character->target->first_name, character->target->middle_name, character->target->last_name);
if (character->last_attacker)
  printf("ATTACKER    =  %s %s %s\n", character->last_attacker->first_name, character->last_attacker->middle_name, character->last_attacker->last_name);
  printf("FIRST_NAME  =  %s\n", character->first_name);
  printf("MIDDLE_NAME =  %s\n", character->middle_name);
  printf("LAST_NAME   =  %s\n", character->last_name);
  return 1;
}

void scenterf(char *ret, int size, char *src)
{
  int padding, len;
  len = strlen(src);
  padding = (size - len) / 2;
  sprintf(ret, "%*s%s%*s", padding, " ", src, size - padding - len, " ");
}

int cmd_lands(List *args)
{
  char *name, buf[25], buf2[75], owner[75];
  List *list;
  location_t *location;
  name = (char *) args->get(0);
  if (strcmp(name, "lands"))
    return 0;
  if (!state) {
    printf("No game started\n");
    return 1;
  }
  if (args->size() > 1 && strcmp((char *)args->get(1), "mine") == 0)
    list = &state->player.lands;
  else
    list = &state->locations;
  
  scenterf(buf, 24, "name");
  scenterf(buf2, 74, "owner");
  printf("  id | %s | %s\n\n", buf, buf2);
  for (int i = 0; i < list->size(); i ++) {
    location = (location_t *) list->get(i);
    owner[0] = 0;
    if (location->owner != NULL)
      snprintf(owner, sizeof(owner), "%s %s %s", location->owner->first_name, location->owner->middle_name, location->owner->last_name);
    scenterf(buf, 24, location->name);     
    scenterf(buf2, 74, owner); 
    printf(" %03d | %s | %s\n", location->id, buf, buf2);
  }
  printf("\n\n");
}

int cmd_here(List *args)
{
  char *name, buf[75], cname[75];
  GList *list;
  character_t *character;
  name = (char *) args->get(0);
  if (strcmp(name, "here"))
    return 0;
  if (!state) {
    printf("No game started\n");
    return 1;
  }
  list = &state->player.location->characters_in_here;
  scenterf(buf, 74, "name");
  printf("  id | %s | rank\n\n", buf);
  for (int i = 0; i < list->size(); i ++) {
    character = (character_t *) list->get(i);
    snprintf(cname, sizeof(cname), "%s %s %s", character->first_name, character->middle_name, character->last_name);
    scenterf(buf, 74, cname); 
    printf(" %03d | %s | %03d \n", character->id, buf, character->rank);
  }
  printf("\n\n");
}

void cmd_create_land(List *args, int i)
{
  char *arg;
  int where;
  if (args->size() <= i)
    printf("create land: Missing argument\n");
  arg = (char *)args->get(i);
  if (!strcmp(arg, "north"))
    where = DIR_NORTH;
  else if (!strcmp(arg, "south"))
    where = DIR_SOUTH;
  else if (!strcmp(arg, "east"))
    where = DIR_EAST;
  else if (!strcmp(arg, "west"))
    where = DIR_WEST;
  else
    printf("create land: Invalid argument %s\n", arg);
  state->create_location(NULL, NULL, where);
}

int cmd_create(List *args)
{
  char *name;
  name = (char *) args->get(0);
  if (strcmp(name, "create"))
    return 0;
  if (!state) {
    printf("No game started\n");
    return 1;
  }
  if (args->size() > 1 && strcmp((char *)args->get(1), "land") == 0)
    cmd_create_land(args, 2);
  return 1;
}

int cmd_go(List *args)
{
  char *name, *arg;
  int where;
  name = (char *) args->get(0);
  if (strcmp(name, "go"))
    return 0;
  if (args->size() < 2) {
    printf("go: Missing argument\n");
    return 0;
  }
  arg = (char *)args->get(1);
  if (!strcmp(arg, "north"))
    where = DIR_NORTH;
  else if (!strcmp(arg, "south"))
    where = DIR_SOUTH;
  else if (!strcmp(arg, "east"))
    where = DIR_EAST;
  else if (!strcmp(arg, "west"))
    where = DIR_WEST;
  else {
    printf("go: Invalid argument %s\n", arg);
    return 0;
  }
  state->go_to(where);
}

int cmd_kill(List *args)
{
  char *name;
  character_t *character;
  name = (char *) args->get(0);
  if (strcmp(name, "kill"))
    return 0;
  if (!state) {
    printf("No game started\n");
    return 1;
  }
  if (args->size() > 1)
    character = search_for_character((char *) args->get(1));
  else if (state->player.target) 
    character = state->player.target;
  else
    character = state->player.last_attacker;
  
  if (character == NULL)
    return 1;
  state->kill(character);
}

#define NR_COMMANDS 10
command_f commands[] = { cmd_exit, cmd_start, cmd_save, cmd_load, cmd_stats, cmd_lands, cmd_here, cmd_create, cmd_go, cmd_kill };

void format_prompt(char * prompt, int size)
{
  if (state != NULL)
    snprintf(prompt, size, "%03d|%03d|%03d @ %s > ", state->round, state->player.rank, state->player.life, (state->player.location != NULL ? state->player.location->name : "--"));
  else
    snprintf(prompt, size, " > ");
}

int main()
{
  int i;
  char *input, prompt[100];
  GList args;
  /*  if (load_saved_state("test.svd", NULL))
      exit(1);*/
  srand(time(NULL));
  while(1) {
    format_prompt(prompt, 100);
    input = readline(prompt);
    if (input == NULL)
      break;
    if (is_empty(input))
      continue;
    add_history(input);
    tokenizer(input, &args);
    for (i = 0; i < args.size(); i++)
      printf("arg %d: %s\n", i, args.get(i));
    if (args.size()) {
      for (i = 0; i < NR_COMMANDS && !commands[i](&args); i++);
    }
    args.clear();
    free(input);
  }
}
