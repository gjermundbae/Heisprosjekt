/*
@file
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"
#include "state.h"

int orders_scanUp(int floor,  struct State* currentState);
int orders_scanDown(int floor,  struct State* currentState);
int orders_isEmpty(struct State* currentState);
void orders_reset(struct State* currentState);
void orders_updateOrders(struct State* currentState);
void orders_checkPeripheralOrders(struct State* currentState, int floor );//consider namechange

void routine_startMotor(struct State* currentState);
void routine_stop(struct State* currentState);
void routine_arrival(struct State* currentState, int currentFloor);
void checkTimerAndCloseDoor(struct State* currentState);
void clear_all_order_lights();
void initialize_State(struct State* currentState, int waitingtime);
void initialize_Hardware();
void checkDirectionAfterStop(struct State* currentState);// Only haoppens when the first order after Stop mode is handled
void checkFloorSensors(struct State* currentState);//Consider namechange
void keepDoorOpen(struct State* currentState);//Consider namechange

int checkForHalt(struct State* currentState);