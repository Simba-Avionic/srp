# Audit runner: QEMU (ARM, SRP EC image) + test client in one container, own netns (tap0 192.168.10.4/24).
FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
      qemu-system-arm iproute2 openssh-client python3 python3-pip python3-venv git ca-certificates procps iputils-ping \
    && rm -rf /var/lib/apt/lists/*
# test client: pytest + UDS over DoIP + SOME/IP (someipy, as used by srp-simulations)
RUN python3 -m venv /opt/venv \
 && /opt/venv/bin/pip install --no-cache-dir pytest doipclient udsoncan loguru pyyaml \
      "git+https://github.com/Simba-Avionic/someipy.git"
ENV PATH=/opt/venv/bin:$PATH PYTHONDONTWRITEBYTECODE=1
