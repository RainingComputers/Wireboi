#ifndef TILES
#define TILES

#include <vector>

#include <SFML/Graphics.hpp>

class tile_map
{
    std::vector<sf::Vertex> vertices;
    int tile_width;

public:
    tile_map(int tile_width);
    
    void set_tile_width(int width);
    
    int add_tile(int x, int y, sf::Color color);
    
    void set_tile_color(unsigned int id, sf::Color color);
    
    void clear();
    
    void draw(sf::RenderWindow* window, sf::RenderStates states);
};

#endif