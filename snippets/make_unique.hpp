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

#ifndef MAKE_UNIQUE_HPP
#define	MAKE_UNIQUE_HPP

#include <memory>
#include <type_traits>
#include <utility>


/* Ride over the missing std::make_unique() utility in C++11.
 * 
 * Based on Stephan T. Lavavej adopted C++14 proposal.
 * http://isocpp.org/files/papers/N3656.txt
 */

namespace cpp
{
    template <typename T, typename... ARGS>
    static std::unique_ptr<T> make_unique_helper( std::false_type , ARGS&&... args ) 
    {
      return std::unique_ptr<T>( new T( std::forward<Args>( args )... ) );
    }

    template <typename T, typename... ARGS>
    static std::unique_ptr<T> make_unique_helper( std::true_type , ARGS&&... args ) 
    {
       static_assert(std::extent<T>::value == 0,
           "make_unique<T[N]>() is forbidden, please use make_unique<T[]>().");

       using U = typename std::remove_extent<T>::type;
       
       return std::unique_ptr<T>( new U[sizeof...(ARGS)]{ std::forward<ARGS>(args)... } );
    }

    template <typename T, typename... ARGS>
    std::unique_ptr<T> make_unique( ARGS&&... args )
    {
       return make_unique_helper<T>( std::is_array<T>() , std::forward<ARGS>( args )... );
    }
}

#endif	/* MAKE_UNIQUE_HPP */

