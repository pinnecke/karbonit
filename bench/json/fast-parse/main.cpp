#include <iostream>
#include <fcntl.h>
#include <karbonit/karbonit.h>
#include <karbonit/json/json-parser.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>


bool json_parse_file(const char* src, bool parse_line_by_line, bool parse_combined_tok_int, int statmode, const char* destdir, const char* filename, size_t num_threads, size_t num_parts)
{
    bool status = false;

    struct stat st;
    stat(src, &st);
    size_t size = st.st_size;

    int fd = open(src, O_RDONLY);
    const char *json_in = (const char *) mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);

    if(parse_line_by_line)
    {
        //if multiple threads are allowed and input should be split in multiple parts
        if((num_threads >= 1) && (num_parts > 1))
        {
            status = json_parse_split_parallel(json_in, size, num_threads, num_parts, parse_combined_tok_int, statmode);
        }
        else
        {
            //check if input should be parsed with combines interpreter and tokenizer
            if(parse_combined_tok_int)
            {
                status = json_parse_split_exp(json_in, size, destdir, filename, NULL);
            }
            else
            {
                status = json_parse_split(json_in, size, destdir, filename);
            }
        }
    }
    else
    {
        rec doc;
        if (destdir == NULL)
        {
            struct json data;
            json_err err;
            json_parser parser;

            //check if input should be parsed with combines interpreter and tokenizer
            if(parse_combined_tok_int)
            {
                json_parse_exp(&data, &err, &parser, json_in);
            }
            else
            {
                json_parse(&data, &err, &parser, json_in);
            }

            json_drop(&data);

        }
        else
        {
            rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

            size_t filepathsize = strlen(destdir) + strlen(filename) + 8;

            char filepath[filepathsize];
            snprintf(filepath, filepathsize, "%s%s%s", destdir, filename, ".carbon");

            FILE *file;
            file = fopen(filepath, "w");

            MEMFILE_SAVE_POSITION(&doc.file);
            MEMFILE_SEEK(&doc.file, 0);
            fwrite(MEMFILE_PEEK(&doc.file, 1), MEMFILE_SIZE(&doc.file), 1, file);
            MEMFILE_RESTORE_POSITION(&doc.file);

            fclose(file);
            rec_drop(&doc);
            remove(filepath);
        }
    }

    close(fd);

    return status;
}


bool json_parse_directory(const char* src, DIR* srcdir, bool parse_line_by_line, bool parse_combined_tok_int, int statmode, const char* destdir, size_t num_threads, size_t num_parts)
{
    bool status = false;

    struct dirent* entry = readdir(srcdir);
    while(entry != NULL)
    {
        if((*entry).d_type == DT_REG)
        {
            bool parseFile = false;

            //check for filetype .txt or .json
            str_buf buf;
            str_buf_create(&buf);
            size_t i = 0;

            while((*entry).d_name[i] != '\0')
            {
                str_buf_add_char(&buf, (*entry).d_name[i]);
                i++;
            }

            char* filetype;
            filetype = strstr(buf.data, ".txt");

            if((filetype != NULL) && (strlen(filetype) == 4))
            {
                parseFile = true;
            }
            else
            {
                filetype = strstr(buf.data, ".json");
                if((filetype != NULL) && (strlen(filetype) == 5))
                {
                    parseFile = true;
                }
            }

            if(parseFile)
            {
                size_t filepathsize = strlen(src) + buf.end + 7;
                char filepath[filepathsize];
                snprintf(filepath, filepathsize, "%s%s", src, buf.data);

                json_parse_file(filepath, parse_line_by_line, parse_combined_tok_int, statmode, destdir, buf.data, num_threads, num_parts);
            }
        }
        entry = readdir(srcdir);
    }

    return status;
}


int main(int argc, char **argv) {
    //credit http://man7.org/linux/man-pages/man3/getopt.3.html 01.02.2020 14:49

    //bench-json-fast-parse <command> [<args>]
    //commands:
    //-d                    -> directory-path expected, parse every file in given directory
    //-l                    -> parse file line by line, one line = one record
    //-e                    -> parse with combined tokenizer and interpreter
    //-t <num_threads>      -> requires -l; number of possible threads
    //-p <num_parts>        -> requires -l; number of parts the file should be split into
    //-s <directory-path>   -> save parse results as carbon files
    //-h <mode>             -> requires -e; use statistic in defined mode:
    //                          - mode = 1: build statistic before parsing the input
    //                          - mode = 2: build statistic while parsing the input as an additional task

    //args:
    //- file-path of file to be parsed (or directory-path if -d)

    int opt;

    bool parse_directory = false;
    bool parse_line_by_line = false;
    bool parse_combined_tok_int = false;
    bool save_results = false;
    size_t num_parts = 1;
    size_t num_threads = 1;
    int statmode = 0;

    const char *src = NULL;
    const char *dest = NULL;

    DIR* srcdir = NULL;
    DIR* destdir = NULL;

    if (argc < 2) {
        std::cout << "At least one argument expected!" << std::endl;
        return 1;
    }

    while ((opt = getopt(argc, argv, "dles:p:t:h:")) != -1) {
        switch (opt) {
            case 'd':
                parse_directory = true;
                break;
            case 'l':
                parse_line_by_line = true;
                break;
            case 'e':
                parse_combined_tok_int = true;
                break;
            case 'p':
                num_parts = atoi(optarg);
                break;
            case 't':
                num_threads = atoi(optarg);
                break;
            case 's':
                save_results = true;
                dest = optarg;
                break;
            case 'h':
                statmode = atoi(optarg);
                break;
            case '?':
                return 1;
            default:
                break;
        }
    }

    //error, if no arguments given
    if (optind >= argc) {
        std::cout << "At least one argument expected!" << std::endl;
        return 1;
    }

    src = *(argv + optind);
    optind++;

    //if a single file has to be parsed
    if (!parse_directory)
    {
        //open src and check if src exists
        FILE* srcfile = fopen(src, "r");
        if (srcfile == NULL)
        {
            std::cout << "\"" << src << "\" does not exist!" << std::endl;
            return 1;
        }
        fclose(srcfile);
    }
    else
    {
        //check if directory exists
        srcdir = opendir(src);
        if (srcdir == NULL)
        {
            std::cout << "\"" << src << "\" does not exist!" << std::endl;
            return 1;
        }
    }

    if (save_results)
    {
        //check if directory exists, to save results in
        destdir = opendir(dest);
        if (destdir == NULL)
        {
            std::cout << "\"" << src << "\" does not exist!" << std::endl;
            return 1;
        }
        closedir(destdir);
    }



    if(parse_directory)
    {
        json_parse_directory(src, srcdir, parse_line_by_line, parse_combined_tok_int, statmode, dest, num_threads, num_parts);
        closedir(srcdir);
    }
    else
    {
        json_parse_file(src, parse_line_by_line, parse_combined_tok_int, statmode, dest, "parsed_file", num_threads, num_parts);
    }

    return 0;
}