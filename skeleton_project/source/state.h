#define DIRECTION_UP = 0;
#define DIRECTION_DOWN = 1;
struct State{
    int fsm_floor;
    int fsm_goal;
    int fsm_stop;
    int fsm_door;
    int fsm_direction;
    int fsm_orders[4][3];
};