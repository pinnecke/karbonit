#include <iostream>
#include <fcntl.h>
#include <karbonit/karbonit.h>
#include <karbonit/json/json-parser.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

bool json_parse_file(const char* src, bool parse_line_by_line, const char* destdir, const char* filename)
{
    bool status = false;

    struct stat st;
    stat(src, &st);
    size_t size = st.st_size;

    int fd = open(src, O_RDONLY);
    const char *json_in = (const char *) mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);

    if(parse_line_by_line)
    {
        status = json_parse_split(json_in, destdir, filename);
    }
    else
    {
        rec doc;
        if (destdir == NULL)
        {
            if (rec_from_json(&doc, json_in, KEY_NOKEY, NULL))
            {
                rec_drop(&doc);
            }
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


bool json_parse_directory(const char* src, DIR* srcdir, bool parse_line_by_line, const char* destdir)
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

                json_parse_file(filepath, parse_line_by_line, destdir, buf.data);
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
    //-s <directory-path>   -> save parse results as carbon files

    //args:
    //- file-path of file to be parsed (or directory-path if -d)


    int opt;

    bool parse_directory = false;
    bool parse_line_by_line = false;
    bool save_results = false;

    const char *src;
    const char *dest;

    DIR* srcdir = NULL;
    DIR* destdir = NULL;

    if (argc < 2) {
        std::cout << "At least one argument expected!" << std::endl;
        return 1;
    }

    while ((opt = getopt(argc, argv, "dls:")) != -1) {
        switch (opt) {
            case 'd':
                parse_directory = true;
                std::cout << "Parse Directory" << std::endl;
                break;
            case 'l':
                std::cout << "Parse LBL" << std::endl;
                parse_line_by_line = true;
                break;
            case 's':
                save_results = true;
                dest = optarg;
                std::cout << "Save Results at " << dest << std::endl;
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


    timestamp start = wallclock();

    if(parse_directory)
    {
        json_parse_directory(src, srcdir, parse_line_by_line, dest);
        closedir(srcdir);
    }
    else
    {
        json_parse_file(src, parse_line_by_line, dest, "parsed_file");
    }
    timestamp end = wallclock();

    std::cout << "Parsen benoetigte "<< (end-start) << std::endl;

    return 0;
}