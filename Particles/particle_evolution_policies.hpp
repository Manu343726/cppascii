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
        template<typename T>
        struct has_step_t
        {
            template<typename U>
            static tml::true_type test( decltype( &U::step ) );
            template<typename U>
            static tml::false_type test(...);
            
            using result = decltype( test<T>( nullptr ) );
        };
        
        template<typename T>
        using has_step = typename has_step_t<T>::result;
        
        //A trait which checks if a type has a step() member function:
        //NOTE: The second parameter is just used to do SFINAE, thats why its optional
        TURBO_DEFINE_FUNCTION( has_call , (typename T , typename PDATA , typename U = void) , (T,PDATA,U) , (tml::false_type) );
        
        template<typename T , typename PDATA>
        struct has_call_t<T,PDATA,dummy_sfinae_thing<decltype( std::declval<T>()( std::declval<PDATA>() ) )>> : public tml::function<tml::true_type> {};
    }   
    
    template<typename POLICY>
    using has_state = impl::has_step<POLICY>;
    
    template<typename POLICY , typename PARTICLE_DATA>
    using is_nonshared_policy = impl::has_call<POLICY,PARTICLE_DATA>;
    
    template<typename POLICY , typename PARTICLE_DATA>
    using is_nonshared_stated_policy = tml::logical_and<is_nonshared_policy<POLICY,PARTICLE_DATA>,impl::has_step<POLICY>>;
    
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
    using is_policy = tml::logical_or<is_nonshared_policy<T,PARTICLE_DATA>,is_shared_policy<T,PARTICLE_DATA>>;
    template<typename T , typename PARTICLE_DATA>
    using is_stated_policy = tml::logical_or<is_nonshared_stated_policy<T,PARTICLE_DATA>,is_shared_stated_policy<T,PARTICLE_DATA>>;
    template<typename T , typename PARTICLE_DATA>
    using is_nonstated_policy = tml::logical_or<is_nonshared_policy<T,PARTICLE_DATA>,is_shared_nonstated_policy<T,PARTICLE_DATA>>;
    
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
    
    
    namespace impl
    {
        template<typename POLICY , typename IS_SHARED , typename IS_STATED>
        struct stepper;
        
        template<typename POLICY>
        struct stepper<POLICY , tml::true_type , tml::true_type>
        {
            static void execute( POLICY& policy , cpp::evolution_policy_step step_type )
            {
                policy->step( step_type );
            }
        };
        
        template<typename POLICY>
        struct stepper<POLICY , tml::false_type , tml::true_type>
        {
            static void execute( POLICY& policy , cpp::evolution_policy_step step_type )
            {
                TURBO_ASSERT( (cpp::has_state<POLICY>) , "mmmmm..." );
                
                policy.step( step_type );
            }
        };
        
        template<typename POLICY , typename IS_SHARED>
        struct stepper<POLICY , IS_SHARED , tml::false_type>
        {
            static void execute( POLICY& policy , cpp::evolution_policy_step step_type )
            {}
        };
        
        
        template<typename POLICY , typename IS_SHARED>
        struct caller;
        
        template<typename POLICY>
        struct caller<POLICY,tml::true_type>
        {
            template<typename PARTICLE_DATA>
            static void execute( POLICY& policy , PARTICLE_DATA& data )
            {
                (*policy)( data );
            }
        };
        
        template<typename POLICY>
        struct caller<POLICY,tml::false_type>
        {
            template<typename PARTICLE_DATA>
            static void execute( POLICY& policy , PARTICLE_DATA& data )
            {
                policy( data );
            }
        };
    }
    
    template<typename POLICY , typename PARTICLE_DATA>
    void policy_call( POLICY& policy , PARTICLE_DATA& data )
    {
        impl::caller<POLICY,cpp::is_shared_policy<POLICY,PARTICLE_DATA>>::execute( policy , data );
    }
    
    template<typename PARTICLE_DATA , typename POLICY>
    void policy_step( POLICY& policy , cpp::evolution_policy_step step_type )
    {
        impl::stepper<POLICY,cpp::is_shared_policy<POLICY,PARTICLE_DATA> , cpp::is_stated_policy<POLICY,PARTICLE_DATA>>::execute( policy , step_type );
    }
}

#endif	/* PARTICLE_EVOLUTION_POLICIES_HPP */

