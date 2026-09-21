use tars::*;
const DATA_TR:&[Data<2,1>] = &[
   Data::new([0.0,0.0],[0.0]),
   Data::new([1.0,0.0],[1.0]),
   Data::new([0.0,1.0],[1.0]),
   Data::new([1.0,1.0],[1.0])
];
fn main() {
    const LR:f32 = 1e1;
    const EPOCHS:usize = 100000;
    let mut model=Model::new(
        Layer::new(

            [[
                rand::random(),
                rand::random()
            ]],
            [rand::random();1]
        )
    );
    let optimizer = BGD::new(LR);
    let mut prev_cost = cost(&model,&DATA_TR);
            println!("epoch: 000000, cost is:{:014.8}",prev_cost);
    {
    for i in 1..EPOCHS+1{
        let grad = num_grad(&model,&DATA_TR);
        optimizer.step(&mut model,&grad);
        let curr_cost = cost(&model,&DATA_TR);
        if i%(EPOCHS/20)==0 {
            println!("epoch: {:06.0}, cost is:{:014.8}, {:07.3}% better",i,curr_cost,((prev_cost-curr_cost)*100.0)/prev_cost);
            prev_cost = curr_cost;

        }
    }
    }
    println!("The model is: {:?}",model);
}
