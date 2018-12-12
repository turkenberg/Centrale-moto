cylinder(h=20, d=15.1, center=false, $fn=500);

translate([0,0,20]){
        cylinder(h=2, d = 42, center = false, $fn=200);
    }

rotate(a=[0,180,0]) { 
   cylinder(h=3.57, d1=8, d2 = 4.3, center=false, $fn=100);
    }
    
    
    rotate(a=[0,90,0]) 
        translate ([-2.75,0,7]){
            cylinder(h=1, d1=3,d2=2, center=false, $fn=100);
            cylinder(h=2, d=2, center=false, $fn=100);
            //translate (v=[0,0,1]){
            //    sphere(d=2, $fn=100);
            //}
        }    
mirror([1,0,0])
    rotate(a=[0,90,0]) 
        translate (v=[-2.75,0,7]){
            cylinder(h=1, d1=3,d2=2, center=false, $fn=100);
            cylinder(h=2, d=2, center=false, $fn=100);
           // translate (v=[0,0,1]){
           //     sphere(d=2, $fn=100);
           // }
        } 


translate([0,0,22]){
    cylinder(h=4,d=23,center = true, $fn=500);
            rotate_extrude($fn=500){
                   translate([19,0,0]){
                        square([2,4],false);
                    }
            }
        }
