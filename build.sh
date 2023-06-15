#!/bin/bash

if [ "$1" == "test" ]
then
    echo "[IGNORABLE] Removing old test executable.."
    rm test_hkNode || true
    echo "[MANDATORY] Generating test make files.."
    cmake -B artifacts -GNinja test

    echo "[MANDATORY] Building test project.."
    cmake --build artifacts

    echo "[MANDATORY] Moving test executable out.."
    mv artifacts/test_hkNode . || true
else
    echo "[IGNORABLE] Removing old executable.."
    rm hkNode || true

    echo "[MANDATORY] Generating make files.."
    cmake -B debug -GNinja src

    echo "[MANDATORY] Building project.."
    cmake --build debug

    echo "[MANDATORY] Moving executable out.."
    mv debug/hkNode . || true
fi
