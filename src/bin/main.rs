const DATA_TR:[[f32;2];5] = [[1.0,2.0],[2.0,4.0],[5.0,10.0],[0.0,0.0],[-2.0,-4.0]];
fn cost(w:f32)->f32{
    let mut loss = 0.0;
    for [x,y] in DATA_TR{
        let erro = y-x * w;
        loss += erro*erro;
    }
    loss
}
fn num_grad(w:f32)->f32{
    let h = 1e-4;
    (cost(w+h)-cost(w-h))/(2.0*h)
}
fn optimizer(w:f32,lr:f32)->f32{
    w - num_grad(w) * lr
}
fn main() {
    const LR:f32 = 1e-2;
    let mut w:f32 = rand::random();
    println!("cost is:{}",cost(w));
    for _ in 0..15{
        w = optimizer(w,LR);
        println!("cost is:{}",cost(w));
    }
}
