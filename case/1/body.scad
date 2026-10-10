wall = 2;
wall_up = 1.5;

width = 50.5 + (wall * 2);
height = 62 + wall + wall_up;
depth = 23 + wall;

speakergrid_size = 20;
speakergrid_margin = 1;
speakergrid_offset_height = speakergrid_margin + (speakergrid_size / 2);
speakergrid_offset_width = speakergrid_margin + (speakergrid_size / 2);
speakergrid_offset_x = height - wall_up - speakergrid_offset_height;
speakergrid_offset_y = width - wall - speakergrid_offset_width;
speakergrid_offset_z = 0;

usb_offset_width_1 = wall + 10;
usb_offset_width_2 = wall + 12;
usb_offset_height = depth - 3;

text_size = 3;
text_depth = 0.8;
text_usb_offset = -5;

usb_1_text = "зарядка";
usb_2_text = "сервис";

switch_width = 14.5;
switch_height = 8.5;
switch_offset_bottom = wall + 2;
switch_offset_top = wall_up + 7;

title_depth = 0.8;
title_1 = "Мини выдра ><";
title_2 = "Автор: Logiкусь";
title_1_pos = height / 2;
title_2_pos = (height / 2) - 6;
title_1_size = 4;
title_2_size = 3;

image_pos = (height / 2) - 19;
image_depth = 1;
image_z_offset = -0.1;
image_size_x = 20;
image_size_y = 20;

corner_space = 10;

corner_rack_size = 2.5;
corner_rack_size_y = 4;
corner_rack_height = (depth - wall) - corner_space;

model_flip = false;
only_front_panel = false;
only_back_panel = false;
no_back_panel = true;

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

module back_text(pos, size, textstr) {
    translate([pos, width / 2, -1])
        rotate([0, 0, -90])
            mirror([1,0,0])
                linear_extrude(height = title_depth + 1)
                    text(
                        textstr,
                        size = size,
                        halign = "center",
                        valign = "center",
                        font = "DejaVu Sans:style=Bold"
                    );
}

module draw_image() {
    resize([image_size_y, image_size_x, image_depth])
        surface(file = "bodyimage.png", center = true, convexity = 5);
}

module title() {
    back_text(title_1_pos, title_1_size, title_1);
    back_text(title_2_pos, title_2_size, title_2);
    
    translate([image_pos, width / 2, image_z_offset])
        mirror([1,0,0])
            rotate([0, 0, 90])
                draw_image();
}

module main_exclude() {
    if (only_front_panel) {
        translate([-wall_up, -1, -1])
            cube([height, width + 2, depth + 2]);
    }
    
    if (only_back_panel) {
        translate([-1, -1, wall])
            cube([height + 2, width + 2, depth]);
    }
    
    if (no_back_panel) {
        translate([-1, -1, -1])
            cube([height + 2, width + 2, wall + 1.001]);
    }
}

module main() {
    difference() {
        cube([height, width, depth]);
        
        translate([wall, wall, wall])
            cube([height - wall - wall_up, width - (wall * 2), depth]);
        
        // ------------- exclude
        
        main_exclude();

        // ------------- speaker grid
        
        translate([speakergrid_offset_x, speakergrid_offset_y, speakergrid_offset_z])
            speaker_grid(speakergrid_size, depth);
        
        // ------------- usb
        
        translate([height - (wall_up / 2), usb_offset_width_1, usb_offset_height])
            rotate([0, 0, 90])
                usb_c_hole(wall_up);
                
        translate([height - (wall_up / 2), width - usb_offset_width_2, usb_offset_height])
            rotate([0, 0, 90])
                usb_c_hole(wall_up);
                
        // ------------- usb text
        
        translate([height - text_depth, usb_offset_width_1, usb_offset_height + text_usb_offset])
            rotate([90, 0, 90])
                linear_extrude(height = text_depth + 1)
                    text(
                        usb_1_text,
                        size = text_size,
                        halign = "center",
                        font = "DejaVu Sans:style=Bold"
                    );
                    
        translate([height - text_depth, width - usb_offset_width_2, usb_offset_height + text_usb_offset])
            rotate([90, 0, 90])
                linear_extrude(height = text_depth + 1)
                    text(
                        usb_2_text,
                        size = text_size,
                        halign = "center",
                        font = "DejaVu Sans:style=Bold"
                    );
                    
        // ------------- switch
        
        translate([height - switch_width - switch_offset_top, -(wall / 2), switch_offset_bottom])
            cube([switch_width, wall + 2, switch_height]);
            
        // ------------- title
        
        title();
    }
    
    difference() {
        // ------------- corner rack
        
        union() {
            
            
            translate([wall, wall, wall])
                cube([corner_rack_size_y, corner_rack_size, corner_rack_height]);
            
            translate([height - wall_up - corner_rack_size_y, wall, wall])
                cube([corner_rack_size_y, corner_rack_size, corner_rack_height]);
            
            translate([wall, width - wall - corner_rack_size, wall])
                cube([corner_rack_size_y, corner_rack_size, corner_rack_height]);
            
            translate([height - wall_up - corner_rack_size_y, width - wall - corner_rack_size, wall])
                cube([corner_rack_size_y, corner_rack_size, corner_rack_height]);
        }
        
        // ------------- exclude
        
        main_exclude();
    }
}

if (model_flip) {
    rotate([180, 0, 0])
        main();
} else {
    rotate([0, 0, 0])
        main();
}
    