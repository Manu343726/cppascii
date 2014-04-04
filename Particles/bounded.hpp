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
#include "type_erased_evolution_policy.hpp"

#include "../snippets/math_2d.h"

namespace cpp
{
    namespace bounded
    {
        
        struct bounded_engine : public cpp::basic_particle_engine
        {
            using obstacle_t = cpp::inverse_bounds<cpp::circle_bounds>;
            using bounds_t   = cpp::rectangle_bounds;
        
            using particle = cpp::policied_particle<cpp::default_particle_data_holder,
                                                    cpp::evolution_policies_pipeline<cpp::default_particle_data_holder>,
                                                    cpp::pixel_particle_drawing_policy>;
        
            void initialize( std::size_t particles_count , const dl32::vector_2df& begin , float speed , const cpp::evolution_policies_pipeline<cpp::default_particle_data_holder>& pipeline )
            {
                std::mt19937 prng;
                std::uniform_real_distribution<float> dist{ 0.0f , 2.0f * 3.141592654f };
                
                for( std::size_t i = 0 ; i < particles_count ; ++i )
                {
                    float angle = dist( prng );
                    dl32::vector_2df particle_speed{ std::cos( angle ) * speed , std::sin( angle ) * speed };
                    
                    _particles.emplace_back( typename particle::data_policy_t{ begin , particle_speed , sf::Color::White } ,
                                             pipeline ,
                                             typename particle::drawing_policy_t{} 
                                           );
                }
            }
                
            template<typename CANVAS>
            void draw( CANVAS& canvas ) const
            {
                cpp::basic_particle_engine::draw( _particles , cpp::pixel_particle_drawing_policy{} , canvas );
            }
            
            void step()
            {
                cpp::basic_particle_engine::step( _particles );
            }
                
        private:
            std::vector<particle> _particles;
        };
    }
}

#endif	/* BOUNDED_RACTANGLE_HPP */

