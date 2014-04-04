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

#ifndef FIREWORKS_HPP
#define	FIREWORKS_HPP

#include "particle_policies.hpp"
#include "particle_data_policies.hpp"
#include "lifetime_evolution_policies.hpp"
#include "space_evolution_policies.hpp"
#include "particle_drawing_policies.hpp"

#include "../snippets/math_2d.h"
#include "particle_evolution_policies.hpp"

#include <random>
#include <iostream>

using namespace std::placeholders;

namespace cpp
{
    namespace fireworks
    {
        template<typename DATA>  //Una política es una política: Bien, parece que la herencia tiene sentido
        class firework_lifetime_policy : public cpp::lifetime_policy<cpp::particle_birth_action<DATA> , cpp::segmented_life_policy<DATA> , cpp::particle_death_action<DATA>>
        {
        private:
            dl32::vector_2df begin;
            float init_speed , grow , degrow;
            
            std::mt19937 prng;
            std::uniform_real_distribution<float> dist;
            
            using birth_policy_type = cpp::particle_birth_action<DATA>;
            using life_policy_type  = cpp::segmented_life_policy<DATA>;
            using death_policy_type = cpp::particle_death_action<DATA>;
            using lifetime_policy_type = cpp::lifetime_policy<birth_policy_type , life_policy_type , death_policy_type>;
            
        public:
            
            firework_lifetime_policy( int lifetime , const dl32::vector_2df begin_ , float speed , float grow_ , float degrow_ ,
                                      float end_child = 0.3f , float end_adult = 0.6f) :
                lifetime_policy_type //Inicializamos la política subyacente (tiempo de vida, políticas de nacimiento, vida, y muerte)
                {
                    lifetime , 
                    birth_policy_type{ std::bind( &firework_lifetime_policy::birth_policy , std::ref( *this ) , _1 ) } ,
                    life_policy_type{ cpp::build_segmented_policy<DATA>() //Python, haha!
                                      ( end_child , &firework_lifetime_policy::first_phase_life_policy  , *this ) //Una vez más confirmamos que los punteros a funciones miembro SON UNA PUTA MIERDA
                                      ( end_adult , &firework_lifetime_policy::second_phase_life_policy , *this )
                                      ( 1.0f      , &firework_lifetime_policy::third_phase_life_policy  , *this )
                                    } ,
                    death_policy_type{ std::bind( &firework_lifetime_policy::death_policy , std::ref( *this ) , _1 ) }
                } ,
                begin{ begin_ } ,
                init_speed{ speed } ,
                prng{ std::random_device{}() } ,
                dist{ 0.0f , 2.0f*3.141592654f } ,
                grow{ grow_ } ,
                degrow{ degrow_ }
            {}   
                 
                
                
                
            //Políticas de evolución de las partículas:    
                
            
            //Al nacer se posicionan en el centro con una diracción de salida aleatoria.
            //La idea es que si la distribución es uniforme (Que lo es, véase el PRNG y distribución usados)
            //parecerá una explosión circular:
            void birth_policy( DATA& particle_data ) 
            {
                //std::cout << "Ohh thats a cute new baby particle" << std::endl;
                
                float angle = dist( prng );
                
                particle_data.position() = begin;
                particle_data.speed() = { std::cos( angle ) * init_speed , std::sin( angle ) * init_speed };
                particle_data.color() = sf::Color::White;
            }
            
            //Al morir se vuelvem magenta:
            void death_policy( DATA& particle_data )
            {
                //std::cout << "A particle is dying..." << std::endl;
                
                particle_data.color() = sf::Color::Magenta;
                
                //Siento si soy brusco, pero: QUIEN PONGA UN IF AQUÍ PARA NO MULTIPLICAR TODO EL RATO UNA VEZ QUE LA
                //PARTÍCULA MUERTA HA PARADO NO TIENE NI PUTA IDEA NI DE PROGRAMAR NI DE COMO FUNCIONA EL HARDWARE HOY EN DÍA
                particle_data.speed() *= 0.0f; //Los muertos no se mueven!
                
                this->respawn();
                 
                std::uniform_real_distribution<float> dist_x{ 100.0f , 700.0f } , dist_y{ 100.0f , 500.0f };
                
                begin.x = dist_x( prng );
                begin.y = dist_y( prng );
            }
            
            //Cuando son "niñas" (Primer tercio de su vida) son rojas:
            void first_phase_life_policy( DATA& particle_data , float age ) const
            {
                //std::cout << "A child particle!" << std::endl;
                
                particle_data.color() = sf::Color::Red;
                
                particle_data.speed() *= grow; //Los niños son muy acelarados...
            }
            
            //Cuando son "adultas" (Segundo tercio de su vida) son verdes:
            void second_phase_life_policy( DATA& particle_data , float age ) const
            {
                //std::cout << "An adult particle" << std::endl;
                
                particle_data.color() = sf::Color::Green;
            }
            
            //Cuando son "ancianas" (Tercer tercio de su vida) son azules:
            void third_phase_life_policy( DATA& particle_data , float age ) const
            {
                //std::cout << "I'm a pretty old particle..." << std::endl;
                
                particle_data.color() = sf::Color::Blue;
                
                particle_data.speed() *= degrow; //Los mayores cada vez van más despacio...
            }
        };
        
        using lifetime_policy = cpp::fireworks::firework_lifetime_policy<cpp::default_particle_data_holder>;
        using shared_lifetime_policy = cpp::shared_policy<cpp::fireworks::lifetime_policy>;
        
        //Una partícula de nuestro sistema de fuegos artificiales es una partícula que usa como política la política
        //del sistema de fuegos artificiales (Valga la redundancia...):
        struct particle : public cpp::policied_particle<cpp::default_particle_data_holder,
                                                        cpp::fireworks::shared_lifetime_policy,
                                                        cpp::pixel_particle_drawing_policy>
        {
            using base = cpp::policied_particle<cpp::default_particle_data_holder,
                                                        cpp::fireworks::shared_lifetime_policy,
                                                        cpp::pixel_particle_drawing_policy>;
            
        
            
            particle( const cpp::fireworks::shared_lifetime_policy& policy ) :
                base
                {
                    cpp::default_particle_data_holder{} , //Inicializamos los datos de la partícula por defecto
                                                          //Al fin y al cabo se van a "inicializar" cuando nazcan 
                                                          //(Ver políticas de evolución más arriba)
                    policy ,
                    cpp::pixel_particle_drawing_policy{}
                }
            {}
        };
        
        
        //Y finalmente el motor del sistema de "fuegos artificiales":
        struct fireworks_engine : public cpp::basic_particle_engine
        {
        private:
            std::vector<cpp::fireworks::particle> particles_; //Conjunto de partículas
            
            //Política de evolución de las partículas:
            
            shared_lifetime_policy particles_lifetime_policy , team_a , team_b , team_c; 
            
            
        public:
            fireworks_engine( int lifetime , const dl32::vector_2df& center , float speed ) :
                particles_lifetime_policy{ std::make_shared<lifetime_policy>( lifetime , center , speed , 1.0003f , 0.9997f ) } ,
                team_a{ std::make_shared<lifetime_policy>( lifetime , center                                   , speed      , 1.0003f , 0.9998f , 0.3f  , 0.6f  ) } ,
                team_b{ std::make_shared<lifetime_policy>( lifetime , center + dl32::vector_2df{ 1.0f , 1.0f } , speed*1.0f , 1.0006f , 0.9997f , 0.2f  , 0.24f ) } ,
                team_c{ std::make_shared<lifetime_policy>( lifetime , center - dl32::vector_2df{ 1.0f , 1.0f } , speed*1.1f , 1.003f  , 0.9992f , 0.04f , 0.5f  ) }
            {
                
                //Las partículas guardan una referencia a la política de evolución que siguen:
                cpp::fireworks::particle main_particle{ particles_lifetime_policy };
                particles_.assign( 1000u , main_particle );       
                
                
                
                //Las partículas guardan una referencia a la política de evolución que siguen:
                cpp::fireworks::particle team_a_particle{ team_a  };
                particles_.insert( std::end( particles_ ) , 1000u , team_a_particle );   
                
                //Las partículas guardan una referencia a la política de evolución que siguen:
                cpp::fireworks::particle team_b_particle{ team_b };
                particles_.insert( std::end( particles_ ) , 1000u , team_b_particle );   
                
                //Las partículas guardan una referencia a la política de evolución que siguen:
                cpp::fireworks::particle team_c_particle{ team_c };
                particles_.insert( std::end( particles_ ) , 1000u , team_c_particle );            
            }
                
                
            //No hay que ser un gurú de C++ para entender las dos funciones de debajo...
                
            template<typename CANVAS>
            void draw( CANVAS& canvas ) const
            {
                cpp::basic_particle_engine::draw( particles_ , cpp::pixel_particle_drawing_policy{} , canvas );
            }
            
            void step()
            {
                cpp::basic_particle_engine::step( particles_ , particles_lifetime_policy ,
                                                         team_a ,
                                                         team_b ,
                                                         team_c 
                                          );
            }
        };
    }
}

#endif	/* FIREWORKS_HPP */

