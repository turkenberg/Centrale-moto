module corps() {
    cylinder(d=20, h=24.6, center=false, $fn=200);
        color("cyan",0.75)
        translate([0, 0, 24.6]){
            cylinder(d=28, h=2, center=false, $fn=200);
        }
}

difference(){
    corps();
    translate([0, 0, 2]){
        color("red",0.75)
        cylinder(d=16.5, h=28, center=false, $fn=200);
    }
}