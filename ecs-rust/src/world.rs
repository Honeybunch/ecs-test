use crate::system::*;

pub struct World {
    systems: Vec<Box<dyn System>>,
}

impl World {
    pub fn new() -> World {
        Self {
            systems: Default::default(),
        }
    }

    pub fn register_system(&mut self, sys: Box<dyn System>) {
        self.systems.push(sys);
    }

    pub fn tick(&mut self, delta_seconds: f32) -> bool {
        for i in 0..self.systems.len() {
            let input: SystemInput = self.filter_input(&self.systems[i]);

            let output = self.systems[i].tick(&input, delta_seconds);

            self.write_output(&output);
        }

        true
    }

    fn filter_input(&self, _sys: &Box<dyn System>) -> SystemInput {
        Default::default()
    }

    fn write_output(&mut self, _output: &SystemOutput) {}
}
