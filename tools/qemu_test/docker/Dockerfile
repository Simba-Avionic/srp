# Audit runner: QEMU (ARM, SRP EC image) + test client in one container, own netns (tap0 192.168.10.4/24).
FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
      qemu-system-arm iproute2 openssh-client python3 python3-pip python3-venv git ca-certificates procps iputils-ping \
    && rm -rf /var/lib/apt/lists/*
# test client: pytest + UDS over DoIP + SOME/IP (someipy, as used by srp-simulations)
RUN python3 -m venv /opt/venv \
 && /opt/venv/bin/pip install --no-cache-dir pytest==9.2.0 doipclient==1.2.2 udsoncan==1.26.1 loguru==0.7.3 pyyaml==6.0.3 \
      "git+https://github.com/Simba-Avionic/someipy.git@095d168a0c3561da4648feffe5d3eecf8bcee581"
ENV PATH=/opt/venv/bin:$PATH PYTHONDONTWRITEBYTECODE=1
