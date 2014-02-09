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

namespace cpp
{
    namespace bind_impl
    {
        template<typename N>
        struct custom_placeholder{};
    }
}

namespace std
{
    template<int N>
    struct is_placeholder<cpp::bind_impl::custom_placeholder<tml::integer<N>>> : std::integral_constant<int,N> {};
} 

namespace cpp
{
    template<typename... INDICES>
    using placeholders_list = tml::list<INDICES...>;
    
    using null_placeholder = tml::empty_list;
    
    template<int BEGIN , int END>
    using generate_placeholders = tml::for_each<tml::make_integer_forward_iterator<BEGIN> , tml::make_integer_forward_iterator<END+1> , tml::function>;
    
    template<int... PLACEHOLDERS>
    using placeholders = cpp::placeholders_list<tml::integer<PLACEHOLDERS>...>;

    
    template<typename F, typename... ARGS , typename... INDICES>
    auto bind( F&& function , cpp::placeholders_list<INDICES...> placeholders , ARGS&&... args ) -> decltype( std::bind( std::forward<F>( function ) , std::forward<ARGS>( args )... , cpp::bind_impl::custom_placeholder<INDICES>{}... ) )
    {   
        return std::bind( std::forward<F>( function ) , std::forward<ARGS>( args )... , cpp::bind_impl::custom_placeholder<INDICES>{}... );
    }
}

#endif	/* BIND_HPP */

