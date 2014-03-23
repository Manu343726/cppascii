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

#ifndef VECTOR_3D_HPP
#define	VECTOR_3D_HPP

#include "operators.hpp"
#include "numeric_comparisons.hpp"

namespace cpp
{
    enum class vector_category : int
    {
        point  = 1,
        vector = 0,
        unknown = -1
    };

    template<typename T>
    struct vector_3d : public dl32::addition_op<vector_3d<T>> ,
                       public dl32::substraction_op<vector_3d<T>> ,
                       public dl32::multiplication_op<vector_3d<T>,T,true> ,
                       public dl32::division_op<vector_3d<T>,T,false>
    {
        T x , y , z , w;

        vector_3d( T x , T y , T z , cpp::vector_category category = cpp::vector_category::point ) :
            x( x ) ,
            y( y ) , 
            z( z ) ,
            w( (T) category )
        {
            if( category == cpp::vector_category::unknown )
                throw;
        }

        vector_3d( cpp::vector_category category = cpp::vector_category::point ) : vector_3d( T() , T() , T() , category ) 
        {}

        template<typename U>
        operator vector_3d<U>() 
        {
            return vector_3d<T>( x , y , z , category() );
        }

        vector_3d& operator+=( const vector_3d& rhs )
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;

            return *this;
        }

        vector_3d& operator-=( const vector_3d& rhs )
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;

            return *this;
        }

        vector_3d& operator*=( const T& rhs )
        {
            x *= rhs;
            y *= rhs;
            z *= rhs;

            return *this;
        }

        vector_3d& operator/=( const T& rhs )
        {
            x /= rhs;
            y /= rhs;
            z /= rhs;

            return *this;
        }

        friend vector_3d operator*( const vector_3d& lhs , const vector_3d& rhs )
        {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
        }

        T squared_length() const
        {
            return x*x + y*y + z*z;
        }

        T length() const
        {
            return std::sqrt( squared_length() );
        }

        void normalize()
        {
            T length = this->length();

            x /= length;
            y /= length;
            z /= length;
        }

        vector_3d normlized() const
        {
            vector_3d result( *this );

            result.normalize();

            return result;
        }

        cpp::vector_category category() const
        {
            if( cpp::wrap( w ) == cpp::wrap( 0 ) )
                return cpp::vector_category::vector;

            if( cpp::wrap( w ) == cpp::wrap( 1 ) )
                return cpp::vector_category::point;

            return cpp::vector_category::unknown;
        }

        bool is_point() const
        {
            return category() == cpp::vector_category::point;
        }

        bool is_vector() const
        {
            return category() == cpp::vector_category::vector;
        }

        void convert_to_point() 
        {
            x = 1;
        }

        void convert_to_vector()
        {
            w = 0;
        }

        vector_3d as_point() const
        {
            vector_3d result( *this );

            result.convert_to_point();

            return result;
        }

        vector_3d as_vector() const
        {
            vector_3d result( *this );

            result.convert_to_vector();

            return result;
        }
    };
}

#endif	/* VECTOR_3D_HPP */

