difference(){
    cylinder(d=20, h=4, center=false, $fn=200 );
    translate([0, 0, 4]) {
        cube(size=[2, 20, 4], center=true);
    }
    
}



rotate([180, 0, 0]) {
    cylinder(d1=12.2, d2=11.4, h=8, center=false, $fn=200);
}

