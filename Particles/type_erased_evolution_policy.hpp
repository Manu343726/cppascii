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

#ifndef TYPE_ERASED_EVOLUTION_POLICY_HPP
#define	TYPE_ERASED_EVOLUTION_POLICY_HPP

#include "../snippets/Turbo/core.hpp"

#include "particle_evolution_policies.hpp"

namespace cpp
{
    /* Type-erased particle evolution policy */
    
    template<typename PARTICLE_DATA>
    class particle_evolution_policy
    {
    public:
        using particle_data_policy = PARTICLE_DATA;
        
        template<typename POLICY>
        particle_evolution_policy( const POLICY& policy ) :
            _policy{ new policy_impl<POLICY>{ policy } }
        {
            TURBO_ASSERT( (cpp::is_policy<POLICY,PARTICLE_DATA>) , "The parameter is not a valid evolution policy class" );
        }

        
        void operator()( PARTICLE_DATA& data )
        {
            (*_policy)( data );
        }
        
        void step( cpp::evolution_policy_step step )
        {
            _policy->step( step );
        }
    
    private:
        
        //Interface for policy implementers:
        struct policy_interface
        {
            virtual ~policy_interface(){}
            
            virtual void operator()( PARTICLE_DATA& data ) = 0;
            
            virtual void step( cpp::evolution_policy_step step ) = 0;  
        };
        
        //Specialized policy implementation
        template<typename POLICY>
        struct policy_impl : public policy_interface
        {
            policy_impl( const POLICY& policy ) : 
                _policy{ policy }
            {}
                
            void operator()( PARTICLE_DATA& data ) override
            {
                cpp::policy_call( _policy , data );
            }

            void step( cpp::evolution_policy_step step_type ) override
            {
                cpp::policy_step<PARTICLE_DATA>( _policy , step_type );
            }
            
        private:
            POLICY _policy;
        };
        
        std::shared_ptr<policy_interface> _policy;
    };
    
    
    
    
    template<typename PARTICLE_DATA_POLICY>
    struct evolution_policies_pipeline
    {
    public:
        using stage_type = cpp::particle_evolution_policy<PARTICLE_DATA_POLICY>;
        using iterator = typename std::vector<cpp::particle_evolution_policy<PARTICLE_DATA_POLICY>>::iterator;
        
        evolution_policies_pipeline() = default;
        
        void operator()( PARTICLE_DATA_POLICY& data )
        {
            /* Nótese que data es una referencia a los datos de la partícula 
             *
             * pipelined_evolution_policy aprovecha esa característica para simular 
             * un pipeline de políticas de evolución, es decir, una política de 
             * evolución por etapas:
             * 
             *     STAGE 1
             *   -----------
             *     STAGE 2
             *   -----------
             *       ...    
             *   -----------
             *     STAGE N
             * 
             *  Donde cada etapa es una política de evolución propiamente dicha.
             * 
             * La idea es que la etapa n+1 coge como datos de entrada el resultado
             * de la etapa n, y así sucesivamente. Al ser los datos de la partícula
             * pasados como una referencia, ése comportamiento es muy fácil de simular 
             * (Un simple bucle a través del pipeline)
             */
            for( auto& policy : _pipeline )
                policy( data );
        }
        
        void step( cpp::evolution_policy_step step_type )
        {
            for( auto& policy : _pipeline )
                policy.step( step_type );
        }
        
        iterator begin() const
        {
            return std::begin( _pipeline );
        }
        
        iterator end() const
        {
            return std::end( _pipeline );
        }
        
        iterator rbegin() const
        {
            return _pipeline.rbegin();
        }
        
        iterator rend() const
        {
            return _pipeline.rend();
        }
        
        const stage_type& operator[]( std::size_t stage ) const
        {
            return _pipeline[stage];
        }
        
        stage_type& operator[]( std::size_t stage )
        {
            return _pipeline[stage];
        }
        
        template<typename POLICY>
        void add_stage( POLICY&& policy )
        {
            _pipeline.push_back( std::forward<POLICY>( policy ) );
        }
        
        template<typename POLICY>
        void insert_stage( std::size_t stage , POLICY&& policy )
        {
            _pipeline.insert( _pipeline.begin() + stage , std::forward<POLICY>( policy ) );
        }
        
        void remove_stage( std::size_t stage )
        {
            _pipeline.erase( _pipeline.begin() + stage );
        }
        
    private:
        std::vector<cpp::particle_evolution_policy<PARTICLE_DATA_POLICY>> _pipeline;
    };
}

#endif	/* TYPE_ERASED_EVOLUTION_POLICY_HPP */

