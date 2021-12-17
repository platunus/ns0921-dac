#define NS0921_REG	0

#define NS0921_LRCK_DISABLE	0b00000000
#define NS0921_LRCK_ENABLE	0b00000001

#define NS0921_BCK_DISABLE	0b00000000
#define NS0921_BCK_ENABLE	0b00000010

#define NS0921_BCK_DIV_2	0b00000000
#define NS0921_BCK_DIV_4	0b00000100
#define NS0921_BCK_DIV_8	0b00001000
#define NS0921_BCK_DIV_16	0b00001100
#define NS0921_BCK_DIV_32	0b00010000

#define NS0921_LRCK_DIV_32	0b00000000
#define NS0921_LRCK_DIV_59	0b00100000
#define NS0921_LRCK_DIV_64	0b01000000
#define NS0921_LRCK_DIV_88	0b01100000

#define NS0921_MCLK_GND		0b00000000
#define NS0921_MCLK_PASS	0b10000000

/* codec mclk clock divider coefficients */
struct _coeff_div {
	int rate;
	int width;
	int bclk_ratio;
	u8 reg;
};


static const struct _coeff_div coeff_div[] = {
/*
	{ 44100, 16, 32, NS0921_BCK_DIV_16  | NS0921_LRCK_DIV_32},
	{ 88200, 16, 32, NS0921_BCK_DIV_8  | NS0921_LRCK_DIV_32},
	{176400, 16, 32, NS0921_BCK_DIV_4  | NS0921_LRCK_DIV_32},
*/
	{ 44100, 16, 64, NS0921_BCK_DIV_8  | NS0921_LRCK_DIV_64},
	{ 88200, 16, 64, NS0921_BCK_DIV_4  | NS0921_LRCK_DIV_64},
	{176400, 16, 64, NS0921_BCK_DIV_2  | NS0921_LRCK_DIV_64},
	{ 44100, 20, 64, NS0921_BCK_DIV_8  | NS0921_LRCK_DIV_64},
	{ 88200, 20, 64, NS0921_BCK_DIV_4  | NS0921_LRCK_DIV_64},
	{176400, 20, 64, NS0921_BCK_DIV_2  | NS0921_LRCK_DIV_64},
	{ 44100, 24, 64, NS0921_BCK_DIV_8  | NS0921_LRCK_DIV_64},
	{ 88200, 24, 64, NS0921_BCK_DIV_4  | NS0921_LRCK_DIV_64},
	{176400, 24, 64, NS0921_BCK_DIV_2  | NS0921_LRCK_DIV_64},
	{ 44100, 32, 64, NS0921_BCK_DIV_8  | NS0921_LRCK_DIV_64},
	{ 88200, 32, 64, NS0921_BCK_DIV_4  | NS0921_LRCK_DIV_64},
	{176400, 32, 64, NS0921_BCK_DIV_2  | NS0921_LRCK_DIV_64},

	{  8000, 16, 88, NS0921_BCK_DIV_32 | NS0921_LRCK_DIV_88},
	{ 16000, 16, 88, NS0921_BCK_DIV_16 | NS0921_LRCK_DIV_88},
	{ 32000, 16, 88, NS0921_BCK_DIV_8  | NS0921_LRCK_DIV_88},
	{  8000, 20, 88, NS0921_BCK_DIV_32 | NS0921_LRCK_DIV_88},
	{ 16000, 20, 88, NS0921_BCK_DIV_16 | NS0921_LRCK_DIV_88},
	{ 32000, 20, 88, NS0921_BCK_DIV_8  | NS0921_LRCK_DIV_88},
	{  8000, 24, 88, NS0921_BCK_DIV_32 | NS0921_LRCK_DIV_88},
	{ 16000, 24, 88, NS0921_BCK_DIV_16 | NS0921_LRCK_DIV_88},
	{ 32000, 24, 88, NS0921_BCK_DIV_8  | NS0921_LRCK_DIV_88},
	{  8000, 32, 88, NS0921_BCK_DIV_32 | NS0921_LRCK_DIV_88},
	{ 16000, 32, 88, NS0921_BCK_DIV_16 | NS0921_LRCK_DIV_88},
	{ 32000, 32, 88, NS0921_BCK_DIV_8  | NS0921_LRCK_DIV_88},

	{ 24000, 16, 59, NS0921_BCK_DIV_16 | NS0921_LRCK_DIV_59}, 
	{ 48000, 16, 59, NS0921_BCK_DIV_8  | NS0921_LRCK_DIV_59},
	{ 96000, 16, 59, NS0921_BCK_DIV_4  | NS0921_LRCK_DIV_59},
	{192000, 16, 59, NS0921_BCK_DIV_2  | NS0921_LRCK_DIV_59},
	{ 24000, 20, 59, NS0921_BCK_DIV_16 | NS0921_LRCK_DIV_59}, 
	{ 48000, 20, 59, NS0921_BCK_DIV_8  | NS0921_LRCK_DIV_59},
	{ 96000, 20, 59, NS0921_BCK_DIV_4  | NS0921_LRCK_DIV_59},
	{192000, 20, 59, NS0921_BCK_DIV_2  | NS0921_LRCK_DIV_59},
	{ 24000, 24, 59, NS0921_BCK_DIV_16 | NS0921_LRCK_DIV_59}, 
	{ 48000, 24, 59, NS0921_BCK_DIV_8  | NS0921_LRCK_DIV_59},
	{ 96000, 24, 59, NS0921_BCK_DIV_4  | NS0921_LRCK_DIV_59},
	{192000, 24, 59, NS0921_BCK_DIV_2  | NS0921_LRCK_DIV_59},
	{ 24000, 32, 59, NS0921_BCK_DIV_16 | NS0921_LRCK_DIV_59}, 
	{ 48000, 32, 59, NS0921_BCK_DIV_8  | NS0921_LRCK_DIV_59},
	{ 96000, 32, 59, NS0921_BCK_DIV_4  | NS0921_LRCK_DIV_59},
	{192000, 32, 59, NS0921_BCK_DIV_2  | NS0921_LRCK_DIV_59},
};

