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

#include "bind.hpp"

#include "Turbo/to_string.hpp"

#include <functional>
#include <iostream>

void f( int a , int b , int c , int d , int e , int f , int g , int h )
{
    std::cout << a << " " << b << " " << c << " " << d << " " << e << " " << f << " " << g << " " << h  << std::endl;
}

void g(int,bool,char){}

struct foo
{
    void f(int,bool,char) {}
};

int main()
{
    using namespace std::placeholders;
    
    foo my_foo;
    
    auto f_call = cpp::bind( f , cpp::placeholders<1,2,3,4,5,6,7,8>{} );
    auto std_f_call = std::bind( f , _1 , 9 , _3 , _4 , _5 , _6 , _7 , _8 );
    
    std::cout << tml::to_string<decltype(f_call)>() << std::endl;
    
    //f_call( 2 , 3 , 4 , 5 , 6 , 7 , 8 );
    //std_f_call( 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 );
}

