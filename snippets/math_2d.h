/*******************************************************************************
* cpp_lib32 project. C++ port of the dx_lib32 project.                         *
*                                                                              *
* Copyright (C) 2012 - 2013, Manuel Sánchez Pérez                              *                     
*                                                                              *
* This file is part of cpp_lib32 project.                                      *
*                                                                              *
* cpp_lib32 project is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as published by  *
* the Free Software Foundation, version 2 of the License.                      *
*                                                                              *
* cpp_lib32 is distributed in the hope that it will be useful,                 *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               * 
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU Lesser General Public License for more details.                          *
*                                                                              *
 You should have received a copy of the GNU Lesser General Public License      *
 along with cpp_lib32 project. If not, see <http://www.gnu.org/licenses/>.     *
*******************************************************************************/

/* Al igual que en muchas otras clases, aquí usaba enable_if para activar una funcionalidad u otra. Al portar el código a MSVC11 para usarlo en
 * las prácticas he cambiado algunas cosas */

#ifndef DL32NEWMATH_H
#define	DL32NEWMATH_H

#include "operators.hpp"
#include "numeric_comparisons.hpp"

#include <cmath>
#include <limits>
#include <sstream>
#include <string>

namespace dl32
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @brief cpp_lib32 vector implementation types settings.
	///
	/// @author	Manu343726
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	enum class vector_impl
	{
		HOMEBREW, ///< Homebrew vector2d/3d implementation.
		DIRECT3D, ///< Implementation based on Direct3D algebra utilities API.
		OPENGL    ///< Implementation based on OpenGL algebra utilities API.
	};

	namespace internal
	{
		const dl32::vector_impl current_vector_impl = dl32::vector_impl::HOMEBREW; ///< Vector implementation configuration.

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @brief This class holds the coordinates of a 2d element.
		///
		/// @details The homebrew implementation of 2d points and 2d vectors 
		///
		/// @author	Manu343726
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		template<typename T>
		struct coords_2d_holder
		{
			//Restrict coordinates to floating-point and integrals only: (Waiting for C++14 concepts lite...)
			static_assert( std::is_floating_point<T>::value || std::is_integral<T>::value , "2d elements must have integral or floating-point types as coordinates");

			static const unsigned int dimensions = 2; ///< Dimensional range of the point.
			typedef T coord_type; ///< Type used for the coordinates.

			union
			{
				struct
				{
					T x; ///< First coordinate.
					T y; ///< Second coordinate.
				};
				T coords[dimensions]; ///< Vector coordinates in array-style. First element corresponds with x, second element with y.
			};

			coords_2d_holder() : x(0) , y(0) {}
			coords_2d_holder(T _x , T _y) : x(_x) , y(_y) {}

			T& operator[](std::size_t index)       { return index == 0 ? x : y; }
			T  operator[](std::size_t index) const { return index == 0 ? x : y; }
		};


		///////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @brief This class provides floating-point and integral equality comparison criteria for 2d entities.
		///
		/// @author	Manu343726
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		template<typename T , bool IS_FLOATING_POINT = std::is_floating_point<T>::value>
		struct coords_2d_comparer;


		//floating-point specialization:
		template<typename T>
		struct coords_2d_comparer<T,true> : public coords_2d_holder<T>
		{
			//Ctor for subclasses:
			coords_2d_comparer()          : coords_2d_holder<T>() {}
			coords_2d_comparer(T x , T y) : coords_2d_holder<T>(x,y) {}


			///////////////////////////////////////////////////////////////////////////////////////////////////////
			/// @brief Equality operator for two 2d entities. 
			/// @details This version is enabled only for floating-point coordinates. 
			/// 
			/// @param m1 First entity to be compared.
			/// @param m2 Second entity to be compared.
			///
			/// @return Returns true if coordinates of m1 and m2 are the same. Returns false in other case.
			///
			/// @author	Manu343726
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			friend bool operator==(const coords_2d_comparer& lhs , const coords_2d_comparer& rhs)
			{
				return cpp::wrap(lhs.x ) == cpp::wrap( rhs.x ) &&
					   cpp::wrap(lhs.y ) == cpp::wrap( rhs.y );
			}
		};

		//Integral specialization:
		template<typename T>
		struct coords_2d_comparer<T,false> : public coords_2d_holder<T>
		{
			//Ctor for subclasses:
			coords_2d_comparer()          : coords_2d_holder<T>() {}
			coords_2d_comparer(T x , T y) : coords_2d_holder<T>(x,y) {}


			///////////////////////////////////////////////////////////////////////////////////////////////////////
			/// @brief Equality operator for two 2d entities. 
			/// @details This version is enabled only for integral coordinates. 
			/// 
			/// @param m1 First entity to be compared.
			/// @param m2 Second entity to be compared.
			///
			/// @return Returns true if coordinates of m1 and m2 are the same. Returns false in other case.
			///
			/// @author	Manu343726
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			friend bool operator==(const coords_2d_comparer& lhs , const coords_2d_comparer& rhs)
			{
				return lhs.x == rhs.x && lhs.y == rhs.y;
			}
		};

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @brief This class provides a homebrew implementation of a 2d vector and its most common algebraic
		///        operations.
		///
		/// @tparam IMPLEMENTER Class that uses (inherits from) this implementation (CRTP subclass)
		/// @tparam T Vector implementation coordinates type. Its default value is float.
		/// @tparam BASIC_ALGEBRA_ONLY Defines the set of algebra operations to be implemented. If is
		///                            is set to true, the class includes addition substraction, multiplication,
		///                            and division. If is set to false, it includes addition and substraction
		///                            only.
		///
		/// @author	Manu343726
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		template<typename IMPLEMENTER , typename T = float>
		struct entity_2d_impl_homebrew : public coords_2d_comparer<T>
		{
    
    
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			/// @brief Default constructor. Set all coordinates to zero.
			///
			/// @author	Manu343726
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			entity_2d_impl_homebrew() : coords_2d_comparer<T>() {}
    
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			/// @brief Constructor. Creates a vector with the specified coordinates.
			///
			/// @param _x First coordinate of the vector.
			/// @param _y Second coordinate of the vector.
			///
			/// @author	Manu343726
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			entity_2d_impl_homebrew(const T& _x ,const T& _y) : coords_2d_comparer<T>(_x , _y) {}
    
    
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			/// @brief Addition implementation. This function performs addition of a provided vector to this instance.
			///
			/// @author	Manu343726
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			IMPLEMENTER& operator+=(const IMPLEMENTER& rhs)
			{
				this->x += rhs.x;
				this->y += rhs.y;
        
				return static_cast<IMPLEMENTER&>(*this);
			}
    
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			/// @brief Substraction implementation. This function performs substraction of a provided vector to this 
			///        instance.
			///
			/// @author	Manu343726
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			IMPLEMENTER& operator-=(const IMPLEMENTER& rhs)
			{
				this->x -= rhs.x;
				this->y -= rhs.y;
        
				return static_cast<IMPLEMENTER&>(*this);
			}
    
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			/// @brief Multiplication implementation. This function performs multiplication of a provided vector to 
			///        this instance.
			/// @details This function is enabled only for implementers wich use complete algebra ( BASIC_ALGEBRA_ONLY == false )
			///
			/// @author	Manu343726
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			IMPLEMENTER& operator*=(const T& n)
			{
				this->x *= n;
				this->y *= n;
        
				return static_cast<IMPLEMENTER&>(*this);
			}
        
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			/// @brief Division implementation. This function performs division of a provided vector to 
			///        this instance.
			/// @details This function is enabled only for implementers wich use complete algebra ( BASIC_ALGEBRA_ONLY == false )
			///
			/// @author	Manu343726
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			IMPLEMENTER& operator/=(const T& n)
			{
				this->x /= n;
				this->y /= n;
        
				return static_cast<IMPLEMENTER&>(*this);
			}
            
            friend T operator*( const IMPLEMENTER& lhs , const IMPLEMENTER& rhs )
            {
                return lhs.x*rhs.x + lhs.y*rhs.y;
            }
    
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			/// @brief Gets a string representation of the 2d vector.
			///
			/// @details MSVC does not still implement std::to_string()
			///
			/// @author	Manu343726
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			std::string to_string() const
			{
				std::stringstream ss;
        
				ss << "{" << this->x << "," << this->y << "}";
        
				return ss.str();
			}


			friend std::ostream& operator<<(std::ostream& os , const entity_2d_impl_homebrew& vector)
			{
				os << vector.to_string();

				return os;
			}
		};

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @brief This class provides a Direct3D algebra utilities API based implementation of a 2d vector and 
		///        its most common algebraic operations. NOT IMPLEMENTED. PROVISIONAL DEFINITION.
		///
		/// @author	Manu343726
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		template<typename IMPLEMENTER , typename T = float>
		struct entity_2d_implt_direct3d{};
	}




	///////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @brief This class represents a vector in 2d space.
	///
	/// @tparam T Type used for point coordinates.
	/// @author	Manu343726
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	struct vector_2d : public dl32::internal::entity_2d_impl_homebrew<dl32::vector_2d<T>,T> ,
		               public dl32::equality_ops<dl32::vector_2d<T>>,        //Binary operator!=(dl32::vector_2d<T>,dl32::vector_2d<T>)
					   public dl32::basic_algebra_ops<dl32::vector_2d<T>>,   //Binary operator+ (dl32::vector_2d<T>,dl32::vector_2d<T>) , binary operator-(dl32::vector_2d<T>,dl32::vector_2d<T>)
					   public dl32::multiplication_op<dl32::vector_2d<T>,T>, //Binary operator* (dl32::vector_2d<T>,T)
				       public dl32::division_op<dl32::vector_2d<T>,T>        //Binary operator/ (dl32::vector_2d<T>,T)
	{
		//Constructors:

		vector_2d()                                                : dl32::internal::entity_2d_impl_homebrew<dl32::vector_2d<T>,T>()                            {}
		vector_2d(const T& _x ,const T& _y)                        : dl32::internal::entity_2d_impl_homebrew<dl32::vector_2d<T>,T>(_x,_y)                       {}
		vector_2d(const vector_2d<T>& p1 , const vector_2d<T>& p2) : dl32::internal::entity_2d_impl_homebrew<dl32::vector_2d<T>,T>( p2.x - p1.x , p2.y - p1.y ) {}
    

		template<typename U>
		operator dl32::vector_2d<U>()
		{
			return dl32::vector_2d<U>( this->x , this->y );
		}
    
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @brief Returns the length of the vector
		///
		/// @author	Manu343726
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		T length() const { return std::sqrt( this->x*this->x + this->y*this->y ); }
        
        vector_2d normal() const
        {
            return vector_2d( this->y , -this->x );
        }
    
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @brief Normalizes this vector (Converts this vector in a unit-lenght vector with the same direction).
		///
		/// @author	Manu343726
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		void normalize()
		{
			//Fast Inverse Square Root? Me lo pensaré...
			T length = this->length();
        
			this->x /= length;
			this->y /= length;
		}
    
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @brief Returns a normalized version of this vector.
		///
		/// @author	Manu343726
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		vector_2d normalized() const
		{
			dl32::vector_2d<T> ret( *this );
			ret.normalize();
        
			return ret;
		}

		void rotate(T angle)
		{
			T new_x;

			new_x   = this->x * std::cos(angle) - this->y * std::sin(angle);
			this->y = this->x * std::sin(angle) + this->y * std::cos(angle);
			this->x = new_x;
		}
        
        vector_2d reflexion( const vector_2d& axis ) const
        {
            vector_2d op( -this->x , -this->y );
            
            return ((2 * (axis * op)) * axis) - op;
        }
	};

	typedef dl32::vector_2d<int> vector_2di;    ///< Alias for integral 2d vectors.
	typedef dl32::vector_2d<float> vector_2df;  ///< Alias for single-precision 2d vectors.
	typedef dl32::vector_2d<double> vector_2dd; ///< Alias for double-precision 2d vectors.
}

//std::to_string() overload:
namespace std
{
	template<typename T>
	std::string to_string(const dl32::vector_2d<T>& vector)
	{
		return vector.to_string();
	}
}

#endif	/* DL32NEWMATH_H */

