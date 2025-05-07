// octacon bottom case
// openscad -o back.stl back.scad
// f3d back.stl

// might be overridden by the Makefile to generate partial object
test=false;
if (test==true) {
    echo("TEST MODE ACTIVE", test);
}

// wall thickness
wd=2.0;
wd2=wd/2.0;

// case size
casew=160.0 + (2 * wd);
casew2=casew/2.0;
caseh=100.0 + (2 * wd);
caseh2=caseh/2.0;
// 20 mm screw - 4mm for inserts on the other side
// 15 mm is enough to hold the base pcb with the pico-2
cased=20-4;
cased2=cased/2.0;

// pcb size
pcbw=150.0;
pcbw2=pcbw/2.0;
pcbh=100.0;
pcbh2=pcbh/2.0;

// corners
corr=3;
cord=corr*2;
coff=corr/2;

// m3 screw holes
m3d=3.2; // extra size to cover 3d print thickness

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

            // holes for case screws
            translate([-(casew2-3.5),-(caseh2-3.5),0]) { screw_hole(cased, m3d); }
            translate([+(casew2-3.5),-(caseh2-3.5),0]) { screw_hole(cased, m3d); }
            translate([-(casew2-3.5),+(caseh2-3.5),0]) { screw_hole(cased, m3d); }
            translate([+(casew2-3.5),+(caseh2-3.5),0]) { screw_hole(cased, m3d); }
            
            // holes for base screws
            translate([-(pcbw2-2.5),-(pcbh2-2.5),0]) { screw_hole(cased, m3d); }
            translate([+(pcbw2-2.5),-(pcbh2-2.5),0]) { screw_hole(cased, m3d); }
            translate([-(pcbw2-2.5),+(pcbh2-2.5),0]) { screw_hole(cased, m3d); }
            translate([+(pcbw2-2.5),+(pcbh2-2.5),0]) { screw_hole(cased, m3d); }
            
            // usb hole (in inch grid: ~ 33-37)
            // micro-b plug body: max 11.7 mm * 8.5 mm
            // micro-b plug: 7mm x 3mm
            // on the pico-2 the micro-usb socket extends ~ 2mm beyond the board
            // hence leaving space for the plug should be enough
            // TODO: 15mm would be middle off connector inside pcb from inside of the box
            // TODO: with this height, we need some curout on the front too
            // z: 15mm - (cased2 - wd)
            // x (just need the middle): startpin=33, -1 (to start a zero), +3 (to get middle of the 6 pins)
            translate([-(pcbw2-((33-1)+3)*2.54),+caseh2, 15-(cased2-wd)]) { cube([8.0, wd*4, 4], center=true);}

            // debug header hole (in inch grid: 21-24)
            // 11.0 x 3.0 mm
            // the debug headers extend ~ 1 mm beyond the pcb
            // startpin=21, -1 (to start a zero), +2 (to get middle of the 4 pins)
            translate([-(pcbw2-((21-1)+2)*2.54),+caseh2,-cased2+(wd+1.5)]) { cube([11.0, wd*4, 3.0], center=true);}    
        }

        // tubes for case screws
        // 3.5 == (wd + wd) - .5 (thickness of screen wall / 2)
        translate([-(casew2-3.5),-(caseh2-3.5),wd2]) { screw_tube(cased-wd, m3d); }
        translate([+(casew2-3.5),-(caseh2-3.5),wd2]) { screw_tube(cased-wd, m3d); }
        translate([-(casew2-3.5),+(caseh2-3.5),wd2]) { screw_tube(cased-wd, m3d); }
        translate([+(casew2-3.5),+(caseh2-3.5),wd2]) { screw_tube(cased-wd, m3d); }

        // ridges to fit matching holes on the front
        translate([-(casew2-wd2),0,cased2]) { cube([wd2-0.2,8,8], center=true); }
        translate([+(casew2-wd2),0,cased2]) { cube([wd2-0.2,8,8], center=true); }
        translate([0,-(caseh2-wd2),cased2]) { cube([8,wd2-0.2,8], center=true); }
        translate([0,+(caseh2-wd2),cased2]) { cube([8,wd2-0.2,8], center=true); }
    }

    // in test-mode generate only parts of the case
    translate([(test==true ? 0 : casew*10), 0, 0]) {
        translate([casew2,0,0]) { cube([casew, caseh+10, cased+10], center=true); }
        translate([0,-caseh2,0]) { cube([casew+10, caseh, cased+10], center=true); }
    }
}

module round_corner(h, d, x, y) {
    r=d/2;
    difference() {
        translate([x,y,0]) { cube([r+0.1,r+0.1,h], center=true); }
        cylinder(h=h+2, d=d, center=true);
    }    
}

module screw_tube(h, d) {
    difference() {
        cylinder(h=h, d=d+2, center=true);
        cylinder(h=h+2, d=d, center=true);
    }
}

module screw_hole(h, d) {
    union() {
        cylinder(h=h+5, d=d, center=true);
        translate([0, 0, -(h-wd)/2]) { cylinder(h=wd+.1, d1=d*2, d2=d, center=true); }
    }
}
