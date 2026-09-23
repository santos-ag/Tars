use crate::Grad;
use crate::Model;
#[derive(Clone, Copy, Debug)]
pub struct BGD {
    lr: f32,
}

impl BGD {
    pub fn new(lr: f32) -> Self {
        Self { lr }
    }
    pub fn step(&self, model: &mut Model, grad: &Grad) {
        for l in 0..model.layers.len() {
            for o in 0..model.layers[l].weights.len() {
                for w in 0..model.layers[l].weights[o].len() {
                    model.layers[l].weights[o][w] -= grad.layers[l].weights[o][w] * self.lr;
                }
            }
            for o in 0..model.layers[l].bias.len() {
                model.layers[l].bias[o] -= grad.layers[l].bias[o] * self.lr;
            }
        }
    }
}
