// octacon frontpanel
// alias openscad=~/Applications/OpenSCAD-2021.01-x86_64.AppImage
// openscad -o front.stl front.scad
// f3d front.stl

// panel size
// - add thickness to w/h so that front-plate covers the sides
pd=2.0;
pw=150.0 + (2 * pd);
pw2=pw/2.0;
pw4=pw/4.0;
ph=100.0 + (2 * pd);
ph2=ph/2.0;

// display box
dw=62.5;
dw2=dw/2.0;
dw4=dw/4.0;
dh=40.5;
dh2=dh/2.0;
// display breakout board 
dbw=71;
dbw2=dbw/2.0;
dbh=44;
dbh2=dbh/2.0;
// account for asymmetric placement of display on breakout board
// dbh-dh = 3.5 : 3mm top, 0.5 mm bottom 
dho=-(3.0/2.0);

// encoder
// EC11 digital encoders
//ew=12;
//ewo=0;
//eh=12;
//em=31;   // occupies space from 25 ... 37
// Alpha dual wiper potentiometers
ew1=9.7;
ew2=6.7;
ew=ew1+ew2;
ewo=ew1-(ew/2.0); // shaft offset to center
eh=14.4;
er=3.7;  // shaft
em=31.8;   // occupies space from 24.6 .. 39

// leds
lw=14;
lh=4;
lm=44;

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

// M3 screw holes
m3r=1.5;
m3rx=1.62; // extra size to cover 3d print thickness
// on the border corners put screw-holes with 1mm offset
m3sb=pd+1+m3r;
// for displays don't add extra offset
m3sd=.5+m3r;

// make it smooth
$fn=25;

difference() {
    // main body
    cube([pw,ph,pd],center=true);

    // displays
    translate([-pw4,dho,0]) { cube([dw,dh,5], center=true); }
    translate([-pw4+5,dho,.5]) { cube([dw,(dh*0.55),1.5], center=true); }
    translate([+pw4,dho,0]) { cube([dw,dh,5], center=true); }
    translate([+pw4+5,dho,.5]) { cube([dw,(dh*0.55),1.5], center=true); }
    // whole breakout
    //translate([-pw4,0,0]) { cube([dbw,dbh,5], center=true); }

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

    // M3 holes for box
    translate([-(pw2-m3sb),-(ph2-m3sb),0]) { cylinder(h=5,r=m3rx, center=true); }
    translate([+(pw2-m3sb),-(ph2-m3sb),0]) { cylinder(h=5,r=m3rx, center=true); }
    translate([-(pw2-m3sb),+(ph2-m3sb),0]) { cylinder(h=5,r=m3rx, center=true); }
    translate([+(pw2-m3sb),+(ph2-m3sb),0]) { cylinder(h=5,r=m3rx, center=true); }

    // M3 holes for displays
    // left
    translate([-(pw4-(dbw2-m3sd)),-(dbh2-m3sd),0]) { cylinder(h=5,r=m3rx, center=true); }
    translate([-(pw4-(dbw2-m3sd)),+(dbh2-m3sd),0]) { cylinder(h=5,r=m3rx, center=true); }
    translate([-(pw4+(dbw2-m3sd)),-(dbh2-m3sd),0]) { cylinder(h=5,r=m3rx, center=true); }
    translate([-(pw4+(dbw2-m3sd)),+(dbh2-m3sd),0]) { cylinder(h=5,r=m3rx, center=true); }
    // right
    translate([+(pw4-(dbw2-m3sd)),-(dbh2-m3sd),0]) { cylinder(h=5,r=m3rx, center=true); }
    translate([+(pw4-(dbw2-m3sd)),+(dbh2-m3sd),0]) { cylinder(h=5,r=m3rx, center=true); }
    translate([+(pw4+(dbw2-m3sd)),-(dbh2-m3sd),0]) { cylinder(h=5,r=m3rx, center=true); }
    translate([+(pw4+(dbw2-m3sd)),+(dbh2-m3sd),0]) { cylinder(h=5,r=m3rx, center=true); }
}

// encoder supports
module encoder_support() {
    difference() {
        cube([ew+sw2,(eh-0.4),esh], center=true);
        cube([ew,eh,eshx], center=true);
    }
}

translate([0,0,esh2]) {
    // bottom row
    translate([0,-em,0]) {
        translate([ewo-(pw4-dw4),0,0]) { encoder_support(); }
        translate([ewo-(pw4+dw4),0,0]) { encoder_support(); }
        translate([ewo+(pw4-dw4),0,0]) { encoder_support(); }
        translate([ewo+(pw4+dw4),0,0]) { encoder_support(); }
    }
    // top row
    translate([0,em,0]) { 
        translate([ewo-(pw4-dw4),0,0]) { encoder_support(); }
        translate([ewo-(pw4+dw4),0,0]) { encoder_support(); }
        translate([ewo+(pw4-dw4),0,0]) { encoder_support(); }
        translate([ewo+(pw4+dw4),0,0]) { encoder_support(); }
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
    translate([0,-(lm+lsg2-1),0]) {
        translate([-(pw4-dw4),0,0]) { led_shield(); }
        translate([-(pw4+dw4),0,0]) { led_shield(); }
        translate([+(pw4-dw4),0,0]) { led_shield(); }
        translate([+(pw4+dw4),0,0]) { led_shield(); }
    }
    // top row
    translate([0,+(lm+lsg2-1),0]) {
        translate([-(pw4-dw4),0,0]) { led_shield(); }
        translate([-(pw4+dw4),0,0]) { led_shield(); }
        translate([+(pw4-dw4),0,0]) { led_shield(); }
        translate([+(pw4+dw4),0,0]) { led_shield(); }
    }    
}