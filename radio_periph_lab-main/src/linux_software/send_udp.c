#include "send_udp.h"


void send_udp(const char* ip_address, const uint32_t* data, uint16_t port) {
    static uint32_t counter = 0;

    // Create socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return;
    }

    // Setup destination address
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip_address, &dest_addr.sin_addr) <= 0) {
        perror("Invalid IP address");
        close(sock);
        return;
    }

    // Build packet
    uint8_t packet[PACKET_SIZE];

    // Insert counter in little-endian
    packet[0] = (uint8_t)(counter & 0xFF);
    packet[1] = (uint8_t)((counter >> 8) & 0xFF);
    packet[2] = (uint8_t)((counter >> 16) & 0xFF);
    packet[3] = (uint8_t)((counter >> 24) & 0xFF);

    // Copy data in little-endian format
    for (int i = 0; i < DATA_COUNT; i++) {
        uint32_t value = data[i];
        packet[4 + i * 4 + 0] = (uint8_t)(value & 0xFF);
        packet[4 + i * 4 + 1] = (uint8_t)((value >> 8) & 0xFF);
        packet[4 + i * 4 + 2] = (uint8_t)((value >> 16) & 0xFF);
        packet[4 + i * 4 + 3] = (uint8_t)((value >> 24) & 0xFF);
    }

    // Send packet
    ssize_t sent = sendto(sock, packet, PACKET_SIZE, 0,
                          (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    if (sent != PACKET_SIZE) {
        perror("Failed to send full packet");
    }

    close(sock);
    counter++;
}