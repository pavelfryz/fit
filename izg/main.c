/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: main.c 219 2012-02-28 17:32:29Z spanel $
 *
 * *) Ovladani programu:
 *      mys     - rotace a zoom sceny
 *      "Esc"   - ukonceni programu
 *      "f"     - prepnuti mezi full screen a window mode (pouze linux)
 *
 * Opravy a modifikace:
 * - 
 */

/******************************************************************************
 * Includes
 */

#ifdef _WIN32
    #include <windows.h>
#endif

/* nase veci... */
#include "render.h"
#include "model.h"
#include "transform.h"
#include "student.h"

/* knihovna SDL */
#include "SDL.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>


/******************************************************************************
 * Prepnuti default rendereru na studentsky
 */

/* je-li makro definovano bude se pouzivat vas studentsky renderer */
/*#define USE_STUDENT_RENDERER*/


/******************************************************************************
 * Globalni konstanty a promenne
 */

/* titulek hlavniho okna */
#ifdef USE_STUDENT_RENDERER
    const char     * PROGRAM_TITLE  = "IZG/2011 Project -- Student";
#else
    const char     * PROGRAM_TITLE  = "IZG/2011 Project";
#endif /* USE_STUDENT_RENDERER */

/* standardni vstupni model */
/*const char         * DEFAULT_MODEL  = "valec.tri";*/
/*const char         * DEFAULT_MODEL  = "koule.tri";*/
const char         * DEFAULT_MODEL  = "bunny.tri";
/*const char         * DEFAULT_MODEL  = "skull.tri";*/

/* defaultni velikost okna */
const int          DEFAULT_WIDTH    = 500;
const int          DEFAULT_HEIGHT   = 500;

/* vykreslovany model */
S_Model             * polymodel     = NULL;

/* nas SW renderer */
S_Renderer          * renderer      = NULL;

/* kreslici buffer knihovny SDL */
SDL_Surface         * screen        = NULL;

/* pomocna promenna pro ukonceni aplikace */
int                 quit            = 0;


/******************************************************************************
 ******************************************************************************
 * funkce pro vyrenderovani nacteneho modelu slozeneho z trojuhelniku
 */

void renderModel()
{
    int         i;
    
    IZG_ASSERT(renderer && polymodel);
    
    /* pomocí rendereru postupne vykreslime vsechny trojuhelniky modelu */
    for( i = 0; i < trivecSize(polymodel->triangles); ++i )
    {
        /* zavolame rutinu pro vykresleni */
        renderer->projectTriangleFunc(renderer, polymodel, i);
    }
}

/******************************************************************************
 * funkce pro vyrenderovani sceny, tj. vykresleni modelu
 */

 void renderScene()
{
    /* test existence frame bufferu a modelu */
    IZG_ASSERT(polymodel && renderer);

    /* nastavit projekcni matici */
    trProjectionPerspective(renderer->camera_dist, renderer->frame_w, renderer->frame_h);

    /* vycistit model matici */
    trLoadIdentity();

    /* nejprve nastavime posuv cele sceny od/ke kamere */
    trTranslate(0.0, 0.0, renderer->scene_move_z);

    /* natoceni cele sceny - jen ve dvou smerech - mys je jen 2D... :( */
    trRotateX(renderer->scene_rot_x);
    trRotateY(renderer->scene_rot_y);

    /* nastavime material */
    renMatAmbient(renderer, &MAT_RED_AMBIENT);
    renMatDiffuse(renderer, &MAT_RED_DIFFUSE);
    renMatSpecular(renderer, &MAT_RED_SPECULAR);

    /* a vykreslime nas model */
    renderModel();
}


/******************************************************************************
 ******************************************************************************
 * funkce zajistujici prekresleni obsahu okna programu
 */

void draw(void)
{
    int     width, height, bpp, x, y;
    Uint8   * dst;
    S_RGBA  * src;

    /* test existence rendereru */
    IZG_ASSERT(renderer && screen);

    /* vymazani frame bufferu */
    renderer->clearBuffersFunc(renderer);

    /* prekreslit scenu!!! */
    renderScene();

    /* velikost bufferu */
    height = MIN(renderer->frame_h, screen->h);
    width = MIN(renderer->frame_w, screen->w);

    /* test typu pixelu */
    bpp = screen->format->BytesPerPixel;
    IZG_ASSERT(bpp == 4);

    /* kopie bufferu do obrazove pameti */
    SDL_LockSurface(screen);
    for( y = 0; y < height; ++y )
    {
        dst = (Uint8 *)screen->pixels + y * screen->pitch;
        src = renderer->frame_buffer + y * renderer->frame_w;
        for( x = 0; x < width; ++x, dst += bpp, ++src )
        {
            *((Uint32 *)dst) = SDL_MapRGB(screen->format, src->red, src->green, src->blue);
        }
    }
    SDL_UnlockSurface(screen);

    /* vymena zobrazovaneho a zapisovaneho bufferu */
    SDL_Flip(screen);
}


/******************************************************************************
 ******************************************************************************
 * funkce reagujici na zmenu rozmeru okna
 * width, height - nova velikost okna
 */

void onResize(SDL_ResizeEvent *resize)
{
    /* test existence rendereru */
    IZG_ASSERT(renderer);

    /* nastaveni noveho frame bufferu v rendereru */
    renderer->createBuffersFunc(renderer, resize->w, resize->h);

    /* zmena velikosti obrazove pameti v SDL knihovne */
    SDL_SetVideoMode(resize->w, resize->h, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
}

/******************************************************************************
 * funkce reagujici na stisknuti klavesnice
 * key - udalost klavesnice
 */

void onKeyboard(SDL_KeyboardEvent *key)
{
    /* test existence rendereru */
    IZG_ASSERT(renderer);

    /* vetveni podle stisknute klavesy */
    switch( key->keysym.sym )
    {
        /* ukonceni programu - klavesa Esc*/
        case SDLK_ESCAPE:
            quit = 1;
            break;

        /* prepnuti do celoobrazovkoveho modu */
        case SDLK_f:
            SDL_WM_ToggleFullScreen(screen);
            break;

        default:
            break;
    }
}

/******************************************************************************
 * funkce reagujici na pohyb mysi
 * mouse - udalost mysi
 */

void onMouseMotion(SDL_MouseMotionEvent *mouse)
{
    if( mouse->state & SDL_BUTTON_LMASK )
    {
        /* rotace cele sceny */
        renSceneRotXY(renderer, mouse->yrel, mouse->xrel);
    }
    else if( mouse->state & SDL_BUTTON_RMASK )
    {
        /* posuv kamery */
        renSceneMoveZ(renderer, mouse->yrel);
    }
}


/******************************************************************************
 ******************************************************************************
 * hlavni funkce programu
 * argc - pocet vstupnich parametru
 * argv - pole vstupnich parametru
 */

int main(int argc, char *argv[])
{
    SDL_Event       event;

    /* nacteni modelu */
    polymodel = modLoad((argc == 2) ? argv[1] : DEFAULT_MODEL);
    IZG_CHECK(polymodel, "Failed to read input model");

    /* vytvoreni SW rendereru */
#ifdef USE_STUDENT_RENDERER
    renderer = studrenCreate();
#else
    renderer = renCreate();
#endif /* USE_STUDENT_RENDERER */

    /* pocatecni velikost bufferu */
    renderer->createBuffersFunc(renderer, DEFAULT_WIDTH, DEFAULT_HEIGHT);

    /* inicializace SDL knihovny */
    if( SDL_Init(SDL_INIT_VIDEO) == -1 )
    {
        IZG_SDL_ERROR("Could not initialize SDL library");
    }

    /* nasteveni okna */
    SDL_WM_SetCaption(PROGRAM_TITLE, 0);

    /* screen */
    screen = SDL_SetVideoMode(DEFAULT_WIDTH, DEFAULT_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);

    /* enable Unicode translation */
    SDL_EnableUNICODE(1);

    /* kreslime dokud nenarazime na SDL_QUIT event */
    while( !quit )
    {
        /* vycteni udalosti */
        while( SDL_PollEvent(&event) )
        {
            switch( event.type )
            {
                /* udalost klavesnice */
                case SDL_KEYDOWN:
                    onKeyboard(&event.key);
                    break;

                /* zmena velikosti okna */
                case SDL_VIDEORESIZE:
                    onResize(&event.resize);
                    break;

                /* udalost mysi */
                case SDL_MOUSEMOTION:
                    onMouseMotion(&event.motion);
                    break;

                /* SDL_QUIT event */
                case SDL_QUIT:
                    quit = 1;
                    break;

                default:
                    break;
            }
        }

        /* vykresleni pres SDL knihovnu */
        draw();
    }

    /* ukonceni SDL */
    SDL_FreeSurface(screen);

    /* shutdown all SDL subsystems */
    SDL_Quit();

    /* zrusime co jsme vytvorili a ukoncime program */
    modRelease(&polymodel);
    renderer->releaseFunc(&renderer);

    return 0;
}


/*****************************************************************************/
/*****************************************************************************/
