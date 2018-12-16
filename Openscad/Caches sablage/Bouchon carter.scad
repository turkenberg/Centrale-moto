difference(){
    cylinder(d=15, h=18, center=false, $fn=200);

    translate([0, 0, 2]){
        cylinder(d=12.3, h=18, center=false, $fn=200);
    }
}