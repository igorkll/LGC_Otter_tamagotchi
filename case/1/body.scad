wall = 2;

width = 80 + wall;
height = 50 + wall;
depth = 20 + wall;

speaker_grid_offset_x = 50;
speaker_grid_offset_y = 0;
speaker_grid_offset_z = 0;

speaker_grid_width = 40;
speaker_grid_length = 55;

module speaker_grid(width, length, depth,
                    spacing    = 5,
                    max_r      = 1.6,
                    edge_ratio = 0.45,
                    margin     = 3) {

    R  = min(width, length)/2 - margin - max_r;
    nx = ceil((width /2) / spacing);
    ny = ceil((length/2) / spacing);

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

difference() {
    cube([width, height, depth]);
    translate([wall, wall, wall]) cube([width - (wall * 2), height - (wall * 2), depth]);
    translate([speaker_grid_offset_x, speaker_grid_offset_y, speaker_grid_offset_z]) speaker_grid(speaker_grid_width, speaker_grid_length, depth);
    
}