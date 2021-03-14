#include "hardware.h"
#include "utilities.h"

int main(){
    struct State fsm;

    orders_reset(&fsm);
    initialize_state (&fsm);
    initialize_hardware();
    
    while(1){
        
        //The elevator does not accept orders until it has arrived at the first floor for the first time
        if(fsm.fsm_floor == 0){
            fsm.fsm_ignoreAllOrdes = 0;
        }
        
        while(hardware_readStopSignal() & !fsm.fsm_ignoreAllOrdes){
            routine_stop(&fsm);
        }

        // Turns off the stoplight after stopbutton is released
        if (fsm.fsm_stop){
            hardware_commandStopLight(0);
        }

        // The first order after stopbutton requires extra direction-logic
        handler_firstOrderAfterStop(&fsm);


        //Checks for new orders and updates the orderbook
        orders_update(&fsm);

        //Checks all floorsensors and decides whether the elevator should make a halt
        handler_floorSensors(&fsm);

        //Reopens the door if stopbutton or obstruction is active
        handler_keepDoorOpen(&fsm);

        //Starts the motor if the door is closed and the orderbook is not empty
        routine_startMotor(&fsm);

        //Closes the door if the timer is up
        handler_closeDoor(&fsm);
        
    }

    return 0;
}
