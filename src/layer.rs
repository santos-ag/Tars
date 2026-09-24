pub fn random_vec(len: usize) -> Vec<f32> {
    let mut vector = vec![0.0; len];
    for i in 0..len {
        vector[i] = rand::random();
    }
    vector
}
pub fn random_mat(lenx: usize, leny: usize) -> Vec<Vec<f32>> {
    let mut mat = vec![vec![0.0; lenx]; leny];
    for i in 0..leny {
        for j in 0..lenx {
            mat[i][j] = rand::random();
        }
    }
    mat
}

use crate::sigmoidf;
pub trait Module {
    fn forward(&self, input: &[f32]) -> Vec<f32>;
}
#[derive(Clone, Debug)]
pub struct Linear {
    pub in_sz: usize,
    pub out_sz: usize,
    pub weights: Vec<Vec<f32>>,
    pub bias: Vec<f32>,
}

impl Linear {
    pub fn new(in_sz: usize, out_sz: usize, weights: Vec<Vec<f32>>, bias: Vec<f32>) -> Self {
        Self {
            in_sz,
            out_sz,
            weights,
            bias,
        }
    }
    pub fn random(in_sz: usize, out_sz: usize) -> Self {
        Self {
            in_sz,
            out_sz,
            weights: random_mat(in_sz, out_sz),
            bias: vec![0.0; out_sz],
        }
    }
}
impl Module for Linear {
    fn forward(&self, input: &[f32]) -> Vec<f32> {
        let mut output = vec![0.0; self.out_sz];
        for i in 0..self.out_sz {
            output[i] = self.bias[i];
            for j in 0..self.in_sz {
                output[i] += input[j] * self.weights[i][j];
            }
        }
        output
    }
}

#[derive(Clone, Copy, Debug)]
pub enum Activation {
    Relu,
    Sigmoid,
}
impl Module for Activation {
    fn forward(&self, input: &[f32]) -> Vec<f32> {
        let mut output = vec![0.0; input.len()];
        for i in 0..input.len() {
            output[i] = match self {
                Activation::Relu => input[i].max(0.0),
                Activation::Sigmoid => sigmoidf(input[i]),
            }
        }
        output
    }
}
//um pouco prolixo,mas so descreve que a opçao guarda aquela valor;opcaoA(valorB) no caso o proprio modulo
#[derive(Clone, Debug)]
pub enum AnyModule {
    Linear(Linear),
    Activation(Activation),
}
impl AnyModule {
    pub fn as_linear(&self) -> &Linear {
        match self {
            Self::Linear(linear) => linear,
            _ => panic!(""),
        }
    }
    pub fn as_mut_linear(&mut self) -> &mut Linear {
        match self {
            Self::Linear(linear) => linear,
            _ => panic!(""),
        }
    }
}
impl Module for AnyModule {
    fn forward(&self, input: &[f32]) -> Vec<f32> {
        match self {
            AnyModule::Linear(linear) => linear.forward(input),
            AnyModule::Activation(activation) => activation.forward(input),
        }
    }
}
