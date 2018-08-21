#pragma GCC optimize ("-O3")
#include <Arduino.h>
#include <avr/pgmspace.h>
#include "music.h"

#define TON_TABLE_ID     99
#define SAMPLES_OFFSET   105
#define ORNAMENTS_OFFSET 169
#define POSITIONS_OFFSET 201

// Table #0 of Pro Tracker 3.3x - 3.4r
static const uint16_t pt3_note_table_pt_33_34r[] PROGMEM = {
  0x0C21,0x0B73,0x0ACE,0x0A33,0x09A0,0x0916,0x0893,0x0818,0x07A4,0x0736,0x06CE,0x066D,
  0x0610,0x05B9,0x0567,0x0519,0x04D0,0x048B,0x0449,0x040C,0x03D2,0x039B,0x0367,0x0336,
  0x0308,0x02DC,0x02B3,0x028C,0x0268,0x0245,0x0224,0x0206,0x01E9,0x01CD,0x01B3,0x019B,
  0x0184,0x016E,0x0159,0x0146,0x0134,0x0122,0x0112,0x0103,0x00F4,0x00E6,0x00D9,0x00CD,
  0x00C2,0x00B7,0x00AC,0x00A3,0x009A,0x0091,0x0089,0x0081,0x007A,0x0073,0x006C,0x0066,
  0x0061,0x005B,0x0056,0x0051,0x004D,0x0048,0x0044,0x0040,0x003D,0x0039,0x0036,0x0033,
  0x0030,0x002D,0x002B,0x0028,0x0026,0x0024,0x0022,0x0020,0x001E,0x001C,0x001B,0x0019,
  0x0018,0x0016,0x0015,0x0014,0x0013,0x0012,0x0011,0x0010,0x000F,0x000E,0x000D,0x000C};

// Table #0 of Pro Tracker 3.4x - 3.5x
static const uint16_t pt3_note_table_pt_34_35[] PROGMEM = {
  0x0C22,0x0B73,0x0ACF,0x0A33,0x09A1,0x0917,0x0894,0x0819,0x07A4,0x0737,0x06CF,0x066D,
  0x0611,0x05BA,0x0567,0x051A,0x04D0,0x048B,0x044A,0x040C,0x03D2,0x039B,0x0367,0x0337,
  0x0308,0x02DD,0x02B4,0x028D,0x0268,0x0246,0x0225,0x0206,0x01E9,0x01CE,0x01B4,0x019B,
  0x0184,0x016E,0x015A,0x0146,0x0134,0x0123,0x0112,0x0103,0x00F5,0x00E7,0x00DA,0x00CE,
  0x00C2,0x00B7,0x00AD,0x00A3,0x009A,0x0091,0x0089,0x0082,0x007A,0x0073,0x006D,0x0067,
  0x0061,0x005C,0x0056,0x0052,0x004D,0x0049,0x0045,0x0041,0x003D,0x003A,0x0036,0x0033,
  0x0031,0x002E,0x002B,0x0029,0x0027,0x0024,0x0022,0x0020,0x001F,0x001D,0x001B,0x001A,
  0x0018,0x0017,0x0016,0x0014,0x0013,0x0012,0x0011,0x0010,0x000F,0x000E,0x000D,0x000C};

// Table #1 of Pro Tracker 3.3x - 3.5x)
static const uint16_t pt3_note_table_st[] PROGMEM = {
  0x0EF8,0x0E10,0x0D60,0x0C80,0x0BD8,0x0B28,0x0A88,0x09F0,0x0960,0x08E0,0x0858,0x07E0,
  0x077C,0x0708,0x06B0,0x0640,0x05EC,0x0594,0x0544,0x04F8,0x04B0,0x0470,0x042C,0x03FD,
  0x03BE,0x0384,0x0358,0x0320,0x02F6,0x02CA,0x02A2,0x027C,0x0258,0x0238,0x0216,0x01F8,
  0x01DF,0x01C2,0x01AC,0x0190,0x017B,0x0165,0x0151,0x013E,0x012C,0x011C,0x010A,0x00FC,
  0x00EF,0x00E1,0x00D6,0x00C8,0x00BD,0x00B2,0x00A8,0x009F,0x0096,0x008E,0x0085,0x007E,
  0x0077,0x0070,0x006B,0x0064,0x005E,0x0059,0x0054,0x004F,0x004B,0x0047,0x0042,0x003F,
  0x003B,0x0038,0x0035,0x0032,0x002F,0x002C,0x002A,0x0027,0x0025,0x0023,0x0021,0x001F,
  0x001D,0x001C,0x001A,0x0019,0x0017,0x0016,0x0015,0x0013,0x0012,0x0011,0x0010,0x000F};

// Table #2 of Pro Tracker 3.4r
static const uint16_t pt3_note_table_asm_34r[] PROGMEM = {
  0x0D3E,0x0C80,0x0BCC,0x0B22,0x0A82,0x09EC,0x095C,0x08D6,0x0858,0x07E0,0x076E,0x0704,
  0x069F,0x0640,0x05E6,0x0591,0x0541,0x04F6,0x04AE,0x046B,0x042C,0x03F0,0x03B7,0x0382,
  0x034F,0x0320,0x02F3,0x02C8,0x02A1,0x027B,0x0257,0x0236,0x0216,0x01F8,0x01DC,0x01C1,
  0x01A8,0x0190,0x0179,0x0164,0x0150,0x013D,0x012C,0x011B,0x010B,0x00FC,0x00EE,0x00E0,
  0x00D4,0x00C8,0x00BD,0x00B2,0x00A8,0x009F,0x0096,0x008D,0x0085,0x007E,0x0077,0x0070,
  0x006A,0x0064,0x005E,0x0059,0x0054,0x0050,0x004B,0x0047,0x0043,0x003F,0x003C,0x0038,
  0x0035,0x0032,0x002F,0x002D,0x002A,0x0028,0x0026,0x0024,0x0022,0x0020,0x001E,0x001D,
  0x001B,0x001A,0x0019,0x0018,0x0015,0x0014,0x0013,0x0012,0x0011,0x0010,0x000F,0x000E};

// Table #2 of Pro Tracker 3.4x - 3.5x
static const uint16_t pt3_note_table_asm_34_35[] PROGMEM = {
  0x0D10,0x0C55,0x0BA4,0x0AFC,0x0A5F,0x09CA,0x093D,0x08B8,0x083B,0x07C5,0x0755,0x06EC,
  0x0688,0x062A,0x05D2,0x057E,0x052F,0x04E5,0x049E,0x045C,0x041D,0x03E2,0x03AB,0x0376,
  0x0344,0x0315,0x02E9,0x02BF,0x0298,0x0272,0x024F,0x022E,0x020F,0x01F1,0x01D5,0x01BB,
  0x01A2,0x018B,0x0174,0x0160,0x014C,0x0139,0x0128,0x0117,0x0107,0x00F9,0x00EB,0x00DD,
  0x00D1,0x00C5,0x00BA,0x00B0,0x00A6,0x009D,0x0094,0x008C,0x0084,0x007C,0x0075,0x006F,
  0x0069,0x0063,0x005D,0x0058,0x0053,0x004E,0x004A,0x0046,0x0042,0x003E,0x003B,0x0037,
  0x0034,0x0031,0x002F,0x002C,0x0029,0x0027,0x0025,0x0023,0x0021,0x001F,0x001D,0x001C,
  0x001A,0x0019,0x0017,0x0016,0x0015,0x0014,0x0012,0x0011,0x0010,0x000F,0x000E,0x000D};

// Table #3 of Pro Tracker 3.4r
static const uint16_t pt3_note_table_real_34r[] PROGMEM = {
  0x0CDA,0x0C22,0x0B73,0x0ACF,0x0A33,0x09A1,0x0917,0x0894,0x0819,0x07A4,0x0737,0x06CF,
  0x066D,0x0611,0x05BA,0x0567,0x051A,0x04D0,0x048B,0x044A,0x040C,0x03D2,0x039B,0x0367,
  0x0337,0x0308,0x02DD,0x02B4,0x028D,0x0268,0x0246,0x0225,0x0206,0x01E9,0x01CE,0x01B4,
  0x019B,0x0184,0x016E,0x015A,0x0146,0x0134,0x0123,0x0113,0x0103,0x00F5,0x00E7,0x00DA,
  0x00CE,0x00C2,0x00B7,0x00AD,0x00A3,0x009A,0x0091,0x0089,0x0082,0x007A,0x0073,0x006D,
  0x0067,0x0061,0x005C,0x0056,0x0052,0x004D,0x0049,0x0045,0x0041,0x003D,0x003A,0x0036,
  0x0033,0x0031,0x002E,0x002B,0x0029,0x0027,0x0024,0x0022,0x0020,0x001F,0x001D,0x001B,
  0x001A,0x0018,0x0017,0x0016,0x0014,0x0013,0x0012,0x0011,0x0010,0x000F,0x000E,0x000D};

// Table #3 of Pro Tracker 3.4x - 3.5x
static const uint16_t pt3_note_table_real_34_35[] PROGMEM = {
  0x0CDA,0x0C22,0x0B73,0x0ACF,0x0A33,0x09A1,0x0917,0x0894,0x0819,0x07A4,0x0737,0x06CF,
  0x066D,0x0611,0x05BA,0x0567,0x051A,0x04D0,0x048B,0x044A,0x040C,0x03D2,0x039B,0x0367,
  0x0337,0x0308,0x02DD,0x02B4,0x028D,0x0268,0x0246,0x0225,0x0206,0x01E9,0x01CE,0x01B4,
  0x019B,0x0184,0x016E,0x015A,0x0146,0x0134,0x0123,0x0112,0x0103,0x00F5,0x00E7,0x00DA,
  0x00CE,0x00C2,0x00B7,0x00AD,0x00A3,0x009A,0x0091,0x0089,0x0082,0x007A,0x0073,0x006D,
  0x0067,0x0061,0x005C,0x0056,0x0052,0x004D,0x0049,0x0045,0x0041,0x003D,0x003A,0x0036,
  0x0033,0x0031,0x002E,0x002B,0x0029,0x0027,0x0024,0x0022,0x0020,0x001F,0x001D,0x001B,
  0x001A,0x0018,0x0017,0x0016,0x0014,0x0013,0x0012,0x0011,0x0010,0x000F,0x000E,0x000D};

// Volume table of Pro Tracker 3.3x - 3.4x
static const uint16_t pt3_volume_table_33_34[16][16] PROGMEM = {
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01},
  {0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,0x02},
  {0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03},
  {0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x02,0x02,0x02,0x03,0x03,0x03,0x04,0x04,0x04},
  {0x00,0x00,0x00,0x01,0x01,0x01,0x02,0x02,0x03,0x03,0x03,0x04,0x04,0x04,0x05,0x05},
  {0x00,0x00,0x00,0x01,0x01,0x02,0x02,0x03,0x03,0x03,0x04,0x04,0x05,0x05,0x06,0x06},
  {0x00,0x00,0x01,0x01,0x02,0x02,0x03,0x03,0x04,0x04,0x05,0x05,0x06,0x06,0x07,0x07},
  {0x00,0x00,0x01,0x01,0x02,0x02,0x03,0x03,0x04,0x05,0x05,0x06,0x06,0x07,0x07,0x08},
  {0x00,0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x05,0x05,0x06,0x06,0x07,0x08,0x08,0x09},
  {0x00,0x00,0x01,0x02,0x02,0x03,0x04,0x04,0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x0A},
  {0x00,0x00,0x01,0x02,0x03,0x03,0x04,0x05,0x06,0x06,0x07,0x08,0x09,0x09,0x0A,0x0B},
  {0x00,0x00,0x01,0x02,0x03,0x04,0x04,0x05,0x06,0x07,0x08,0x08,0x09,0x0A,0x0B,0x0C},
  {0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D},
  {0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E},
  {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F}};

// Volume table of Pro Tracker 3.5x
static const uint16_t pt3_volume_table_35[16][16] PROGMEM = {
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01},
  {0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02},
  {0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,0x02,0x03,0x03,0x03},
  {0x00,0x00,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x04,0x04},
  {0x00,0x00,0x01,0x01,0x01,0x02,0x02,0x02,0x03,0x03,0x03,0x04,0x04,0x04,0x05,0x05},
  {0x00,0x00,0x01,0x01,0x02,0x02,0x02,0x03,0x03,0x04,0x04,0x04,0x05,0x05,0x06,0x06},
  {0x00,0x00,0x01,0x01,0x02,0x02,0x03,0x03,0x04,0x04,0x05,0x05,0x06,0x06,0x07,0x07},
  {0x00,0x01,0x01,0x02,0x02,0x03,0x03,0x04,0x04,0x05,0x05,0x06,0x06,0x07,0x07,0x08},
  {0x00,0x01,0x01,0x02,0x02,0x03,0x04,0x04,0x05,0x05,0x06,0x07,0x07,0x08,0x08,0x09},
  {0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x05,0x05,0x06,0x07,0x07,0x08,0x09,0x09,0x0A},
  {0x00,0x01,0x01,0x02,0x03,0x04,0x04,0x05,0x06,0x07,0x07,0x08,0x09,0x0A,0x0A,0x0B},
  {0x00,0x01,0x02,0x02,0x03,0x04,0x05,0x06,0x06,0x07,0x08,0x09,0x0A,0x0A,0x0B,0x0C},
  {0x00,0x01,0x02,0x03,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0A,0x0B,0x0C,0x0D},
  {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E},
  {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F}};

struct Channel_Parameters
{
    uint16_t address_in_pattern;
    uint16_t ornament_pointer;
    uint16_t sample_pointer;
    uint16_t ton;

    uint8_t loop_ornament_position;
    uint8_t ornament_length;
    uint8_t position_in_ornament;
    uint8_t loop_sample_position;
    uint8_t sample_length;
    uint8_t position_in_sample;
    uint8_t volume;
    uint8_t number_of_notes_to_skip;
    uint8_t note;
    uint8_t slide_to_note;
    uint8_t amplitude;

    bool envelope_enabled;
    bool enabled;
    bool simple_gliss;

    int16_t current_amplitude_sliding;
    int16_t ton_slide_count;
    int16_t current_on_off;
    int16_t on_off_delay;
    int16_t off_on_delay;
    int16_t ton_slide_delay;
    int16_t current_ton_sliding;
    int16_t ton_accumulator;
    int16_t ton_slide_step;
    int16_t ton_delta;

    int8_t note_skip_counter;

    uint8_t current_noise_sliding;
    uint8_t current_envelope_sliding;
};

static Channel_Parameters channels[3];

static uint16_t env_base;
static int16_t cur_env_slide;
static int16_t env_slide_add;
static int8_t cur_env_delay;
static int8_t env_delay;
static uint8_t noise_base;
static uint8_t delay_value;
static uint8_t add_to_noise;
static uint8_t delay_counter;
static uint8_t current_position;

static int8_t add_to_env;
static uint8_t temp_mixer;

static uint8_t pt3_get_version()
{
    return music_peek(13) - '0';
}

static uint16_t pt3_patterns_offset()
{
    return music_peek2(103);
}

static uint16_t pt3_get_ornament_ptr(uint8_t i)
{
    return music_peek2(ORNAMENTS_OFFSET + i * 2);
}

static uint16_t pt3_get_sample_ptr(uint8_t i)
{
    return music_peek2(SAMPLES_OFFSET + i * 2);
}

bool pt3_init(const uint8_t *pt3)
{
    if (pgm_read_byte(pt3) != 'P'
        || pgm_read_byte(pt3 + 1) != 'r'
        || pgm_read_byte(pt3 + 2) != 'o'
        || pgm_read_byte(pt3 + 3) != 'T'
        /* Skipped some letters */
        || pgm_read_byte(pt3 + 11) != '3')
    {
        return false;
    }

    music_set_ram(pt3, 0);

    delay_counter = 1;
    delay_value = music_peek(100);

    for (int i = 0 ; i < 3 ; ++i)
    {
        uint8_t pos = music_peek(POSITIONS_OFFSET);
        channels[i].address_in_pattern =
            music_peek2(pt3_patterns_offset() + pos * 2 + i * 2);

        channels[i].ornament_pointer = pt3_get_ornament_ptr(0);
        channels[i].loop_ornament_position = music_peek(channels[i].ornament_pointer++);
        channels[i].ornament_length = music_peek(channels[i].ornament_pointer++);
        channels[i].sample_pointer = pt3_get_sample_ptr(1);
        channels[i].loop_sample_position = music_peek(channels[i].sample_pointer++);
        channels[i].sample_length = music_peek(channels[i].sample_pointer++);

        channels[i].volume = 15;
        channels[i].note_skip_counter = 1;
    }

    music_clear_ay();

    return true;
}

static uint16_t get_note_freq(uint8_t note)
{
    switch (music_peek(TON_TABLE_ID))
    {
    case 0:
        if (pt3_get_version() <= 3)
            return pgm_read_word(&pt3_note_table_pt_33_34r[note]);
        else
            return pgm_read_word(&pt3_note_table_pt_34_35[note]);
    case 1:
        return pgm_read_word(&pt3_note_table_st[note]);
    case 2:
        if (pt3_get_version() <= 3)
            return pgm_read_word(&pt3_note_table_asm_34r[note]);
        else
            return pgm_read_word(&pt3_note_table_asm_34_35[note]);
    default:
        if (pt3_get_version() <= 3)
            return pgm_read_word(&pt3_note_table_real_34r[note]);
        else
            return pgm_read_word(&pt3_note_table_real_34_35[note]);
    }
}

static void pt3_play_channel(Channel_Parameters *ch)
{
    int prnote = ch->note;
    int prsliding = ch->current_ton_sliding;
    bool quit = false;
    uint8_t counter = 0;
    uint8_t flag9 = 0, flag8 = 0, flag5 = 0, flag4 = 0;
    uint8_t flag3 = 0, flag2 = 0, flag1 = 0;

    do
    {
        uint8_t val = music_peek(ch->address_in_pattern);
        switch (val)
        {
        case 0xf0 ... 0xff:
            ch->ornament_pointer = pt3_get_ornament_ptr(val - 0xf0);
            ch->loop_ornament_position = music_peek(ch->ornament_pointer++);
            ch->ornament_length = music_peek(ch->ornament_pointer++);
            ++ch->address_in_pattern;
            ch->sample_pointer = pt3_get_sample_ptr(music_peek(ch->address_in_pattern) / 2);
            ch->loop_sample_position = music_peek(ch->sample_pointer++);
            ch->sample_length = music_peek(ch->sample_pointer++);
            ch->envelope_enabled = false;
            ch->position_in_ornament = 0;
            break;
        case 0xd1 ... 0xef:
            ch->sample_pointer = pt3_get_sample_ptr(val - 0xd0);
            ch->loop_sample_position = music_peek(ch->sample_pointer++);
            ch->sample_length = music_peek(ch->sample_pointer++);
            break;
        case 0xd0:
            quit = true;
            break;
        case 0xc1 ... 0xcf:
            ch->volume = val - 0xc0;
            break;
        case 0xc0:
            ch->position_in_sample = 0;
            ch->current_amplitude_sliding = 0;
            ch->current_noise_sliding = 0;
            ch->current_envelope_sliding = 0;
            ch->position_in_ornament = 0;
            ch->ton_slide_count = 0;
            ch->current_ton_sliding = 0;
            ch->ton_accumulator = 0;
            ch->current_on_off = 0;
            ch->enabled = false;
            quit = true;
            break;
        case 0xb2 ... 0xbf:
            ch->envelope_enabled = true;
            music_set_ay_reg(AY_ENVT, val - 0xb1);
            env_base = music_peek(++ch->address_in_pattern);
            env_base = (env_base << 8) + music_peek(++ch->address_in_pattern);
            ch->position_in_ornament = 0;
            cur_env_slide = 0;
            cur_env_delay = 0;
            break;
        case 0xb1:
            ch->number_of_notes_to_skip = music_peek(++ch->address_in_pattern);
            break;
        case 0xb0:
            ch->envelope_enabled = false;
            ch->position_in_ornament = 0;
            break;
        case 0x50 ... 0xaf:
            ch->note = val - 0x50;
            ch->position_in_sample = 0;
            ch->current_amplitude_sliding = 0;
            ch->current_noise_sliding = 0;
            ch->current_envelope_sliding = 0;
            ch->position_in_ornament = 0;
            ch->ton_slide_count = 0;
            ch->current_ton_sliding = 0;
            ch->ton_accumulator = 0;
            ch->current_on_off = 0;
            ch->enabled = true;
            quit = true;
            break;
        case 0x40 ... 0x4f:
            ch->ornament_pointer = pt3_get_ornament_ptr(val - 0x40);
            ch->loop_sample_position = music_peek(ch->ornament_pointer++);
            ch->ornament_length = music_peek(ch->ornament_pointer++);
            ch->position_in_ornament = 0;
            break;
        case 0x20 ... 0x3f:
            noise_base = val - 0x20;
            break;
        case 0x10 ... 0x1f:
            if (val == 0x10)
            {
                ch->envelope_enabled = false;
            }
            else
            {
                music_set_ay_reg(AY_ENVT, val - 0x10);
                env_base = music_peek(++ch->address_in_pattern);
                env_base = (env_base << 8) + music_peek(++ch->address_in_pattern);
                ch->envelope_enabled = true;
                cur_env_slide = 0;
                cur_env_delay = 0;
            }
            ch->sample_pointer = pt3_get_sample_ptr(music_peek(++ch->address_in_pattern) / 2);
            ch->loop_sample_position = music_peek(ch->sample_pointer++);
            ch->sample_length = music_peek(ch->sample_pointer++);
            ch->position_in_ornament = 0;
            break;
        case 0x9:
            flag9 = ++counter;
            break;
        case 0x8:
            flag8 = ++counter;
            break;
        case 0x5:
            flag5 = ++counter;
            break;
        case 0x4:
            flag4 = ++counter;
            break;
        case 0x3:
            flag3 = ++counter;
            break;
        case 0x2:
            flag2 = ++counter;
            break;
        case 0x1:
            flag1 = ++counter;
            break;
        }
        ++ch->address_in_pattern;
    }
    while (!quit);

    while (counter > 0)
    {
        if (counter == flag1)
        {
            ch->ton_slide_delay = music_peek(ch->address_in_pattern++);
            ch->ton_slide_count = ch->ton_slide_delay;
            ch->ton_slide_step = music_peek2(ch->address_in_pattern);
            ch->address_in_pattern += 2;
            ch->simple_gliss = true;
            ch->current_on_off = 0;
            if (ch->ton_slide_count == 0 && pt3_get_version() >= 7)
                ++ch->ton_slide_count;
        }
        else if (counter == flag2)
        {
            ch->simple_gliss = false;
            ch->current_on_off = 0;
            ch->ton_slide_delay = music_peek(ch->address_in_pattern);
            ch->ton_slide_count = ch->ton_slide_delay;
            ch->address_in_pattern += 3;
            ch->ton_slide_step = music_peek2(ch->address_in_pattern);
            if (ch->ton_slide_step < 0)
                ch->ton_slide_step = -ch->ton_slide_step;
            ch->address_in_pattern += 2;
            ch->ton_delta = get_note_freq(ch->note) - get_note_freq(prnote);
            ch->slide_to_note = ch->note;
            ch->note = prnote;
            if (pt3_get_version() >= 6)
                ch->current_ton_sliding = prsliding;
            if (ch->ton_delta - ch->current_ton_sliding < 0)
                ch->ton_slide_step = -ch->ton_slide_step;
        }
        else if (counter == flag3)
        {
            ch->position_in_sample = music_peek(ch->address_in_pattern++);
        }
        else if (counter == flag4)
        {
            ch->position_in_ornament = music_peek(ch->address_in_pattern++);
        }
        else if (counter == flag5)
        {
            ch->on_off_delay = music_peek(ch->address_in_pattern++);
            ch->off_on_delay = music_peek(ch->address_in_pattern++);
            ch->current_on_off = ch->on_off_delay;
            ch->ton_slide_count = 0;
            ch->current_ton_sliding = 0;
        }
        else if (counter == flag8)
        {
            env_delay = music_peek(ch->address_in_pattern++);
            cur_env_delay = env_delay;
            env_slide_add = music_peek2(ch->address_in_pattern);
            ch->address_in_pattern += 2;
        }
        else if (counter == flag9)
        {
            uint8_t b = music_peek(ch->address_in_pattern++);
            delay_value = b;
            //delay_counter = b;
        }
        --counter;
    }
    ch->note_skip_counter = ch->number_of_notes_to_skip;
}

static void pt3_change_registers(Channel_Parameters *ch)
{
    uint8_t j, b1, b0;
    uint16_t w;
    if (ch->enabled)
    {
        ch->ton = music_peek2(ch->sample_pointer + ch->position_in_sample * 4 + 2);
        ch->ton += ch->ton_accumulator;
        b0 = music_peek(ch->sample_pointer + ch->position_in_sample * 4);
        b1 = music_peek(ch->sample_pointer + ch->position_in_sample * 4 + 1);
        if (b1 & 0x40)
            ch->ton_accumulator = ch->ton;
        j = ch->note + music_peek(ch->ornament_pointer + ch->position_in_ornament);
        if ((int8_t)j < 0)
            j = 0;
        else if (j > 95)
            j = 95;
        w = get_note_freq(j);
        ch->ton = (ch->ton + ch->current_ton_sliding + w) & 0xfff;
        if (ch->ton_slide_count > 0)
        {
            if (!--ch->ton_slide_count)
            {
                ch->current_ton_sliding += ch->ton_slide_step;
                ch->ton_slide_count = ch->ton_slide_delay;
                if (!ch->simple_gliss)
                {
                    if ((ch->ton_slide_step < 0 && ch->current_ton_sliding <= ch->ton_delta)
                        || (ch->ton_slide_step >= 0 && ch->current_ton_sliding >= ch->ton_delta))
                    {
                        ch->note = ch->slide_to_note;
                        ch->ton_slide_count = 0;
                        ch->current_ton_sliding = 0;
                    }
                }
            }
        }
        ch->amplitude = b1 & 0xf;
        if (b0 & 0x80)
        {
            if (b0 & 0x40)
            {
                if (ch->current_amplitude_sliding < 15)
                {
                    ++ch->current_amplitude_sliding;
                }
            }
            else
            {
                if (ch->current_amplitude_sliding > -15)
                {
                    --ch->current_amplitude_sliding;
                }
            }
        }
        ch->amplitude += ch->current_amplitude_sliding;
        if ((int8_t)ch->amplitude < 0)
            ch->amplitude =  0;
        else if (ch->amplitude > 15)
            ch->amplitude = 15;
        
        if (pt3_get_version() <= 4)
            ch->amplitude = pgm_read_byte(&pt3_volume_table_33_34[ch->volume][ch->amplitude]);
        else
            ch->amplitude = pgm_read_byte(&pt3_volume_table_35[ch->volume][ch->amplitude]);

        if ((b0 & 1) == 0 && ch->envelope_enabled)
            ch->amplitude |= 16;
        if (b1 & 0x80)
        {
            if (b0 & 0x20)
            {
                j = ((b0 >> 1) | 0xF0) + ch->current_envelope_sliding;
            }
            else
            {
                j = ((b0 >> 1) & 0xf) + ch->current_envelope_sliding;
            }
            if (b1 & 0x20)
                ch->current_envelope_sliding = j;
            add_to_env += j;
        }
        else
        {
            add_to_noise = (b0 >> 1) + ch->current_noise_sliding;
            if (b1 & 0x20)
                ch->current_noise_sliding = add_to_noise;
        }
        temp_mixer |= (b1 >> 1) & 0x48;
        if (++ch->position_in_sample >= ch->sample_length)
            ch->position_in_sample = ch->loop_sample_position;
        if (++ch->position_in_ornament >= ch->ornament_length)
            ch->position_in_ornament = ch->loop_ornament_position;
    }
    else
    {
        ch->amplitude = 0;
    }
    temp_mixer >>= 1;
    if (ch->current_on_off > 0)
    {
        if (!--ch->current_on_off)
        {
            ch->enabled = !ch->enabled;
            if (ch->enabled)
            {
                ch->current_on_off = ch->on_off_delay;
            }
            else
            {
                ch->current_on_off = ch->off_on_delay;
            }
        }
    }
}

void pt3_loop()
{
    if (!--delay_counter)
    {
        if (!--channels[0].note_skip_counter)
        {
            if (!music_peek(channels[0].address_in_pattern))
            {
                ++current_position;
                uint8_t pos = music_peek(POSITIONS_OFFSET + current_position);
                if (pos == 0xff)
                {
                    current_position = 0;
                    pos = music_peek(POSITIONS_OFFSET);
                }
                channels[0].address_in_pattern =
                    music_peek2(pt3_patterns_offset() + pos * 2);
                channels[1].address_in_pattern =
                    music_peek2(pt3_patterns_offset() + pos * 2 + 2);
                channels[2].address_in_pattern =
                    music_peek2(pt3_patterns_offset() + pos * 2 + 4);
                noise_base = 0;
            }
            pt3_play_channel(&channels[0]);
        }
        if (!--channels[1].note_skip_counter)
        {
            pt3_play_channel(&channels[1]);
        }
        if (!--channels[2].note_skip_counter)
        {
            pt3_play_channel(&channels[2]);
        }
        delay_counter = delay_value;
    }

    add_to_env = 0;
    temp_mixer = 0;

    pt3_change_registers(&channels[0]);
    pt3_change_registers(&channels[1]);
    pt3_change_registers(&channels[2]);

    music_set_ay_reg(AY_MIXER, temp_mixer);

    for (int i = 0 ; i < 3 ; ++i)
    {
        music_set_ay_reg(AY_TONA + 2 * i, channels[i].ton);
        music_set_ay_reg(AY_TONA + 2 * i + 1, channels[i].ton >> 8);
        music_set_ay_reg(AY_AMPA + i, channels[i].amplitude);
    }

    music_set_ay_reg(AY_NOISE, (noise_base + add_to_noise) & 31);
    uint16_t env = env_base + add_to_env + cur_env_slide;
    music_set_ay_reg(AY_ENV, env);
    music_set_ay_reg(AY_ENV + 1, env >> 8);

    if (cur_env_delay > 0)
    {
        if (!--cur_env_delay)
        {
            cur_env_delay = env_delay;
            cur_env_slide += env_slide_add;
        }
    }

    music_send_ay();
}
