for f in assets/shaders/*.vert assets/shaders/*.frag; do
    echo "Compiling: $f"
    /usr/bin/glslc "$f" -o "${f%}.spv"
done
