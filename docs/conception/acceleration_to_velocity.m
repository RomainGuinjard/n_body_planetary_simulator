function [vxp1, vyp1] = acceleration_to_velocity(accx, accy, vx, vy, dt)
vxp1 = vx + accx*dt;
vyp1 = vy + accy*dt;
end

