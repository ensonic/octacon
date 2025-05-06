// octacon frontpanel
// openscad -o front.stl front.scad
// f3d front.stl

// might be overridden by the Makefile to generate partial object
test=false;
if (test==true) {
    echo("TEST MODE ACTIVE", test);
}

// wall thickness
wd=2.0;

// case size
casew=160.0 + (2 * wd);
casew2=casew/2.0;
caseh=100.0 + (2 * wd);
caseh2=caseh/2.0;
// 4mm for inserts, 16mm from top of panel to FPC mount
cased=20;
cased2=cased/2.0;

// pcb size
pcbw=150.0;
pcbw2=pcbw/2.0;
pcbw4=pcbw/4.0;
pcbh=100.0;
pcbh2=pcbh/2.0;

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

// led/pot positions on pcb
lppx1=17.5;
lppx2=52.5;
ppy=15.35;

// Alpha dual wiper potentiometers
ew=14.4;
eh1=9.7;
eh2=6.7;
eh=eh1+eh2;
eho=eh1-(eh/2.0); // shaft offset to center
er=3.7;  // shaft
em=31.8;   // occupies space from 24.6 .. 39  // TODO: use ppy

// leds
lw=14;
lh=4;
lm=46; // we have ~ 7mm from the top of the led&pots pcb
lsh=9;
lsh2=lsh/2.0;
lsg2=2;

// corners
corr=3;
cord=corr*2;
coff=corr/2;

// m3 screw holes
m3d=3.2; // extra size to cover 3d print thickness
// m4 insert holes
m4d=4.2;

// threaded inserts: outer diameter 5mm, depth 4mm

// make it smooth
$fn=25;

difference() {
    union() {
        difference() {
            // main body
            cube([casew,caseh,cased],center=true);

            // space for inside
            translate([0,0,wd]) { cube([casew - (2.0 * wd), caseh - (2.0 * wd), cased-wd],center=true); }
            
            // rounded corners
            translate([-(casew2-corr),-(caseh2-corr),0]) { round_corner(cased+5, cord, -coff, -coff); }
            translate([+(casew2-corr),-(caseh2-corr),0]) { round_corner(cased+5, cord, +coff, -coff); }
            translate([-(casew2-corr),+(caseh2-corr),0]) { round_corner(cased+5, cord, -coff, +coff); }
            translate([+(casew2-corr),+(caseh2-corr),0]) { round_corner(cased+5, cord, +coff, +coff); }

            // displays
            translate([-pcbw4,  dho,0]) { cube([dw,dh,cased+5], center=true); }
            translate([-pcbw4+5,dho,.5]) { cube([dw,(dh*0.55),cased-1], center=true); }
            translate([+pcbw4,  dho,0]) { cube([dw,dh,cased+5], center=true); }
            translate([+pcbw4+5,dho,.5]) { cube([dw,(dh*0.55),cased-1], center=true); }

            // leds
            // bottom row
            translate([0,-lm,0]) { 
                translate([-pcbw2+lppx2,0.0]) { cube([lw,lh,cased+5], center=true); }
                translate([-pcbw2+lppx1,0.0]) { cube([lw,lh,cased+5], center=true); }
                translate([+pcbw2-lppx2,0.0]) { cube([lw,lh,cased+5], center=true); }
                translate([+pcbw2-lppx1,0.0]) { cube([lw,lh,cased+5], center=true); }
            }
            // top row
            translate([0,lm,0]) { 
                translate([-pcbw2+lppx2,0.0]) { cube([lw,lh,cased+5], center=true); }
                translate([-pcbw2+lppx1,0.0]) { cube([lw,lh,cased+5], center=true); }
                translate([+pcbw2-lppx2,0.0]) { cube([lw,lh,cased+5], center=true); }
                translate([+pcbw2-lppx1,0.0]) { cube([lw,lh,cased+5], center=true); }
            }

            // potentiometers
            // bottom row
            translate([0,-em,0]) { 
                translate([-pcbw2+lppx2,0,0]) { cylinder(h=cased+5,r=er, center=true); }
                translate([-pcbw2+lppx1,0,0]) { cylinder(h=cased+5,r=er, center=true); }
                translate([+pcbw2-lppx2,0,0]) { cylinder(h=cased+5,r=er, center=true); }
                translate([+pcbw2-lppx1,0,0]) { cylinder(h=cased+5,r=er, center=true); }
            }
            // top row
            translate([0,em,0]) { 
                translate([-pcbw2+lppx2,0,0]) { cylinder(h=cased+5,r=er, center=true); }
                translate([-pcbw2+lppx1,0,0]) { cylinder(h=cased+5,r=er, center=true); }
                translate([+pcbw2-lppx2,0,0]) { cylinder(h=cased+5,r=er, center=true); }
                translate([+pcbw2-lppx1,0,0]) { cylinder(h=cased+5,r=er, center=true); }
            }

            // tube-holes for case-inserts
            translate([-(casew2-3.5),-(caseh2-3.5),wd/2]) { insert_tube_hole(cased-wd, m4d); }
            translate([+(casew2-3.5),-(caseh2-3.5),wd/2]) { insert_tube_hole(cased-wd, m4d); }
            translate([-(casew2-3.5),+(caseh2-3.5),wd/2]) { insert_tube_hole(cased-wd, m4d); }
            translate([+(casew2-3.5),+(caseh2-3.5),wd/2]) { insert_tube_hole(cased-wd, m4d); }
        }

        // tubes for case screw inserts
        // 3.5 == (wd + wd) - .5 (thickness of screen wall / 2)
        translate([-(casew2-3.5),-(caseh2-3.5),wd/2]) { insert_tube(cased-wd, m4d); }
        translate([+(casew2-3.5),-(caseh2-3.5),wd/2]) { insert_tube(cased-wd, m4d); }
        translate([-(casew2-3.5),+(caseh2-3.5),wd/2]) { insert_tube(cased-wd, m4d); }
        translate([+(casew2-3.5),+(caseh2-3.5),wd/2]) { insert_tube(cased-wd, m4d); }

        // led boxes
        // bottom row
        translate([0,-(lm+lsg2-1),-(cased2-lsh2)]) {
            translate([-pcbw2+lppx2,0,0]) { led_shield(); }
            translate([-pcbw2+lppx1,0,0]) { led_shield(); }
            translate([+pcbw2-lppx2,0,0]) { led_shield(); }
            translate([+pcbw2-lppx1,0,0]) { led_shield(); }
        }
        // top row
        translate([0,+(lm+lsg2-1),-(cased2-lsh2)]) {
            translate([-pcbw2+lppx2,0,0]) { led_shield(); }
            translate([-pcbw2+lppx1,0,0]) { led_shield(); }
            translate([+pcbw2-lppx2,0,0]) { led_shield(); }
            translate([+pcbw2-lppx1,0,0]) { led_shield(); }
        }
    }

    // in test-mode generate only parts of the case
    translate([(test==true ? 0 : casew*10), 0, 0]) {
        translate([casew2,0,0]) { cube([casew, caseh+10, cased+10], center=true); }
        translate([0,-caseh2,0]) { cube([casew+10, caseh, cased+10], center=true); }
    }
}

// modules

module round_corner(h, d, x, y) {
    r=d/2;
    difference() {
        translate([x,y,0]) { cube([r+0.1,r+0.1,h], center=true); }
        cylinder(h=h+2, d=d, center=true);
    }    
}

module insert_tube(h, d) {
    difference() {
        cylinder(h=h, d=d+1, center=true);
        insert_tube_hole(h, d);
    }
}

module insert_tube_hole(h, d) {
    // 0.2 is to leave some space to melt in the inserts
    translate([0, 0, (h/2.0)-4]) { cylinder(h=4.2, d=d, center=false); }
}


// led shielding
module led_shield() {
    lshw=lw+2.0; // 2 is the size of the led diffusor
    lshh=lh+2.0;
    difference() {
        cube([lshw+wd,lshh+wd,lsh], center=true);
        cube([lshw,   lshh,   lsh+.5], center=true);
    }
}
