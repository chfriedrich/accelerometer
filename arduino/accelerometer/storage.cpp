
#include "storage.h"

storage g_data;

void storage::init(uint32_t buffersize_byte)
{
    data = (uint64_t*)ps_malloc(buffersize_byte);
    data_len = buffersize_byte >> 3;  // division by 8
    clear();
}

void storage::clear()
{
    status = 0;
    wp = 0;
    storage_full = 0;
    nr_recordings = 0;
    for(int i=0; i<MAXIMUM_RECORDINGS; i++)
    {
        start_idx[i] = -1;
        rec_len[i]   = -1;
    }
}

void storage::addSingle(int32_t x, int32_t y, int32_t z)
{
    uint64_t x1 = (int64_t)x + (1<<19);
    uint64_t y1 = (int64_t)y + (1<<19);
    uint64_t z1 = (int64_t)z + (1<<19);
    uint64_t entry = (z1<<40) + (y1<<20) + (x1);
    data[wp] = entry;
    wp++;
}

void storage::addMultiple(int32_t x[], int32_t y[], int32_t z[], int32_t len)
{
    for(int i=0; i<len; i++)
    {
        addSingle(x[i], y[i], z[i]);
    }
}

void storage::start_record()
{
    start_idx[nr_recordings] = wp;
    Serial.printf("Record started. wp = %d, nr_recordings = %d\n", wp, nr_recordings);
    Serial.printf("start_idx = %d, %d, %d\n", start_idx[0], start_idx[1], start_idx[2]);
    Serial.printf("rec_len   = %d, %d, %d\n", rec_len[0], rec_len[1], rec_len[2]);
}

void storage::end_record()
{
    rec_len[nr_recordings] = wp - start_idx[nr_recordings];
    nr_recordings++;
    Serial.printf("Record stopped. wp = %d, nr_recordings = %d\n", wp, nr_recordings);
    Serial.printf("start_idx = %d, %d, %d\n", start_idx[0], start_idx[1], start_idx[2]);
    Serial.printf("rec_len   = %d, %d, %d\n", rec_len[0], rec_len[1], rec_len[2]);
}

int32_t storage::get_nr_of_recordings()
{
    return nr_recordings;  
}

void storage::get_record(int32_t nr, int32_t &startidx, int32_t &len)
{
    startidx = start_idx[nr];
    len = rec_len[nr];
    Serial.printf("Asked for recording nr %d, start index %d, length %d.\n", nr, startidx, len);
}

uint64_t storage::get_entry(int32_t idx)
{
    if(idx < data_len)
        return data[idx];
    else   
        return 0;
}

















