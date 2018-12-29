const struct { const char name; const char * pin; } pin[]{
	{'0',"pin0"},
	{'1',"pin1"},
	{'2',"pin2"},
	{'3',"pin3"},
	{'4',"pin4"},
	{'5',"pin5"},
	{'6',"pin6"},
	{'7',"pin7"}
};

int main()
{
	uint8_t hour = 45;
	uint8_t check = 255;
	for (unsigned int i = 0; i < 8; i++) {
		std::cout << ((hour & check) >> 7);
		hour = hour << 1;
	}
}
