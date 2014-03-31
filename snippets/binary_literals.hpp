/* 
 * File:   binary_literals.hpp
 * Author: manu343726
 *
 * Created on 22 de marzo de 2014, 20:31
 */

#ifndef BINARY_LITERALS_HPP
#define	BINARY_LITERALS_HPP

/* User defined binary literals for C++11. 
 * I'm not going to wait for C++14 binary literals! 
 */

#include "Turbo/core.hpp"
#include "Turbo/for_loops.hpp"
#include "Turbo/utility.hpp"

#include <climits>

namespace cpp
{
    TURBO_DEFINE_FUNCTION( make_binary , ( typename T , typename CHARS ) , ( T , CHARS ) );
    TURBO_DEFINE_FUNCTION( find_integral_type , (typename LENGTH) , (LENGTH) );

    
    template<std::size_t LENGTH>
    struct find_integral_type_t<tml::size_t<LENGTH>>
    {
        using integral_types = tml::list<unsigned char,unsigned short int,unsigned int,unsigned long int,unsigned long long int>;
        
        template<typename T>
        using sizeof_function = tml::function<tml::size_of<T>>;
        
        using sizes = tml::for_each_in_list<integral_types,
                                            sizeof_function
                                           >;
        
        template<typename T>
        using filter = tml::bigger_or_equal<tml::size_t<sizeof(T)*CHAR_BIT>,
                                            tml::size_t<LENGTH>
                                           >;
        
        using filtered = tml::for_each<tml::begin<integral_types>,
                                       tml::end<integral_types>,
                                       tml::function,
                                       filter
                                      >;
        
        static_assert( !filtered::is_empty , "No integral type matches the number of bits you have written" );
        
        using result = tml::first<filtered>;
    };
    
    
    template<typename T>
    struct make_binary_t<T,tml::empty_list>
    {
        using result = tml::value_t<T,0>;
    };
        
    template<typename T , char HEAD , char... TAIL>
    struct make_binary_t<T,tml::list<tml::character<HEAD>,tml::character<TAIL>...>>
    {
        static_assert( HEAD == '0' || HEAD == '1' , "That character is not a bit" );

        using current = tml::value_t<T,( ( HEAD - '0' ) << sizeof...(TAIL) )>;
        
        using result = tml::value_t<T, current::value | cpp::make_binary<T,tml::list<tml::character<TAIL>...>>::value>;
    };
}

template<char... BITS>
constexpr auto operator"" _b()
{
    return cpp::make_binary<cpp::find_integral_type<tml::size_t<sizeof...(BITS)>> ,
                            tml::list<tml::character<BITS>...>
                           >::value;
}

#endif	/* BINARY_LITERALS_HPP */

