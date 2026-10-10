$fn = 128;

wall = 2;
wall_up = 1.5;

width = 50.5 + (wall * 2);
height = 62 + wall + wall_up;
thickness = wall;

buttonhole_size = 3;
buttonhole_bottom_offset = 3 + wall;
buttonhole_offsets = [
    wall + 5,
    wall + 15,
    (width - wall) - 5,
    (width - wall) - 15
];

// -----------------------------------------------

difference() {
    cube([height, width, thickness]);
    
    for (buttonhole_offset = buttonhole_offsets) {
        translate([buttonhole_bottom_offset, buttonhole_offset, -0.5])
            cylinder(h = wall + 1, d = buttonhole_size);
    }
}
