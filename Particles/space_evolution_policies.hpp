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

#ifndef SPACE_EVOLUTION_POLICIES_HPP
#define	SPACE_EVOLUTION_POLICIES_HPP

#include "../snippets/aabb_2d.h"
#include "../snippets/math_2d.h"

namespace cpp
{
    
    struct collision_data
    {
        dl32::vector_2df collision_point;
        dl32::vector_2df collision_normal;
        bool collision_flag;
        
        collision_data( bool flag = false ,
                        const dl32::vector_2df& normal = dl32::vector_2df{} , 
                        const dl32::vector_2df& point = dl32::vector_2df{}
                      ) :
            collision_point{ point } ,
            collision_normal{ normal } ,
            collision_flag{ flag }
        {}
            
        explicit operator bool() const
        {
            return collision_flag;
        }
    };
    
    
    struct recltangle_bounds
    {
        cpp::aabb_2d<float> aabb;
        
        recltangle_bounds( cpp::aabb_2d<float> aabb_ ) :
            aabb{ std::move( aabb_ ) }
        {}
        
        template<typename POINT>
        cpp::collision_data operator()( const POINT& point ) const
        {
            auto relative_position = aabb.relative_position( point );
            
            switch( relative_position )
            {  //Nótese que las normales apuntan hacia el centro
                case cpp::aabb_2d_area::inside: return { true };
                case cpp::aabb_2d_area::north:  return { false , dl32::vector_2df{ 0.0f , -1.0f } };
                case cpp::aabb_2d_area::south:  return { false , dl32::vector_2df{ 0.0f ,  1.0f } };
                case cpp::aabb_2d_area::east:   return { false , dl32::vector_2df{-1.0f ,  0.0f } };
                case cpp::aabb_2d_area::west:   return { false , dl32::vector_2df{ 1.0f ,  0.0f } };
                case cpp::aabb_2d_area::north_east: return { false , aabb.center() - aabb.top_right_corner()    };
                case cpp::aabb_2d_area::north_west: return { false , aabb.center() - aabb.top_left_corner()     };
                case cpp::aabb_2d_area::south_east: return { false , aabb.center() - aabb.bottom_right_corner() };
                case cpp::aabb_2d_area::south_west: return { false , aabb.center() - aabb.bottom_left_corner()  };
                default: throw;
            }
        }
    };
    
    template<typename BOUNDS>
    class bounded_space_evolution_policy
    {
    public:
        bounded_space_evolution_policy( const BOUNDS& bounds ) :
            _bounds{ bounds } ,
            _state{ bounds_state::unknown }
        {}
        
        template<typename PARTICLE_DATA>
        void operator()( PARTICLE_DATA& data )
        {
            static const dl32::vector_2df gravity{ 0.00001f , -0.0000998f };
            
            auto collision_data = _bounds( data.position() );
            
            //Si la partícula está saliendo de los límites (Antes estaba dentro y ahora está fuera):
            if( (bool)_state && !collision_data )
            {
                auto input_direction  = data.speed().normalized();
                auto output_direction = input_direction.reflexion( collision_data.collision_normal ); 

                data.speed() = data.speed().length() * output_direction;
            }
            
            data.speed() += gravity;
            
            _state = collision_data.collision_flag ? bounds_state::inside : bounds_state::outside;
        }
        
        void step( cpp::evolution_policy_step step_type )
        {}
        
    private:
        enum class bounds_state : bool
        {
            inside  = true  , 
            outside = false ,
            unknown = false
        };
        
        BOUNDS       _bounds;
        bounds_state _state;
    };
}

#endif	/* SPACE_EVOLUTION_POLICIES_HPP */

