#include <carbon/compressor/compressor-utils.h>
#include <carbon/carbon-io-device.h>

size_t carbon_vlq_encode(size_t length, uint8_t *buffer) {
    size_t num_bytes = 0;
    do {
        if(length > 127L) {
            buffer[num_bytes] = (length & 0x7F) | 0x80;
        } else {
            buffer[num_bytes] = length & 0x7F;
        }

        length >>= 7;
        ++num_bytes;
    } while(num_bytes < 10 && length > 0);

    return num_bytes;
}

size_t carbon_vlq_decode(uint8_t const *buffer, size_t *num_bytes) {
    *num_bytes = 0;
    size_t length = 0;
    size_t bit_pos = 0;
    while(*num_bytes < 10) {
        uint8_t byte = buffer[*num_bytes];

        length = length + (((size_t)(byte & 0x7Fu)) << bit_pos);
        bit_pos += 7;
        (*num_bytes)++;

        if((byte & 0x80) == 0)
            break;
    }

    return length;
}

void carbon_vlq_encode_to_io(size_t length, carbon_io_device_t *dst)
{
    uint8_t buf[10];
    size_t len = carbon_vlq_encode(length, buf);
    carbon_io_device_write(dst, buf, 1, len);
}

size_t carbon_vlq_decode_from_io(carbon_io_device_t *src, bool *ok)
{
    uint8_t buf[10];
    size_t len = 0;
    size_t value = 0;

    if(carbon_io_device_read(src, buf, 1, 10) < 1) {
        *ok = false;
        return 0;
    }

    value = carbon_vlq_decode(buf, (size_t *)&len);
    carbon_io_device_seek(src, (size_t)((ssize_t)carbon_io_device_tell(src) - 10L + (ssize_t)len));

    *ok = true;
    return value;
}

char const *carbon_str_reverse(char const *str)
{
    size_t length = strlen(str);
    char *cpy = malloc(length + 1);

    for(size_t i = 0; i < length; ++i) {
        cpy[i] = str[length - 1 -i ];
    }

    return cpy;
}

size_t carbon_vlq_encoded_length(size_t length)
{
    size_t num_bytes = 0;
    do {
        length >>= 7;
        ++num_bytes;
    } while(num_bytes < 10 && length > 0);

    return num_bytes;
}
