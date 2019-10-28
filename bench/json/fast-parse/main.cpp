#include <iostream>
#include <fcntl.h>
#include <jakson/jakson.h>

int main(void) {

        /* the working directory must be 'tests/jakson-tool' to find this file */
        int fd = open("bench/json/fast-parse/assets/random.json", O_RDONLY);
        int json_in_len = lseek(fd, 0, SEEK_END);
        const char *json_in = (const char *) mmap(0, json_in_len, PROT_READ, MAP_PRIVATE, fd, 0);

        json_parser parser;
        json json;
        json_err error_desc;
        json_parser_create(&parser);
        bool status = json_parse(&json, &error_desc, &parser, json_in);

        std::cout << (status ? "Jupp" : "Nope") << std::endl;

        json_drop(&json);

        return 0;
}
