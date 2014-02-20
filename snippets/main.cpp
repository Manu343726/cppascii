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
#include "bind.hpp"
#include "event.hpp"

#include <iostream>
#include <algorithm>
#include <iterator>


struct foo
{
    cpp::event<foo,int,int,int> hey;
    
    void do_something()
    {
        int a = 0 , b = 1 , c = 2;
        
        std::cout << "Hey, is there anybody out there?" << std::endl;
        
        hey.raise_event( *this , a , b , c );
    }
};

struct bar
{
    void handler(foo& sender , int& a , int& b, int& c )
    {
        std::cout << "Yes! I'm a bar! How are you foo?" << std::endl;
    }
};

void handler(foo& sender , int& a , int& b, int& c )
{
    std::cout << "I'm just an old global function... like in C, do you remember? No classes, no lambdas. Just plain functions." << std::endl;
}

int main()
{
    using namespace cpp::polinomial_symbols;
    using namespace cpp::polinomial_grammar;
    
    cpp::polinomial p = (3*X^3) + (2*X^2) - (X) + 1;
    
    std::cout << p(1) << std::endl;

    using namespace std::placeholders;
    
    std::function<void(int,int,int,int,int,int,int,int)> f = [](int a,int b,int c,int d,int e,int f,int g,int h){ std::cout << a << b << c << d << e << f << g << h << std::endl; };
    
    auto f_call1 = cpp::bind( f , 1 , cpp::generate_placeholders<1,6>{} , 8 );
    auto f_call2 = cpp::bind( f , 1 , cpp::generate_placeholders<1,3>{} , 5 , cpp::generate_placeholders<4,5>{} , 8 );
    f_call1( 2 , 3 , 4 , 5 , 6 , 7 );
    f_call2( 2 , 3 , 4     , 6 , 7 );
    
    foo afoo;
    bar abar;
    
    afoo.hey.add_handler( abar , &bar::handler );
    afoo.hey.add_handler( [](foo&,int&,int&,int&){ std::cout << "Whats up foo? I'm a lambda!" << std::endl; } );
    afoo.hey.add_handler( handler );
    
    afoo.do_something();
    
   
    cpp::string str = "hola!";
    
    std::copy( std::begin( str ) , std::end( str ) , std::ostream_iterator<char>( std::cout ) );
    std::cout << std::endl;
    
    cpp::string str2 = std::move( str );
    
    std::swap( str , str2 );
}


