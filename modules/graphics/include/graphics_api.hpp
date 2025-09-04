#pragma once

#ifdef RENDY_GRAPHICS_EXPORTS
    #define RENDY_API __declspec(dllexport)
#else
    #define RENDY_API __declspec(dllimport)
#endif