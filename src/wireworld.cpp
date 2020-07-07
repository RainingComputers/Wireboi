#include "wireworld.hpp"

#define CONDUCTOR_COLOUR    sf::Color(255, 128, 0)
#define HEAD_COLOUR         sf::Color(0, 128, 255)
#define TAIL_COLOUR         sf::Color(255, 255, 255)

wireworld::wireworld(int cell_width):
    tiles(cell_width)
{
    /* Starts with even cycle */
    conductor_map = nullptr;
    reset();
}

void wireworld::reset()
{
    cycle = false;
    gen = 0;
    nheads = 0;
    ntails = 0;

    heads[false].clear();
    heads[true].clear();
    tails[false].clear();
    tails[true].clear();

    if(conductor_map != nullptr) 
        delete[] conductor_map;
}

void wireworld::update()
{
    conductor_update_q.clear();

    /* All heads will become tails */
    for(cell head : heads[cycle]) 
    {
        tails[!cycle].push_back(head);

        /* Change tile colour */
        tiles.set_tile_color(head.id, TAIL_COLOUR);
    }
        
    /* Count the number of head neighbours in the cycle, 
        update conductor map after cycle type update,
        store the conductors that needs to be checked in a queue */
    for(cell head : heads[cycle])
    {
        for(cell i : {cell(head.x+1, head.y), cell(head.x-1, head.y),
            cell(head.x, head.y+1), cell(head.x, head.y-1),
            cell(head.x+1, head.y+1), cell(head.x-1, head.y-1),
            cell(head.x+1, head.y-1), cell(head.x-1, head.y+1)})
        {
            int idx = i.x+w*i.y;

            if(conductor_map[idx].n)
            {
                conductor_map[idx].n++;
                
                /* If discovered for first time, add to update queue */
                if(conductor_map[idx].n == 2) 
                    conductor_update_q.push_back(i);
            }
        }
    }    

    /* All tails will become conductors */
    for(cell tail : tails[cycle])
    {
        int idx = tail.x+w*tail.y;

        conductor_map[idx].n = 1;
        conductor_map[idx].id = tail.id;
        
        /* Update tile colour */
        tiles.set_tile_color(conductor_map[idx].id, CONDUCTOR_COLOUR);
    }

    /* Clear current cycle heads and tails */
    heads[cycle].clear();
    tails[cycle].clear();

    /* Update cycle type */
    cycle = !cycle;

    /* Check conductor queue and update */
    for(cell i : conductor_update_q)
    {
        int x = i.x;
        int y = i.y;

        /* Number of neighbours */
        int n = conductor_map[x+w*y].n;
        int id = conductor_map[x+w*y].id;
        
        /* If only less than 2 neighbours, turn into head */
        if(n == 2 || n == 3)
        {
            conductor_map[x+w*y].n = 0;
            heads[cycle].push_back(cell(x, y));
            heads[cycle].back().id = id;
        }
        /* otherwise, still conductor */
        else if(n > 0)
        {
            conductor_map[x+w*y].n = 1;
            tiles.set_tile_color(id, CONDUCTOR_COLOUR);
        }
    }

    /* Draw heads */
    for(cell head : heads[cycle])
        tiles.set_tile_color(head.id, HEAD_COLOUR);

    /* Update generation count */
    gen++;
    nheads = heads[cycle].size();
    ntails = tails[cycle].size();
}

void wireworld::draw(sf::RenderWindow* window, sf::RenderStates states)
{
    /* Draw the cells */
    tiles.draw(window, states);
}

bool wireworld::loadpng(const std::string& file_name)
{
    sf::Image img;
    if(!img.loadFromFile(file_name)) return false;

    sf::Vector2u img_size = img.getSize();
    w = img_size.x;
    h = img_size.y;

    /* Intialize conductor map */
    conductor_map = new conductor_cell[w*h];
    for(int x=0; x<w; x++)
        for(int y=0; y<h; y++)
            conductor_map[x+w*y].n = 0;

    /* Read image pixel by pixel */
    for(int x=0; x<img_size.x; x++)
        for(int y=0; y<img_size.y; y++)
        {
            sf::Color pixel = img.getPixel(x, y);

            /* Add cell to world */
            if(pixel == CONDUCTOR_COLOUR) 
            {
                conductor_map[x+w*y].n = 1;
                conductor_map[x+w*y].id = tiles.add_tile(x, y, CONDUCTOR_COLOUR);
            }
            else if(pixel == HEAD_COLOUR) 
            {
                heads[cycle].emplace_back(x, y);
                heads[cycle].back().id = tiles.add_tile(x, y, HEAD_COLOUR);
            }
            else if(pixel == TAIL_COLOUR) 
            {
                tails[cycle].emplace_back(x, y);
                tails[cycle].back().id = tiles.add_tile(x, y, TAIL_COLOUR);
            }

        }

    return true;
}

int wireworld::generation() 
{ 
    return gen; 
}

int wireworld::no_heads() 
{ 
    return nheads; 
}

int wireworld::no_tails() 
{ 
    return ntails; 
}

wireworld::~wireworld()
{
    delete[] conductor_map;
}

int wireworld::get_width()
{
    return w;
}

int wireworld::get_height()
{
    return h;
}