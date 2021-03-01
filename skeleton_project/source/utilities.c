#include "utilities.h"


int scanUp(int floor, struct State S){
    for( int f = floor ; f < HARDWARE_NUMBER_OF_FLOORS -1 ; f++){
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
