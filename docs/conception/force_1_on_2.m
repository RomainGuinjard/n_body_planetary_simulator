function [Fx, Fy] = force_1_on_2(mass1, pos1, mass2, pos2)
    G = 6.674E-11;
    
    Fx = pos1(1) - pos2(1);
    Fy = pos1(2) - pos2(2);
    
    norm = G*mass1*mass2 / ((Fx^2 + Fy^2)^0.5)^3;
    
    Fx = norm*Fx; Fy = norm*Fy;
end

