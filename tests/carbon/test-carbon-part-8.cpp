#include <gtest/gtest.h>
#include <printf.h>

#include <jakson/jakson.h>
#include <fcntl.h>

TEST(CarbonTest, CarbonFromEmptyJson)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "{}";
        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);
        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);

        //printf("INS:\t%s\n", json_in);
        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "{}") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromEmptyArray)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "[]";
        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);
        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);

//        printf("INS:\t%s\n", json_in);
//        printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, "[]") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonNull)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "null";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);
        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be 'null'
        
        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "null") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonTrue)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "true";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be 'true'
        
        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "true") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonFalse)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "false";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be 'false'

        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "false") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonNumberSigned)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "42";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);
        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '42'

        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "42") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonNumberUnsigned)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "-42";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '-42'

        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "-42") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonNumberFloat)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "-42.23";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '-42.23'

        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "-42.23") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonString)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "\"Hello, World!\"";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '"Hello, World!"'

        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "\"Hello, World!\"") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonObjectSingle)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "{\"k\":\"v\"}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '{"k":"v"}'

        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "{\"k\":\"v\"}") == 0);

        str_buf_drop(&sb1);
}


TEST(CarbonTest, CarbonFromJsonObjectEmptyArrayProp)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "{\"k\":[]}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '{"k":[]}'

        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "{\"k\":[]}") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonObjectEmptyObjectProp)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "{\"k\":{}}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '{"k":{}}'

        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "{\"k\":{}}") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonObjectTrueProp)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "{\"k\":true}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '{"k":true}'

        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "{\"k\":true}") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonObjectFalseProp)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "{\"k\":false}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '{"k":false}'

        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "{\"k\":false}") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonObjectNullProp)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "{\"k\":null}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '{"k":null}'

        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "{\"k\":null}") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonObjectUnsignedProp)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "{\"k\":42}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '{"k":42}'

        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "{\"k\":42}") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonObjectSignedProp)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "{\"k\":-42}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '{"k":-42}'

        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "{\"k\":-42}") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonObjectFloatProp)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "{\"k\":-42.23}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '{"k":-42.23}'

        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "{\"k\":-42.23}") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonColumnNumber)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "{\"x\":[1, 2, 3]}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        arr_it it;
        field_e field_type;
        rec_read_begin(&it, &doc);
        ASSERT_TRUE(arr_it_next(&it));
        arr_it_field_type(&field_type, &it);
        ASSERT_TRUE(field_type == FIELD_OBJECT_UNSORTED_MULTIMAP);
        obj_it *oit = item_get_object(&(it.item));
        ASSERT_TRUE(obj_it_next(oit));
        internal_obj_it_prop_type(&field_type, oit);
        ASSERT_TRUE(field_is_column_or_subtype(field_type));
        ASSERT_TRUE(field_type == FIELD_COLUMN_U8_UNSORTED_MULTISET);
        obj_it_drop(oit);
        rec_read_end(&it);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '[1, 2, 3]'

        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "{\"x\":[1, 2, 3]}") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonColumnNullableNumber)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "{\"x\":[1, null, 3]}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        arr_it it;
        field_e field_type;
        rec_read_begin(&it, &doc);
        ASSERT_TRUE(arr_it_next(&it));
        arr_it_field_type(&field_type, &it);
        ASSERT_TRUE(field_type == FIELD_OBJECT_UNSORTED_MULTIMAP);
        obj_it *oit = item_get_object(&(it.item));
        ASSERT_TRUE(obj_it_next(oit));
        internal_obj_it_prop_type(&field_type, oit);
        ASSERT_TRUE(field_is_column_or_subtype(field_type));
        ASSERT_TRUE(field_type == FIELD_COLUMN_U8_UNSORTED_MULTISET);
        obj_it_drop(oit);
        rec_read_end(&it);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '{"x":[1, null, 3]}'

        //printf("INS:\t%s\n", json_in);
        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "{\"x\":[1, null, 3]}") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonNonColumn)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "[1, null, 3, \"a\"]";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        arr_it it;
        field_e field_type;
        rec_read_begin(&it, &doc);
        ASSERT_TRUE(arr_it_next(&it));
        arr_it_field_type(&field_type, &it);
        ASSERT_TRUE(field_is_number(field_type));
        rec_read_end(&it);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '[1, null, 3, \"a\"]'

        //printf("INS:\t%s\n", json_in);
        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);
        ASSERT_TRUE(strcmp(json_out, "[1, null, 3, \"a\"]") == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonColumnOptimizeFix)
{
        rec_new context;
        rec doc;
        col_state state_out;

        insert *ins = rec_create_begin(&context, &doc, KEY_NOKEY, OPTIMIZE);
        insert *cins = insert_column_begin(&state_out, ins, COLUMN_U8, 4);
        insert_u8(cins, 3);
        insert_u8(cins, 4);
        insert_u8(cins, 5);
        insert_column_end(&state_out);
        rec_create_end(&context);

        str_buf sb1;
        str_buf_create(&sb1);
        const char *json = rec_to_json(&sb1, &doc);
        ASSERT_TRUE(strcmp(json, "[3, 4, 5]") == 0);
        rec_drop(&doc);
        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonExample)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        /* example json taken from 'https://json.org/example.html' */
        json_in = "{\"web-app\":{\"servlet\":[{\"servlet-name\":\"cofaxCDS\", \"servlet-class\":\"org.cofax.cds.CDSServlet\", \"init-param\":{\"configGlossary: installationAt\":\"Philadelphia, PA\", \"configGlossary: adminEmail\":\"ksm@pobox.com\", \"configGlossary: poweredBy\":\"Cofax\", \"configGlossary: poweredByIcon\":\"/images/cofax.gif\", \"configGlossary: staticPath\":\"/content/static\", \"templateProcessorClass\":\"org.cofax.WysiwygTemplate\", \"templateLoaderClass\":\"org.cofax.FilesTemplateLoader\", \"templatePath\":\"templates\", \"templateOverridePath\":\"\", \"defaultListTemplate\":\"listTemplate.htm\", \"defaultFileTemplate\":\"articleTemplate.htm\", \"useJSP\":false, \"jspListTemplate\":\"listTemplate.jsp\", \"jspFileTemplate\":\"articleTemplate.jsp\", \"cachePackageTagsTrack\":200, \"cachePackageTagsStore\":200, \"cachePackageTagsRefresh\":60, \"cacheTemplatesTrack\":100, \"cacheTemplatesStore\":50, \"cacheTemplatesRefresh\":15, \"cachePagesTrack\":200, \"cachePagesStore\":100, \"cachePagesRefresh\":10, \"cachePagesDirtyRead\":10, \"searchEngineListTemplate\":\"forSearchEnginesList.htm\", \"searchEngineFileTemplate\":\"forSearchEngines.htm\", \"searchEngineRobotsDb\":\"WEB-INF/robots.db\", \"useDataStore\":true, \"dataStoreClass\":\"org.cofax.SqlDataStore\", \"redirectionClass\":\"org.cofax.SqlRedirection\", \"dataStoreName\":\"cofax\", \"dataStoreDriver\":\"com.microsoft.jdbc.sqlserver.SQLServerDriver\", \"dataStoreUrl\":\"jdbc: microsoft: sqlserver: //LOCALHOST: 1433;DatabaseName=goon\", \"dataStoreUser\":\"sa\", \"dataStorePassword\":\"dataStoreTestQuery\", \"dataStoreTestQuery\":\"SET NOCOUNT ON;select test='test';\", \"dataStoreLogFile\":\"/usr/local/tomcat/logs/datastore.log\", \"dataStoreInitConns\":10, \"dataStoreMaxConns\":100, \"dataStoreConnUsageLimit\":100, \"dataStoreLogLevel\":\"debug\", \"maxUrlLength\":500}}, {\"servlet-name\":\"cofaxEmail\", \"servlet-class\":\"org.cofax.cds.EmailServlet\", \"init-param\":{\"mailHost\":\"mail1\", \"mailHostOverride\":\"mail2\"}}, {\"servlet-name\":\"cofaxAdmin\", \"servlet-class\":\"org.cofax.cds.AdminServlet\"}, {\"servlet-name\":\"fileServlet\", \"servlet-class\":\"org.cofax.cds.FileServlet\"}, {\"servlet-name\":\"cofaxTools\", \"servlet-class\":\"org.cofax.cms.CofaxToolsServlet\", \"init-param\":{\"templatePath\":\"toolstemplates/\", \"log\":1, \"logLocation\":\"/usr/local/tomcat/logs/CofaxTools.log\", \"logMaxSize\":\"\", \"dataLog\":1, \"dataLogLocation\":\"/usr/local/tomcat/logs/dataLog.log\", \"dataLogMaxSize\":\"\", \"removePageCache\":\"/content/admin/remove?cache=pages&id=\", \"removeTemplateCache\":\"/content/admin/remove?cache=templates&id=\", \"fileTransferFolder\":\"/usr/local/tomcat/webapps/content/fileTransferFolder\", \"lookInContext\":1, \"adminGroupID\":4, \"betaServer\":true}}], \"servlet-mapping\":{\"cofaxCDS\":\"/\", \"cofaxEmail\":\"/cofaxutil/aemail/*\", \"cofaxAdmin\":\"/admin/*\", \"fileServlet\":\"/static/*\", \"cofaxTools\":\"/tools/*\"}, \"taglib\":{\"taglib-uri\":\"cofax.tld\", \"taglib-location\":\"/WEB-INF/tlds/cofax.tld\"}}}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        u32 max = 10000;
        timestamp t1 = wallclock();
        for (u32 i = 0; i < max; i++) {
                rec d;
                rec_from_json(&d, json_in, KEY_NOKEY, NULL);
                rec_drop(&d);
        }
        timestamp t2 = wallclock();
        printf("%.2fmsec/opp, %.4f ops/sec\n", (t2-t1)/(float)max, 1.0f/((t2-t1)/(float)max/1000.0f));


        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '[1, null, 3, \"a\"]'

        //printf("INS:\t%s\n", json_in);
        //printf("SRT:\t%s\n", json_out);

        //rec_hexdump_print(stdout, &doc);
        //u64 carbon_len = 0;
        //rec_raw_data(&carbon_len, &doc);
        //printf("INS len: %zu\n", strlen(json_in));
        //printf("SRT len: %zu\n", carbon_len);
        //printf("%0.2f%% space saving\n", 100 * (1 - (carbon_len / (float) strlen(json_in))));

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonUnitArrayPrimitive)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "{\"x\":[1]}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '{"x":[1]}'

        //printf("INS:\t%s\n", json_in);
        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonUnitArrayObject)
{
        rec doc;

        const char *json_in;
        const char *json_out;

        json_in = "{\"x\":[{\"y\":1}]}";

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        str_buf sb1;
        str_buf_create(&sb1);
        json_out = rec_to_json(&sb1, &doc);    // shall be '{"x":[{"y":1}]}'

        //printf("INS:\t%s\n", json_in);
        //printf("SRT:\t%s\n", json_out);

        rec_drop(&doc);

        ASSERT_TRUE(strcmp(json_out, json_in) == 0);

        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonFromJsonSimpleExample)
{
        rec doc;

        const char *json_in = "{\"k\":{\"x\":[1,2,3], \"y\":\"z\"}}";
        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);
        //rec_hexdump_print(stdout, &doc);
        //rec_print(stdout, &doc);
        rec_drop(&doc);
}

TEST(CarbonTest, CarbonFromJsonFromExcerpt)
{
        rec doc;

        /* the working directory must be 'tests/carbon' */
        int fd = open("./assets/ms-academic-graph.json", O_RDONLY);
        ASSERT_NE(fd, -1);
        u64 json_in_len = lseek(fd, 0, SEEK_END);
        const char *json_in = (const char *) mmap(0, json_in_len, PROT_READ, MAP_PRIVATE, fd, 0);

        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        u64 carbon_out_len = 0;
        rec_raw_data(&carbon_out_len, &doc);

        ASSERT_LT(carbon_out_len, json_in_len);
        //printf("%0.2f%% space saving\n", 100 * (1 - (carbon_out_len / (float) json_in_len)));

        str_buf sb1;
        str_buf_create(&sb1);
        const char *json_out = rec_to_json(&sb1, &doc);
        ASSERT_TRUE(strcmp(json_in, json_out) == 0);

        rec_drop(&doc);
        str_buf_drop(&sb1);
}

TEST(CarbonTest, CarbonResolveDotPathForObjects)
{
        rec doc;
        find find;
        field_e result_type;
        u64 number;

        const char *json_in = "{\"a\":1, \"b\":{\"c\":[1,2,3], \"d\":[\"Hello\", \"World\"], \"e\":[4], \"f\":[\"!\"], \"the key\":\"x\"}}";
        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        ASSERT_TRUE(find_begin(&find, "0", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_OBJECT_UNSORTED_MULTIMAP);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "1", &doc));
        ASSERT_FALSE(find_has_result(&find));
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.a", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_NUMBER_U8);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_OBJECT_UNSORTED_MULTIMAP);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.c", &doc));
        ASSERT_FALSE(find_has_result(&find));
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.c", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_COLUMN_U8_UNSORTED_MULTISET);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.c.0", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_NUMBER_U8);
        ASSERT_TRUE(find_result_unsigned(&number, &find));
        ASSERT_EQ(number, 1U);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.c.1", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_NUMBER_U8);
        ASSERT_TRUE(find_result_unsigned(&number, &find));
        ASSERT_EQ(number, 2U);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.c.2", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_NUMBER_U8);
        ASSERT_TRUE(find_result_unsigned(&number, &find));
        ASSERT_EQ(number, 3U);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.c.3", &doc));
        ASSERT_FALSE(find_has_result(&find));
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.d", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_ARRAY_UNSORTED_MULTISET);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.d.0", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_STRING);
        ASSERT_TRUE(strncmp(find_result_string(&number, &find), "Hello", number) == 0);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.d.1", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_STRING);
        ASSERT_TRUE(strncmp(find_result_string(&number, &find), "World", number) == 0);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.d.2", &doc));
        ASSERT_FALSE(find_has_result(&find));
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.e", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_COLUMN_U8_UNSORTED_MULTISET);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.e.0", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_NUMBER_U8);
        ASSERT_TRUE(find_result_unsigned(&number, &find));
        ASSERT_EQ(number, 4U);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.e.1", &doc));
        ASSERT_FALSE(find_has_result(&find));
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.f", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_ARRAY_UNSORTED_MULTISET);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.f.0", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_STRING);

        ASSERT_TRUE(strncmp(find_result_string(&number, &find), "!", number) == 0);
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.f.1", &doc));
        ASSERT_FALSE(find_has_result(&find));
        ASSERT_TRUE(find_end(&find));

        ASSERT_TRUE(find_begin(&find, "0.b.\"the key\"", &doc));
        ASSERT_TRUE(find_has_result(&find));
        ASSERT_TRUE(find_result_type(&result_type, &find));
        ASSERT_EQ(result_type, FIELD_STRING);
        ASSERT_TRUE(strncmp(find_result_string(&number, &find), "x", number) == 0);
        ASSERT_TRUE(find_end(&find));

        rec_drop(&doc);
}

TEST(CarbonTest, CarbonResolveDotPathForObjectsBench)
{
        rec doc;
        find find;

        const char *json_in = "{\"a\":1, \"b\":{\"c\":[1,2,3], \"d\":[\"Hello\", \"World\"], \"e\":[4], \"f\":[\"!\"], \"the key\":\"x\"}, \"array\":[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 420, 421, 422, 423, 424, 425, 426, 427, 428, 429, 430, 431, 432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 446, 447, 448, 449, 450, 451, 452, 453, 454, 455, 456, 457, 458, 459, 460, 461, 462, 463, 464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479, 480, 481, 482, 483, 484, 485, 486, 487, 488, 489, 490, 491, 492, 493, 494, 495, 496, 497, 498, 499, 500, 501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511, 512, 513, 514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524, 525, 526, 527, 528, 529, 530, 531, 532, 533, 534, 535, 536, 537, 538, 539, 540, 541, 542, 543, 544, 545, 546, 547, 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 558, 559, 560, 561, 562, 563, 564, 565, 566, 567, 568, 569, 570, 571, 572, 573, 574, 575, 576, 577, 578, 579, 580, 581, 582, 583, 584, 585, 586, 587, 588, 589, 590, 591, 592, 593, 594, 595, 596, 597, 598, 599, 600, 601, 602, 603, 604, 605, 606, 607, 608, 609, 610, 611, 612, 613, 614, 615, 616, 617, 618, 619, 620, 621, 622, 623, 624, 625, 626, 627, 628, 629, 630, 631, 632, 633, 634, 635, 636, 637, 638, 639, 640, 641, 642, 643, 644, 645, 646, 647, 648, 649, 650, 651, 652, 653, 654, 655, 656, 657, 658, 659, 660, 661, 662, 663, 664, 665, 666, 667, 668, 669, 670, 671, 672, 673, 674, 675, 676, 677, 678, 679, 680, 681, 682, 683, 684, 685, 686, 687, 688, 689, 690, 691, 692, 693, 694, 695, 696, 697, 698, 699, 700, 701, 702, 703, 704, 705, 706, 707, 708, 709, 710, 711, 712, 713, 714, 715, 716, 717, 718, 719, 720, 721, 722, 723, 724, 725, 726, 727, 728, 729, 730, 731, 732, 733, 734, 735, 736, 737, 738, 739, 740, 741, 742, 743, 744, 745, 746, 747, 748, 749, 750, 751, 752, 753, 754, 755, 756, 757, 758, 759, 760, 761, 762, 763, 764, 765, 766, 767, 768, 769, 770, 771, 772, 773, 774, 775, 776, 777, 778, 779, 780, 781, 782, 783, 784, 785, 786, 787, 788, 789, 790, 791, 792, 793, 794, 795, 796, 797, 798, 799, 800, 801, 802, 803, 804, 805, 806, 807, 808, 809, 810, 811, 812, 813, 814, 815, 816, 817, 818, 819, 820, 821, 822, 823, 824, 825, 826, 827, 828, 829, 830, 831, 832, 833, 834, 835, 836, 837, 838, 839, 840, 841, 842, 843, 844, 845, 846, 847, 848, 849, 850, 851, 852, 853, 854, 855, 856, 857, 858, 859, 860, 861, 862, 863, 864, 865, 866, 867, 868, 869, 870, 871, 872, 873, 874, 875, 876, 877, 878, 879, 880, 881, 882, 883, 884, 885, 886, 887, 888, 889, 890, 891, 892, 893, 894, 895, 896, 897, 898, 899, 900, 901, 902, 903, 904, 905, 906, 907, 908, 909, 910, 911, 912, 913, 914, 915, 916, 917, 918, 919, 920, 921, 922, 923, 924, 925, 926, 927, 928, 929, 930, 931, 932, 933, 934, 935, 936, 937, 938, 939, 940, 941, 942, 943, 944, 945, 946, 947, 948, 949, 950, 951, 952, 953, 954, 955, 956, 957, 958, 959, 960, 961, 962, 963, 964, 965, 966, 967, 968, 969, 970, 971, 972, 973, 974, 975, 976, 977, 978, 979, 980, 981, 982, 983, 984, 985, 986, 987, 988, 989, 990, 991, 992, 993, 994, 995, 996, 997, 998, 999]}";
        rec_from_json(&doc, json_in, KEY_NOKEY, NULL);

        dot path1, path2, path3, path4, path5, path6, path7, path8, path9, path10, path11, path12,
                path13, path14, path15, path16, path17, path18, path19, path20, path21, path22,
                path23, path24, path25;

        ASSERT_TRUE(dot_from_string(&path1, "0"));
        ASSERT_TRUE(dot_from_string(&path2, "1"));
        ASSERT_TRUE(dot_from_string(&path3, "0.a"));
        ASSERT_TRUE(dot_from_string(&path4, "0.b"));
        ASSERT_TRUE(dot_from_string(&path5, "0.c"));
        ASSERT_TRUE(dot_from_string(&path6, "0.b.c"));
        ASSERT_TRUE(dot_from_string(&path7, "0.b.c.0"));
        ASSERT_TRUE(dot_from_string(&path8, "0.b.c.1"));
        ASSERT_TRUE(dot_from_string(&path9, "0.b.c.2"));
        ASSERT_TRUE(dot_from_string(&path10, "0.b.c.3"));
        ASSERT_TRUE(dot_from_string(&path11, "0.b.d"));
        ASSERT_TRUE(dot_from_string(&path12, "0.b.d.0"));
        ASSERT_TRUE(dot_from_string(&path13, "0.b.d.1"));
        ASSERT_TRUE(dot_from_string(&path14, "0.b.d.2"));
        ASSERT_TRUE(dot_from_string(&path15, "0.b.e"));
        ASSERT_TRUE(dot_from_string(&path16, "0.b.e.0"));
        ASSERT_TRUE(dot_from_string(&path17, "0.b.e.1"));
        ASSERT_TRUE(dot_from_string(&path18, "0.b.f"));
        ASSERT_TRUE(dot_from_string(&path19, "0.b.f.0"));
        ASSERT_TRUE(dot_from_string(&path20, "0.b.f.1"));
        ASSERT_TRUE(dot_from_string(&path21, "0.b.\"the key\""));

        ASSERT_TRUE(dot_from_string(&path22, "0.array.0"));
        ASSERT_TRUE(dot_from_string(&path23, "0.array.999"));
        ASSERT_TRUE(dot_from_string(&path24, "0.array.250"));
        ASSERT_TRUE(dot_from_string(&path25, "0.array.750"));

        printf("ops/sec\n");
        for (int j = 0; j < 1; j++) {
                u32 max = 1000;
                timestamp t1 = wallclock();
                for (u32 i = 0; i < max; i++) {
                        ASSERT_TRUE(find_exec(&find, &path1, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path2, &doc));
                        ASSERT_FALSE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path3, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path4, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path5, &doc));
                        ASSERT_FALSE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path6, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path7, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path8, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path9, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path10, &doc));
                        ASSERT_FALSE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path11, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path12, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path13, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path14, &doc));
                        ASSERT_FALSE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path15, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path16, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path17, &doc));
                        ASSERT_FALSE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path18, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path19, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path20, &doc));
                        ASSERT_FALSE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path21, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path22, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path23, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path24, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));

                        ASSERT_TRUE(find_exec(&find, &path25, &doc));
                        ASSERT_TRUE(find_has_result(&find));
                        ASSERT_TRUE(find_end(&find));
                }
                timestamp t2 = wallclock();
                printf("%.4f\n", 1.0f/((t2-t1)/(float)max/(26*1000.0f)));
                fflush(stdout);
        }


        dot_drop(&path1);
        dot_drop(&path2);
        dot_drop(&path3);
        dot_drop(&path4);
        dot_drop(&path5);
        dot_drop(&path6);
        dot_drop(&path7);
        dot_drop(&path8);
        dot_drop(&path9);
        dot_drop(&path10);
        dot_drop(&path11);
        dot_drop(&path12);
        dot_drop(&path13);
        dot_drop(&path14);
        dot_drop(&path15);
        dot_drop(&path16);
        dot_drop(&path17);
        dot_drop(&path18);
        dot_drop(&path19);
        dot_drop(&path20);
        dot_drop(&path21);

        dot_drop(&path22);
        dot_drop(&path23);
        dot_drop(&path24);
        dot_drop(&path25);

        rec_drop(&doc);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}