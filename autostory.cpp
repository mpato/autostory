#include <stdio.h>
#include <strings.h>
#include <math.h>

#include "autostory.h"
const char *names[][2] = {{"Miguel", "Salome"}, {"Joao", "Joana"}, {"Jorge", "Jorgina"}, {"Carlos", "Carla"}, {"Tiago", "Tatiana"}, {"Ibo", "Ibonne"},
			  {"Andre", "Andreia"}, {"Basco", "Basca"}, {"Orlando", "Orlanda"}, {"Joost", "Justa"}, {"Paulo", "Paula"}, {"Ricardo", "Ricardina"},
			  {"John", "Jane"}, {"Jack", "Jaqueline"}, {"Rodrigo", "Ranica"}, {"Matias", "Matilde"}/*seriously?*/, {"Karl", "Karlina"}, 
			  {"Frederik", "Frida"}, {"Jonathan", "Jannie"}, {"Pedro", "Petra"}, {"Peter", "Pita"}, {"Eugenio", "Eugenia"}, {"Mario", "Maria"}, 
			  {"Isofeio", "Isabel"}, {"Rauk", "Rute"}, {"Censored", "Ana"}, {"Emm","Emma"}, {"Al", "Alsina"}, {"Ruk", "Rita"}, {"Ybo","Ybonne"}, 
			  {"Mariok", "Mariana"}, {"Saul", "Sara"}, {"Antonio", "Anabela"}, {"Bauk", "Barbara"}, {"Bamus", "Banessa"}, {"Otto", "Olga"},
			  {"Neil", "Natasha"}, {"Elliot", "Ellen"}, {"Judie", "Judie"}, {"Julio", "Julia"}, {"Bictor", "Bictoria"}};
const char *name_starts[] = {"", "", "", "", "", "Mac", "Van ", "Von ", "da ", "do ", "de ", "dos ", "O'", "e ", "y ", "Uk ", "der ", "Van der", "del ", "do", "De", 
			     "D'A", "D'E", "D'O"};
const char *name_parts[] = {"aber", "smith", "maud", "hotten", "bell", "oliv", "eira", "per", "silva", "carra", "jola", "beth", "neves", "casius", "miranda", "balcus", 
			    "ramm", "stein", "ivan", "morgan", "aba", "cal", "santos", "ma", "ria", "pat", "o", "a", "an", "tu", "nes", "go", "mes", "mez", "ra", "mi", "rez",
			    "sou", "cou", "to"};
const char *location_type_names[] = {"Hall of %s", "Room of %s", "%s street", "%s road", "%s plaza", "%s square", "%s place", "%s market", "%s boulevard", 
				     "%s avenue", "%s terrace", "%s park", "%s piaza"};
const char *location_name_parts[] = {"south", "north", "east", "west", "berry", "ox", "shire", "peter", "lane", "on", "bridge", "stour", "cram", "ham", "brih", "land",
                                     "ivan", "malcata", "porto", "porto", "maia", "jorge", "ferreirinha", "balera", "avon", "new", "town", "field", "balcus", "stein", "burg"};

int my_rand(int max)
{
  return (rand() / (double) RAND_MAX) * max;
}

/*int tt[4];
int test_rand()
{
  double total;
  tt[0] = 0;
  tt[1] = 0;
  tt[2] = 0;
  tt[3] = 0;
  for (int i = 0; i < 10000; i++)
    tt[my_rand(4)] ++;

  total = tt[0] + tt[1] + tt[2] + tt[3];
  printf("0 %lf\n", tt[0] / total);
  printf("1 %lf\n", tt[1] / total);
  printf("2 %lf\n", tt[2] / total);
  printf("3 %lf\n", tt[3] / total);
}
*/

int get_progress_limit(int rank)
{
  return (int) pow(1.5, rank) + 10;
}

void get_random_character_first_name(char *name, int size, int gender)
{
  int i;
  i = my_rand(sizeof(names) / sizeof(char *) / 2);
  strncpy(name, names[i][gender], size);
  name[size - 1] = '\0';
}

void get_random_character_name(char *name, int size, int with_starts)
{
  int i;
  int tsize, tstart, tmp;
  name[0] = 0;
  tsize = 1; //null terminator
  if (with_starts) {
    i = my_rand(sizeof(name_starts) / sizeof(char *));
    strncat(name, name_starts[i], size - tsize);
    tstart = strlen(name_starts[i]);
    tsize += tstart; 
  } else
    tstart = 0;
  while (tsize < size) {
    i = my_rand(sizeof(name_parts) / sizeof(char *));
    strncat(name, name_parts[i], size - tsize);
    tsize += strlen(name_parts[i]); 
    tsize += my_rand(20);
    printf("tsize: %d\n", tsize);
  }
  if (tstart < size - 1 && (tstart - 1 < 0 || IS_CAPITAL_LETTER(name[tstart - 1])))
    name[tstart] += ('A' - 'a');
  printf("\n");
}

void get_random_location_name(char *name, int size)
{
  int i1, i2;
  int tsize, tmp;
  char tname[size];
  //test_rand();
  tname[0] = 0;
  i1 = my_rand(sizeof(location_type_names) / sizeof(char *));
  tsize = strlen(location_type_names[i1]) + 1; //null terminator
  while (tsize < size) {
    i2 = my_rand(sizeof(location_name_parts) / sizeof(char *));
    strncat(tname, location_name_parts[i2], size - tsize);
    tsize += strlen(location_name_parts[i2]); 
    tsize += my_rand(10);
  }
  tname[0] += ('A' - 'a');
  snprintf(name, size, location_type_names[i1], tname);
}

location_t *state_t::disconnect_location(location_t *a, int direction)
{
  location_t *b;
  b = a->neighboors[direction];
  a->neighboors[direction] = NULL;
  if (b != NULL)
    b->neighboors[3 - direction] = NULL;
}

location_t *state_t::connect_locations(location_t *a, int direction, location_t *b)
{
  disconnect_location(a, direction);
  a->neighboors[direction] = b;
  disconnect_location(b, 3 - direction);
  b->neighboors[3 - direction] = a;
}

location_t *state_t::create_location(char *name, char *description, character_t *owner)
{
  location_t *location;
  
  location = new location_t();
  if (name == NULL)
    get_random_location_name(location->name, 25);
  else
    strcpy(location->name, name);
  if (description != NULL)
    strcpy(location->description, description);
  location->owner = owner;
  if (owner)
    owner->lands.add(location);
  locations.add_new(location);
  return location;
}

location_t *state_t::create_location(char *name, char *description, location_t *src, int where, character_t *owner)
{
  location_t *location;
  if (where < 0 || where >= 4 || src == NULL)
    return NULL;
  location = create_location(name, description, owner);
  if (location != NULL)
    connect_locations(src, where, location);
  return location;
}

int state_t::create_location(char *name, char *description, int where)
{
  location_t *ret;
  if (player.location->owner != &player)
    return 0;
  ret = create_location(name, description, player.location, where, &player);
  populate(ret, player.rank * 2, 10);
  end_round();
  return ret != NULL;
}

character_t *state_t::create_character(char *first_name, char *middle_name, char *last_name, int rank)
{
  character_t *character;
  
  character = new character_t();
  character->gender = my_rand(2);
  if (first_name == NULL)
    get_random_character_first_name(character->first_name, sizeof(character->first_name), character->gender);
  else
    strcpy(character->first_name, first_name);
  if (middle_name == NULL)
    get_random_character_name(character->middle_name, sizeof(character->middle_name), 0);
  else
    strcpy(character->middle_name, middle_name);
  if (last_name == NULL)
    get_random_character_name(character->last_name, sizeof(character->last_name), 1);
  else
    strcpy(character->last_name, last_name);
  if (rank < 0)
    rank = 10;
  character->rank = my_rand(rank);
  character->strength = 100 * character->rank + my_rand(100);
  character->defense = 100 * character->rank + my_rand(100);
  characters.add_new(character);
  return character;
}

void state_t::populate(location_t *location, int rank, int number)
{
  character_t *character;
  for(int i = 0; i < number; i++) {
    character = create_character(NULL, NULL, NULL, rank);
    if (character != NULL)
      go_to(character, location);
  }    
}

void state_t::end_round()
{
  character_t *character;
  int limit;
  round ++;
  for (int i = 0; i < characters.size(); i++) {
    character = (character_t *) characters.get(i);
    if (character != &player && character->last_attacker != NULL)
	kill(character, character->last_attacker);
  }
  for (int i = 0; i < characters.size(); i++) {
    character = (character_t *) characters.get(i);
    if (!character->life)
      continue;
    character->age ++;    
    limit = get_progress_limit(character->rank);
    if (character->progress >= limit) {
      character->rank ++;
      character->strength += 50 + my_rand(50);
      character->defense += 50 + my_rand(50);
      character->life += 25 + my_rand(25);
      character->progress -= limit;
    }  
  }
}

void state_t::start(character_t &character)
{
  location_t *location;
  player = character;
  player.strength = 100;
  player.defense = 100;
  characters.add_new(&player);
  location = create_location(NULL, NULL, &player);
  go_to(&player, location);
  end_round();
}

void state_t::go_to(character_t *character, location_t *location)
{
  if (character->location != NULL)
    character->location->characters_in_here.remove(character);
  if (location != NULL)
    location->characters_in_here.add(character);
  character->location = location;
}

void state_t::go_to(int where)
{
  if (where < 0 || where >= 4 || player.location->neighboors[where] == NULL) {
    event_listener->on_cannot_go(where);
    return;
  }
  go_to(&player, player.location->neighboors[where]);
  end_round();
}

int state_t::is_valid_target(character_t *attacker, character_t *target)
{
  return target->life && target->location == attacker->location;
}

void state_t::kill(character_t *attacker, character_t *target)
{
  int defense, strength, damage;
  if (target == NULL || attacker == NULL  || target == attacker)
    return;
  strength = my_rand(attacker->strength);
  defense = my_rand(target->defense);
  target->last_attacker = attacker;
  damage = MAX(strength - defense, 0);
  target->life = MAX(target->life - damage, 0);
  event_listener->on_attack(attacker, target, damage);
  if (target->life == 0) {
    attacker->target = NULL;
    attacker->progress += target->rank;
    go_to(target, NULL);
    event_listener->on_kill(attacker, target);
  } else
    attacker->target = target;
}

void state_t::kill(character_t *target) 
{
  if (!is_valid_target(&player, target))
    return;
  kill(&player, target);
  end_round();
}

void get_alignment_name (char *ret, int size, int align)
{
  switch(align) {
  case ALIGN_GOOD:
    strncpy(ret, "GOOD", size);
    break;
  case ALIGN_NEUTRAL:
    strncpy(ret, "NEUTRAL", size);
    break;
  case ALIGN_EVIL:
    strncpy(ret, "EVIL", size);
    break;
  }
  ret[size - 1] = '\0';
}
