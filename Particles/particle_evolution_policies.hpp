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

#include "../snippets/Turbo/core.hpp"

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
    
    
    
    //Alias for shared policies:
    template<typename POLICY>
    using shared_policy = std::shared_ptr<POLICY>;
    
    //Traits to detect when a type has a step() member function, to type-erase it acordingly:
    namespace impl
    {
        //Just a template to do SFINAE
        template<typename T> using dummy_sfinae_thing = void;
        
        //A trait which checks if a type has a step() member function:
        //NOTE: The second parameter is just used to do SFINAE, thats why its optional
        TURBO_DEFINE_FUNCTION( has_step , (typename T , typename U = void) , (T,U) , (tml::false_type) );
        
        template<typename T>
        struct has_step_t<T,dummy_sfinae_thing<decltype( std::declval<T>().step( std::declval<cpp::evolution_policy_step>() ) )>> : public tml::function<tml::true_type> {};
        
        //A trait which checks if a type has a step() member function:
        //NOTE: The second parameter is just used to do SFINAE, thats why its optional
        TURBO_DEFINE_FUNCTION( has_call , (typename T , typename PDATA , typename U = void) , (T,PDATA,U) , (tml::false_type) );
        
        template<typename T , typename PDATA>
        struct has_call_t<T,PDATA,dummy_sfinae_thing<decltype( std::declval<T>()( std::declval<PDATA>() ) )>> : public tml::function<tml::true_type> {};
    }   
    
    template<typename POLICY , typename PARTICLE_DATA>
    using is_nonshared_policy = impl::has_call<POLICY,PARTICLE_DATA>;
    
    template<typename POLICY , typename PARTICLE_DATA>
    using is_nonshared_stated_policy = decltype( is_nonshared_policy<POLICY,PARTICLE_DATA>{} && impl::has_step<POLICY>{} );
    
    template<typename POLICY , typename PARTICLE_DATA>
    using is_nonshared_nonstated_policy = decltype( is_nonshared_policy<POLICY,PARTICLE_DATA>{} && !impl::has_step<POLICY>{} );

    TURBO_DEFINE_FUNCTION( is_shared_policy , (typename T , typename PDATA) , (T,PDATA) , (tml::false_type) );
    TURBO_DEFINE_FUNCTION( is_shared_stated_policy , (typename T , typename PDATA) , (T,PDATA) , (tml::false_type) );
    TURBO_DEFINE_FUNCTION( is_shared_nonstated_policy , (typename T , typename PDATA) , (T,PDATA) , (tml::false_type) );
    
    template<typename T , typename PDATA>
    struct is_shared_policy_t<cpp::shared_policy<T>,PDATA> : public tml::function<is_nonshared_policy<T,PDATA>> {};
    template<typename T , typename PDATA>
    struct is_shared_stated_policy_t<cpp::shared_policy<T>,PDATA> : public tml::function<is_nonshared_stated_policy<T,PDATA>> {};
    template<typename T , typename PDATA>
    struct is_shared_nonstated_policy_t<cpp::shared_policy<T>,PDATA> : public tml::function<is_nonshared_nonstated_policy<T,PDATA>> {};
    
    template<typename T , typename PARTICLE_DATA>
    using is_policy = decltype( is_nonshared_policy<T,PARTICLE_DATA>{} || is_shared_policy<T,PARTICLE_DATA>{} );
    template<typename T , typename PARTICLE_DATA>
    using is_stated_policy = decltype( is_nonshared_stated_policy<T,PARTICLE_DATA>{} || is_shared_stated_policy<T,PARTICLE_DATA>{} );
    template<typename T , typename PARTICLE_DATA>
    using is_nonstated_policy = decltype( is_nonshared_policy<T,PARTICLE_DATA>{} || is_shared_nonstated_policy<T,PARTICLE_DATA>{} );
    
    namespace evolution_policy_categories
    {
        struct shared {};
        struct nonshared {};
        struct stated {};
        struct non_stated {};
    }
    
    enum class evolution_policy_category
    {
        shared,
        nonshared,
        stated,
        nonstated,
        shared_stated,
        nonshared_stated,
        nonshared_nonstated
    };
    
    /* Type-erased particle evolution policy */
    
    template<typename PARTICLE_DATA>
    class particle_evolution_policy
    {
    public:
        using particle_data_policy = PARTICLE_DATA;
        
        template<typename POLICY>
        particle_evolution_policy( const POLICY& policy ) :
            _policy{ new policy_impl<POLICY,cpp::is_shared_policy<POLICY,PARTICLE_DATA>,cpp::is_stated_policy<POLICY,PARTICLE_DATA>>{ policy } }
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
        template<typename POLICY , typename IS_SHARED , typename IS_STATED>
        struct policy_impl;
        
        
        template<typename POLICY>
        struct policy_impl<POLICY,tml::false_type,tml::false_type> : public policy_interface
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
            {}
            
        private:
            POLICY _policy;
        };
        
        template<typename POLICY>
        struct policy_impl<POLICY,tml::false_type,tml::true_type> : public policy_interface
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
        
        
        template<typename POLICY>
        class policy_impl<POLICY,tml::true_type,tml::false_type> : public policy_interface
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
            {}

        private:
            //Note that the shared policy is type-erased too
            POLICY _policy;
        };
        
        template<typename POLICY>
        struct policy_impl<POLICY,tml::true_type,tml::true_type> : public policy_interface
        {
        public:
            policy_impl( const POLICY& policy ) : 
                _policy{ policy }
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
            POLICY _policy;
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

