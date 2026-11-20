#include "protocol.hpp"
using namespace std;
#include <vector>
#include <cstring>

constexpr std::uint8_t kSof = 0xA5;              // 帧头
constexpr std::uint8_t kCmdGimbalCmd = 0x01;     // 上位机 → 下位机：GimbalCmd
constexpr std::uint8_t kCmdGimbalState = 0x02;   // 下位机 → 上位机：GimbalState
constexpr std::size_t kMaxDataLen = 64;          // LEN 合法范围 0~64
constexpr std::size_t kHeaderLen = 3;            // SOF(1) + LEN(1) + CMD(1)
constexpr std::size_t kCrcLen = 2;               // 帧尾 CRC16
constexpr std::size_t kCmdDataLen = 13;          // uint32 + float + float + uint8
constexpr std::size_t kStateDataLen = 17;        // uint32 + 3*float + uint8


void put_u32_le(vector<uint8_t> &v,uint32_t val)
{
    v.push_back(val & 0xFFu);
    v.push_back((val>>8) & 0xFFu);
    v.push_back((val>>16) & 0xFFu);
    v.push_back((val>>24) & 0xFFu);
}

void put_f32_le(vector<uint8_t> &v,float f)
{
    uint32_t bits=0;
    memcpy(&bits,&f,sizeof(f));
    put_u32_le(v,bits);
}

uint32_t get_u32_le(const uint8_t*data)
{
    return static_cast<uint32_t>(data[0])
          |(static_cast<uint32_t>(data[1])<<8)
          |(static_cast<uint32_t>(data[2])<<16)
          |(static_cast<uint32_t>(data[3])<<24);
}

float get_f32_le(const uint8_t*data)
{
    uint32_t t=get_u32_le(data);
    float f=0.0;
    memcpy(&f,&t,sizeof(t));
    return f;
}

uint16_t crc16(const uint8_t* data, size_t len) 
{
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; ++i) 
    {
        crc ^= static_cast<uint16_t>(data[i]) << 8;
        for (int b = 0; b < 8; ++b) 
        {
            if (crc & 0x8000) 
            {
                crc = static_cast<uint16_t>((crc << 1) ^ 0x1021);
            } 
            else 
            {
                crc = static_cast<uint16_t>(crc << 1);
            }
        }
    }
    return crc;
}

vector<uint8_t> encode(const GimbalCmd& cmd)
{
    vector<uint8_t> v;
    v.push_back(kSof);
    v.push_back(static_cast<uint8_t>(kCmdDataLen));
    v.push_back(kCmdGimbalCmd);
    put_u32_le(v,cmd.time_ms);
    put_f32_le(v,cmd.yaw);
    put_f32_le(v,cmd.pitch);
    v.push_back(cmd.fire);
    uint16_t crc=crc16(v.data(),kHeaderLen+kCmdDataLen);
    v.push_back(static_cast<uint8_t>(crc));
    v.push_back(static_cast<uint8_t>(crc>>8));
    return v;
}

void Decoder::feed(const uint8_t* data, size_t len)
{
    if(data==nullptr || len==0) return;
    buf.insert(buf.end(),data,data+len);
}

bool Decoder::next(GimbalState& out)
{
    for(;;)
    {
        if(buf.empty()) return false;
        if(buf[0]!=kSof) 
        {
            buf.erase(buf.begin());
            continue;
        }
        if(buf.size()<kHeaderLen) return false;
        if(buf[1]>kMaxDataLen) 
        {
            buf.erase(buf.begin());
            continue;
        }
        size_t n=buf[1];
        size_t total=kHeaderLen+n+kCrcLen;
        if(buf.size()<total) return false;

        uint16_t buf_crc=static_cast<uint16_t>(buf[total-2])
                        |(static_cast<uint16_t>(buf[total-1])<<8);
        uint16_t crc=crc16(buf.data(),total-2);
        if(crc!=buf_crc )
        {
            buf.erase(buf.begin());
            continue;
        }
        bool is_state = (n == kStateDataLen && buf[2] == kCmdGimbalState);
        if(is_state)
        {
            out.time_ms=get_u32_le(buf.data()+kHeaderLen);
            out.yaw=get_f32_le(buf.data()+kHeaderLen+4);
            out.pitch=get_f32_le(buf.data()+kHeaderLen+8);
            out.bullet_speed=get_f32_le(buf.data()+kHeaderLen+12);
            out.enemy_color=buf[kHeaderLen+16];
        }
        buf.erase(buf.begin(),buf.begin()+total);
        if(is_state) return true;
    }
}


