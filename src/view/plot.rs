use petgraph::graph::UnGraph;

pub fn plot() {
    let g = UnGraph::<(), ()>::from_edges(&[(0, 1), (1, 2), (2, 3), (0, 3)]);

    println!("Graph: {:?}", g);
}
