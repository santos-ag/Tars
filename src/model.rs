use crate::Layer;
use crate::Module;
use std::fmt;
#[derive(Clone, Debug)]
pub struct Model {
    pub layers: Vec<Layer>,
}

impl Model {
    pub fn new(topology: &[usize]) -> Self {
        let mut layers = vec![];
        for l in 0..(topology.len() - 1) {
            let mut weight = vec![vec![0.0; topology[l]]; topology[l + 1]];
            let mut bias = vec![0.0; topology[l + 1]];
            for o in 0..topology[l + 1] {
                for w in 0..topology[l] {
                    weight[o][w] = rand::random();
                }
                bias[o] = rand::random();
            }
            let layer = Layer::new(topology[l], topology[l + 1], weight, bias);
            layers.push(layer);
        }
        let model = Model { layers };
        model
    }
    pub fn forward(&self, input: &[f32]) -> Vec<f32> {
        let mut act = input.to_vec();
        for l in &self.layers {
            act = l.forward(&act);
        }
        act
    }
}
impl fmt::Display for Model {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        for l in 0..self.layers.len() {
            writeln!(f, "\nlayer {l}\n")?;
            for o in 0..self.layers[l].weights.len() {
                writeln!(
                    f,
                    "\nneuronio de saida {o} com bias {}\n",
                    self.layers[l].bias[o]
                )?;
                for w in 0..self.layers[l].weights[o].len() {
                    writeln!(f, "Peso {w} com valor {}", self.layers[l].weights[o][w])?;
                }
            }
        }
        Ok(())
    }
}
