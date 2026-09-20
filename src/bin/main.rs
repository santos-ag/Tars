use tars::*;
const DATA_TR:[[f32;2];5] = [[1.0,15.0],[2.0,47.0],[5.0,143.0],[0.0,-17.0],[-2.0,-81.0]];

fn main() {
    const LR:f32 = 1e-2;
    const EPOCHS:usize = 150;
    let mut w:f32 = rand::random();
    let mut b:f32 = rand::random();
    w=w*5.0-1.0;
    b = b*5.0-1.0;
    let mut prev_cost = cost(w,b,&DATA_TR);
            println!("epoch: 000000, cost is:{:014.8}",prev_cost);
    {
    for i in 1..EPOCHS{
        [w,b] = optimizer(w,b,LR,&DATA_TR);
        let curr_cost = cost(w,b,&DATA_TR);
        if i%(EPOCHS/20)==0 {
            println!("epoch: {:06.0}, cost is:{:014.8}, {:07.3}% better",i,curr_cost,((prev_cost-curr_cost)*100.0)/prev_cost);

        }
        prev_cost = curr_cost;
    }
    }
    println!("w is : {w},b is {b}");
}
