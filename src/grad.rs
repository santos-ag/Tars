use crate::cost;
use crate::Data;
use crate::Model;
#[derive(Clone,Copy,Debug)]
pub struct Grad<const IN:usize,const OUT:usize>{
    pub weights:[[f32;IN];OUT],
    pub bias:[f32;OUT]
}

pub fn num_grad<const IN:usize,const OUT:usize>(model: &Model<IN>,data:&[Data<IN,OUT>])->Grad<IN,OUT>{
    let mut tempM = *model;

    let mut grad=Grad{
        weights:[[0.0;IN];OUT],
        bias:[0.0;OUT]
        };

    let h = 1e-3;

    for i in 0..model.layer1.weights[0].len(){
        tempM.layer1.weights[0][i] += h;
        let costp = cost(&tempM,data);
        tempM.layer1.weights[0][i] -= 2.0*h;
        let costm = cost(&tempM,data);
        grad.weights[0][i] = (costp-costm)/(2.0*h);
        tempM.layer1.weights[0][i] += h;
    }
    for i in 0..model.layer1.bias.len(){
        tempM.layer1.bias[i] += h;
        let costp = cost(&tempM,data);
        tempM.layer1.bias[i] -= 2.0*h;
        let costm = cost(&tempM,data);
        grad.bias[i] = (costp-costm)/(2.0*h);
        tempM.layer1.bias[i] += h;
    }
    grad

}
