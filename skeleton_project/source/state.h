/**
 * @file
 * @brief Driver for the elevator hardware.
 */
#include <time.h>
#define DIRECTION_UP 0
#define DIRECTION_DOWN 1
struct State{
    int fsm_floor;
    int fsm_goal;
    int fsm_stop;
    int fsm_door;
    int fsm_direction;
    int fsm_orders[HARDWARE_NUMBER_OF_FLOORS][3];
    int fsm_resetElevator;
    clock_t timer_startTime;
    clock_t timer_waitingTime;
    
};
