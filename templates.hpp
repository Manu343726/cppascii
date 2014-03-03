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

/**********************************************************************************
* Note that this code probably doesn't compile: It contains code examples written *
* partly as pseudocode                                                            *
**********************************************************************************/

//Class template tipo EDA
/////////////////////////
template<typename T>
struct foo
{
	T value;

	foo( const T& init_value = T{} ) : value{ init_value } 
	{}
};


//Template functions:
/////////////////////

template<typename T>
void f( const T& ) {}

f( 0 );
f( 'a' );
f( true );
f( 0u );
f( 0.0 );
f( 0.0f );
f( foo<int>{ 1 } );


//typedefs:
///////////

typedef int int_alias;
typedef unsigned int my_size_t;
....

// New C++11 type aliases syntax:
using int_alias = int;
using my_size_t = unsigned int;
//Preferible: Mucho más legible (Se parece a la inicialización de variables* y a otras construcciones muy parecidas, como los alias de namespaces)
//* Y lo más gracioso es que en metaprogramación son justamente eso: Inicialización de variables (Tipos).


//Template aliases ("typedefs"). Thank you C++11!
/////////////////////////////////////////////////

template<typename T>
using foo_alias = foo<T>;

foo_alias<int> a_int_foo;



//Template type parameters
//////////////////////////

template<typename FIRST , typename SECOND>
struct pair
{
	FIRST first;
	SECOND second;
};


//Template non-type parameters:
///////////////////////////////

template<std::size_t VALUE>
struct compile_time_size_t
{
	static const std::size_t value = VALUE;
};

//Mix:
//////

template<typename T , T VALUE>
struct integral_constant
{
	static const T value = VALUE;
};

using true_type  = integral_constant<bool,true>;
using false_type = integral_constant<bool,false>;


template<typename T , std::size_t LENGTH>
struct array
{
	T[LENGTH] _data;
};

//Template template parameters:
///////////////////////////////

template<template<typename> class T>
using give_me_a_one_type_parameter_template_and_i_will_return_you_a_boolean_instance = T<bool>;

using boolean_foo = give_me_a_one_type_parameter_template_instance_and_i_will_return_you_a_boolean_instance<foo>;



//Bonus syntax: Default template parameters:
////////////////////////////////////////////

template<typename T , typename ALLOCATOR = std::default_allocator<T>>
struct my_standard_container
{
	T* _data;
	ALLOCATOR alloc;
};


//Bonus syntax: Variadic templates:
///////////////////////////////////

template<typename... Ts>
struct typelist {};

using a_list     = typelist<float,int,bool>;
using other_list = typelist<float>;


template<typename... Ts>
using variadic_pack_length = integral_constant<std::size_t,sizeof...(Ts)>;


//LOL, a function that could get ANYTHING as parameter:

template<typename... Ts>
void f( const Ts&... args )
{

}



//Un ejemplo más complejo (realista):


void print()
{
	std::cout << std::endl << "finished";
}

template<typename HEAD , typename... TAIL>
void print( const HEAD& head , const TAIL&... tail )
{
	std::cout << std::endl << std::to_string( head ) << " (" << sizeof...(tail) << " parameters left)";

	print( tail... );
}

//Safe printf()!!!! Thanks C++11







//Template specialization:
//////////////////////////

template<typename T>
struct bar;

template<>
struct bar<char>
{

};


template<>
struct bar<float>
{

};




//Partial template specialization:
//////////////////////////////////

template<typename LEFT , typename RIGHT>
struct concat;

template<typename... Ts , typename... Us>
struct concat<typelist<Ts...>,typelist<Us...>> : typelist<Ts...,Us...> {};




template<typename T>
struct is_pointer : public false_type {};

template<typename T>
struct is_pointer<T*> : public true_type {};


template<typename T>
struct remove_const 
{
	using type = T;
	using result = T;
};


template<typename T>
struct remove_const<const T>
{
	using type   = T;
	using result = T;
};


//SFINAE:
/////////

template<bool condition , typename T = void>
struct enable_if;

template<typename T>
struct enable_if<true,T>
{
	using type = T;
};

template<typename T>
struct enable_if<false,T>
{};

//SFINAE example: http://stackoverflow.com/questions/21182729/specializing-single-method-in-a-big-template-class/21183551#21183551

//Otro ejemplo, relacionado con los type traits anteriores: Saber si una clase tiene un tipo miembro llamado "result"
template<typename T>
struct has_result_member_type
{
	using yes = char[1];
	using no  = char[2];

	template<typename U>
	yes test( typename U::result );
	no  test( ... ); //Haha, SFINAE it to here!

	static const bool result = sizeof( test( std::declval<typename T::result>() ) ) == sizeof( yes );
};

// QUEDA MUCHO MÁS, PERO YA ES METERSE EN EL DOMINIO DE LA METAPROGRAMACIÓN, POLICY-BASED-DESSIGNS, DISEÑO BASADO EN COMPONENTES, ETC

