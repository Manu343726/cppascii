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

#ifndef SCOPED_RESOURCE_HPP
#define	SCOPED_RESOURCE_HPP


/* RAII-based resource managing for non-RAII resources (Old C resources, OpenGL resources, etc) 
 *
 * Copyright © 2014 Manuel Sánchez Pérez
 * 
 * 
 * This template helps to automate the lifetime/releasing of resources not managed by the C++ RAII idiom (Scope based resource management), like low-level handlers, OpenGL textures, etc.
 * Simply create a cpp::scoped_resource with the action to be performed at the end of the scope, and the resource itself.
 * 
 * For example: A glut quadric.
 * A glut quadric is retrieved to the user in the form of a handler (pointer). Its created by the gluNewQuadric() function, and released by the gluDeleteQuadric() function.
 * That means a class which uses a quadric should write a call to glutDeleteCuadric() inside its destructor body. 
 * Writting a destructor in C++ is something thats rarely done, because resources are managed with the RAII (Resource Adquisition Is Initialization) idiom,
 * which in a few words consits on linking the lifetime of a resource to its scope, through constructors and destructors. 
 * So normally a C++ class is a set (aggregate) of RAII-based resources, and no user-defined destructor is needed in a user class.
 * 
 * Instead of writting a destructor, which as we saw above is useless nowadays, we proppose to wrap a non-RAII resource inside a wrapper which does RAII for it:
 * 
 * cpp::scoped_resource<GLUQuadric*> my_no_leaked_quadric( []( const GLUQuadric*& q ) { gluDeleteQuadric( q ); } , //Action to be performed at end of resource scope. 
 *                                                         gluNewQuadric()                                         //Resource initialization
 *                                                       );  
 */


#include <utility>
#include <functional>

namespace cpp
{
    template<typename T>
    struct scoped_resource
    {
    public:
        using end_scope_action_t = std::function<void(T&)>;
        
    private:
        T _resource;
        end_scope_action_t _end_scope_action;
        
    public:
        template<typename... ARGS>
        scoped_resource( end_scope_action_t action , ARGS&&... args ) : 
            _resource{ std::forward<ARGS>( args )... } ,
            _end_scope_action{ action }
        {}

        scoped_resource( end_scope_action_t init, end_scope_action_t destroy ) :
            _resource{},
            _end_scope_action{destroy}
        {
            init(_resource);
        }
        
        const T& ref() const
        {
            return _resource;
        }
        
        T& ref()
        {
            return _resource;
        }
        
        operator const T&() const
        {
            return ref();
        }
        
        operator T&() 
        {
            return ref();
        }
            
        ~scoped_resource()
        {
            _end_scope_action( this->ref() );
        }
    };
}

#endif	/* SCOPED_RESOURCE_HPP */

