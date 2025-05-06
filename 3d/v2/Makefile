all: back.stl back.test.stl front.stl front.test.stl led.stl

%.test.stl : %.scad
	openscad -Dtest=true -o $@ $<

%.stl : %.scad
	openscad -o $@ $<
