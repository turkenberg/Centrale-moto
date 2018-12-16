difference(){
    cylinder(d=16, h=18, center=false, $fn=200);

    translate([0, 0, 2]){
        cylinder(d=13, h=18, center=false, $fn=200);
    }
}