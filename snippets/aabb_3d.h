/* Manuel S�nchez P�rez */
/* Rayner Tan           */

/* Esta clase representa AABBs 2D. Contiene m�todos para la facil creaci�n de AABBs, as� como test de colisi�n y pertenencia. */


#ifndef AABB_2D_H
#define AABB_2D_H

#include "ig_types.h"
#include "value_wrapper.hpp"
#include "numeric_comparisons.hpp"

#include <vector>

namespace ig {

    template<typename T>
    class aabb_3d : public ig::entity_3d_traits<T> {
    public:
        typedef ig::entity_3d_traits<T> traits;
    private:

        aabb_3d(typename traits::coordinate_type x, typename traits::coordinate_type y, typename traits::coordinate_type z,
                typename traits::coordinate_type width, typename traits::coordinate_type height , typename traits::coordinate_type depth ) :
        origin(x , y , z),
        size(width, height, depth) {
        }

    public:
        typename traits::position_type origin;
        typename traits::size_type size;

        //Construction through the named constructor idiom:

        static aabb_3d empty() {
            return aabb_3d(0, 0, 0, 0 , 0 , 0);
        }

        static aabb_3d from_coords_and_size(typename traits::coordinate_type x, typename traits::coordinate_type y, typename traits::coordinate_type z,
                                            typename traits::coordinate_type width, typename traits::coordinate_type height , typename traits::coordinate_type depth ) 
        {
            return aabb_3d(x, y, z, width, height, depth);
        }

        static aabb_3d from_coords_and_size(const typename traits::position_type& origin, const typename traits::size_type& size) {
            return aabb_3d(origin.x, origin.y,origin.z, size.x, size.y,size.z);
        }

        static aabb_3d from_corners(const typename traits::position_type& front_bottom_left, const typename traits::position_type& back_top_right) {
            return aabb_3d(front_bottom_left.x, front_bottom_left.y, front_bottom_left.z,
                           (back_top_right.x - front_bottom_left.x), (back_top_right.y - front_bottom_left.y), (back_top_right.z - front_bottom_left.z));
        }

        static aabb_3d from_limits(typename traits::coordinate_type top, typename traits::coordinate_type bottom, typename traits::coordinate_type left, typename traits::coordinate_type right , 
                                   typename traits::coordinate_type front , typename traits::coordinate_type back ) 
        {
            return aabb_3d(left, bottom, front, (right - left), (top - bottom ) , (back-bottom));
        }

        static aabb_3d from_points(const std::initializer_list<typename traits::position_type>& points) {
            typename traits::coordinate_type top(points.begin()->y), 
                                             bottom(points.begin()->y), 
                                             left(points.begin()->x), 
                                             right(points.begin()->x),
                                             front(points.begin()->z), 
                                             back(points.begin()->z);

            for (auto& point : points) {
                if (left > point.x) left = point.x;
                if (right < point.x) right = point.x;
                if (top < point.y) top = point.y;
                if (bottom > point.y) bottom = point.y;
                if (back < point.z) back = point.z;
                if (front > point.z) front = point.z;
            }

            return from_limits(top, bottom, left, right , back , front);
        }
        
        
        void expand( T width_offset , T height_offset , T depth_offset )
        {
            size.x += width_offset;
            size.y += height_offset;
            size.z += depth_offset;
            
            origin.x -= width_offset  / 2;
            origin.y -= height_offset / 2;
            origin.z -= depth_offset  / 2;
        }

        //Conversion between different coordinate types:

        template<typename U>
        operator aabb_3d<U>() const
        {
            return ig::aabb_3d<U>::from_coords_and_size(origin, size);
        }

        //Getters (Info)

        typename traits::position_type center() const {
            return origin + (size / 2);
        }

        typename traits::coordinate_type width() const {
            return size.x;
        }

        typename traits::coordinate_type height() const {
            return size.y;
        }

        typename traits::coordinate_type top() const {
            return origin.y + size.y;
        }

        typename traits::coordinate_type left() const {
            return origin.x;
        }

        typename traits::coordinate_type bottom() const {
            return origin.y;
        }

        typename traits::coordinate_type right() const {
            return origin.x + size.x;
        }

        typename traits::position_type front_top_left_corner() const {
            return position_type(origin.x, origin.y + size.y , origin.z);
        }

        typename traits::position_type front_top_right_corner() const {
            return position_type(origin.x + size.x, origin.y + size.y , origin.z);
        }

        typename traits::position_type front_bottom_right_corner() const {
            return position_type(origin.x + size.x, origin.y , origin.z);
        }

        typename traits::position_type front_bottom_left_corner() const {
            return origin;
        }
        
        typename traits::position_type back_top_left_corner() const {
            return position_type(origin.x, origin.y + size.y , origin.z + size.z);
        }

        typename traits::position_type back_top_right_corner() const {
            return position_type(origin.x + size.x, origin.y + size.y , origin.z + size.z);
        }

        typename traits::position_type back_bottom_right_corner() const {
            return position_type(origin.x + size.x, origin.y , origin.z + size.z);
        }

        typename traits::position_type back_bottom_left_corner() const {
            return position_type(origin.x, origin.y , origin.z + size.z);
        }

        static bool overlap(const aabb_3d& a, const aabb_3d& b) {
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

            T a_left   = a.left();
            T a_right  = a.right();
            T a_bottom = a.bottom();
            T a_top    = a.top();
            T a_front  = a.front();
            T a_back   = a.back();

            T b_left   = b.left();
            T b_right  = b.right();
            T b_bottom = b.bottom();
            T b_top    = b.top();
            T b_front  = b.front();
            T b_back   = b.back();

            bool a_right_bigger_b_left = a_right > b_left;
            bool a_left_lesser_b_right = a_left < b_right;
            bool a_top_bigger_b_bottom = a_top > b_bottom;
            bool a_bottom_lesser_b_top = a_bottom < b_top;
            bool a_back_bigger_b_front = a_back > b_front;
            bool a_front_lesser_b_back = a_front < b_back;

            return a_right_bigger_b_left &&
                   a_left_lesser_b_right &&
                   a_top_bigger_b_bottom &&
                   a_bottom_lesser_b_top &&
                   a_back_bigger_b_front &&
                   a_front_lesser_b_back;

        }

        bool belongs_to(const typename traits::position_type& point) const {
            return cpp::wrap( point.x ) >= cpp::wrap( left() )   &&
                   cpp::wrap( point.x ) <= cpp::wrap( right() )  &&
                   cpp::wrap( point.y ) >= cpp::wrap( bottom() ) &&
                   cpp::wrap( point.y ) <= cpp::wrap( top() )    &&
                   cpp::wrap( point.z ) >= cpp::wrap( front() )  &&
                   cpp::wrap( point.z ) <= cpp::wrap( back() );
        }
    };

    //Apa�os (Simplifican las cosas):

    template<typename T>
    class aabb_3d<ig::math::vector_3d<T >> : public aabb_3d<T>
    {
    };

    template<typename T>
    class aabb_3d<ig::entity_3d_traits<T >> : public aabb_3d<T>
    {
    };
}

#endif
