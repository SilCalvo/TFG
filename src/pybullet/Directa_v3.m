clear all
close all
clc

% --- 1. Variables Simbólicas ---
syms q1 q2 q3 q4 q5 real
syms l1 l2 l3 l4 l5 real

% --- 2. Función para Matriz DH Estándar ---
% T = RotZ(theta) * TransZ(d) * TransX(a) * RotX(alpha)
dh_matrix = @(theta, d, a, alpha) ...
    [cos(theta), -sin(theta)*cos(alpha),  sin(theta)*sin(alpha), a*cos(theta);
     sin(theta),  cos(theta)*cos(alpha), -cos(theta)*sin(alpha), a*sin(theta);
     0,           sin(alpha),             cos(alpha),            d;
     0,           0,                      0,                     1];

% --- 3. Construcción de Matrices (Ajustadas al URDF) ---

% [BASE -> HOMBRO (J2)]
T01 = dh_matrix(q1, l1, 0, -pi/2); 

% [HOMBRO -> CODO]
T12 = dh_matrix(q2 - pi/2, 0, l2, 0);

% [CODO -> ANTEBRAZO]
T23 = dh_matrix(q3, 0, l3, 0);

% [ANTEBRAZO -> MUÑECA 1]
T34 = dh_matrix(q4, 0, l4, 0);

% [MUÑECA 1 -> TCP (MANO)]
T45 = dh_matrix(q5, 0, l5, pi/2);

% Matriz final de traslación pura hasta la punta de la mano (l5)
T_Tool = [1 0 0 0; 0 1 0 0; 0 0 1 l5; 0 0 0 1];

% --- 4. Cálculo Total ---
T_Total = T01 * T12 * T23 * T34 * T45 % * T_Tool;

% --- 5. Validación Numérica  ---
vals_L = {0.4, 0.3, 0.3, 0.1, 0.1}; % l1(base+hombro), l2, l3, l4, l5
vars_L = {l1, l2, l3, l4, l5};

fprintf('--- RESULTADOS ---\n');

% CASO 1: q3 = 90 grados (Codo doblado)
q_test1 = {0, 0, deg2rad(90), 0, 0};
T_num1 = double(subs(T_Total, [vars_L, {q1,q2,q3,q4,q5}], [vals_L, q_test1]));
fprintf('\nCASO 1 (q3=90):\n');
fprintf('Posición X: %.2f (Esperado: ~0.50)\n', T_num1(1,4)); 
fprintf('Posición Y: %.2f (Esperado:  0.00)\n', T_num1(2,4));
fprintf('Posición Z: %.2f (Esperado: ~0.70)\n', T_num1(3,4));

% CASO 2: q2 = 90 grados (Hombro bajado)
q_test2 = {0, deg2rad(90),0, 0, 0};
T_num2 = double(subs(T_Total, [vars_L, {q1,q2,q3,q4,q5}], [vals_L, q_test2]));
fprintf('\nCASO 2 (q2=90):\n');
fprintf('Posición X: %.3f (Esperado: ~0.80)\n', T_num2(1,4)); 
fprintf('Posición Y: %.3f (Esperado:  0.00)\n', T_num2(2,4));
fprintf('Posición Z: %.3f (Esperado: ~0.40)\n', T_num2(3,4));

