/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: student.c 217 2012-02-28 17:10:06Z spanel $
 */

#include "student.h"
#include "transform.h"

#include <memory.h>


/*****************************************************************************
 * Funkce vytvori vas renderer a nainicializuje jej */

S_Renderer * studrenCreate()
{
    S_StudentRenderer * renderer = (S_StudentRenderer *)malloc(sizeof(S_StudentRenderer));
    IZG_CHECK(renderer, "Cannot allocate enough memory");

    /* inicializace default rendereru */
    renInit(&renderer->base);

    /* nastaveni ukazatelu na vase upravene funkce */
    renderer->base.projectTriangleFunc = studrenProjectTriangle;

    /* nastaveni osvetlovaciho modelu */
    renderer->base.calcReflectanceFunc = studrenPhongReflectance;

    return (S_Renderer *)renderer;
}

/****************************************************************************
 * Funkce vyhodnoti Phonguv osvetlovaci model pro zadany bod
 * a vraci barvu, kterou se bude kreslit do frame bufferu
 * point - souradnice bodu v prostoru
 * normal - normala k povrchu v danem bode */

S_RGBA studrenPhongReflectance(S_Renderer *pRenderer, const S_Coords *point, const S_Coords *normal)
{
    S_Coords    lvec, vvec;
    double      diffuse, specular, r, g, b;
    S_RGBA      color;

    IZG_ASSERT(pRenderer && point && normal);

    /* vektor ke zdroji svetla */
    lvec = makeCoords(pRenderer->light_position.x - point->x,
                      pRenderer->light_position.y - point->y,
                      pRenderer->light_position.z - point->z);
    coordsNormalize(&lvec);

    /* vektor k pozorovateli */
    vvec = makeCoords(-point->x, -point->y, -pRenderer->camera_dist - point->z);
    coordsNormalize(&vvec);

    /* ambientni cast */
    r = pRenderer->light_ambient.red * pRenderer->mat_ambient.red;
    g = pRenderer->light_ambient.green * pRenderer->mat_ambient.green;
    b = pRenderer->light_ambient.blue * pRenderer->mat_ambient.blue;

    /* difuzni cast */
    diffuse = lvec.x * normal->x + lvec.y * normal->y + lvec.z * normal->z;
    if( diffuse > 0 )
    {
        r += diffuse * pRenderer->light_diffuse.red * pRenderer->mat_diffuse.red;
        g += diffuse * pRenderer->light_diffuse.green * pRenderer->mat_diffuse.green;
        b += diffuse * pRenderer->light_diffuse.blue * pRenderer->mat_diffuse.blue;
    }

    /* reflexni cast */
    specular = vvec.x * (normal->x * 2 * diffuse - lvec.x) +
               vvec.y * (normal->y * 2 * diffuse - lvec.y) +
               vvec.z * (normal->z * 2 * diffuse - lvec.z);
    if( specular > 0 )
    {
        r += specular * pRenderer->light_specular.red * pRenderer->mat_specular.red;
        g += specular * pRenderer->light_specular.green * pRenderer->mat_specular.green;
        b += specular * pRenderer->light_specular.blue * pRenderer->mat_specular.blue;
    }

    /* saturace osvetleni*/
    r = MIN(1, r);
    g = MIN(1, g);
    b = MIN(1, b);

    /* kreslici barva */
    color.red = ROUND2BYTE(255 * r);
    color.green = ROUND2BYTE(255 * g);
    color.blue = ROUND2BYTE(255 * b);
    return color;
}

/* Minimum ze tri zadanych hodnot */
#define MIN3(x,y,z) MIN(MIN(x,y),z)
/* Maximum ze tri zadanych hodnot */
#define MAX3(x,y,z) MAX(MAX(x,y),z)

/****************************************************************************
 * Nova fce pro rasterizace trojuhelniku do frame bufferu
 * vcetne interpolace z souradnice a prace se z-bufferem
 * a interpolaci normaly pro Phonguv osvetlovaci model
 * v1, v2, v3 - ukazatele na vrcholy trojuhelniku ve 3D pred projekci
 * n1, n2, n3 - ukazatele na normaly ve vrcholech ve 3D pred projekci
 * x1, y1, ... - vrcholy trojuhelniku po projekci do roviny obrazovky */

void studrenDrawTriangle(S_Renderer *pRenderer,
                         S_Coords *v1, S_Coords *v2, S_Coords *v3,
                         S_Coords *n1, S_Coords *n2, S_Coords *n3,
                         int x1, int y1,
                         int x2, int y2,
                         int x3, int y3
                         )
{
    S_RGBA  color;      /* kreslici barva */

    /* Najde obdelnik ve kterem lezi vykreslovany trojuhelnik*/
    int minX = MAX(MIN3(x1,x2,x3),0);
    int minY = MAX(MIN3(y1,y2,y3),0);
    int maxX = MIN(MAX3(x1,x2,x3),pRenderer->frame_w-1);
    int maxY = MIN(MAX3(y1,y2,y3),pRenderer->frame_h-1);

    /* Smernice jednotlivych hran */
    int dx1 = x2-x1, dx2 = x3-x2, dx3 = x1-x3;
    int dy1 = y2-y1, dy2 = y3-y2, dy3 = y1-y3;

    /* Hranove funkce */
    int hf1 = (minX-x1)*dy1-(minY-y1)*dx1;
    int hf2 = (minX-x2)*dy2-(minY-y2)*dx2;
    int hf3 = (minX-x3)*dy3-(minY-y3)*dx3;

    double alpha, beta, gamma, depth;
    double invDet = 1./(dx3*dy2-dy3*dx2);

    S_Coords v,n;

    int x,y;
    for(x=minX;x<=maxX;++x){
      int e1 = hf1, e2 = hf2, e3 = hf3;
      for(y=minY;y<=maxY;++y){
        if ((e1>=0) &&(e2>=0) && (e3>=0)){
          alpha = ((x-x3)*dy2-(y-y3)*dx2)*invDet;
          beta  = e3*invDet;
          gamma = 1.0 - alpha - beta;
          /* Hloubka pro z-buffer */
          depth = v1->z * alpha + v2->z * beta + v3->z * gamma;

          if(depth<DEPTH(pRenderer,x,y)){
            DEPTH(pRenderer,x,y) = depth;
            /* Interpolace bodu ve 3d */
            v.x = v1->x * alpha + v2->x * beta + v3->x * gamma;
            v.y = v1->y * alpha + v2->y * beta + v3->y * gamma;
            v.z = depth;
            /* Interpolace normaly */
            n.x = n1->x * alpha + n2->x * beta + n3->x * gamma;
            n.y = n1->y * alpha + n2->y * beta + n3->y * gamma;
            n.z = n1->z * alpha + n2->z * beta + n3->z * gamma;

            /* Vyhodnotime osvetlovaci model */
            color = pRenderer->calcReflectanceFunc(pRenderer, &v, &n);
            PIXEL(pRenderer,x,y) = color;
          }
        }
        /* Hodnota hranove funkce pro bod x,y+1 */
        e1-=dx1; e2-=dx2; e3-=dx3;
      }
      /* Hodnota hranove funkce pro bod x+1,y */
      hf1+=dy1; hf2+=dy2; hf3+=dy3;
    }
}

/*****************************************************************************
 * Vykresli i-ty trojuhelnik modelu
 * Pred vykreslenim aplikuje na vrcholy a normaly trojuhelniku
 * aktualne nastavene transformacni matice!
 * i - index trojuhelniku */

void studrenProjectTriangle(S_Renderer *pRenderer, S_Model *pModel, int i)
{
    S_Coords    aa, bb, cc;             /* souradnice vrcholu po transformaci ve 3D pred projekci */
    S_Coords    nn, n1, n2, n3;         /* normala trojuhelniku po transformaci */
    int         u1, v1, u2, v2, u3, v3; /* souradnice vrcholu po projekci do roviny obrazovky */
    S_Triangle  * triangle;

    IZG_ASSERT(pRenderer && pModel && i >= 0 && i < trivecSize(pModel->triangles));

    /* z modelu si vytahneme trojuhelnik */
    triangle = trivecGetPtr(pModel->triangles, i);

    /* transformace vrcholu matici model */
    trTransformVertex(&aa, cvecGetPtr(pModel->vertices, triangle->v[0]));
    trTransformVertex(&bb, cvecGetPtr(pModel->vertices, triangle->v[1]));
    trTransformVertex(&cc, cvecGetPtr(pModel->vertices, triangle->v[2]));

    /* promitneme vrcholy trojuhelniku na obrazovku */
    trProjectVertex(&u1, &v1, &aa);
    trProjectVertex(&u2, &v2, &bb);
    trProjectVertex(&u3, &v3, &cc);

    /* transformace normaly trojuhelniku matici model */
    trTransformVector(&nn, cvecGetPtr(pModel->trinormals, triangle->n));

    /* transformace normal vrcholu matici model */
    trTransformVector(&n1, cvecGetPtr(pModel->normals, triangle->v[0]));
    trTransformVector(&n2, cvecGetPtr(pModel->normals, triangle->v[1]));
    trTransformVector(&n3, cvecGetPtr(pModel->normals, triangle->v[2]));

    /* normalizace normal */
    coordsNormalize(&nn);
    coordsNormalize(&n1);
    coordsNormalize(&n2);
    coordsNormalize(&n3);

    /* je troj. privraceny ke kamere, tudiz viditelny? */
    if( !renCalcVisibility(pRenderer, &aa, &nn) )
    {
        /* odvracene troj. vubec nekreslime */
        return;
    }

    /* rasterizace celeho trojuhelniku */
    studrenDrawTriangle(pRenderer,
                    &aa, &bb, &cc, &n1, &n2, &n3,
                    u1, v1, u2, v2, u3, v3
                    );
}

/*****************************************************************************
 *****************************************************************************/
