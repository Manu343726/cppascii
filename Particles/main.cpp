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
#include "bounded.hpp"
#include "SFML-2.1/include/SFML/Graphics/Color.hpp"

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

cpp::fireworks::fireworks_engine engine{ 15000u , dl32::vector_2df{400.0f , 300.0f } , 0.006f };
cpp::bounded::bounded_engine bounded_engine;
                                           
                                           
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
        
        bounded_engine.step();
        bounded_engine.draw( window );
        
        window.display();
    }
}

void init_pipeline()
{
    auto lambda = []( cpp::default_particle_data_holder& ) {};
    
    TURBO_ASSERT( (tml::logical_not<cpp::is_stated_policy<decltype(lambda),cpp::default_particle_data_holder>>) , "lambda with state???" );
    
    cpp::evolution_policies_pipeline<cpp::default_particle_data_holder> pipeline;
    
    pipeline.add_stage( cpp::make_bounds_policy( cpp::inverse_bounds<cpp::circle_bounds>{ dl32::vector_2df{ 400.0f , 300.0f } , 300.0f } ) );
    pipeline.add_stage( cpp::make_bounds_policy( cpp::rectangle_bounds{ cpp::aabb_2d<float>::from_coords_and_size( 0.0f , 0.0f , 800.0f , 600.0f ) } ) );
    pipeline.add_stage( []( cpp::default_particle_data_holder& data )
                        {
                           data.speed() *= 1.0001f;
                        }
                      );
    pipeline.add_stage( []( cpp::default_particle_data_holder& data )
                        {
                           data.color() = sf::Color{ (int)data.position().x % 256 , 
                                                     (int)data.position().y % 256 , 
                                                     (int)data.position().y % 256 };
                        }
                      );
    
    
    bounded_engine.initialize( 100000u , dl32::vector_2df{400.0f , 300.0f } , 0.06f , pipeline );
}

int main()
{
    window.create( sf::VideoMode( 800 , 600 ) , "Particles" );
    
    std::cout << sizeof( cpp::fireworks::particle ) << std::endl;
    std::cout << sizeof( typename decltype( bounded_engine )::particle ) << std::endl;
    
    init_pipeline();
    
    game_loop();
}


