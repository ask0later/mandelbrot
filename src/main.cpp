#include "mandelbrot.h"

int main()
{
    SDL_Window*   window     = NULL;
    SDL_Renderer* renderer   = NULL;
    SDL_Texture*  texture    = NULL;
    TTF_Font*     timer_font = NULL;
    
    Init_SDL(&window, &renderer, &texture, &timer_font);

    Mandelbrot(&window, &renderer, &texture, &timer_font);
    
    deInit_SDL(&window, &renderer, &texture, &timer_font);

    return 0;
}