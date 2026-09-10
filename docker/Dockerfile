# syntax=docker/dockerfile:1
#
# CS 378H standard build environment — the "course Dockerfile" of the project
# handouts (P1 §3). One image carries the whole toolchain for all four
# projects:
#
#   P1  wasm32:  clang / lld (wasm-ld), the three runtime-skeleton toolchains
#                (Rust with the wasm32-unknown-unknown target, Zig, C++), and
#                the wasmrun host harness, which supplies the runtime's `host`
#                I/O imports and runs a linked LO module.
#   P2+ native:  as / ld / cc (binutils, gcc), CMake, and llvm-dwarfdump for
#                Project 4's DWARF checks.
#
# The image is amd64-only by design: the native back end targets x86-64/Linux,
# and one architecture for all four projects beats a per-project split. On ARM
# Macs it runs under Rosetta (Docker Desktop: enable "Rosetta for x86/amd64
# emulation"; colima: --vm-type=vz --vz-rosetta). The wasm32 tools would run
# natively on arm64, but the image stays uniform so that what passes in the
# container on any laptop is what the grading runs see.
#
# Build:  docker build --platform=linux/amd64 -t cs378h:latest docker/
# Run:    docker run --rm -it --platform=linux/amd64 -v "$PWD":/work cs378h:latest
#
# The build fetches the wasmrun harness from the lo-runtime repository (see
# WASMRUN_GIT below); build the image from a machine that can reach it.
FROM --platform=linux/amd64 debian:bookworm-slim

# Native pipeline (as, ld, cc, cmake), the wasm32 C/C++ toolchain (clang),
# wasm-ld (lld), and llvm-dwarfdump (llvm). git and fetch tooling for builds.
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        binutils \
        gcc \
        cmake \
        git \
        clang \
        lld \
        llvm \
        curl \
        ca-certificates \
        xz-utils \
    && rm -rf /var/lib/apt/lists/*

# Rust (stable, minimal) plus the wasm32 target the rust skeleton builds
# against (`cargo build --target wasm32-unknown-unknown`).
ENV RUSTUP_HOME=/opt/rustup \
    CARGO_HOME=/opt/cargo \
    PATH=/opt/cargo/bin:$PATH
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs \
        | sh -s -- -y --default-toolchain stable --profile minimal \
    && rustup target add wasm32-unknown-unknown \
    && rustc --version

# Zig — the zig skeleton (`zig build`, `zig build -Dtarget=wasm32-freestanding`).
ARG ZIG_VERSION=0.13.0
RUN curl -sSL "https://ziglang.org/download/${ZIG_VERSION}/zig-linux-x86_64-${ZIG_VERSION}.tar.xz" \
        | tar -xJ -C /opt \
    && ln -s "/opt/zig-linux-x86_64-${ZIG_VERSION}/zig" /usr/local/bin/zig \
    && zig version

# OCaml — for teams writing their *compiler* in OCaml. This is only the
# compiler side: the three runtime skeletons (P3) are Rust, Zig and C++, so
# OCaml never appears on the runtime side, and an OCaml compiler still links its
# emitted wasm object against one of those runtimes.
#
# Debian packages the whole standard toolchain (`ocamlc`, `ocamlopt`,
# `ocamllex`, `menhir`, `ocamlfind`, `dune`) into /usr/bin. Its `dune` is 2.9,
# though, which rejects the `(lang dune 3.x)` anyone developing against a
# current dune will have written — so a newer dune and menhir are layered on via
# opam. `ocaml-system` binds the switch to the apt compiler instead of building
# one, turning a multi-minute bootstrap into about ten seconds. The switch's
# tools are surfaced through /usr/local/bin, which precedes /usr/bin, so they
# shadow the apt versions while those remain as a fallback.
#
# OCAMLPATH is required, not decorative: without it `ocamlfind` does not see the
# switch's libraries and any `(libraries ...)` stanza naming a non-stdlib
# package fails with `Library "x" not found`.
ENV OPAMROOT=/opt/opam \
    OPAMYES=1 \
    OPAMCOLOR=never \
    OCAMLPATH=/opt/opam/default/lib \
    CAML_LD_LIBRARY_PATH=/opt/opam/default/lib/stublibs
RUN apt-get update && apt-get install -y --no-install-recommends \
        ocaml-nox \
        ocaml-findlib \
        dune \
        menhir \
        opam \
        bubblewrap \
    && rm -rf /var/lib/apt/lists/* \
    && opam init --bare --disable-sandboxing --no-setup -q \
    && opam switch create default ocaml-system -q \
    && opam install -y dune menhir -q \
    && for tool in dune menhir ocamlfind; do \
           ln -sf "$(opam var bin)/$tool" "/usr/local/bin/$tool"; \
       done \
    && chmod -R go=rX /opt/opam \
    && ocamlc -version \
    && dune --version \
    && menhir --version

# wasmrun — the WASM host harness, canonical in the lo-runtime repository
# (tools/wasmrun). Installed at image build so the harness named by the P1
# handout is on PATH out of the box.
ARG WASMRUN_GIT=https://github.com/CS378H-Master/lo-runtime
RUN cargo install --git "${WASMRUN_GIT}" wasmrun --root /usr/local \
    && command -v wasmrun

WORKDIR /work
