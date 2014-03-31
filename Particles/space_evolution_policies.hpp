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

#include <SFML/Graphics.hpp>

#include "../snippets/Turbo/to_string.hpp"
#include "../snippets/Turbo/core.hpp"

#include <iostream>
#include <type_traits>

namespace cpp
{
    enum class bounds_state
    {
        inside  , 
        outside ,
        unknown
    };
    
    cpp::bounds_state inverse_bounds_state( cpp::bounds_state state )
    {
        if( state == cpp::bounds_state::inside )  return cpp::bounds_state::outside;
        if( state == cpp::bounds_state::outside ) return cpp::bounds_state::inside;
        if( state == cpp::bounds_state::unknown ) return cpp::bounds_state::unknown;
        throw;
    }
    
    struct bounding_data
    {
        dl32::vector_2df collision_point;
        dl32::vector_2df bounds_normal;
        bounds_state     state;
        
        bounding_data( bounds_state flag = cpp::bounds_state::unknown ,
                        const dl32::vector_2df& normal = dl32::vector_2df{} , 
                        const dl32::vector_2df& point = dl32::vector_2df{}
                      ) :
            collision_point{ point } ,
            bounds_normal{ normal } ,
            state{ flag }
        {}
        
        bounding_data opposite() const
        {
            return { cpp::inverse_bounds_state( state ) , dl32::vector_2df{ -bounds_normal.x , -bounds_normal.y } , collision_point };
        }
    };
    
    
    template<typename BOUNDS>
    struct inverse_bounds
    {
        BOUNDS bounds;
        
        template<typename... ARGS>
        inverse_bounds( ARGS&&... args ) :
            bounds{ std::forward<ARGS>( args )... }
        {}
        
        template<typename POINT>
        cpp::bounding_data operator()( const POINT& point ) const
        {
            return bounds( point ).opposite();
        }
    };
    
    template<typename BOUNDS>
    struct bounds_inverser
    {
        cpp::inverse_bounds<BOUNDS> inversed() const
        {
            return cpp::inverse_bounds<BOUNDS>{ static_cast<const BOUNDS&>( *this ) };
        }
    };
    
    struct rectangle_bounds : public cpp::bounds_inverser<rectangle_bounds>
    {
        cpp::aabb_2d<float> aabb;
        
        rectangle_bounds( cpp::aabb_2d<float> aabb_ ) :
            aabb{ std::move( aabb_ ) }
        {}
        
        template<typename POINT>
        cpp::bounding_data operator()( const POINT& point ) const
        {
            auto relative_position = aabb.relative_position( point );
            
            switch( relative_position )
            {  //Nótese que las normales apuntan hacia el centro
                case cpp::aabb_2d_area::inside: return { cpp::bounds_state::inside };
                case cpp::aabb_2d_area::north:  return { cpp::bounds_state::outside , dl32::vector_2df{ 0.0f , -1.0f } };
                case cpp::aabb_2d_area::south:  return { cpp::bounds_state::outside , dl32::vector_2df{ 0.0f ,  1.0f } };
                case cpp::aabb_2d_area::east:   return { cpp::bounds_state::outside , dl32::vector_2df{-1.0f ,  0.0f } };
                case cpp::aabb_2d_area::west:   return { cpp::bounds_state::outside , dl32::vector_2df{ 1.0f ,  0.0f } };
                case cpp::aabb_2d_area::north_east: return { cpp::bounds_state::outside , aabb.center() - aabb.top_right_corner()    };
                case cpp::aabb_2d_area::north_west: return { cpp::bounds_state::outside , aabb.center() - aabb.top_left_corner()     };
                case cpp::aabb_2d_area::south_east: return { cpp::bounds_state::outside , aabb.center() - aabb.bottom_right_corner() };
                case cpp::aabb_2d_area::south_west: return { cpp::bounds_state::outside , aabb.center() - aabb.bottom_left_corner()  };
                default: throw;
            }
        }
    };
    
    struct circle_bounds : public cpp::bounds_inverser<circle_bounds>
    {
        dl32::vector_2df center;
        float radious;
        
        circle_bounds( const dl32::vector_2df& center_ , float radious_ ) :
            center{ center_ } ,
            radious{ radious_ }
        {}
            
        template<typename POINT>
        cpp::bounding_data operator()( const POINT& point ) const
        {
            if( cpp::wrap( ( point - center ).length() ) > cpp::wrap( radious ) )
                return { cpp::bounds_state::outside , ( center - point ).normalized() };
            else
                return { cpp::bounds_state::inside , ( center - point ).normalized() };
        }
    };
    
    template<typename BOUNDS>
    class bounded_space_evolution_policy
    {
    public:
        template<typename... ARGS>
        bounded_space_evolution_policy( ARGS&&... args ) :
            _bounds{ std::forward<ARGS>( args )... } ,
            _state{ bounds_state::unknown }
        {}
        
        template<typename PARTICLE_DATA>
        void operator()( PARTICLE_DATA& data )
        {
            static const dl32::vector_2df gravity{ 0.00001f , -0.0000998f };
            
            auto collision_data = _bounds( data.position() );
            
            //Si la partícula está atravesando los límites (Antes estaba dentro y ahora está fuera o viceversa):
            if( _state == cpp::bounds_state::inside && collision_data.state == cpp::bounds_state::outside )
            {
                auto input_direction  = data.speed().normalized();
                auto output_direction = input_direction.reflexion( collision_data.bounds_normal ); 

                data.speed() = data.speed().length() * output_direction;
            }
            
            _state = collision_data.state;
        }
        
        void step( cpp::evolution_policy_step step_type )
        {}
        
    private:
        
        BOUNDS       _bounds;
        cpp::bounds_state _state;
    };
    
    template<typename BOUNDS>
    cpp::bounded_space_evolution_policy<BOUNDS> make_bounds_policy( BOUNDS&& bounds )
    {
        return cpp::bounded_space_evolution_policy<BOUNDS>{ std::forward<BOUNDS>( bounds ) };
    }
}

#endif	/* SPACE_EVOLUTION_POLICIES_HPP */

