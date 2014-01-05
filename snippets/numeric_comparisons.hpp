/****************************************************************************
* Snippets, ejemplos, y utilidades del curso de C++ orientado a videojuegos *
* Copyright © 2014 Manuel Sánchez Pérez                                     *
*                                                                           *
* This program is free software. It comes without any warranty, to          *
* the extent permitted by applicable law. You can redistribute it           *
* and/or modify it under the terms of the Do What The Fuck You Want         *
* To Public License, Version 2, as published by Sam Hocevar. See            *
* http://www.wtfpl.net/  and the COPYING file for more details.             *
****************************************************************************/

#ifndef FLOAT_COMP_HPP
#define	FLOAT_COMP_HPP

#include <type_traits>
#include <cmath>

#include "value_wrapper.hpp"

/* Eśte módulo provee una sencilla implementación de comparadores para números, teniendo en cuenta si son números en coma
 * flotante o no. Por supuesto está abierta a debate y/o modificaciones, tampoco soy perfecto programando...
 * 
 * No os preocupéis si ahora no entendéis la implementación, con el tiempo deberíais entenderla (Ese es el objetivo del curso).
 * Por ahora fijaos en la implementación de numeric_comparer_impl::equal() para coma flotante (La primera). 
 */

namespace cpp
{
    namespace misc
    {
        //Equivalente a std::pow() (Potencia) en tiempo de compilación (Totálmente genérico por supuesto. Nótese que exp debe ser entero):
        template<typename T , typename U , typename sfinae_it_please = typename std::enable_if<std::is_integral<U>::value>::type>
        constexpr auto pow( T base , U exp ) -> decltype( base * base )
        {
            return exp == 0 ? 1 : ( exp < 0 ? ( 1.0 / cpp::misc::pow( base , -exp ) ) : base * cpp::misc::pow( base , exp - 1 ) );
        }
    }
    
    /* La idea es proveer exactamente la misma interfaz al usuario, pero la implementación es diferente para tipos en coma flotante
     * y tipos que no son coma flotante. Ésto suena a polimorfismo, verdad? Por qué no uso polimorfismo? 
     *  - Respuesta corta: Ya lo veremos en el tema 2.
     *  - Respuesta larga: Para que perder tiempo y recursos en runtime con cosas que puedes hacer en tiempo de compilación? Esto debería ser un
     *    dogma sagrado para todos vosotros. Cosas como constexpr son increíblemente difíciles de implementar (VS2012/13 no lo tiene implementado
     *    básicamente porque Microsoft es incapaz de implementarlo. Pero no es raro, les pasa con la mitad de C++...). Algo tan dificil no se implementaría
     *    si no fuera tan importante.
     */
    namespace
    {
        template<typename T , typename U = T , std::size_t PRECISION = 5 , bool is_floating_point = std::is_floating_point<T>::value ||
                                                                                                    std::is_floating_point<U>::value
                >
        struct numeric_comparer_impl
        {
            static_assert( PRECISION >= 3 , "No vas a conseguir mucho con tan poca precisión" );
            
            static bool equal( const T& lhs , const U& rhs )
            {
                //La idea es: El usuario especifica la precisión del rango de error como el número de decimales.
                //Es decir, si precision vale 3, epsilon es 0.001, si vale 5, 0.00001, etc.
                constexpr decltype( T() * T() ) epsilon = cpp::misc::pow( 10.0 , (int)(-PRECISION) );
                
                return std::abs( lhs - rhs ) < epsilon;
            }
        };
        
        template<typename T , typename U , std::size_t PRECISION>
        struct numeric_comparer_impl<T,U,PRECISION,false>
        {
            static bool equal( const T& lhs , const U& rhs )
            {
                return lhs == rhs;
            }
        };
    }
    
    //Por qué uso funciones globales y no una clase con funciones estáticas (A lo Java)? En C++ se tiende a proveer la funcionalidad genérica mediante
    //funciones, para reducir (evitar) el acoplamiento. Cualquier biblioteca de la biblioteca estándar es un ejemplo. 
    //Por ejemplo: http://en.cppreference.com/w/cpp/header/algorithm
    
    namespace numeric_comparisons
    {
        template<typename T , typename U = T>
        bool equal( const T& lhs , const U& rhs )
        {
            return numeric_comparer_impl<T,U>::equal( lhs , rhs );
        }
        
        template<typename T , typename U = T>
        bool not_equal( const T& lhs , const U& rhs )
        {
            return !numeric_comparer_impl<T,U>::equal( lhs , rhs );
        }
        
        template<typename T , typename U = T>
        bool bigger_than( const T& lhs , const U& rhs )
        {
            return lhs > rhs;
        }
        
        template<typename T , typename U = T>
        bool bigger_or_equal( const T& lhs , const U& rhs )
        {
            return lhs > rhs || cpp::numeric_comparisons::equal( lhs , rhs );
        }
        
        template<typename T , typename U = T>
        bool less_than( const T& lhs , const U& rhs )
        {
            return lhs < rhs;
        }
        
        template<typename T , typename U = T>
        bool less_or_equal( const T& lhs , const U& rhs )
        {
            return lhs < rhs || cpp::numeric_comparisons::equal( lhs , rhs );
        } 
    }
    
    
    
    template<typename LHS , typename RHS , bool LHS_F , bool RHS_F>
    bool operator==( const cpp::value_wrapper<LHS,LHS_F>& lhs , const cpp::value_wrapper<RHS,RHS_F>& rhs )
    {
        return cpp::numeric_comparisons::equal( lhs.ref() , rhs.ref() );
    }
    
    template<typename LHS , typename RHS , bool LHS_F , bool RHS_F>
    bool operator!=( const cpp::value_wrapper<LHS,LHS_F>& lhs , const cpp::value_wrapper<RHS,RHS_F>& rhs )
    {
        return cpp::numeric_comparisons::not_equal( lhs.ref() , rhs.ref() );
    }
        
    template<typename LHS , typename RHS , bool LHS_F , bool RHS_F>
    bool operator>( const cpp::value_wrapper<LHS,LHS_F>& lhs , const cpp::value_wrapper<RHS,RHS_F>& rhs )
    {
        return cpp::numeric_comparisons::bigger_than( lhs.ref() , rhs.ref() );
    }
            
    template<typename LHS , typename RHS , bool LHS_F , bool RHS_F>
    bool operator<( const cpp::value_wrapper<LHS,LHS_F>& lhs , const cpp::value_wrapper<RHS,RHS_F>& rhs )
    {
        return cpp::numeric_comparisons::less_than( lhs.ref() , rhs.ref() );
    }
                
    template<typename LHS , typename RHS , bool LHS_F , bool RHS_F>
    bool operator>=( const cpp::value_wrapper<LHS,LHS_F>& lhs , const cpp::value_wrapper<RHS,RHS_F>& rhs )
    {
        return cpp::numeric_comparisons::bigger_or_equal( lhs.ref() , rhs.ref() );
    }
    
    template<typename LHS , typename RHS , bool LHS_F , bool RHS_F>
    bool operator<=( const cpp::value_wrapper<LHS,LHS_F>& lhs , const cpp::value_wrapper<RHS,RHS_F>& rhs )
    {
        return cpp::numeric_comparisons::less_or_equal( lhs.ref() , rhs.ref() );
    }
}

#endif	/* FLOAT_COMP_HPP */

