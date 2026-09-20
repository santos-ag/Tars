use tars::*;
const DATA_TR:[[f32;2];5] = [[1.0,2.0],[2.0,4.0],[5.0,10.0],[0.0,0.0],[-2.0,-4.0]];

fn main() {
    const LR:f32 = 1e-2;
    let mut w:f32 = rand::random();
    println!("cost is:{}",cost(w,&DATA_TR));
    for _ in 0..15{
        w = optimizer(w,LR,&DATA_TR);
        println!("cost is:{}",cost(w,&DATA_TR));
    }
}
