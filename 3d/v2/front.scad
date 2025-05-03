// octacon frontpanel
// alias openscad=~/Applications/OpenSCAD-2021.01-x86_64.AppImage
// openscad -o front.stl front.scad
// f3d front.stl

// panel size
pd=2.0;
pw=150.0 + (2 * pd);
pw2=pw/2.0;
pw4=pw/4.0;
ph=100.0 + (2 * pd);
ph2=ph/2.0;

// threaded inserts: outer diameter 5mm, depth 4mm