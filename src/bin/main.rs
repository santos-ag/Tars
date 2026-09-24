use tars::{Activation::Relu, *};
const DATA_TR: &[Data<2, 2>] = &[
    Data::new([0.1, 0.2], [0.15, 0.02]),
    Data::new([0.2, 0.8], [0.50, 0.16]),
    Data::new([0.3, 0.4], [0.35, 0.12]),
    Data::new([0.4, 0.9], [0.65, 0.36]),
    Data::new([0.5, 0.5], [0.50, 0.25]),
    Data::new([0.6, 0.2], [0.40, 0.12]),
    Data::new([0.7, 0.3], [0.50, 0.21]),
    Data::new([0.8, 0.6], [0.70, 0.48]),
    Data::new([0.9, 0.1], [0.50, 0.09]),
    Data::new([0.9, 0.9], [0.90, 0.81]),
];

fn main() {
    let mut model = Sequential::new(2).linear(4).relu().linear(4).relu();
    let input = DATA_TR[0].input;
    println!("{:?},", model.forward(&input));

    /*
    const LR:f32 = 2e1;
    const EPOCHS:usize = 100000;
    let mut model=Model::new(&[2,4,2]);

    let optimizer = BGD::new(LR);
    let mut prev_cost = cost(&model,&DATA_TR);
            println!("epoch: 000000, cost is:{:014.8}",prev_cost);

    for i in 1..EPOCHS+1{
        let grad = num_grad(&model,&DATA_TR);
        optimizer.step(&mut model,&grad);
        let curr_cost = cost(&model,&DATA_TR);
        if i%(EPOCHS/20)==0 {
            println!("epoch: {:06.0}, cost is:{:014.8}, {:07.3}% better",i,curr_cost,((prev_cost-curr_cost)*100.0)/prev_cost);
            prev_cost = curr_cost;

        }
    }

    println!("{model}");

    for d in DATA_TR{
        println!("Para as entradas: {:?} o modelo retorna: {:?} esperado:{:?} ",d.input,model.forward(&d.input),d.target);
    }*/
}
