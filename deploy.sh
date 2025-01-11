#!/bin/bash

# Exit on any error
set -e

# Default remote path
REMOTE_PATH="/home/pi/bin"

# Help message
show_help() {
    echo "Usage: $0 <remote_host> [remote_path]"
    echo
    echo "Arguments:"
    echo "  remote_host   The remote host to deploy to (e.g., pi@raspberrypi.local)"
    echo "  remote_path   Optional: Remote path to deploy to (default: /home/pi/bin)"
    echo
    echo "Example:"
    echo "  $0 pi@raspberrypi.local /home/pi/apps"
    exit 1
}

# Check arguments
if [ $# -lt 1 ]; then
    show_help
fi

REMOTE_HOST="$1"

# Override default remote path if provided
if [ $# -gt 1 ]; then
    REMOTE_PATH="$2"
fi

echo "Building project..."
# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure and build
cmake ..
make -j$(nproc)

echo "Creating remote directory..."
ssh "$REMOTE_HOST" "mkdir -p $REMOTE_PATH"

echo "Deploying to $REMOTE_HOST:$REMOTE_PATH..."
rsync -avz --progress bin/fclock "$REMOTE_HOST:$REMOTE_PATH/"

echo "Starting fclock on remote host..."
ssh -t "$REMOTE_HOST" "$REMOTE_PATH/fclock"
