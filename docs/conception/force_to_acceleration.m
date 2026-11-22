function [Ax, Ay] = force_to_acceleration(Fx, Fy, mass)
Ax = Fx/mass;
Ay = Fy/mass;
end

