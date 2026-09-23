use crate::Data;
use crate::Layer;
use crate::Model;
use crate::cost;
#[derive(Clone, Debug)]
pub struct Grad {
    pub layers: Vec<Layer>,
}

pub fn num_grad<const IN: usize, const OUT: usize>(model: &Model, data: &[Data<IN, OUT>]) -> Grad {
    let mut temp_m = model.clone();

    let mut grad = Grad {
        layers: model.layers.clone(),
    };

    let h = 1e-3;
    for l in 0..model.layers.len() {
        for o in 0..model.layers[l].weights.len() {
            for w in 0..model.layers[l].weights[o].len() {
                temp_m.layers[l].weights[o][w] += h;
                let costp = cost(&temp_m, data);
                temp_m.layers[l].weights[o][w] -= 2.0 * h;
                let costm = cost(&temp_m, data);
                grad.layers[l].weights[o][w] = (costp - costm) / (2.0 * h);
                temp_m.layers[l].weights[o][w] += h;
            }
        }
        for o in 0..model.layers[l].bias.len() {
            temp_m.layers[l].bias[o] += h;
            let costp = cost(&temp_m, data);
            temp_m.layers[l].bias[o] -= 2.0 * h;
            let costm = cost(&temp_m, data);
            grad.layers[l].bias[o] = (costp - costm) / (2.0 * h);
            temp_m.layers[l].bias[o] += h;
        }
    }
    grad
}
