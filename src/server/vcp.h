/**
 * File:        vcp.h
 * Author:      Justin Lewis Salmon
 * Student ID:  10000937
 * Created on:  28 February 2012
 * 
 * Defines constants, structs, function definitions and global variables 
 * for the virtual coffee pot.
 */

#include <time.h>

#ifndef VCP_H_
#define VCP_H_

#define STATE_READY	0
#define STATE_BREWING   1
#define STATE_POURING   2
#define STATE_WAITING   3
#define STATE_OFF       4
#define NUM_STATES 	4

#define EVENT_BREW	0
#define EVENT_STOP	1
#define EVENT_POUR	2
#define EVENT_COLLECT	3
#define NUM_EVENTS	4

#define NUM_POTS 	5
#define TEAPOT          5

#define E_INVALID_ADDS  406
#define E_TEAPOT        418
#define E_OFF           419
#define E_BUSY          420
#define E_STILL_BREWING 421
#define E_STILL_POURING 422
#define E_ALRDY_BREWING 423
#define E_ALRDY_POURING 424
#define E_NOT_POURING   425
#define E_CUP_WAITING   426
#define E_NO_CUP        427
#define E_WAITING_ADDS  428
#define E_OVERFLOW      504
#define E_CUP_COLD      505

#define VAL_ADDS_STR    \
"Milk types:\tCream, Half-and-half, Whole-milk, Part-skim, Skim, Non-dairy\n\
Syrup types:\tVanilla, Almond, Raspberry\n\
Sweeteners:\tWhite-sugar, Sweetener, Raw-cane, Honey\n\
Spice types:\tCinnamon, Cardamom\n\
Alcohol types:\tBrandy, Rum, Whiskey, Aquavit, Kahlua\n\
Volume units:\t[1-5], dash, splash, little, medium, lots\n\
\t\tOr specify no quantity to have the pot pour a custom amount.\n"

#define VAL_ADDS_ARR    \
{"Cream", "Half-and-half", "Whole-milk", "Part-skim", "Skim", "Non-dairy", \
"Vanilla", "Almond", "Raspberry", \
"White-sugar", "Sweetener", "Raw-cane", "Honey", \
"Cinnamon", "Cardamom", \
"Brandy", "Whiskey", "Rum", "Aquavit", "Kahlua", \
"dash", "splash", "little", "medium", "lots", "1", "2", "3", "4", "5", "unspecified"}
#define VAL_ADDS_ARR_LEN 31
#define BEVERAGE        \
"                 ) (\n\
                (    )\n\
               ____(___ \n\
            _|`--------`| \n\
           (C|          |__ \n\
         /` `\\          /  `\\ \n\
         \\    `========`    / \n\
          `'--------------'`\n"

#define BREWING_TIME    20
#define POURING_TIME    10
#define T_TO_COLD       60
#define MAX_ADDS        5

#define TRUE            1
#define FALSE           0
#define SUCCESS         0
#define FAILURE         1

typedef void (*tfp) ();

typedef struct {
    int next_state;
    int error;
    tfp action;
} pot_state_table;

/* A single VCP */
typedef struct {
    int pot_id;
    int current_state;
    int waiting_adds;
    pthread_t current_thread;
    time_t brew_end_time;
    time_t pour_end_time;
    char* adds;
    pot_state_table states[NUM_STATES][NUM_EVENTS];
} pot_struct;

pot_struct pots[NUM_POTS];
char buf[1024];

int propfind(pot_struct*, char*);
int brew(pot_struct*, char*);
int get(pot_struct*, char*, char*);
int pour(pot_struct*);
int when(pot_struct*);

void brewing_action(pot_struct*);
void pouring_action(pot_struct*);
void waiting_action(pot_struct*);
void ready_action(pot_struct*);
void off_action(pot_struct*);
void null_action();

void brew_alarm(int);
void pour_alarm(int);

int validate_adds(char*);
int valid_add(char*);
void format_adds(char*);
char *replace(char*, char*, char*);
char *get_state_str(int);
void init_pot(pot_struct*, int);
void calc_etc(char*, pot_struct*);

#endif /* VCP_H_ */
