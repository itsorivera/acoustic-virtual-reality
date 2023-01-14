//---------------------------------------------------------------------------
#ifndef UnitViewH
#define UnitViewH

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ExtCtrls.hpp"
#include "MathFunctions.h"
#include <Dialogs.hpp>

//---------------------------------------------------------------------------
class TFormView : public TForm
{
__published:	// IDE-managed Components
    TTimer *Timer1;
    TOpenDialog *OpenDialog1;
    TButton *btnCargarCubo;
    TButton *btnCalcular;
    TEdit *edRayos;
    TCheckBox *chbRecorrido;
    TEdit *edTri;
        TCheckBox *chbTriangulos;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall btnCargarCuboClick(TObject *Sender);
    void __fastcall btnCalcularClick(TObject *Sender);
    void __fastcall chbRecorridoClick(TObject *Sender);
    void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift, TPoint &MousePos, bool &Handled);
    void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift, TPoint &MousePos, bool &Handled);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall chbTriangulosClick(TObject *Sender);
private:	// User declarations
    HDC     hdc;
    HGLRC   hrc;
    int     PixelFormat;
public:		// User declarations

    __fastcall TFormView(TComponent* Owner);
    void __fastcall SetPixelFormatDescriptor(void);
    void __fastcall IdleLoop(TObject*, bool&);
    void __fastcall glVertexp(point p);
    void __fastcall glColorc(color c);
    void __fastcall DrawScene(void);
    void __fastcall DrawGrid(void);
    void __fastcall DrawCoordinate(point p,color c);
    void __fastcall DrawAxis(void);
    void __fastcall DrawPlane(plane p, int tmpG);
    void __fastcall DrawSource(source f);
    void __fastcall DrawReceptor(receptor q);
    void __fastcall DrawVector(point p, vector v, bool f, float m, int i, int j);

    bool      Z;                        //Z operation
    int       h,                        //window Height
              w,                        //window Width
              TessTrianglePoint,        //tesselation triangle point counter
              NGroups,                  //Number of groups
              NPoints;                  //Number of points
    double    a,                        //Alpha [horizontal viewer angle]
              b,                        //Beta [vertical viewer angle]
              d,                        //observer Distance
              f,                        //Fov [focus angle]
              g,                        //Grig space
              e;                        //grid Elements
    point     c,                        //Central point
              o;                        //Observer point
    vector    u,                        //Unitary vector [mouse application]
              Up;                       //up vector for OpenGL viewport
    color     Red,                      //red default color
              Blue,                     //blue default color
              Green,                    //green default color
              Yellow,                   //yellow default color
              LightGray,                //lightgray default color
              DarkGray,
              Black;
    int       MouseOperation,
              LastMousePos[2],
              numTri,                   //N�mero de triangulos de la sala
              durSim,                   //Duraci�n de la simulaci�n
              numRec;                   //N�mero de receptores
    double    tiempo,                   //Tiempo para animaci�n
              distancia;                //Distancia recorrida para animaci�n
    bool      cuartoCargado,            //Si se carga la sala
              simularPropagacion,       //Si se realiza la simulaci�n
              verRecorrido,
              verTriangulos;             //Si se desea visualizar los rayos
    room      r;                        //Sala c�bica
    source    s;                        //Fuente sonora
    receptor  r1;                       //Receptor ac�stico / Micr�fono
    vector ** vis_vector;               //Vectores para la visualizaci�n de reflexiones
    double ** vis_modvec;               //Modulo de los vectores para controlar distancia recorrida
    int    ** vis_timacu;               //Tiempo de vuelo acumulado del rayo en cada reflexion
    reflection  *ry;                    //Resultado del trazado de rayos
    matEnergia  mE, mEN;                //matriz para la transicion de energia
    matDouble   mAS;                    //matriz para los angulos solidos
    matInt      mTV;                    //matriz para los tiempos de vuelo

    matDouble   matDistancia;           //profe
    matInt      matTiempoV;             //profe
    matDouble   matAnguloS;             //profe

    arrDouble   aASR;                   //arreglo para los angulos solidos del receptor
    arrInt      aTVR;                   //arreglo para los tiempos de vuelo de caada triangulo hacia el receptor
    double      alfa,delta;


};
//---------------------------------------------------------------------------
extern PACKAGE TFormView *FormView;
//---------------------------------------------------------------------------
#endif
