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

#ifndef PARTICLE_DRAWING_POLICIES_HPP
#define	PARTICLE_DRAWING_POLICIES_HPP

#include <SFML/Graphics.hpp>

namespace cpp
{
    struct pixel_particle_drawing_policy
    {
        pixel_particle_drawing_policy() = default;
        pixel_particle_drawing_policy( const pixel_particle_drawing_policy& ) = default;
        
        //Política de dibujo de una partícula:
        template<typename DATA>
        void operator()( std::vector<sf::Vertex>& pixels , DATA& particle_data ) const
        {
            pixels.emplace_back( sf::Vector2f{ particle_data.position().x , particle_data.position().y } ,
                                 particle_data.color() 
                               );
        }
        
        //Política de dibujo del conjunto de partículas:
        template<typename PARTICLES>
        void operator()( const PARTICLES& particles , sf::RenderTarget& target ) const
        {
            std::vector<sf::Vertex> vertices;
            
            for( auto& particle : particles )
                particle.draw( vertices );
            
            target.draw( vertices.data() , vertices.size() , sf::Points );
        }
    };
}

#endif	/* PARTICLE_DRAWING_POLICIES_HPP */

