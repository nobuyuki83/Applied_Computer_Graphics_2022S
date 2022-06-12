difference(){
    union() {
        translate([-5,-5,0]){
            cube([10,10,100]);
        }
        cylinder(10, r=20);
        translate([0,0,100]) {
            sphere(13);
        }
    }
    union() {
        translate([0,0,100]){
            sphere(10);
        }        
        translate([0,0,150]){
            cube([100,100,100], center=true);
        }
    }
    
}
