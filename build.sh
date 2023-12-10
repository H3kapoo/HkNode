#!/bin/bash
echo "[IGNORABLE] Removing old executable.."
rm hkNode || true

echo "[MANDATORY] Generating make files.."
cmake -B debug -GNinja src

echo "[MANDATORY] Building project.."
cmake --build debug

echo "[MANDATORY] Moving executable out.."
# mv debug/hkNode . || true
# mv debug/windowFrameWithLabel . || true
mv debug/containersPinch . || true
