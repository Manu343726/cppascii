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
#include <utility>
#include <vector>

namespace cpp
{
    enum class evolution_policy_step
    {
        global , individual
    };
    
    template<typename PARTICLE_DATA_POLICY>
    struct evolution_policy_traits
    {
        using particle_data_policy = PARTICLE_DATA_POLICY;
        
        using policy_signature = std::function<void(PARTICLE_DATA_POLICY&)>;
        using step_signature   = std::function<void(cpp::evolution_policy_step)>;
    };
    
    
    
    
    
    
    
    
    
    /* Type-erased particle evolution policy */
    
    template<typename PARTICLE_DATA>
    class particle_evolution_policy
    {
    public:
        using particle_data_policy = PARTICLE_DATA;
        
        template<typename POLICY>
        particle_evolution_policy( const POLICY& policy ) :
            _policy{ new policy_impl<POLICY,has_step<POLICY>>{ policy } }
        {}
            
        
        void operator()( PARTICLE_DATA& data ) const
        {
            (*_policy)( data );
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
        //Traits to detect when a type has a step() member function, to type-erase it acordingly:
        
        //Just a template to do SFINAE
        template<typename T> using dummy_sfinae_thing = void;
        
        //A trait which checks if a type has a step() member function:
        //NOTE: The second parameter is just used to do SFINAE, thats why its optional
        TURBO_DEFINE_FUNCTION( has_step , (typename T , typename U = void) , (T,U) , (std::false_type) );
        
        template<typename T>
        struct has_step_t<T,dummy_sfinae_thing<decltype( std::declval<T>().step() )>> : public tml::function<std::false_type> {};
        
        
        /*--------------------------------------------*/
        
        //Interface for policy implementers:
        struct policy_interface
        {
            virtual ~policy_interface(){}
            
            virtual void operator()( PARTICLE_DATA& data ) = 0;
            
            virtual void step( cpp::evolution_policy_step step ) = 0;  
        };
        
        //Specialized policy implementation
        template<typename POLICY , typename HAS_STEP = std::false_type>
        struct policy_impl;
        
        //Specialization for any function-like type with a step() member function:
        template<typename POLICY>
        struct policy_impl<POLICY,std::true_type> : public policy_interface
        {
        public:
            policy_impl( const POLICY& policy ) : 
                _policy{ policy }
            {}

            void operator()( PARTICLE_DATA& data ) override
            {
                _policy( data );
            }

            void step( cpp::evolution_policy_step step_type ) override
            {
                _policy.step( step_type );
            }
            
        private:
            POLICY _policy;
        };
        
        //Specialization for shared_ptrs of policies (Policies shared with shared_ptrs)
        template<typename T>
        class policy_impl<std::shared_ptr<T>,std::false_type> : public policy_interface
        {
        public:
            template<typename... ARGS>
            policy_impl( ARGS&&... args ) : 
                _policy{ std::forward<ARGS>( args )... }
            {}

            void operator()( PARTICLE_DATA& data ) override
            {
                (*_policy)( data );
            }

            void step( cpp::evolution_policy_step step_type ) override
            {
                _policy->step( step_type );
            }

        private:
            //Note that the shared policy is type-erased too
            std::shared_ptr<T> _policy;
        };
        
        
        
        //Specialization for other function-like types which don't have a step() member function:
        template<typename T>
        struct policy_impl<T,std::false_type> : public policy_interface
        {
            template<typename... ARGS>
            policy_impl( ARGS&&... args ) :
                _policy{ std::forward<ARGS>( args )... }
            {}
                
                void operator()( PARTICLE_DATA& data ) override
                {
                    _policy( data );
                }
                
                void step( cpp::evolution_policy_step step_type ) override
                {
                    /* does nothing */
                }
                
        private:
            T _policy;
        };

        
        
        std::shared_ptr<policy_interface> _policy;
    };
    
    template<typename PARTICLE_DATA_POLICY>
    class pipelined_evolution_policy
    {
    public:
        using stage_type = cpp::particle_evolution_policy<PARTICLE_DATA_POLICY>;
        
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
        
        auto begin() const
        {
            return std::begin( _pipeline );
        }
        
        auto end() const
        {
            return std::end( _pipeline );
        }
        
        auto rbegin() const
        {
            return _pipeline.rbegin();
        }
        
        auto rend() const
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

#endif	/* PARTICLE_EVOLUTION_POLICIES_HPP */

