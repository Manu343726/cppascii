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

#ifndef LIFETIME_EVOLUTION_POLICIES_HPP
#define	LIFETIME_EVOLUTION_POLICIES_HPP

#include "particle_evolution_policies.hpp"

#include <functional>
#include <map>


namespace cpp
{    
    /* Una partícula nace, vive, y muere: Tratamos esos tres aspectos de manera independiente. 
       Esta clase representa la política de evolución de una partícula como el agregado de las tres 
       políticas de nacimiento, vida, y muerte 
     */
    template<typename BIRTH , typename LIFE , typename DEATH>
    struct lifetime_policy
    {
    private:
        int _life_ahead , _lifetime;
        
        BIRTH birth;
        LIFE life;
        DEATH death;
        
    protected:
        void respawn()
        {
            _life_ahead = _lifetime;
        }
        
    public:
        lifetime_policy( int lifetime = 0 , const BIRTH& birth_policy = BIRTH{} , const LIFE& life_policy = LIFE{} , const DEATH& death_policy = DEATH{} ) :
            _life_ahead{ lifetime } ,
            _lifetime{ lifetime } ,
            birth{ birth_policy } ,
            life{ life_policy } ,
            death{ death_policy }
        {}
            
        bool is_alive() const 
        {
            return _life_ahead >= 0;
        }
        
        explicit operator bool() const
        {
            return is_alive();
        }
        
        template<typename PARTICLE_DATA>
        void operator()( PARTICLE_DATA& particle_data )
        {   
            if( _life_ahead == _lifetime ) birth( particle_data );
            if( _life_ahead >  0 )         life( particle_data , 1.0f - ( (float)_life_ahead / _lifetime ) ); //A la política de vida se le pasa un segundo argumento en el intervalo (0,1) que indica la edad de la partícula
            if( _life_ahead == 0 )         death( particle_data );
        }   
        
        void step( cpp::evolution_policy_step step_type ) 
        {
            if( is_alive() && step_type == cpp::evolution_policy_step::global ) _life_ahead--;
        }
    };
    
    //Estas son las signaturas de las diferentes acciones que se pueden usar
    //como política de evolución de una partícula a lo largo de sus diferentes etapas
    //(Ver nota siguiente)
    
    template<typename PARTICLE_DATA>
    using particle_birth_action = std::function<void(PARTICLE_DATA&)>;
    
    template<typename PARTICLE_DATA>
    using particle_life_action = std::function<void(PARTICLE_DATA& , float age )>;
    
    template<typename PARTICLE_DATA>
    using particle_death_action = std::function<void(PARTICLE_DATA&)>;
    

    //Pero, ¿Y si queremos configurar las tres políticas en tiempo de ejecución? Esta clase encapsula una política de evolución parametrizando su construcción con tres
    //funciones que ejecutan las tres políticas antes mencionadas:
    template<typename PARTICLE_DATA_POLICY>
    struct configurable_lifetime_policy : public cpp::lifetime_policy<cpp::particle_birth_action<PARTICLE_DATA_POLICY>,
                                                                      cpp::particle_life_action<PARTICLE_DATA_POLICY>,
                                                                      cpp::particle_death_action<PARTICLE_DATA_POLICY>
                                                                     >
    {
        configurable_lifetime_policy( int lifetime = 0 , 
                                      const cpp::particle_birth_action<PARTICLE_DATA_POLICY>& birth_policy = []( PARTICLE_DATA_POLICY& ) {} ,
                                      const cpp::particle_life_action<PARTICLE_DATA_POLICY>&  life_policy  = []( PARTICLE_DATA_POLICY& , float ) {} ,
                                      const cpp::particle_death_action<PARTICLE_DATA_POLICY>& death_policy = []( PARTICLE_DATA_POLICY& ) {}
                                    ) :
                                        cpp::lifetime_policy<cpp::particle_birth_action<PARTICLE_DATA_POLICY>,
                                                             cpp::particle_life_action<PARTICLE_DATA_POLICY>,
                                                             cpp::particle_death_action<PARTICLE_DATA_POLICY>
                                                            >
                                       { lifetime ,
                                         birth_policy , 
                                         life_policy  ,
                                         death_policy 
                                       }
        {}
    };


    /* La vida de una partícula puede ser muy larga... ¿Y si quiero especificar diferentes políticas a lo largo de su vida? 
       Esta clase especifica diferentes políticas para diferentes intervalos en la vida de una partícula: */
    template<typename PARTICLE_DATA>
    struct segmented_life_policy
    {
        using action_type = cpp::particle_life_action<PARTICLE_DATA>;
        using segment_action_pair_type = std::pair<float,action_type>;
        
        //Son ejemplos, no tengo la eficiencia en mente. Lo suyo sería que el conjunto de pares (intervalo,política) fuera compartido por varias (todas) las partículas,
        //no tener el mismo conjunto duplicado para cada partícula.
        std::map<float,action_type> segments; 

        //Vale, lo admito, pregunté: http://stackoverflow.com/questions/22283108/mapping-float-value-to-function-depending-on-specified-intervals
        
        segmented_life_policy( const decltype( segments )& segs ) : segments{ segs }
        {}
        
        segmented_life_policy( const std::initializer_list<segment_action_pair_type>& pairs ) :
            segments{ pairs }
        {}

        void operator()( PARTICLE_DATA& particle_data , float age ) const
        {
            auto segment_it = segments.lower_bound( age );

            static_assert( std::is_same<typename decltype(segment_it)::value_type::second_type,action_type>::value , "Ooops" );

            if( segment_it != std::end( segments ) )
                (segment_it->second)( particle_data , age );
            else
                throw std::invalid_argument{ "Incomplete lifetime segments specification: No segment matches this age" };
        }
    };
    
    
    //No me gustan tantas llaves, vamos a hacer un builder. (Bueno vale, al final será lo mismo pero con () en lugar de {},
    //era por poner un ejemplo de éste útil patrón más que nada).
    template<typename PARTICLE_DATA>
    struct segmented_life_policy_builder
    {
        using action_type = cpp::particle_life_action<PARTICLE_DATA>;
        using segment_action_pair_type = std::pair<float,action_type>;

        std::map<float,action_type> segments; 
        
        
        
        //Para qué hacer interfaces fluidas con . cuando tenemos (), a lo python?   
        
        //Sobrecarga para funciones normales:
        segmented_life_policy_builder& operator()( float segment_end , const action_type& action )
        {
            segments[ segment_end ] = action;
            
            return *this;
        }
        
        //Sobrecarga para funciones miembro:
        template<typename F , typename THIS_REF>
        segmented_life_policy_builder& operator()( float segment_end , F&& function , THIS_REF&& this_ref )
        {
            using namespace std::placeholders;
            
            action_type action = std::bind( std::forward<F>( function ) , std::ref( std::forward<THIS_REF>( this_ref ) ) , _1 , _2 );
            
            return (*this)( segment_end , action );
        }
        
        
        //Finalemente devolvemos el mapa. Nótese que lo movemos explícitamente: La instancia del builder
        //es un rvalue al fin y al cabo. Por extensión el mapa guardado por él también, así que evitamos copias.
        //Habría hecho una RVO el optimizador? Si no recuerdo mal no, porque el mapa pertenece al objeto, 
        //no a la función miembro. No es tán inteligente como para ver que el objeto es un rvalue y darse cuenta del
        //uso que le estás dando al mapa. (No estoy seguro al 100%)
        //Una vez más, es mejor no divagar y mirar el código generado.
        operator std::map<float,action_type> ()
        {
            return std::move( segments );
        }
    };
    
    template<typename DATA>
    cpp::segmented_life_policy_builder<DATA> build_segmented_policy()
    {
        return cpp::segmented_life_policy_builder<DATA>{};
    }
}

#endif	/* LIFETIME_EVOLUTION_POLICIES_HPP */

