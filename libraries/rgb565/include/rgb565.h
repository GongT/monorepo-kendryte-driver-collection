#pragma once

#include <stdint.h>

__attribute__((always_inline)) inline const uint16_t rgb888_to_565(
    const uint8_t r, const uint8_t g, const uint8_t b)
{
    uint16_t pixel_value = (((uint16_t)(r & 0xF8)) << 8) |
                           (((uint16_t)(g & 0xFC)) << 3) | ((b & 0xF8) >> 3);
    return pixel_value; // (pixel_value >> 8) | (pixel_value << 8);
}

__attribute__((always_inline)) inline const uint8_t alpha_merge_channel(
    const uint8_t color1, const uint8_t color2, const uint8_t color2_alpha)
{
    return (((uint32_t)color2) * color2_alpha) / 255 +
           (((uint32_t)color1) * (255 - color2_alpha)) / 255;
}

__attribute__((always_inline)) inline const uint32_t alpha_merge_rgb888(
    const uint32_t color1, const uint32_t color2, const uint8_t color2_alpha)
{
    return alpha_merge_channel(color1 & 0x00FF0000, color2 & 0x00FF0000, color2_alpha) << 16 |
           alpha_merge_channel(color1 & 0x0000FF00, color2 & 0x0000FF00, color2_alpha) << 8 |
           alpha_merge_channel(color1 & 0x000000FF, color2 & 0x000000FF, color2_alpha);
}
