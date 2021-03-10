
#include "hardware.h"
#include "utilities.h"
//hei
int main(){
    struct State fsm;

    orders_reset(&fsm);
    initialize_State (&fsm);
    initialize_Hardware();
    
    while(1){
        
        if(fsm.fsm_floor == 0){
            fsm.fsm_resetElevator=1;
        }
        
        while(hardware_readStopSignal() & fsm.fsm_resetElevator){
            stopRoutine(&fsm);
        }

        //Turns off the stoplight after stopbutton is released
        initialize_clearAllOrderLights(0);
        
        //The first order after stopbutton requires extra direction-logic
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
