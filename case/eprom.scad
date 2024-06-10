$fn=20;

breite = 95;
tiefe = 8;
hoehe = 65;

schiene = 2.5;
platinendicke = 1.8;


wanddicke = 1.4;
edge_radius = wanddicke;

module schienen() {
    cube([schiene, tiefe, schiene]);

    translate([breite-schiene, 0, 0])
    cube([schiene, tiefe, schiene]);

    translate([0, 0, schiene + platinendicke]) {
        cube([schiene, tiefe, schiene]);
        
        translate([breite-schiene, 0, 0])
        cube([schiene, tiefe, schiene]);
    }
}

schraube=3;

module schr() {
    translate([schraube/2, 20, schraube/2])
    rotate([90, 0, 0])
    cylinder(h=20, d=schraube);
}


module auflage() {
    // pcbauflage
    translate([0, 0, 0])
    linear_extrude(hoehe)
    polygon(points=[[0,0], [0, 2.5], [2.5, 0]]);

    translate([breite, 0, hoehe])
    rotate([0, 180, 0])
    linear_extrude(hoehe)
    polygon(points=[[0,0], [0, 2.5], [2.5, 0]]);

    translate([breite, 0, 0])
    rotate([0, -90, 0])
    linear_extrude(breite)
    polygon(points=[[0,0], [0, 2.5], [2.5, 0]]);

    translate([0, 0, hoehe])
    rotate([0, 90, 0])
    linear_extrude(breite)
    polygon(points=[[0,0], [0, 2.5], [2.5, 0]]);
}

difference() {
    union() {
        difference() {
            hull() {
                sphere(r=edge_radius);
                translate([breite, 0, 0]) sphere(r=edge_radius);
                translate([breite, tiefe, 0]) sphere(r=edge_radius);
                translate([0, tiefe, 0]) sphere(r=edge_radius);
                translate([0, 0, hoehe]) {
                    sphere(r=edge_radius);
                    translate([breite, 0, 0]) sphere(r=edge_radius);
                    translate([breite, tiefe, 0]) sphere(r=edge_radius);
                    translate([0, tiefe, 0]) sphere(r=edge_radius);
                }
            }
            union() {
                translate([0, 0-10, 0]) cube([breite, tiefe+10, hoehe]);
                
            }
        }
        auflage();
    }
    translate([1.8, 0, 1]) schr();
    translate([breite-schraube-1.8, 0, 1]) schr();
    translate([1.8, 0, hoehe-schraube-1]) schr();
    translate([breite-schraube-1.8, 0, hoehe-schraube-1]) schr();
}
