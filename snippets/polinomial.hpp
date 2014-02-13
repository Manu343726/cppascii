
#ifndef POLINOMIAL_HPP
#define POLINOMAIL_HPP

#include "operators.hpp"
#include "value_wrapper.hpp"

#include <vector>
#include <cmath>

namespace cpp
{
    struct monomial
    {
        int exponent;
        float term;

        monomial( float ter , int exp = 0 ) :
            term( ter ) ,
            exponent( exp ) 
        {}

        float operator()( float x ) const
        {
            return std::pow( x , exponent ) * term;
        }
        
        friend monomial operator-( const monomial& lhs )
        {
            return { -lhs.term , lhs.exponent };
        }
    };

    class polinomial
    {
        std::vector<cpp::monomial> terms;

    public:
        polinomial( decltype( terms ) _terms ) :
            terms{ std::move( _terms ) } 
        {}
            
        polinomial( float ind_term ) :
            terms{ 1 , cpp::monomial{ ind_term , 0 } }
        {}

        float operator()( float x ) const 
        {
            float result = 0.0f;
            
            for( const auto& term : terms )
                result += term( x );
            
            return result;
        }
    };
    
    
    
    
    //Brujería bellow:
    
    
    namespace polinomial_symbols
    {
        struct x_t
        {
            operator cpp::monomial() const
            {
                return cpp::monomial{ 1 , 1 };
            }
        } X;
        
        
        // a*X
        cpp::monomial operator*( float coef , const x_t& )
        {
            return { coef , 1 };
        }
        
        //X^n
        cpp::monomial operator^( const x_t& , int exp )
        {
            return { 1 , exp };
        }
        
        cpp::monomial operator^( const cpp::monomial& lhs , int exp )
        {
            return { lhs.term , exp };
        }
    }
    
    namespace polinomial_grammar
    {
        template<typename T>
        struct child_node
        {
            const T& node;
            
            child_node( const T& _node ) :
                node( _node ) 
            {}
            
            void process( std::vector<cpp::monomial>& terms , bool opposite ) const
            {
                if( opposite )
                    terms.emplace_back( -node );
                else
                    terms.emplace_back( node );
            }
        };
        
        template<typename LHS , typename RHS>
        struct addition_op
        {
            const LHS& lhs;
            const RHS& rhs;
            
            addition_op( const LHS& _lhs , const RHS& _rhs ) :
                lhs( _lhs ) , 
                rhs( _rhs )
            {}
            
            void process( std::vector<cpp::monomial>& terms , bool opposite ) const
            {
                lhs.process( terms , opposite );
                rhs.process( terms , opposite );
            }
            
            operator cpp::polinomial() const
            {
                std::vector<cpp::monomial> terms;
                
                process( terms , false );
                
                return cpp::polinomial{ terms };
            }
            
            cpp::polinomial operator()() const
            {
                return static_cast<cpp::polinomial>( *this );
            }
        };
        
        template<typename LHS , typename RHS>
        struct substraction_op
        {
            const LHS& lhs;
            const RHS& rhs;
            
            substraction_op(const LHS& _lhs , const RHS& _rhs ) :
                lhs( _lhs ) , 
                rhs( _rhs )
            {}
            
            void process( std::vector<cpp::monomial>& terms , bool opposite ) const
            {
                lhs.process( terms ,  opposite );
                rhs.process( terms , !opposite );
            }
            
            operator cpp::polinomial() const
            {
                std::vector<cpp::monomial> terms;
                
                process( terms , false );
                
                return cpp::polinomial{ terms };
            }
            
            cpp::polinomial operator()() const
            {
                return static_cast<cpp::polinomial>( *this );
            }
        };
        
        
        //monomio + número
        addition_op<child_node<cpp::monomial>,child_node<float>> operator+( const cpp::monomial& lhs , const float& rhs )
        {
            return { child_node<cpp::monomial>{ lhs } , child_node<float>{ rhs } };
        }
        
        //monomio + monomio
        addition_op<child_node<cpp::monomial>,child_node<cpp::monomial>> operator+( const cpp::monomial& lhs , const cpp::monomial& rhs )
        {
            return { child_node<cpp::monomial>{ lhs } , child_node<cpp::monomial>{ rhs } };
        }
        
        
        
        template<typename RHS>
        substraction_op<child_node<cpp::monomial>,child_node<RHS>> operator-( const cpp::monomial& m , const RHS& rhs )
        {
            return { child_node<cpp::monomial>{ m } , child_node<RHS>{ rhs } };
        }
    }
}

#endif /* POLYNOMIAL_HPP */
