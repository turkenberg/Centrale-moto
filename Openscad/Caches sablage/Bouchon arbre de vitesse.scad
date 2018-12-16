module corps() {
    cylinder(d=20, h=44.5, center=false, $fn=200);
        color("cyan",0.75)
        translate([0, 0, 44.5]){
            cylinder(d=25, h=2, center=false, $fn=200);
        }
}

difference(){
    corps();
    translate([0, 0, 2]){
        color("red",0.75)
        cylinder(d=16.15, h=50, center=false, $fn=200);
    }
}