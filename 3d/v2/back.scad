// octacon bottom case
// alias openscad=~/Applications/OpenSCAD-2021.01-x86_64.AppImage
// openscad -o back.stl back.scad
// f3d back.stl

// wall thickness
wd=2.0;

// case size
pw=160.0 + (2 * wd);
pw2=pw/2.0;
pw4=pw/4.0;
ph=100.0 + (2 * wd);
ph2=ph/2.0;
// 20 mm screw - 4mm for inserts on the other side
// 15 mm is enought to hold the base pcb with the pico2
pd=20-4;

// pcb size
pcbw=150.0;
pcbw2=pcbw/2.0;
pcbh=100.0;
pcbh2=pcbh/2.0;

// corners
cr=3;
cd=cr*2;
co=cr/2;

// m3 screw holes
m3d=3.2; // extra size to cover 3d print thickness

// make it smooth
$fn=25;

difference() {
    // main body
    cube([pw,ph,pd],center=true);

    // space for inside
    translate([0,0,wd]) { cube([pw - (2.0 * wd), ph - (2.0 * wd), pd-wd],center=true); }
    
    // rounded corners
    translate([-(pw2-cr),-(ph2-cr),0]) { round_corner(pd+5, cd, -co, -co); }
    translate([+(pw2-cr),-(ph2-cr),0]) { round_corner(pd+5, cd, +co, -co); }
    translate([-(pw2-cr),+(ph2-cr),0]) { round_corner(pd+5, cd, -co, +co); }
    translate([+(pw2-cr),+(ph2-cr),0]) { round_corner(pd+5, cd, +co, +co); }

    // holes for case screews
    translate([-(pw2-3.5),-(ph2-3.5),0]) { screw_hole(pd, m3d); }
    translate([+(pw2-3.5),-(ph2-3.5),0]) { screw_hole(pd, m3d); }
    translate([-(pw2-3.5),+(ph2-3.5),0]) { screw_hole(pd, m3d); }
    translate([+(pw2-3.5),+(ph2-3.5),0]) { screw_hole(pd, m3d); }
    
    // holes for base screws
    translate([-(pcbw2-2.5),-(pcbh2-2.5),0]) { screw_hole(pd, m3d); }
    translate([+(pcbw2-2.5),-(pcbh2-2.5),0]) { screw_hole(pd, m3d); }
    translate([-(pcbw2-2.5),+(pcbh2-2.5),0]) { screw_hole(pd, m3d); }
    translate([+(pcbw2-2.5),+(pcbh2-2.5),0]) { screw_hole(pd, m3d); }
    
    // usb hole
    
    // debug header hole
}

// tubes for case screews
// 3.5 == (wd + wd) - .5 (thickness of screen wall / 2)
translate([-(pw2-3.5),-(ph2-3.5),wd/2]) { screw_tube(pd-wd, m3d); }
translate([+(pw2-3.5),-(ph2-3.5),wd/2]) { screw_tube(pd-wd, m3d); }
translate([-(pw2-3.5),+(ph2-3.5),wd/2]) { screw_tube(pd-wd, m3d); }
translate([+(pw2-3.5),+(ph2-3.5),wd/2]) { screw_tube(pd-wd, m3d); }



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
