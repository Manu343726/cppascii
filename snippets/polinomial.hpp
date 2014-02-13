
#ifndef POLINOMIAL_HPP
#define POLINOMAIL_HPP

#include "operators.hpp"
#include "value_wrapper.hpp"

#include "Turbo/core.hpp"

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
            T node;
            
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
            LHS lhs;
            RHS rhs;
            
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
            LHS lhs;
            RHS rhs;
            
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
        
        
        //Un poco de ayuda para la resolución de sobrecargas:
        
        TURBO_DEFINE_FUNCTION( is_operator , (typename T) , (T) , (tml::false_type) );
        
        template<typename LHS , typename RHS>
        struct is_operator_t<addition_op<LHS,RHS>>     : public tml::function<tml::true_type> {};
        
        template<typename LHS , typename RHS>
        struct is_operator_t<substraction_op<LHS,RHS>> : public tml::function<tml::true_type> {};
        
        //Sobrecarga de operadores:
        
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
        
        //monomio + operador binario
        template<typename OP , typename = typename std::enable_if<is_operator<OP>::value>::type>
        addition_op<child_node<cpp::monomial>,OP> operator+( const cpp::monomial& lhs , const OP& rhs )
        {
            return { child_node<cpp::monomial>{ lhs } , rhs };
        }
        
        //operador binario + monomio
        template<typename OP , typename = typename std::enable_if<is_operator<OP>::value>::type>
        addition_op<OP,child_node<cpp::monomial>> operator+( const OP& lhs , const cpp::monomial& rhs )
        {
            return { lhs , child_node<cpp::monomial>{ rhs } };
        }
        
        //múmero + operador binario
        template<typename OP , typename = typename std::enable_if<is_operator<OP>::value>::type>
        addition_op<child_node<float>,OP> operator+( const float& lhs , const OP& rhs )
        {
            return { child_node<float>{ lhs } , rhs };
        }
        
        //operador binario + número
        template<typename OP , typename = typename std::enable_if<is_operator<OP>::value>::type>
        addition_op<OP,child_node<float>> operator+( const OP& lhs , const float& rhs )
        {
            return { lhs , child_node<float>{ rhs } };
        }
        
        //operador binario + operador binario
        template<typename OP1 , typename OP2 , 
                 typename = typename std::enable_if<is_operator<OP1>::value && is_operator<OP2>::value>::type>
        addition_op<OP1,OP2> operator+( const OP1& lhs , const OP2& rhs )
        {
            return { lhs , rhs };
        }
        
        
        //monomio - número
        substraction_op<child_node<cpp::monomial>,child_node<float>> operator-( const cpp::monomial& lhs , const float& rhs )
        {
            return { child_node<cpp::monomial>{ lhs } , child_node<float>{ rhs } };
        }
        
        //monomio - monomio
        substraction_op<child_node<cpp::monomial>,child_node<cpp::monomial>> operator-( const cpp::monomial& lhs , const cpp::monomial& rhs )
        {
            return { child_node<cpp::monomial>{ lhs } , child_node<cpp::monomial>{ rhs } };
        }
        
        //monomio - operador binario
        template<typename OP , typename = typename std::enable_if<is_operator<OP>::value>::type>
        substraction_op<child_node<cpp::monomial>,OP> operator-( const cpp::monomial& lhs , const OP& rhs )
        {
            return { child_node<cpp::monomial>{ lhs } , rhs };
        }
        
        //operador binario - monomio
        template<typename OP , typename = typename std::enable_if<is_operator<OP>::value>::type>
        substraction_op<OP,child_node<cpp::monomial>> operator-( const OP& lhs , const cpp::monomial& rhs )
        {
            return { lhs , child_node<cpp::monomial>{ rhs } };
        }
        
        //múmero - operador binario
        template<typename OP , typename = typename std::enable_if<is_operator<OP>::value>::type>
        substraction_op<child_node<float>,OP> operator-( const float& lhs , const OP& rhs )
        {
            return { child_node<float>{ lhs } , rhs };
        }
        
        //operador binario - número
        template<typename OP , typename = typename std::enable_if<is_operator<OP>::value>::type>
        substraction_op<OP,child_node<float>> operator-( const OP& lhs , const float& rhs )
        {
            return { lhs , child_node<float>{ rhs } };
        }
        
        //operador binario - operador binario
        template<typename OP1 , typename OP2 , 
                 typename = typename std::enable_if<is_operator<OP1>::value && is_operator<OP2>::value>::type>
        substraction_op<OP1,OP2> operator-( const OP1& lhs , const OP2& rhs )
        {
            return { lhs , rhs };
        }
    }
}

#endif /* POLYNOMIAL_HPP */
