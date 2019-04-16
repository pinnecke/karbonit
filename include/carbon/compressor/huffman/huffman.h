#ifndef CARBON_HUFFMAN_HUFFMAN_H
#define CARBON_HUFFMAN_HUFFMAN_H

#include <glob.h>
#include <limits.h>

#include <carbon/carbon-common.h>


/**
  Huffman library. Example usage:

    carbon_huffman_encoder_t encoder;
    carbon_huffman_encoder_create(&encoder);
    carbon_huffman_encoder_learn_frequencies(&encoder, "aaaaaaaabbbbccd");
    carbon_huffman_encoder_bake_code(&encoder);

    for(size_t i = 0; i < UCHAR_MAX; ++i) {
        if(encoder.frequencies[i]) {
            printf("%c ", (char)i);
            carbon_huffman_bitstream_dump(&encoder.codes[i]);
            printf("\n");
        }
    }

    carbon_huffman_bitstream_t stream;
    carbon_huffman_bitstream_create(&stream);
    carbon_huffman_encode(&encoder, &stream, "aaabb");
    carbon_huffman_bitstream_dump(&stream);
    printf("\n");

    carbon_huffman_decoder_t decoder;
    carbon_huffman_decoder_create(&decoder, encoder.codes);
    printf("Decoded: %s\n", carbon_huffman_decode(&decoder, &stream));
    carbon_huffman_decoder_drop(&decoder);
    carbon_huffman_encoder_drop(&encoder);
  */


typedef struct carbon_huffman_bitstream {
    size_t num_bits;
    size_t capacity;
    uint8_t *data;
} carbon_huffman_bitstream_t;

typedef carbon_huffman_bitstream_t carbon_huffman_dictionary_t[UCHAR_MAX];

typedef struct carbon_huffman_tree_node {
    struct carbon_huffman_tree_node *left;
    struct carbon_huffman_tree_node *right;
    size_t frequency;
    char   symbol;
} carbon_huffman_tree_node_t;

typedef struct carbon_huffman_encoder {
    size_t frequencies[UCHAR_MAX];
    carbon_huffman_dictionary_t codes;
} carbon_huffman_encoder_t;

typedef struct carbon_huffman_decoder {
    carbon_huffman_dictionary_t codes;
    carbon_huffman_tree_node_t *tree;
} carbon_huffman_decoder_t;

void carbon_huffman_bitstream_create(carbon_huffman_bitstream_t *stream);

void carbon_huffman_bitstream_write(carbon_huffman_bitstream_t *stream, bool bit);

void carbon_huffman_bitstream_concat(carbon_huffman_bitstream_t *stream, carbon_huffman_bitstream_t *other);

void carbon_huffman_bitstream_dump(carbon_huffman_bitstream_t *stream);

void carbon_huffman_bitstream_drop(carbon_huffman_bitstream_t *stream);

void carbon_huffman_encoder_create(carbon_huffman_encoder_t *encoder);

void carbon_huffman_encoder_learn_frequencies(carbon_huffman_encoder_t *encoder, char const *data);

void carbon_huffman_encoder_bake_code(carbon_huffman_encoder_t *encoder);

void carbon_huffman_encoder_drop(carbon_huffman_encoder_t *encoder);

void carbon_huffman_adaptive_update(carbon_huffman_encoder_t *encoder, char const **entries, size_t num_entries, size_t fade);

void carbon_huffman_create_all_eq_encoder(carbon_huffman_encoder_t *encoder);


void carbon_huffman_decoder_create(carbon_huffman_decoder_t *decoder, carbon_huffman_dictionary_t dictionary);

void carbon_huffman_decoder_drop(carbon_huffman_decoder_t *decoder);

void carbon_huffman_encode(carbon_huffman_encoder_t *encoder, carbon_huffman_bitstream_t *stream, char *data);

char *carbon_huffman_decode(carbon_huffman_decoder_t *decoder, carbon_huffman_bitstream_t *stream);

#endif