wall = 2;

width = 50 + wall;
height = 80 + wall;
depth = 20 + wall;

speaker_grid_offset_border = 25;

speaker_grid_offset_x = height - speaker_grid_offset_border;
speaker_grid_offset_y = width / 2;
speaker_grid_offset_z = 0;

speaker_grid_size = 20;

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

difference() {
    cube([height, width, depth]);
    translate([wall, wall, wall]) cube([height - (wall * 2), width - (wall * 2), depth]);
    translate([speaker_grid_offset_x, speaker_grid_offset_y, speaker_grid_offset_z]) speaker_grid(speaker_grid_size, depth);
    
}