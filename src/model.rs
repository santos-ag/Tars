use crate::Activation;
use crate::AnyModule;
use crate::Linear;
use crate::Module;
//use std::fmt;

#[derive(Clone, Debug)]
pub struct Sequential {
    pub modules: Vec<AnyModule>,
    input_size: usize,
}
impl Sequential {
    pub fn new(input_size: usize) -> Self {
        Self {
            modules: Vec::new(),
            input_size,
        }
    }
    pub fn linear(mut self, output_size: usize) -> Self {
        let linear = Linear::random(self.input_size, output_size);
        self.modules.push(AnyModule::Linear(linear));
        self.input_size = output_size;
        self
    }
    pub fn relu(mut self) -> Self {
        self.modules.push(AnyModule::Activation(Activation::Relu));
        self
    }
    pub fn sigmoid(mut self) -> Self {
        self.modules
            .push(AnyModule::Activation(Activation::Sigmoid));
        self
    }
}

impl Module for Sequential {
    fn forward(&self, input: &[f32]) -> Vec<f32> {
        let mut output = input.to_vec();
        for module in &self.modules {
            output = module.forward(&output);
        }
        output
    }
}

/*
#[derive(Clone, Debug)]
pub struct Model {
    pub modules: Vec<AnyModule>,
}

impl Model {
    pub fn new(net: &[AnyModule]) -> Self {
        Self {
            modules: net.to_vec(),
        }
    }
    fn forward(&self, input: &[f32]) -> Vec<f32> {
        let mut output = input.to_vec();
        for module in &self.modules {
            output = module.forward(&output);
        }
        output
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
}*/
