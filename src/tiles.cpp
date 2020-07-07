#include "tiles.hpp"

tile_map::tile_map(int tile_width)
{
    set_tile_width(tile_width);
}

void tile_map::set_tile_width(int width)
{
    tile_width = width;
}

int tile_map::add_tile(int x, int y, sf::Color color)
{
    /* ID for this tile, used for changing color */
    unsigned int id = vertices.size();

    /* Calculate absolute x, y coordinates */
    int X = x*tile_width;
    int Y = y*tile_width;

    /* Create rectangle out of two triangles */
    vertices.emplace_back(sf::Vector2f(X, Y+tile_width), color);
    vertices.emplace_back(sf::Vector2f(X, Y), color);
    vertices.emplace_back(sf::Vector2f(X+tile_width, Y), color);
    vertices.emplace_back(sf::Vector2f(X, Y+tile_width), color);
    vertices.emplace_back(sf::Vector2f(X+tile_width, Y), color);
    vertices.emplace_back(sf::Vector2f(X+tile_width, Y+tile_width), color);

    return id;
}

void tile_map::set_tile_color(unsigned int id, sf::Color color)
{
    for(int i=0; i<6; i++)
        vertices[id+i].color = color;
}

void tile_map::clear()
{
    vertices.clear();
}

void tile_map::draw(sf::RenderWindow* window, sf::RenderStates states)
{
    window->draw(&vertices[0], vertices.size(), sf::Triangles, states);
}