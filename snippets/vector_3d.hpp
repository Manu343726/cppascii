/* 
 * File:   vector_3d.hpp
 * Author: manu343726
 *
 * Created on 3 de diciembre de 2013, 12:53
 */

#ifndef VECTOR_3D_HPP
#define	VECTOR_3D_HPP

#include "operators.h"
#include "floating_point_helper.h"

namespace ig
{
    namespace math
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
            
            vector_3d( T x , T y , T z , ig::math::vector_category category = ig::math::vector_category::point ) :
                x( x ) ,
                y( y ) , 
                z( z ) ,
                w( (T) category )
            {
                if( category == ig::math::vector_category::unknown )
                    throw;
            }
            
            vector_3d( ig::math::vector_category category = ig::math::vector_category::point ) : vector_3d( T() , T() , T() , category ) 
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
            
            ig::math::vector_category category() const
            {
                if( dl32::numeric_comparer<T>::is_zero( w ) )
                    return ig::math::vector_category::vector;
                
                if( dl32::numeric_comparer<T>::are_equal( w , 1 ) )
                    return ig::math::vector_category::point;
                
                return ig::math::vector_category::unknown;
            }
            
            bool is_point() const
            {
                return category() == ig::math::vector_category::point;
            }
            
            bool is_vector() const
            {
                return category() == ig::math::vector_category::vector;
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
}

#endif	/* VECTOR_3D_HPP */

