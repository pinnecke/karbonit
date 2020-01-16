/*
 * Copyright 2019 Marcus Pinnecke
 */

#include <karbonit/carbon/binary.h>
#include <libs/libb64/libb64.h>

#define code_of(x, data_len)      (x + data_len + 2)
#define data_of(x)      (x)

void binary_field_print(str_buf *dst, const binary_field *field)
{
        str_buf_add(dst, "{");
        str_buf_add(dst, "\"type\":\"");
        str_buf_add_nchar(dst, field->mime, field->mime_len);
        str_buf_add(dst, "\", \"encoding\":\"base64\", \"value\":\"");
        binary_field_value_print(dst, field);
        str_buf_add(dst, "\"}");
}

void binary_field_value_print(str_buf *dst, const binary_field *field)
{
        base64_encodestate state;
        base64_init_encodestate(&state);

        size_t required_buff_size = 3 * (field->blob_len + 1);
        char *buffer = malloc(required_buff_size);
        ZERO_MEMORY(buffer, required_buff_size);
        memcpy(buffer, field->blob, field->blob_len);

        u64 code_len = base64_encode_block(buffer, field->blob_len + 2,
                                           code_of(buffer, field->blob_len), &state);
        base64_encode_blockend(code_of(buffer, field->blob_len), &state);
        str_buf_add_nchar(dst, code_of(buffer, field->blob_len), code_len);
        free(buffer);
}