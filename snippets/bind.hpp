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

#ifndef BIND_HPP
#define	BIND_HPP

#include <functional>
#include <type_traits>

#include "Turbo/core.hpp"
#include "Turbo/for_loops.hpp"
#include "Turbo/numeric_iterators.hpp"
#include "Turbo/list.hpp"
#include "Turbo/utility.hpp"

namespace cpp
{
    template<typename... INDICES>
    using placeholders_list = tml::list<INDICES...>;
    
    using null_placeholder = tml::empty_list;
    
    template<int BEGIN , int END>
    using generate_placeholders = tml::for_each<tml::make_integer_forward_iterator<BEGIN> , tml::make_integer_forward_iterator<END+1> , tml::function>;
    
    template<int... PLACEHOLDERS>
    using placeholders = cpp::placeholders_list<tml::integer<PLACEHOLDERS>...>;
    
    
    
    namespace bind_impl
    {
        template<typename N>
        struct custom_placeholder{};
        
        template<typename N>
        using integer_secuence = tml::for_each<tml::make_integer_forward_iterator<0> , tml::make_integer_forward_iterator<N::value> , tml::function>;
        
        //bypass base case:
        std::tuple<> bypass()
        {
            return std::make_tuple();
        }
        
        //Non-placeholder bypass:
        template<typename HEAD , typename... TAIL>
        auto bypass( HEAD&& head , TAIL&&... tail )
        {
            return std::tuple_cat( std::make_tuple( std::forward<HEAD>( head ) ) , 
                                   cpp::bind_impl::bypass( std::forward<TAIL>( tail )... )
                                 );
        }
        
        //Placeholder bypass:
        template<typename... INDICES , typename... TAIL>
        auto bypass( cpp::placeholders_list<INDICES...>&& , TAIL&&... tail )
        {
            return std::tuple_cat( std::make_tuple( cpp::bind_impl::custom_placeholder<INDICES>{}... ) ,
                                   cpp::bind_impl::bypass( std::forward<TAIL>( tail )... )
                                 );
        }
        
        template<typename F , typename... ARGS , typename... INDICES>
        auto bind_impl( F&& function , const std::tuple<ARGS...>& args , tml::list<INDICES...>&& )
        {
            static_assert( sizeof...(INDICES) == sizeof...(ARGS) , "Something gone very wrong..." );
            
            return std::bind( std::forward<F>( function ) , std::get<INDICES::value>( args )... );
        }
    }
}

namespace std
{
    template<int N>
    struct is_placeholder<cpp::bind_impl::custom_placeholder<tml::integer<N>>> : std::integral_constant<int,N> {};
} 

namespace cpp
{    
    template<typename F, typename... ARGS>
    auto bind( F&& function , ARGS&&... args )
    {   
        return cpp::bind_impl::bind_impl( std::forward<F>( function ) , 
                                          cpp::bind_impl::bypass( std::forward<ARGS>( args )... ), 
                                          cpp::bind_impl::integer_secuence<std::tuple_size<decltype( cpp::bind_impl::bypass( std::forward<ARGS>( args )... ) )>>{} 
                                        );
    }
}

#endif	/* BIND_HPP */

