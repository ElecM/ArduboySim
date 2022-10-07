/**======================================================================================
|
|  文件说明：该文件提供内存分配函数的API实现部分
|  作者：SillyMem  (1508555728@qq.com | SillyMem)
|  时间：2020.05.08
|  版本：v2.0
|
|======================================================================================**/

#include  "heap.h"

BYTE_ALIGN_INSTRU   u8_t mem_pool[MEM_POOL_SIZE];
//定义内存池

BYTE_ALIGN_INSTRU    mcb  mem_ctr_blk[MEM_MCB_NUM];
//定义内存控制块


//内存碎片清理函数
void  MemCleanFragment(void)
{
    u16_t i;
    mcb *mem_ctr_blk_point;  // mcb mem_ctr_blk_point
    mem_ctr_blk_point = mem_ctr_blk[0].mem_next;
    #if  MEM_DEBUG >= 1
      MemDebug("cleanning up the memory fragmentation...");
    #endif
    for(i=0;i<mem_ctr_blk[0].mem_blk_num - 1 ;i++)
    {
       if((mem_ctr_blk_point->mem_blk_ofs + mem_ctr_blk_point->mem_blk_num) == \
           mem_ctr_blk_point->mem_next->mem_blk_ofs)
       {
           mem_ctr_blk_point->mem_next->mem_blk_ofs = mem_ctr_blk_point->mem_blk_ofs;
           mem_ctr_blk_point->mem_next->mem_blk_num += mem_ctr_blk_point->mem_blk_num;

           //删除该节点，并归还给空闲链表
           mem_ctr_blk_point->mem_prv->mem_next = mem_ctr_blk_point->mem_next;
           mem_ctr_blk_point->mem_next->mem_prv = mem_ctr_blk_point->mem_prv;

           mem_ctr_blk_point->mem_next = mem_ctr_blk[2].mem_next;
           mem_ctr_blk[2].mem_next    = mem_ctr_blk_point;
           mem_ctr_blk[2].mem_blk_num ++ ;
           mem_ctr_blk[0].mem_blk_num -- ;
       }
       else
       {
           mem_ctr_blk_point = mem_ctr_blk_point->mem_next;
       }
    }
     #if  MEM_DEBUG >= 1
      MemDebug("cleanning up the memory fragment completed");
    #endif
}



//内存管理初始化
void MemAllocInit(void)
{
   u16_t i;

   #if  MEM_DEBUG >= 1
      MemDebug("memory's allocatation initialization start...");
   #endif

   //内存管理链表初始化，内存管理链表使用双向链表
   //mem_ctr_blk[0]作为管理链表表头
   mem_ctr_blk[0].mem_next = &mem_ctr_blk[1];
   mem_ctr_blk[0].mem_prv  = &mem_ctr_blk[1];
   mem_ctr_blk[1].mem_next = &mem_ctr_blk[0];
   mem_ctr_blk[1].mem_prv  = &mem_ctr_blk[0];

   mem_ctr_blk[0].mem_blk_num  = 1;    //mcb_NUM 表头下面挂载的mcb的个数初始化为1
   mem_ctr_blk[0].mem_blk_ofs  = 0;    //UsedBytes;已经使用的总字节数初始化为0

   //初始化第一块内存管理块mcb
   mem_ctr_blk[1].mem_blk_ofs = 0;
   mem_ctr_blk[1].mem_blk_num = MEM_POOL_SIZE/MEM_BLK_SIZE;

   //内存空闲链表初始化，空闲链表使用单向链表
   //mem_ctr_blk[2]作为空闲链表表头
   //空闲链表中表头中空闲块的个数初始化为MEM_mcb_NUM - 3
   mem_ctr_blk[2].mem_blk_num = MEM_MCB_NUM - 3;
   for(i=2; i<MEM_MCB_NUM - 1; i++)
   {
       mem_ctr_blk[i].mem_next = &mem_ctr_blk[i+1];
       mem_ctr_blk[i].mem_blk_num = 0;
       mem_ctr_blk[i].mem_blk_ofs = 0;
   }
   mem_ctr_blk[MEM_MCB_NUM - 1].mem_blk_num = 0;
   mem_ctr_blk[MEM_MCB_NUM - 1].mem_blk_ofs = 0;

   #if  MEM_DEBUG >= 1
      MemDebug("memory's allocatation initialization completed");
   #endif
}


//内存分配函数
void *MemAlloc(MEM_ALC_BYT_TYP byte_size)
{

    u16_t i=0;
    MEM_DATA_TYPE return_addr_ofs;
    #if  MEM_DATA_BITS == 8
    u8_t         mem_alc_blk_num;
    #endif
    #if  MEM_DATA_BITS == 16
    u16_t        mem_alc_blk_num;
    #endif
    #if  MEM_DATA_BITS == 32
    u32_t        mem_alc_blk_num;
    #endif

    mcb* mem_ctr_blk_point;
    mem_ctr_blk_point = mem_ctr_blk[0].mem_next;

    #if  MEM_DEBUG >= 1
      MemDebug("memory's allocatation start...");
    #endif
    //计算所需字节数
    byte_size = (byte_size + MEM_DATA_BYTES);

    //寻找可用的内存控制块
    for(i=0;i< mem_ctr_blk[0].mem_blk_num;i++)
    {
       if( byte_size <= (mem_ctr_blk_point->mem_blk_num*MEM_BLK_SIZE))
       {
          //计算所需的内存块个数
          mem_alc_blk_num = byte_size/MEM_BLK_SIZE+((byte_size%MEM_BLK_SIZE)+MEM_BLK_SIZE - 1)/MEM_BLK_SIZE;

          //更新内存使用的总字节数
          mem_ctr_blk[0].mem_blk_ofs += mem_alc_blk_num;

          //计算返回的偏移量
          return_addr_ofs = mem_ctr_blk_point->mem_blk_ofs*MEM_BLK_SIZE;

          //在返回地址前面注入此次使用的内存块的个数
          #if  MEM_ADDR_BITS == 16
          *(u16_t*)(&mem_pool[return_addr_ofs]) = (u16_t)((mem_alc_blk_num));
          return_addr_ofs = return_addr_ofs + 2;
          #endif
          #if  MEM_ADDR_BITS == 32
          *(u32_t*)(&mem_pool[return_addr_ofs]) = (u32_t)((mem_alc_blk_num));

          return_addr_ofs = return_addr_ofs + 4;
          #endif

          //如果所需内存块个数与当前内存控制块中的块数相等，则
          //在内存管理链表钟删除该节点并归还给空闲链表
          if(mem_alc_blk_num == mem_ctr_blk_point->mem_blk_num )
          {
              mem_ctr_blk_point->mem_prv->mem_next = mem_ctr_blk_point->mem_next;
              mem_ctr_blk_point->mem_next->mem_prv = mem_ctr_blk_point->mem_prv;

              mem_ctr_blk_point->mem_next = mem_ctr_blk[2].mem_next;
              mem_ctr_blk[2].mem_next    = mem_ctr_blk_point;

              mem_ctr_blk[2].mem_blk_num ++ ;
              mem_ctr_blk[0].mem_blk_num -- ;

          }
          else
          {
              //更新内存块的偏移量和内存块个数
              mem_ctr_blk_point->mem_blk_ofs = mem_ctr_blk_point->mem_blk_ofs + mem_alc_blk_num;
              mem_ctr_blk_point->mem_blk_num -= mem_alc_blk_num;
          }

          #if  MEM_DEBUG >= 1
            MemDebug("memory's allocatation successed");
          #endif

          return (mem_pool + return_addr_ofs);
       }
       else
         mem_ctr_blk_point = mem_ctr_blk_point->mem_next;
    }
    #if  MEM_DEBUG >= 1
      MemDebug("memory's allocatation fail");
    #endif
    return NULL;
}


//内存释放函数
void  MemFree(void *p)
{
     u16_t i;
     MEM_DATA_TYPE  mem_alc_num;
     MEM_DATA_TYPE  mem_blk_ofs;
     mcb *mem_ctr_blk_point;
     mcb *mem_idl_blk_point;


     #if  MEM_DEBUG >= 1
        MemDebug("memory's freeing start...");
     #endif

     //从指针前面取出内存块的个数和求出该块内存的实际偏移量
     #if  MEM_ADDR_BITS == 16
     mem_alc_num  =(MEM_DATA_TYPE)(*(u16_t*)((u8_t *)p-2));
     mem_blk_ofs = (MEM_DATA_TYPE)(((u16_t)((u8_t *)p - mem_pool - 2))/MEM_BLK_SIZE);
     #endif

     #if  MEM_ADDR_BITS == 32
     mem_alc_num = (MEM_DATA_TYPE)(*(u32_t*)((u8_t *)p-4));
     mem_blk_ofs = (MEM_DATA_TYPE)(((u32_t)((u8_t *)p - mem_pool - 4))/MEM_BLK_SIZE);
     #endif


     //开始指向内存管理链表的第一个控制块
     mem_ctr_blk_point = mem_ctr_blk[0].mem_next;

     //按照从小到大顺序插入新的内存控制块到内存控制链表中
     for(i=0;i<mem_ctr_blk[0].mem_blk_num; i++)
     {
        if(mem_blk_ofs < mem_ctr_blk_point->mem_blk_ofs)
        {
           //临时存放空闲链表的第二个内存块地址
           mem_idl_blk_point = mem_ctr_blk[2].mem_next->mem_next;

           //装入即将要释放的内存块的大小属性和偏移量属性到从空闲链表中取出的控制块中
           mem_ctr_blk[2].mem_next->mem_blk_ofs = mem_blk_ofs;
           mem_ctr_blk[2].mem_next->mem_blk_num = mem_alc_num;

           //更新管理链表表头属性
           mem_ctr_blk[0].mem_blk_ofs -= mem_alc_num;

           //插入管理链表中
           mem_ctr_blk[2].mem_next->mem_next = mem_ctr_blk_point;
           mem_ctr_blk[2].mem_next->mem_prv  = mem_ctr_blk_point->mem_prv;
           mem_ctr_blk_point->mem_prv->mem_next = mem_ctr_blk[2].mem_next;
           mem_ctr_blk_point->mem_prv = mem_ctr_blk[2].mem_next;

           //更新内存管理链表中挂载的内存块的个数
           mem_ctr_blk[0].mem_blk_num ++ ;

           //将空闲链表的表头指向下一个空闲内存块
           mem_ctr_blk[2].mem_next = mem_idl_blk_point;

           //更新空闲链表下挂载的控制块个数
           mem_ctr_blk[2].mem_blk_num --;

           break;
        }
        else
        {
            mem_ctr_blk_point = mem_ctr_blk_point->mem_next;
        }
     }

     #if  MEM_RT_CLE_FRAG >= 1
       MemCleanFragment();
     #endif

     #if  MEM_DEBUG >= 1
        if(i < mem_ctr_blk[0].mem_blk_num)
          MemDebug("memory's freeing successed");
        else
          MemDebug("memory's freeing fail");
     #endif

}

void *MemCopy(void *dest, const void *src, MEM_ALC_BYT_TYP count)
{
  if (dest == 0 || src == 0 )
    return 0;

  char *tmp_dest = dest;
  const char *tmp_src = src;

  while (count--)
  {
      *tmp_dest++ = *tmp_src++ ;
  }

  return dest;
}




//获取内存使用总字节数
MEM_DATA_TYPE MemGetUsedBytes(void)
{
   return mem_ctr_blk[0].mem_blk_ofs*MEM_BLK_SIZE;
}



