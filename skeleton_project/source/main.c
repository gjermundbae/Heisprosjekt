
#include "hardware.h"
#include "utilities.h"

int main(){
    struct State fsm;

    resetOrders(&fsm);
    initialize_State(&fsm, 3);
    initialize_Hardware();
    
    while(1){
        
        if(fsm.fsm_floor == 0){
            fsm.fsm_resetElevator=1;
        }
        
        while(hardware_read_stop_signal() & fsm.fsm_resetElevator){
            stopRoutine(&fsm);
        }

        //Turns off the stoplight after stopbutton is released
        hardware_command_stop_light(0);
        
        //The first order aften a stopcase requires extra direction-logic
        checkDirectionAfterStop(&fsm);

        //Checks for new orders and updates the orderbook
        updateOrders(&fsm);

        //Checks all floorsensors and decides whether the elevator should make a halt
        checkFloorSensors(&fsm);

        //Reopens the door if stopbutton or obstruction is active
        keepDoorOpen(&fsm);

        //Starts the motor if the door is closed and the orderbook is not empty
        startMotor(&fsm);

        //Closes the door if the timer is up
        checkTimerAndCloseDoor(&fsm);

    }
    return 0;
}
