FROM mcr.microsoft.com/devcontainers/python:3.12
RUN pip install --no-cache-dir platformio \
 && apt-get update && apt-get install -y --no-install-recommends git \
 && rm -rf /var/lib/apt/lists/*
RUN usermod -aG dialout vscode
USER vscode
# Pre-fetch the ESP32 toolchain so the first build is fast
RUN pio pkg install -g -p espressif32
