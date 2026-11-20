#pragma once
#include "simulator.hpp"

class MyScheduler
{
public:
    MyScheduler();
    Action decide(const View& v, int e);

private:
    int current_direction[kElevators];   // 1 上行，-1 下行，0 不确定
};