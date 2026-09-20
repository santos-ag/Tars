#[derive(Clone,Copy,Debug)]
pub struct Data<const IN: usize,const OUT:usize>{
    pub input: [f32;IN],
    pub target: [f32;OUT]
}

pub fn cost(w:f32,b:f32,data:&[[f32;2]])->f32{
    let mut loss = 0.0;
    for [x,y] in data{
        let erro = y-(x*w+b);
        loss += erro*erro;
    }
    loss
}
pub fn num_grad(w:f32,b:f32,data:&[[f32;2]])->[f32;2]{
    let h = 1e-3;
    [
        (cost(w+h,b,&data)-cost(w-h,b,&data))/(2.0*h),
        (cost(w,b+h,&data)-cost(w,b-h,&data))/(2.0*h),
    ]
}
pub fn optimizer(w:f32,b:f32,lr:f32,data:&[[f32;2]])->[f32;2]{
    [
        w - num_grad(w,b,&data)[0] * lr,
        b - num_grad(w,b,&data)[1]*lr
    ]
}


