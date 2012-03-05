/*
 * vcp.c
 *
 *  Created on: Feb 27, 2012
 *      Author: jl2-salmon
 */
#include <stdio.h>
#include "vcp.h"

pot_struct pots[NUM_POTS];

void init_pots() {

	int event = 0;
	int last_state = STATE_OFF;

	int inch;

	for (int i = 0; i < NUM_POTS; ++i) {

		pots[i].pot_id = i;
		pots[i].current_state = STATE_READY;

		for (int j = 0; j < NUM_STATES; ++j) {
			for (int k = 0; k < NUM_EVENTS; ++k) {
				pots[i].states[j][k].next_state = STATE_OFF;
				pots[i].states[j][k].action = null_action;
			}
		}

		/* From STATE_OFF, only STATE_READY and STATE_BREWING are valid */
		pots[i].states[STATE_OFF][EVENT_BREW].next_state = STATE_BREWING;
		pots[i].states[STATE_OFF][EVENT_BREW].action = brewing_action;
		pots[i].states[STATE_OFF][EVENT_READY].next_state = STATE_READY;
		pots[i].states[STATE_OFF][EVENT_READY].action = ready_action;
		/* From STATE_READY, valid states are STATE_OFF and STATE_BREWING */
		pots[i].states[STATE_READY][EVENT_STOP].next_state = STATE_OFF;
		pots[i].states[STATE_READY][EVENT_STOP].action = off_action;
		pots[i].states[STATE_READY][EVENT_BREW].next_state = STATE_BREWING;
		pots[i].states[STATE_READY][EVENT_BREW].action = brewing_action;
		/* From STATE_BREWING, both EVENT_OFF and EVENT_POURING are valid */
		pots[i].states[STATE_BREWING][EVENT_STOP].next_state = STATE_OFF;
		pots[i].states[STATE_BREWING][EVENT_STOP].action = off_action;
		pots[i].states[STATE_BREWING][EVENT_POUR].next_state = STATE_POURING;
		pots[i].states[STATE_BREWING][EVENT_POUR].action = pouring_action;
		/* From STATE_POURING, only STATE_READY is valid */
		pots[i].states[STATE_POURING][EVENT_READY].next_state = STATE_READY;
		pots[i].states[STATE_POURING][EVENT_READY].action = ready_action;
	}
}

void off_action() {
	printf("Switching off\n");
}

void brewing_action() {
	printf("Brewing...\n");
}

void pouring_action() {
	printf("Pouring...\n");
}

void ready_action() {
	printf("Ready\n");
}

void null_action() {
	//do nothing
}
