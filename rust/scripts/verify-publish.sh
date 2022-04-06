#!/bin/bash
# Check that the Rust crates can all be packaged up for publication. This cannot use use `cargo
# publish --dry-run` because of the dependency between ittapi and ittapi-sys.
set -e
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
pushd $SCRIPT_DIR

# Vendor all dependencies into the `vendor` directory and configure Cargo to look there for
# dependencies. This is necessary so that we can package up ittapi-sys there and ittapi can refer to
# it.
rm -rf .cargo vendor
cargo vendor
mkdir .cargo
cat > .cargo/config.toml << EOT
[source.crates-io]
replace-with = "vendored-sources"
[source.vendored-sources]
directory = "vendor"
EOT

# Package up ittapi-sys and place it in the vendor directory.
cargo package --manifest-path ../ittapi-sys/Cargo.toml
pushd vendor
tar xf ../../target/package/ittapi-sys-0.*.crate
ITTAPI_SYS_DIR=$(echo ittapi-sys-0.*)
echo '{"files":{}}' > $ITTAPI_SYS_DIR/.cargo-checksum.json
popd

# Package up ittapi.
cargo package --manifest-path ../ittapi/Cargo.toml
popd
