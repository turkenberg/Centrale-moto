//$fn=100

color("cyan",0.75)
    translate([1.5, 0, 0]) {
        cylinder(h=2,d=28.5, $fn=200);
    }
    

color("grey",0.75)
difference(){
    hull() {
        
        translate([0, 0, -2]) {
            cylinder(d=47,8, h=2, $fn=200);
        }

        translate([0, 32.8, -2]) {
            cylinder(d=18, h=2, $fn=200);
        }

        translate([0, -32.8, -2]) {
            cylinder(d=18, h=2, $fn=200);
        }
    }

    {
    translate([0, -32.8, -2]) {
            cylinder(d=7.8, h=6, center=true, $fn=200);
        }

    translate([0, 32.8, -2]) {
            cylinder(d=7.8, h=6, center=true, $fn=200);
        }
    }
}