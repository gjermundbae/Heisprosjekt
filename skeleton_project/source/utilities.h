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
void orders_update(struct State* currentState);
void orders_checkPeripheralOrders(struct State* currentState, int floor );//consider namechange

void routine_startMotor(struct State* currentState);
void routine_stop(struct State* currentState);
void routine_arrival(struct State* currentState, int currentFloor);

void initialize_State(struct State* currentState, int waitingtime);
void initialize_Hardware();
void initialize_clearAllOrderLights();

void handler_closeDoor(struct State* currentState);
void handler_keepDoorOpen(struct State* currentState);//Consider namechange
void handler_firstOrderAfterStop(struct State* currentState);// Only haoppens when the first order after Stop mode is handled
void handler_floorSensors(struct State* currentState);//Consider namechange

int check_Halt(struct State* currentState);