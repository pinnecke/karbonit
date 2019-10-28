#include <iostream>
#include <fcntl.h>
#include <jakson/jakson.h>

void run(const char *path, int sample)
{
        int fd = open(path, O_RDONLY);
        int json_in_len = lseek(fd, 0, SEEK_END);
        const char *json_in = (const char *) mmap(0, json_in_len, PROT_READ, MAP_PRIVATE, fd, 0);

        json_parser parser;
        json json;
        json_err error_desc;
        json_parser_create(&parser);

        timestamp start = wallclock();
        json_parse(&json, &error_desc, &parser, json_in);

        timestamp end = wallclock();

        std::cout << path << ";" << sample << ";" << (end - start) << std::endl;


        json_drop(&json);
       // close(fd);
}

void f(const char *g) {
        for (unsigned i = 0; i < 15; i++) {
                run(g, i);
        }
}

int main(void) {

        std::cout << "file;sample;read-time-ms"<< std::endl;
        f("bench/json/fast-parse/assets/mag-1.json");
        f("bench/json/fast-parse/assets/mag-2.json");
        f("bench/json/fast-parse/assets/mag-3.json");
        f("bench/json/fast-parse/assets/mag-4.json");


        return 0;
}
