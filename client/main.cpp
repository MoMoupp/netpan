#include <iostream>
#include "logonmain.h"
#include"panmain.h"
using namespace std;

int main(int argc, char const *argv[])
{

    while (1)
    {
        LongonMain main;
        main.run();
        PanMain main2;
        bool falg = main2.run();
        if(!falg){
            break;
        }
    }



    return 0;
}
