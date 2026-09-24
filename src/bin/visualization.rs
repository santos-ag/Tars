use tars::view::plot::*;

fn main() {
    let g = NetGraph::new(vec![1, 2, 3]);
    g.plot();
    println!("{:?}", g.nodes);
}
