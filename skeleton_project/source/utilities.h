/**
* @file
* @brief Utility-functions that is used to make decisions regarding 
* the initialization and handling of the fsm, as well as the high level logic of the hardware
* 
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"
#include "state.h"

/**
 * @brief Searches through all orders above the last floor visited
 *
 * @param currentState pointer to the current state of the elevator
 *
 * @return true if there exists an order above the current floor
 */
int orders_scanUp(struct State* currentState);

/**
 * @brief Searches through all orders under the last floor visited
 * 
 * @param currentState pointer to the current state of the elevator
 *
 * @return true if there exists an order under the current floor
 */
int orders_scanDown(struct State* currentState);

/**
 * @brief Searches for true elements in the orderbook of the @p currentState
 *
 * @param currentState pointer to the current state of the elevator
 *
 * @return true if the orderbook is empty
 */
int orders_isEmpty(struct State* currentState);

/**
 * @brief Resets all elements in the orderbook of the @p currentState 
 *
 * @param currentState pointer to the current state of the elevator
 *
 */
void orders_reset(struct State* currentState);

/**
 * @brief Checks if any of the order buttons is pressed and sets the corresponding element 
 * in the orderbook of the @p currentState to 1
 *
 * @param currentState pointer to the current state of the elevator
 */
void orders_update(struct State* currentState);


/**
 * @brief Checks for additional orders in the direction of the @p currentState .
 * If there is no more orders in the peripheral and the door is closed, the
 * direction of the @p currentState is flipped. 
 * If there is an active order in the current floor heading
 * in the new direction, the routine_arrival function is called
 *
 * @param currentState pointer to the current state of the elevator
 */
void orders_checkPeripherals(struct State* currentState);

/**
 * @brief Starts the motor in the direction of the @p currentState provided
 * that the door is closed and that there exists an order
 *
 * @param currentState pointer to the current state of the elevator
 */
void routine_startMotor(struct State* currentState);

/**
 * @brief Excecutes the routine associated with the stop-button being pressed.
 * This involves:
 *  Resetting the orderbook and all order lights
 *  Setting fsm_stop of the @p currentState to 1
 *  Halting the elevator
 *  Opening the doors if the elevator is at a defined floor
 *
 * @param currentState pointer to the current state of the elevator
 */
void routine_stop(struct State* currentState);

/**
 * @brief Excecutes the routine associated with the elevator halting at a floor.
 * This involves:
 * Halting the elevator
 * Opening the door
 * Resetting the orders and order lights on the current floor
 *
 * @param currentState pointer to the current state of the elevator
 */
void routine_arrival(struct State* currentState);

/**
 * @brief Resets all lights realated to the elevator and initializes the input/output ports
 * 
 * @warning Will exit if the initialization of the input/ouput ports fails
 */
void initialize_hardware();

/**
 * @brief Sets all values of @p currentState to the preferred initial values.
 * This involves:
 * Setting fsm_floor to the top floor
 * Setting the fsm_ignoreAllOrders of the @p currentState and setting an order in the first floor, which will send the elevator to the first floor
 * Resetting fsm_stop, fsm_door and fsm_startTime
 * Setting fsm_direction to down
 *
 * @param currentState pointer to the current state of the elevator
 */
void initialize_state(struct State* currentState);

/**
 * @brief Resets all lights related to the elevator
 *
 */
void initialize_clearAllOrderLights();

/**
 * @brief Closes the doors and turns off the door-light if the doors have been open 
 * for more than the fsm_waitingTime of the @p currentState
 *
 * @param currentState pointer to the current state of the elevator
 */
void handler_closeDoor(struct State* currentState);

/**
 * @brief Restarts the door-timer of the @p currentState if the obstruction signal or stop button is
 * active while the doors are open
 *
 * @param currentState pointer to the current state of the elevator
 */
void handler_keepDoorOpen(struct State* currentState);

/**
 * @brief Handles the special case when the first order after a stop-state is recieved
 * This requires extra logic because the elevator might be between floors
 * 
 * @param currentState pointer to the current state of the elevator
 * 
 * @warning The fsm_floor of the @p currentState might be manipulated
 * in order to prevent bugs in the system
 */
void handler_firstOrderAfterStop(struct State* currentState);

/**
 * @brief If there is an active floor sensor, the function check_halt is called to decide whether or not to 
 * take a halt. If so, the halt is excecuted.
 * The floor light of the active floor will be set
 *
 * @param currentState pointer to the current state of the elevator
 */
void handler_floorSensors(struct State* currentState);

/**
 * @brief Compares the current floor, the current direction and the orderbook of the @p currentState
 * to decide whether or not the elevator should take a halt
 *
 * @param currentState pointer to the current state of the elevator
 * 
 * @return true if the elevator should take a halt, false if not
 */
int check_halt(struct State* currentState);