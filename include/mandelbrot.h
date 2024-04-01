#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define float8 __m256
#define int8   __m256i

#define add_float8 _mm256_add_ps
#define sub_float8 _mm256_sub_ps
#define mul_float8 _mm256_mul_ps

#define set_single_float8 _mm256_set1_ps
#define set_each_float8 _mm256_set_ps

#define cmp_float8 _mm256_cmp_ps

#define cast_float_to_int _mm256_castps_si256

#define mask_float8 _mm256_movemask_ps



#define sub_int8 _mm256_sub_epi32
#define set_single_int8 _mm256_set1_epi32

enum ControlEvent
{
    QUIT = 78,
    LOOP = 38 
};

const int WIDTH = 1000;
const int HEIGHT = 800;
const float SCALE_VALUE = 1.1f;

const float dx = 1.f / WIDTH, dy = 1.f / HEIGHT;

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
