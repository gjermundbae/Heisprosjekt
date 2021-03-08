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
void updateOrders(struct State* currentState);
void checkPeripheralOrders(struct State* currentState, int floor, clock_t * timer_startTime);
void arrivalRoutine(struct State* currentState, int currentFloor);
void checkTimerAndCloseDoor(struct State* currentState, clock_t * timer_start, clock_t * timer_wait);