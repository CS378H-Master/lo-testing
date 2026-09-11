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
# The version is set by what the OCaml teams actually build against: OCaml
# 5.4.0 / dune 3.23.1 with Jane Street core v0.17.2, core_unix v0.17.1 and
# ppx_let v0.17.1. Debian's apt OCaml is far older, and those libraries will not
# build against it, so apt's ocaml/dune/menhir packages are deliberately NOT
# installed — the whole toolchain comes from an opam switch, compiler included.
# That is a real source bootstrap (~90 packages) and the slowest layer in this
# image by a wide margin, which is exactly why it sits in its own layer.
#
# dune and menhir are unpinned on purpose: dune accepts older `(lang dune 3.x)`
# stanzas, so the newest is the safest choice — pinning 3.23.1 would reject
# anyone who wrote `(lang dune 3.24)`. The libraries are pinned, because their
# APIs are what the code is written against.
#
# PATH carries the switch's bin dir wholesale rather than symlinking the obvious
# four tools: dune shells out to many more (ocamldep, ocamlmklib, ocamlobjinfo,
# ocamlrun, ocamllex...). OCAMLPATH is likewise required, not decorative:
# without it `ocamlfind` does not see the switch's libraries and any
# `(libraries ...)` stanza naming a non-stdlib package fails with
# `Library "x" not found`.
ENV OPAMROOT=/opt/opam \
    OPAMYES=1 \
    OPAMCOLOR=never \
    PATH=/opt/opam/default/bin:$PATH \
    OCAMLPATH=/opt/opam/default/lib \
    CAML_LD_LIBRARY_PATH=/opt/opam/default/lib/stublibs
RUN apt-get update && apt-get install -y --no-install-recommends \
        opam \
        m4 \
        patch \
        unzip \
        bzip2 \
        pkg-config \
        libgmp-dev \
    && rm -rf /var/lib/apt/lists/* \
    && opam init --bare --disable-sandboxing --no-setup -q \
    && opam switch create default 5.4.0 -q \
    && opam install -y -q 'dune>=3.23.1' menhir ocamlfind \
           core.v0.17.2 core_unix.v0.17.1 ppx_let.v0.17.1 \
    && opam clean -a -c -r --logs -q \
    && chmod -R go=rX /opt/opam \
    && ocamlc -version \
    && dune --version \
    && menhir --version \
    && ocamlfind list | grep -E '^(core|core_unix|ppx_let) '

# wasmrun — the WASM host harness, canonical in the lo-runtime repository
# (tools/wasmrun). Installed at image build so the harness named by the P1
# handout is on PATH out of the box.
ARG WASMRUN_GIT=https://github.com/CS378H-Master/lo-runtime
RUN cargo install --git "${WASMRUN_GIT}" wasmrun --root /usr/local \
    && command -v wasmrun

WORKDIR /work
