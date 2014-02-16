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

#include "polinomial.hpp"
#include "string.hpp"

#include <iostream>
#include <algorithm>
#include <iterator>

int main()
{
    using namespace cpp::polinomial_symbols;
    using namespace cpp::polinomial_grammar;
    
    cpp::polinomial p = (3*X^3) + (2*X^2) - (X) + 1;
    
    std::cout << p(1) << std::endl;

    
    
   
    cpp::string str = "hola!";
    
    std::copy( std::begin( str ) , std::end( str ) , std::ostream_iterator<char>( std::cout ) );
    std::cout << std::endl;
    
    cpp::string str2 = std::move( str );
    
    std::swap( str , str2 );
}


