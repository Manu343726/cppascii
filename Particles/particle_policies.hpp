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
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>

namespace cpp
{
    
    /* Ésta clase encapsula los datos de una partícula (Que pueden ser muy variados y representados de maneras muy diferentes) en una interfaz concreta.
     * Se entiende que cualquier clase con esta interfaz es una policy class que encapsula datos de una partícula.
     * Ésta es solo la policy class (Implementación de la policy) por defecto. */
    class default_particle_data_holder
    {
        sf::Vertex _color_position;
        sf::Vector2f _speed;
        
    public:
        default_particle_data_holder() = default;
        
        template<typename POSITION , typename SPEED , typename COLOR>
        default_particle_data_holder( const POSITION& position , const SPEED& speed , const COLOR& color ) :
            _color_position{ position , color } ,
            _speed{ speed }
        {}
            
        sf::Color& color()
        {
            return _color_position.color;
        }
        
        sf::Color color() const
        {
            return _color_position.color;
        }
        
        sf::Vector2f& position()
        {
            return _color_position.position;
        }
        
        sf::Vector2f position() const
        {
            return _color_position.position;
        }
        
        sf::Vector2f& speed()
        {
            return _speed;
        }
        
        sf::Vector2f speed() const
        {
            return _speed;
        }
        
        sf::Vertex vertex() const
        {
            return _color_position;
        }
    };
        
    
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
        
        void step() 
        {
            if( is_alive() ) _life_ahead--;
        }
        
        void respawn() 
        {
            _life_ahead = _lifetime;
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
    template<typename PARTICLE_DATA_HOLDER = cpp::default_particle_data_holder>
    struct configurable_lifetime_policy : public cpp::lifetime_policy<cpp::particle_birth_action<PARTICLE_DATA_HOLDER>,
                                                                      cpp::particle_life_action<PARTICLE_DATA_HOLDER>,
                                                                      cpp::particle_death_action<PARTICLE_DATA_HOLDER>
                                                                     >
    {
        configurable_lifetime_policy( int lifetime = 0 , 
                                      const cpp::particle_birth_action<PARTICLE_DATA_HOLDER>& birth_policy = []( PARTICLE_DATA_HOLDER& ) {} ,
                                      const cpp::particle_life_action<PARTICLE_DATA_HOLDER>&  life_policy  = []( PARTICLE_DATA_HOLDER& , float ) {} ,
                                      const cpp::particle_death_action<PARTICLE_DATA_HOLDER>& death_policy = []( PARTICLE_DATA_HOLDER& ) {}
                                    ) :
                                        cpp::lifetime_policy<cpp::particle_birth_action<PARTICLE_DATA_HOLDER>,
                                                             cpp::particle_life_action<PARTICLE_DATA_HOLDER>,
                                                             cpp::particle_death_action<PARTICLE_DATA_HOLDER>
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
    
        
    /* Una partícula está formada por sus datos (Posición, velocidad, color, etc) y la política con la que éstos evolucionan */
    
    template<typename DATA = cpp::default_particle_data_holder , typename LIFETIME_POLICY = cpp::configurable_lifetime_policy<DATA>>
    class policied_particle
    {
    private:
        DATA particle_data;
        std::reference_wrapper<LIFETIME_POLICY> lifetime_policy; //Nótese que es una referencia: Diferentes partículas comparten
                                                                 //la misma política.

    public:
        policied_particle( const DATA& data , const std::reference_wrapper<LIFETIME_POLICY>& policy ) :
            particle_data{ data } ,
            lifetime_policy{ policy }
        {}

        void step()
        {
            particle_data.position() += particle_data.speed(); //La partícula avanza
            
            lifetime_policy.get()( particle_data ); //Ejecutamos la política de evolución de los datos
        }
        
        bool is_alive() const
        {
            return lifetime_policy.get().is_alive();
        }
        
        sf::Vertex vertex() const
        {
            return particle_data.vertex();
        }
    };
}

#endif	/* FIREWORKS_HPP */

