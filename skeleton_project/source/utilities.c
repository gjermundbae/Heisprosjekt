#include "utilities.h"



int scanUp(int floor, struct State S){
    for( int f = floor ; f < HARDWARE_NUMBER_OF_FLOORS-1 ; f++){
        if (S.fsm_orders[f+1][0] == 1 | S.fsm_orders[f+1][1] == 1 | S.fsm_orders[f+1][2] == 1){
            return 1;
        }
    }
    return 0;
}

int scanDown(int floor, struct State S){
    for( int f = 0 ; f < floor ; f++){
        if ((S.fsm_orders[f][0] == 1) | (S.fsm_orders[f][1] == 1) | (S.fsm_orders[f][2] == 1)){
            return 1;
        }
    }
    return 0;
}

int order_isEmpty(struct State S){
    for(int f = 0 ; f < HARDWARE_NUMBER_OF_FLOORS ; f++){
        if ((S.fsm_orders[f][0] == 1) | (S.fsm_orders[f][1] == 1) | (S.fsm_orders[f][2] == 1)){
            return 0;
        }
    }
    return 1;
}
void resetOrders(struct State* S){
    for(int f = 0 ; f < HARDWARE_NUMBER_OF_FLOORS ; f++){
        S->fsm_orders[f][0]=0;
        S->fsm_orders[f][1]=0;
        S->fsm_orders[f][2]=0;
        
    }
}

void stopRoutine(struct State* S, clock_t * timer_startTime){
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    hardware_command_stop_light(1);
    resetOrders(S);
    S->fsm_stop = 1;
    for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
        if(hardware_read_floor_sensor(f)){
            hardware_command_door_open(1);
            S->fsm_door = 1;
            *timer_startTime = clock();
        }
    }
}
void updateOrders(struct State* currentState){
        for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
            if(currentState->fsm_resetElevator){
            /* Internal orders */
                if(hardware_read_order(f, HARDWARE_ORDER_INSIDE)){
                    hardware_command_order_light(f, HARDWARE_ORDER_INSIDE, 1);
                    currentState->fsm_orders[f][2] = 1;
                }
            /* Orders going up */
                if(hardware_read_order(f, HARDWARE_ORDER_UP)){
                    hardware_command_order_light(f, HARDWARE_ORDER_UP, 1);
                    currentState->fsm_orders[f][1] = 1;
                }

                /* Orders going down */
                if(hardware_read_order(f, HARDWARE_ORDER_DOWN)){
                    hardware_command_order_light(f, HARDWARE_ORDER_DOWN, 1);
                    currentState->fsm_orders[f][0] = 1;
                }
            }
        }
}
void checkPeripheralOrders(struct State* currentState, int floor, clock_t * timer_startTime){
    switch (currentState->fsm_direction){
        case DIRECTION_DOWN:
            if(!scanDown(floor, *currentState) & !(currentState->fsm_door)){
                hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                currentState->fsm_direction = DIRECTION_UP;
                if(currentState->fsm_orders[floor][1]){
                    hardware_command_door_open(1);
                    currentState->fsm_door = 1;
                    *timer_startTime = clock();
                    currentState->fsm_orders[floor][1] = 0;
                    hardware_command_order_light(floor, HARDWARE_ORDER_UP, 0);
                }    
        
            }
            break;
        case DIRECTION_UP:
            if(!scanUp(floor, *currentState) & !currentState->fsm_door){
                hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                currentState->fsm_direction = DIRECTION_DOWN;
                if(currentState->fsm_orders[floor][0]){
                    hardware_command_door_open(1);
                    currentState->fsm_door = 1;
                    timer_startTime = clock();
                    currentState->fsm_orders[floor][0] = 0;
                    hardware_command_order_light(floor, HARDWARE_ORDER_DOWN, 0);
                }
            }
            break;
    }

    
}
void arrivalRoutine(struct State* currentState, int currentFloor){
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    hardware_command_order_light(currentFloor,HARDWARE_ORDER_DOWN, 0);
    hardware_command_order_light(currentFloor, HARDWARE_ORDER_UP, 0);
    hardware_command_order_light(currentFloor, HARDWARE_ORDER_INSIDE, 0);
    hardware_command_door_open(1);
    currentState->fsm_door = 1;
    currentState->fsm_floor = currentFloor;
    currentState->fsm_orders[currentFloor][0]=0;
    currentState->fsm_orders[currentFloor][1]=0;
    currentState->fsm_orders[currentFloor][2]=0;
}
void checkTimerAndCloseDoor(struct State* currentState, clock_t * timer_start, clock_t * timer_wait){
    if((((double)(clock() - *timer_start))/ CLOCKS_PER_SEC >= *timer_wait) & currentState->fsm_door){
                currentState->fsm_door = 0;
                hardware_command_door_open(0);
        }
}