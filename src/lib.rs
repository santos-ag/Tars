pub fn cost(w:f32,data:&[[f32;2]])->f32{
    let mut loss = 0.0;
    for [x,y] in data{
        let erro = y-x * w;
        loss += erro*erro;
    }
    loss
}
pub fn num_grad(w:f32,data:&[[f32;2]])->f32{
    let h = 1e-4;
    (cost(w+h,&data)-cost(w-h,&data))/(2.0*h)
}
pub fn optimizer(w:f32,lr:f32,data:&[[f32;2]])->f32{
    w - num_grad(w,&data) * lr
}


