use crate::Layer;
#[derive(Clone,Copy,Debug)]
pub struct Model<const IN:usize>{
    pub layer1:Layer<IN,1>
}

impl <const IN:usize>Model<IN>{
    pub fn new(layer1:Layer<IN,1>)->Self{
        Self{
            layer1
        }
    }
}
