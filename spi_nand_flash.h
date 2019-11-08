/*-------------------------------------------------------------------------------/
/  Low level disk interface header module for Toshiba SPI Flash TC58CVG1S3HRAIG  /
 *     Created on: Sept 08, 2019												 / 			This file contains low level I/O function prototypes
 *      Author: Nevermind														 /
/-------------------------------------------------------------------------------*/



#ifndef SPI_TOSHIBANANDFLASH_IO_H			/* Guard against multiple inclusion */
#define SPI_TOSHIBANANDFLASH_IO_H


/* Includes */

#include "diskio.h"
#include "fsl_common.h"
#include "fsl_dspi_edma.h"

/*-------------------------------------------------------------------------------/
                   NAND flash Command Set										 /
/-------------------------------------------------------------------------------*/

#define Write_Enable 		0x06	// Write Enable  6h must be enabled before page program/block erase/block protection operations
#define Write_Disable       0x04	//Write Disable command
#define Reset 				0xFF	// Reset any current operation
#define Program_Load 		0x02    // transfer data to the internal buffer
#define Program_Execute 	0x10    // program data from internal buffer into the cel array
#define Get_Feature 		0x0F    // read status registers command
#define Set_Feature 		0x1F	// set feature command
#define Read_Cell_Array     0x13    // read from NAND cell array in the internal NAND buffer
#define Read_NAND_buffer    0x03    // read data from the internal NAND buffer
#define Block_Erase 		0xD8	// block erase command



/*-------------------------------------------------------------------------------/
                   NAND flash Registers											 /
/-------------------------------------------------------------------------------*/

#define StatusRegister 	    0xC0
#define FeatureTableA0 		0xA0
#define FeatureTableB0		0xB0
#define FeatureTableC0		0xC0



/*-------------------------------------------------------------------------------/
              NAND flash Size considerations									 /
/-------------------------------------------------------------------------------*/

#ifdef ECC_OFF
		// ECC_OFF
		#define PageSize			2048	// no. of bytes int a pagie 2048 B date excluding spare area
		#define SpareArea			128     // n0. of bytes in the spare area of each page
		#define MaxPageNumber		64      // no. of pages in a block
		#define Total_PageSize 		2176 	// 2048 + 128 = 2176 Bytes in a page including spare area
		#define BlockNumber			2048	// no. of blocks


	#else
		// ECC_ON
		#define PageSize       	    2048   // no. of bytes int a pagie 2048 B date excluding spare area
		#define SpareArea 		    64     // n0. of bytes in the spare area of each page
		#define MaxPageNumber		64 	   // no. of pages in a block
		#define Total_PageSize      2112   // 2048 + 64 = 4224 Bytes  in a page including spare area
		#define MaxBlockNumber		2048   // no. of blocks

#endif

/*-------------------------------------------------------------------------------/
              Variables and whatnots											 /
/-------------------------------------------------------------------------------*/

typedef uint8_t byte;

extern uint8_t masterRxData[Total_PageSize] ;    // buffers for sending and recieving data
extern uint8_t masterTxData[Total_PageSize] ;

extern uint32_t WriteBytes;						 // no. of bytes to be written/read
extern uint32_t ReadBytes;

typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W/Erase Error */
	RES_NOTRDY,		/* 3: Not Ready */
} DRESULT;



/*
 *  Function that returns 2 bytes: Manufacture ID, Series: ex: 98 CB
 */
void ReadID(SPI_Type *EXAMPLE_DSPI_MASTER_BASEADDR,dspi_master_edma_handle_t *g_dspi_edma_m_handle);


/* PageRead.

    Cell array to data buffer - 115 - 280 us


  * A function that reads from a certain block, from a certain page, a certain number of bytes in an RX buffer.
 * Transfer to SPI the commands, via DMA, from a global buffer (TX) and return the required data to another buffer (RX).
 * SPI_Type * EXAMPLE_DSPI_MASTER_BASEADDR - the address of the SPI module used (ex: SPI0).
 * dspi_master_edma_handle_t * g_dspi_edma_m_handle - transfer handle via DMA.
 * byte * RXrambuffer - buffer in which spi reads.
 * int block - the number of the block from which we read. 2048 blocks
 * int page - the number of the page from which we read. 64 pages / block
 * int length - how many bytes of page to return.
 */
DRESULT PageRead(SPI_Type *EXAMPLE_DSPI_MASTER_BASEADDR,dspi_master_edma_handle_t *g_dspi_edma_m_handle,byte* RXrambuffer,uint32_t block,uint32_t page,uint32_t offset, uint32_t length );


/* PageProgram.

Programming time - 450 - 600 us / page

  * Function that writes in a certain block, on a certain page, a certain number of bytes in a TX buffer.
 * Transfer to SPI the commands, via DMA, from a global buffer (TX) and return the required data to another buffer (RX).
 * SPI_Type * EXAMPLE_DSPI_MASTER_BASEADDR - the address of the SPI module used (ex: SPI0).
 * dspi_master_edma_handle_t * g_dspi_edma_m_handle - transfer handle via DMA.
 * byte * RXrambuffer - buffer in which spi reads.
 * int block - the number of the block in which we write. 2048 blocks
 * int page - the number of the page we write. 64 pages / block
 * int length - how many bytes we write on the page.
 *
 */
DRESULT PageProgram(SPI_Type *EXAMPLE_DSPI_MASTER_BASEADDR,dspi_master_edma_handle_t *g_dspi_edma_m_handle,byte* TXrambuffer,byte* RXrambuffer,uint32_t block,uint32_t page,uint32_t offset,uint32_t length);


/* Resets current command
 *
 */
void Reset_Command(SPI_Type *EXAMPLE_DSPI_MASTER_BASEADDR,dspi_master_edma_handle_t *g_dspi_edma_m_handle);


/*
 	 Block erase time - 2-7 ms/block
 */
DRESULT BlockErase(SPI_Type *EXAMPLE_DSPI_MASTER_BASEADDR,dspi_master_edma_handle_t *g_dspi_edma_m_handle ,byte* RXrambuffer,uint32_t block);

/*
 *  Function that returns nothing and sets a feature.
 */
void SetFeature(SPI_Type *EXAMPLE_DSPI_MASTER_BASEADDR,dspi_master_edma_handle_t *g_dspi_edma_m_handle,byte* RXrambuffer,uint32_t reg,uint32_t data);

/*
 *  Function that returns 1 byte corresponding to a feature
 */
byte GetFeature(SPI_Type *EXAMPLE_DSPI_MASTER_BASEADDR,dspi_master_edma_handle_t *g_dspi_edma_m_handle, byte reg);




/*
 * 		Enjoy and have a great day guyz :)
 */


#endif /* SPI_TOSHIBANANDFLASH_IO_H_ */
