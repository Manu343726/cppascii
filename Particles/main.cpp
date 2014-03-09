/****************************************************************************
* Snippets, ejemplos, y utilidades del curso de C++ orientado a videojuegos *
* https://github.com/Manu343726/CppVideojuegos/                             *
*                                                                           * 
* Copyright © 2014 Manuel Sánchez Pérez                                     *
*                                                                           *
* This program is free software. It comes without any warranty, to          *
* the extent permitted by applicable law. You can redistribute it           *
* and/or modify it under the terms of the Do What The Fuck You Want         *
* To Public License, Version 2, as published by Sam Hocevar. See            *
* http://www.wtfpl.net/  and the COPYING file for more details.             *
****************************************************************************/

#include "fireworks.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <vector>
#include <unordered_map>
#include <random>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>

sf::RenderWindow window;





cpp::fireworks::fireworks_engine engine{ 15000u , sf::Vector2f{400.0f , 300.0f } , 0.006f };

void events_loop()
{
    sf::Event event_data;
    
    while( window.pollEvent( event_data ) )
    {
        switch( event_data.type )
        {
            case sf::Event::Closed:
                window.close(); break;
        }
    }
}

void game_loop()
{    
    while( window.isOpen() )
    {
        events_loop();
        
        window.clear( sf::Color::Black ); 
        
        engine.step();
        engine.draw( window );
        
        window.display();
    }
}

int main()
{
    window.create( sf::VideoMode( 800 , 600 ) , "Particles" );
    
    game_loop();
}


