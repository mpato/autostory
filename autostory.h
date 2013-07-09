#ifndef AUTOSTORY_H
#define AUTOSTORY_H
#include <string.h>
#include "utils.h"

class character_t;
class location_t;
class nation_t;

#define GENDER_MALE 0
#define GENDER_FEMALE 1
#define ALIGN_GOOD 0
#define ALIGN_NEUTRAL 1
#define ALIGN_EVIL 2
#define REL_PARENTHOOD 0
#define REL_MARRIAGE 1
#define DIR_NORTH 0
#define DIR_SOUTH 3
#define DIR_WEST 1
#define DIR_EAST 2

class object_t 
{
 public:
  int id;
};

class OList : public List
{
 public:
  int next_id;
  
  OList() {
    next_id = 1;
  }

  void add_new(object_t *p) {
    if (p == NULL)
      return;
    p->id = next_id++;
    internal_add(p);
  }

 void add(object_t *p) {
    if (p == NULL)
      return;
    if (p->id >= next_id)
      next_id = p->id + 1;
    internal_add(p);
  }
  
  object_t *search_by_id(int id) {
    object_t *o;
    for (int i = 0; i < pos; i++) {
      o = (object_t *) get(i);
      if (o->id == id)
	return o;
    }
    return NULL;
  }

};

class nation_t : public object_t
{
 public:
  char name[25];
  character_t *ruler;
  //location_t *lands;
  nation_t() {
    id = 0;
    name[0] = '\0';
    ruler = NULL;
  }
};

class location_t : public object_t
{
 public:
  char description[100];
  char name[25];
  character_t *owner;
  nation_t *nation;
  int width, height;
  location_t *neighboors[4];
  GList characters_in_here;
  location_t() {
    id = 0;
    name[0] = '\0';
    description[0] = '\0';
    owner = NULL;
    nation = NULL;
    width = 0;
    height = 0;
    memset(neighboors, 0, sizeof(neighboors));
  }
};

class relationship_t
{
 public:
  character_t *a, *b;
  int type;
};

class character_t : public object_t
{
 public:
  int life;
  int rank;
  int progress;
  int strength;
  int defense;
  int age;
  int gender;
  int alignment;
  int money;
  nation_t *nation;
  location_t *location;
  char first_name[25];
  char middle_name[25];
  char last_name[25];
  GList relationships;
  GList lands;
  character_t *target;
  character_t *last_attacker;
  
  character_t() {
    id = 0;
    life = 100;
    rank = 0;
    progress = 0;
    strength = 0;
    defense = 0;
    age = 0;
    gender = 0;
    alignment = ALIGN_NEUTRAL;
    money = 0;
    nation = NULL;
    first_name[0] = '\0';
    middle_name[0] = '\0';
    last_name[0] = '\0';
    target = NULL;
    last_attacker = NULL;
  }  
  
  void get_full_name(char *ret, int size) {
    snprintf(ret, size, "%s %s %s", first_name, middle_name, last_name);
  }
};

#define NPC_STATE_IDLE    0
#define NPC_STATE_ATTACK  1

class npc_t : public character_t {
 public:
  int state;
  npc_t() {
    state = NPC_STATE_IDLE;
  }
  void next_state();
  void run_state();
};

class event_listener_i {
 public:
  virtual void on_attack(character_t *attacker, character_t *target, int damage) = 0;
  virtual void on_kill(character_t *attacker, character_t *target) = 0;
  virtual void on_cannot_go(int direction) = 0;
};

class state_t {
 private:
  event_listener_i *event_listener;
  location_t *connect_locations(location_t *a, int direction, location_t *b);
  location_t *disconnect_location(location_t *a, int direction);
  location_t *create_location(char *name, char *description, character_t *owner);
  location_t *create_location(char *name, char *description, location_t *src, int where, character_t *owner);
  void go_to(character_t *character, location_t *location);
  int is_valid_target(character_t *attacker, character_t *target);
  void kill(character_t *attacker, character_t *target);
  character_t *create_character(char *first_name, char *middle_name, char *last_name, int rank);
  void populate(location_t *location, int rank, int number);
 public:
  state_t(event_listener_i *e) {
    event_listener = e;
  }
  int round;
  OList characters;
  OList nations;
  OList locations;
  character_t player;
  int create_location(char *name, char *description, int where);
  void end_round();
  void start(character_t &player);
  void go_to(int where);
  void kill(character_t *target);
};

void get_alignment_name (char *, int, int);
int get_progress_limit(int rank);
#endif
