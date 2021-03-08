/*
@file
*/


#include <time.h>
#include "hardware.h"
#include "state.h"

int scanUp(int floor,  struct State S);
int scanDown(int floor,  struct State S);
int order_isEmpty(struct State S);
void resetOrders(struct State* S);
//void openDoor(struct State* S);
//void stopLoop(struct State* S, clock_t* startTime);
void stopRoutine(struct State* S, clock_t * timer_startTime);

