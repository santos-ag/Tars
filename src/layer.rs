use crate::sigmoidf;
pub trait Module{
    fn forward(&self,input:&[f32])->Vec<f32>;
}
#[derive(Clone,Debug)]
pub struct Layer{
    pub in_sz:usize,
    pub out_sz:usize,
    pub weights:Vec<Vec<f32>>,
    pub bias:Vec<f32>
}

impl Layer{
    pub fn new(in_sz:usize,out_sz:usize,weights:Vec<Vec<f32>>,bias:Vec<f32>)->Self{
        Self{
            in_sz,
            out_sz,
            weights,
            bias
        }
    }
}
impl Module for Layer{
    fn forward(&self,input:&[f32])->Vec<f32>{
        let mut output = vec![0.0;self.out_sz];
        for i in 0..self.out_sz{
            output[i] = self.bias[i];
            for j in 0..self.in_sz{
                output[i]+=input[j]*self.weights[i][j];
            }
            output[i] = sigmoidf(output[i]);
        }
        output
    }
}
impl Module for Layer{
    fn forward(&self,input:&[f32])->Vec<f32>{
        let mut output = vec![0.0;self.out_sz];
        for i in 0..self.out_sz{
            output[i] = self.bias[i];
            for j in 0..self.in_sz{
                output[i]+=input[j]*self.weights[i][j];
            }
            output[i] = sigmoidf(output[i]);
        }
        output
    }
}
