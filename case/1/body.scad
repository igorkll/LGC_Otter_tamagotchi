wall = 2;

width = 50 + wall;
height = 80 + wall;
depth = 20 + wall;

speakergrid_offset_border = 25;
speakergrid_offset_x = height - speakergrid_offset_border;
speakergrid_offset_y = width / 2;
speakergrid_offset_z = 0;
speakergrid_size = 20;

usb_offset_width = wall;
usb_offset_height = wall;

// -----------------------------------------------

module speaker_grid(side, depth,
                    spacing    = 2.5,
                    max_r      = 1,
                    edge_ratio = 0.5,
                    margin     = 1) {

    R  = min(side, side)/2 - margin - max_r;
    nx = ceil((side/2) / spacing);
    ny = ceil((side/2) / spacing);

    for (i = [-nx : nx], j = [-ny : ny]) {
        x = i * spacing;
        y = j * spacing;
        d = sqrt(x*x + y*y);
        if (d <= R) {
            r = max_r * (1 - (1 - edge_ratio) * pow(d / R, 2));
            translate([x, y, -depth/2])
                cylinder(h = depth, r = r, $fn = 24);
        }
    }
}

module usb_c_hole(wall, clearance = 0.3) {
    w = 8.3 + 2*clearance;   // ширина
    h = 2.5 + 2*clearance;   // высота

    // Скруглённый прямоугольник (hull из двух цилиндров)
    translate([0, 0, 0])
        rotate([90, 0, 0])
            linear_extrude(height = wall + 2, center = true)
                hull() {
                    translate([-(w/2 - h/2), 0]) circle(r = h/2, $fn = 32);
                    translate([ (w/2 - h/2), 0]) circle(r = h/2, $fn = 32);
                }
}

difference() {
    cube([height, width, depth]);
    
    translate([wall, wall, wall])
        cube([height - (wall * 2), width - (wall * 2), depth]);
    
    translate([speakergrid_offset_x, speakergrid_offset_y, speakergrid_offset_z])
        speaker_grid(speakergrid_size, depth);
    
    translate([height - (wall / 2), usb_offset_width, usb_offset_height])
        rotate([0, 0, 90])
            usb_c_hole(wall);
}