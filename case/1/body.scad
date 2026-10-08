wall = 2;

width = 80 + wall;
height = 50 + wall;
depth = 20 + wall;

speaker_grid_offset_x = 0;
speaker_grid_offset_y = 0;
speaker_grid_offset_z = 0;

speaker_grid() {
    
}

difference() {
    cube([width, height, depth]);
    translate([wall, wall, wall]) cube([width - (wall * 2), height - (wall * 2), depth]);
    translate([speaker_grid_offset_x, speaker_grid_offset_y, speaker_grid_offset_z]) speaker_grid();
    
}