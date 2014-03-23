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

#ifndef PARTICLE_EVOLUTION_POLICIES_HPP
#define	PARTICLE_EVOLUTION_POLICIES_HPP

#include <functional>
#include <memory>

#include "lifetime_evolution_policies.hpp"

namespace cpp
{
    /* Esta plantilla define un wrapper de una política de evolución a la que se le aplica type-erasure.
       Esto permite que el usuario utilice la política directamente (POLICY) o la comparta mediante conteo de referencias
       ( std::shared_ptr<POLICY> ) o referencias ( std::reference_wrapper<POLICY> ). */
    
    template<typename T>
    class evolution_policy
    {
    public:
        template<typename... ARGS>
        evolution_policy( ARGS&&... args ) : 
            _policy{ std::forward<ARGS>( args )... }
        {}
            
        template<typename... ARGS>
        void operator()( ARGS&&... args ) const
        {
            _policy( std::forward<ARGS>( args )... );
        }

        template<typename... ARGS>
        void operator()( ARGS&&... args )
        {
            _policy( std::forward<ARGS>( args )... );
        }
        
        void step( cpp::evolution_policy_step step_type )
        {
            _policy.step( step_type );
        }
            
    private:
        T _policy;
    };
    
    template<typename T>
    class evolution_policy<std::shared_ptr<T>>
    {
    public:
        template<typename... ARGS>
        evolution_policy( ARGS&&... args ) : 
            _policy{ std::make_shared( std::forward<ARGS>( args )... ) }
        {}
            
        template<typename... ARGS>
        void operator()( ARGS&&... args ) const
        {
            (*_policy)( std::forward<ARGS>( args )... );
        }

        template<typename... ARGS>
        void operator()( ARGS&&... args )
        {
            (*_policy)( std::forward<ARGS>( args )... );
        }
        
        void step( cpp::evolution_policy_step step_type )
        {
            _policy->step( step_type );
        }
            
    private:
        std::shared_ptr<T> _policy;
    };
    
    
    template<typename T>
    class evolution_policy<std::reference_wrapper<T>>
    {
    public:
        template<typename... ARGS>
        evolution_policy( ARGS&&... args ) : 
            _policy{ T{ std::forward<ARGS>( args )... } }
        {}
            
        evolution_policy( const T& ref ) :
            _policy{ ref }
        {}
            
        evolution_policy( T& ref ) :
            _policy{ ref }
        {}
            
        evolution_policy( const std::reference_wrapper<T>& ref_wrapper ) :
            _policy{ ref_wrapper }
        {}
        
            
        template<typename... ARGS>
        void operator()( ARGS&&... args ) const
        {
            _policy( std::forward<ARGS>( args )... );
        }

        template<typename... ARGS>
        void operator()( ARGS&&... args )
        {
            _policy( std::forward<ARGS>( args )... );
        }
        
        void step( cpp::evolution_policy_step step_type )
        {
            _policy.get().step( step_type );
        }
            
    private:
        std::reference_wrapper<T> _policy;
    };
    
}

#endif	/* PARTICLE_EVOLUTION_POLICIES_HPP */

