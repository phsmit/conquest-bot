#include "bot.h"


#include <iostream>
#include <vector>

int main( int argc, const char* argv[] )
{
    std::cout << "No move" << std::endl;

    std::vector<int> v;

    v.push_back(3);
    v.push_back(5);

    for (auto itr = v.begin(); itr != v.end(); ++itr) {
        std::cout << "No move" << std::endl;
    }

}
