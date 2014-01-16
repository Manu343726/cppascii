/* 
 * File:   particle.hpp
 * Author: manu343726
 *
 * Created on 16 de enero de 2014, 9:17
 */

#ifndef PARTICLE_HPP
#define	PARTICLE_HPP

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

namespace cpp
{
    enum class particle_state
    {
        alive ,     //viva
        dead  ,     //muerta
        no_lifetime //no se tiene en cuenta el tiempo de vida (Partícula inmortal)
    };
    
    class particle
    {
    private:
        sf::Vector2f position , speed;
        int          life_ahead;
        float        mass;
        
    public:
        sf::Color    color;
        
        particle() : 
            position()      ,
            speed()         ,
            life_ahead( 0 ) , //Nace muerta
            mass()
        {
        }
        
        particle( const sf::Vector2f& init_pos , const sf::Color& color , const sf::Vector2f& init_speed , float mass , int lifetime = -1 ) :
            position( init_pos ) ,
            speed( init_speed )  ,
            color( color )       ,
            mass( mass )         ,
            life_ahead( lifetime )
        {}
        
        void restart( const sf::Vector2f& init_pos , const sf::Color& color , const sf::Vector2f& init_speed , float mass , int lifetime = -1 )
        { 
            position   = init_pos;
            speed      = init_speed;
            this->color      = color;
            mass       = mass;
            life_ahead = lifetime;
        }
        
        void kill()
        {
            life_ahead = 0;
        }
        
        cpp::particle_state state() const
        {
            if( life_ahead  < 0 ) return cpp::particle_state::no_lifetime;
            if( life_ahead == 0 ) return cpp::particle_state::dead;
            if( life_ahead  > 0 ) return cpp::particle_state::alive;
        }
        
        const sf::Vector2f& current_position() const
        {
            return position;
        }
        
        const sf::Vector2f& current_speed() const
        {
            return speed;
        }
        
        void step( const sf::Vector2f& force )
        {
            if( state() == cpp::particle_state::dead ) return;
            
            auto acceleration = force / mass;
            
            speed    += acceleration;
            position += speed;
            
            if( life_ahead > 0 )
                life_ahead--;
        }
    };
}

#endif	/* PARTICLE_HPP */

