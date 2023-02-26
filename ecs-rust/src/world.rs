pub struct World;

impl World {
    pub fn new() -> World {
        Self
    }

    pub fn tick(&mut self, delta_seconds: f32) -> bool {
        true
    }
}
