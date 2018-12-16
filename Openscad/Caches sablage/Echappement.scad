color("grey")

cylinder(d1=40.6, d2=40.4, h=23.3, center=false, $fn=200);

difference(){

    hull() {
        
        translate([0, 0, -2]) {
            cylinder(d=47,8, h=2, $fn=200);
        }

        translate([0, 27.28, -2]) {
            cylinder(d=12, h=2, $fn=200);
        }

        translate([0, -27.28, -2]) {
            cylinder(d=12, h=2, $fn=200);
        }
    }

    {
    translate([0, -27.28, -2]) {
            cylinder(d=7, h=6, center=true, $fn=200);
        }

    translate([0, 27.28, -2]) {
            cylinder(d=7, h=6, center=true, $fn=200);
        }
    }
}