@echo off

pushd out\build\x64-debug

ninja --quiet run%1

popd