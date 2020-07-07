#ifndef SMOOTH
#define SMOOTH

class smooth
{
    float velocity;
    float min_val;
    bool dir_up;
    bool dir_down;

public:
    float val;
    
    smooth(float velocity, float intial_val=0);
    
    void up();
    
    void down();
    
    void stop();
    
    void update(float delta_time);
};

#endif