
#include <zephyr.h>
#include <modem/lte_lc.h>
#include <modem/gps.h>
#include <net/socket.h>
#include <stdio.h>
#include <string.h>

#define THINGSBOARD_HOST "demo.thingsboard.io"
#define THINGSBOARD_PORT 80
#define THINGSBOARD_TOKEN "YOUR_ACCESS_TOKEN"
#define VIBRATE_INTERVAL K_SECONDS(180)

static float latitude, longitude;

void gps_handler(const struct gps_event *evt)
{
    if (evt->type == GPS_EVT_PVT) {
        if (evt->pvt.fix) {
            latitude = evt->pvt.latitude;
            longitude = evt->pvt.longitude;
            printk("GPS: Lat: %f, Lon: %f\n", latitude, longitude);
        }
    }
}

void send_http_post()
{
    int sock;
    struct sockaddr_in server;

    char payload[128];
    snprintf(payload, sizeof(payload), "{\"latitude\":%.6f,\"longitude\":%.6f}", latitude, longitude);

    char request[512];
    snprintf(request, sizeof(request),
             "POST /api/v1/%s/telemetry HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %d\r\n"
             "\r\n"
             "%s",
             THINGSBOARD_TOKEN, THINGSBOARD_HOST, strlen(payload), payload);

    server.sin_family = AF_INET;
    server.sin_port = htons(THINGSBOARD_PORT);
    inet_pton(AF_INET, "18.216.69.13", &server.sin_addr);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printk("HTTP connect failed\n");
        return;
    }

    send(sock, request, strlen(request), 0);
    close(sock);
}

void main(void)
{
    printk("Connecting to LTE...\n");
    if (lte_lc_init_and_connect() != 0) {
        printk("LTE connection failed\n");
        return;
    }
    printk("Connected to LTE\n");

    struct gps_config gps_cfg = {
        .nav_mode = GPS_NAV_MODE_SINGLE_FIX,
        .interval = 1,
        .priority = false,
        .delete_mask = 0,
        .use_case = GPS_USE_CASE_SINGLE_FIX
    };

    gps_init(gps_handler);

    while (1) {
        gps_start(&gps_cfg);
        k_sleep(K_SECONDS(60));
        gps_stop();
        send_http_post();
        k_sleep(VIBRATE_INTERVAL);
    }
}
