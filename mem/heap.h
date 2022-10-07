/**======================================================================================
|
|  文件说明：该文件提供内存分配函数的API
|  作者：SillyMem  (1508555728@qq.com)
|  时间：2020.05.08
|  版本：v2.02
|**====================================================================================**/
#ifndef  __RTS_HEAP1_H__
#define  __RTS_HEAP1_H__

#include  "mem_cfg.h"
#include "datatype.h"


#ifdef __cplusplus
extern "C"{
#endif


#if  MEM_ADDR_BITS == 16
#define  MEM_DATA_TYPE   u16_t
#define  MEM_DATA_BYTES  2
#endif

#if  MEM_ADDR_BITS == 32
#define  MEM_DATA_TYPE   u32_t
#define  MEM_DATA_BYTES  4
#endif


typedef struct  _mcb   //声明内存控制块
{
   struct _mcb*   mem_next;    //指向下一个内存控制块
   struct _mcb*   mem_prv;     //指向上一个内存控制块

   MEM_DATA_TYPE  mem_blk_ofs;  //内存管理块所管理的内存块的偏移地址
   MEM_DATA_TYPE  mem_blk_num;  //内存管理块所管理的内存块的块数

}mcb;


/**========================================== MEM API ================================================**/

void  MemAllocInit(void);                      /*< 内存分配函数初始化 */
void  *MemAlloc(MEM_ALC_BYT_TYP byte_size);    /*< 内存分配函数 */
void  MemFree(void *p);                        /*< 内存释放函数 */
void  MemCleanFragment(void);                  /*< 内存碎片清理函数 */
MEM_DATA_TYPE MemGetUsedBytes(void);           /*< 获取内存已使用的字节总数 */


#define malloc(size)  MemAlloc(size)
#define free(p)       MemFree(p)
#define memcpy(dest,src,count)  MemCopy(dest,src,count)

/**===============================================================================================**/


#ifdef __cplusplus
}
#endif



#endif

