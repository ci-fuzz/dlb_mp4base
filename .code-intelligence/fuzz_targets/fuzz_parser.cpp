#include <stddef.h>
#include <stdint.h>

#include <parser.h>
#include <registry.h>

#include <test_util.h>

const char* write_to_file(const uint8_t *Data, size_t Size) {
   const char* filename = "/tmp/fuzz_input";
   FILE *fp;
   fp = fopen(filename, "w+");
   fwrite(Data, Size, 1, fp);
   fclose(fp);
   return filename;
}

static void test_parsing(parser_handle_t parser){
    if (parser->show_info != NULL)
    {
        parser->show_info(parser);
    }

    {
        mp4_sample_handle_t sample = sample_create();

        // assure( sample != NULL );
        // assure( parser->get_sample != NULL );

        {
            int number_of_samples = 0;
            int err = parser->get_sample(parser, sample);
            while (err == 0 && number_of_samples < 50)
            {
                number_of_samples += 1;

                // fails with H264 and AVC parser: assure( sample->data != NULL );

                err = parser->get_sample(parser, sample);
            }
        }

        sample->destroy(sample);
    }
}

bbio_handle_t bbio_from_file(const char *filename)
{
    bbio_handle_t mp4_src = reg_bbio_get('f', 'r');
    // assure(mp4_src != NULL);
    mp4_src->open(mp4_src, (const int8_t*)filename);

    return mp4_src;
}

static void test_parser(const char * filename, const int8_t * parser_type) {
    reg_parser_init();

    parser_aac_reg();
    parser_ac3_reg();
    parser_ec3_reg();
    //parser_mlp_reg();
    parser_avc_reg();

    // assure( reg_parser_get("non_existing", 0) == 0 );

    {
        uint32_t dsi_type = DSI_TYPE_MP4FF;
        parser_handle_t parser = reg_parser_get(parser_type, dsi_type);
        // assure( parser != NULL );

        {
            bbio_handle_t es;
            int err;
            const char *abs_path = filename;// string_cat(string_cat(SIGNALS_DIR, "/"), filename);

            
            es = bbio_from_file(abs_path);
    
            {
                uint32_t es_idx = 0;
                ext_timing_info_t timing_info = {1, 0, 1};
                err = parser->init(parser, &timing_info, es_idx, es);
            }
            if (err == 0)
            {
                test_parsing(parser);
            }
            else
            {
                // printf("%s parser could not parse %s\n", parser_type, abs_path);
            }
            es->close(es);
        }
        parser->destroy(parser);
    }
}

// extern "C" int FUZZ_INIT_WITH_ARGS(int *argc, char ***argv) {
extern "C" int FUZZ_INIT() {

  reg_bbio_init();
  bbio_file_reg();
  bbio_buf_reg();

  return 0;
}

extern "C" int FUZZ(const uint8_t *Data, size_t Size) {
  auto file = write_to_file(Data, Size);

  test_parser(file, (const int8_t *)"aac");
  test_parser(file, (const int8_t *)"ac3");
  test_parser(file, (const int8_t *)"ec3");
  test_parser(file, (const int8_t *)"avc");

  return 0;
}
