#include <iostream>
#include <fcntl.h>
#include <karbonit/karbonit.h>
#include <karbonit/json/json-parser.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

//TODO handle empty files


bool json_parse_file(const char* src, bool parse_line_by_line, const char* destdir, const char* filename, size_t num_threads, size_t num_parts)
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
            status = json_parse_split_parallel(json_in, size, num_threads, num_parts);
        }
        else
        {
            status = json_parse_split(json_in, size, destdir, filename);
        }
    }
    else
    {
        rec doc;
        if (destdir == NULL)
        {
            /*if (rec_from_json(&doc, json_in, KEY_NOKEY, NULL))
            {
                rec_drop(&doc);
            }*/

            struct json data;
            json_err err;
            json_parser parser;

            json_parse(&data, &err, &parser, json_in);
            //FILE* datei = fopen("/home/steven/Schreibtisch/Check.json", "w");
            //json_print(datei, &data);
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


bool json_parse_directory(const char* src, DIR* srcdir, bool parse_line_by_line, const char* destdir, size_t num_threads, size_t num_parts)
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

                json_parse_file(filepath, parse_line_by_line, destdir, buf.data, num_threads, num_parts);
            }
        }
        entry = readdir(srcdir);
    }

    return status;
}

void checkResult()
{
    size_t max = 93672;
    size_t p[] = {4745, 4653, 4731, 4669, 4741, 4772, 4711, 4647
                , 4660, 4584, 4758, 4718, 4716, 4609, 4564, 4759
                , 4715, 4531, 4675, 4714};

    FILE* orig;
    FILE* curr;

    size_t i = 1, j = 1, k = 1;

    char filepathorig[100];
    char filepathtest[100];
    char a, b;

    while (j <= max)
    {
        //open orig-file
        snprintf(filepathorig, 100, "/home/steven/Schreibtisch/test/orig/parsed_file%zu.carbon", j);
        orig = fopen(filepathorig, "r");

        //open test-file
        snprintf(filepathtest, 100, "/home/steven/Schreibtisch/test/%zu/parsed_file%zu.carbon", k, i);
        curr = fopen(filepathtest, "r");

        if(orig == NULL)
        {
            std::cout << "Fehler orig existiert nicht " << j << std::endl;
            goto next;
        }

        if(curr == NULL)
        {
            std::cout << "Fehler test existiert nicht " << k << " " << i << std::endl;
            fclose(orig);
            goto next;
        }

        //compare them
        a = fgetc(orig);
        b = fgetc(curr);

        while(a != EOF && b != EOF)
        {
            if(a != b)
            {
                std::cout << "Fehler Inhalt unterschiedlich " << j << std::endl;
                break;
            }
            a = fgetc(orig);
            b = fgetc(curr);
        }

        if((a == EOF && b != EOF) || (a != EOF && b == EOF))
        {
            std::cout << "Fehler unterschiedliche Laenge " << j << std::endl;
        }

        //close files
        fclose(orig);
        fclose(curr);

        next:
        //next file
        j++;
        i++;
        if(i > p[k-1])
        {
            i=1;
            k++;
        }
    }
}

void clearFolders()
{
    size_t max = 93672;
    size_t p[] = {4745, 4653, 4731, 4669, 4741, 4772, 4711, 4647
            , 4660, 4584, 4758, 4718, 4716, 4609, 4564, 4759
            , 4715, 4531, 4675, 4714};

    size_t i = 1, j = 1, k = 1;

    char filepathtest[100];

    while (j <= max)
    {
        //remove test-file
        snprintf(filepathtest, 100, "/home/steven/Schreibtisch/test/%zu/parsed_file%zu.carbon", k, i);
        remove(filepathtest);

        //next file
        j++;
        i++;
        if(i > p[k-1])
        {
            i=1;
            k++;
        }
    }
}


int main(int argc, char **argv) {
    //credit http://man7.org/linux/man-pages/man3/getopt.3.html 01.02.2020 14:49

    //bench-json-fast-parse <command> [<args>]
    //commands:
    //-d                    -> directory-path expected, parse every file in given directory
    //-l                    -> parse file line by line, one line = one record
    //-t <num_threads>      -> requires -l; number of possible threads
    //-p <num_parts>        -> requires -l; number of parts the file should be split into
    //-s <directory-path>   -> save parse results as carbon files

    //args:
    //- file-path of file to be parsed (or directory-path if -d)

    int opt;

    bool parse_directory = false;
    bool parse_line_by_line = false;
    bool save_results = false;
    size_t num_parts = 1;
    size_t num_threads = 1;

    const char *src = NULL;
    const char *dest = NULL;

    DIR* srcdir = NULL;
    DIR* destdir = NULL;

    if (argc < 2) {
        std::cout << "At least one argument expected!" << std::endl;
        return 1;
    }

    while ((opt = getopt(argc, argv, "dls:p:t:")) != -1) {
        switch (opt) {
            case 'd':
                parse_directory = true;
                std::cout << "Parse Directory" << std::endl;
                break;
            case 'l':
                std::cout << "Parse LBL" << std::endl;
                parse_line_by_line = true;
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



    if(parse_directory)
    {
        json_parse_directory(src, srcdir, parse_line_by_line, dest, num_threads, num_parts);
        closedir(srcdir);
    }
    else
    {
        /*int t = 1000;
        int n = 1;
        int w = 0;

        //8 CPUs
        //1-8 Threads echt parallel
        //9-16 Threads Hyperthreading
        //17-24 Threads ausserhalb
        //#Parts = #Threads

        std::cout << "Phase 1" << std::endl;
        while (t < 101)
        {
            std::cout << t << " Threads" << std::endl;
            w = 0;

            //je 5 wiederholungen
            while (w < 10)
            {
                timestamp start = wallclock();
                json_parse_file(src, parse_line_by_line, dest, "parsed_file", t, t);
                timestamp end = wallclock();

                std::cout << (end - start) << std::endl;
                w++;
            }
            t++;
        }*/

        /*int t = 2;
        int n = 2;
        int w = 0;

        std::cout << "Phase 2" << std::endl;
        while (t < 21)
        {
            std::cout << t << " Threads" << std::endl;
            n = t;
            while (n == t)
            {
                std::cout << n << " Parts" << std::endl;
                w = 0;
                //je 5 wiederholungen
                while (w < 5) {
                    timestamp start = wallclock();
                    json_parse_file(src, parse_line_by_line, dest, "a", t, n);
                    timestamp end = wallclock();

                    std::cout << (end - start) << std::endl;
                    w++;
                }
                n++;
            }
            t++;
        }*/

        int w = 0;

        while (w < 1)
        {
            w++;
            //clearFolders();
            timestamp start = wallclock();
            json_parse_file(src, parse_line_by_line, dest, "parsed_file", num_threads, num_parts);
            timestamp end = wallclock();
            //checkResult();

            std::cout << (end - start) << std::endl;
        }
    }

    return 0;
}