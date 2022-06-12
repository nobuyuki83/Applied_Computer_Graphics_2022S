difference(){
    union() {
        translate([-5,-5,0]){
            cube([10,10,100]);
        }
        sphere(r=10);
        translate([0,0,100]) {
            cube([20,30,30],center=true);
        }
    }
    translate([0,0,110]){
        cube([10,40,40],center=true);
    }        
    
}
