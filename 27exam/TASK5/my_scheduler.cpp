#include "my_scheduler.hpp"
#include <cmath>
#include <algorithm>
using namespace std;

MyScheduler::MyScheduler()
{
    for(int i=0;i<kElevators;i++)
    {
        current_direction[i]=0;
    }
}

Action MyScheduler::decide(const View& v, int e)
{
    //读信息
    Car car=v.cars[e];
    Car other_car=v.cars[1-e];
    int now_load=car.load;
    int now_time=v.now;
    int now_floor=car.floor;
    bool other_free=(!other_car.busy && other_car.load==0);
    //更新方向
    if(car.doing==Action::Up || car.doing==Action::OpenUp) current_direction[e]=1;
    else if(car.doing==Action::Down || car.doing==Action::OpenDown) current_direction[e]=-1;
    //向上打分
    int score_up=0;

    for(int i=now_floor+1;i<=kFloors;i++)
    {
        int score_base=400;
        int max_wait=0;
        int dist=i-now_floor;      
        int other_dist=abs(i-other_car.floor);
        if(car.car_call[i]==-1 && v.hall_down[i]==-1 &&v.hall_up[i]==-1) continue;
        if(car.car_call[i]==-1 && now_load>=car.capacity) continue;
        if(car.car_call[i]!=-1) score_base=1000;

        if(v.hall_up[i]!=-1) max_wait=max(max_wait,now_time-v.hall_up[i]);
        if(v.hall_down[i]!=-1) max_wait=max(max_wait,now_time-v.hall_down[i]);
        if(other_free && other_dist<dist) max_wait=0;

        int score=score_base+max_wait*10-dist*2;
        if(score>score_up) score_up=score;
    }
    //向下打分
    int score_down=0;
    for(int i=now_floor-1;i>0;i--)
    {
        int score_base=400;
        int max_wait=0;
        int dist=now_floor-i;
        int other_dist=abs(i-other_car.floor);
        if(car.car_call[i]==-1 && v.hall_down[i]==-1 && v.hall_up[i]==-1) continue;
        if(car.car_call[i]==-1 && now_load>=car.capacity) continue;
        if(car.car_call[i]!=-1) score_base=1000;

        if(v.hall_up[i]!=-1) max_wait=max(max_wait,now_time-v.hall_up[i]);
        if(v.hall_down[i]!=-1) max_wait=max(max_wait,now_time-v.hall_down[i]);
        if(other_free && other_dist<dist) max_wait=0;

        int score=score_base+max_wait*10-dist*2;
        if(score>score_down) score_down=score;
    }
    //下一步方向
    if(current_direction[e]==1)
    {
        if(score_up<=0)
        {
            if(score_down>0) current_direction[e]=-1;
            else current_direction[e]=0;
        }
    }
    else if(current_direction[e]==-1)
    {
        if(score_down<=0)
        {
            if(score_up>0) current_direction[e]=1;
            else current_direction[e]=0;
        }
    }
    else
    {
        if(score_up<=0 && score_down<=0) current_direction[e]=0;
        else if(score_up>=score_down ) current_direction[e]=1;
        else current_direction[e]=-1;
    }
    
    //开门
     if(car.car_call[now_floor]!=-1)
     {
         if(current_direction[e]==1) return Action::OpenUp;
         else if(current_direction[e]==-1) return Action::OpenDown;
         else 
         {
             if(current_direction[e]==-1) return Action::OpenDown;
             else return Action::OpenUp;
         }

     }
    
    else
    {
        if(now_load<kCapacity[e])
        {
            if(current_direction[e]!=-1 && v.hall_up[now_floor]!=-1) return Action::OpenUp;
            else if(current_direction[e]!=1 && v.hall_down[now_floor]!=-1) return Action::OpenDown;
        }
    }
    //移动
    if(current_direction[e]==1) return Action::Up;
    else if(current_direction[e]==-1) return Action::Down;
    else
      {
          for(int i=0;i<5;i++)
          {
              if(v.now>kClasses[i].begin-15*60 && v.now<kClasses[i].begin) 
              {
                  if(now_floor>1) return Action::Down;
              }
              if(v.now>kClasses[i].end && v.now<kClasses[i].end+10*60)
              {
                  if(now_floor<6) return Action::Up;
              }
          }

      }
    
    
    return Action::Idle;
}

