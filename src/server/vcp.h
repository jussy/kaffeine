/*
 * vcp.h
 *
 *  Created on: Feb 28, 2012
 *      Author: jl2-salmon
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

#define E_OFF           -1
#define E_BUSY          -2
#define E_CUP_COLD      -3
#define E_OVERFLOW      -4
#define E_TEAPOT        -5
#define E_STILL_BREWING -6
#define E_STILL_POURING -7
#define E_CUP_WAITING   -8
#define E_NOT_POURING   -9
#define E_NO_CUP        -10
#define E_ALRDY_BREWING -11
#define E_ALRDY_POURING -12

#define VALID_ADDITIONS "Milk types:\tCream, Half-and-half, Whole-milk, Part-skim, Skim, Non-dairy\nSyrup types:\tVanilla, Almond, Raspberry\nSweeteners:\tWhite-sugar, Sweetener, Raw-cane, Honey\nSpice types:\tCinnamon, Cardamom\nAlcohol types:\tBrandy, Rum, Whiskey, Aquavit, Kahlua\nVolume units:\t[1-5], dash, splash, little, medium, lots\n"
#define BEVERAGE        "               ) (\n              (    )\n             ____(___ \n          _|`--------`| \n         (C|          |__ \n       /` `\\          /  `\\ \n       \\    `========`    / \n        `'--------------'`\n"

#define BREWING_TIME    20
#define T_TO_COLD       60
#define T_TO_OVERFLOW   10

#define TRUE            1
#define FALSE           0

typedef void (*tfp) ();

typedef struct {
    int next_state;
    int error;
    tfp action;
} pot_state_table;

typedef struct {
    int pot_id;
    pthread_t current_thread;
    int current_state;
    time_t brew_end_time;
    pot_state_table states[NUM_STATES][NUM_EVENTS];
} pot_struct;

pot_struct pots[NUM_POTS];

int propfind(pot_struct*, char*);
int brew(pot_struct*, char*);
int get(pot_struct*, char*, char*);
int when(pot_struct*);

void brewing_action(pot_struct*);
void pouring_action(pot_struct*);
void waiting_action(pot_struct*);
void ready_action(pot_struct*);
void off_action(pot_struct*);
void null_action();
void init_pot(pot_struct*, int);
void catch_alarm(int);

#endif /* VCP_H_ */
