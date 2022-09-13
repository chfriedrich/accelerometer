
#ifndef storage_h
#define storage_h

#include <Arduino.h>

#define MAXIMUM_RECORDINGS 1024

class storage
{
  public:
	storage() {};
	~storage() {};

	void init(uint32_t buffersize_byte);
	void addSingle(int32_t x, int32_t y, int32_t z);
	void addMultiple(int32_t x[], int32_t y[], int32_t z[], int32_t len);
	int32_t get_nr_of_recordings();
	void get_record(int32_t nr, int32_t &startidx, int32_t &len);
	void clear();
	void start_record();
	void end_record();
	uint64_t get_entry(int32_t idx);

  private:
	int32_t wp;
	uint64_t *data;
	uint32_t data_len;  // number of 64 bit entries which can be stored in data

	int32_t nr_recordings = 0;
	int32_t start_idx[MAXIMUM_RECORDINGS];
	int32_t rec_len[MAXIMUM_RECORDINGS];

	uint8_t status = 0;  //  0...idle, 1...recording
	uint8_t storage_full;
};

extern storage g_data;

#endif