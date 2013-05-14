#ifndef AUTOSTORY_H
#define AUTOSTORY_H

struct character_t;
struct location_t;
struct nation_t;

#define GENDER_MALE 0
#define GENDER_FEMALE 1
#define ALIGN_GOOD 0
#define ALIGN_NEUTRAL 1
#define ALIGN_EVIL 2

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
  int alignment;
  location_t *lands;
  nation_t *nation;
  relationship_t *relationships;
  char first_name[25];
  char middle_name[25];
  char last_name[50];
};

struct state_t {
  int round;
  int max_characters, nr_characters;
  int max_nations, nr_nations;
  int max_locations, nr_locations;
  character_t *characters;
  nation_t *nations;
  location_t *locations;
};

#endif
