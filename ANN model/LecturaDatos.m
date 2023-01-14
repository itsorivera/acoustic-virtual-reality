function [ funcion_interpolada ] = LecturaDatos(x,y,z)
%     close all
%     clear all
%     clc
    
    %x, y ,z son las coordenadas de la posición para la cual quiero interpolar
    %una función
    
%     x=0.35;
%     y=0.35;
%     z=0.35;
    
    l_inf=-1;
    l_sup=1;
    pas=1;
    dife = zeros(1,3);
    
    for a = l_inf: pas: l_sup-pas
       if x>=a && x<= a+pas
           x_lim(1) = a;
           x_lim(2) = a+pas;
           dife(1)=x-x_lim(1);       
       end 
       
       if y>=a && y<= a+pas
           y_lim(1) = a;
           y_lim(2) = a+pas;
           dife(2)=y-y_lim(1);       
       end 
       
       if z>=a && z<= a+pas
           z_lim(1) = a;
           z_lim(2) = a+pas;
           dife(3)=z-z_lim(1);       
       end 
    end
        
    cont=1;
    for c=1:2
        for b=1:2
            for a=1:2
%                 nombreArchivo=sprintf('Data/DatReceptor_%g_%g_%g.txt',x_lim(a),y_lim(b),z_lim(c));
                nombreArchivo=sprintf('Data/DatReceptor_%g_%g_%g.txt',x_lim(a),y_lim(b),z_lim(c));
                fid=fopen(nombreArchivo,'rt');
                FUNCIONES(cont,:)=fscanf(fid,'%15g\n',[1,1000]);
                fclose(fid);
                cont = cont + 1;             
            end
        end    
    end
    
    %%tapa inferior (1:4)
    %variacion en X (1:2) - y estatico
    Yi_1 = FUNCIONES(1,:) * (1-dife(1)) + FUNCIONES(2,:) * (dife(1));
    %variacion en X (3:4) - y estatico
    Yi_2 = FUNCIONES(3,:) * (1-dife(1)) + FUNCIONES(4,:) * (dife(1));
    %funcion inferior 
    Zi = Yi_1 * (1-dife(2)) + Yi_2 * (dife(2));
    
    %%tapa superior (5:8)
    %variacion en x (5:6) - y estatico
    Ys_1 = FUNCIONES(5,:) * (1-dife(1)) + FUNCIONES(6,:) * (dife(1));
    %variacion en x (7:8) - y estatico
    Ys_2 = FUNCIONES(7,:) * (1-dife(1)) + FUNCIONES(8,:) * (dife(1));
    %funcion inferior 
    Zs = Ys_1 * (1-dife(2)) + Ys_2 * (dife(2));
    
%     for m = 1:cont-1
%         subplot(4,2,m);
%         plot(FUNCIONES(m,1:200));
%     end
    
    %%Funcion interpolada
    funcion_interpolada = Zi * (1-dife(3)) + Zs * dife(3);
%     plot(funcion_interpolada)
end 
