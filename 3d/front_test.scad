// octacon frontpanel
// alias openscad=~/Applications/OpenSCAD-2021.01-x86_64.AppImage
// openscad -o front_test.stl front_test.scad
// f3d front_test.stl

// panel size
pw=30.0;
pw2=pw/2.0;
pw4=pw/4.0;
ph=38.0;
ph2=ph/2.0;

dw4=pw4; // simplify to keep it centered

// encoder shaft
ew=12;
eh=12;
er=3.7;
em=0;

// leds
lw=14;
lh=4;
lm=11;

// support
sw=4;
sw2=sw/2.0;
sw4=sw/4.0;
esh=5;
esh2=esh/2.0;
eshx=esh+0.2;
lsh=10;
lsh2=lsh/2.0;
lshx=lsh+0.2;
lsg=3;
lsg2=lsg/2.0;

// screw holes
m3r=1.62;

// make it smooth
$fn=50;

// main body
difference() {
    cube([pw,ph,2],center=true);

    // encoders
    // top row
    translate([0,em,0]) { 
        translate([-(pw4-dw4),0,0]) { cylinder(h=5,r=er, center=true); }
    }

    // leds
    // top row
    translate([0,lm,0]) { 
        translate([-(pw4-dw4),0,0]) { cube([lw,lh,5], center=true); }
    }

    // M3 holes
    translate([-(pw2-3),-(ph2-3),0]) { cylinder(h=5,r=m3r, center=true); }
   
}
// encoder supports
module encoder_support() {
    difference() {
        cube([ew+sw2,(eh-0.1),esh], center=true);
        cube([ew,eh,eshx], center=true);
    }
}

translate([0,0,esh2]) {
    // top row
    translate([0,em,0]) { 
        translate([-(pw4-dw4),0,0]) { encoder_support(); }
    }
}
// led shielding
module led_shield() {
    difference() {
        cube([lw+sw,(lh+lsg)+sw,lsh], center=true);
        cube([lw+sw2,(lh+lsg)+sw2,lshx], center=true);
    }
}

translate([0,0,lsh2]) {
    // top row
    translate([0,+(lm+lsg2-1),0]) {
        translate([-(pw4-dw4),0,0]) { led_shield(); }
    }    
}