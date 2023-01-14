//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <gl\gl.h>
#include <gl\glu.h>
#include <ctype.h>
#include "Classes.h"
#include "MathFunctions.h"
#include "Definitions.h"
#include "UnitView.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtCtrls"
#pragma resource "*.dfm"
TFormView *FormView;

//---------------------------------------------------------------------------
__fastcall TFormView::TFormView(TComponent* Owner) : TForm(Owner) {
    int i,j;
    Application->OnIdle=IdleLoop;
    FormView->Top=0;//ToolBarHeight;
    FormView->Left=0;
    FormView->Height=FormView->ClientHeight;
    FormView->Width=FormView->ClientWidth;

//default colors
    Red.R      =1;
    Red.G      =0.65;
    Red.B      =0.65;;
    Green.R    =0.65;
    Green.G    =1;
    Green.B    =0.65;
    Blue.R     =0.65;
    Blue.G     =0.65;
    Blue.B     =1;
    Yellow.R   =1;
    Yellow.G   =1;
    Yellow.B   =0.4;
    LightGray.R=0.55;
    LightGray.G=0.55;
    LightGray.B=0.55;
    DarkGray.R =0.4;
    DarkGray.G =0.4;
    DarkGray.B =0.4;
//default values
    a=19*PI/60;
    b=13*PI/60;
    d=10;
    f=0.5;      //approximately 30 degrees (or PI*30/180 rad)
    g=0.2;
    e=40;

    c=0;
    w=ClientWidth;
    h=ClientHeight;
    cuartoCargado=false;
    simularPropagacion=false;
    verRecorrido=false;
}

//---------------------------------------------------------------------------
void __fastcall TFormView::FormCreate(TObject *Sender) {
//OpenGL initialize
    hdc=GetDC(Handle);
    SetPixelFormatDescriptor();
    hrc=wglCreateContext(hdc);
    if(hrc==NULL)
        ShowMessage("hrc==NULL");
    if(wglMakeCurrent(hdc,hrc)==false)
        ShowMessage("Could not MakeCurrent");
//OpenGL default parameters
    glClearColor(0.6,0.6,0.6,0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0,1.0);
    glLineWidth(1);
    glPointSize(4);

    a   =19*PI/60;
    b   =13*PI/60;
    d   =10;
    f   =0.5;      //approximately 30 degrees (or PI*30/180 rad)
    g   =0.2;
    e   =40;
    c   =0;
}

//---------------------------------------------------------------------------
void __fastcall TFormView::FormResize(TObject *Sender) {
    //resize OpenGL viewport
    w=ClientWidth;
    h=ClientHeight;
    if(h==0)h=1;
    glViewport(0,0,w,h);
}

//---------------------------------------------------------------------------
void __fastcall TFormView::SetPixelFormatDescriptor() {
//OpenGL initialize
    PIXELFORMATDESCRIPTOR pfd= {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        24,
        0,0,0,0,0,0,
        0,0,
        0,0,0,0,0,
        32,
        0,
        0,
        PFD_MAIN_PLANE,
        0,
        0,0,
    };
    PixelFormat=ChoosePixelFormat(hdc,&pfd);
    SetPixelFormat(hdc,PixelFormat,&pfd);
}

//---------------------------------------------------------------------------
inline void __fastcall TFormView::glVertexp(point p)
{
//draw point
    glVertex3f(p.x,p.y,p.z);
}

//---------------------------------------------------------------------------
inline void __fastcall TFormView::glColorc(color c)
{
//color RGB
    glColor3f(c.R,c.G,c.B);
}

//---------------------------------------------------------------------------
void __fastcall TFormView::IdleLoop(TObject*,bool& done) {
//draw scene
    DrawScene();
}

//---------------------------------------------------------------------------
void __fastcall TFormView::DrawScene(void) {
    int     i,j;
    double  df, //Distance of Front-plane of view
    db; //Distance of Back-plane of view

    //OpenGL parameters
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    db=sqrt(3*e*g*e*g+2*e*g*(abs(c.x)+abs(c.y)+abs(c.z))+c.x*c.x+c.y*c.y+c.z*c.z);
    if(d>db)
        df=db;
    else
        df=0.99*d;

    gluPerspective(180*f/PI,double(w)/double(h),d-df,d+db);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    o.x=d*cos(b)*cos(a)+c.x;
    o.y=d*cos(b)*sin(a)+c.y;
    o.z=d*sin(b)+c.z;
    Up.x=-sin(a);
    Up.y=cos(a);
    Up.z=0;
    Up=Normal(Up/(c-o));

    gluLookAt(o.x,o.y,o.z,c.x,c.y,c.z,Up.x,Up.y,Up.z);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    if(simularPropagacion){
        for (i=0;i<s.NRAYS;i++){
            for (j=0;j<ry[i].N-1;j++){
                vector vg;
                point pnt;
                double ma,ms; //Distancia recorria en reflexion actual y siguiente
                    vg=vis_vector[i][j];
                    ma=vis_modvec[i][j];
                    ms=vis_modvec[i][j+1];
                    pnt=ry[i].r[j];

                    if (distancia>ma && distancia<ms){
                        DrawVector(pnt,vg,true,ma,i,j);
                    }
                    else if (distancia > ms){
                        DrawVector(pnt,vg,false,ma,i,j);
                    }
            }
        }
    }

    //draw room
    if(cuartoCargado){
        int tmpG=0;
        if(simularPropagacion){
            tmpG=floor(1000*distancia/V_SON);
            if(tmpG>durSim){
                tmpG=durSim-1;
            }
        }
        for (i=0;i<r.NP;i++)
            DrawPlane(r.p[i],tmpG);
        DrawSource(s);
        DrawReceptor(r1);
    }

    //draw grid
    //DrawGrid();
    //draw coordinates
    glDisable(GL_DEPTH_TEST);
    DrawAxis();
    glEnable(GL_DEPTH_TEST);
    //swap OpenGL buffers
    SwapBuffers(hdc);
}

//---------------------------------------------------------------------------
inline void __fastcall TFormView::DrawVector(point p, vector v, bool f, float m, int i, int j){
    glColorc(Yellow);

    if(f){//gr�fico din�mico del vector
        double mod, rad;
        v=Normal(v);
        mod=distancia-m;
        v=v*mod;
        rad=0.1*pow((1-r.p[ry[i].Plane[j]].m.alfa)*(1-r.p[ry[i].Plane[j]].m.delta),j); ;
        glPushMatrix();
              glTranslatef(v.x+p.x, v.y+p.y, v.z+p.z);
              gluSphere(gluNewQuadric(), rad, 35, 56);
        glPopMatrix();
    }
    if(verRecorrido){//Recorrido){
        glLineWidth(1);
        glBegin(GL_LINES);
            glVertex3f(p.x,p.y,p.z);
            glVertex3f(v.x+p.x, v.y+p.y, v.z+p.z);
        glEnd();
    }
}

//---------------------------------------------------------------------------
inline void __fastcall TFormView::DrawPlane(plane p, int tmpG) {

    int triID=0;
    color col;
    int i;
    double x;

    for(i=0;i<p.NT;i++){
        col=p.Color;
        triID=p.t[i].ID;

        //dibujar el plano con el color de calor
        //si vas a simular
        if(simularPropagacion){
            col.getHeatMapColor(mEN.energia[triID][tmpG]);

        }


        //inner
        glColorc(col*(1-(p.n*Normal(c-o)*0.2)));
        glBegin(GL_TRIANGLES);
            glVertexp(p.t[i].p0);
            glVertexp(p.t[i].p1);
            glVertexp(p.t[i].p2);
        glEnd();

        //triangle contour
        if(verTriangulos){
        glColorc(col*0.5);
            glLineWidth(1);
  	    glBegin(GL_LINE_LOOP);
            glVertexp(p.t[i].p0);
            glVertexp(p.t[i].p1);
            glVertexp(p.t[i].p2);
      	glEnd();
        }

    }

    //contour of plane
    glColorc(p.Color*0.5);
    glLineWidth(1);
  	glBegin(GL_LINE_LOOP);
        for(i=0;i<p.NP;i++)
            glVertexp(p.p[i]);
  	glEnd();
}

//---------------------------------------------------------------------------
inline void __fastcall TFormView::DrawSource(source f) {

    int i;
    //inner
    glColorc(f.Color);
    glBegin(GL_TRIANGLES);
        for(i=0;i<20;i++){
            glVertexp(f.p + f.IcoFace[i].p0*f.VisualRadius);
            glVertexp(f.p + f.IcoFace[i].p1*f.VisualRadius);
            glVertexp(f.p + f.IcoFace[i].p2*f.VisualRadius);
        }
    glEnd();
//contour
    glColorc(f.Color*0.5);
    glLineWidth(1);
  	for(i=0;i<20;i++){
        glBegin(GL_LINE_LOOP);
            glVertexp(f.p + f.IcoFace[i].p0*f.VisualRadius);
            glVertexp(f.p + f.IcoFace[i].p1*f.VisualRadius);
            glVertexp(f.p + f.IcoFace[i].p2*f.VisualRadius);
     	glEnd();
    }
}

//---------------------------------------------------------------------------
inline void __fastcall TFormView::DrawReceptor(receptor q) {
    glColorc(q.Color);
    glBegin(GL_QUADS);
        for(int i=0;i<32;i++)
            for(int j=0;j<4;j++)
                glVertexp(q.p+(q.SphereFace[i][j]*q.VisualRadius));
    glEnd();

    glColorc(q.Color*0.5);
    //contour of sphere faces
    for(int i=0;i<32;i++){
        glBegin(GL_LINE_LOOP);
            for(int j=0;j<4;j++)
                glVertexp(q.p+(q.SphereFace[i][j]*q.VisualRadius));
  	    glEnd();
    }
}

//---------------------------------------------------------------------------
inline void __fastcall TFormView::DrawGrid(void) {
    double l;
//inner lines
    glColorc(LightGray);
    glBegin(GL_LINES);
    for(l=g; l<e*g-g/2; l+=g) {
        glVertex3f(l,-e*g,0);
        glVertex3f(l,e*g,0);
        glVertex3f(-e*g,l,0);
        glVertex3f(e*g,l,0);
    }
    for(l=-g; l>-e*g+g/2; l-=g) {
        glVertex3f(l,-e*g,0);
        glVertex3f(l,e*g,0);
        glVertex3f(-e*g,l,0);
        glVertex3f(e*g,l,0);
    }
    glEnd();
//contour lines
    glColorc(DarkGray);
    glBegin(GL_LINE_LOOP);
    glVertex3f(e*g,e*g,0);
    glVertex3f(e*g,-e*g,0);
    glVertex3f(-e*g,-e*g,0);
    glVertex3f(-e*g,e*g,0);
    glEnd();
//axis lines
    glBegin(GL_LINES);
    glVertex3f(-e*g,0,0);
    glVertex3f(e*g,0,0);
    glVertex3f(0,-e*g,0);
    glVertex3f(0,e*g,0);
    glVertex3f(0,0,-e*g);
    glVertex3f(0,0,e*g);
    glEnd();
}

//---------------------------------------------------------------------------
inline void __fastcall TFormView::DrawCoordinate(point p,color c) {
//draw coordinates
    glColorc(c);
    glBegin(GL_LINES);
    glVertex3f(p.x,0,0);
    glVertex3f(p.x,p.y,0);
    glVertex3f(0,p.y,0);
    glVertex3f(p.x,p.y,0);
    glVertex3f(p.x,p.y,0);
    glVertex3f(p.x,p.y,p.z);
    glEnd();
}

//---------------------------------------------------------------------------
inline void __fastcall TFormView::DrawAxis(void) {
//x, y and z coloured axis
    glLineWidth(2);
    glColorc(Red);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(0.1*e*g,0,0);
    glEnd();
    glColorc(Green);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(0,0.1*e*g,0);
    glEnd();
    glColorc(Blue);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(0,0,0.1*e*g);
    glEnd();
    glLineWidth(2);
}

//---------------------------------------------------------------------------
void __fastcall TFormView::Timer1Timer(TObject *Sender) {
    //En Definitions.h defino la velocidad SPEED
    tiempo++;
    distancia=SPEED*tiempo;
}

//---------------------------------------------------------------------------
void __fastcall TFormView::chbRecorridoClick(TObject *Sender)
{
    if(verRecorrido){
        verRecorrido=false;
    }
    else{
        verRecorrido=true;
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormView::btnCargarCuboClick(TObject *Sender){
    if (!cuartoCargado){
        cuartoCargado=true;

        //Posici�n del micr�fono
        r1.p=-1.0;
        //Posici�n de la fuente sonora
        s.p=1.5;

        int ndivisiones=edTri->Text.ToInt();
        //Inicializaciones cubo
        r.NewPlanes(6);
        //-------------square 1 back
        r.p[0].NewPoints(4);
        r.p[0].p[0].x=-2.0f;
        r.p[0].p[0].y=2.0f;
        r.p[0].p[0].z=2.0f;
        r.p[0].p[1].x=-2.0f;
        r.p[0].p[1].y=-2.0f;
        r.p[0].p[1].z=2.0f;
        r.p[0].p[2].x=-2.0f;
        r.p[0].p[2].y=-2.0f;
        r.p[0].p[2].z=-2.0f;
        r.p[0].p[3].x=-2.0f;
        r.p[0].p[3].y=2.0f;
        r.p[0].p[3].z=-2.0f;
        r.p[0].MoreTriangle(ndivisiones);//PointGenTriangle();

        //-------------square 2 front
        r.p[1].NewPoints(4);
        r.p[1].p[0].x=2.0f;
        r.p[1].p[0].y=2.0f;
        r.p[1].p[0].z=2.0f;
        r.p[1].p[1].x=2.0f;
        r.p[1].p[1].y=2.0f;
        r.p[1].p[1].z=-2.0f;
        r.p[1].p[2].x=2.0f;
        r.p[1].p[2].y=-2.0f;
        r.p[1].p[2].z=-2.0f;
        r.p[1].p[3].x=2.0f;
        r.p[1].p[3].y=-2.0f;
        r.p[1].p[3].z=2.0f;
        r.p[1].MoreTriangle(ndivisiones);//PointGenTriangle();

        //-------------square 3 left
        r.p[2].NewPoints(4);
        r.p[2].p[0].x=-2.0f;
        r.p[2].p[0].y=-2.0f;
        r.p[2].p[0].z=2.0f;
        r.p[2].p[1].x=2.0f;
        r.p[2].p[1].y=-2.0f;
        r.p[2].p[1].z=2.0f;
        r.p[2].p[2].x=2.0f;
        r.p[2].p[2].y=-2.0f;
        r.p[2].p[2].z=-2.0f;
        r.p[2].p[3].x=-2.0f;
        r.p[2].p[3].y=-2.0f;
        r.p[2].p[3].z=-2.0f;
        r.p[2].MoreTriangle(ndivisiones);//PointGenTriangle();

        //-------------square 4 right
        r.p[3].NewPoints(4);
        r.p[3].p[0].x=2.0f;
        r.p[3].p[0].y=2.0f;
        r.p[3].p[0].z=2.0f;
        r.p[3].p[1].x=-2.0f;
        r.p[3].p[1].y=2.0f;
        r.p[3].p[1].z=2.0f;
        r.p[3].p[2].x=-2.0f;
        r.p[3].p[2].y=2.0f;
        r.p[3].p[2].z=-2.0f;
        r.p[3].p[3].x=2.0f;
        r.p[3].p[3].y=2.0f;
        r.p[3].p[3].z=-2.0f;
        r.p[3].MoreTriangle(ndivisiones);//PointGenTriangle();

        //-------------square 5 top
        r.p[4].NewPoints(4);
        r.p[4].p[0].x=-2.0f;
        r.p[4].p[0].y=-2.0f;
        r.p[4].p[0].z=2.0f;
        r.p[4].p[1].x=-2.0f;
        r.p[4].p[1].y=2.0f;
        r.p[4].p[1].z=2.0f;
        r.p[4].p[2].x=2.0f;
        r.p[4].p[2].y=2.0f;
        r.p[4].p[2].z=2.0f;
        r.p[4].p[3].x=2.0f;
        r.p[4].p[3].y=-2.0f;
        r.p[4].p[3].z=2.0f;
        r.p[4].MoreTriangle(ndivisiones);//PointGenTriangle();

        //-------------square 1 bottom
        r.p[5].NewPoints(4);
        r.p[5].p[0].x=-2.0f;
        r.p[5].p[0].y=2.0f;
        r.p[5].p[0].z=-2.0f;
        r.p[5].p[1].x=-2.0f;
        r.p[5].p[1].y=-2.0f;
        r.p[5].p[1].z=-2.0f;
        r.p[5].p[2].x=2.0f;
        r.p[5].p[2].y=-2.0f;
        r.p[5].p[2].z=-2.0f;
        r.p[5].p[3].x=2.0f;
        r.p[5].p[3].y=2.0f;
        r.p[5].p[3].z=-2.0f;
        r.p[5].MoreTriangle(ndivisiones);//PointGenTriangle();

        //Inicializar normales de los planos
        // se calculan las normales con la normal de su primer triangulo
        // se calcula el baricentro de los tri�ngulos
        int cont_t=0;
        for (int i=0; i<r.NP; i++) {
            r.p[i].n=TriangleNormal(r.p[i].t[0]);
            for (int j=0;j<r.p[i].NT;j++){
                r.p[i].t[j].CalculateProjection();
                r.p[i].t[j].Centroid();
                r.p[i].t[j].ID=cont_t;
                cont_t++;
            }
        }
        numTri=cont_t;
        numRec=1;

        //calcular los angulos solidos de la matriz
        //inizializo la matriz
        mAS.init(numTri, numTri);
        //mTV.clear();
        mTV.init(numTri, numTri);

        //con el profe
        matDistancia.init(numTri, numTri);
        matTiempoV.init(numTri, numTri);
        matAnguloS.init(numTri, numTri);

        //calcular los angulos solidos del receptor
        aASR.init(numTri);
        aTVR.init(numTri);
        
        for(int i=0; i<r.NP; i++){//recorrer todos los planos
            vector normalP = r.p[i].n;
            for(int j=0; j<r.p[i].NT; j++){//recorrer todos los triangulos que tiene ese plano i
                    //con el profe INICIO
                    int triAID=r.p[i].t[j].ID;
                    point pntA=r.p[i].t[j].bc;
                    //con el profe FIN

                    //lleno los arreglos AS y TV para el receptor
                    aASR.d[triAID]=r1.solidAngle(pntA);
                    aTVR.i[triAID]=int(1000*pntA.distancia(r1.p)/V_SON);

                    

               for(int k=0; k<r.NP; k++){
                    
                   for(int l=0; l<r.p[k].NT; l++){
                        
                        //con el profe INICIO
                        int triBID=r.p[k].t[l].ID;
                        point pntB=r.p[k].t[l].bc;

                        //vector vDist=pntA-pntB;
                        if(i!=k){

                            vector vDist = pntB - pntA;
                            double angle = (normalP.Angle(vDist))*(M_PI/180);

                            matDistancia.d[triAID][triBID]=pntA.distancia(pntB);
                            matTiempoV.i[triAID][triBID]=int(1000*pntA.distancia(pntB)/V_SON);
                            matAnguloS.d[triAID][triBID]=r.p[k].t[l].solidAngle(pntA)*fabs(cos(angle));//*sen del angulo sin((M_PI/2)-angle)
                            
                            if(matTiempoV.i[triAID][triBID]==0)
                                matTiempoV.i[triAID][triBID]=1;
                        }
                        //con el profe FIN

                        if(i!=k){

                            int indx, indy;     //indics para la matriz de transicion de energia, donde y tiempo de choque
                            indx = r.p[i].t[j].ID;//me indica el id del triangulo en eje x
                            indy = r.p[k].t[l].ID;//me indica el id del triangulo en eje y

                            //llenando la matriz de angulos solidos
                            //mAS.d[indx][indy] = r.p[i].t[j].solidAngle(r.p[k].t[l].bc);
                            mAS.d[indx][indy] = r.p[k].t[l].solidAngle(r.p[i].t[j].bc);

                            //llenando la matriz de tiempos de vuelo
                            vector vd;

                            double dv;
                            //int tv;

                            vd = r.p[i].t[j].bc - r.p[k].t[l].bc;
                            dv = vd.Module();
                            //t*v=d
                            //t=d/v
                            //tv = int((1000*dv/V_SON));
                            mTV.i[indx][indy] = int((1000*dv/V_SON));

                        }
                        
                   }
               }                
            }
        }
        
        //calculo el porcentaje de energia para cada traingulo
        for (int i=0; i<numTri; i++) {
            //necesito un valro total
            double AT=0.0;
            double ATReceptor=0.0;

            ATReceptor += aASR.d[i];
            
            for (int j=0;j<numTri;j++){
                AT += matAnguloS.d[i][j];
            }

            //otro for para catualizar valores
            for (int j=0;j<numTri;j++){
                //matriz porcentaje
                matAnguloS.d[i][j]/=AT;//ya me queda normalizado, me quedan porcentajes. Lo voy haciendo por 
                
                aASR.d[j]/=ATReceptor;//arreglo actualizado y normalizado como porcentajes
            }
        }





        //grabo los datos de los angulos solidos y tiempos de vuelo calculados
        mAS.grabarArchivo('A', numTri);
        mTV.grabarArchivo('T', numTri);

        //con el profe INICIO 4/2/21
        matDistancia.grabarArchivo('d', numTri);
        matTiempoV.grabarArchivo('t', numTri);
        matAnguloS.grabarArchivo('s', numTri);
        //con el profe FIN

        //grabo arreglos de AS y TV de los triangulos hacia el receptor
        aTVR.grabarArchivo('T', numTri);
        aASR.grabarArchivo('A', numTri);        
        


    }
    else{
        cuartoCargado=false;
        r.Clear();
        numTri=0;
        if (simularPropagacion){
            simularPropagacion=false;
            Timer1->Enabled=false;
            distancia=0.0;
            tiempo=0.0;
            delete ry;
            delete s.Rays;
            delete r1.eR;

            mE.clear();
            mEN.clear();
            mAS.clear();
            mTV.clear();

            matDistancia.clear();
            matTiempoV.clear();
            matAnguloS.clear();
            
            ry=NULL;
            s.Rays=NULL;
            s.NRAYS=0;
            r1.eR=NULL;
            r1.NIt=0;
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormView::btnCalcularClick(TObject *Sender)
{
    if(simularPropagacion==false && cuartoCargado==true){

        double eneRay,eneRes; //Energ�a del rayo y energ�a residual
        durSim=1000; //Duraci�n de la simulaci�n (1 seg o 1000 miliseg)
        r1.createTimeSamples(durSim);
        s.eF=100;
        s.createRays(edRayos->Text.ToInt());
        edRayos->Text=s.NRAYS;

        eneRay=s.eF/s.NRAYS;

        ry=NULL;
        ry=r.RayTracing(s.p,s.Rays,s.NRAYS);

        //voy a generarme unas matrices de calor. 2 matrices porque no quiero que la energia que llegue se pierda
        //haremos que esa energia se mantenga
        mE.init(numTri ,durSim);
        mEN.init(numTri ,durSim);//normalizada, para la visualizacion

        vis_vector = new vector*[s.NRAYS];
        vis_modvec = new double*[s.NRAYS];
        vis_timacu = new int*[s.NRAYS];

        for(int R=0; R<s.NRAYS;R++){

            eneRes=eneRay; //Inicializo energ�a residual

            vis_vector[R] = new vector[ry[R].N-1];
            vis_modvec[R] = new double[ry[R].N];
            vis_timacu[R] = new int[ry[R].N];
            vis_modvec[R][0] = 0.0;
            vis_timacu[R][0] = 0.0;

            for (int i=0; i<ry[R].N-1;i++){
                //Guardo el vector con la ruta de la reflexion
                vis_vector[R][i] = ry[R].r[i+1]-ry[R].r[i];
                //Registro la distancia acumulada de las reflexiones anteriores
                vis_modvec[R][i+1] = vis_modvec[R][i] + ry[R].d[i+1];
                vis_timacu[R][i+1] = int((1000*vis_modvec[R][i+1]/V_SON));
            }

            //TRAZADO DE RAYOS EN DONDE SE ALMACENERA LA ENERGIA EN LA MATRIZ DIFUSA mE
            for (int i=1; i<ry[R].N; i++){

                //necesito las coorde de donde guardar valroes de E
                int indTri, indTim;     //indics para la matriz de transicion de energia, donde y tiempo de choque
                indTri = ry[R].idTriangle[i];//me indica el id en donde se choco
                indTim = vis_timacu[R][i];//tiempo cuando se produce el choque


                alfa=r.p[ry[R].Plane[i]].m.alfa;
                delta=r.p[ry[R].Plane[i]].m.delta;

                //a veeer
                mE.energia[indTri][indTim]+=(eneRes*(1-alfa)*delta);

                //recepcion de energia especular que atraviesan al receptor
                r1.receptionRayTracing(ry[R].r[i-1],vis_vector[R][i-1],vis_timacu[R][i],r.maxd,eneRes);
                eneRes=eneRes*(1-alfa)*(1-delta);//energia que se refleja especularmente
                

                //r1.receptionRayTracing(o, v, tVueloR,)
            }
        }


        //TRANSICION DE ENERGIA con matriz tiempo y triangulos. E difusa durante el t. mE
        //M milisegundis
        for(int M=0; M<durSim; M++){//columna x colum viendo si hay energia
            for(int T1=0; T1<numTri; T1++){//T1 triangulo 1; para ver el triangulo origen de energia



                //transicion de energia a los triangulos
                for(int T2=0; T2<numTri; T2++){//para ver los triangulos destino de la energiaT2
                    //necesitamos t vuelo y % de energia a pasar
                    //el t vuelo, coonsidero tmbn el instante en el que me encuentro (M)
                    int tVuelo = M + matTiempoV.i[T1][T2];
                    if(tVuelo<1000){//no podemos pasarnos del riempo de simulacion
                        //modificamos la mat de energ
                        mE.energia[T2][tVuelo]+=(mE.energia[T1][M]*matAnguloS.d[T1][T2])*(1-alfa);//de
                        //mE.energia[T2][tVuelo]+=mE.energia[T1][M]*(matAnguloS.d[T1][T2]+aASR.d[T1]);//de 
                        //mE.energia[T1][tVueloR]+=mE.energia[T1][M]*aASR.d[T1];//de     
                    }
                }

                int tVueloR = M + aTVR.i[T1];
                if(tVueloR<1000){
                        //r1.eR[tVueloR]+=mE.energia[T1][M]*aASR.d[T1];
                }


                

            }
        }

        


        r1.grabarArchivo(); //Crea un archivo con la respuesta en el receptor
        
        //para que la E no se peirda, hare que se acumule
        for(int i=0; i<mEN.ele; i++){
            for(int j=0; j<mEN.tim; j++){
                mEN.energia[i][j]=mEN.energia[i][j-1] + mE.energia[i][j];//mEnergia acumulativa para visualizacion
            }

        }

        //normalizo la matriz de E con el maximo valor
        mEN.normalizar(mEN.maxEne());

        //creacion de archivos con matriz de energia difusa y matriz para visualizacion
        mE.grabarArchivo('E', s.NRAYS, numTri);
        //la matriz para visualizar
        mEN.grabarArchivo('A', s.NRAYS, numTri);


        distancia=0.0;
        tiempo=0.0;
        simularPropagacion=true;
        Timer1->Enabled=true;
    }
    else if (simularPropagacion==true){
        simularPropagacion=false;
        Timer1->Enabled=false;
        distancia=0.0;
        tiempo=0.0;

        //liberamos variables
        mE.clear();
        mEN.clear();
        mAS.clear();
        mTV.clear();

        matDistancia.clear();
        matTiempoV.clear();
        matAnguloS.clear();

        delete s.Rays;
        delete r1.eR;
        delete ry;
        ry=NULL;
        s.Rays=NULL;
        s.NRAYS=0;
        r1.eR=NULL;
        r1.NIt=0;
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormView::FormMouseMove(TObject *Sender,
        TShiftState Shift, int X, int Y) {
    int dx,dy;
    int k;
    vector n,nplane;
    point i,m,z;
    double v;

    if(Shift.Contains(ssShift))
        v=4;
    else if(Shift.Contains(ssCtrl))
        v=2;
    else
        v=1;

    if(Shift.Contains(ssLeft) ) {
        dx = X-LastMousePos[0];//Negativo mueve a la derecha
        dy = Y-LastMousePos[1];//Negativo mueve a arriba

        //viewer rotation

        if(dx<0)
            a+=v*PI/60;
        else
            a-=v*PI/60;

        if(dy>0) {
            b+=v*PI/60;
        } else {
            b-=v*PI/60;
        }
    }

    LastMousePos[0] = X;
    LastMousePos[1] = Y;
}
//---------------------------------------------------------------------------

void __fastcall TFormView::FormMouseWheelDown(TObject *Sender,
      TShiftState Shift, TPoint &MousePos, bool &Handled)
{
    //viewer zoom
    double v;
    if(Shift.Contains(ssShift)) {
        v=4;
    } else if(Shift.Contains(ssCtrl)) {
        v=8;
    } else {
        v=2;
    }
    if(d<100*v*2*g)
        d+=v*2*g*tan(0.5/(v*2))/tan(f/(v*2));
}
//---------------------------------------------------------------------------

void __fastcall TFormView::FormMouseWheelUp(TObject *Sender,
      TShiftState Shift, TPoint &MousePos, bool &Handled)
{
//viewer zoom
    double v;
    if(Shift.Contains(ssShift)) {
        v=4;
    } else if(Shift.Contains(ssCtrl)) {
        v=8;
    } else {
        v=2;
    }
    if(d>v*2*g)
        d-=v*2*g*tan(0.5/(v*2))/tan(f/(v*2));   //0.5 is the initial fov
}
//---------------------------------------------------------------------------


void __fastcall TFormView::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    ShowMessage("Test");
}
//---------------------------------------------------------------------------

void __fastcall TFormView::chbTriangulosClick(TObject *Sender)
{
       if(chbTriangulos->Checked==true){
                verTriangulos=true;


        }else{
                verTriangulos=false;

        }
}
//---------------------------------------------------------------------------


