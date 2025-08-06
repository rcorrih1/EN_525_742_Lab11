#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <stdint.h>

#define PORT 25344
#define RANDOM_COUNT 1024
#define PACKET_SIZE (4 + (RANDOM_COUNT * 2)) // 4 bytes counter + 2048 bytes random uint16_t

// Convert uint32_t to big-endian byte array
void uint32_to_big_endian(uint32_t val, uint8_t *buf) {
    buf[0] = (val >> 24) & 0xFF;
    buf[1] = (val >> 16) & 0xFF;
    buf[2] = (val >> 8) & 0xFF;
    buf[3] = val & 0xFF;
}

// Write uint16_t to buffer in little-endian
void write_uint16_le(uint16_t val, uint8_t *buf) {
    buf[0] = val & 0xFF;
    buf[1] = (val >> 8) & 0xFF;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s [ip address] [number of packets]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *ip_address = argv[1];
    int packet_count = atoi(argv[2]);

    if (packet_count <= 0) {
        fprintf(stderr, "Invalid number of packets: %s\n", argv[2]);
        return EXIT_FAILURE;
    }

    int sockfd;
    struct sockaddr_in dest_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip_address, &dest_addr.sin_addr) <= 0) {
        perror("Invalid IP address");
        close(sockfd);
        return EXIT_FAILURE;
    }

    // Seed RNG
    srand((unsigned int) time(NULL));

    uint8_t packet[PACKET_SIZE];

    for (uint32_t counter = 0; counter < (uint32_t)packet_count; ++counter) {
        // 4-byte big-endian counter
        uint32_to_big_endian(counter, packet);

        // 1024 random uint16_t values, written in little-endian
        for (int i = 0; i < RANDOM_COUNT; ++i) {
            uint16_t r = (uint16_t)(rand() & 0xFFFF); // Random 16-bit number
            write_uint16_le(r, &packet[4 + i * 2]);
        }

        ssize_t sent = sendto(sockfd, packet, PACKET_SIZE, 0,
                              (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (sent < 0) {
            perror("sendto failed");
            close(sockfd);
            return EXIT_FAILURE;
        }

        printf("Sent packet %u to %s:%d\n", counter, ip_address, PORT);
    }

    close(sockfd);
    return EXIT_SUCCESS;
}
