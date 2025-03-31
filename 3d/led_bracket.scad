// octacon led bracket (2 required)
// alias openscad=~/Applications/OpenSCAD-2021.01-x86_64.AppImage
// openscad -o led_bracket.stl led_bracket.scad
// f3d led_bracket.stl

// panel size
pd=2.0;
pw=150.0 + (2 * pd);
pw2=pw/2.0;
pw4=pw/4.0;

// display
dw=62.5;
dw2=dw/2.0;
dw4=dw/4.0;

// leds
lw=14;
lh=4;

// support
sw=4;
sw2=sw/2.0;
sw4=sw/4.0;
lsh=10;
lsh2=lsh/2.0;
lshx=lsh+0.2;
lsg=3;
lsg2=lsg/2.0;

// bracket size
bw=(lw+sw)+(pw2+(pw4+dw4))-(pw2-(pw4+dw4));
bw2=bw/2.0;
bh=(lh+lsg)+sw;

// clip
cr=3.5;

// make it smooth
$fn=25;

// main body
difference() {
    cube([bw,bh,2],center=true);
    
    // led clips
    translate([-(pw4-dw4),0,0]) { cylinder(h=5, r=cr, center=true); }
    translate([-(pw4+dw4),0,0]) { cylinder(h=5, r=cr, center=true); }
    translate([+(pw4-dw4),0,0]) { cylinder(h=5, r=cr, center=true); }
    translate([+(pw4+dw4),0,0]) { cylinder(h=5, r=cr, center=true); }
}
translate([-(bw2+(sw2-sw4)),0,lsh2-1]) { cube([sw2,bh,lsh], center=true); }
translate([+(bw2+(sw2-sw4)),0,lsh2-1]) { cube([sw2,bh,lsh], center=true); }