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

#ifndef VALUE_WRAPPER_HPP
#define	VALUE_WRAPPER_HPP

/* Por ahora no miréis éstas cosas. No está pensado para que lo entendáis, por eso no está comentado en castellano
 * (Tengo la costumbre de comentar en inglés, pero para vosotros lo hago en castellano)
 */

#include "value_wrapper.hpp"

#include <utility>
#include <type_traits>

namespace cpp
{
    template<typename T , bool is_lvalue>
    struct value_wrapper;


    //Specialization for rvalues:
    template<typename T>
    struct value_wrapper<T,false>
    {
    private:
        T _value;

    public:
        explicit value_wrapper( T&& v ) : _value( std::move( v ) )
        {}

        //read
        const T& ref() const
        {
            return _value;
        }

        //write
        T& ref() 
        {
            return _value;
        }
    };

    //Specialization for lvalues
    template<typename T>
    struct value_wrapper<T,true>
    {
    private:
        const T& _value;

    public:
        explicit value_wrapper( const T& v ) : _value( v )
        {}

        const T& ref() const
        {
            return _value;
        }
    };
    
    template<typename T>
    using value_wrapper_t = cpp::value_wrapper<typename std::decay<T>::type , 
                                               std::is_lvalue_reference<T>::value
                                              >;
    
    
    template<typename T>
    cpp::value_wrapper_t<T> wrap( T&& value )
    {
        return cpp::value_wrapper_t<T>( std::forward<T>( value ) );
    }
}

#endif	/* VALUE_WRAPPER_HPP */

