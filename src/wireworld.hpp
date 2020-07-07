#ifndef WIREWORLD
#define WIREWORLD

#include <fstream>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include "tiles.hpp"

struct cell
{
    unsigned int id;
    int x, y;
    cell(int X, int Y) {x = X; y = Y;}
};

struct conductor_cell
{
    unsigned int id;

    /* Number of head neighbours
        n = 0, Not conductor
        n = 1, Conductor, 0 head neighbours
        n = 2, Conductor, 1 head neighbours
        n = 3, Conductor, 2 head neighbours
        and so on... */
    uint8_t n;
};

class wireworld
{
    conductor_cell* conductor_map;
    std::vector<cell> heads[2];
    std::vector<cell> tails[2];
    bool cycle;
    int w, h;

    std::vector<cell> conductor_update_q;
    
    int gen, nheads, ntails;

    tile_map tiles;

public:
    wireworld(int cell_width);

    void reset();

    bool loadpng(const std::string& file_name);
    
    void update();
    
    void draw(sf::RenderWindow* window, sf::RenderStates states);
    
    int generation();
    
    int no_heads();
    
    int no_tails();

    int get_width();

    int get_height();
    
    ~wireworld();
};

#endif