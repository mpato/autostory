#ifndef AUTOSTORY_H
#define AUTOSTORY_H

struct character_t;
struct location_t;
struct nation_t;

struct nation_t
{
  int id;
  character_t *ruler;
  location_t *lands;
};

struct location_t
{
  int id;
  character_t *owner;
  int size;
  location_t *neighboors;
};

struct relationship_t
{
  character_t *a, *b;
  int type;
};

struct character_t
{
  int id;
  int life;
  int rank;
  int strength;
  int age;
  int gender;
  location_t *lands;
  nation_t *nation;
  relationship_t *relationships;
  char first_name[25];
  char middle_name[25];
  char last_name[50];
};

struct state_t {
  character_t *characters;
  nation_t *nations;
  location_t *locations;
};

#endif
