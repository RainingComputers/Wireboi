#ifndef UI
#define UI

#include <string>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "smooth.hpp"

class world_ui
{
    sf::Font font;
    sf::Text stat_label;
    sf::Text disp_label;
    sf::Text speed_label;

    smooth offset_x, offset_y, scale;
    sf::Vector2i zoom_centre;

    /* Simulation control flags */
    bool pause_sim;
    bool step_sim;
    bool sim_limit;
    bool reset_sim;
    bool show_ui;
    int8_t speed_mode;

    /* Key press flags */
    bool can_step;
    bool can_inc_speed;
    bool can_dec_speed;
    bool panning;

    /* Keep track of mouse delta durin panning */
    sf::Vector2i mouse_start;

    /* Texture */
    sf::Texture play_texture;
    sf::Texture pause_texture;
    sf::Texture step_texture;
    sf::Texture reset_texture;
    sf::Texture inc_texture;
    sf::Texture dec_texture;
    sf::Texture plus_texture;
    sf::Texture minus_texture;

    /* Buttons */
    sf::RectangleShape play_pause_button;
    sf::RectangleShape reset_button;
    sf::RectangleShape step_button;
    sf::RectangleShape plus_button;
    sf::RectangleShape minus_button;
    sf::RectangleShape inc_button;
    sf::RectangleShape dec_button;

    void inc_speed();

    void dec_speed();

    void toggle_pause();

    void zoom_in();

    void zoom_out();

    void set_speed_string();

public:
    world_ui();
    
    void print_stat(int gen, float upms, int heads, int tails);

    void print_disp_stat(int w, int h, int fps);
    
    void update_event(sf::Event event, const sf::RenderWindow& window, int w, int h);
    
    sf::RenderStates get_render_states();
    
    void update(float delta_time, int w, int h);

    void draw(sf::RenderWindow* window);

    float limit_delta_time();

    bool limit();

    bool pause();

    bool step();

    bool reset();

    void ack_step();

    void ack_reset();

    void auto_adjust(int w, int h, int img_w, int img_h);
};

#endif