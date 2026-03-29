FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04

# Instalacja wszystkich narzędzi w jednej warstwie
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get install -y python3-pip curl git \
    && pip3 install cpplint==1.6.1 --break-system-packages \
    # Pobieranie Bazeliska
    && curl -L https://github.com/bazelbuild/bazelisk/releases/latest/download/bazelisk-linux-amd64 -o /usr/local/bin/bazel \
    && chmod +x /usr/local/bin/bazel \
    # Pobieranie Buildifiera
    && curl -L https://github.com/bazelbuild/buildtools/releases/latest/download/buildifier-linux-amd64 -o /usr/local/bin/buildifier \
    && chmod +x /usr/local/bin/buildifier \
    && curl -L https://github.com/bazelbuild/buildtools/releases/download/v8.5.1/buildozer-linux-amd64 -o /usr/local/bin/buildozer \
    && chmod +x /usr/local/bin/buildozer \
    # Czyszczenie cache apt, żeby obraz był lżejszy
    && apt-get clean && rm -rf /var/lib/apt/lists/*