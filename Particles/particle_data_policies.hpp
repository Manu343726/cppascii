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

#ifndef DATA_POLICIES_HPP
#define	DATA_POLICIES_HPP

#include "../snippets/math_2d.h"

#include <SFML/Graphics.hpp>

namespace cpp
{
    /* Ésta clase encapsula los datos de una partícula (Que pueden ser muy variados y representados de maneras muy diferentes) en una interfaz concreta.
     * Se entiende que cualquier clase con esta interfaz es una policy class que encapsula datos de una partícula.
     * Ésta es solo la policy class (Implementación de la policy) por defecto. 
     *
     * Por defecto se entiende que una partícula tiene posición, velocidad, y color
     */
    class default_particle_data_holder
    {
        sf::Color _color;
        dl32::vector_2df _position , _speed;
        
    public:
        default_particle_data_holder() :
        _color{ sf::Color::White }
        {}
        
        template<typename POSITION , typename SPEED , typename COLOR>
        default_particle_data_holder( const POSITION& position , const SPEED& speed , const COLOR& color ) :
            _color{ color } , 
            _position{ position } ,
            _speed{ speed }
        {}
            
        sf::Color& color()
        {
            return _color;
        }
        
        sf::Color color() const
        {
            return _color;
        }
        
        dl32::vector_2df& position()
        {
            return _position;
        }
        
        dl32::vector_2df position() const
        {
            return _position;
        }
        
        dl32::vector_2df& speed()
        {
            return _speed;
        }
        
        dl32::vector_2df speed() const
        {
            return _speed;
        }
    };
}

#endif	/* DATA_POLICIES_HPP */

