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

#ifndef POLYMORPHISM_HPP
#define	POLYMORPHISM_HPP

#include <vector>
#include <memory>

namespace cpp
{
    template<typename INTERFACE , template<typename> class GENERIC_IMPL>
    class free_polymorphism
    {
        
        
        
    private:
        std::vector<std::shared_ptr<INTERFACE>> _implementers;
    };
    
    
}

#endif	/* POLYMORPHISM_HPP */

