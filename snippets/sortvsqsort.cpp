
#include <iostream>
#include <algorithm>
#include <chrono>
#include <vector>
#include <list>
#include <limits>

using container_t = std::vector<int>;

container_t generate_data( std::size_t count )
{
    container_t result;

    std::random_device rndev;
    std::default_random_engine prng( rndev() );
    std::uniform_int_distribution<unsigned char> cdist( std::numeric_limits<int>::min() , std::numeric_limits<int>::max() );

    for( std::size_t i = 0 ; i  < count ; ++i )
        result.push_back( cdist( prng ) );

    return result;
}

int compare_ints(const void* a, const void* b)   // comparison function
{
    typename container_t::value_type arg1 = *reinterpret_cast<const typename container_t::value_type*>(a);
    typename container_t::value_type arg2 = *reinterpret_cast<const typename container_t::value_type*>(b);
    if(arg1 < arg2) return -1;
    if(arg1 > arg2) return 1;
    return 0;
}

int main(int argc, char* argv[])
{
    std::cout << "Generating data... (" << argv[1] << " elements)" << std::endl;

    auto begin = std::chrono::high_resolution_clock::now();
    
    auto a = generate_data( std::atoi( argv[1] ) );
    auto b = a;

    auto end   = std::chrono::high_resolution_clock::now();

    std::cout << "Finished. Total time: " << std::chrono::duration_cast<std::chrono::milliseconds>( end - begin ).count() << "ms" << std::endl;

    std::cout << "Ready. Starting std::sort()... " << std::endl;
    begin = std::chrono::high_resolution_clock::now();

    std::sort( std::begin( a ) , std::end( a ) , std::less<typename container_t::value_type>() );

    end   = std::chrono::high_resolution_clock::now();

    std::cout << "Finished. Total time: " << std::chrono::duration_cast<std::chrono::milliseconds>( end - begin ).count() << "ms" << std::endl;

    std::cout << "Ready. Starting std::qsort()... " << std::endl;
    begin = std::chrono::high_resolution_clock::now();

    std::qsort( &b[0] , b.size() , sizeof(typename container_t::value_type) , compare_ints );

    end   = std::chrono::high_resolution_clock::now();

    std::cout << "Finished. Total time: " << std::chrono::duration_cast<std::chrono::milliseconds>( end - begin ).count() << "ms" << std::endl;

}
