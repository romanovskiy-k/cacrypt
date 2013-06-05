#!/bin/bash
uncrustify -l CPP -c ./uncrustify-cpp.cfg --no-backup $(find src -name "*.cpp" -o -name "*.h")
uncrustify -l C -c ./uncrustify.cfg --no-backup $(find src -name "*.cl")
