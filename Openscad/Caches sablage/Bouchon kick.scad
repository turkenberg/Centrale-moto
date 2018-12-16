module corps() {
    cylinder(d=18, h=24.6, center=false, $fn=200);
        color("cyan")
        translate([0, 0, 24.6]){
            cylinder(d=28, h=2, center=false, $fn=200);
        }
}

color("white")
difference(){
    corps();
    translate([0, 0, 2]){
        color("red")
        cylinder(d=15.5, h=28, center=false, $fn=200);
    }
}