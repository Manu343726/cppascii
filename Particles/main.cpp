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

#include "particle.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <vector>
#include <unordered_map>
#include <random>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>


constexpr std::size_t particles_count = 10000;
          float blackhole_mass        = 10000.0f;
constexpr float blackhole_deadzone    = 10.0f;

std::vector<cpp::particle> particles;
sf::Vector2f particles_source;
sf::Vector2f blackhole_position;

sf::RenderWindow window;
//std::unordered_map<int,std::function<void(sf::Event&)>> events_manager;


void events_loop()
{
    sf::Event event_data;
    
    while( window.pollEvent( event_data ) )
    {
        switch( event_data.type )
        {
            case sf::Event::MouseMoved:
                if( sf::Keyboard::isKeyPressed( sf::Keyboard::LControl ) )
                {
                    particles_source.x = event_data.mouseMove.x;
                    particles_source.y = event_data.mouseMove.y;
                }
                else
                {
                    blackhole_position.x = event_data.mouseMove.x;
                    blackhole_position.y = event_data.mouseMove.y;
                }
                break;
            case sf::Event::MouseWheelMoved:
                if( event_data.mouseWheel.delta > 0 )
                    blackhole_mass *= 2;
                else
                    blackhole_mass /= 2;
                break;
            case sf::Event::MouseButtonPressed:
                if( event_data.mouseButton.button == sf::Mouse::Button::Left  ) particles.resize( particles.size() * 2 );
                if( event_data.mouseButton.button == sf::Mouse::Button::Right ) particles.resize( (particles.size() / 2) + 1 );
                break;
            case sf::Event::Closed:
                window.close(); break;
        }
        
        //events_manager[(int)event_data.type]( event_data );
    }
}

void game_loop()
{
    std::random_device rndev;
    std::default_random_engine prng( rndev() );
    std::uniform_real_distribution<float> fdist( -1.0f , 1.0f);
    std::uniform_int_distribution<unsigned char> cdist( 0 , 255 );
    
    std::vector<sf::Vertex> vertices;
    
    typename std::chrono::high_resolution_clock::time_point begin , end;
    
    while( window.isOpen() )
    {
        begin = std::chrono::high_resolution_clock::now();
                
        events_loop();
        
        window.clear( sf::Color::Black );    
        vertices.clear();
        
        for( auto& particle : particles )
        {      
            //Matamos a las partículas que se salen de la ventana
            if( particle.current_position().x < 0 || particle.current_position().x > window.getSize().x || 
                particle.current_position().y < 0 || particle.current_position().y > window.getSize().y )
                particle.kill();
            
            if( particle.state() != cpp::particle_state::dead )
            {
                auto distance       = blackhole_position - particle.current_position(); //Vector partícula --> agujero negro
                float square_length = distance.x * distance.x + distance.y * distance.y;
                float length        = std::sqrt( square_length );

                //Volvemos a física del instituto! Fuerza de gravedad es proporcional a la masa e inversamente proporcional al cuadrado de la distancia.
                particle.step( ( blackhole_mass / square_length ) * ( distance / length ) );
            }
            else //Si la partícula está muerta, la reiniciamos (La mandamos a la fuente)
            {
                sf::Color color( cdist( prng ) , cdist( prng ) , cdist( prng ) ); //Color aleatorio
                sf::Vector2f init_speed( fdist( prng ) , fdist( prng ) );         //Dirección de salida aleatoria
                init_speed /= std::sqrt( init_speed.x * init_speed.x + init_speed.y * init_speed.y ) * 10; //Velocidad normalizada a diez pixels/frame


                particle.restart( particles_source , 
                                  color ,
                                  init_speed ,
                                  1000.0f //Masa de la partícula
                                );

            }

            sf::Vertex vertex( particle.current_position() , particle.color );

            vertices.emplace_back( particle.current_position() , particle.color );
        }

        window.draw( vertices.data() , vertices.size() , sf::Points );
        window.display();
        
        end = std::chrono::high_resolution_clock::now();
        std::stringstream ss;
        
        ss << std::fixed << std::setprecision( 0 )
           << "Paticle simulation (" << particles.size() << " particles, " 
           << ( 1000.0f / std::chrono::duration_cast<std::chrono::milliseconds>( end - begin ).count() ) << " FPS)";
        
        window.setTitle( ss.str() );
    }
}

int main()
{
    window.create( sf::VideoMode( 800 , 600 ) , "Particles" );
    
    particles.resize( particles_count );
    
    game_loop();
}


