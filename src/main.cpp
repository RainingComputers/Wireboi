#include <iostream>
#include <vector>
#include <cmath>

#include <SFML/Graphics.hpp>

#include "wireworld.hpp"
#include "ui.hpp"
#include "assets.hpp"
#include "choosepngfile.hpp"

#define MAX_FPS     64

void usage()
{
    std::cout<<"wireboi INPUT_FILE"<<std::endl;
}

int main(int argc, char *argv[])
{
    int w = 960;
    int h = 540;
    std::string file = "";

    /* Check for correct usage */
    if(argc == 1)
        file = choose_file("Wireboi open image");
    else if(argc == 2)
        file = argv[1];
    else
    {
        usage();
        return EXIT_FAILURE;
    }

    if(file == "") return EXIT_FAILURE;

    /* Create window */
    sf::Image appicon;
    appicon.loadFromMemory(&icon_png, icon_png_len);
    sf::RenderWindow window(sf::VideoMode(w, h), "WireBoi");
    window.setIcon(appicon.getSize().x, appicon.getSize().y, appicon.getPixelsPtr());

    /* Intilize wireworld */
    wireworld world(1);
    /* Load file */
    if(!world.loadpng(file))
    {
        std::cout<<"ERROR: Unable to load input file"<<std::endl;
        return EXIT_FAILURE;   
    }

    /* Intialize ui */
    world_ui ui;
    ui.auto_adjust(w, h, world.get_width(), world.get_height());

    /* Intialize fps counter */
    sf::Clock clock;
    float upms = 0;
    float fps = 0;
    float last_update = 0;
    float last_draw = 0;
    float last_loop = 0;
    float delta_time_draw = 0;
    float delta_time_update = 0;
    float delta_time_loop = 0;
    float time = 0;

    while (window.isOpen())
    {
        time = clock.getElapsedTime().asMicroseconds();
        delta_time_loop = time-last_loop;

        /* Event polling */
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::Resized)
            {
                /* Update the view to the new size of the window */
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
                w = event.size.width;
                h = event.size.height;
            }

            ui.update_event(event, window, w, h);
        }

        time = clock.getElapsedTime().asMicroseconds();
        /* Update wireworld */
        delta_time_update = time-last_update;
        if(ui.pause())
        {
            upms = 0;

            if(ui.step())
            {
                world.update();
                ui.ack_step();
            }
            else if(ui.reset())
            {
                world.reset();
                if(!world.loadpng(file))
                {
                    std::cout<<"ERROR: Unable to load input file"<<std::endl;
                    return EXIT_FAILURE;   
                }
                ui.ack_reset();
            }
        }
        else if(!ui.limit() || delta_time_update >= ui.limit_delta_time())
        {
            world.update();
            last_update = clock.getElapsedTime().asMicroseconds();
            if(delta_time_update != 0) upms = 1000/delta_time_update;
        }

        time = clock.getElapsedTime().asMicroseconds();
        /* Draw world */
        delta_time_draw = time-last_draw;
        if(delta_time_draw >= 1000000/MAX_FPS)
        {
            window.clear(sf::Color::Black);

            world.draw(&window, ui.get_render_states());

            /* Update and draw UI */
            ui.print_stat(world.generation(), upms, world.no_heads(), world.no_tails());
            ui.print_disp_stat(w, h, fps);
            ui.update(delta_time_draw/1000, w, h);

            ui.draw(&window);

            window.display();

            last_draw = clock.getElapsedTime().asMicroseconds();

            fps = 1000000/delta_time_draw;
        }

        /* Prevent 100% cpu usage if speed is not required */
        if((ui.limit() && ui.limit_delta_time() > 1000000/MAX_FPS) || ui.pause()) 
        {            
            sf::sleep(sf::microseconds((1000000/MAX_FPS)-delta_time_loop));
        }

        last_loop = clock.getElapsedTime().asMicroseconds();
            
    }

    return 0;
}