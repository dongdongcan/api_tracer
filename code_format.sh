#!/bin/bash

for file in $(find . -type f); do
    if [[ $file == *.cpp || $file == *.h ]]; then
        echo "Formatting $file..."
        clang-format -i "$file"
    fi
done

echo "All matching files have been formatted."

