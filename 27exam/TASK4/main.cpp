#include <iostream>
#include <cstdint>
#include <algorithm>
#include <type_traits>
using namespace std;

struct CameraFrame
{
    int w,h;
    const unsigned char* buf;
};
struct RosImage
{
    int w,h,step;
    const unsigned char* data;
};
struct VideoFrame
{
    int w,h;
    const unsigned char* yuv;
};

namespace virt
{
    class ImageSource
    {
    public:
        virtual~ImageSource()=default;
        virtual int width() const=0;
        virtual int height() const=0;
        virtual unsigned char at(int x,int y) const=0;
    };
    class CameraSource:public ImageSource
    {
    private:
        CameraFrame f1;
    public: 
        explicit CameraSource(CameraFrame f):f1(f){}
        virtual int width () const override
        {
             return f1.w;  
        }
        virtual int height() const override
        {
            return f1.h;
        }
        virtual unsigned char at(int x,int y) const override
        {
            return f1.buf[y*f1.w+x];
        }

    };
    class RosSource:public ImageSource 
    {
    private:
        RosImage r1;
    public:
        explicit RosSource(RosImage f):r1(f){}
        virtual int width() const override
        {
            return r1.w;
        }
        virtual int height() const override
        {
            return r1.h;
        }
        virtual unsigned char at(int x,int y) const override
        {
            return r1.data[y*r1.step+x];
        }
    };
    class VideoSource:public ImageSource
    {
    private:
        VideoFrame v1;
    public:
        explicit VideoSource(VideoFrame f): v1(f){}
        virtual int width() const override
        {
            return v1.w;
        }
        virtual int height() const override
        {
            return v1.h;
        }
        virtual unsigned char at(int x,int y) const override
        {
            return max((int)v1.yuv[y*v1.w+x]-16,0);
        }
    };
    uint64_t preprocess(const ImageSource& src)
        {
            uint64_t add=0;
            int X=src.width();
            int Y=src.height();
            for(int y=0;y<Y;y++)
            {
                for(int x=0;x<X;x++)
                {
                    add+=src.at(x,y);
                }
            }
            return add;
        }
}

namespace tmpl
{
    template<typename T>
    unsigned char at(const T& Image,int x,int y)
    {
        if constexpr (is_same_v<T,CameraFrame>)
        {
            return Image.buf[y*Image.w+x];
        }
        else if constexpr (is_same_v<T,RosImage>)
        {
            return Image.data[y*Image.step+x];
        }
        else if constexpr (is_same_v<T,VideoFrame>)
        {
            return (unsigned char)max((int)Image.yuv[y*Image.w+x]-16,0);
        }
    }

        template<typename Frame>
        uint64_t preprocess(const Frame& f)
        {
            uint64_t add=0;
            int X=f.w;
            int Y=f.h;
            for(int y=0;y<Y;y++)
            {
                for(int x=0;x<X;x++)
                {
                    add+=at(f,x,y);
                }
            }
            return add;
        }
}
int main()
{
    const int w=4;
    const int h=2;
    const int step=5;
    unsigned char c_buf[w*h]={1,2,3,4,5,6,7,8};
    unsigned char r_data[h*step]={1,2,3,4,0,5,6,7,8,0};
    unsigned char v_yuv[w*h]={1,2,3,4,5,6,7,8};

    CameraFrame c1{w,h,c_buf};

    RosImage r1{w,h,step,r_data};

    VideoFrame v1{w,h,v_yuv};
    //camera测试
    cout<<"camera像素值累加和:"<<endl;
    cout<<"法一：虚函数"<<virt::preprocess(virt::CameraSource(c1))<<endl;
    cout<<"法二：模板"<<tmpl::preprocess(c1)<<endl;
    //ros测试
    cout<<"ros像素值累加和:"<<endl;
    cout<<"法一：虚函数"<<virt::preprocess(virt::RosSource(r1))<<endl;
    cout<<"法二：模板"<<tmpl::preprocess(r1)<<endl;
    //video测试
    cout<<"video像素值累加和:"<<endl;
    cout<<"法一：虚函数"<<virt::preprocess(virt::VideoSource(v1))<<endl;
    cout<<"法二：模板"<<tmpl::preprocess(v1)<<endl;
    return 0;
}
