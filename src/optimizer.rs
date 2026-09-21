use crate::Model;
use crate::Grad;
#[derive(Clone,Copy,Debug)]
pub struct BGD{
    lr:f32
}

impl BGD{
    pub fn new(lr:f32)->Self{
        Self{
            lr
        }
    }
    pub fn step<const IN:usize,const OUT:usize>(&self,model:&mut Model<IN>,grad:&Grad<IN,OUT>){
        for i in 0..model.layer1.weights[0].len(){
            model.layer1.weights[0][i] -= grad.weights[0][i] * self.lr;

        }
        for i in 0..model.layer1.bias.len(){
            model.layer1.bias[i] -= grad.bias[i] * self.lr;

        }    
    }
}
