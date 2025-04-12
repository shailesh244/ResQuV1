
# gps_http_post for nRF9160 Feather

## Overview
This Zephyr project connects the nRF9160 Feather to LTE, obtains GPS location, and sends it to ThingsBoard via HTTP POST every 180 seconds.

## Setup Instructions

### Prerequisites
- nRF Connect SDK (v2.5.0+)
- GPS antenna
- LTE SIM with data
- Zephyr toolchain setup

### Build and Flash

```bash
west build -b nrf9160_feather_ns
west flash
```

### Configure
Replace `YOUR_ACCESS_TOKEN` in `main.c` with your ThingsBoard device access token.

## License
MIT
