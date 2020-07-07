#include "smooth.hpp"
  
smooth::smooth(float ctrl_velocity, float initial_val)
{
    val = initial_val;
    velocity = ctrl_velocity;

    dir_up = false;
    dir_down = false;
}

void smooth::up()
{
    dir_down = false;
    dir_up = true;
}

void smooth::down()
{
    dir_down = true;
    dir_up = false;
}

void smooth::stop()
{
    dir_down = false;
    dir_up = false;
}

void smooth::update(float delta_time)
{
    if(dir_up) val += delta_time*velocity;
    else if(dir_down) val -= delta_time*velocity;
}


