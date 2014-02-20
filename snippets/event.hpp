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

#ifndef EVENT_HPP
#define	EVENT_HPP

#include "bind.hpp"

#include <functional>
#include <algorithm>
#include <vector>
#include "Turbo/core.hpp"

namespace cpp
{
    struct null_sender_t {} null_sender;
    using no_sender = null_sender_t;
    
    template<typename SENDER , typename... ARGS>
    class event
    {
    public:
        using handler_function_t = std::function<void(SENDER&,ARGS&...)>;
    
        
        void add_handler( const handler_function_t& handler )
        {
            _handlers.push_back( handler );
        }
        
        template<typename CLASS>
        void add_handler( CLASS& object_ref , void(CLASS::*member_function)( SENDER& , ARGS&... ) )
        {
            using namespace std::placeholders;
            
            constexpr std::size_t handler_args_count = 1 + sizeof...(ARGS);
            
            handler_function_t handler = cpp::bind( member_function , std::ref( object_ref ) , cpp::generate_placeholders<1,handler_args_count>{} );
            
            _handlers.push_back( handler );
        }
    
        void remove_handler( const handler_function_t& handler )
        {
            auto it = std::find( std::begin( _handlers ) , std::end( _handlers) , handler );
            
            if( it != std::end( _handlers ) )
                _handlers.erase( it );
        }
        
        template<typename DUMMY_SFINAE = SENDER ,
                 typename = typename std::enable_if<std::is_same<DUMMY_SFINAE,cpp::no_sender>::value>::type>
        void raise_event( ARGS&... args )
        {
            for( auto& handler : _handlers )
                handler( cpp::null_sender , args... );
        }
        
        template<typename DUMMY_SFINAE = SENDER ,
                 typename = typename std::enable_if<!std::is_same<DUMMY_SFINAE,cpp::no_sender>::value>::type>
        void raise_event( SENDER& sender , ARGS&... args )
        {
            for( auto& handler : _handlers )
                handler( sender , args... );
        }  
        
    private:
        std::vector<handler_function_t> _handlers;     
    };
    
    
    TURBO_DEFINE_FUNCTION( event_maker_helper , (typename FUNCTION) , (FUNCTION) );
    
    template<typename SENDER , typename... ARGS>
    struct event_maker_helper_t<void(SENDER&,ARGS&...)> : tml::function<cpp::event<SENDER,ARGS...>> {};
    
    
    template<typename FUNCTION>
    cpp::event_maker_helper<FUNCTION> make_event( const FUNCTION& function )
    {
        return cpp::event_maker_helper<FUNCTION>{};
    }
}

#endif	/* EVENT_HPP */

