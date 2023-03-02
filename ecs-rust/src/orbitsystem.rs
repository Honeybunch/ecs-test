use crate::system::*;

#[derive(Clone)]
pub struct OrbitSystem {}

impl OrbitSystem {
    pub fn new() -> OrbitSystem {
        Self {}
    }
}

impl System for OrbitSystem {
    fn tick(&mut self, _input: &SystemInput, _delta_seconds: f32) -> SystemOutput {
        let out: SystemOutput = Default::default();

        println!("Orbit System Ticks");

        out
    }
}
