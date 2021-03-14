#include "utilities.h"


int orders_scanUp(struct State* currentState){
    int orderFound = 0;

    for( int f = currentState->fsm_floor ; f < HARDWARE_NUMBER_OF_FLOORS-1 ; f++){

        if (currentState->fsm_orders[f+1][0] | currentState->fsm_orders[f+1][1] | currentState->fsm_orders[f+1][2] ){
            orderFound = 1;
        }
    }
    return orderFound;
}


int orders_scanDown(struct State* currentState){
    int orderFound = 0;

    for( int f = 0 ; f < currentState->fsm_floor ; f++){

        if (currentState->fsm_orders[f][0]  | currentState->fsm_orders[f][1]  | currentState->fsm_orders[f][2] ){
            orderFound = 1;
        }
    }
    return orderFound;
}


int orders_isEmpty(struct State* currentState){
    int isEmpty = 1;

    for(int f = 0 ; f < HARDWARE_NUMBER_OF_FLOORS ; f++){

        if (currentState->fsm_orders[f][0]  | currentState->fsm_orders[f][1]  | currentState->fsm_orders[f][2] ){
            isEmpty = 0;
        }
    }
    return isEmpty;
}


void orders_reset(struct State* currentState){

    for(int f = 0 ; f < HARDWARE_NUMBER_OF_FLOORS ; f++){

        for(int b = 0 ; b < HARDWARE_NUMBER_OF_BUTTONS ; b++){
            currentState->fsm_orders[f][b]=0;
        }
    }
}


void orders_update(struct State* currentState){

    for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){

        if(!(currentState->fsm_ignoreAllOrdes)){
            // Internal orders
            if(hardware_readOrder(f, HARDWARE_ORDER_INSIDE)){
                hardware_commandOrderLight(f, HARDWARE_ORDER_INSIDE, 1);
                currentState->fsm_orders[f][2] = 1;
            }
            // Orders going up 
            if(hardware_readOrder(f, HARDWARE_ORDER_UP)){
                hardware_commandOrderLight(f, HARDWARE_ORDER_UP, 1);
                currentState->fsm_orders[f][1] = 1;
            }
            // Orders going down 
            if(hardware_readOrder(f, HARDWARE_ORDER_DOWN)){
                hardware_commandOrderLight(f, HARDWARE_ORDER_DOWN, 1);
                currentState->fsm_orders[f][0] = 1;
            }
        }
    }
}


void orders_checkPeripherals(struct State* currentState){

    switch (currentState->fsm_direction){
        case DIRECTION_DOWN:
            if(!orders_scanDown(currentState) & !(currentState->fsm_door)){
                currentState->fsm_direction = DIRECTION_UP;
                if(currentState->fsm_orders[currentState->fsm_floor][1]){
                    routine_arrival(currentState);
                }    
        
            }
            break;

        case DIRECTION_UP:
            if(!orders_scanUp(currentState) & !currentState->fsm_door){
                currentState->fsm_direction = DIRECTION_DOWN;
                if(currentState->fsm_orders[currentState->fsm_floor][0]){
                    routine_arrival(currentState);
                }
            }
            break;
    }
}


void routine_stop(struct State* currentState){ 
    hardware_commandMovement(HARDWARE_MOVEMENT_STOP);
    hardware_commandStopLight(1);
    initialize_clearAllOrderLights();
    orders_reset(currentState);
    currentState->fsm_stop = 1;

    for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
        if(hardware_readFloorSensor(f)){
            hardware_commandDoorOpen(1);
            currentState->fsm_door = 1;
            currentState->timer_startTime = clock();
        }
    }
}


void routine_arrival(struct State* currentState){
    hardware_commandMovement(HARDWARE_MOVEMENT_STOP);

    for(int i = 0 ; i < HARDWARE_NUMBER_OF_BUTTONS ; i++){
        hardware_commandOrderLight(currentState->fsm_floor, i, 0);    
    }
    

    hardware_commandDoorOpen(1);
    currentState->fsm_door = 1;

    for(int b = 0 ; b < HARDWARE_NUMBER_OF_BUTTONS ; b++){
        currentState->fsm_orders[currentState->fsm_floor][b]=0;
    }

    currentState->timer_startTime = clock();

}


void routine_startMotor(struct State* currentState){

    if (!orders_isEmpty(currentState) & !(currentState->fsm_door)){
        
        switch (currentState->fsm_direction){
            case DIRECTION_UP:
                hardware_commandMovement(HARDWARE_MOVEMENT_UP);
                break;

            case DIRECTION_DOWN:
                hardware_commandMovement(HARDWARE_MOVEMENT_DOWN);
                break;
            
        }
    }
}


void handler_closeDoor(struct State* currentState){
    int elapsedTime =  (int)(((clock() - currentState->timer_startTime))/ CLOCKS_PER_SEC);
    
    if(currentState->fsm_door){
    
        if( elapsedTime >= currentState->timer_waitingTime ){
                currentState->fsm_door = 0;
                hardware_commandDoorOpen(0);
        }
    }
    
}


void handler_firstOrderAfterStop(struct State* currentState){

    if(currentState->fsm_stop & !orders_isEmpty(currentState)){
        currentState->fsm_stop=0;

        switch (currentState->fsm_direction){
            case (DIRECTION_DOWN):
                if (!orders_scanDown(currentState)){
                    currentState->fsm_direction = DIRECTION_UP;
                    currentState->fsm_floor--;
                }
                break;
            
            case (DIRECTION_UP):
                if (!orders_scanUp(currentState)){
                    currentState->fsm_direction = DIRECTION_DOWN;
                    currentState->fsm_floor++;
                }
                break;   

        }
    }
}


void handler_floorSensors(struct State* currentState){

    for(int currentFloor = 0; currentFloor < HARDWARE_NUMBER_OF_FLOORS; currentFloor++){

        if(hardware_readFloorSensor(currentFloor)){

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
            switch (check_halt(currentState)){
            case (1):
                routine_arrival(currentState);
                currentState->timer_startTime = clock();
                break;

            //The elevator should also take a halt if there is no peripheral orders
            case (0):
                orders_checkPeripherals(currentState);
                break;
            }             
            hardware_commandFloorIndicatorOn(currentFloor);

        }
    }
}


void handler_keepDoorOpen(struct State* currentState){

    if( (hardware_readStopSignal() | hardware_readObstructionSignal()) & currentState->fsm_door){
        hardware_commandDoorOpen(1);

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
            hardware_commandOrderLight(f, type, 0);
        }
    }
}


void initialize_state(struct State* currentState){

    currentState->fsm_floor = HARDWARE_NUMBER_OF_FLOORS;
    currentState->fsm_door = 0;
    currentState->fsm_direction = DIRECTION_DOWN;
    orders_reset(currentState);
    currentState->fsm_orders[0][2] = 1;
    currentState->fsm_ignoreAllOrdes = 1;
    currentState->fsm_stop = 0;
    currentState->timer_startTime = 0;
    currentState->timer_waitingTime = HARDWARE_WAITINGTIME;
}


void initialize_hardware(){

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