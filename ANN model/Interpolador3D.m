close all
clear all 
clc 

%limites en X
lx_inf = -1;
lx_sup = 1;

%limites en Y
ly_inf = -1;
ly_sup = 1;

%limites en Z
lz_inf = -1;
lz_sup = 1;

%numero de puntos en X-Y-Z
npx = 11.0;
npy = 11.0;
npz = 11.0;

%establecemos el paso en X Y Z
pasx = (lx_sup - lx_inf) / npx;
pasy = (ly_sup - ly_inf) / npy;
pasz = (lz_sup - lz_inf) / npz;


cont = 1;
for vx = lx_inf : pasx : lx_sup
    for vy = ly_inf : pasy : ly_sup
        for vz = lz_inf : pasz : lz_sup
            Fn = LecturaDatos(vx, vy, vz);
            INPUT(cont, 1)  = vx;
            INPUT(cont, 2)  = vy;
            INPUT(cont, 3)  = vz;
            TARGET(cont, :) = Fn(1:200);
            cont = cont + 1;
        end
    end
end

INPUT  = INPUT';
TARGET = TARGET';

save('DatosSimAcus','INPUT','TARGET');