
#ifndef POLINOMIAL_HPP
#define POLINOMAIL_HPP

#include "operators.hpp"

#include <vector>

namespace cpp
{
	namespace polinomial_symbols
	{
		struct x_t{} X;
	}

	struct monomial
	{
		int exponent;
		float term;

		monomial( float ter , int exp ) : 
			term( ter ) , 
			exponent( exp )
		{}

		float operator()( float x ) const
		{
			return std::pow( x , exponent ) * term;
		}
	};
		
    class polinomial
    {
        std::vector<cpp::monomial> terms;

	public:
		polinomial( decltype(terms) _terms ) : 
			terms( std::move( _terms ) ) 
		{}

        
        float operator()( float x ) const
        {
            
        }

    };
}

#endif /* POLYNOMIAL_HPP */
