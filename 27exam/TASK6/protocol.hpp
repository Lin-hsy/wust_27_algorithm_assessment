#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
#include "packets.hpp"
using namespace std;

std::uint16_t crc16(const std::uint8_t* data, std::size_t len);

std::vector<std::uint8_t> encode(const GimbalCmd& cmd);   // 打包成完整的一帧（共 18 字节）

class Decoder 
{
private:
    vector<uint8_t> buf;
public:
    void feed(const std::uint8_t* data, std::size_t len); // 喂入一次串口读取得到的字节，长度任意
    bool next(GimbalState& out);                          // 取出一帧解析好的 GimbalState，没有完整的帧时返回 false
};