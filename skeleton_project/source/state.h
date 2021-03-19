/**
 * @file
 * @brief States for the finite state machine
 */

#include <time.h>

/**
 * @struct State
 * 
 * @brief A struct with members that hold information about the elevators state.
 * 
 * fsm_floor is the last floor of the elevator, form 0 to 3
 * 
 * fsm_stop is true when in stop-mode and false if not
 * 
 * fsm_door is true when the door is open, and false when closed 
 * 
 * fsm_direction is the current direction of the elevator;
 * 1 when it is going down and 0 when going up
 * 
 * fsm_orders is the orderbook; a matrix that keeps track of the orders, 
 * so a 1 indicates an unattended order, and a 0 means no orders.
 * The first column indicates the down-order buttons outside the elevator, 
 * the second column indicates the up-order buttons outside the elevator and 
 * the third column is the order buttons inside the elevator.
 * Further indicates the first row the first floor, 
 * and so on to the last row, which indicates the top floor. 
 * 
 * fsm_ignoreAllOrdes When active, the elevator ignores all orders pending the arrival at the first floor 
 * Reset after arrival at the first floor
 * 
 * fsm_startTime is the time when the door timer starts
 * 
 * fsm_waitingTime is the door waiting-/countdowntime 
 */
struct State{
    int fsm_floor;
    int fsm_stop;
    int fsm_door;
    int fsm_direction;
    int fsm_orders[HARDWARE_NUMBER_OF_FLOORS][HARDWARE_NUMBER_OF_BUTTONS];
    int fsm_ignoreAllOrdes;
    clock_t fsm_startTime;
    clock_t fsm_waitingTime;
};
