use ittapi::{Domain, Task};
use std::{thread::sleep, time::Duration};

fn main() {
    let task_duration = Duration::from_millis(50);
    let domain = Domain::new("test");

    let task1 = Task::begin(&domain, "task#1");
    println!("Running task#1...");
    sleep(task_duration);
    task1.end();

    let task2 = Task::begin(&domain, "task#2");
    println!("Running task#2...");
    sleep(task_duration);
    task2.end()
}
