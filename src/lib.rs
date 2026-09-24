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
pub mod math;
pub use math::*;
pub mod view;

pub fn cost<const IN: usize, const OUT: usize>(model: &Sequential, data: &[Data<IN, OUT>]) -> f32 {
    let mut loss = 0.0;
    for sample in data {
        let mut erro = 0.0;
        let target = &sample.target;
        let pred = model.forward(&sample.input);
        for i in 0..OUT {
            erro += (target[i] - pred[i]) * (target[i] - pred[i]);
        }
        loss += erro / (OUT as f32);
    }
    loss / data.len() as f32
}
