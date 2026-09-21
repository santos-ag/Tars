#[derive(Clone,Copy,Debug)]
pub struct Data{
    pub input: [f32;2],
    pub target: [f32;1]
}
impl Data {
    pub const fn new(input:[f32;2],target:[f32;1])->Self{
        Self {
            input,
            target
        }
    }
}

#[derive(Clone,Copy,Debug)]
pub struct Grad{
    pub weights:[f32;2],
    pub bias:[f32;1]
}

#[derive(Clone,Copy,Debug)]
pub struct Model{
    pub weights:[f32;2],
    pub bias:[f32;1]
}

impl Model{
    pub fn new(weights:[f32;2],bias:[f32;1])->Self{
        Self{
            weights,
            bias
        }
    }
}

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
    pub fn step(&self,model:&mut Model,grad:&Grad){
        for i in 0..model.weights.len(){
            model.weights[i] -= grad.weights[i] * self.lr;

        }
        for i in 0..model.bias.len(){
            model.bias[i] -= grad.bias[i] * self.lr;

        }    
    }
}
pub fn sigmoidf(x:f32)->f32{
    1.0/(1.0+(-x).exp())
}
pub fn forward(model: &Model,input:[f32;2])->f32{
    let mut z = model.bias[0];
    for i in 0..model.weights.len(){
        z+=input[i]*model.weights[i];
    }
    sigmoidf(z)
}
pub fn cost(model:&Model,data:&[Data;4])->f32{
    let mut loss = 0.0;
    for sample in data{
        let erro = sample.target[0]-forward(model,sample.input);
        loss += erro*erro;
    }
    loss
}
pub fn num_grad(model: &Model,data:&[Data;4])->Grad{
    let mut tempM = *model;

    let mut grad=Grad{
        weights:[0.0;2],
        bias:[0.0;1]
    };

    let h = 1e-3;

    for i in 0..model.weights.len(){
        tempM.weights[i] += h;
        let costp = cost(&tempM,data);
        tempM.weights[i] -= 2.0*h;
        let costm = cost(&tempM,data);
        grad.weights[i] = (costp-costm)/(2.0*h);
        tempM.weights[i] += h;
    }
    for i in 0..model.bias.len(){
        tempM.bias[i] += h;
        let costp = cost(&tempM,data);
        tempM.bias[i] -= 2.0*h;
        let costm = cost(&tempM,data);
        grad.bias[i] = (costp-costm)/(2.0*h);
        tempM.bias[i] += h;
    }
    grad

}



