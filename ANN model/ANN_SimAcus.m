clear all 
close all 
clc

load('DatosSimAcus.mat');

% prepoceso los datos en el intervalo [-1, 1]
[inputN, inS]  = mapminmax(INPUT);
[targetN, tgS] = mapminmax(TARGET);

NN1     = 15;           %Numero neuronas capa1
NN2     = 35;           %Numero neuronas capa2
NN3     = 75;           %Numero neuronas capa3
FAo     = 'tansig';     %funcion de activacion capas ocultas
FAs     = 'purelin';    %funcion activacion capa de salida
AlgApr  = 'trainrp';    %algoritmo de aprendizaje

%tipo de red
%Defino la arquitectura
net = newff(inputN, targetN, [NN1 NN2 NN3], {FAo, FAo, FAo, FAs}, AlgApr);

%MODIFICACION DE PARAMTEROS DE ENTRENAMIENTO 

%HOLD OUT
vectorSize = size(INPUT, 2);
trainRatio = 0.80;
valRatio   = 0.10;
testRatio  = 0.10;
[trainInd, valInd, testInd] = dividerand(vectorSize, trainRatio, valRatio, testRatio);

net.divideFcn = 'divideind';
net.divideParam.trainInd = trainInd;
net.divideParam.testInd  = testInd;
net.divideParam.valInd   = valInd;

net.trainParam.show     = 50;
net.trainParam.min_grad = 1e-15;
net.trainParam.lr       = 0.1;      %Tasa de aprendizaje
net.trainParam.epochs   = 1000;     % Numero de epocas
net.trainParam.goal     = 1e-12;    %error minimo aceptable
net.trainParam.max_fail = 150;      %numero de fallas consecutivas de los datos de validacion 

% %Ejecucion red sin entrenar
% output1N = sim(net,INPUT);
% %Postproceso la salida con el factor de preprocesamiento de los targets
% OUTPUT_SE = mapminmax('reverse',output1N,tgS);

%Entrenamiento de la red
[net, tr] = train(net, inputN, targetN);
outputN   = sim(net, inputN);

%pos-procesamiento de la salida con el factor de preprocesamiento de los
%targets
OUTPUT = mapminmax('reverse', outputN, tgS);

save('DatosSimAcus.mat', 'INPUT', 'TARGET', 'OUTPUT');
save('InHoldOut','trainInd', 'valInd', 'testInd');