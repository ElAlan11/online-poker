#include "playermin.h"

PlayerMin::PlayerMin()
{
    stack = 1000;
    isOut = 0;
    inTheMatch = false;
}

PlayerMin::PlayerMin(int num, string name){
    playerNum = num;
    nickname = name;
    stack = 1000;
    inTheMatch = true;
    isOut = 0;
}
