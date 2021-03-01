#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"
//#include "state.h" in utilities
#include "utilities.h"

clock_t timer_startTime = 0;
clock_t waitingTime = 3;


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
    fsm.fsm_floor=0;
    fsm.fsm_orders[0][2] = 1;
    fsm.fsm_stop = 0;
    fsm.fsm_direction = DIRECTION_DOWN;
    int error = hardware_init();
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }
    


    hardware_command_movement(HARDWARE_MOVEMENT_DOWN);

    while(1){

        while(hardware_read_stop_signal()){
                    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                    hardware_command_stop_light(1);
                    clear_all_order_lights();
                    resetOrders(&fsm);
                    fsm.fsm_stop = 1;
        }
        hardware_command_stop_light(0);
        if(fsm.fsm_stop==1 & order_isEmpty(fsm)==0){
            fsm.fsm_stop=0;
            if(fsm.fsm_direction==DIRECTION_DOWN & scanDown(fsm.fsm_floor, fsm)==0){
                fsm.fsm_direction = DIRECTION_UP;
            }
            else if(fsm.fsm_direction==DIRECTION_UP & scanUp(fsm.fsm_floor, fsm)==0){
                fsm.fsm_direction = DIRECTION_DOWN;
            }
        }

        /* All buttons must be polled, like this:
            Added functionality: If current floor matches order-book -> make a stop
         */
        for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
            if(hardware_read_floor_sensor(f)){
                fsm.fsm_floor = f;
                if(f==0){
                    fsm.fsm_direction = DIRECTION_UP;
                }
                if(f==HARDWARE_NUMBER_OF_FLOORS-1){
                    fsm.fsm_direction = DIRECTION_DOWN;
                }
                if(fsm.fsm_direction == DIRECTION_DOWN){
                    
                    if(fsm.fsm_orders[f][0] == 1 | fsm.fsm_orders[f][2] == 1){
                        hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                        hardware_command_order_light(f, HARDWARE_ORDER_DOWN, 0);
                        hardware_command_order_light(f, HARDWARE_ORDER_INSIDE, 0);
                        hardware_command_door_open(1);
                        fsm.fsm_door = 1;
                        timer_startTime = clock();
                    
                        
                        fsm.fsm_orders[f][0]=0;
                        fsm.fsm_orders[f][2]=0;
                    }
                    else if(scanDown(f, fsm) == 0){
                        hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                        fsm.fsm_direction = DIRECTION_UP;
                        if(fsm.fsm_orders[f][1]){
                            hardware_command_door_open(1);
                            fsm.fsm_door = 1;
                            timer_startTime = clock();
                            fsm.fsm_orders[f][1] = 0;
                            hardware_command_order_light(f, HARDWARE_ORDER_UP, 0);
                        }    
                    }
                }
                else { //if(fsm.fsm_direction == DIRECTION_UP)
                    if(fsm.fsm_orders[f][1] == 1 | fsm.fsm_orders[f][2] == 1){
                        
                        hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                        hardware_command_order_light(f, HARDWARE_ORDER_UP, 0);
                        hardware_command_order_light(f, HARDWARE_ORDER_INSIDE, 0);
                        hardware_command_door_open(1);
                        fsm.fsm_door = 1;
                        timer_startTime = clock();

                        fsm.fsm_floor = f;
                        fsm.fsm_orders[f][1]=0;
                        fsm.fsm_orders[f][2]=0;
                    }
                    else if(scanUp(f, fsm) == 0){
                        hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                        fsm.fsm_direction = DIRECTION_DOWN;
                        if(fsm.fsm_orders[f][0]){
                            hardware_command_door_open(1);
                            fsm.fsm_door = 1;
                            timer_startTime = clock();
                            fsm.fsm_orders[f][0] = 0;
                            hardware_command_order_light(f, HARDWARE_ORDER_DOWN, 0);
                        }
                    }
                    
                }
                hardware_command_floor_indicator_on(f);
            }
        }

        /* Lights are set and cleared like this: */
        for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
            /* Internal orders */
            if(hardware_read_order(f, HARDWARE_ORDER_INSIDE)){
                hardware_command_order_light(f, HARDWARE_ORDER_INSIDE, 1);
                fsm.fsm_orders[f][2] = 1;
            }

            /* Orders going up */
            if(hardware_read_order(f, HARDWARE_ORDER_UP)){
                hardware_command_order_light(f, HARDWARE_ORDER_UP, 1);
                fsm.fsm_orders[f][1] = 1;
            }

            /* Orders going down */
            if(hardware_read_order(f, HARDWARE_ORDER_DOWN)){
                hardware_command_order_light(f, HARDWARE_ORDER_DOWN, 1);
                fsm.fsm_orders[f][0] = 1;
            }
        }

        /* Code to clear all lights given the obstruction signal */
        if(hardware_read_obstruction_signal() & fsm.fsm_door==1){
            hardware_command_door_open(1);
            fsm.fsm_door = 1;
            timer_startTime = clock();
            
        }
      
        
        
        
        if (order_isEmpty(fsm)== 0 & fsm.fsm_door==0){
            if(fsm.fsm_direction == DIRECTION_UP){
                hardware_command_movement(HARDWARE_MOVEMENT_UP);
            }
            else{
                hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
            }

        }
        
        
        if((((double)(clock() - timer_startTime))/ CLOCKS_PER_SEC >= waitingTime) & fsm.fsm_door==1){
            fsm.fsm_door = 0;
            hardware_command_door_open(0);
        }

    }

    return 0;
}
