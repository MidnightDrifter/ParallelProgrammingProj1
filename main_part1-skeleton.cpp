#include <fstream>   /* ifstream */
#include <cstdio>    /* sscanf */
#include <pthread.h> /* thread stuff */


int main( int argc, char ** argv ) 
{
    if ( argc != 4 ) {
        std::cout << "expected 3 parameters 1) init population file to read 2) number  iterations 3) final population file to write" << std::endl;
        return 1;
    }
    
    int num_iter = 0;
    std::sscanf(argv[2],"%i",&num_iter);

    // input  file argv[1]
    // output file argv[3]

}
