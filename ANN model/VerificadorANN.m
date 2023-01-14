clear all
close all
clc

load("DatosSimAcus.mat");
load("InHoldOut.mat");      %Data 

s0 = size(OUTPUT, 2);       %1728 muestras 
s1 = size(OUTPUT, 1);       %200 caracteristicas
s2 = size(trainInd, 2);     %1382 datos de entrenamiento
s3 = size(testInd, 2);      %173 datos de prueba

for a = 1 : s0
    for b = 1 : s1
        if OUTPUT(b,a) < 0
            OUTPUT(b,a) = 0;
        end
    end
end

x = (1:s1);

%COMENTAR CADA FIGURA PARA OBSERVAR SU DESPLIEGUE AL IGUAL QUE EL ERROR

%% VERIFICACION ENTRENAMIENTO
figure
for a = 1 : s2
    plot(x, TARGET(:, trainInd(a)), 'r', x, OUTPUT(:, trainInd(a)),'b');
    title('Verificación en Entrenamiento')
    xlabel('Caracteristicas [0-200]') 
    ylabel('Intensidad') 
%     pause(0.1);
end

%% VERIFICACION TESTEO

figure
for a = 1 : s3
    plot(x, TARGET(:, testInd(a)), 'r', x, OUTPUT(:, testInd(a)),'b');
    title('Verificación en Testeo')
    xlabel('Caracteristicas [0-200]') 
    ylabel('Intensidad') 
%     pause(0.1);
end

%% MEAN SQUARE ERROR +
for a = 1 : s3
    ERROR        = (TARGET(:, testInd(a)) - OUTPUT(:,testInd(a)).^2);
    MSError      = mse(OUTPUT, TARGET);
    vectError(a) = mean(ERROR);
end


