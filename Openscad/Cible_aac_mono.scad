// CORPS
longueur_totale = 28;
diametre_interieur = 11;
epaisseur_corps = 2;
profondeur_clavette = 3;
diametre_clavette = 2;

diametre_exterieur = diametre_interieur + epaisseur_corps;

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

// PORTE-CIBLE
// position de la cible
hauteur_HT_cible = 21; // à partir de la base
angle_clavette_cible = -130; // en degrés
rayon_centre_cible = 15;
// caracteristiques de la cible
diametre_cible = 10;
diametre_cible_interieur = 3;
epaisseur_cible = 2;
// caracteristiques du porte-cible
epaisseur_porte_cible = 1;
rayon_centre_contrepoids = 14;
angle_masse_contrepoids = 70;
profondeur_contrepoids = 12;
diametre_support_contrepoids = diametre_exterieur + epaisseur_porte_cible;
r_int_contrepoids = (rayon_centre_contrepoids - profondeur_contrepoids)/2;

module contrepoids(){ // origine sur l'axe de rotation
    
    //rotate_extrude(angle = angle_masse_contrepoids, convexity = 2)
    partial_rotate_extrude(angle_masse_contrepoids, 15, 2)
    translate([rayon_centre_contrepoids,0,0])
    rotate([0,0,0])
        square([profondeur_contrepoids,epaisseur_porte_cible + epaisseur_cible],center = false);
}

contrepoids();


module plaque(){
    difference(){ 
            union(){
                    hull(){
                        cylinder(h = epaisseur_porte_cible + epaisseur_cible, d = diametre_exterieur + epaisseur_porte_cible, center = false, $fn = 100);
                        translate([rayon_centre_cible,0,0])
                            cylinder(h = epaisseur_porte_cible + epaisseur_cible, d = diametre_cible + epaisseur_porte_cible, center = false, $fn = 100);
                    }
                    hull(){ //hull entre anneau central et corps du contre-poids
                        cylinder(h = epaisseur_porte_cible + epaisseur_cible, d = diametre_exterieur + epaisseur_porte_cible, center = false, $fn = 100);
                        translate([-rayon_centre_contrepoids,0,0]) //Depuis le centre du contre-poids
                            cylinder(h = epaisseur_porte_cible + epaisseur_cible, d = diametre_support_contrepoids, center = false, $fn = 100);
                    }
                    //contrepoids();
                } 
            translate([rayon_centre_cible,0,epaisseur_porte_cible])
                difference(){ // Percer le centre de l'aimant
                    cylinder(h = epaisseur_cible, d = diametre_cible, center = false, $fn = 100);
                    cylinder(h = epaisseur_cible, d = diametre_cible_interieur, center = false, $fn = 100);
                }
    }
}

module cible(){
    difference() {
        rotate([0,0,angle_clavette_cible]) plaque(); //percer la plaque
        cylinder(h = longueur_totale, d = diametre_exterieur, center = false, $fn = 100);
    }
}

// CONTRE-POIDS



// BALANCING UPPER-PLATE
// Recompiler Openscad depuis cette branche (unstaged): https://github.com/blobule/openscad/tree/probe
// then http://forum.openscad.org/Volume-and-Center-of-mass-td15421.html
// Ou bien utilise MeshLabs pour trouver le CoM de la plaque!

// DESSIN

corps();
translate([0,0,hauteur_HT_cible]) cible();

// HELPERS
