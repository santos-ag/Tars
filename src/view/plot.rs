use petgraph::graph::UnGraph;

#[derive(Clone, Debug)]
pub struct NetGraph<T> {
    pub nodes: Vec<T>,
}

impl<T> NetGraph<T> {
    pub fn new(nodes: Vec<T>) -> Self {
        Self { nodes }
    }

    pub fn plot(&self) {
        let g = UnGraph::<i32, i32>::from_edges(&[(0, 1), (1, 2), (2, 3), (0, 3)]);
        println!("Network Graph: {:?}", g);
    }
}
