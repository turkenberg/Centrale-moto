module corps() {
    cylinder(d=18, h=44.5, center=false, $fn=200);
        color("cyan")
        translate([0, 0, 44.5]){
            cylinder(d=25, h=2, center=false, $fn=200);
        }
}

color("white")
difference(){
    corps();
    translate([0, 0, 2]){
        color("red")
        cylinder(d=14.15, h=50, center=false, $fn=200);
    }
}