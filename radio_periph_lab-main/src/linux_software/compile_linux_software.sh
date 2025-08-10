# Build script for streaming program and radio tuner/setter
# 
# sed -i 's/\r$//' compile_linux_software.sh	strip script of windows whitespace chars
# chmod +x compile_linux_software.sh			make script executable
# ./compile_linux_software.sh					run script

# Go to the script's directory
cd "$(dirname "$0")" || exit 1

# Set output binary name
OUTPUT1="stream_udp_data"
OUTPUT2="tune_set_radio"

# Source files
SRC1="read_fifo.c"
SRC2="send_udp.c"
SRC3="get_a_pointer.c"
SRC4="stream_udp_data.c"
SRC5="tune_set_radio.c"

# Compile with gcc
/usr/bin/gcc -Wall -O2 -o "$OUTPUT1" "$SRC1" "$SRC2" "$SRC3" "$SRC4"
/usr/bin/gcc -Wall -O2 -o "$OUTPUT2" "$SRC3" "$SRC5"

# Check if compilation succeeded
if [ $? -eq 0 ]; then
    echo "Build successful. Executable: $OUTPUT1, $OUTPUT2"
else
    echo "Build failed."
fi