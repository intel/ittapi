//! To check that the high-level Rust API works, we perform two kinds of checks here:
//!  1. `run_without_collector`: run the test binary and check that the ITT calls do not break
//!     anything.
//!  2. `run_with_collector`: run the test binary in VTune and check that the created report has
//!     captured the tasks; this relies on having VTune installed and configured (e.g., `vtune`
//!     binary available on the PATH, like done with `source vtune-vars.sh`).
use std::fs::remove_dir_all;
use std::path::Path;
use std::process::Command;
use std::str::from_utf8;

#[test]
fn run_without_collector() {
    let stdout = run(&["cargo", "run"]);
    assert!(stdout.contains("Running task#1..."));
    assert!(stdout.contains("Running task#2..."));
}

#[test]
fn run_with_collector() {
    // First, clean up any previously created results.
    const RESULT_DIR: &str = "target/vtune-results";
    if Path::new(RESULT_DIR).exists() {
        remove_dir_all(RESULT_DIR).unwrap();
    }

    // Then, collect the results using VTune.
    let result_dir_flag = &format!("-result-dir={}", RESULT_DIR);
    let collect_stdout = run(&[
        "vtune",
        "-collect",
        "hotspots",
        result_dir_flag,
        "-knob",
        "sampling-mode=hw",
        "cargo",
        "run",
    ]);
    assert!(collect_stdout.contains("Running task#1..."));
    assert!(collect_stdout.contains("Running task#2..."));

    // Finally, check that the created report actually contains the measured tasks. It may be
    // interesting to filter by task here: e.g.,
    // https://www.intel.com/content/www/us/en/develop/documentation/vtune-help/top/command-line-interface/generating-command-line-reports/filtering-and-grouping-reports.html.
    let report_stdout = run(&["vtune", "-report", "summary", result_dir_flag]);
    assert!(report_stdout.contains("task#1 "));
    assert!(report_stdout.contains("task#2 "));
}

/// Helper for executing system commands, returning the stdout.
fn run(command: &[&str]) -> String {
    let output = Command::new(command[0])
        .args(&command[1..])
        .output()
        .expect("failed to execute process");

    let stdout = from_utf8(&output.stdout).unwrap();
    eprintln!("=== stdout ===\n{}\n==============", stdout);
    let stderr = from_utf8(&output.stderr).unwrap();
    eprintln!("=== stderr ===\n{}\n==============", stderr);

    assert!(
        output.status.success(),
        "failed to run command: `{}`",
        command.join(" ")
    );
    stdout.to_string()
}
