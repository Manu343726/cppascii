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

#ifndef PARTICLE_HPP
#define	PARTICLE_HPP

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

namespace cpp
{
    enum class particle_state
    {
        alive ,     //viva
        dead  ,     //muerta
        no_lifetime //no se tiene en cuenta el tiempo de vida (Partícula inmortal)
    };
    
    class particle
    {
    private:
        sf::Vector2f position , speed;
        int          life_ahead;
        float        mass;
        
    public:
        sf::Color    color;
        
        particle() : 
            position()      ,
            speed()         ,
            life_ahead( 0 ) , //Nace muerta
            mass()
        {
        }
        
        particle( const sf::Vector2f& init_pos , const sf::Color& color , const sf::Vector2f& init_speed , float mass , int lifetime = -1 ) :
            position( init_pos ) ,
            speed( init_speed )  ,
            color( color )       ,
            mass( mass )         ,
            life_ahead( lifetime )
        {}
        
        void restart( const sf::Vector2f& init_pos , const sf::Color& color , const sf::Vector2f& init_speed , float mass , int lifetime = -1 )
        { 
            position   = init_pos;
            speed      = init_speed;
            this->color      = color;
            this->mass       = mass;
            life_ahead = lifetime;
        }
        
        void kill()
        {
            life_ahead = 0;
        }
        
        cpp::particle_state state() const
        {
            if( life_ahead  < 0 ) return cpp::particle_state::no_lifetime;
            if( life_ahead == 0 ) return cpp::particle_state::dead;
            if( life_ahead  > 0 ) return cpp::particle_state::alive;
        }
        
        const sf::Vector2f& current_position() const
        {
            return position;
        }
        
        const sf::Vector2f& current_speed() const
        {
            return speed;
        }
        
        void step( const sf::Vector2f& force )
        {
            if( state() == cpp::particle_state::dead ) return;
            
            auto acceleration = force / mass;
            
            speed    += acceleration;
            position += speed;
            
            if( life_ahead > 0 )
                life_ahead--;
        }
    };
}

#endif	/* PARTICLE_HPP */

