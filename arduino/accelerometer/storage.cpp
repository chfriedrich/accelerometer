
#include "storage.h"

void storage::init(uint32_t buffersize_byte)
{
    xdata = (int32_t*)ps_malloc(buffersize_byte);
    ydata = (int32_t*)ps_malloc(buffersize_byte);
    zdata = (int32_t*)ps_malloc(buffersize_byte);
    clear();
}

void storage::clear()
{
    status = 0;
    wp = 0;
    storage_full = 0;

    for(int i=0; i<MAXIMUM_RECORDINGS; i++)
    {
        start_idx[i] = -1;
        rec_len[i]   = -1;
    }
}

void storage::addSingle(int32_t x, int32_t y, int32_t z)
{
    xdata[wp] = x;
    ydata[wp] = y;
    zdata[wp] = z;
    wp++;
}

void storage::addMultiple(int32_t x[], int32_t y[], int32_t z[], int32_t len)
{
    for(int i=0; i<len; i++)
    {
        addSingle(x[i], y[i], z[i]);
    }
}