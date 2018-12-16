color("grey",0.75)

cylinder(d1=40.6, d2=32, h=23.3, center=false, $fn=200);

difference(){

    hull() {
        
        translate([0, 0, -5]) {
            cylinder(d=50, h=5, $fn=200);
        }

        translate([0, 27.28, -5]) {
            cylinder(d=12, h=5, $fn=200);
        }

        translate([0, -27.28, -5]) {
            cylinder(d=12, h=5, $fn=200);
        }
    }

    {
    translate([0, -27.28, -2]) {
            cylinder(d=7, h=10, center=true, $fn=200);
        }

    translate([0, 27.28, -2]) {
            cylinder(d=7, h=10, center=true, $fn=200);
        }
    }
}