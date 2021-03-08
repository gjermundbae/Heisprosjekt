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
        if (S.fsm_orders[f][0] == 1 | S.fsm_orders[f][1] == 1 | S.fsm_orders[f][2] == 1){
            return 1;
        }
    }
    return 0;
}

int order_isEmpty(struct State S){
    for(int f = 0 ; f < HARDWARE_NUMBER_OF_FLOORS ; f++){
        if (S.fsm_orders[f][0] == 1 | S.fsm_orders[f][1] == 1 | S.fsm_orders[f][2] == 1){
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
/*
void openDoor(struct State* S){
    for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
            if(hardware_read_floor_sensor(f)){
                S->fsm_floor = f;
                hardware_command_door_open(1);
                S->fsm_door = 1;
}

void stopLoop(struct State* S, clock_t* startTime){
    while(hardware_read_stop_signal()){
        hardware_command_movement(HARDWARE_MOVEMENT_STOP);
        hardware_command_stop_light(1);
        //clear_all_order_lights();
        resetOrders(&S);
        S->fsm_stop = 1;
        for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
            if(hardware_read_floor_sensor(f)){
                hardware_command_door_open(1);
                S->fsm_door = 1;
                *startTime = clock();
            }
        }
    }
}
*/
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