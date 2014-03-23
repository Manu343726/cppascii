/* 
 * File:   bounded_ractangle.hpp
 * Author: manu343726
 *
 * Created on 23 de marzo de 2014, 16:53
 */

#ifndef BOUNDED_RACTANGLE_HPP
#define	BOUNDED_RACTANGLE_HPP

#include "particle_policies.hpp"
#include "particle_data_policies.hpp"
#include "lifetime_evolution_policies.hpp"
#include "space_evolution_policies.hpp"
#include "particle_drawing_policies.hpp"

#include "../snippets/math_2d.h"

namespace cpp
{
    namespace bounded
    {
        
        using particle = cpp::policied_particle<cpp::default_particle_data_holder,
                                                cpp::bounded_space_evolution_policy<cpp::recltangle_bounds>,
                                                cpp::pixel_particle_drawing_policy>;
        
        struct bounded_engine : public cpp::particle_engine
        {
            bounded_engine( std::size_t particles_count , const dl32::vector_2df& begin , float speed , const cpp::aabb_2d<float>& bounds )
            {
                std::mt19937 prng;
                std::uniform_real_distribution<float> dist{ 0.0f , 2.0f * 3.141592654f };
                
                for( std::size_t i = 0 ; i < particles_count ; ++i )
                {
                    float angle = dist( prng );
                    dl32::vector_2df particle_speed{ std::cos( angle ) * speed , std::sin( angle ) * speed };
                    
                    _particles.emplace_back( typename cpp::bounded::particle::data_policy_t{ begin , particle_speed , sf::Color::White } ,
                                             typename cpp::bounded::particle::evolution_policy_t{ bounds } ,
                                             typename cpp::bounded::particle::drawing_policy_t{} 
                                           );
                }
            }
                
            template<typename CANVAS>
            void draw( CANVAS& canvas ) const
            {
                cpp::particle_engine::draw( _particles , cpp::pixel_particle_drawing_policy{} , canvas );
            }
            
            void step()
            {
                cpp::particle_engine::step( _particles );
            }
                
        private:
            std::vector<cpp::bounded::particle> _particles;
        };
    }
}

#endif	/* BOUNDED_RACTANGLE_HPP */

