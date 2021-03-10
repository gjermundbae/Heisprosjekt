#include "utilities.h"

int orders_scanUp(int floor, struct State* currentState){
    for( int f = floor ; f < HARDWARE_NUMBER_OF_FLOORS-1 ; f++){
        if (currentState->fsm_orders[f+1][0] | currentState->fsm_orders[f+1][1] | currentState->fsm_orders[f+1][2] ){
            return 1;
        }
    }
    return 0;
}

int orders_scanDown(int floor, struct State* currentState){
    for( int f = 0 ; f < floor ; f++){
        if (currentState->fsm_orders[f][0]  | currentState->fsm_orders[f][1]  | currentState->fsm_orders[f][2] ){
            return 1;
        }
    }
    return 0;
}

int orders_isEmpty(struct State* currentState){
    for(int f = 0 ; f < HARDWARE_NUMBER_OF_FLOORS ; f++){
        if (currentState->fsm_orders[f][0]  | currentState->fsm_orders[f][1]  | currentState->fsm_orders[f][2] ){
            return 0;
        }
    }
    return 1;
}
void orders_reset(struct State* currentState){
    for(int f = 0 ; f < HARDWARE_NUMBER_OF_FLOORS ; f++){
        currentState->fsm_orders[f][0]=0;
        currentState->fsm_orders[f][1]=0;
        currentState->fsm_orders[f][2]=0;
        
    }
}


void orders_update(struct State* currentState){
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

void orders_checkPeripherals(struct State* currentState, int floor){
    switch (currentState->fsm_direction){
        case DIRECTION_DOWN:
            if(!scanDown(floor, currentState) & !(currentState->fsm_door)){
                hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                currentState->fsm_direction = DIRECTION_UP;
                if(currentState->fsm_orders[floor][1]){
                    hardware_command_door_open(1);
                    currentState->fsm_door = 1;
                    currentState->timer_startTime = clock();
                    currentState->fsm_orders[floor][1] = 0;
                    hardware_command_order_light(floor, HARDWARE_ORDER_UP, 0);
                }    
        
            }
            break;

        case DIRECTION_UP:
            if(!scanUp(floor, currentState) & !currentState->fsm_door){
                hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                currentState->fsm_direction = DIRECTION_DOWN;
                if(currentState->fsm_orders[floor][0]){
                    hardware_command_door_open(1);
                    currentState->fsm_door = 1;
                    currentState->timer_startTime = clock();
                    currentState->fsm_orders[floor][0] = 0;
                    hardware_command_order_light(floor, HARDWARE_ORDER_DOWN, 0);
                }
            }
            break;
    }
}

void routine_stop(struct State* currentState){ 
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    hardware_command_stop_light(1);
    clear_all_order_lights();
    resetOrders(currentState);
    currentState->fsm_stop = 1;
    for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
        if(hardware_read_floor_sensor(f)){
            hardware_command_door_open(1);
            currentState->fsm_door = 1;
            currentState->timer_startTime = clock();
        }
    }
}

void routine_arrival(struct State* currentState, int currentFloor){
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    hardware_command_order_light(currentFloor, HARDWARE_ORDER_DOWN, 0);
    hardware_command_order_light(currentFloor, HARDWARE_ORDER_UP, 0);
    hardware_command_order_light(currentFloor, HARDWARE_ORDER_INSIDE, 0);
    hardware_command_door_open(1);
    currentState->fsm_door = 1;
    currentState->fsm_floor = currentFloor;
    currentState->fsm_orders[currentFloor][0] = 0;
    currentState->fsm_orders[currentFloor][1] = 0;
    currentState->fsm_orders[currentFloor][2] = 0;
}

void routine_startMotor(struct State* currentState){
    if (!order_isEmpty(currentState) & !(currentState->fsm_door)){
        if(currentState->fsm_direction == DIRECTION_UP){
            hardware_command_movement(HARDWARE_MOVEMENT_UP);
        }
        else{
            hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
        }
    }
}

void handler_closeDoor(struct State* currentState){
    if((((double)(clock() - currentState->timer_startTime))/ CLOCKS_PER_SEC >= currentState->timer_waitingTime) & currentState->fsm_door){
                currentState->fsm_door = 0;
                hardware_command_door_open(0);
        }
}

void handler_firstOrderAfterStop(struct State* currentState){
    if(currentState->fsm_stop & !order_isEmpty(currentState)){
        currentState->fsm_stop=0;
        switch (currentState->fsm_direction){
            case (DIRECTION_DOWN):
                if (!scanDown(currentState->fsm_floor, currentState)){
                    currentState->fsm_direction = DIRECTION_UP;
                    currentState->fsm_floor--;
                }
                break;
            
            case (DIRECTION_UP):
                if (!scanUp(currentState->fsm_floor, currentState)){
                    currentState->fsm_direction = DIRECTION_DOWN;
                    currentState->fsm_floor++;
                }
                break;   
        }
    }
}
void hanlder_floorSensors(struct State* currentState){
    for(int currentFloor = 0; currentFloor < HARDWARE_NUMBER_OF_FLOORS; currentFloor++){
        if(hardware_read_floor_sensor(currentFloor)){
            // Setting the new floor
            currentState->fsm_floor = currentFloor;
                //Flipping the direction if the elevator is at the end floors 
            switch (currentFloor){
                case (0):
                    currentState->fsm_direction = DIRECTION_UP;
                    break;

                case (HARDWARE_NUMBER_OF_FLOORS-1):
                    currentState->fsm_direction = DIRECTION_DOWN;
                    break;
            }

            //Checking if the elevator should take a halt at the current floor
            switch (check_halt(currentState)){
            case (1):
                arrivalRoutine(currentState, currentFloor);
                currentState->timer_startTime = clock();
                break;
            case (0):
                checkPeripherals(currentState, currentFloor);
                break;
            }             
            hardware_command_floor_indicator_on(currentFloor);
        }
    }
}

void handler_keepDoorOpen(struct State* currentState){
    if( (hardware_read_stop_signal() | hardware_read_obstruction_signal()) & currentState->fsm_door){
        hardware_command_door_open(1);
        currentState->fsm_door = 1;
        currentState->timer_startTime = clock();
    }
}

void initialize_clearAllOrderLights(){
    HardwareOrder order_types[3] = {
        HARDWARE_ORDER_UP,
        HARDWARE_ORDER_INSIDE,
        HARDWARE_ORDER_DOWN
    };

    for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
        for(int i = 0; i < 3; i++){
            HardwareOrder type = order_types[i];
            hardware_command_order_light(f, type, 0);
        }
    }
}
void initialize_State(struct State* currentState){
    currentState->fsm_floor = HARDWARE_NUMBER_OF_FLOORS;
    currentState->fsm_door = 0;
    currentState->fsm_direction = DIRECTION_DOWN;
    resetOrders(currentState);
    currentState->fsm_orders[0][2] = 1;
    currentState->fsm_resetElevator = 0;
    currentState->fsm_stop = 0;
    currentState->timer_startTime = 0;
    currentState->timer_waitingTime = HARDWARE_WAITINGTIME;
}

void initialize_Hardware(){
    int error = hardware_init();
    if(error){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }
}

int check_halt(struct State* currentState){
    int halt = 0;
    if ( (currentState->fsm_direction == DIRECTION_DOWN) & currentState->fsm_orders[currentState->fsm_floor][0] ){
        halt = 1;
    }
    else if( (currentState->fsm_direction == DIRECTION_UP) & currentState->fsm_orders[currentState->fsm_floor][1] ){
        halt = 1;
    }
    else if( currentState->fsm_orders[currentState->fsm_floor][2] ){
        halt = 1;
    }
    else if( currentState->fsm_door & ( currentState->fsm_orders[currentState->fsm_floor][0] | currentState->fsm_orders[currentState->fsm_floor][1]) ){
        halt = 1;
    }
    return halt;
}