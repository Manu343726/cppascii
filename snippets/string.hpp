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

//Una vez en la vida y nunca más: Muy mal ejemplo de C++, ejemplo tìpico de C++... (Irónico, verdad? Que la gente aprenda The Rule Of Zero de una maldita vez...)

namespace cpp
{
    template<typename T>
    class basic_string : public cpp::instantation_profiler<cpp::basic_string<T>>
    {
    public:
        using value_type = T; //Alias del tipo caracter usado, para el usuario (y nosotros)
        
    private:
        value_type* _data;
        std::size_t _array_length; //Ojo: Una cosa es la longitud de la cadena, y otra la longitud del array que la guarda. 
        std::size_t _string_length;
        
    public:
        //El estado por defecto es cadena sin instanciar y contadores a cero (Es un caso especial de cadena vacía)
        basic_string() :
            _data{ nullptr } ,
            _array_length{ 0 } ,
            _string_length{ 0 }
        {}
            
        //Reutilizamos el ctor de strings C para el ctor de copia (El código es idéntico)
        basic_string( const basic_string& other ) : basic_string( other.data() )
        {}  
        
        //Constructor de conversión desde C-style strings:
        basic_string( const value_type* c_str ) :
            _array_length{  std::strlen( c_str ) + 1 } , //He decidido usar null-terminated strings, porque es a lo que estáis acostumbrados. Pero no tiene por qué ser así, ya que tienes el contador.
            _string_length{ std::strlen( c_str ) }
        {
            _data = new value_type[_array_length]; //Crear array
            
            std::copy( c_str , c_str + _array_length , _data ); //Copiar de uno a otro
        }
  
#ifndef USE_COPY_AND_SWAP
        //Igual que en el contructor de copia: Usa la sobrecarga para C strings, para no repetir código
        basic_string& operator=( const basic_string& other )
        {
            return (*this).operator=( other.data() );
        }  
#endif
        //Asignación de lvalues y c-strings:
        //Está pensada para tanto raw strings como c-style strings. Si por mi fuera símplemente lo hacía para raw strings (Cogiendo un array por referencia constante, no usando el decaying a punteros),
        //pero igual es pasarse...
        basic_string& operator=( const value_type* c_str )
        {
            //Actualizo contadores
            _array_length  = std::strlen( c_str ) + 1;
            _string_length = std::strlen( c_str );
            
            //Redimensiono el array
            delete _data;
            _data = new value_type[_array_length];
    
            //Copio de uno a otro
            std::copy( c_str , c_str + _array_length , _data );
            
            //Ojo, muy importante
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
        //ctor para rvalues: Te robo lo tuyo y tu te quedas sin nada (Move semantics)
        basic_string(basic_string&& other )
        {
            _data = std::move( other._data );
            other._data = nullptr;
            
            _array_length = std::move( other._array_length );
            other._array_length = 0;
            
            _string_length = std::move( other._string_length );
            other._string_length = 0;
        }
        
        //Asignación de rvalues: Me libro de lo que tengo ahora, te robo lo tuyo, y tu te quedas sin nada (Move semantics)
        basic_string& operator=( basic_string&& other )
        {
            delete[] _data;
            
            _data = std::move( other._data );
            other._data = nullptr;
            
            _array_length = std::move( other._array_length );
            other._array_length = 0;
            
            _string_length = std::move( other._string_length );
            other._string_length = 0;
            
            
            return *this;
        }
#else
        //ctor para rvalues: Mira, yo estoy vacío y tu tienes cosas, pero tu vas a morir dentro de poco: Hacemos un trueque, vale? ("copy" and swap)
        basic_string( basic_string&& other ) : basic_string() //Ojo: importante
        {
            using std::swap;
            
            //LLamada sin cualificar: Que encuentre que sobrecarga es a través de ADL. Si no lo encuentra, 
            //le hemos dicho explícitamente que pruebe con std::swap()
            swap( *this , other ); 
        }
        
        //Asignación tanto de lvalues como rvalues: Me quedo con "una copia" (En el caso de rvalues no es una copia) 
        //de ti, y hago un trueque con ella. Así yo me quedo con lo tuyo y tu copia destruye lo mío al morir. (Copy and swap)
        basic_string& operator=( basic_string other )
        {
            using std::swap;
            
            //LLamada sin cualificar: Que encuentre que sobrecarga es a través de ADL. Si no lo encuentra, 
            //le hemos dicho explícitamente que pruebe con std::swap()
            swap( *this , other );
            
            return *this;
        }       
#endif
        
        
        std::size_t size() const
        {
            return _string_length;
        }
        
        //Hay gente que en las cadenas le gusta usar length(), por eso se provee éste alias
        std::size_t length() const
        {
            //Lo que os digo siempre: Reutilizar funcionalidad que ya tenéis hecha. 
            //Podría haber escrito "return _string_length;", pero eso es duplicar código.
            //La regal de oro es: Un fragmento de código debería aparecer solo una vez en toda la implementación, aunque sea de tres caracteres, me da igual.
            //Un solo sitio, un solo sitio en el que mirar bugs (Mantenibilidad++)
            //Cambio en el futuro? Cambio en un solo sitio, no en treinta (Mantenibilidad/Escalabilidad++)
            return size(); 
        }
        
        value_type* const data() const
        {
            return _data;
        }
        
        //Aquí uso el convenio de la biblioteca estandar: [] no chequea, at() si:
        
        //Sobrecarga para lectura:
        const value_type& operator[]( std::size_t index ) const
        {
            return _data[index];
        }
        
        //Sobrecarga para escritura:
        value_type& operator[]( std::size_t index )
        {
            return static_cast<value_type&>( (*this)[index] );
        }
        
        //Sobrecagra para lectura:
        value_type at( std::size_t index ) const
        {
            if( index >= _string_length ) throw std::out_of_range{};
            
            return (*this)[index]; //No duplicidad de código (OTRA VEZ. NO ME CANSARÉ DE REPETIRLO JAMÁS)
        }
        
        //Sobrecarga para escritura:
        value_type& at( std::size_t index )
        {
            if( index >= _string_length ) throw std::out_of_range{};
            
            return (*this)[index]; //No duplicidad de código (OTRA VEZ. NO ME CANSARÉ DE REPETIRLO JAMÁS)
        }
        
        //Uuhhh un iterador, como mola!
        struct iterator : public std::iterator<std::bidirectional_iterator_tag,T>, //Dile "a C++" que iterator es un iterador, y de un tipo concreto (Bidireccional en éste caso)
                          public dl32::postincrement_op<iterator> , public dl32::postdecrement_op<iterator> , public dl32::complete_comparison_ops<iterator> //Soy un vago (Ya lo veremos más adelante, en el tema 5)
        {
            std::reference_wrapper<cpp::basic_string<T>> _str;
            std::size_t _index;
            
            iterator( const decltype(_str)& str_ref , std::size_t index ) :
                _str( str_ref ) ,
                _index( index )
            {}

            //Desreferenciar el "puntero" (Iterador): Leer el caracter de la cadea en el que estamos
            value_type& operator*()
            {
                return _str.get()[_index];
            }
            
            //Lo mismo, pero para lectura
            value_type operator*() const
            {
                return _str.get()[_index];
            }
            
            //Avanzar "puntero" (iterador)
            void operator++()
            {
                ++_index;
            }
            
            //Retroceder "puntero" (iterador)
            void operator--()
            {
                --_index;
            }
            
            //Comparación de iteradores
            friend bool operator==( const iterator& lhs , const iterator& rhs )
            {
                return lhs._index == rhs._index;
            }
            
            //Comparación de iteradores
            friend bool operator>( const iterator& lhs , const iterator& rhs )
            {
                return lhs._index > rhs._index;
            }
        };
        
        iterator begin() const
        {
            //Lo del cast es una mierda, lo se. Pero no es mi culpa que *this sea const cpp::string&
            return iterator{ std::ref( const_cast<cpp::basic_string<T>&>( *this ) ) , 0u };
        }
        
        iterator end() const
        {
            //La misma mierda de cast que antes
            return iterator{ std::ref( const_cast<cpp::basic_string<T>&>( *this ) ) , _string_length };
        }
        
        
        //Lo declaramos friend para que nos deje meter mano por dentro
        template<typename U>
        friend void swap( cpp::basic_string<U>& lhs , cpp::basic_string<U>& rhs );
    };
    
    //Nuestra propia versión de swap, para evitar recursividad infinita:
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

//Ya que estamos sobrecargamos std::swap(), que no nos cuesta nada:
namespace std
{
    template<typename T>
    void swap( cpp::basic_string<T>& lhs , cpp::basic_string<T>& rhs )
    {
        cpp::swap( lhs , rhs );
    }
}

#endif	/* STRING_HPP */

