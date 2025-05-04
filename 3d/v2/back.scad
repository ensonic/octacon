// octacon bottom case
// alias openscad=~/Applications/OpenSCAD-2021.01-x86_64.AppImage
// openscad -o back.stl back.scad
// f3d back.stl

// wall thickness
wd=2.0;

// case size
casew=160.0 + (2 * wd);
casew2=casew/2.0;
caseh=100.0 + (2 * wd);
caseh2=caseh/2.0;
// 20 mm screw - 4mm for inserts on the other side
// 15 mm is enough to hold the base pcb with the pico-2
cased=20-4;

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
    
    // usb hole
    
    // debug header hole
}

// tubes for case screws
// 3.5 == (wd + wd) - .5 (thickness of screen wall / 2)
translate([-(casew2-3.5),-(caseh2-3.5),wd/2]) { screw_tube(cased-wd, m3d); }
translate([+(casew2-3.5),-(caseh2-3.5),wd/2]) { screw_tube(cased-wd, m3d); }
translate([-(casew2-3.5),+(caseh2-3.5),wd/2]) { screw_tube(cased-wd, m3d); }
translate([+(casew2-3.5),+(caseh2-3.5),wd/2]) { screw_tube(cased-wd, m3d); }

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
