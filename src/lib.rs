pub mod data;
pub use data::*;
pub mod layer;
pub use layer::*;
pub mod optimizer;
pub use optimizer::*;
pub mod grad;
pub use grad::*;
pub mod model;
pub use model::*;







pub fn sigmoidf(x:f32)->f32{
    1.0/(1.0+(-x).exp())
}
pub fn forward<const IN:usize>(model: &Model<IN>,input:[f32;IN])->f32{
    let mut z = model.layer1.bias[0];
    for i in 0..model.layer1.weights[0].len(){
        z+=input[i]*model.layer1.weights[0][i];
    }
    sigmoidf(z)
}
pub fn cost<const IN:usize,const OUT:usize>(model:&Model<IN>,data:&[Data<IN,OUT>])->f32{
    let mut loss = 0.0;
    for sample in data{
        let erro = sample.target[0]-forward(model,sample.input);
        loss += erro*erro;
    }
    loss
}




