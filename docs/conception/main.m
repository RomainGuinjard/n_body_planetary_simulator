dt = 0.1;
t = 0;

mass1 = 1000000000;
pos1 = [5 0];
velocity1 = [-0.1 0.05];
radius1 = 1;
label1 = "planet";

mass2 = 1000000000;
pos2 = [0 0]; 
velocity2 = [0 0];
radius2 = 3;
label2 = "sun";

pos1_p1 = pos1; velocity1_p1 = velocity1;
pos2_p1 = pos2; velocity2_p1 = velocity2;

figure;
axis([-10 10 -10 10]);
grid on;
hold on;

h1 = plot(0, 0, 'ro', 'MarkerSize', 10, 'MarkerFaceColor', 'r');
h2 = plot(0, 0, 'bo', 'MarkerSize', 10, 'MarkerFaceColor', 'b');

while t<10000 

        [Fx, Fy] = force_1_on_2(mass1, pos1, mass2, pos2);
        Force_ext_x2 = Fx;
        Force_ext_y2 = Fy;

        Force_ext_x1 = -Force_ext_x2;
        Force_ext_y1 = -Force_ext_y2;
       

        [Ax1, Ay1] = force_to_acceleration(Force_ext_x1, Force_ext_y1, mass1) ;
        [Vx1p1, Vy1p1] = acceleration_to_velocity(Ax1, Ay1, velocity1(1), velocity1(2), dt)  ;    
        [X1p1, Y1p1] = velocity_to_position(Vx1p1, Vy1p1, pos1(1), pos1(2), dt);        
        velocity1_p1 = [Vx1p1 Vy1p1];
        pos1_p1 = [X1p1 Y1p1];
        
    

        [Ax2, Ay2] = force_to_acceleration(Force_ext_x2, Force_ext_y2, mass2);
        [Vx2p1, Vy2p1] = acceleration_to_velocity(Ax2, Ay2, velocity2(1), velocity2(2), dt)     ; 
        [X2p1, Y2p1] = velocity_to_position(Vx2p1, Vy2p1, pos2(1), pos2(2), dt);        
        velocity2_p1 = [Vx2p1 Vy2p1];
        pos2_p1 = [X2p1 Y2p1];
    

    set(h1, 'XData', X1p1, 'YData', Y1p1);
    set(h2, 'XData', X2p1, 'YData', Y2p1);
    pause(0.005);
    t = t+dt;

    pos1 = pos1_p1; velocity1 = velocity1_p1;
    pos2 = pos2_p1; velocity2 = velocity2_p1;

end

