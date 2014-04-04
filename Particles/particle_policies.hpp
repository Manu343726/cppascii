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


/* Un sencillo ejemplo de policy-based dessign: Una biblioteca para definir sistemas de partículas */

#ifndef PARTICLE_POLICIES_HPP
#define	PARTICLE_POLICIES_HPP

#include <functional>
#include <map>
#include <exception>
#include <algorithm>

#include "../snippets/Turbo/core.hpp"

#include "particle_evolution_policies.hpp"
#include "lifetime_evolution_policies.hpp"

namespace cpp
{
    /* Una partícula está formada por sus datos (Posición, velocidad, color, etc), la política con la que éstos evolucionan, y la política con la que se muestra (dibuja) */
    
    template<typename DATA_POLICY , typename EVOLUTION_POLICY , typename DRAWING_POLICY>
    class policied_particle
    {    
    public:
        using data_policy_t      = DATA_POLICY;
        using evolution_policy_t = EVOLUTION_POLICY;
        using drawing_policy_t   = DRAWING_POLICY;
        
        using my_type = cpp::policied_particle<DATA_POLICY,EVOLUTION_POLICY,DRAWING_POLICY>;
        
        policied_particle( const data_policy_t& data_policy , const evolution_policy_t& evolution_policy , const drawing_policy_t& drawing_policy ) :
            _data_policy{ data_policy } ,
            _evolution_policy{ evolution_policy }
        {}

        void step()
        {
            _data_policy.position() += _data_policy.speed();
            
            cpp::policy_call( _evolution_policy , _data_policy ); //Ejecutamos la política de evolución de los datos
            
            cpp::policy_step<DATA_POLICY>( _evolution_policy , cpp::evolution_policy_step::individual );
        }
        
        template<typename CANVAS>
        void draw( CANVAS& canvas ) const
        {
            _drawing_policy( canvas , _data_policy ); //Ejecutamos la política de dibujo de los datos sobre un canvas dado
        }
        
    private:     
        data_policy_t      _data_policy;
        evolution_policy_t _evolution_policy;
        drawing_policy_t   _drawing_policy;
        
        //using particle_size = decltype( tml::size_of<DATA_POLICY>{} + tml::size_of<EVOLUTION_POLICY>{} + tml::size_of<DRAWING_POLICY>{} );
        
        //TURBO_ASSERT( ( tml::less_or_equal<particle_size,tml::size_t<50>> ) , "Too much fatty particle" );
    };
    
    struct basic_particle_engine
    {
    private:
        template<typename PARTICLE_DATA>
        void step_evolution_policies() const
        {}
        
        template<typename PARTICLE_DATA , typename HEAD , typename... TAIL>
        void step_evolution_policies( HEAD& head , TAIL&... tail ) const
        {
            cpp::policy_step<PARTICLE_DATA>( head , cpp::evolution_policy_step::global );
            
            step_evolution_policies<PARTICLE_DATA>( tail... );
        }
        
    protected:
        template<typename PARTICLES , typename... EVOLUTION_POLICIES>
        void step( PARTICLES& particles , EVOLUTION_POLICIES&... evolution_policies) const
        {
            using particle_type = typename std::remove_reference<decltype( *(std::declval<decltype( std::begin( particles ) )>()) )>::type;
            using particle_data = typename particle_type::data_policy_t;
            
            for( auto& particle : particles )
                particle.step();
            
            step_evolution_policies<particle_data>( evolution_policies... );
        }
        
        template<typename PARTICLES , typename DRAWING_POLICY , typename CANVAS>
        void draw( PARTICLES& particles , DRAWING_POLICY drawing_policy , CANVAS& canvas ) const
        {
            drawing_policy( particles , canvas );
        }
    };
}

#endif	/* FIREWORKS_HPP */

