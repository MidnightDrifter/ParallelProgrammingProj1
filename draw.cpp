// convert coordinate file (required by assignment) to a picture
#include <fstream>   /* ifstream */
#include <iostream>

int main( int argc, char ** argv ) 
{
    char ** data;
    int x,y;

    std::ifstream file_init_population( argv[1], std::ifstream::in );
    file_init_population >> x >> y;

    data = new char*[y+2];
    for ( int j=0; j<y+2; ++j ) { 
        data[j] = new char[x+2]; 
    }
    for ( int i=0; i<x+2; ++i ) {
        for ( int j=0; j<y+2; ++j ) {
            data[i][j] = 0;
        }
    }

    int i,j;
    while ( !file_init_population.eof() ) {
        file_init_population >> i >> j;
        data[i+1][j+1] = 1;
    }

    for ( int i=1; i<=x; ++i ) {
        for ( int j=1; j<=y; ++j ) {
            std::cout << (data[i][j]%2?"#":".") << " ";
        }
        std::cout << std::endl;
    }

    for ( int j=0; j<y+2; ++j ) { 
        delete [] data[j]; 
    }
    delete [] data;
}
