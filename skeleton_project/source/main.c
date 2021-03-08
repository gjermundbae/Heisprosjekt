#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"
//#include "state.h" in utilities
#include "utilities.h"

clock_t timer_startTime = 0;
clock_t timer_waitingTime = 3;


static void clear_all_order_lights(){
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


int main(){
    struct State fsm;
    resetOrders(&fsm);
    fsm.fsm_floor=4;
    fsm.fsm_orders[0][2] = 1;
    fsm.fsm_stop = 0;
    fsm.fsm_resetElevator = 0;
    fsm.fsm_direction = DIRECTION_DOWN;
    int error = hardware_init();
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }
    
    hardware_command_movement(HARDWARE_MOVEMENT_DOWN);

    while(1){
        
        if(fsm.fsm_floor == 0){
            fsm.fsm_resetElevator=1;
        }
        while(hardware_read_stop_signal()){
            clear_all_order_lights();
            stopRoutine(&fsm, &timer_startTime);
        }
        hardware_command_stop_light(0);
        
        if(fsm.fsm_stop==1 & order_isEmpty(fsm)==0){
            fsm.fsm_stop=0;
            if(fsm.fsm_direction==DIRECTION_DOWN & scanDown(fsm.fsm_floor, fsm)==0){
                fsm.fsm_direction = DIRECTION_UP;
                fsm.fsm_floor--;
            }
            else if(fsm.fsm_direction==DIRECTION_UP & scanUp(fsm.fsm_floor, fsm)==0){
                fsm.fsm_direction = DIRECTION_DOWN;
                fsm.fsm_floor++;
            }
            
        }
        updateOrders(&fsm);

        /* All buttons must be polled, like this:
            Added functionality: If current floor matches order-book -> make a stop
         */
        for(int currentFloor = 0; currentFloor < HARDWARE_NUMBER_OF_FLOORS; currentFloor++){
            if(hardware_read_floor_sensor(currentFloor)){
                // Setting the new floor
                fsm.fsm_floor = currentFloor;
                    //Flipping the direction if the elevator is at the end floors 
                if(currentFloor==0){
                    fsm.fsm_direction = DIRECTION_UP;
                }
                else if(currentFloor==HARDWARE_NUMBER_OF_FLOORS-1){
                    fsm.fsm_direction = DIRECTION_DOWN;
                }
                //Checking if the elevator should take a halt at the current floor
                if(((fsm.fsm_direction== DIRECTION_DOWN &fsm.fsm_orders[currentFloor][0]) | (fsm.fsm_direction == DIRECTION_UP & fsm.fsm_orders[currentFloor][1]) | fsm.fsm_orders[currentFloor][2]) | (fsm.fsm_door & (fsm.fsm_orders[currentFloor][0] | fsm.fsm_orders[currentFloor][1]) )){
                    arrivalRoutine(&fsm, currentFloor);
                    timer_startTime = clock();
                }
                else{
                    checkPeripheralOrders(&fsm, currentFloor, &timer_startTime);
                }                
                hardware_command_floor_indicator_on(currentFloor);
            }
        }
        /* Code to clear all lights given the obstruction signal */
        if( (hardware_read_stop_signal() | hardware_read_obstruction_signal()) & fsm.fsm_door){
            hardware_command_door_open(1);
            fsm.fsm_door = 1;
            timer_startTime = clock();
        }
        if (!order_isEmpty(fsm) & !fsm.fsm_door){
            if(fsm.fsm_direction == DIRECTION_UP){
                hardware_command_movement(HARDWARE_MOVEMENT_UP);
            }
            else{
                hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
            }
        }
        checkTimerAndCloseDoor(&fsm, &timer_startTime, &timer_waitingTime);

    }

    return 0;
}
