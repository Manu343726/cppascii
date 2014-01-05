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

#include "numeric_comparisons.hpp"
#include "value_wrapper.hpp"

#include <iostream>

int main()
{
    float pi( 3.141592654f ) , pi2( pi );
    
    for( std::size_t i = 0 ; i < 100 ; ++i )
        pi2 += 0.001;
    
    for( std::size_t i = 0 ; i < 100 ; ++i )
        pi2 -= 0.001;
    
    std::cout << std::boolalpha; //Que pinte true y false por dios
    
    std::cout << ( 2*pi2 - pi2 == pi ) << std::endl; //Ya os digo yo que ni de coña este es true
    std::cout << cpp::numeric_comparisons::equal( 2*pi - pi , pi ) << std::endl; //Pero este si
    std::cout << ( cpp::wrap( 2*pi2 - pi ) == cpp::wrap( pi ) ) << std::endl; //Mucho más intuitivo de usar, verdad?
    std::cout << ( cpp::wrap( 2*pi2 - pi ) != cpp::wrap( pi ) ) << std::endl; //Mucho más intuitivo de usar, verdad?
    std::cout << ( cpp::wrap( 2*pi2 - pi ) >= cpp::wrap( pi ) ) << std::endl; //Mucho más intuitivo de usar, verdad?
    std::cout << ( cpp::wrap( 2*pi2 - pi ) <= cpp::wrap( pi ) ) << std::endl; //Mucho más intuitivo de usar, verdad?
}

