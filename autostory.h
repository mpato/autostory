#ifndef AUTOSTORY_H
#define AUTOSTORY_H

struct character;
struct location;
struct nation;

struct nation
{
  character *ruler;
  location *lands;
}

struct location
{
  character *owner;
  int size;
  location *neighboors;
}

struct relationship
{
  character *a, *b;
  int type;
}

struct character
{
  int life;
  int rank;
  int strength;
  int age;
  int gender;
  location *lands;
  nation *nation;
  relationship *relationships;
}
#endif
