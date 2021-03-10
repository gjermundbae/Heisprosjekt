/*
@file

*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"
#include "state.h"

/**
 * @brief Searches through all orders above the last floor visited in the @p currentState
 *
 * @param currentState current state of the elevator
 *
 * @return 1 if there exists an order above the current floor
 */
int orders_scanUp(struct State* currentState);

/**
 * @brief Searches through all orders under the last floor visited in the @p currentState
 *
 * @param currentState current state of the elevator
 *
 * @return 1 if there exists an order under the current floor
 */
int orders_scanDown(struct State* currentState);

/**
 * @brief Searches through the current orderbook in the @p currentState
 *
 * @param currentState current state of the elevator
 *
 * @return 1 if the orderbook is empty
 */
int orders_isEmpty(struct State* currentState);

/**
 * @brief Sets all elements in the orderbook of the @p currentState to 0
 *
 * @param currentState current state of the elevator
 *
 */
void orders_reset(struct State* currentState);

/**
 * @brief Checks if any of the order buttons is pressed and sets the corresponding element 
 * in the orderbook of the @p currentState to 1
 *
 * @param currentState current state of the elevator
 */
void orders_update(struct State* currentState);

/**
 * @brief Checks for additional orders in the direction of the @p currentState .
 * If there is no more orders in the perpheral and the door is closed, the
 * direction of the @p currentState is flipped. 
 * If there is an active order in the current floor heading
 * in the new direction, the routine_arrival function is called
 *
 * @param currentState current state of the elevator
 */
void orders_checkPeripheralOrders(struct State* currentState);//consider namechange

/**
 * @brief Starts the motor in the direction of the @p currentState provided
 * that the door is closed and that there exists an order
 *
 * @param currentState current state of the elevator
 */
void routine_startMotor(struct State* currentState);


void routine_stop(struct State* currentState);
void routine_arrival(struct State* currentState);

void initialize_State(struct State* currentState, int waitingtime);
void initialize_hardware();
void initialize_clearAllOrderLights();

void handler_closeDoor(struct State* currentState);
void handler_keepDoorOpen(struct State* currentState);//Consider namechange
void handler_firstOrderAfterStop(struct State* currentState);// Only haoppens when the first order after Stop mode is handled
void handler_floorSensors(struct State* currentState);//Consider namechange

int check_halt(struct State* currentState);