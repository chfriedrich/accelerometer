
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

  private:
    int32_t wp;
    int32_t *xdata;
    int32_t *ydata;
    int32_t *zdata;
    int32_t start_idx[MAXIMUM_RECORDINGS];
    int32_t rec_len[MAXIMUM_RECORDINGS];
    uint8_t status = 0;  //  0...idle, 1...recording
    uint8_t storage_full;

    void clear();
};

#endif