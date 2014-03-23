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

#ifndef AABB_2D_H
#define AABB_2D_H

#include "math_2d.h"
#include "binary_literals.hpp"
#include "numeric_comparisons.hpp"

#include <vector>
#include <bitset>

namespace cpp {
    
    
    //Check the Cohen-Sutherland clipping algorithm
    enum class aabb_2d_area : std::uint8_t
    {
        inside     = 0000_b,
        north      = 1000_b,
        south      = 0100_b,
        east       = 0010_b,
        west       = 0001_b,
        north_east = 1010_b,
        north_west = 1001_b,
        south_east = 0110_b,
        south_west = 0101_b,
        unknown    = 1111_b
    };

    template<typename T>
    class aabb_2d
    {
    private:

        aabb_2d(T x, T y, T width, T height) :
        origin(x, y),
        size(width, height) {
        }

    public:
        dl32::vector_2d<T> origin;
        dl32::vector_2d<T> size;

        //Construction through the named constructor idiom:

        static aabb_2d empty() {
            return aabb_2d(0, 0, 0, 0);
        }

        static aabb_2d from_coords_and_size(T x, T y, T width, T height) {
            return aabb_2d(x, y, width, height);
        }

        static aabb_2d from_coords_and_size(const dl32::vector_2d<T>& origin, const dl32::vector_2d<T>& size) {
            return aabb_2d(origin.x, origin.y, size.x, size.y);
        }

        static aabb_2d from_corners(const dl32::vector_2d<T>& bottom_left, const dl32::vector_2d<T>& top_right) {
            return aabb_2d(bottom_left.x, bottom_left.y, (top_right.x - bottom_left.x), (top_right.y - bottom_left.y));
        }

        static aabb_2d from_limits(T top, T bottom, T left, T right) {
            return aabb_2d(left, bottom, (right - left), (top - bottom ));
        }

        static aabb_2d from_points(const std::initializer_list<dl32::vector_2d<T>>& points) {
            T top(points.begin()->y), bottom(points.begin()->y), left(points.begin()->x), right(points.begin()->x);

            for (auto& point : points) {
                if (top < point.y) top = point.y;
                if (bottom > point.y) bottom = point.y;
                if (left > point.x) left = point.x;
                if (right < point.x) right = point.x;
            }

            return from_limits(top, bottom, left, right);
        }
        
        
        void expand( T width_offset , T height_offset )
        {
            size.x += width_offset;
            size.y += height_offset;
            
            origin.x -= width_offset / 2;
            origin.y -= height_offset / 2;
        }

        //Conversion between different coordinate types:

        template<typename U>
        operator aabb_2d<U>() const
        {
            return cpp::aabb_2d<U>::from_coords_and_size(origin, size);
        }

        //Getters (Info)

        dl32::vector_2d<T> center() const {
            return origin + (size / 2);
        }

        T width() const {
            return size.x;
        }

        T height() const {
            return size.y;
        }

        T top() const {
            return origin.y + size.y;
        }

        T left() const {
            return origin.x;
        }

        T bottom() const {
            return origin.y;
        }

        T right() const {
            return origin.x + size.x;
        }

        dl32::vector_2d<T> top_left_corner() const {
            return dl32::vector_2d<T>(origin.x, origin.y + size.y);
        }

        dl32::vector_2d<T> top_right_corner() const {
            return dl32::vector_2d<T>(origin.x + size.x, origin.y + size.y);
        }

        dl32::vector_2d<T> bottom_right_corner() const {
            return dl32::vector_2d<T>(origin.x + size.x, origin.y);
        }

        dl32::vector_2d<T> bottom_left_corner() const {
            return origin;
        }

        static bool overlap(const aabb_2d& a, const aabb_2d& b) {
            /*******************************
             *                              *
             *     overlap_x                *
             *       <-->                   *
             *   o------o                   *
             *   |      |                   *
             *   |   o--+---o  ^            *
             *   |   |  |   |  | overlap_y  *
             *   o---+--o   |  v            *
             *       |      |               *
             *       o------o               *
             *                              *
             * Note the coorinate system:   *
             *                              *
             *   ^ y                        *
             *   |                          *
             *   O--> x                     *
             *                              *
             *******************************/

            T a_left = a.left();
            T a_right = a.right();
            T a_bottom = a.bottom();
            T a_top = a.top();

            T b_left = b.left();
            T b_right = b.right();
            T b_bottom = b.bottom();
            T b_top = b.top();

            bool a_right_bigger_b_left = a_right > b_left;
            bool a_left_lesser_b_right = a_left < b_right;
            bool a_top_bigger_b_bottom = a_top > b_bottom;
            bool a_bottom_lesser_b_top = a_bottom < b_top;

            return a_right_bigger_b_left &&
                    a_left_lesser_b_right &&
                    a_top_bigger_b_bottom &&
                    a_bottom_lesser_b_top;

        }

        bool belongs_to(const dl32::vector_2d<T>& point) const {
            return cpp::wrap( point.x ) >= cpp::wrap( left() ) &&
                   cpp::wrap( point.x ) <= cpp::wrap( right() ) &&
                   cpp::wrap( point.y ) >= cpp::wrap( bottom() ) &&
                   cpp::wrap( point.y ) <= cpp::wrap( top() );
        }
        
        cpp::aabb_2d_area relative_position( const dl32::vector_2d<T>& point ) const
        {
            std::bitset<4> bits{ 0 };
            
            bits[3] = cpp::wrap( point.y ) > cpp::wrap( top() );
            bits[2] = cpp::wrap( point.y ) < cpp::wrap( bottom() );
            bits[1] = cpp::wrap( point.x ) > cpp::wrap( right() );
            bits[0] = cpp::wrap( point.x ) < cpp::wrap( left() );
            
            return static_cast<cpp::aabb_2d_area>( bits.to_ulong() );
        }
    };
}

#endif
