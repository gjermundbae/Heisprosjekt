/**
 * @file
 * @brief States for the elevator .
 */

#include <time.h>
#define DIRECTION_UP 0
#define DIRECTION_DOWN 1

/**
 * @brief A struct with members that hold information about the elevtaors state.
 * 
 * @var fsm_floor is the current/last florr of the elevator, form 0 to 3
 * 
 * @var fsm_stop is 1 when in stop-mode and 0 if not
 * 
 * @var fsm_door is 1 when the door is open, and 0 when closed
 * 
 * @var fsm_direction is the current direction of the elevator;
 * 1 when it is going down and 0 when going up
 * 
 * @var fsm_orders is the orderbook; a matrix that keeps track of the orders, 
 * so a 1-element indicates an unattended order, and a 0 means no orders.
 * The fist column indicates the down-order buttons outside the elevator, 
 * the second column the up-order buttons outside the elevator and 
 * the third coulumn is the order buttons inside the elevator.
 * Further indicates the fist row the first floor, 
 * and so on to the last row, which indicates the top floor. 
 * 
 * @var fsm_ignoreAllOrdes is 1 when we want to ignore all orders ie from when the elevator 
 * first start to when it first comes to the fisrt floor. 0 when we want to recieve orders.
 * 
 * @var timer_startTime is the time when we want to start a countdown 
 * 
 * @var timer_waitingTime is the waiting-/countdowntime 
 * (which is 3 secs for all use in this project)
 */

struct State{
    int fsm_floor;
    int fsm_stop;
    int fsm_door;
    int fsm_direction;
    int fsm_orders[HARDWARE_NUMBER_OF_FLOORS][3];
    int fsm_ignoreAllOrdes;
    clock_t timer_startTime;
    clock_t timer_waitingTime;
};
