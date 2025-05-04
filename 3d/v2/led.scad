// octacon led diffusor
// alias openscad=~/Applications/OpenSCAD-2021.01-x86_64.AppImage
// openscad -o led.stl led.scad
// f3d led.stl
//
// slicer settings:
// - normal (0.15mm)
// - infill pattern=tri-hexagon
//
// Use transparent PLA

// wall thickness
wd=2.0;

// leds
lw=14;
lh=4;

union() {
    cube([lw,lh, wd], center=true);
    translate([0,0,-wd/2]) { cube([lw+2,lh+2,0.25],center=true); }
}
