#include "ui.hpp"
#include "assets.hpp"

///REMOVE DIS
#include <iostream>

#define PAD_X 5
#define PAD_Y 5

#define SCROLL_VEL      0.5
#define ZOOM_VEL        0.002
#define MOUSE_ZOOM_VEL  0.2    
#define BUTTON_WIDTH    64
#define FONT_SIZE       22

#define NUM_MODES       7
/* Speed modes in ups per sec */
static int modes[] = {1, 2, 5, 10, 20, 100, -1};
/* Speed modes in microseoncds */
static int modes_delta_time[] = {1000000, 500000, 200000, 100000, 50000, 10000, 0};

#define DEFAULT_MODE    2

world_ui::world_ui() : 
    zoom_centre(0, 0),
    offset_x(SCROLL_VEL), offset_y(SCROLL_VEL), scale(ZOOM_VEL, 1),
    play_pause_button(sf::Vector2f(BUTTON_WIDTH, BUTTON_WIDTH)), 
    reset_button(sf::Vector2f(BUTTON_WIDTH, BUTTON_WIDTH)),
    step_button(sf::Vector2f(BUTTON_WIDTH, BUTTON_WIDTH)), 
    plus_button(sf::Vector2f(BUTTON_WIDTH, BUTTON_WIDTH)),
    minus_button(sf::Vector2f(BUTTON_WIDTH, BUTTON_WIDTH)), 
    inc_button(sf::Vector2f(BUTTON_WIDTH, BUTTON_WIDTH)),
    dec_button(sf::Vector2f(BUTTON_WIDTH, BUTTON_WIDTH))
{
    /* Load font */
    font.loadFromMemory(&mono_ttf, mono_ttf_len);

    /* Load textures */
    play_texture.loadFromMemory(&play_gif, play_gif_len);
    pause_texture.loadFromMemory(&pause_gif, pause_gif_len);
    step_texture.loadFromMemory(&corner_down_right_gif, corner_down_right_gif_len);
    reset_texture.loadFromMemory(&reset_gif, reset_gif_len);
    inc_texture.loadFromMemory(&chevrons_right_gif, chevrons_right_gif_len);
    dec_texture.loadFromMemory(&chevrons_left_gif, chevrons_left_gif_len);
    plus_texture.loadFromMemory(&plus_gif, plus_gif_len);
    minus_texture.loadFromMemory(&minus_gif, minus_gif_len);

    /* Set texture for buttons */
    play_pause_button.setTexture(&play_texture);
    reset_button.setTexture(&reset_texture);
    step_button.setTexture(&step_texture);
    plus_button.setTexture(&plus_texture);
    minus_button.setTexture(&minus_texture);
    inc_button.setTexture(&inc_texture);
    dec_button.setTexture(&dec_texture);

    /* Label in top left corner */
    stat_label.setCharacterSize(FONT_SIZE);
    stat_label.setFont(font);
    stat_label.setFillColor(sf::Color::White);

    /* Label in top right corener */
    disp_label.setCharacterSize(FONT_SIZE);
    disp_label.setFont(font);
    disp_label.setFillColor(sf::Color::White);

    /* Label for current speed mode, appears above play button */
    speed_label.setCharacterSize(FONT_SIZE);
    speed_label.setFont(font);
    speed_label.setFillColor(sf::Color::White);

    /* Simulation controls and modes */
    pause_sim = true;
    step_sim = false;
    sim_limit = true;
    reset_sim = false;
    show_ui = true;
    speed_mode = DEFAULT_MODE;

    /* Key press flags */
    can_step = true;
    can_inc_speed = true;
    can_dec_speed = true;
    panning = false;

    /* Keep track of mouse delta during pan mode */
    mouse_start = sf::Vector2i(0, 0);

    /* Set speed string */
    set_speed_string();
}

void world_ui::auto_adjust(int w, int h, int img_w, int img_h)
{
    scale.val = w/img_w;

    if(scale.val > 16) scale.val = 16;
}

void world_ui::set_speed_string()
{
    if(modes[speed_mode] == -1) speed_label.setString("INFx");
    else speed_label.setString(std::to_string(modes[speed_mode])+"x");
}

void world_ui::print_stat(int gen, float ups, int heads, int tails)
{
    /* Generation = xxxx label */
    std::string gen_label = "Generation = " + std::to_string(gen);
    
    /* Choose unit for ups/sec */
    std::string unit = " updates/ms";
    if(ups < 1)
    {
        ups*=1000;
        unit = " updates/sec";
    }

    /* xxx.xx updates/sec label */
    std::string ups_string = std::to_string((int)ceil(ups));
    std::string ups_label = "\n" + ups_string + unit;
    
    /* xxxx heads, xxxx tails label */
    std::string heads_label = "\n" + std::to_string(heads) + " heads, ";
    std::string tails_label = std::to_string(tails) + " tails";

    /* Top left corner */
    stat_label.setString(gen_label+ups_label+heads_label+tails_label);
}

void world_ui::print_disp_stat(int w, int h, int fps)
{
    /* Resolution */
    std::string res_string = std::to_string(w)+'x'+std::to_string(h);

    /* FPS label */
    std::string fps_string = std::to_string(fps);
    std::string fps_label = "\n" + fps_string + " fps";

    /* Scale label */
    std::string scale_string = std::to_string(scale.val);
    /* Round to 2 decimal places */
    if(scale_string.length() > 4)
        scale_string = scale_string.substr(0, scale_string.size()-4);
    std::string scale_label = "\n" + scale_string + "x scale";

    /* Put it in top right corner */
    disp_label.setString(res_string+fps_label+scale_label);
    int txt_w = disp_label.getLocalBounds().width;
    disp_label.setPosition(sf::Vector2f(w-txt_w-PAD_X, PAD_Y));
}


sf::RenderStates world_ui::get_render_states()
{
    sf::Transform transform = sf::Transform::Identity;
    transform.scale(scale.val, scale.val);
    transform.translate(offset_x.val, offset_y.val);

    sf::RenderStates states;
    states.transform = transform;

    return states;
}

void world_ui::update(float delta_time, int w, int h)
{
    offset_x.update(delta_time);
    offset_y.update(delta_time);
    scale.update(delta_time);

    /* Set button position */
    sf::Vector2f centre(w/2-BUTTON_WIDTH/2, h-BUTTON_WIDTH);
    play_pause_button.setPosition(centre);
    step_button.setPosition(centre+sf::Vector2f(BUTTON_WIDTH, 0));
    inc_button.setPosition(centre+sf::Vector2f(2*BUTTON_WIDTH, 0));
    plus_button.setPosition(centre+sf::Vector2f(3*BUTTON_WIDTH, 0));
    reset_button.setPosition(centre-sf::Vector2f(BUTTON_WIDTH, 0));
    dec_button.setPosition(centre-sf::Vector2f(2*BUTTON_WIDTH, 0));
    minus_button.setPosition(centre-sf::Vector2f(3*BUTTON_WIDTH, 0));
    
    /* Put speed label */
    int txt_w = speed_label.getLocalBounds().width;
    speed_label.setPosition(w/2-txt_w/2, h-BUTTON_WIDTH-FONT_SIZE);
}

void world_ui::draw(sf::RenderWindow* window)
{
    if(!show_ui) return;

    window->draw(stat_label);
    window->draw(disp_label);
    window->draw(play_pause_button);
    window->draw(step_button);
    window->draw(inc_button);
    window->draw(plus_button);
    window->draw(reset_button);
    window->draw(dec_button);
    window->draw(minus_button);
    window->draw(speed_label);
}

void world_ui::inc_speed()
{
    speed_mode++;
    if(speed_mode > NUM_MODES-1) speed_mode = NUM_MODES-1;

    if(speed_mode == NUM_MODES-1) sim_limit = false;

    set_speed_string();
}

void world_ui::dec_speed()
{
    speed_mode--;
    if(speed_mode < 0) speed_mode = 0;
    sim_limit = true;

    set_speed_string();
}

void world_ui::toggle_pause()
{
    pause_sim = !pause_sim;

    if(!pause_sim) play_pause_button.setTexture(&pause_texture);
    else play_pause_button.setTexture(&play_texture);
}

void world_ui::zoom_in()
{
    scale.val+=MOUSE_ZOOM_VEL;
}

void world_ui::zoom_out()
{
    scale.val-=MOUSE_ZOOM_VEL;
}

void world_ui::update_event(sf::Event event, const sf::RenderWindow& window, int w, int h)
{
    if(event.type == sf::Event::MouseButtonReleased) 
    {
        if(event.mouseButton.button == sf::Mouse::Left)
            panning = false;
    }
    else if(panning)
    {
        sf::Vector2i current_pos = sf::Mouse::getPosition(window);
        sf::Vector2i delta = current_pos-mouse_start;


        offset_x.val += delta.x/scale.val;
        offset_y.val += delta.y/scale.val;

        mouse_start = current_pos;
    }
    else if(event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
            case sf::Keyboard::Left:
            case sf::Keyboard::A:
                offset_x.up();
                break;
            case sf::Keyboard::Right:
            case sf::Keyboard::D:
                offset_x.down();
                break;   
            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                offset_y.up();
                break; 
            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                offset_y.down();
                break; 
            case sf::Keyboard::Equal:
                scale.up();
                break; 
            case sf::Keyboard::Dash:
                scale.down();
                break; 
            case sf::Keyboard::F6:   
            case sf::Keyboard::Space:
                toggle_pause();
                break;
            case sf::Keyboard::F10:
                if(pause_sim&&can_step) 
                {
                    step_sim = true;
                    can_step = false;
                }
                break;
            case sf::Keyboard::Comma:
                if(can_dec_speed) 
                {
                    dec_speed();
                    can_dec_speed = false;
                }
                break;
            case sf::Keyboard::Period:
                if(can_inc_speed)
                {
                    inc_speed();
                    can_inc_speed = false;
                }
                break;
        }
    }
    else if(event.type == sf::Event::KeyReleased)
    {
        switch (event.key.code)
        {
            case sf::Keyboard::Left:
            case sf::Keyboard::Right:
            case sf::Keyboard::A:
            case sf::Keyboard::D:
                offset_x.stop();
                break;   
            case sf::Keyboard::Up:
            case sf::Keyboard::Down:
            case sf::Keyboard::W:
            case sf::Keyboard::S:
                offset_y.stop();
                break; 
            case sf::Keyboard::Equal:
            case sf::Keyboard::Dash:
                scale.stop();
                break;    
            case sf::Keyboard::F10:
                can_step = true;
                break;
            case sf::Keyboard::Comma:
                can_dec_speed = true;
                break;
            case sf::Keyboard::Period:
                can_inc_speed = true;
                break;
            case sf::Keyboard::G:
                show_ui = !show_ui;
                break;

        }
    }
    else if(event.type == sf::Event::MouseWheelScrolled)
    {
        /* Mouse wheel for zoom */
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
        {
            int mouse_delta = event.mouseWheelScroll.delta;

            if(mouse_delta > 0) zoom_in();
            else if(mouse_delta < 0) zoom_out();
        }
    }
    else if(event.type == sf::Event::MouseButtonPressed)
    {
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);

        /* Check if any of the dock buttons are being pressed */
        if(mouse_pos.y > h-BUTTON_WIDTH && 
            mouse_pos.x > w/2-3.5*BUTTON_WIDTH && mouse_pos.x < w/2+3.5*BUTTON_WIDTH &&
            show_ui)
        {
            int button = (mouse_pos.x-(w/2-3.5*BUTTON_WIDTH))/BUTTON_WIDTH;
            
            if(button == 0) zoom_out();
            else if(button == 1) dec_speed();
            else if(button == 2) 
            {
                if(!pause_sim) toggle_pause();
                reset_sim = true;
            }
            else if(button == 3) toggle_pause();
            else if(button == 4) step_sim = true;
            else if(button == 5) inc_speed();
            else if(button == 6) zoom_in();

        }
        /* Mouse press for panning */
        if(event.mouseButton.button == sf::Mouse::Left)
        {
            panning = true;
            mouse_start = mouse_pos;
        }
            
    }
}

bool world_ui::pause()
{
    return pause_sim;
}

bool world_ui::step()
{
    return step_sim;
}

bool world_ui::reset()
{
    return reset_sim;
}

void world_ui::ack_step()
{
    step_sim = false;
}

void world_ui::ack_reset()
{
    reset_sim = false;
}

bool world_ui::limit()
{
    return sim_limit;
}

float world_ui::limit_delta_time()
{
    return modes_delta_time[speed_mode];
}