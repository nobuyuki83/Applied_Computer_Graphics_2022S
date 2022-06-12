
union() {
    translate([-5,-5,0]){
        cube([10,10,20]);
    }
    translate([-5,-5,15]){
        cube([60,10,10]);
    }
    cube([10,25,25],center=true);
    translate([50,00,20]) {
        rotate([0,90,00]){
            cylinder(h=20,r1=10,r2=0);
        }
    }
}
    

