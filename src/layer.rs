#[derive(Clone,Copy,Debug)]
pub struct Layer<const IN:usize,const OUT:usize>{
    pub weights:[[f32;IN];OUT],
    pub bias:[f32;OUT]
}

impl <const IN:usize,const OUT:usize>Layer<IN,OUT>{
    pub fn new(weights:[[f32;IN];OUT],bias:[f32;OUT])->Self{
        Self{
            weights,
            bias
        }
    }
}
