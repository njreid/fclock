#!/bin/bash

# Exit on any error
set -e

# Default values
REMOTE_HOST="rpi"
REMOTE_PATH="~/apps"
BUILD=1
BUILD_MODE="release"  # Always use release mode for deployment

# Help message
show_help() {
    echo "Usage: $0 [options]"
    echo
    echo "Options:"
    echo "  -h, --help           Show this help message"
    echo "  --host HOSTNAME      Remote host (default: rpi)"
    echo "  --path PATH          Remote path (default: ~/bin)"
    echo "  --no-build          Skip building, deploy existing binary"
    echo
    echo "Examples:"
    echo "  $0                    # Build and deploy to rpi:~/bin"
    echo "  $0 --host pi2        # Deploy to different host"
    echo "  $0 --path ~/apps     # Deploy to different path"
    exit 1
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            ;;
        --host)
            REMOTE_HOST="$2"
            shift 2
            ;;
        --path)
            REMOTE_PATH="$2"
            shift 2
            ;;
        --no-build)
            BUILD=0
            shift
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            ;;
    esac
done

# Build if requested
if [ $BUILD -eq 1 ]; then
    echo "Building release version..."
    ./build.sh --release
fi

# Ensure binary exists
if [ ! -f build-arm/bin/fclock ]; then
    echo "Error: Binary not found at build-arm/bin/fclock"
    echo "Run './build.sh --release' first or remove --no-build option"
    exit 1
fi

# Create remote directories
echo "Creating remote directories..."
ssh "$REMOTE_HOST" "mkdir -p $REMOTE_PATH/fclock/{fonts,assets}"

# Deploy binary
echo "Deploying binary..."
scp build-arm/bin/fclock "$REMOTE_HOST:$REMOTE_PATH/fclock/"

# Deploy fonts and config
echo "Deploying assets, fonts and config..."
rsync -av --delete assets/ "$REMOTE_HOST:$REMOTE_PATH/fclock/assets/"
rsync -av --delete fonts/ "$REMOTE_HOST:$REMOTE_PATH/fclock/fonts/"
rsync -av src/fclock.conf "$REMOTE_HOST:$REMOTE_PATH/fclock/"

echo "Deployment complete!"
echo "Binary location: $REMOTE_PATH/fclock/fclock"
echo "Asset location: $REMOTE_PATH/fclock/fonts"
echo "Fonts location: $REMOTE_PATH/fclock/fonts"
echo "Config location: $REMOTE_PATH/fclock/fclock.conf"
