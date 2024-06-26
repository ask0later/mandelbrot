#include "mandelbrot.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void Mandelbrot(SDL_Window** window, SDL_Renderer** renderer, SDL_Texture** texture, TTF_Font** timer_font)
{
    SDL_Event event = {};
    Uint8* pixels = NULL;
    xOy_set set = {0, 0, 2.f};

    CtorPixels(&pixels);

    

    char text[MAX_NUM_FPS_DIGITS] = {};

    int control = LOOP;
    while (control == LOOP)
    {
        while (SDL_PollEvent(&event))
        {
            ControlEvent(&event, &set, &control);
        }

        LockTexture(texture, &pixels);

        Uint32 time_1 = SDL_GetTicks();

        CalculatePixelsAVX2(pixels, &set);

        Uint32 time_2 = SDL_GetTicks();

        SDL_UnlockTexture(*texture);
        SDL_RenderCopy(*renderer, *texture, NULL, NULL);
        

        sprintf(text, "FPS: %d", (int) (1000.f / ((float) time_2 - (float) time_1)));
        PrintText(text, renderer, *timer_font);
        
        SDL_RenderPresent(*renderer);
    }

    DtorPixels(pixels);
}

int CalculatePixelsAVX2(Uint8* pixels, xOy_set* set)
{
    float8 max_radius_2 = set_single_float8(MAX_RADIUS_2);
    
    int index_pixel = 0;

    for (int index_y = 0; index_y < HEIGHT; index_y ++)
    {
        float8 y_0 = set_single_float8(((HEIGHT / 2) - index_y + set->offset_y) * dy * set->scale);
                                               // offset on mid height

        for (int index_x = 0; index_x < WIDTH; index_x += 8)
        {  
            int tmp = index_x - (WIDTH / 2) + set->offset_x;
            float8 x_0 = add_float8(
                                       set_single_float8(tmp),
                                       set_each_float8(7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f));
            
            x_0 = mul_float8(x_0, set_single_float8(dx * set->scale));

            float8 x = x_0;
            float8 y = y_0;

            int8 iteration_vector = set_single_int8(0);
            
            int step = 0;

            for (;step < MAX_STEPS; step++)
            {
                float8 x2 = mul_float8(x, x);
                float8 y2 = mul_float8(y, y);
                float8 xy = mul_float8(x, y);
                float8 r2 = add_float8(x2, y2);

                float8 cmp  = cmp_float8(max_radius_2, r2, _CMP_GE_OQ);         // filled with -1 if the condition is met, otherwise 0
                                                                                //          0xFF...F                           0x00...0             

                iteration_vector = sub_int8(iteration_vector, cast_float_to_int(cmp));      // if the condition is met, then plus 1 iteration is added
                                                                                    

                if (!mask_float8(cmp))                                                                              //          0xFF...F                           0x00...0             
                    break;

                x = add_float8(sub_float8(x2, y2), x_0);          // x = x * x - y * y + x_0;
                y = add_float8(add_float8(xy, xy), y_0);          // y = 2 * (x * y) + y_0;
            }


            int* iterations_numbers = (int*) (&iteration_vector);

            for (size_t i = 0; i < 8; i++)
            {
                FillPixel(pixels, &index_pixel, iterations_numbers[i]);
            }
        }
    }

    return 0;
}

int CalculatePixels(Uint8* pixels, xOy_set* set)
{
    int index_pixel = 0;

    for (int index_y = 0; index_y < HEIGHT; index_y++)
    {
        float y_0 = ((HEIGHT / 2) - index_y) * dy * set->scale + set->offset_y * dy;           // offset on mid height

        for (int index_x = 0; index_x < WIDTH; index_x++)
        {
            float x_0 = (index_x - (WIDTH / 2)) * dx * set->scale + set->offset_x * dx;       // offset on mid width

            float x = x_0, y = y_0;
            
            int step = 0;
            for (; (step < MAX_STEPS) && ((x * x) + (y * y) < MAX_RADIUS_2); step++)
            {
                float tmp = x;

                x = x * x - y * y + x_0;
                y = 2 * (tmp * y) + y_0;
            }

            FillPixel(pixels, &index_pixel, step);
        }
    }

    return 0;
}

void FillPixel(Uint8* pixels, int* index_pixel, int iterations_num)
{    
    if (iterations_num < MAX_STEPS)
    {
        float I = sqrtf(sqrtf((float) iterations_num / (float) MAX_STEPS)) * 255.f;
        unsigned char c = (unsigned char) I;

        pixels[*index_pixel] = (Uint8) (255 - c);
        pixels[*index_pixel + 1] = (Uint8) ((c % 5) * 50);
        pixels[*index_pixel + 2] = (Uint8) (c);
        pixels[*index_pixel + 3] = (Uint8) (255);
        (*index_pixel) += 4;
    }
    else 
    {
        pixels[*index_pixel] = 0;
        pixels[*index_pixel + 1] = 0;
        pixels[*index_pixel + 2] = 0;
        pixels[*index_pixel + 3] = 255;  // for exit point
        (*index_pixel) += 4; 
    }

    return;
}

int ControlEvent(SDL_Event* event, xOy_set* set, int* control)
{
    if (event->type != SDL_KEYDOWN && event->type != SDL_KEYUP)
        return 0;

    if (event->key.type != SDL_KEYDOWN)
        return 0;
    

    if (event->type == SDL_QUIT)
    {
        *control = QUIT;
    }

    if (event->key.keysym.mod == KMOD_LCTRL)
    {
        switch(event->key.keysym.scancode) 
        {
            case SDL_SCANCODE_EQUALS:       // scan code +
                set->scale /= (float) SCALE_VALUE;
                set->offset_x *= SCALE_VALUE;
                set->offset_y *= SCALE_VALUE; 
                break;
            case SDL_SCANCODE_MINUS:        // scan code -
                set->scale *= (float) SCALE_VALUE;
                set->offset_x /= SCALE_VALUE;
                set->offset_y /= SCALE_VALUE;
                break;
        }
    }
    else
    {
        switch(event->key.keysym.scancode) 
        {
            case SDL_SCANCODE_ESCAPE:
                *control = QUIT; 
                return 1;
            case SDL_SCANCODE_LEFT:
                (set->offset_x)-= 10;
                break;
            case SDL_SCANCODE_RIGHT:
                (set->offset_x)+= 10;
                break;
            case SDL_SCANCODE_UP:
                (set->offset_y)+= 10;
                break;
            case SDL_SCANCODE_DOWN:
                (set->offset_y)-= 10;
                break;
            default:
                break;
        }   
    }

    return 0;
}

void Init_SDL(SDL_Window** window, SDL_Renderer** renderer, SDL_Texture** texture, TTF_Font** font)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    *window = SDL_CreateWindow("Mandelbrot", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    
    SDL_SetRenderDrawColor (*renderer, 0, 0, 0, 0);
    SDL_RenderClear(*renderer);
    SDL_RenderPresent(*renderer);


    *texture = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    SDL_RenderSetScale(*renderer, 1, 1);

    *font = TTF_OpenFont("a_Albionic.ttf", 25);
    
    return;
}

void CtorPixels(Uint8** pixels)
{   
    *pixels = (Uint8*) calloc(WIDTH * HEIGHT * 4, sizeof(Uint8));

    return;
}

void DtorPixels(Uint8* pixels)
{
    free(pixels);
}

void deInit_SDL(SDL_Window** window, SDL_Renderer** renderer, SDL_Texture** texture, TTF_Font** font)
{
    SDL_DestroyWindow(*window);
    SDL_DestroyRenderer(*renderer);
    SDL_DestroyTexture(*texture);
    TTF_CloseFont(*font);

    TTF_Quit();
    SDL_Quit();
}

void LockTexture(SDL_Texture** texture, Uint8** pixels)
{
    int pitch = WIDTH * 4;

    void** void_pixels = (void**) pixels;
    SDL_LockTexture(*texture, NULL, void_pixels, &pitch);

    return;
}

void PrintText(char text[], SDL_Renderer** renderer, TTF_Font* font)
{
    SDL_Color color = {255, 0, 0, 255};

    SDL_Surface* info = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(*renderer, info);

    SDL_Rect rect = {.x = 0, .y = 0, .w = 100, .h = 40};
    
    SDL_RenderCopy(*renderer, texture, NULL, &rect);

    SDL_FreeSurface(info);
    SDL_DestroyTexture(texture);
}




