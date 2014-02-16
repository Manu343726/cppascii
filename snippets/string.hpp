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

#ifndef STRING_HPP
#define	STRING_HPP

#include <cstring>
#include <algorithm>
#include <exception>

#include "operators.hpp"
#include "instantation_profiler.hpp"


#define USE_COPY_AND_SWAP

//Una vez en la vida y nunca más: Muy mal ejemplo de C++, ejemplo tìpico de C++...

namespace cpp
{
    template<typename T>
    class basic_string : public cpp::instantation_profiler<cpp::basic_string<T>>
    {
    public:
        using value_type = T;
        
    private:
        value_type* _data;
        std::size_t _array_length;
        std::size_t _string_length;
        
    public:
        basic_string() :
            _data{ nullptr } ,
            _array_length{ 0 } ,
            _string_length{ 0 }
        {}
            
        basic_string( const basic_string& other ) : basic_string( other.data() )
        {}  
        
        basic_string( const value_type* c_str ) :
            _array_length{  std::strlen( c_str ) + 1 } ,
            _string_length{ std::strlen( c_str ) }
        {
            _data = new value_type[_array_length];
            
            std::copy( c_str , c_str + _array_length , _data );
        }
  
#ifndef USE_COPY_AND_SWAP
        basic_string& operator=( const basic_string& other )
        {
            return (*this).operator=( other.data() );
        }  
#endif
        
        basic_string& operator=( const value_type* c_str )
        {
            _array_length  = std::strlen( c_str ) + 1;
            _string_length = std::strlen( c_str );
            
            delete _data;
            _data = new value_type[_array_length];
    
            std::copy( c_str , c_str + _array_length , _data );
            
            
            return *this;
        }
            
        ~basic_string()
        {
            delete[] _data;
            _data = nullptr;
            _array_length  = 0;
            _string_length = 0;
        }
        
        
#ifndef USE_COPY_AND_SWAP
        basic_string(basic_string&& other )
        {
            _data = other._data;
            other._data = nullptr;
            
            _array_length = other._array_length;
            other._array_length = 0;
            
            _string_length = other._string_length;
            other._string_length = 0;
        }
        
        basic_string& operator=( basic_string&& other )
        {
            delete[] _data;
            
            _data = other._data;
            other._data = nullptr;
            
            _array_length = other._array_length;
            other._array_length = 0;
            
            _string_length = other._string_length;
            other._string_length = 0;
            
            
            return *this;
        }
#else
        basic_string( basic_string&& other ) : basic_string()
        {
            using std::swap; //enable ADL
            
            swap( *this , other );
        }
        
        basic_string& operator=( basic_string other )
        {
            using std::swap; //enable ADL
            
            swap( *this , other );
            
            return *this;
        }       
#endif
        
        std::size_t size() const
        {
            return _string_length;
        }
        
        std::size_t length() const
        {
            return size();
        }
        
        value_type* const data() const
        {
            return _data;
        }
        
        
        
        value_type operator[]( std::size_t index ) const
        {
            return _data[index];
        }
        
        value_type& operator[]( std::size_t index )
        {
            return _data[index];
        }
        
        value_type at( std::size_t index ) const
        {
            if( index >= _string_length ) throw std::out_of_range{};
            
            return (*this)[index];
        }
        
        value_type& at( std::size_t index )
        {
            if( index >= _string_length ) throw std::out_of_range{};
            
            return (*this)[index];
        }
        
        
        struct iterator : public std::iterator<std::bidirectional_iterator_tag,T>,
                          public dl32::postincrement_op<iterator> , public dl32::postdecrement_op<iterator> , public dl32::complete_comparison_ops<iterator>
        {
            std::reference_wrapper<cpp::basic_string<T>> _str;
            std::size_t _index;
            
            iterator( const decltype(_str)& str_ref , std::size_t index ) :
                _str( str_ref ) ,
                _index( index )
            {}
            
            value_type& operator*()
            {
                return _str.get()[_index];
            }
            
            value_type operator*() const
            {
                return _str.get()[_index];
            }
            
            void operator++()
            {
                _index++;
            }
            
            void operator--()
            {
                _index--;
            }
            
            
            friend bool operator==( const iterator& lhs , const iterator& rhs )
            {
                return lhs._index == rhs._index;
            }
            
            friend bool operator>( const iterator& lhs , const iterator& rhs )
            {
                return lhs._index > rhs._index;
            }
        };
        
        iterator begin() const
        {
            return iterator{ std::ref( const_cast<cpp::basic_string<T>&>( *this ) ) , 0u };
        }
        
        iterator end() const
        {
            return iterator{ std::ref( const_cast<cpp::basic_string<T>&>( *this ) ) , _string_length };
        }
        
        
        
        template<typename U>
        friend void swap( cpp::basic_string<U>& lhs , cpp::basic_string<U>& rhs );
    };
    
    template<typename T>
    void swap( cpp::basic_string<T>& lhs , cpp::basic_string<T>& rhs )
    {
        using std::swap;
        
        swap( lhs._data          , rhs._data          );
        swap( lhs._array_length  , rhs._array_length  );
        swap( lhs._string_length , rhs._string_length );
    }
    
    
    
    
    using string    = cpp::basic_string<char>;
    using wstring   = cpp::basic_string<wchar_t>;
    using u16string = cpp::basic_string<char16_t>;
    using u32string = cpp::basic_string<char32_t>;
}

namespace std
{
    template<typename T>
    void swap( cpp::basic_string<T>& lhs , cpp::basic_string<T>& rhs )
    {
        cpp::swap( lhs , rhs );
    }
}

#endif	/* STRING_HPP */

