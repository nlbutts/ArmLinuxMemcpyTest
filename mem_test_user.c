#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <sys/mman.h>
#include <arm_neon.h>

static void * srcBuf;
static void * dstBuf;

static void * mmSrc;
static void * mmDst;

#define BUFFER_ADDR         0x38000000
#define BUFFER_SIZE         10*1024*1024

static void printPerformance(struct timespec * start, struct timespec * stop, char * msg)
{
    double diff;
    double throughput;
    double startf;
    double stopf;

    startf = start->tv_sec;
    startf += start->tv_nsec * 1E-9;
    stopf = stop->tv_sec;
    stopf += stop->tv_nsec * 1E-9;

    diff = stopf - startf;
    printf("%s = %f seconds\n", msg, diff);
    throughput = BUFFER_SIZE / diff;
    throughput /= 1000000;
    printf("Throughput = %f MB/sec\n", throughput);
}

/**
 * Helper function for allocating DMA memory. Returns 0 on success, -1 on failure
 */
static int requestDmaMem()
{
    srcBuf = NULL;
    dstBuf = NULL;
    int rv;

    //srcBuf = malloc(BUFFER_SIZE);
    rv = posix_memalign(&srcBuf, getpagesize(), BUFFER_SIZE);
    //srcBuf = malloc(BUFFER_SIZE);
    printf("posix_memalign rv=%d srcBuf = %08X\n", rv, (uint32_t)srcBuf);
    rv = posix_memalign(&dstBuf, getpagesize(), BUFFER_SIZE);
    //dstBuf = malloc(BUFFER_SIZE);
    printf("posix_memalign rv=%d dstBuf = %08X\n", rv, (uint32_t)dstBuf);

    mmSrc = mmap(srcBuf, BUFFER_SIZE, PROT_NONE, MAP_ANONYMOUS, 0, 0);
    mmDst = mmap(dstBuf, BUFFER_SIZE, PROT_NONE, MAP_ANONYMOUS, 0, 0);
    printf("mmSrc=%08X\n", (uint32_t) mmSrc);
    printf("mmDst=%08X\n", (uint32_t) mmDst);

    if ((srcBuf == NULL) | (dstBuf == NULL))
    {
        printf("Failed to allocate memory\n");
        return -1;
    }

    rv = mlock(srcBuf, BUFFER_SIZE);
    printf("Locking srcBuf rv=%d\n", rv);
    rv = mlock(dstBuf, BUFFER_SIZE);
    printf("Locking dstBuf rv=%d\n", rv);

    return 0;
}

#if 1
extern void memcpyf(void *dst, void *src, uint32_t size);
#else
void memcpyf(void *dst, void *src, uint32_t size)
{
    uint64_t * srcPtr = (uint64_t*)src;
    uint64_t * dstPtr = (uint64_t*)dst;
    uint64_t * endPtr = srcPtr + size/8;
    while (srcPtr < endPtr)
    {
        *(dstPtr++) = *(srcPtr++);
    }
}
#endif

static void performMemoryTest(void)
{
    struct timespec start;
    struct timespec stop;
    uint32_t * srcptr;
    uint32_t * dstptr;
    int i;

    clock_gettime(CLOCK_REALTIME, &start);
    memset(srcBuf, 0x55, BUFFER_SIZE);
    clock_gettime(CLOCK_REALTIME, &stop);
    printPerformance(&start, &stop, "Write performance to memory");

    clock_gettime(CLOCK_REALTIME, &start);
    memcpy(dstBuf, srcBuf, BUFFER_SIZE);
    clock_gettime(CLOCK_REALTIME, &stop);
    printPerformance(&start, &stop, "Copy performance from src to dst");

    clock_gettime(CLOCK_REALTIME, &start);
    memcpyf(dstBuf, srcBuf, BUFFER_SIZE);
    clock_gettime(CLOCK_REALTIME, &stop);
    printPerformance(&start, &stop, "Copy performance from src to dst");

    uint32_t * srcPtr = (uint32_t*)srcBuf;
    uint32_t * dstPtr = (uint32_t*)dstBuf;
    for (int i = 0; i < BUFFER_SIZE/4; i++)
    {
        srcPtr[i] = i;
    }
    memcpyf(dstBuf, srcBuf, BUFFER_SIZE);
    int compare = memcmp(srcBuf, dstBuf, BUFFER_SIZE);
    printf("Compare result=%d\n", compare);
}

/**
 * Helper function to free DMA memory
 */
static void freeDmaMem()
{
    if(srcBuf != NULL)
    {
        free(srcBuf);
    }
    srcBuf = NULL;

    if (dstBuf != NULL)
    {
        free(dstBuf);
    }
}

int main()
{
    printf("Memory test\r");

    if (requestDmaMem() >= 0)
    {
        performMemoryTest();
    }
    freeDmaMem();

}
