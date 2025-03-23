// octacon frontpanel
// alias openscad=~/Applications/OpenSCAD-2021.01-x86_64.AppImage
// openscad -o front.stl front.scad
// f3d front.stl

// panel size
pw=150.0;
pw2=pw/2.0;
pw4=pw/4.0;
ph=100.0;

// display
dw=55.4;
dw2=dw/2.0;
dw4=dw/4.0;
dh=27.78;
dh2=dh/2.0;

// encoder shaft
ew=12;
eh=15;
er=3.5;
em=27;

// leds
lw=14;
lh=4;
lm=40;

// support
sw=4;
sw2=sw/2.0;
sw4=sw/4.0;
esh=6;
esh2=esh/2.0;
eshx=esh+0.2;
lsh=10;
lsh2=lsh/2.0;
lshx=lsh+0.2;
lsg=6;
lsg2=lsg/2.0;

// make it smooth
$fn=50;

// main body
difference() {
    cube([pw,ph,2],center=true);

    // displays
    translate([-pw4,0,0]) { cube([dw,dh,5], center=true); }
    translate([+pw4,0,0]) { cube([dw,dh,5], center=true); }

    // encoders
    // bottom row
    translate([0,-em,0]) { 
        translate([-(pw4-dw4),0,0]) { cylinder(h=5,r=er, center=true); }
        translate([-(pw4+dw4),0,0]) { cylinder(h=5,r=er, center=true); }
        translate([+(pw4-dw4),0,0]) { cylinder(h=5,r=er, center=true); }
        translate([+(pw4+dw4),0,0]) { cylinder(h=5,r=er, center=true); }
    }
    // top row
    translate([0,em,0]) { 
        translate([-(pw4-dw4),0,0]) { cylinder(h=5,r=er, center=true); }
        translate([-(pw4+dw4),0,0]) { cylinder(h=5,r=er, center=true); }
        translate([+(pw4-dw4),0,0]) { cylinder(h=5,r=er, center=true); }
        translate([+(pw4+dw4),0,0]) { cylinder(h=5,r=er, center=true); }
    }

    // leds
    // bottom row
    translate([0,-lm,0]) { 
        translate([-(pw4-dw4),0,0]) { cube([lw,lh,5], center=true); }
        translate([-(pw4+dw4),0,0]) { cube([lw,lh,5], center=true); }
        translate([+(pw4-dw4),0,0]) { cube([lw,lh,5], center=true); }
        translate([+(pw4+dw4),0,0]) { cube([lw,lh,5], center=true); }
    }
    // top row
    translate([0,lm,0]) { 
        translate([-(pw4-dw4),0,0]) { cube([lw,lh,5], center=true); }
        translate([-(pw4+dw4),0,0]) { cube([lw,lh,5], center=true); }
        translate([+(pw4-dw4),0,0]) { cube([lw,lh,5], center=true); }
        translate([+(pw4+dw4),0,0]) { cube([lw,lh,5], center=true); }
    }
}
// encoder supports
module encoder_support() {
    difference() {
        cube([ew+sw,eh+sw,esh], center=true);
        cube([ew,eh,eshx], center=true);
    }
}

translate([0,0,esh2]) {
    // bottom row
    translate([0,-em,0]) {
        translate([-(pw4-dw4),0,0]) { encoder_support(); }
        translate([-(pw4+dw4),0,0]) { encoder_support(); }
        translate([+(pw4-dw4),0,0]) { encoder_support(); }
        translate([+(pw4+dw4),0,0]) { encoder_support(); }
    }
    // top row
    translate([0,em,0]) { 
        translate([-(pw4-dw4),0,0]) { encoder_support(); }
        translate([-(pw4+dw4),0,0]) { encoder_support(); }
        translate([+(pw4-dw4),0,0]) { encoder_support(); }
        translate([+(pw4+dw4),0,0]) { encoder_support(); }
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
    // bottom row
    translate([0,-(lm+lsg2),0]) {
        translate([-(pw4-dw4),0,0]) { led_shield(); }
        translate([-(pw4+dw4),0,0]) { led_shield(); }
        translate([+(pw4-dw4),0,0]) { led_shield(); }
        translate([+(pw4+dw4),0,0]) { led_shield(); }
    }
    // top row
    translate([0,+(lm+lsg2),0]) {
        translate([-(pw4-dw4),0,0]) { led_shield(); }
        translate([-(pw4+dw4),0,0]) { led_shield(); }
        translate([+(pw4-dw4),0,0]) { led_shield(); }
        translate([+(pw4+dw4),0,0]) { led_shield(); }
    }    
}