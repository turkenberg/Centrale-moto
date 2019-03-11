// CORPS ====================
longueur_totale = 28;
diametre_interieur = 12;
epaisseur_corps = 3;
profondeur_clavette = 3;
diametre_clavette = 3;
diametre_exterieur = diametre_interieur + epaisseur_corps;

// PORTE-CIBLE===============
// Position
hauteur_HT_cible = 20; // à partir de la base
angle_clavette_cible = -50; // en degrés
rayon_centre_cible = 15;
enfoncement_cible = 1;
// Geométrie
diametre_cible = 10;
diametre_cible_interieur = 3;
epaisseur_cible = 2;
epaisseur_cible_pleine = 1;
ratio_cible = 0.7;
// Equilibrage
epaisseur_porte_cible = 1;
rayon_centre_contrepoids = 8;
angle_masse_contrepoids = 95;
profondeur_contrepoids = 15;
diametre_support_contrepoids = diametre_exterieur + epaisseur_porte_cible;
r_int_contrepoids = (rayon_centre_contrepoids - profondeur_contrepoids)/2;
// =========================

// EQUILIBRAGE TOURNANT
//PLAQUE_EN_0();
//CIBLES_EN_0(7); // <-- 23 cibles car: Dneodyne = 7 vs Dabs = 1 + infill @30% => 7*(1/0.30) = facteur 23
//translate([0.423,0,2.9]) color("red") sphere(r=1,center = true, $fn = 32);

// DESSIN ======================================
CORPS();
PLAQUE();
//CIBLE();

module clavette(){
    rotate([0,90,0]) hull() {
        cylinder(h = diametre_exterieur / 2, d = diametre_clavette, center = false, $fn=100);
        translate([-profondeur_clavette,0,0]) cylinder(h = diametre_exterieur / 2, d = diametre_clavette, center = false, $fn=100);
    }
}

module corps() {
    difference() {
        cylinder(h = longueur_totale, d = diametre_exterieur, center = false, $fn = 100);
        cylinder(h = longueur_totale, d = diametre_interieur, center = false, $fn = 100);
        clavette();
    }
}
module contrepoids(){ // origine sur l'axe de rotation
    
    //rotate_extrude(angle = angle_masse_contrepoids, convexity = 2)
    rotate([0,0,-angle_masse_contrepoids/2])
    rotate_extrude2(angle = angle_masse_contrepoids, convexity = 10, size = 1000)
    translate([rayon_centre_contrepoids,0,0])
        square([profondeur_contrepoids,epaisseur_porte_cible + epaisseur_cible + enfoncement_cible],center = false);
}

module plaque(){
    difference(){ 
            union(){
                    hull(){
                        cylinder(h = epaisseur_porte_cible + epaisseur_cible + enfoncement_cible, d = diametre_exterieur + epaisseur_porte_cible, center = false, $fn = 100);
                        translate([rayon_centre_cible,0,0])
                            cylinder(h = epaisseur_porte_cible + epaisseur_cible + enfoncement_cible, d = diametre_exterieur + epaisseur_porte_cible, center = false, $fn = 100);
                    }
                    hull(){ //hull entre anneau central et corps du contre-poids
                        cylinder(h = epaisseur_porte_cible + epaisseur_cible + enfoncement_cible, d = diametre_exterieur + epaisseur_porte_cible, center = false, $fn = 100);
                        translate([-rayon_centre_contrepoids,0,0]) //Depuis le centre du contre-poids
                            cylinder(h = epaisseur_porte_cible + epaisseur_cible + enfoncement_cible, d = diametre_support_contrepoids, center = false, $fn = 100);
                    }
                    rotate([0,0,180])
                        contrepoids();
                }
            //translate([rayon_centre_cible,0, epaisseur_porte_cible ]) cible();
            translate([rayon_centre_cible,0, epaisseur_porte_cible]) 
                difference(){
                    cylinder(h = enfoncement_cible + epaisseur_cible, d = diametre_cible, center = false, $fn = 100);
                    cylinder(h = enfoncement_cible + epaisseur_cible, d = diametre_cible_interieur, center = false, $fn = 100);
                }
    }
}

module plaque_percee(){
    difference() {
        plaque(); //percer la plaque
        cylinder(h = longueur_totale, d = diametre_exterieur, center = false, $fn = 100);
    }
}
module rotate_extrude2(angle=360, convexity=2, size=1000) { // HELPER

  module angle_cut(angle=90,size=1000) {
    x = size*cos(angle/2);
    y = size*sin(angle/2);
    translate([0,0,-size]) 
      linear_extrude(2*size) polygon([[0,0],[x,y],[x,size],[-size,size],[-size,-size],[x,-size],[x,-y]]);
  }

  // support for angle parameter in rotate_extrude was added after release 2015.03 
  // Thingiverse customizer is still on 2015.03
  angleSupport = (version_num() > 20150399) ? true : false; // Next openscad releases after 2015.03.xx will have support angle parameter
  // Using angle parameter when possible provides huge speed boost, avoids a difference operation

  if (angleSupport) {
    rotate_extrude(angle=angle,convexity=convexity, $fn=200)
      children();
  } else {
    rotate([0,0,angle/2]) difference() {
      rotate_extrude(convexity=convexity, $fn=600) children();
      angle_cut(angle, size);
    }
  }
}
module cible(){
                difference(){ // Percer le centre de l'aimant
                    cylinder(h = epaisseur_cible, d = diametre_cible, center = false, $fn = 100);
                    cylinder(h = epaisseur_cible_pleine, d = diametre_cible_interieur, center = false, $fn = 100);
                    translate([0,0,epaisseur_cible_pleine]) cylinder(h = epaisseur_cible - epaisseur_cible_pleine, d1 = diametre_cible_interieur, d2 = diametre_cible_interieur + (diametre_cible - diametre_cible_interieur) * ratio_cible, center = false, $fn = 100);
                }
}


// POSITIONNEMENT
module CORPS(){ // Dessin et positionnement
    color("blue") corps();
}

module PLAQUE(){ // Dessin et positionnement
    //translate([0,0,hauteur_HT_cible+epaisseur_porte_cible+epaisseur_cible+enfoncement_cible])
    translate([0,0,hauteur_HT_cible])  
        rotate([0,0,angle_clavette_cible])
        color("cyan") plaque_percee();
}

module CIBLE(){ // Dessin et positionnement
    rotate([0,0,angle_clavette_cible]) 
    translate([rayon_centre_cible, 0, epaisseur_porte_cible])
    translate([0,0, hauteur_HT_cible])
    color("red") cible();   
}

// EQUILIBRAGE
module PLAQUE_EN_0(){
    color("yellow") plaque_percee();
}

module CIBLES_EN_0(density_int = 7){
    //rotate([0,0,angle_clavette_cible]) 
    //translate([rayon_centre_cible, 0, epaisseur_porte_cible])
    //translate([0,0, hauteur_HT_cible])
    for (i=[1:density_int])
        translate([rayon_centre_cible,0,-i*epaisseur_cible])
        color("pink") cible();
}


