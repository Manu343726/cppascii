/* 
 * File:   main.cpp
 * Author: manu343726
 *
 * Created on 16 de enero de 2014, 9:15
 */

#include "particle.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <vector>
#include <unordered_map>
#include <random>
#include <iostream>


constexpr std::size_t particles_count = 10000;
constexpr float blackhole_mass        = 10000.0f;
constexpr float blackhole_deadzone    = 10.0f;

std::vector<cpp::particle> particles;
sf::Vector2f particles_source;

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
                particles_source.x = event_data.mouseMove.x;
                particles_source.y = event_data.mouseMove.y;
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
    
    sf::Vector2f blackhole_position( window.getSize().x / 2.0f , window.getSize().y / 2.0f );
    
    std::vector<sf::Vertex> vertices;
    
    while( window.isOpen() )
    {
        events_loop();
        
        window.clear( sf::Color::Black );    
        vertices.clear();
        
        for( auto& particle : particles )
        {                    
            if( particle.current_position().x < 0 || particle.current_position().x > window.getSize().x || 
                particle.current_position().y < 0 || particle.current_position().y > window.getSize().y )
                particle.kill();

            if( particle.state() != cpp::particle_state::dead )
            {
                auto distance       = particle.current_position() - blackhole_position;
                float square_length = distance.x * distance.x + distance.y * distance.y;
                float length        = std::sqrt( square_length );

                particle.step( ( blackhole_mass / square_length ) * ( distance / length ) );
            }
            else
            {
                sf::Color color( cdist( prng ) , cdist( prng ) , cdist( prng ) );
                sf::Vector2f init_speed( fdist( prng ) , fdist( prng ) );
                init_speed /= std::sqrt( init_speed.x * init_speed.x + init_speed.y * init_speed.y ) * 10;


                particle.restart( particles_source , 
                                  color ,
                                  init_speed ,
                                  1000.0f
                                );

            }
            
            std::cout << particle.current_position().x << std::endl;

            sf::Vertex vertex( particle.current_position() , particle.color );

            vertices.emplace_back( particle.current_position() , particle.color );
        }

        window.draw( vertices.data() , vertices.size() , sf::Points );
        window.display();
    }
}

int main()
{
    window.create( sf::VideoMode( 800 , 600 ) , "Particles" );
    
    particles.resize( particles_count );
    
    game_loop();
}


