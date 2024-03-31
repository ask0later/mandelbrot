#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

enum ControlEvent
{
    QUIT = 78,
    LOOP = 38 
};

const int WIDTH = 1000;
const int HEIGHT = 800;
const float SCALE_VALUE = 1.1f;
const float dx = 1.f / (float) WIDTH, dy = 1.f / (float) HEIGHT;
const int MAX_STEPS = 256;
const int MAX_NUM_FPS_DIGITS = 10;

const float MAX_RADIUS_2 = 100.f;


struct xOy_set
{
    int     offset_x;
    int     offset_y;
    float   scale;
};


void Mandelbrot(SDL_Window** window, SDL_Renderer** renderer, SDL_Texture** texture, TTF_Font** timer_font);

int CalculatePixels(Uint8* pixels, xOy_set* set);
int CalculatePixelsAVX2(Uint8* pixels, xOy_set* set);
int ControlEvent(SDL_Event* event, xOy_set* set, int* control);


void Init_SDL(SDL_Window** window, SDL_Renderer** renderer, SDL_Texture** texture, TTF_Font** font);
void deInit_SDL(SDL_Window** window, SDL_Renderer** renderer, SDL_Texture** texture, TTF_Font** font);

void CtorPixels(Uint8** pixels);
void DtorPixels(Uint8* pixels);
void FillPixel(Uint8* pixels, int* index_pixel, int iterations_num);

void PrintText(char text[], SDL_Renderer** renderer, TTF_Font* font);
void LockTexture(SDL_Texture** texture, Uint8** pixels);
