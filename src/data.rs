#[derive(Clone, Copy, Debug)]
pub struct Data<const IN: usize, const OUT: usize> {
    pub input: [f32; IN],
    pub target: [f32; OUT],
}
impl<const IN: usize, const OUT: usize> Data<IN, OUT> {
    pub const fn new(input: [f32; IN], target: [f32; OUT]) -> Self {
        Self { input, target }
    }
}
