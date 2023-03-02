#[derive(Default)]
pub struct SystemInput {}

#[derive(Default)]
pub struct SystemOutput {}

pub trait System {
    fn tick(&mut self, input: &SystemInput, delta_seconds: f32) -> SystemOutput;
}
